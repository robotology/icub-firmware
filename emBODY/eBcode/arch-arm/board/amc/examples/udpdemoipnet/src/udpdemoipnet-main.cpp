/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

    This program is a basic demo for the new IPAL which uses object EOMtheIPnet.
    That is useful to test compatibility with FW framework developed for the ems board.
    
    In order to use EOMtheIPnet we need to launch the application with the embobj style.
    However, for latest boards we need to use the stm32hal library genrated w/ cubemx
    and the embot environment on top of that.
    
    This program, hence is also an example of how to use legacy embobj code inside the
    new stm32hal-embot framework.
    
    
    The program launches three user treads: 
    - IPALproc: inits an starts the ipal, inits support for sockets, executes function ipal_sys_process_communication()
    - IPALtick: periodically calls function ipal_sys_timetick_increment()
    - tUDPsrvr: performs communication by listening on port 3333 
      

#endif


#if defined(EMBOT_USE_rtos_cmsisos2)
//#error CAVEAT: this project does not run if EMBOT_USE_rtos_cmsisos2 is defined
#endif



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------



// embobj required to start the system
#include "EOMtheSystem.h"
// EOMtheSystem includes also the others, thus is the only one needed ...
//#include "EOVtheSystem.h"
//#include "EOtheMemoryPool.h"
//#include "EOtheErrormanager.h"

// legacy abslayer required by embobj
//#include "hal.h"
//#include "ipal.h"

// includes used to see used variables for configuration

// to see a fake hal_cfg required by eOmsystem_cfg_t
//#include "hal_core_cfg.h"

//#warning CHECK: maybe we can use a dummy hal_core_cfg_t .............
//const hal_core_cfg_t hal_dummy_cfg = {};



// - used variables

static void s_used_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);

static const eOmsystem_cfg_t s_used_syscfg =
{
    .codespaceoffset    = 0,            // if the application is placed at zero offset, otherwise put in here the offset
    .halcfg             = nullptr,      // dont need any in this case
    .osalcfg            = nullptr,      // dont need any in this case
    .fsalcfg            = nullptr       // never used any
};

static const eOerrman_cfg_t s_used_errcfg = 
{
    s_used_errman_OnError
};

static void s_initialiser(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition of main
// --------------------------------------------------------------------------------------------------------------------

int main(void)
{   
    eom_sys_Initialise( &s_used_syscfg,
                        &eom_thesystem_mempool_cfg_osal,    // mempool
                        &s_used_errcfg,                     // errman
                        &eom_timerman_DefaultCfg,
                        &eom_callbackman_DefaultCfg
                      );  

    // the system executes s_initialiser() and then schedules the other threads
    eom_sys_Start(eom_sys_GetHandle(), s_initialiser);
}


// static funtions
#include "embot_core.h"

static void s_init_ipnet();
static void s_init_ledmanager();
static void s_init_server();
static void s_start_ETHmonitor_thread();

static void s_initialiser(void)
{
    // we init the emBOBJ object EOMtheIPnet. 
    // it starts two threads which manage IPAL
    // as a result of that we can at least ping the board
    s_init_ipnet();
    
    // we init the emBOT object embot::app::theLEDmanager
    s_init_ledmanager();
    
    // we also start a new thread which manages a udp socket
    s_init_server();
    
    // also starting a periodic thread where we can ask the status of the eth switch
    s_start_ETHmonitor_thread();
}


// for the ipnet we need:
#include "EOMtheIPnet.h"

// for the ipal config we use (variable ipal_cfg2) we need:
#include "ipal_cfg2.h"

// for others
#include "embot_os.h"
#include "embot_prot_eth.h"

embot::prot::eth::IPv4 localIPaddress {10, 0, 1, 99};

static void s_init_ipnet()
{
    static const eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_MyCfg = 
    { 
        .numberofsockets = 2,
        .maxdatagramenqueuedintx = 4
    };
    
    // start the ipnet
    static const eOmipnet_cfg_t eom_ipnet_Cfg = 
    { 
        .procpriority = embot::core::tointegral(embot::os::Priority::high44), // default = 220 .. but we must give a different value
        .procstacksize = 4*1024, // default = 1024 .. 
        .procmaxidletime = 25000, // 25 ms
        .procwakeuponrxframe = eobool_true, 
        .tickpriority = embot::core::tointegral(embot::os::Priority::high43), // default = 219 .. but we must give a different value
        .tickstacksize = 4*1024 // default = 128 .. but that does not allow to run prints or else
    };
 
    static eOmipnet_cfg_addr_t eomipnet_addr = { };
    eomipnet_addr.macaddr = ipal_cfg2.eth->eth_mac;
    eomipnet_addr.ipaddr = ipal_cfg2.eth->eth_ip;
    eomipnet_addr.ipmask = ipal_cfg2.eth->eth_mask;
    
    localIPaddress = ipal_cfg2.eth->eth_ip;
        
    ipal_cfg_any_t * ipal_cfg2use = &ipal_cfg2;
           
    embot::core::print(std::string("IPnet: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + 
                       " starting with IP addr " + embot::prot::eth::IPv4(ipal_cfg2.eth->eth_ip).to_string()); 

    // ok, we can safely initialise the IPnet
    eom_ipnet_Initialise(&eom_ipnet_Cfg,
                         ipal_cfg2use, 
                         &eomipnet_addr,
                         &eom_ipnet_dtgskt_MyCfg
                         );
    
}

#include "embot_app_theLEDmanager.h"

static void s_init_ledmanager()
{
    static const std::initializer_list<embot::hw::LED> allleds = 
    { 
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three, 
        embot::hw::LED::four, embot::hw::LED::five, embot::hw::LED::six
    };  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);   

    // and we pulse the first led
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second, 0);  
}

embot::os::ValueThread *serverthread {nullptr};
static void serverthread_startup(embot::os::Thread *t, void *p);
static void serverthread_onvalue(embot::os::Thread *t, embot::os::Value v, void *p);

void tUDPsrvr(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }
    
static void s_init_server()
{
    embot::core::print("INIT: creating the server thread. it manages a server socket");  
    
    constexpr uint16_t stacksize{6*1024};
    constexpr embot::core::relTime timeout {100*embot::core::time1millisec};
    constexpr uint8_t valueQsize {4};
    embot::os::ValueThread::Config configServer { 
        stacksize, 
        embot::os::Priority::normal24, 
        serverthread_startup, nullptr,
        timeout,
        valueQsize,
        serverthread_onvalue,
        "srvUDPthread"
    };
       
        
    // create the server thread 
    serverthread = new embot::os::ValueThread;          
    // and start it
    serverthread->start(configServer, tUDPsrvr);         
}


#include "EOsocketDatagram.h"
#include "EOMmutex.h"

EOsocketDatagram* s_skt_ethcmd {nullptr};
EOpacket* s_rxpkt_ethcmd {nullptr};
EOpacket* s_txpkt_ethcmd {nullptr};
EOaction* s_action_ethcmd {nullptr};


constexpr eOipv4port_t serverPort {3333};
constexpr uint16_t capacityofUDPpacket {1200};
embot::prot::eth::IPv4 hostIPaddress {10, 0, 1, 104};

constexpr embot::os::Value valueFROMsocket {1};

void alertserverthread(void *p)
{
    serverthread->setValue(valueFROMsocket);
}

bool s_connectsocket2host(const embot::prot::eth::IPv4& hostaddress, EOsocketDatagram *skt, embot::core::relTime timeout);

static void serverthread_startup(embot::os::Thread *t, void *p)
{
    // in here we use mostly emBOBJ objects 
    
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
    // unfortunately ... the emBOBJ object EOaction does not know about embot::os::Thread ... so i need to ask it to
    // execute a callback. directly.    
    eo_action_SetCallback(s_action_ethcmd, alertserverthread, nullptr, nullptr);
    
    // and finally open the socket
    eo_socketdtg_Open(s_skt_ethcmd, serverPort, eo_sktdir_TXRX, eobool_false, NULL, s_action_ethcmd, NULL);
    
    embot::core::print(std::string("UDP server: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + 
                  " a UDP socket is listening at " + embot::prot::eth::IPv4(localIPaddress).to_string() + ":" + std::to_string(serverPort) 
                  ); 

    // we also want to connect to the host
    if(true == s_connectsocket2host(hostIPaddress, s_skt_ethcmd, 3000*embot::core::time1millisec))
    {
        embot::core::print(std::string("UDP server: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + 
                          " connected to host " + embot::prot::eth::IPv4(hostIPaddress).to_string() 
                          );    
    }
    else
    {
        embot::core::print(std::string("UDP server: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + 
                          " FAILED connection to host " + embot::prot::eth::IPv4(hostIPaddress).to_string() 
                          );  
    }    
}

bool parser(EOpacket *rxpkt, EOpacket *txpkt);

static void serverthread_onvalue(embot::os::Thread *t, embot::os::Value v, void *p)
{
    if(v == 0)
    {
        return;
    }
    
    EOsocketDatagram *socket {nullptr};
    
    switch(v)
    {
        case valueFROMsocket:
        {
            socket = s_skt_ethcmd;
        } break;
        
        default:
        {
            return;
        } break;
    }
    
    if(nullptr != socket)
    {
        eOresult_t res = eo_socketdtg_Get(socket, s_rxpkt_ethcmd, eok_reltimeINFINITE);
        if(eores_OK == res)
        {
            eOipv4addr_t remaddr {0};
            eOipv4port_t remport {};
            eo_packet_Addressing_Get(s_rxpkt_ethcmd, &remaddr, &remport);  

            // we parse the rx packet 
            if(true == parser(s_rxpkt_ethcmd, s_txpkt_ethcmd))
            {   
                // if in here we have received a valid udp packet and we have just stopped the countdown inside the parser 
                
                // transmit a pkt back to the host. the call blocks until success or timeout expiry
                if(true == s_connectsocket2host({remaddr}, socket, 1000*embot::core::time1millisec))
                {
                    eo_socketdtg_Put(socket, s_txpkt_ethcmd);
                }
            }
            else
            {   
                // this attempt is still-blocking, but it blocks for less time
                s_connectsocket2host({remaddr}, socket, 250*embot::core::time1millisec);
            }
        }            
        
    }
    
}

#define MIRROR_COMMAND

#include <string.h>
    
bool parser(EOpacket *rxpkt, EOpacket *txpkt)
{    
    eOipv4addr_t remaddr {0};
    eOipv4port_t remport {};
    eo_packet_Addressing_Get(s_rxpkt_ethcmd, &remaddr, &remport); 
        
    uint8_t *pData {nullptr};
    uint16_t size {0};
        
    eo_packet_Payload_Get(s_rxpkt_ethcmd, &pData, &size);
    
    std::string nn = embot::core::TimeFormatter(embot::core::now()).to_string();
        
    embot::core::print(std::string("UDP server: @ ") + nn + 
                        " received a packet from " + embot::prot::eth::IPv4(remaddr).to_string() + ":" + std::to_string(remport)
                       );
        
    embot::core::print(std::string("size = ") + std::to_string(size) + ", string = " + std::string(reinterpret_cast<char*>(pData))
                       );
    
#if !defined(MIRROR_COMMAND)    
    return false;
#else
    
    char replybuffer[128] = {0};
    snprintf(replybuffer, sizeof(replybuffer), "@ %s -> RX = %s", nn.c_str(), pData);
    // copy payload of rx packet inside tx packet
    //eo_packet_Payload_Set(txpkt, pData, size);
    eo_packet_Payload_Set(txpkt, reinterpret_cast<uint8_t*>(replybuffer), strlen(replybuffer));
    // assign destination: same ip address of sender, port 3333
    eo_packet_Destination_Set(txpkt, remaddr, remport);
    return true;
#endif    
}

// blocking call
bool s_connectsocket2host(const embot::prot::eth::IPv4& hostaddress, EOsocketDatagram *skt, embot::core::relTime timeout)
{
    static bool host_connected {false};
    static embot::prot::eth::IPv4 host_ipaddress {};

    // eOipv4addr_t and embot::prot::eth::IPv4 have the same memory layout
    if((false == host_connected) || (hostaddress.v != host_ipaddress.v))
    {
        host_ipaddress = hostaddress;
        host_connected = false;
               
        // it is blocking
        if(eores_OK == eo_socketdtg_Connect(skt, hostaddress.v, timeout)) 
        {
            host_connected = true;
        }
        else
        {
            host_connected = false;
        }
    }
    
    if(true == host_connected)
    {  
        // embot::hw::LED::three signals that the board is connected
        embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance(); 
        theleds.get(embot::hw::LED::three).on();
    }
    else
    {
        // pulse embot::hw::LED::three
        embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
        //theleds.get(embot::hw::LED::three).pulse(200*embot::core::time1millisec, 0);           
        embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 30, std::bitset<64>(0b010101));
        theleds.get(embot::hw::LED::three).wave(&ledwave); 
    }
    
    return host_connected;
}

static void s_used_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    // this handler is called by object EOtheErrorManager which is the error handler used by the embOBJ framework
    // we can put in here what we want. In here we use emBOT:: calls
    
#if 1
    
    std::string timenow = embot::core::TimeFormatter(embot::core::now()).to_string();
    std::string eobjstr = (nullptr == caller) ? ("EO?") : ((nullptr == caller->eobjstr) ? ("EO?") : (std::string(caller->eobjstr)));
    std::string infostr = (nullptr == info) ? ("no extra info") : (std::string(info));
    std::string taskid = (nullptr == caller) ? ("0") : (std::to_string(caller->taskid));
    std::string errortype = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
    static volatile uint32_t counter = 0;
    
    if(eo_errortype_trace == errtype)
    {
        // if it is a simple trace we just return
        embot::core::print(std::string("[TRACE] (") + eobjstr + std::string(" @") + timenow + ") -> " + infostr);  
        return;            
    }
    else
    {
        // we print the error 
        embot::core::print(std::string("[eobj: ") + eobjstr + ", tsk: " + taskid + std::string("] ") + errortype + ": " + infostr);
        
        // and if it is NOT fatal we return
        if(errtype <= eo_errortype_error)
        {
            return;
        }

        // else we stay in here forever

        for(;;)
        {
            counter ++;
        }
        
    }
        

#else   
    // but we keep embOBJ style code. just for comparison
    const char empty[] = "EO?";
    const char *eobjstr = (NULL == caller) ? (empty) : ((NULL == caller->eobjstr) ? (empty) : (caller->eobjstr));
    const uint32_t taskid = (NULL == caller) ? (0) : (caller->taskid);
    
    char text[128];
    uint64_t tt = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    uint32_t sec = tt/(1000*1000);
    uint32_t tmp = tt%(1000*1000);
    uint32_t msec = tmp / 1000;
    uint32_t usec = tmp % 1000;    
    
    if(eo_errortype_trace == errtype)
    {   // it is a trace
        
        if(NULL != info)
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> %s.", eobjstr, sec, msec, usec, info); 
        }
        else
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> ...", eobjstr, sec, msec, usec); 
        }
        hal_trace_puts(text);
        return;            
    }    
    
    snprintf(text, sizeof(text), "[eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype), info);
    hal_trace_puts(text);

    if(errtype <= eo_errortype_error)
    {
        return;
    }

    for(;;);
#endif    
}

// thread tETHmon

// the ETH mon is a periodic thread 
embot::os::PeriodicThread *tETHmon {nullptr};

void tETHmon_startup(embot::os::Thread *t, void *param);
void tETHmon_onperiod(embot::os::Thread *t, void *param);
void t_ETHmon(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }
 
static void s_start_ETHmonitor_thread()
{        
    tETHmon = new embot::os::PeriodicThread;
    
    embot::os::PeriodicThread::Config tConfig { 
        6*1024, 
        embot::os::Priority::belownorm17, 
        tETHmon_startup,                // the startup function
        nullptr,                        // it param
        1000*embot::core::time1millisec, // the period
        tETHmon_onperiod,
        "tETHmon"
    };
    
    tETHmon->start(tConfig, t_ETHmon);      
}

//#define USE_KS

#if defined(USE_KS)
#include "embot_hw_chip_KSZ8563.h"
embot::hw::chip::KSZ8563 *eths {nullptr}; 
#else
#include "embot_hw_eth.h"
#endif

// code for the periodic thread tETHmon
void tETHmon_startup(embot::os::Thread *t, void *param)
{
    embot::core::print(std::string("calling tETHmon_startup() @ ") + embot::core::TimeFormatter(embot::core::now()).to_string());   

#if defined(USE_KS)   
    eths = new embot::hw::chip::KSZ8563;
    eths->init({});
#else
#endif        
    // add in here initialization code for the driver which asks the ETH switch
}


void tETHmon_onperiod(embot::os::Thread *t, void *param)
{
//    embot::core::print(std::string("calling tETHmon_onperiod() @ ") + embot::core::TimeFormatter(embot::core::now()).to_string());   

     // add in here code which asks the ETH switch about its status

    static bool prevlink1isup = false;    
    static bool prevlink2isup = false;
    
    bool link1isup = false;    
    bool link2isup = false;

#if defined(USE_KS)    
    embot::hw::chip::KSZ8563::Link lnk1 { embot::hw::chip::KSZ8563::Link::DOWN };
    embot::hw::chip::KSZ8563::Link lnk2 { embot::hw::chip::KSZ8563::Link::DOWN };
    eths->read(embot::hw::chip::KSZ8563::PHY::one, lnk1);
    eths->read(embot::hw::chip::KSZ8563::PHY::two, lnk2);
    
    link1isup = (embot::hw::chip::KSZ8563::Link::UP == lnk1) ? true : false;
    link2isup = (embot::hw::chip::KSZ8563::Link::UP == lnk2) ? true : false;
#else    
    link1isup = embot::hw::eth::islinkup(embot::hw::PHY::one);
    link2isup = embot::hw::eth::islinkup(embot::hw::PHY::two);
#endif
    
    if((prevlink1isup != link1isup) || ((prevlink2isup != link2isup)))
    {            
        std::string msg = std::string("ETH link 1 is ") + (link1isup ? "UP" : "DOWN") + " ETH link 2 is " + (link2isup ? "UP" : "DOWN");
        embot::core::print(msg + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 
    }
    prevlink1isup = link1isup;
    prevlink2isup = link2isup;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

