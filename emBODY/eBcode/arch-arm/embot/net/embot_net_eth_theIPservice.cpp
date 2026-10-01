

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_net_eth_theIPservice.h"

#include <vector>

#include "embot_os_Thread.h"
#include "embot_os_theScheduler.h"
#include "embot_os_rtos.h"
#include "embot_net_lwip.h"

// lwip types used by the receive callback. they live in the global namespace
struct udp_pcb;
struct pbuf;

namespace embot::net::eth {

namespace {

//    #warning: add evRX inside class Impl so that ....
//    // events of the IP service thread. They match theIPservice::Event
//    constexpr embot::os::EventMask evRX  = embot::os::bitpos2event(0); // embot::core::tointegral(theIPservice::Event::RX));
//    constexpr embot::os::EventMask evTX  = embot::os::bitpos2event(1);
//    constexpr embot::os::EventMask evCMD = embot::os::bitpos2event(2);

    // while resolving, the ARP request is repeated with this period until the timeout expires
    constexpr embot::core::relTime arpretryperiod = 100 * embot::core::time1millisec;

    // runs the thread. same trick used by embot::os::theTimerManager
    void tIPservice(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }
}

// --------------------------------------------------------------------------------------------------------------------
// - pimpl
// --------------------------------------------------------------------------------------------------------------------

// Everything which touches the IP stack, the table of attached sockets, the scratch packets and the pending
// command state is used ONLY by the IP service thread: no lock is needed for them.
// The only objects shared with the callers are the command slot (protected by mtxcaller + done semaphore).

struct theIPservice::Impl
{
    
    static constexpr embot::os::EventMask evRX  = embot::os::bitpos2event(embot::core::tointegral(theIPservice::Event::RX));
    static constexpr embot::os::EventMask evTX  = embot::os::bitpos2event(embot::core::tointegral(theIPservice::Event::TX));
    static constexpr embot::os::EventMask evCMD = embot::os::bitpos2event(embot::core::tointegral(theIPservice::Event::CMD));
    
    Config config {};
    State st {State::notactive};
    embot::os::EventThread *thread {nullptr};

    // - the command slot. one command at a time ------------------------------------------------------------------

    struct Cmd
    {
        enum class Op { none, attach, detach, resolve };
        Op op {Op::none};
        Socket *socket {nullptr};
        IPaddress address {};
        embot::core::relTime tout {0};
        bool result {false};
    };

    Cmd cmd {};
    embot::os::rtos::mutex_t *mtxcaller {nullptr};      // serialises the callers. it has priority inheritance
    embot::os::rtos::semaphore_t *done {nullptr};       // released once, by finish(), when a command is completed

    // state of a resolve() which is waiting for the ARP reply
    bool resolving {false};
    embot::core::Time arpdeadline {0};
    embot::core::Time arplast {0};

    // - attached sockets ----------------------------------------------------------------------------------------

    struct Entry
    {
        Socket *socket {nullptr};
        embot::net::lwip::udp::OBJ *pcb {nullptr};
    };
    std::vector<Entry> table {};

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
    
    bool callerisok() const
    {
        embot::os::Thread *caller = embot::os::theScheduler::getInstance().scheduled();
        
        // nullptr: scheduler not started or called from an ISR. 
        if((nullptr == caller) || (caller == thread))
        {
            return false;
        }
        
        // in embot::os::Priority a higher value means a higher priority
        return (caller->getPriority() < thread->getPriority());
    }
    
    bool execute(Cmd::Op op, Socket *s, const IPaddress &a, embot::core::relTime tout)
    {
        // not started, or called by a thread which cannot use it (the service thread itself would wait for itself) 
        if((nullptr == thread) || (false == callerisok()))
        {
            return false;
        }

        embot::os::rtos::Lock lock(mtxcaller);

        cmd.op = op;
        cmd.socket = s;
        cmd.address = a;
        cmd.tout = tout;
        cmd.result = false;

        thread->setEvent(evCMD);

        // the service thread always completes the command (also resolve(), at the latest at its timeout)
        #warning evaluate if add a timeout .... but how ....
        embot::os::rtos::semaphore_acquire(done, embot::core::reltimeWaitForever);

        return cmd.result;
    }


    // - service thread side --------------------------------------------------------------------------------------

    // the only place where a command is completed. it is called only when the slot holds a command,
    // so it releases the semaphore exactly once per command (the old code could release it twice for ARP)
    void finish(bool result)
    {
        if(Cmd::Op::none == cmd.op)
        {
            return;
        }
        cmd.result = result;
        cmd.op = Cmd::Op::none;
        resolving = false;
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
        if((table.size() >= config.sockets.numberofattachable) || (nullptr != find(&s)))
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

        table.push_back({&s, pcb});
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
        transmit(*e);

        // releasing the pcb stops the reception: after it the stack does not use the socket anymore
        embot::net::lwip::udp::release(e->pcb);
        table.erase(table.begin() + (e - table.data()));
        return true;
    }

    // it sends all the packets of the output pipe of a socket
    void transmit(Entry &e)
    {
        while(true == e.socket->remTXfifo(txpacket))
        {
            auto *pk = embot::net::lwip::pkt::retrieve(txpacket.size());
            if(nullptr == pk)
            {
                continue;   // no memory: the packet is dropped
            }
            embot::net::lwip::pkt::load(pk, txpacket.data(), txpacket.size());
            embot::net::lwip::udp::send(e.pcb, pk, {txpacket.address().addr, txpacket.address().port});
            // the pbuf is always released here by us, also if the send fails (as the old code did after a successful send)
            embot::net::lwip::pkt::release(pk);
        }
    }

    void transmitall()
    {
        for(auto &e : table)
        {
            transmit(e);
        }
    }

    void startcommand(embot::core::Time now)
    {
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

    // executed at every event and at every timeout (mask = 0).
    void onevent(embot::os::EventMask eventmask)
    {
        embot::core::Time now = embot::core::now();

        // the stack tick: the thread wakes up at least every tickperiod.
        while((now - lasttick) >= config.tickperiod)
        {
            embot::net::lwip::sys::tick();
            lasttick += config.tickperiod;
        }

        if(true == embot::core::binary::mask::check(eventmask, evCMD)) 
        //if((0 != (mask & evCMD)) && (Cmd::Op::none != cmd.op))
        {
            if(Cmd::Op::none != cmd.op)
            {
                startcommand(now);
            }
        }

        // received frames. they end up in onRXdatagram()
        embot::net::lwip::sys::process();

        // we scan the sockets at every wake up (not only when evTX is set): what is sent depends on the content of the pipes,
        // not on the number of events received. no packet can be left behind because an event was lost
        transmitall();

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
    pImpl->table.reserve(config.sockets.numberofattachable);   // so that push_back() never allocates

    pImpl->thread = new embot::os::EventThread;

    embot::os::EventThread::Config cfg {};
    cfg.stacksize = config.stacksize;
    cfg.priority = config.priority;
    cfg.startup = Impl::startupthread;
    cfg.param = pImpl.get();
    cfg.timeout = config.tickperiod;
    cfg.onevent = Impl::oneventthread;
    cfg.name = "tIPservice";

    pImpl->thread->start(cfg, tIPservice);

    return true;
}

bool theIPservice::set(State st)
{
    pImpl->st = st;     // TODO: not used yet by the thread
    return true;
}

theIPservice::State theIPservice::state() const
{
    return pImpl->st;
}

bool theIPservice::resolve(const embot::net::eth::IPaddress &remoteaddress, embot::core::relTime tout)
{
    return pImpl->execute(Impl::Cmd::Op::resolve, nullptr, remoteaddress, tout);
}

bool theIPservice::attach(embot::net::eth::Socket &socket, embot::core::relTime tout)
{
    (void)tout;
    return pImpl->execute(Impl::Cmd::Op::attach, &socket, {}, 0);
}

bool theIPservice::detach(embot::net::eth::Socket &socket, embot::core::relTime tout)
{
    (void)tout;
    return pImpl->execute(Impl::Cmd::Op::detach, &socket, {}, 0);
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

