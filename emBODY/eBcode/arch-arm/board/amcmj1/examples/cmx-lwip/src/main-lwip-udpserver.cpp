
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_hw_dualcore.h"
#include "stm32hal.h"
   


#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_sys.h"


#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

#include "embot_app_scope.h"

#include <vector>

#include "embot_hw_bsp_config.h"

#include "embot_hw_led.h"
#include "embot_hw_sys.h"


void eventbasedthread_startup(embot::os::Thread *t, void *param);
void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);

void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

void tMAIN(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}

void initSystem(embot::os::Thread *t, void* initparam)
{    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
             
    static const std::initializer_list<embot::hw::LED> allleds = 
    { 
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three
    };  
    
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    
    theleds.init(allleds);  
    for(const auto &l : allleds)
    {
        theleds.get(l).on();
        theleds.get(l).off();

    }        
    
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        5*embot::core::time1millisec,
        eventbasedthread_onevent,
        "mainThreadEvt"
    };
       
        
    // create the main thread 
    embot::os::EventThread *thr {nullptr};
    thr = new embot::os::EventThread;          
    // and start it. w/ osal it will be displayed w/ label tMAIN
    thr->start(configEV, tMAIN); 
    
    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
}


// --------------------------------------------------------------------------------------------------------------------

int main(void)
{         
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig 
    {
        embot::core::time1millisec, initcfg, idlecfg, onOSerror, 
        embot::hw::FLASHpartitionID::eapplication01
    };
    
    bool iamthemaster = embot::hw::dualcore::ismaster();

    if(true == iamthemaster)
    {
        constexpr embot::hw::dualcore::Config dualcoreconfig {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate};
        embot::hw::dualcore::config(dualcoreconfig);
    }
    

    embot::os::init(osconfig);     
    embot::os::start();
}


// -- all the code for the lwip UDP server

constexpr embot::os::Event evtLWIPtick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::core::relTime tickperiod = 100*embot::core::time1millisec;
constexpr embot::os::Event evtLWIPrxframe = embot::core::binary::mask::pos2mask<embot::os::Event>(1);

// this is a test program for embot::lwip
// we can use simple functions or objects
// in here are both versions

//#define TEST_net_lwip_functions
#define TEST_net_lwip_objects

#include "embot_net_lwip.h"


constexpr embot::net::eth::IPconfig ipconfig 
{
    {0x70, 0x9A, 0x0B, 0x00, 0x00, 0x00},   // mac address
    {10, 0, 1, 99},                         // ip address
    {255, 255, 255, 0},                     // netmask
    {10, 0, 1, 104}                         // gateway    
};   
    
embot::net::eth::Port listeningport666 {666};
embot::net::eth::Port listeningport667 {667};

struct packet {
    uint8_t data[1500];
    size_t  size;
};
    
packet pkt{}; 
   

#if defined(TEST_net_lwip_objects)

// global UDPscoket. i will use std::move(). or as an alternative i can use a pointer
embot::net::lwip::UDPsocket uu666 {};
embot::net::lwip::UDPsocket uu667 {};
     
#elif defined(TEST_net_lwip_functions)

    embot::net::lwip::udp::OBJ *udp0 {nullptr};
    
#endif
    
    
void onrx(void *arg, struct udp_pcb *upcb, struct pbuf *rxpkt, const embot::net::eth::IPaddress *ipaddr, const embot::net::eth::Port port)
{
    if (nullptr == rxpkt)
    {
        return; // in this case no need to destroy rxpkt
    }

#if defined(TEST_net_lwip_objects)
    
    // rxpacket get ownership of rxpkt and frees it internally w/ pbuf_free() when it goes out of scope (as lwip requires)
    embot::net::lwip::Packet rxpacket(rxpkt); 

    size_t rxpktsize = rxpacket.size();
    if(rxpktsize > sizeof(pkt.data))
    {   // rxpacket is out of scope: pbuf_free automatically called
        embot::core::print("UDP packet too large, size = " + std::to_string(rxpktsize));
        return; 
    }

    // copy what is in rxpacket (aka inside rxpkt) into our buffer
    pkt.size = rxpacket.copyto(pkt.data);

    // gets the transmission address
    const embot::net::eth::SocketAddress socketaddress {*ipaddr, port};
    
    // i will need to transmit over the same socket whose lwip descriptor is given in teh callback by upcb
    // so i create a wrapping socket over it
    embot::net::lwip::UDPsocket socket = embot::net::lwip::UDPsocket::wrap(upcb); 
    
    embot::core::print("socket listening on port " + std::to_string(socket.localport()) + " received a frame of " + std::to_string(rxpktsize) +  " bytes from " + socketaddress.to_string() );

    // prepare a reply packet of same size
    embot::net::lwip::Packet replypacket(rxpacket.size());

    if(!replypacket.isvalid())
    {
        embot::core::print("onrx() cannot allocate replypacket");
        return; // replypacket and rxpacket is automatically freed
    }

    // now i assume i received text, i change first letter and then load it into the reply packet
    if(666 == socket.localport())
    {
        pkt.data[0] = '#';
    }
    else
    {
        pkt.data[0] = '-';
    }
    replypacket.load(pkt.data, pkt.size);
    

    // and i send the reply over it
    socket.send(replypacket, socketaddress);

    // no need to manually delete rxpkt and the packet used to reply. when out of scope the objects do that for me
    
#elif defined(TEST_net_lwip_functions)

    // rxPKT is nothing but the received pointer
    embot::net::lwip::pkt::OBJ * rxPKT = rxpkt;
    
    size_t rxpktsize = embot::net::lwip::pkt::size(rxPKT);
    if(rxpktsize > sizeof(pkt.data))
    {   // must destroy the rxpkt because lwip requires that
        embot::core::print("UDP packet too large, size = " + std::to_string(rxpktsize));
        embot::net::lwip::pkt::release(rxPKT);
        return; 
    }

    // copy what is in rxPKT (aka inside rxpkt) into our buffer
    pkt.size = embot::net::lwip::pkt::copyto(rxPKT, pkt.data);

    // gets the transmission address
    const embot::net::eth::SocketAddress socketaddress {*ipaddr, port};
    
    embot::core::print("received a frame of " + std::to_string(rxpktsize) +  " bytes from " + socketaddress.to_string() );

    // prepare a reply packet of same size
    embot::net::lwip::pkt::OBJ *replyPKT = embot::net::lwip::pkt::retrieve(rxpktsize);

    if (nullptr == replyPKT)
    {   // must deallocate rxpkt because lwip requires that
        embot::core::print("onrx() cannot allocate replyPKT");
        embot::net::lwip::pkt::release(rxPKT);
        return; 
    }

    // now i assume i received text, i change first letter and then load it into a prely
    pkt.data[0] = '#';
    embot::net::lwip::pkt::load(replyPKT, pkt.data, pkt.size);

    // and i send the reply over the same socket
    embot::net::lwip::udp::OBJ * rxUDP = upcb;
    embot::net::lwip::udp::send(rxUDP, replyPKT, socketaddress);

    // must now manually delete rxpkt and the packet used to reply
    embot::net::lwip::pkt::release(rxPKT);
    embot::net::lwip::pkt::release(replyPKT);
    
#endif    
}


void udpserver_init(embot::net::eth::Port listport, void *arg)
{
#if defined(TEST_net_lwip_objects)    
    
    embot::net::lwip::UDPsocket sock {}; // ctor internally calls udp_new()

    if (!sock.isvalid())
    {
        return; // failure of udp_new(): the dtor cleans up what is needed
    }

    // i want to listen from any IP address, but only using a given port
    bool binded = sock.bind({embot::net::eth::IPany, listport});

    if (false == binded)
    {   // bind has failed: sock goes out of scope and the ftor calls udp_remove()
        return; 
    }
    
    // set a callback upon reception of a frame on the socket
    sock.recv({onrx, arg});

    // now i can move ownership to the global object uu
    if(listeningport666 == listport)
    {
        uu666 = std::move(sock); 
    }
    else
    {
        uu667 = std::move(sock); 
    }
    
#elif defined(TEST_net_lwip_functions)

    udp0 = embot::net::lwip::udp::retrieve();
    
    embot::net::lwip::udp::OBJ *uu[6] = {nullptr};
    
//    for(uint8_t i=0; i<6; i++)
//    {
//        uu[i] = embot::net::lwip::udp::retrieve();
////        embot::net::lwip::udp::release(pp[i]);
//    }
    
//    embot::net::lwip::pkt::OBJ *pp[20] = {nullptr};
//    for(uint8_t i=0; i<20; i++)
//    {
//        pp[i] = embot::net::lwip::pkt::retrieve(1500);
////        embot::net::lwip::udp::release(pp[i]);
//    }
    
    
    if(nullptr == udp0)
    {
        return;
    }
    
    bool binded =  embot::net::lwip::udp::bind(udp0, {embot::net::eth::IPany, listport});
    
    if(false == binded)
    {   // bind has failed: must explicitly call a udp_remove()
        embot::net::lwip::udp::release(udp0);
        udp0 = nullptr;
        return;
    }
    
    // set a callback upon reception of a frame on the socket
    embot::net::lwip::udp::recv(udp0, {onrx, arg});  
    
#endif
}



void ONE(){};
embot::app::scope::Signal *signalONE {nullptr};
constexpr embot::app::scope::SignalEViewer::Config ccONE{ONE, embot::app::scope::SignalEViewer::Config::LABEL::one};

void TWO(){};
embot::app::scope::Signal *signalTWO {nullptr};
constexpr embot::app::scope::SignalEViewer::Config ccTWO{TWO, embot::app::scope::SignalEViewer::Config::LABEL::two};

// used to wake ut the thread for processing a received frame
void alertRXframe(void *p)
{
    embot::os::Thread *t =  reinterpret_cast<embot::os::Thread*>(p);
    t->setEvent(evtLWIPrxframe);
}

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{   

    signalONE = new embot::app::scope::SignalEViewer(ccONE);  
    signalTWO = new embot::app::scope::SignalEViewer(ccTWO);      


    // i start lwip    
    embot::core::Callback cbk {alertRXframe, t};
    embot::net::lwip::sys::init(ipconfig, cbk); 
    
    // i start a udp server listening on main port  
    udpserver_init(listeningport666, t);
    udpserver_init(listeningport667, t);
  
    // i impose a regular tick for the lwip statck
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtLWIPtick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
}


void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{      
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtLWIPtick)) 
    {               
        embot::net::lwip::sys::tick();
        if(embot::net::lwip::arp::isresolved({10, 0, 1, 104}))
        {
            embot::app::theLEDmanager::getInstance().get(embot::hw::LED::one).on();
        }
        else
        {
            embot::app::theLEDmanager::getInstance().get(embot::hw::LED::one).off();
            embot::net::lwip::arp::resolve({10, 0, 1, 104}, false);
        }
	}  

    if(true == embot::core::binary::mask::check(eventmask, evtLWIPrxframe)) 
    {               
        embot::net::lwip::sys::process();
        //embot::net::lwip::tick();
	} 
        
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
