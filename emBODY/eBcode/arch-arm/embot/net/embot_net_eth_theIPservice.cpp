

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_net_eth_theIPservice.h"

#include <vector>
#include <atomic>

#include "embot_os_Thread.h"
#include "embot_os_theScheduler.h"
#include "embot_os_rtos.h"
#include "embot_net_lwip.h"

// lwip types used by the receive callback. they live in the global namespace
struct udp_pcb;
struct pbuf;


namespace embot::net::eth {


// --------------------------------------------------------------------------------------------------------------------
// - pimpl
// --------------------------------------------------------------------------------------------------------------------

// Everything which touches the IP stack, the table of attached sockets, the scratch packets and the pending
// command state is used ONLY by the IP service thread: no lock is needed for them.
// The only objects shared with the callers are the command slot (protected by mtxcaller + done semaphore + the 
// atomic cstate, which makes a command cancellable when the caller gives up on a timeout).

struct theIPservice::Impl
{
    // keep it like that so that the name tIPservice is shown in eventviewer
    static void tIPservice(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

    
    // while resolving, the ARP request is repeated with this period until the timeout expires
    static constexpr embot::core::relTime arpretryperiod = 100 * embot::core::time1millisec;
    
    static constexpr embot::os::EventMask evRX  = embot::os::bitpos2event(embot::core::tointegral(theIPservice::Event::RX));
    static constexpr embot::os::EventMask evTX  = embot::os::bitpos2event(embot::core::tointegral(theIPservice::Event::TX));
    static constexpr embot::os::EventMask evCMD = embot::os::bitpos2event(embot::core::tointegral(theIPservice::Event::CMD));
    
    Config config {};
    embot::os::EventThread *thread {nullptr};

    // - the command slot. one command at a time ------------------------------------------------------------------

    struct Cmd
    {
        enum class Op { none, attach, detach, resolve };
        Op op {Op::none};
        Socket *socket {nullptr};
        IPaddress address {};
        embot::core::relTime tout {0};      // for resolve: the time allowed to get the ARP reply
        bool result {false};
    };

    // life of a command, shared by the caller and the service thread. every transition is a compare-and-swap, 
    // so a command is either executed (posted -> running -> done) or cancelled (posted -> cancelled), never both:
    //   idle      -> posted     caller: the slot is filled and the service thread is alerted
    //   posted    -> running    service: it starts the command
    //   posted    -> cancelled  caller: timeout before the service has started the command. it will never be executed
    //   running   -> done       service: finish() has set the result and released the semaphore
    //   done/cancelled -> idle  caller: it has taken the result. only now another caller can use the slot
    enum class CState : uint8_t { idle, posted, running, done, cancelled };
    
    Cmd cmd {};
    std::atomic<CState> cstate {CState::idle};
    embot::os::rtos::mutex_t *mtxcaller {nullptr};      // serialises the callers. it has priority inheritance
    embot::os::rtos::semaphore_t *done {nullptr};       // released once, by finish(), when a command is completed

    // while a resolve() is running, the caller waits up to tout + this slack, so that a resolve with tout = 0 
    // (one attempt) can be completed by the service thread also when the caller has the highest priority (init thread)
    embot::core::relTime resolveslack() const { return 2 * config.tickperiod; }

    // state of a resolve() which is waiting for the ARP reply
    bool resolving {false};
    embot::core::Time arpdeadline {0};
    embot::core::Time arplast {0};

    // - attached sockets ----------------------------------------------------------------------------------------

    // the table is kept ordered: the sockets with Socket::TXpriority::high come first, then the normal ones, 
    // so that the transmission can scan it in order.
    struct Entry
    {
        Socket *socket {nullptr};
        embot::net::lwip::udp::OBJ *pcb {nullptr};
        bool high {false};
    };
    std::vector<Entry> table {};
    size_t numhigh {0};         // the first numhigh entries of table are high priority
    size_t rrnext {0};          // round robin among the normal sockets: index (inside the normal ones) where the scan starts

    // scratch packets of the IP service thread
    Packet rxpacket {};
    Packet txpacket {};

    embot::core::Time lasttick {0};


    Impl()
    {
        mtxcaller = embot::os::rtos::mutex_new();
        done = embot::os::rtos::semaphore_new(1, 0);
    }

    ~Impl()
    {
        embot::os::rtos::semaphore_delete(done);
        embot::os::rtos::mutex_delete(mtxcaller);
    }


    // - caller side: any thread but the IP service thread --------------------------------------------------------

    // a command can be issued only by a thread which is not the service thread and whose priority is lower than
    // the one of the service thread. the latter is the assumption which makes the single command slot safe 
    // (the service thread preempts the caller and priority inheritance of mtxcaller works).
    // the only exception is the init thread, for the commands which allow it (see initallowed).
    
    bool callerisok(bool initallowed) const
    {
        embot::os::Thread *caller = embot::os::theScheduler::getInstance().scheduled();
        
        // nullptr: scheduler not started or called from an ISR. 
        if((nullptr == caller) || (caller == thread))
        {
            return false;
        }
        
        // the init thread has the highest priority, but if it blocks waiting for the command the service thread runs. 
        // this is allowed only for commands which do not need an owner thread (resolve): a socket attached by the 
        // init thread would be left without an owner as soon as the init thread terminates.
        if((true == initallowed) && (embot::os::Thread::Type::Init == caller->getType()))
        {
            return true;
        }
        
        // in embot::os::Priority a higher value means a higher priority
        return (caller->getPriority() < thread->getPriority());
    }
    
    // blocking, but it gives up after @tout (reltimeWaitForever: never). tout bounds the whole call: the wait for the 
    // slot (another caller may be using it) plus the wait for the completion of the command.
    // if it gives up before the service thread has started the command, the command is cancelled and will never run.
    // if the service thread has already started it, the caller waits for its end, which is soon (attach and detach are
    // instantaneous, resolve ends at its own timeout).
    bool execute(Cmd::Op op, Socket *s, const IPaddress &a, embot::core::relTime tout, bool initallowed = false)
    {
        // not started, or called by a thread which cannot use it (the service thread itself would wait for itself) 
        if((nullptr == thread) || (false == callerisok(initallowed)))
        {
            return false;
        }

        const bool forever = (embot::core::reltimeWaitForever == tout);
        const embot::core::Time t0 = embot::core::now();

        if(false == embot::os::rtos::mutex_take(mtxcaller, tout))
        {
            return false;
        }

        // what is left of the timeout. a resolve gets a small extra to let the service thread do its attempts (see resolveslack())
        embot::core::relTime wait = tout;
        if(false == forever)
        {
            embot::core::relTime elapsed = static_cast<embot::core::relTime>(embot::core::now() - t0);
            wait = (elapsed >= tout) ? 0 : (tout - elapsed);
            if(Cmd::Op::resolve == op)
            {
                wait += resolveslack();
            }
        }

        cmd.op = op;
        cmd.socket = s;
        cmd.address = a;
        cmd.tout = tout;
        cmd.result = false;
        cstate.store(CState::posted);

        thread->setEvent(evCMD);

        bool result {false};
        if(true == embot::os::rtos::semaphore_acquire(done, wait))
        {
            result = cmd.result;
        }
        else
        {
            CState expected = CState::posted;
            if(true == cstate.compare_exchange_strong(expected, CState::cancelled))
            {
                // the service thread has not started it and now it never will: the slot is free again
                cstate.store(CState::idle);
                embot::os::rtos::mutex_release(mtxcaller);
                return false;
            }
            // the service thread has started the command (running, or even done in the meantime): it ends soon
            embot::os::rtos::semaphore_acquire(done, embot::core::reltimeWaitForever);
            result = cmd.result;
        }

        cstate.store(CState::idle);
        embot::os::rtos::mutex_release(mtxcaller);
        return result;
    }


    // - service thread side --------------------------------------------------------------------------------------

    // the only place where a command is completed. it is called only when the command is running,
    // so it releases the semaphore exactly once per command (the old code could release it twice for ARP)
    void finish(bool result)
    {
        if(CState::running != cstate.load())
        {
            return;
        }
        cmd.result = result;
        resolving = false;
        cstate.store(CState::done);
        embot::os::rtos::semaphore_release(done);
    }

    Entry * find(const Socket *s)
    {
        for(auto &e : table)
        {
            if(e.socket == s) { return &e; }
        }
        return nullptr;
    }

    // the stack calls it (inside lwip::sys::process(), so in the service thread) for every datagram received by a socket.
    static void onRXdatagram(void *arg, struct udp_pcb *pcb, struct pbuf *rxpkt, const IPaddress *ipaddr, const Port port)
    {
        theIPservice::getInstance().pImpl->rxdatagram(arg, rxpkt, ipaddr, port);
    }

    // the stack calls it when a frame has arrived: it may be a different thread or an ISR, so it only wakes up the service thread.
    static void onRXframe(void *)
    {
        theIPservice::getInstance().alert(Event::RX);
    }

    void rxdatagram(void *arg, struct pbuf *rxpkt, const IPaddress *ipaddr, const Port port)
    {
        if(nullptr == rxpkt)
        {
            return;
        }

        auto *pk = reinterpret_cast<embot::net::lwip::pkt::OBJ*>(rxpkt);
        Socket *s = reinterpret_cast<Socket*>(arg);

        size_t siz = embot::net::lwip::pkt::size(pk);
        // lwip frames may be non contiguous, so we copy into the scratch packet. we must be sure it fits.
        if((nullptr != s) && (nullptr != ipaddr) && (siz <= rxpacket.capacity()))
        {
            embot::net::lwip::pkt::copyto(pk, rxpacket.data());
            rxpacket.setsize(siz);
            rxpacket.setaddress({*ipaddr, port});
            embot::net::lwip::pkt::release(pk);
            // the packet is dropped if the input pipe of the socket is full
            s->addRXfifo(rxpacket);
            return;
        }

        // lwip needs the pbuf to be released in any case
        embot::net::lwip::pkt::release(pk);
    }

    bool doattach(Socket &s)
    {
        if((table.size() >= config.numberofattachablesockets) || (nullptr != find(&s)))
        {
            return false;
        }

        // the scratch packets must be able to hold any packet of the pipes of the socket: 
        // otherwise Ring::pop() would drop it and the draining of the pipe would stop
        if((s.config().outputpipe.maxsizeofpacket > txpacket.capacity()) || (s.config().inputpipe.maxsizeofpacket > rxpacket.capacity()))
        {
            return false;
        }

        auto *pcb = reinterpret_cast<embot::net::lwip::udp::OBJ*>(embot::net::lwip::udp::retrieve());
        if(nullptr == pcb)
        {
            return false;
        }

        const Socket::Properties &p = s.properties();

        // if anything fails after retrieve(), the pcb is released: no leak
        if((false == embot::net::lwip::udp::bind(pcb, {p.bindingaddress, p.localport})) ||
           (false == embot::net::lwip::udp::recv(pcb, {onRXdatagram, &s})))
        {
            embot::net::lwip::udp::release(pcb);
            return false;
        }

        // high priority sockets first. the vector was reserved in initialise(), so insert() does not allocate
        Entry ne {&s, pcb, (Socket::TXpriority::high == p.txpriority)};
        if(true == ne.high)
        {
            table.insert(table.begin() + numhigh, ne);
            numhigh++;
        }
        else
        {
            table.push_back(ne);
        }
        return true;
    }

    bool dodetach(Socket &s)
    {
        Entry *e = find(&s);
        if(nullptr == e)
        {
            return false;
        }

        // first we send what the owner has already put inside the output pipe
        while(true == transmitone(*e)) {}

        // releasing the pcb stops the reception: after it the stack does not use the socket anymore
        embot::net::lwip::udp::release(e->pcb);
        if(true == e->high)
        {
            numhigh--;
        }
        table.erase(table.begin() + (e - table.data()));
        rrnext = 0;
        return true;
    }

    // it sends one packet of the output pipe of a socket. false if the pipe is empty.
    // a packet which the stack cannot send is lost: it is counted and the owner is told (Socket::Error)
    bool transmitone(Entry &e)
    {
        if(false == e.socket->remTXfifo(txpacket))
        {
            return false;
        }

        auto *pk = embot::net::lwip::pkt::retrieve(txpacket.size());
        if(nullptr == pk)
        {
            e.socket->fail(Socket::Error::txnomemory);
            return true;
        }
        embot::net::lwip::pkt::load(pk, txpacket.data(), txpacket.size());
        bool ok = embot::net::lwip::udp::send(e.pcb, pk, {txpacket.address().addr, txpacket.address().port});
        // the pbuf is always released here by us, also if the send fails (as the old code did after a successful send)
        embot::net::lwip::pkt::release(pk);

        if(true == ok)
        {
            e.socket->txsent();
        }
        else
        {
            e.socket->fail(Socket::Error::txsendfailed);
        }
        return true;
    }

    // order of transmission: all the packets of the high priority sockets, then ONE packet of a normal socket
    // (round robin among them) and again from the high ones. a high priority packet put in a pipe while 
    // we send a normal one waits for at most one packet. the normal sockets cannot starve each other.
    void transmitall()
    {
        const size_t numnormal = table.size() - numhigh;

        while(true)
        {
            for(size_t i=0; i<numhigh; i++)
            {
                while(true == transmitone(table[i])) {}
            }

            bool sent {false};
            for(size_t k=0; k<numnormal; k++)
            {
                size_t idx = (rrnext + k) % numnormal;
                if(true == transmitone(table[numhigh + idx]))
                {
                    rrnext = (idx + 1) % numnormal;
                    sent = true;
                    break;
                }
            }

            if(false == sent)
            {
                break;
            }
        }
    }

    void startcommand(embot::core::Time now)
    {
        // the caller may have cancelled the command in the meantime: in such a case the swap fails and we do nothing
        CState expected = CState::posted;
        if(false == cstate.compare_exchange_strong(expected, CState::running))
        {
            return;
        }

        switch(cmd.op)
        {
            case Cmd::Op::attach:  { finish(doattach(*cmd.socket)); } break;
            case Cmd::Op::detach:  { finish(dodetach(*cmd.socket)); } break;
            case Cmd::Op::resolve:
            {
                if(true == embot::net::lwip::arp::resolve(cmd.address, true))
                {
                    finish(true);
                }
                else if(0 == cmd.tout)
                {
                    finish(false);
                }
                else
                {
                    resolving = true;
                    arplast = now;
                    arpdeadline = now + cmd.tout;
                }
            } break;
            default: break;
        }
    }

    void checkresolve(embot::core::Time now)
    {
        if(false == resolving)
        {
            return;
        }

        if(true == embot::net::lwip::arp::isresolved(cmd.address))
        {
            finish(true);
        }
        else if(now >= arpdeadline)
        {
            finish(false);
        }
        else if((now - arplast) >= arpretryperiod)
        {
            embot::net::lwip::arp::resolve(cmd.address, true);
            arplast = now;
        }
    }

    // executed once, in the service thread, before any event
    void startup()
    {
        // the scratch packets
        rxpacket = Packet(config.maxsizeofpacket);
        txpacket = Packet(config.maxsizeofpacket);

        // the stack is initialised by the thread which uses it, before it can process any event.
        embot::net::lwip::sys::init(config.ipconfig, {onRXframe, nullptr});

        lasttick = embot::core::now();
    }

    // executed at every event and at every timeout (mask = 0). it does only what the events ask for.
    void onevent(embot::os::EventMask eventmask)
    {
        embot::core::Time now = embot::core::now();

        // the stack tick: the thread wakes up at least every tickperiod.
        while((now - lasttick) >= config.tickperiod)
        {
            embot::net::lwip::sys::tick();
            lasttick += config.tickperiod;
        }

        // a command is looked for at every wake up, not only when evCMD is set: its state says if there is one (a load)
        if(CState::posted == cstate.load())
        {
            startcommand(now);
        }

        // received frames. they end up in onRXdatagram(). checkinput() empties the whole queue of the driver, so evRX is enough.
        // we also call it at the timeout (eventmask = 0): if the stack had no memory for a frame, the frame stays in the driver
        // and no new interrupt comes for it. so it is recovered at the next tick at the latest.
        if((true == embot::core::binary::mask::check(eventmask, evRX)) || (0 == eventmask))
        {
            embot::net::lwip::sys::process();
        }

        // the packets of the sockets. transmit() alerts evTX after every packet put in a pipe and the events are flags 
        // which are not lost, so a packet cannot be left behind. an empty pipe is checked without a mutex.
        if(true == embot::core::binary::mask::check(eventmask, evTX))
        {
            transmitall();
        }

        checkresolve(now);
    }

    static void startupthread(embot::os::Thread *, void *p) { reinterpret_cast<Impl*>(p)->startup(); }
    static void oneventthread(embot::os::Thread *, embot::os::EventMask m, void *p) { reinterpret_cast<Impl*>(p)->onevent(m); }
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

theIPservice& theIPservice::getInstance()
{
    static theIPservice *p = new theIPservice;
    return *p;
}

theIPservice::theIPservice(): pImpl(std::make_unique<Impl>()) {}

theIPservice::~theIPservice() = default;


bool theIPservice::initialise(const Config &config)
{
    if((false == config.isvalid()) || (nullptr != pImpl->thread))
    {
        return false;
    }

    pImpl->config = config;
    pImpl->table.reserve(config.numberofattachablesockets);   // so that push_back() never allocates

    pImpl->thread = new embot::os::EventThread;

    embot::os::EventThread::Config cfg {};
    cfg.stacksize = config.stacksize;
    cfg.priority = config.priority;
    cfg.startup = Impl::startupthread;
    cfg.param = pImpl.get();
    cfg.timeout = config.tickperiod;
    cfg.onevent = Impl::oneventthread;
    cfg.name = "tIPservice";

        pImpl->thread->start(cfg, Impl::tIPservice);

    return true;
}

bool theIPservice::resolve(const embot::net::eth::IPaddress &remoteaddress, embot::core::relTime tout)
{
    return pImpl->execute(Impl::Cmd::Op::resolve, nullptr, remoteaddress, tout, true);
}

bool theIPservice::attach(embot::net::eth::Socket &socket, embot::core::relTime tout)
{
    return pImpl->execute(Impl::Cmd::Op::attach, &socket, {}, tout);
}

bool theIPservice::detach(embot::net::eth::Socket &socket, embot::core::relTime tout)
{
    return pImpl->execute(Impl::Cmd::Op::detach, &socket, {}, tout);
}

bool theIPservice::alert(Event ev)
{
    if(nullptr == pImpl->thread)
    {
        return false;
    }

    embot::os::EventMask m = (Event::RX == ev) ? Impl::evRX : ((Event::TX == ev) ? Impl::evTX : Impl::evCMD);
    return pImpl->thread->setEvent(m);
}

} // namespace embot::net::eth

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

