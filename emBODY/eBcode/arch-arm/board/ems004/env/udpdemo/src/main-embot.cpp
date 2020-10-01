/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#if defined(EMBOT_USE_rtos_cmsisos2)

#warning CAVEAT: this project does not run if EMBOT_USE_rtos_cmsisos2 is defined

#endif

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_os.h"


// declarations

void initSystem(embot::os::Thread *t, void* initparam);
void onIdle(embot::os::Thread *t, void* idleparam);


int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 512, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os    
    embot::os::start();

    // just because i am paranoid (thescheduler.start() never returns)
    for(;;);    
}



// extra include

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

// static 
static void s_embobj_services_start();
static void s_embobj_ipnet_start();
static void s_start_server_thread();

// definitions

void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}


void initSystem(embot::os::Thread *t, void* initparam)
{
    embot::core::print("this is a demo");    
        
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
        
    
    embot::core::print("creating the LED pulser: it will blink a LED at 1 Hz.");
        
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three, embot::hw::LED::four};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::four).pulse(embot::core::time1second); 
    
    // starting embobj services required by the ipnet
    s_embobj_services_start();
    
    // starting the ipnet    
    s_embobj_ipnet_start();
        
    // starting a thread which opens a socket and listen to it.
        
    s_start_server_thread();

    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
}


#include "EOVtheSystem_hid.h"
#include "EOtheErrorManager.h"
#include "EOMtheTimerManager.h"
#include "EOMtheCallbackManager.h"


void erromanager_onerror(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    embot::core::print(std::string(eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype)) + ": " + info);
    
    if(eo_errortype_fatal == errtype)
    {
        embot::core::print("STOPPING IN HERE FOREVER");
        for(;;);
    }
}

eOresult_t start_dummy(void (*fn)(void))
{
    embot::core::print("called sys_start_dummy() ???"); 
    
    return eores_OK;
}

void *get_task(void)
{
    //#warning TOBESOLVED
    return nullptr;    
}

uint64_t get_now(void)
{
    return embot::core::now();
}

void set_now(uint64_t t)
{
    
}

uint64_t get_nano(void)
{
    return 1000*embot::core::now();
}

static void s_embobj_services_start()
{   
    // EOVtheSystem
    const eOerrman_cfg_t errmancfg { erromanager_onerror };
        
    
    eov_sys_hid_Initialise( nullptr,            // default memory pool cfg,
                            &errmancfg,         // user defined callback for error manager 
                            start_dummy,        // dummy function
                            get_task,
                            get_now, 
                            set_now, 
                            get_nano,
                            nullptr);
    
    // EOMtheTimerManager + EOMtheCallbackManager
    // i use default configuration for now
    eom_timerman_Initialise(nullptr);
    eom_callbackman_Initialise(nullptr);
}

#include "EOMtheIPnet.h"
#include "embot_prot_eth.h"

embot::prot::eth::IPv4 myIPaddress {"10.0.1.10"};

extern const ipal_cfg_t *ipal_cfgMINE;

static void s_embobj_ipnet_start()
{
    const ipal_cfg_t* ipalcfg = ipal_cfgMINE;
    eOmipnet_cfg_addr_t eomipnet_addr;

    const eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_MyCfg = 
    {  
        2,    // one for eth command and one for can gateway
        32    // can gateway needs to be able to manage many small packets
    };
    
    // prepare the eOmipnet_cfg_addr_t
    
    eomipnet_addr.ipaddr = ipalcfg->eth_ip;
    eomipnet_addr.ipmask = ipalcfg->eth_mask;
    eomipnet_addr.macaddr = ipalcfg->eth_mac;
    
    myIPaddress = ipalcfg->eth_ip; 
    
    
    embot::core::print(std::string("IPnet: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + 
                       " starting with IP addr " + embot::prot::eth::IPv4(eomipnet_addr.ipaddr).to_string()); 

    // start the ipnet
    static const eOmipnet_cfg_t eom_ipnet_Cfg = 
    { 
        EO_INIT(.procpriority)              embot::core::tointegral(embot::os::Priority::high44), // default = 220 .. but we must give a different value
        EO_INIT(.procstacksize)             4*1024,  
        EO_INIT(.procmaxidletime)           20000, 
        EO_INIT(.procwakeuponrxframe)       eobool_true, 
        EO_INIT(.tickpriority)              embot::core::tointegral(embot::os::Priority::high43), // default = 219 .. but we must give a different value
        EO_INIT(.tickstacksize)             4*1024 // default = 128 .. but that does not allow to run prints or else
     };
    
    
    eom_ipnet_Initialise(&eom_ipnet_Cfg,
                         ipalcfg, 
                         &eomipnet_addr,
                         &eom_ipnet_dtgskt_MyCfg
                         );

 }

 
 // server thread
 
 // using embot whenever it is possible
 
#include "EOpacket.h"
#include "EOsocketDatagram.h"
#include "EOMtask.h"
#include "EOMmutex.h"
 
constexpr uint16_t capacityofUDPpacket {1200};
 
static EOsocketDatagram*        s_skt_ethcmd                = NULL;
static EOpacket*                s_rxpkt_ethcmd              = NULL;
static EOpacket*                s_txpkt_ethcmd              = NULL;
embot::os::ValueThread *tUDPserver {nullptr};
static EOaction*                s_action_ethcmd             = NULL;

static const eOipv4port_t       s_ethcmd_port               = 3333; 
//static const eOmessage_t        s_message_from_skt_ethcmd   = 0x00000001;

static const eOipv4addr_t hostipaddr = EO_COMMON_IPV4ADDR(10, 0, 1, 104);

//static const uint8_t s_priority = 25; // as in embot::os::Priority::normal25

static eObool_t eom_eupdater_main_connectsocket2host(eOipv4addr_t remaddr, EOsocketDatagram *skt, uint32_t usec);
static void s_ethcommand_startup(EOMtask *p, uint32_t t);
static void s_ethcommand_run(EOMtask *p, uint32_t t);


extern void task_ethcommand(void *p)
{
    eom_task_Start((EOMtask*)(p));
}


constexpr embot::os::Value value_RXframe {1};
constexpr embot::os::Value value_CONNECT {2};


void sendvalRXframe(void *p)
{
    embot::os::ValueThread *t = reinterpret_cast<embot::os::ValueThread*>(p);
    t->setValue(value_RXframe);
}


void sendvalCONNECT(void *p)
{
    embot::os::ValueThread *t = reinterpret_cast<embot::os::ValueThread*>(p);
    t->setValue(value_CONNECT);   
}


embot::os::Timer *connTimer {nullptr};

void tUDPserver_startup(embot::os::Thread *t, void *param)
{
    // init the rx and tx packets 
    s_rxpkt_ethcmd = eo_packet_New(capacityofUDPpacket);  
    s_txpkt_ethcmd = eo_packet_New(capacityofUDPpacket);

    // init the action used for various tasks
    s_action_ethcmd = eo_action_New();  

    // initialise the socket 
    s_skt_ethcmd = eo_socketdtg_New(  2, capacityofUDPpacket, eom_mutex_New(), // input queue: it is formed by 2 packets
                                      2, capacityofUDPpacket, eom_mutex_New()  // output queue: it is formed by 2 packets
                                   );   

    // set the action triggered on reception of a udp packet: to send a message s_message_from_skt_ethcmd to this task
    eo_action_SetCallback(s_action_ethcmd, sendvalRXframe, t, nullptr); 
    // and finally open the socket
    eo_socketdtg_Open(s_skt_ethcmd, s_ethcmd_port, eo_sktdir_TXRX, eobool_false, NULL, s_action_ethcmd, NULL);
        
    
    eom_eupdater_main_connectsocket2host(hostipaddr, s_skt_ethcmd, 3000*eok_reltime1ms);
           
    // start now a periodic timer which ask the thread to attempt the connection or to check it
    connTimer = new embot::os::Timer;
    embot::os::Action act(embot::os::CallbackToThread(sendvalCONNECT, t, nullptr));
    embot::os::Timer::Config cfg{5*embot::core::time1second, act, embot::os::Timer::Mode::forever, 0};
    connTimer->start(cfg);               
}

void tUDPserver_onvalue(embot::os::Thread *t, embot::os::Value value, void *param);

void t_UDPserver(void*p) { tUDPserver->run(); }
 
static void s_start_server_thread()
{        
    tUDPserver = new embot::os::ValueThread;
    
    embot::os::ValueThread::Config tConfig { 
        6*1024, 
        embot::os::Priority::high40, 
        tUDPserver_startup,
        nullptr,
        embot::core::reltimeWaitForever, // 50*embot::core::time1millisec, // timeout embot::core::reltimeWaitForever
        16, // queue size
        tUDPserver_onvalue,
        "tUDPserver"
    };
    
    tUDPserver->start(tConfig, t_UDPserver);      
}


static eObool_t eom_eupdater_main_connectsocket2host(eOipv4addr_t remaddr, EOsocketDatagram *skt, uint32_t usec)
{
    static eObool_t     host_connected = eobool_false;
    static eOipv4addr_t host_ipaddress = 0;
    
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();
    
    //if((eobool_false == host_connected) || (remaddr != host_ipaddress))
//    if(remaddr != host_ipaddress)
    {
        
        if(eobool_false == host_connected)
        {
            embot::core::print( "attempting to connect to remote address " + 
                                embot::prot::eth::IPv4(remaddr).to_string() + 
                                " for " + 
                                embot::core::TimeFormatter(usec).to_string()
                                );            
        }
        else
        {
            embot::core::print( "checking the connection with remote address " + 
                                embot::prot::eth::IPv4(remaddr).to_string() + 
                                " for " + 
                                embot::core::TimeFormatter(usec).to_string()
                                ); 
            
        }
            
        host_ipaddress = remaddr;
        host_connected = eobool_false;
        
        
        theleds.get(embot::hw::LED::three).on();
               
        // it is blocking
        if(eores_OK == eo_socketdtg_Connect(skt, remaddr, usec)) 
        {
            host_connected = eobool_true;
        }
        else
        {
            host_connected = eobool_false;
        }
        
        theleds.get(embot::hw::LED::three).off();
        
        embot::core::print( "the host is " + 
                            std::string((eobool_false == host_connected) ? "disconnected (red LED off)" : "connected (red LED on)")
                            );
    }
    
    if(eobool_true == host_connected)
    { 
        theleds.get(embot::hw::LED::one).stop();        
        theleds.get(embot::hw::LED::one).on();
    }
    else
    {
        // pulse it
        //theleds.get(embot::hw::LED::three).pulse(200*embot::core::time1millisec, 0);           
        //embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 30, std::bitset<64>(0b000001));
        //theleds.get(embot::hw::LED::one).wave(&ledwave); 
        theleds.get(embot::hw::LED::one).off();
    }
    

    return(host_connected);
}

static eObool_t eupdater_parser_process_mirror(EOpacket *rxpkt, EOpacket *txpkt);
static eObool_t eupdater_parser_process_replyinfo(EOpacket *rxpkt, EOpacket *txpkt);

using fpParser = eObool_t (*)(EOpacket*, EOpacket *);

void tUDPserver_onvalue(embot::os::Thread *t, embot::os::Value value, void *param)
{    
    if(0 == value)
    {
        return;
    }
        
    
    fpParser parser = nullptr;
    EOsocketDatagram *socket = nullptr;

    
    switch(value)
    {
        case value_RXframe: 
        {
            socket = s_skt_ethcmd;
            //parser = eupdater_parser_process_mirror;
            parser = eupdater_parser_process_replyinfo;            
        } break;

        case value_CONNECT:
        {
            if(eobool_true ==  eom_eupdater_main_connectsocket2host(hostipaddr, s_skt_ethcmd, 250*eok_reltime1ms))
            {
                connTimer->stop();
            }            
        } break;

        default:
        {
            socket = NULL;
            parser = NULL;
        } break;        
    }
    
    if((NULL == socket) || (NULL == parser))
    {
        return;
    }


    // get the rx packet
    eOresult_t res = eo_socketdtg_Get(socket, s_rxpkt_ethcmd, eok_reltimeINFINITE);
    

    if(eores_OK == res)
    {
        eOipv4addr_t remaddr = 0;
        eOipv4port_t remport = 0;
        eo_packet_Addressing_Get(s_rxpkt_ethcmd, &remaddr, &remport);   

// if we uncomment that we allow this program to accept messages only from the host
//        if(remaddr != hostipaddr)
//        {   // dont want anybody else but the host (pc104)
//            return;
//        }
        
        // we parse the rx packet 
        if(eobool_true == parser(s_rxpkt_ethcmd, s_txpkt_ethcmd))
        {              
            // transmit a pkt back to the host. the call blocks until success or timeout expiry
            if(eobool_true == eom_eupdater_main_connectsocket2host(remaddr, socket, 100*eok_reltime1ms))
            {
                embot::core::print("and ... trasmitting now @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
                eo_socketdtg_Put(socket, s_txpkt_ethcmd);
            }
        }
        else
        {   
            // this attempt is still-blocking, but it blocks for less time
            eom_eupdater_main_connectsocket2host(remaddr, socket, 50*eok_reltime1ms);
        } 
    }
               
}



// marco.accame@iit
// tested with netcat on linux (https://help.ubidots.com/en/articles/937233-sending-tcp-udp-packets-using-netcat)
// shell-1 $ netcat -u 10.0.1.1 -p 11111 3333
// and in this shell you just digit text and hit return
//
// 
// shell-2 $ netcat -u -l 3333
// and in this shell you will see what the board sends back (if printable)
// 
// or also 
// shell-1a $ echo hello world | netcat -w 0 -u 10.0.1.1 -p 21111 3333
// in here we transmit "hello world" and we exit 
// or also
// shell-1b $ for i in {1..10}; do echo "test-$i"| netcat -w 0 -u 10.0.1.1 -p 31111 3333; sleep 0.1; done
// it sends 10 udp frames with a 100 ms interval

static eObool_t eupdater_parser_process_replyinfo(EOpacket *rxpkt, EOpacket *txpkt)
{
    
    uint8_t *datarx {nullptr};
    uint8_t *datatx {nullptr};
    uint16_t sizerx {0};
    uint16_t sizetx {0};
    uint16_t capacitytx {0};
    eOipv4addr_t remaddr {0};
    eOipv4port_t remport {0};
    
    // get info from rx packet
    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport); 
    eo_packet_Payload_Get(rxpkt, &datarx, &sizerx);
    

    embot::core::print("Received a UDP frame with " + std::to_string(sizerx) + " bytes from " 
    + embot::prot::eth::IPv4(remaddr).to_string() + ":" + std::to_string(remport));
    
    if((0 != sizerx) && (nullptr != datarx))
    {
        if(isprint(datarx[0]) && ('\n' == datarx[sizerx-1])) 
        {
            // it may be a string sent with netcat
            datarx[sizerx] = 0; // adding a terminator
            embot::core::print("> " + std::string(reinterpret_cast<char*>(datarx)));
        }
        else
        {
            for(int i=0; i<sizerx; i++)
            {
                embot::core::print("byte[" + std::to_string(i) + "] = " + std::to_string(datarx[i]));
            }
        }
        
        embot::core::print("and i am sending info back");
        
        // ok, just mirror it
        //eo_packet_Payload_Get(txpkt, &datatx, &sizetx);
        //eo_packet_Capacity_Get(txpkt, &capacitytx);
        
        std::string txt {"board " + myIPaddress.to_string() + " has rx at port " + std::to_string(s_ethcmd_port) + 
                         " a UDP packet from " + 
                        embot::prot::eth::IPv4(remaddr).to_string() + ":" + std::to_string(remport) + 
                        " of lenght " + std::to_string(sizerx) + 
                        " @ time " + embot::core::TimeFormatter(embot::core::now()).to_string() + " \n"};

        // copy payload of rx packet inside tx packet
        char *data = const_cast<char*>(txt.c_str()); 
        eo_packet_Payload_Set(txpkt, reinterpret_cast<uint8_t*>(data), txt.length());
        // assign destination: same ip address of sender, port 3333
        //eo_packet_Destination_Set(txpkt, remaddr, remport);
        eo_packet_Destination_Set(txpkt, remaddr, s_ethcmd_port);
    }
    else
    {
        embot::core::print(" .... but the payload has a problem");
    }
    
    
    return eobool_true;
}

static eObool_t eupdater_parser_process_mirror(EOpacket *rxpkt, EOpacket *txpkt)
{
    
    uint8_t *datarx {nullptr};
    uint8_t *datatx {nullptr};
    uint16_t sizerx {0};
    uint16_t sizetx {0};
    uint16_t capacitytx {0};
    eOipv4addr_t remaddr {0};
    eOipv4port_t remport {0};
    
    // get info from rx packet
    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport); 
    eo_packet_Payload_Get(rxpkt, &datarx, &sizerx);
    

    embot::core::print("Received a UDP frame with " + std::to_string(sizerx) + " bytes: ");
    
    if((0 != sizerx) && (nullptr != datarx))
    {
        for(int i=0; i<sizerx; i++)
        {
            embot::core::print("byte[" + std::to_string(i) + "] = " + std::to_string(datarx[i]));
        }
        
        embot::core::print(" and i am sending it back");
        
        // ok, just mirror it
        //eo_packet_Payload_Get(txpkt, &datatx, &sizetx);
        //eo_packet_Capacity_Get(txpkt, &capacitytx);

        // copy payload of rx packet inside tx packet
        eo_packet_Payload_Set(txpkt, datarx, sizerx);
        // assign destination: same ip address of sender, port 3333
        //eo_packet_Destination_Set(txpkt, remaddr, remport);
        eo_packet_Destination_Set(txpkt, remaddr, s_ethcmd_port);
    }
    else
    {
        embot::core::print(" .... but the payload has a problem");
    }
    
    
    return eobool_true;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



