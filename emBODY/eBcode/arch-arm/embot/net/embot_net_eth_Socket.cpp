

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_net_eth_Socket.h"

#include <vector>
#include <algorithm>
#include <atomic>

#include "embot_os_rtos.h"
#include "embot_net_eth_theIPservice.h"

using namespace embot::net::eth;

// - helpers -----------------------------------------------------------------------------------------------------------

// - Pipe: a ring of packets with preallocated memory ------------------------------------------------------------------

// The slots are allocated only once, in the constructor. push() and pop() copy, never allocate.
// All the methods are thread-safe: they use an internal mutex.
namespace utils {

class Ring
{
public:
    enum class Push { ok, replaced, full, toobig };     // replaced: ok, but the oldest packet was lost (Policy::dropoldest)
    enum class Pop { ok, empty, dropped };

    explicit Ring(const Socket::Pipe &p)
        : capacity_(std::min<size_t>(std::max<size_t>(1, p.maxpackets), Socket::Pipe::maxpacketslimit)), slotsize_(p.maxsizeofpacket), policy_(p.policy)
    {
        mtx_ = embot::os::rtos::mutex_new();
        slots_.reserve(capacity_);
        for(size_t i=0; i<capacity_; i++)
        {
            slots_.emplace_back(Packet(p.maxsizeofpacket));
        }
    }

    ~Ring()
    {
        embot::os::rtos::mutex_delete(mtx_);
    }

    Ring(const Ring&) = delete;
    Ring& operator=(const Ring&) = delete;

    // copies p into the tail slot. 
    // full: the ring is full and the policy is dropnewest. toobig: p does not fit in a slot (nothing is dropped).
    // with dropoldest a full ring never refuses: the oldest packet is overwritten and the count does not change.
    Push push(const Packet &p)
    {
        embot::os::rtos::Lock lock(mtx_);
        if(p.size() > slotsize_) { return Push::toobig; }
        Push r {Push::ok};
        if(count_.load() >= capacity_)
        {
            if(Socket::Policy::dropnewest == policy_) { return Push::full; }
            head_ = (head_ + 1) % capacity_;
            count_.fetch_sub(1);
            r = Push::replaced;
        }
        slots_[(head_ + count_.load()) % capacity_].load(p);
        count_.fetch_add(1);
        return r;
    }

    // moves the head slot into out and removes it. 
    // if out is too small the packet is removed and dropped (Pop::dropped): 
    // in such a way a packet which cannot be read does not block the ring.
    Pop pop(Packet &out)
    {
        // a quick check which does not take the mutex: an empty ring is the most common case when the IP service scans the sockets. 
        // a packet pushed just now and not seen here is not lost: whoever pushes alerts the IP service, which scans again
        if(0 == count_.load()) { return Pop::empty; }
        embot::os::rtos::Lock lock(mtx_);
        if(0 == count_.load()) { return Pop::empty; }
        Packet &s = slots_[head_];
        bool ok = out.load(s);
        head_ = (head_ + 1) % capacity_;
        count_.fetch_sub(1);
        return ok ? Pop::ok : Pop::dropped;
    }

    size_t size() const
    {
        return count_.load();
    }

    size_t capacity() const { return capacity_; }

private:
    std::vector<Packet> slots_ {};
    size_t capacity_ {1};
    size_t slotsize_ {0};
    Socket::Policy policy_ {Socket::Policy::dropnewest};
    size_t head_ {0};
    std::atomic<size_t> count_ {0};         // written only with mtx_ taken, but readable without it
    embot::os::rtos::mutex_t *mtx_ {nullptr};
};

} // namespace utils


// - Socket::Impl ------------------------------------------------------------------------------------------------------

struct Socket::Impl
{
    Config config {};
    Properties props {};
    bool opened {false};            // written only by the owner thread

    utils::Ring input;              // filled by theIPservice (addRXfifo), emptied by the owner (receive)
    utils::Ring output;             // filled by the owner (transmit), emptied by theIPservice (remTXfifo)

    // it counts the packets in the input pipe: released by addRXfifo() only after a successful push which does not
    // replace a packet, acquired by receive(). maxtokens = capacity of the pipe + 1 as in the old EOsocket.
    embot::os::rtos::semaphore_t *rxtokens {nullptr};

    // it protects props.action: setaction() (owner thread) vs addRXfifo() and remTXfifo() (IP service thread)
    embot::os::rtos::mutex_t *mtxaction {nullptr};

    // the counters are written by both the owner and the IP service thread. 32 bit atomics are lock free.
    std::atomic<uint32_t> rxok {0};
    std::atomic<uint32_t> txok {0};
    std::atomic<uint32_t> errors[numberoferrors] {};        // indexed by Error. [0] (none) is not used
    std::atomic<Error> lasterror {Error::none};

    explicit Impl(const Config &c)
        : config(c), input(c.inputpipe), output(c.outputpipe)
    {
        rxtokens = embot::os::rtos::semaphore_new(input.capacity() + 1, 0);
        mtxaction = embot::os::rtos::mutex_new();
    }

    ~Impl()
    {
        embot::os::rtos::semaphore_delete(rxtokens);
        embot::os::rtos::mutex_delete(mtxaction);
    }

    Action getaction() const
    {
        embot::os::rtos::Lock lock(mtxaction);
        return props.action;
    }

    // it only counts. used for the errors detected inside a call of the owner thread, which returns false anyway
    void count(Error e)
    {
        errors[embot::core::tointegral(e)].fetch_add(1);
        lasterror.store(e);
    }

    // it counts and tells the owner: used by the IP service thread
    void notify(Error e)
    {
        count(e);
        getaction().onerr.execute();
    }
};


// - Socket ------------------------------------------------------------------------------------------------------------

Socket::Socket(const Config &config)
    : pImpl(std::make_unique<Impl>(config))
{
}

Socket::~Socket()
{
    // a socket which is destroyed while attached must be detached first. if close() fails (e.g. the destructor runs in a thread 
    // which is not allowed to call it) theIPservice keeps a dangling pointer: the owner MUST close() the socket before.
    if((nullptr != pImpl) && (true == pImpl->opened))
    {
        close(embot::core::reltimeWaitForever);
    }
}


bool Socket::open(const Properties &props, embot::core::relTime tout)
{
    if((true == pImpl->opened) || (false == pImpl->config.isvalid()))
    {
        return false;
    }

    {
        embot::os::rtos::Lock lock(pImpl->mtxaction);
        pImpl->props = props;
    }

    // blocking: when it returns true the IP service has bound the port and the socket can receive.
    // when it returns false because of a timeout the request has been cancelled: the socket is not attached.
    if(false == theIPservice::getInstance().attach(*this, tout))
    {
        return false;
    }

    pImpl->opened = true;
    return true;
}


bool Socket::close(embot::core::relTime tout)
{
    if(false == pImpl->opened)
    {
        return false;
    }

    // blocking: the IP service first sends what is still inside the output pipe and then detaches the socket.
    // what is already inside the input pipe stays there and can be retrieved by receive().
    // on a timeout the request has been cancelled: the socket is still attached.
    if(false == theIPservice::getInstance().detach(*this, tout))
    {
        return false;
    }

    pImpl->opened = false;
    return true;
}


bool Socket::connect(const IPaddress &remoteaddress, embot::core::relTime tout)
{
    return theIPservice::getInstance().resolve(remoteaddress, tout);
}


bool Socket::transmit(const Packet &packet)
{
    if(false == pImpl->opened)
    {
        pImpl->count(Error::txnotopen);
        return false;
    }

    // without a destination the IP stack would refuse the packet later, in the IP service thread: we tell it now
    if(false == packet.address().isvalid())
    {
        pImpl->count(Error::txnoaddress);
        return false;
    }

    // copy into the output pipe. it fails if the pipe is full (dropnewest) or the packet is too big.
    // with dropoldest the oldest packet is lost: the call succeeds but the loss is counted.
    switch(pImpl->output.push(packet))
    {
        case utils::Ring::Push::ok:                                             break;
        case utils::Ring::Push::replaced:   pImpl->count(Error::txpipefull);    break;
        case utils::Ring::Push::full:       pImpl->count(Error::txpipefull);    return false;
        case utils::Ring::Push::toobig:     pImpl->count(Error::txtoobig);      return false;
    }

    // and tell the IP service that there is something to transmit.
    // as the IP service has a priority higher than the one of the owner, it can send the packet
    // before this function returns.
    return theIPservice::getInstance().alert(theIPservice::Event::TX);
}


size_t Socket::input() const
{
    return pImpl->input.size();
}


bool Socket::receive(Packet &packet, embot::core::relTime tout)
{
    // wait for a token, which is released after every packet put inside the input pipe.
    // with tout = 0 we just try.
    if(false == embot::os::rtos::semaphore_acquire(pImpl->rxtokens, tout))
    {
        return false;
    }

    return (utils::Ring::Pop::ok == pImpl->input.pop(packet));
}


const Socket::Config & Socket::config() const
{
    return pImpl->config;
}


Socket::Properties Socket::properties() const
{
    // the fields but the action never change after open(). the action is read with the mutex
    Properties p = pImpl->props;
    p.action = pImpl->getaction();
    return p;
}


void Socket::setaction(const Action &action)
{
    // the IP service reads the actions with the same mutex, so it never sees a half-written Callback
    embot::os::rtos::Lock lock(pImpl->mtxaction);
    pImpl->props.action = action;
}


Socket::Stats Socket::stats() const
{
    Stats s {};
    s.rxok = pImpl->rxok.load();
    s.txok = pImpl->txok.load();
    s.rxpipefull = pImpl->errors[embot::core::tointegral(Error::rxpipefull)].load();
    s.rxtoobig = pImpl->errors[embot::core::tointegral(Error::rxtoobig)].load();
    s.txnotopen = pImpl->errors[embot::core::tointegral(Error::txnotopen)].load();
    s.txpipefull = pImpl->errors[embot::core::tointegral(Error::txpipefull)].load();
    s.txtoobig = pImpl->errors[embot::core::tointegral(Error::txtoobig)].load();
    s.txnomemory = pImpl->errors[embot::core::tointegral(Error::txnomemory)].load();
    s.txsendfailed = pImpl->errors[embot::core::tointegral(Error::txsendfailed)].load();
    s.txnoaddress = pImpl->errors[embot::core::tointegral(Error::txnoaddress)].load();
    s.lasterror = pImpl->lasterror.load();
    return s;
}


Socket::Stats Socket::take()
{
    // every counter is read and zeroed in one atomic step, so no event is lost between the read and the reset. 
    // the snapshot as a whole is not atomic: two counters may be read at slightly different instants.
    Stats s {};
    s.rxok = pImpl->rxok.exchange(0);
    s.txok = pImpl->txok.exchange(0);
    s.rxpipefull = pImpl->errors[embot::core::tointegral(Error::rxpipefull)].exchange(0);
    s.rxtoobig = pImpl->errors[embot::core::tointegral(Error::rxtoobig)].exchange(0);
    s.txnotopen = pImpl->errors[embot::core::tointegral(Error::txnotopen)].exchange(0);
    s.txpipefull = pImpl->errors[embot::core::tointegral(Error::txpipefull)].exchange(0);
    s.txtoobig = pImpl->errors[embot::core::tointegral(Error::txtoobig)].exchange(0);
    s.txnomemory = pImpl->errors[embot::core::tointegral(Error::txnomemory)].exchange(0);
    s.txsendfailed = pImpl->errors[embot::core::tointegral(Error::txsendfailed)].exchange(0);
    s.txnoaddress = pImpl->errors[embot::core::tointegral(Error::txnoaddress)].exchange(0);
    s.lasterror = pImpl->lasterror.exchange(Error::none);
    return s;
}


// - private: used only by theIPservice --------------------------------------------------------------------------------

bool Socket::addRXfifo(const Packet &packet)
{
    switch(pImpl->input.push(packet))
    {
        case utils::Ring::Push::full:
        {
            // dropnewest: the new packet is lost
            pImpl->notify(Error::rxpipefull);
            return false;
        }
        case utils::Ring::Push::toobig:
        {
            pImpl->notify(Error::rxtoobig);
            return false;
        }
        case utils::Ring::Push::replaced:
        {
            // dropoldest: the number of packets in the pipe did not change, so no new token: 
            // there is one token for each packet in the pipe
            pImpl->rxok.fetch_add(1);
            pImpl->notify(Error::rxpipefull);
        } break;
        case utils::Ring::Push::ok:
        {
            pImpl->rxok.fetch_add(1);
            embot::os::rtos::semaphore_release(pImpl->rxtokens);
        } break;
    }

    // the action is executed in the context of the IP service thread
    pImpl->getaction().onrx.execute();

    return true;
}


bool Socket::remTXfifo(Packet &packet)
{
    // a packet which cannot be moved into @packet is dropped and the next one is tried: the pipe is never blocked
    while(true)
    {
        switch(pImpl->output.pop(packet))
        {
            case utils::Ring::Pop::empty:   return false;
            case utils::Ring::Pop::dropped: pImpl->notify(Error::txtoobig); break;
            case utils::Ring::Pop::ok:     return true;
        }
    }
}


void Socket::txsent()
{
    pImpl->txok.fetch_add(1);
    // the packet has been accepted by the IP stack: the owner can be told. executed in the IP service thread
    pImpl->getaction().ontx.execute();
}


void Socket::fail(Error e)
{
    pImpl->notify(e);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

