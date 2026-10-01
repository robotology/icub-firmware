

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
//constexpr embot::net::eth::SocketsConfig ipsockets { 4, 4 };

#include "embot_net_eth.h"
#include "embot_net_eth_Packet.h"
#include "embot_net_eth_Socket.h"
#include "embot_net_eth_theIPservice.h"

constexpr embot::net::eth::IPaddress localaddress {10, 0, 1, 99};
constexpr embot::net::eth::IPaddress hostaddress {10, 0, 1, 104};

constexpr embot::net::eth::IPconfig ipconfig
{
    {0x70, 0x9A, 0x0B, 0x00, 0x00, 0x00},   // mac address
    localaddress,                           // ip address
    {255, 255, 255, 0},                     // netmask
    {10, 0, 1, 104}                         // gateway 
};


constexpr embot::net::eth::theIPservice::Config ipSERcfg 
{
    ipconfig,
    {4, 4},
    embot::os::Priority::system50,
    4*1024,
    10*embot::core::time1millisec,
    1500
};

constexpr embot::os::Event evt666rxframe = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evt666sendreply = embot::core::binary::mask::pos2mask<embot::os::Event>(1); 
constexpr embot::os::Event evt666txframe = embot::core::binary::mask::pos2mask<embot::os::Event>(2);    

void alert666RXframe(void *p)
{
    embot::os::Thread *t =  reinterpret_cast<embot::os::Thread*>(p);
    t->setEvent(evt666rxframe);
}

void alert666TXframe(void *p)
{
    embot::os::Thread *t =  reinterpret_cast<embot::os::Thread*>(p);
    t->setEvent(evt666txframe);
} 
    
embot::net::eth::Socket *sock666 {nullptr};
    
void eventbasedthread_startup(embot::os::Thread *t, void *param)
{  
    embot::core::Callback on666rx {alert666RXframe, t};
    embot::core::Callback on666tx {alert666TXframe, t};    
    embot::net::eth::Socket::Properties props666
    {
        666, embot::net::eth::IPany,    // listens on port 666 and accepts from ant IP address
        {
            on666rx,                    // when a frame arrives on 666 port this callback is executed
            on666tx                     // when a frame is delivered to eth ETH peripheral this callback is executed
        }
    };    

    // start the service. it starts lwip, related ETH hw, manages ping, can accepts some sockets
    embot::net::eth::theIPservice::getInstance().initialise(ipSERcfg);
        
    // and now ... i create the server socket 
    // for now default size of its pipes is 1 packet in tx and rx FIFO of size 1500 bytes
    sock666 = new embot::net::eth::Socket({});  
    // i attach the socket to theIPservice
    sock666->open(props666);
    // force ARP to host
    sock666->connect(hostaddress);
    
}
constexpr size_t packetcapacity {1500};
embot::net::eth::Packet rxP {packetcapacity};     
embot::net::eth::Packet txP {packetcapacity};

uint8_t buffer[1500] {0}; // maybe we could reuse teh apckets

void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{      
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }
    
    embot::core::Time timenow {embot::core::now()};
    embot::core::TimeFormatter tnf {timenow};    
    
    if(true == embot::core::binary::mask::check(eventmask, evt666sendreply)) 
    {
        bool ok = sock666->transmit(txP);
        // clear txP
        txP.setsize(0);
        txP.setaddress({});
        embot::core::print("frame given for transmission @ " + tnf.to_string());        
	}     

    if(true == embot::core::binary::mask::check(eventmask, evt666rxframe)) 
    {               
        bool ok = sock666->receive(rxP);
        if(true == ok)
        {
            size_t s = rxP.size();
            embot::net::eth::SocketAddress sa {rxP.address()};
            embot::core::print("frame of " + std::to_string(s) + " bytes received @ " + tnf.to_string() + " from " + sa.to_string() + " and preparing a reply back");
            // prepare txP
            std::memmove(buffer, rxP.data(), s);
            buffer[0] = '-';
            txP.load(s, buffer);
            txP.setaddress(sa);
            // clear rxP
            rxP.setsize(0);
            rxP.setaddress({});
            
            t->setEvent(evt666sendreply);
        }
        
        size_t n = sock666->input();
        if(n >0)
        {
            embot::core::print("there are other " + std::to_string(n) + " frames in input. sending a evt666rxframe to process a new one");
            t->setEvent(evt666rxframe);
        }
	}  
    
    if(true == embot::core::binary::mask::check(eventmask, evt666txframe)) 
    {       
        embot::core::print("frame trasmitted @ " + tnf.to_string());
	} 
        
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

