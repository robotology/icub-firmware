

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_net_eth_Socket.h"

#include <vector>
#include <algorithm>

#include "embot_os_rtos.h"
#include "embot_net_eth_theIPservice.h"

using namespace embot::net::eth;

// - helpers -----------------------------------------------------------------------------------------------------------

namespace {

//    #warning: metti in embot::os
//    // RAII lock on an embot::os::rtos mutex. Mutexes have priority inheritance.
//    class Lock
//    {
//    public:
//        explicit Lock(embot::os::rtos::mutex_t *m) : m_(m)
//        {
//            embot::os::rtos::mutex_take(m_, embot::core::reltimeWaitForever);
//        }
//        ~Lock()
//        {
//            embot::os::rtos::mutex_release(m_);
//        }
//        Lock(const Lock&) = delete;
//        Lock& operator=(const Lock&) = delete;
//    private:
//        embot::os::rtos::mutex_t *m_;
//    };

//    // single place where a Callback is executed. TODO: verify the name of the method of embot::core::Callback
//    inline void run(const embot::core::Callback &cb)
//    {
//        cb.execute();
//    }

} // namespace


// - Pipe: a ring of packets with preallocated memory ------------------------------------------------------------------

// The slots are allocated only once, in the constructor. push() and pop() copy, never allocate.
// All the methods are thread-safe: they use an internal mutex.
namespace utils {

class Ring
{
public:
    explicit Ring(const Socket::Pipe &p)
        : capacity_(std::max<uint8_t>(1, p.maxpackets))
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

    // copies p into the tail slot. false if the ring is full or if p does not fit in a slot
    bool push(const Packet &p)
    {
        embot::os::rtos::Lock lock(mtx_);
        if(count_ >= capacity_) { return false; }
        Packet &s = slots_[(head_ + count_) % capacity_];
        if(false == s.load(p.size(), p.data())) { return false; }
        s.setaddress(p.address());
        count_++;
        return true;
    }

    // moves the head slot into out and removes it. false if the ring is empty.
    // if out is too small the packet is removed and dropped and the function returns false:
    // in such a way a packet which cannot be read does not block the ring.
    bool pop(Packet &out, bool &dropped)
    {
        embot::os::rtos::Lock lock(mtx_);
        dropped = false;
        if(0 == count_) { return false; }
        Packet &s = slots_[head_];
        bool ok = out.load(s.size(), s.data());
        if(true == ok) { out.setaddress(s.address()); } else { dropped = true; }
        head_ = (head_ + 1) % capacity_;
        count_--;
        return ok;
    }

    size_t size() const
    {
        embot::os::rtos::Lock lock(mtx_);
        return count_;
    }

    size_t capacity() const { return capacity_; }

private:
    std::vector<Packet> slots_ {};
    size_t capacity_ {1};
    size_t head_ {0};
    size_t count_ {0};
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

    // it counts the packets in the input pipe: released by addRXfifo() only after a successful push,
    // acquired by receive(). maxtokens = capacity of the pipe + 1 as in the old EOsocket.
    embot::os::rtos::semaphore_t *rxtokens {nullptr};

    // it protects props.action: setaction() (owner thread) vs addRXfifo() and remTXfifo() (IP service thread)
    embot::os::rtos::mutex_t *mtxaction {nullptr};

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
};


// - Socket ------------------------------------------------------------------------------------------------------------

Socket::Socket(const Config &config)
    : pImpl(std::make_unique<Impl>(config))
{
}

Socket::~Socket()
{
    // a socket which is destroyed while attached must be detached first
    if((nullptr != pImpl) && (true == pImpl->opened))
    {
        close();
    }
}

Socket::Socket(Socket&&) noexcept = default;
Socket& Socket::operator=(Socket&&) noexcept = default;


bool Socket::open(const Properties &props)
{
    if(true == pImpl->opened)
    {
        return false;
    }

    {
        embot::os::rtos::Lock lock(pImpl->mtxaction);
        pImpl->props = props;
    }

    // blocking: when it returns the IP service has bound the port and the socket can receive
    if(false == theIPservice::getInstance().attach(*this, embot::core::reltimeWaitForever))
    {
        return false;
    }

    pImpl->opened = true;
    return true;
}


bool Socket::close()
{
    if(false == pImpl->opened)
    {
        return false;
    }

    // blocking: the IP service first sends what is still inside the output pipe and then detaches the socket.
    // what is already inside the input pipe stays there and can be retrieved by receive().
    if(false == theIPservice::getInstance().detach(*this, embot::core::reltimeWaitForever))
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
        return false;
    }

    // copy into the output pipe. it fails if the pipe is full or the packet is too big
    if(false == pImpl->output.push(packet))
    {
        return false;
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

    bool dropped {false};
    return pImpl->input.pop(packet, dropped);
}


const Socket::Config & Socket::config() const
{
    return pImpl->config;
}


const Socket::Properties & Socket::properties() const
{
    return pImpl->props;
}


void Socket::setaction(const Action &action)
{
    // the IP service reads the actions with the same mutex, so it never sees a half-written Callback
    embot::os::rtos::Lock lock(pImpl->mtxaction);
    pImpl->props.action = action;
}


// - private: used only by theIPservice --------------------------------------------------------------------------------

bool Socket::addRXfifo(const Packet &packet)
{
    if(false == pImpl->input.push(packet))
    {
        // pipe full or packet too big: the packet is dropped. the IP service can count it.
        return false;
    }

    // one token for each packet in the pipe
    embot::os::rtos::semaphore_release(pImpl->rxtokens);

    // the action is executed in the context of the IP service thread
    pImpl->getaction().onrx.execute();

    return true;
}


bool Socket::remTXfifo(Packet &packet)
{
    bool dropped {false};
    if(false == pImpl->output.pop(packet, dropped))
    {
        return false;
    }

    pImpl->getaction().ontx.execute();

    return true;
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

