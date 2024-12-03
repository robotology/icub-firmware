/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_os.h"
#include "embot_os_rtos.h"
#include "embot_hw_sys.h"
#include "eEmemorymap.h"

void onIdle(embot::os::Thread *t, void* idleparam);
void initSystem(embot::os::Thread *t, void* initparam);

constexpr embot::os::InitThread::Config initcfg = { 6*1024, initSystem, nullptr };
constexpr embot::os::IdleThread::Config idlecfg = { 4*1024, nullptr, nullptr, onIdle };
constexpr embot::core::Callback onOSerror = { };
constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};
  
static_assert(0x08120000 == EENV_MEMMAP_EUPDATER_ROMADDR, "see EENV_MEMMAP_EUPDATER_ROMADDR");
constexpr uint32_t offset {EENV_MEMMAP_EUPDATER_ROMADDR};  

#include "embot_hw_dualcore.h"



constexpr embot::hw::dualcore::Config dcc {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate };


int main(void)
{
    // relocate the vector table  
    embot::hw::sys::relocatevectortable(offset);
    
    embot::hw::dualcore::config(dcc);
    // config and start system
    embot::os::init(osconfig); 
    embot::os::start();
}


// extra include

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

#include "embot_os_eom.h"

static void s_udpnode_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);

constexpr embot::os::EOM::Config eomcfg {{s_udpnode_errman_OnError}};

static void s_initialiser(void);

void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

void initSystem(embot::os::Thread *t, void* initparam)
{
    embot::core::print("eUpdater: this is me");    
    
    embot::os::theTimerManager::Config tcfg {};
    tcfg.stacksize = 8*1024;    
    embot::os::theTimerManager::getInstance().start(tcfg);     
    embot::os::theCallbackManager::getInstance().start({});  
    embot::os::EOM::initialise(eomcfg);
    
    // ok, now you can run whatever you want ...
    s_initialiser();        
                
    embot::core::print("eUpdater: quitting the INIT thread. Normal scheduling starts");    
}


static void s_eom_eupdater_main_init(void);

static void s_initialiser(void)
{
    s_eom_eupdater_main_init();
}



#include "EOVtheSystem.h" 

#include "hal.h"
#include "embot_hw.h"
#include "embot_hw_eeprom.h"
#include "embot_hw_led.h"
#include "embot_hw_flash_bsp.h"


#include "ipal.h"

// for the ipnet we need:
#include "EOMtheIPnet.h"

// for the ipal config we use (variable ipal_cfg2) we need:
#include "ipal_cfg2.h"



// embobj  
#include "EoCommon.h"
#include "EOVtheSystem.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EOMtheIPnet.h"

#include "EOaction.h"
#include "EOpacket.h"
#include "EOMmutex.h"
#include "EOtimer.h"
#include "EOsocketDatagram.h"

#if !defined(_MAINTAINER_APPL_)
#include "eupdater-info.h"
#include "eupdater_cangtw.h"
#else
#include "emaintainer-info.h"
#endif

#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"

#include "eEsharedServices.h"

#include "updater-core.h"
#include "eupdater_parser.h"


extern void task_ethcommand(void *p);


enum { capacityofUDPpacket = 1200 };

static const eOipv4addr_t hostipaddr = EO_COMMON_IPV4ADDR(10, 0, 1, 104);

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_eupdater_main_init(void);

static void s_ethcommand_startup(EOMtask *p, uint32_t t);
static void s_ethcommand_run(EOMtask *p, uint32_t t);


extern eObool_t eom_eupdater_main_connectsocket2host(eOipv4addr_t remaddr, EOsocketDatagram *skt, uint32_t usec);

#ifdef PARSE_ETH_ISR
static void s_verify_eth_isr(uint8_t* pkt_ptr, uint32_t size);
#endif    

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// task eth command

static EOsocketDatagram*        s_skt_ethcmd                = NULL;
static EOpacket*                s_rxpkt_ethcmd              = NULL;
static EOpacket*                s_txpkt_ethcmd              = NULL;
static EOMtask*                 s_task_ethcommand           = NULL;
static EOaction*                s_action_ethcmd             = NULL;

static const eOipv4port_t       s_ethcmd_port               = 3333; 
static const eOmessage_t        s_message_from_skt_ethcmd   = 0x00000001;

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

extern void task_ethcommand(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start((EOMtask*)p);
}  

static void s_udpnode_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    const char empty[] = "EO?";
    // const char *err = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
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
        embot::core::print(text);
        return;            
    }    
    
    snprintf(text, sizeof(text), "[eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype), info);
    embot::core::print(text);

    if(errtype <= eo_errortype_error)
    {
        return;
    }

    for(;;);
}


static uint8_t *ipaddr = NULL;

static void s_eom_eupdater_main_init(void)
{
    const ipal_cfg2_t* ipalcfg2 = NULL;
    eOmipnet_cfg_addr_t* eomipnet_addr;
#ifndef _USE_IPADDR_FROM_IPAL_CFG_
    const eEipnetwork_t *ipnet = NULL;
#endif    

    const eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_MyCfg = 
    { 
#if !defined(_MAINTAINER_APPL_)    
        .numberofsockets            = 2,    // one for eth command and one for can gateway
        .maxdatagramenqueuedintx    = 32    // can gatweay needs to be able to manage many small packets
#else
        .numberofsockets            = 2, 
        .maxdatagramenqueuedintx    = 2
#endif
    };

#if !defined(_MAINTAINER_APPL_)
    ipalcfg2 = &ipal_cfg2;
#else
    ipalcfg = emaintainer_ipal_cfg;
#endif    
    
    
    ipaddr = (uint8_t*)&(ipalcfg2->eth->eth_ip);
    
    
//#if !defined(_MAINTAINER_APPL_)     
//    updater_core_trace("MAIN", "starting eUpdater:");
//#else
//    updater_core_trace("MAIN", "starting eMaintainer");
//#endif  

    // eeprom is used for shared services but is initted also inside there
    //hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    embot::hw::eeprom::init(embot::hw::EEPROM::one, {});


#if !defined(_MAINTAINER_APPL_)    
    eo_armenv_Initialise((const eEmoduleInfo_t*)&eupdater_modinfo_extended, NULL);

    // the info about application00 are saved in ROM
    const eEmoduleExtendedInfo_t * eapp_modinfo_extended = (const eEmoduleExtendedInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
    
    // the info about application01 are saved in a flash sector of other bank
    const embot::hw::flash::Partition& app01 { embot::hw::sys::partition(embot::hw::flash::Partition::ID::eapplication01) };
    const eEmoduleExtendedInfo_t * eappcm4_modinfo_extended = (const eEmoduleExtendedInfo_t*)(app01.address + EENV_MODULEINFO_OFFSET);

    // read application00 (core cm7 bank0)
    if((eapp_modinfo_extended->moduleinfo.info.entity.type == ee_procApplication) &&
       (eapp_modinfo_extended->moduleinfo.info.rom.addr == EENV_MEMMAP_EAPPLICATION_ROMADDR)
      )
    {
        ee_sharserv_part_proc_synchronise(ee_procApplication,(const eEmoduleInfo_t*)eapp_modinfo_extended);
    }

    // read application01 (core cm4 bank1)
    if((eappcm4_modinfo_extended->moduleinfo.info.entity.type == ee_procApplication) &&
       (eappcm4_modinfo_extended->moduleinfo.info.rom.addr == app01.address)               
      )
    {
        ee_sharserv_part_proc_synchronise(ee_procOther01, (const eEmoduleInfo_t*)eappcm4_modinfo_extended);
    }
    
#else
    eo_armenv_Initialise((const eEmoduleInfo_t*)&emaintainer_modinfo_extended, NULL);
#endif    
    eov_env_SharedData_Synchronise(eo_armenv_GetHandle());
  
    
#ifndef _USE_IPADDR_FROM_IPAL_CFG_
    if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
    {
        eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout

        ipaddr = (uint8_t*)&(eomipnet_addr->ipaddr);
    }
    else
#endif        
    {
        eomipnet_addr = NULL;
        ipaddr = (uint8_t*)&(ipalcfg2->eth->eth_ip);
    }
 
#ifdef PARSE_ETH_ISR   
    extern void (*hal_eth_lowLevelUsePacket_ptr)(uint8_t* pkt_ptr, uint32_t size);    
    hal_eth_lowLevelUsePacket_ptr = s_verify_eth_isr;
#endif

//    // start the ipnet
//    updater_core_trace("MAIN", "starting ::ipnet with IP addr: %d.%d.%d.%d\n\r", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]); 

    constexpr eOmipnet_cfg_t eom_ipnet_Cfg = 
    { 
        .procpriority = embot::core::tointegral(embot::os::Priority::high44), // default = 220 .. but we must give a different value
        .procstacksize = 4*1024, // default = 1024 .. 
        .procmaxidletime = 25*embot::core::time1millisec, 
        .procwakeuponrxframe = eobool_true, 
        .tickpriority = embot::core::tointegral(embot::os::Priority::high43), // default = 219 .. but we must give a different value
        .tickstacksize = 4*1024 // default = 128 .. but that does not allow to run prints or else
    };
    ipal_cfg_any_t * ipal_cfg2use = &ipal_cfg2;
    
    eom_ipnet_Initialise(&eom_ipnet_Cfg, //&eom_ipnet_DefaultCfg,
                         ipal_cfg2use, 
                         eomipnet_addr,
                         &eom_ipnet_dtgskt_MyCfg
                         );
    
    // init the leds    
    embot::hw::led::init(embot::hw::LED::one);
    embot::hw::led::init(embot::hw::LED::two);
    embot::hw::led::init(embot::hw::LED::three);
//    embot::hw::led::init(embot::hw::LED::four);
//    embot::hw::led::init(embot::hw::LED::five);
//    embot::hw::led::init(embot::hw::LED::six);
    
    

    // the eth command task 
//    updater_core_trace("MAIN", "starting ::taskethcommand");                       
    //#warning MARCOACCAME: non va bene 101, uso 25
    s_task_ethcommand = eom_task_New(eom_mtask_MessageDriven, 25, 6*1024, s_ethcommand_startup, s_ethcommand_run,  16, 
                                    eok_reltimeINFINITE, NULL, 
                                    task_ethcommand, "ethcommand");
    s_task_ethcommand = s_task_ethcommand;
    
    updater_core_init();
    
#if !defined(_MAINTAINER_APPL_)
    // there is also the can gateway
//    updater_core_trace("MAIN", "starting ::cangateway");    
    eupdater_cangtw_init();
#endif

    // eval if and when jumping
//    updater_core_trace("MAIN", "calling ::evalwhenjumping");
    eupdater_parser_evalwhenjumping(); 
}


static void s_ethcommand_startup(EOMtask *p, uint32_t t)
{
    // init the rx and tx packets 
    s_rxpkt_ethcmd = eo_packet_New(capacityofUDPpacket);  
    s_txpkt_ethcmd = eo_packet_New(capacityofUDPpacket);

    // init the action used for various tasks
    s_action_ethcmd = eo_action_New();  

    // initialise the socket 
    s_skt_ethcmd = eo_socketdtg_New(  2, capacityofUDPpacket, eom_mutex_New(), // input queue
                                      2, capacityofUDPpacket, eom_mutex_New()  // output queue
                                   );   

//    updater_core_trace("MAIN", "opening a txrx socket on port %d for eth messages\n\r", s_ethcmd_port);


    // set the rx action on socket to be a message s_message_from_skt_ethcmd to this task object
    eo_action_SetMessage(s_action_ethcmd, s_message_from_skt_ethcmd, p);
    eo_socketdtg_Open(s_skt_ethcmd, s_ethcmd_port, eo_sktdir_TXRX, eobool_false, NULL, s_action_ethcmd, NULL);

#if !defined(_MAINTAINER_APPL_)
    // before we go on and start connection to server, we wait the task of cangateway is started and has initted its socket.
    // if we dont wait, then the init of the socket will be stopped until arp connection to host is achieved. 
    eupdater_cangtw_block_until_startup();
#endif

    eom_eupdater_main_connectsocket2host(hostipaddr, s_skt_ethcmd, 3000*eok_reltime1ms);    
}

static void s_ethcommand_run(EOMtask *p, uint32_t t)
{
    // read the packet.
    eOresult_t res;
    EOsocketDatagram *socket = NULL;
    eObool_t (*parser)(EOpacket*, EOpacket *) = NULL;

    // the message that we have received
    eOmessage_t msg = (eOmessage_t)t;

    switch(msg)
    {
        case s_message_from_skt_ethcmd:
        {
            socket = s_skt_ethcmd;
            parser = eupdater_parser_process_ethcmd;
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


    //hal_led_toggle(hal_led1);

    res = eo_socketdtg_Get(socket, s_rxpkt_ethcmd, eok_reltimeINFINITE);
    

    if(eores_OK == res)
    {
        eOipv4addr_t remaddr = 0;
        eOipv4port_t remport = 0;
        eo_packet_Addressing_Get(s_rxpkt_ethcmd, &remaddr, &remport);   

//        if(remaddr != hostipaddr)
//        {   // dont want anybody else but the host (pc104)
//            return;
//        }
        
        if(eobool_true == parser(s_rxpkt_ethcmd, s_txpkt_ethcmd))
        {   // if in here we have just stopped the countdown inside the parser
            
            // transmit a pkt back to the host. the call blocks until success or timeout expiry
            if(eobool_true == eom_eupdater_main_connectsocket2host(remaddr, socket, 1000*eok_reltime1ms))
            {
                eo_socketdtg_Put(socket, s_txpkt_ethcmd);
            }
        }
        else
        {   
            // this attempt is still-blocking, but it blocks for less time
            eom_eupdater_main_connectsocket2host(remaddr, socket, 250*eok_reltime1ms);
        } 
    }

}

static eObool_t     host_connected = eobool_false;
static eOipv4addr_t host_ipaddress = 0;

// blocking
extern eObool_t eom_eupdater_main_connectsocket2host(eOipv4addr_t remaddr, EOsocketDatagram *skt, uint32_t usec)
{
    //eOipv4addr_t remaddr = 0;
    //eOipv4port_t remport = 0;

    // print stats of rx packet
    //eo_packet_Destination_Get(rxpkt, &remaddr, &remport);
    
    if((eobool_false == host_connected) || (remaddr != host_ipaddress))
    {
        host_ipaddress = remaddr;
        host_connected = eobool_false;
        
//        uint32_t waitime = 100*ipaddr[3];
//        hal_sys_delay(waitime);
        
        // it is blocking
        if(eores_OK == eo_socketdtg_Connect(skt, remaddr, usec)) 
        {
            host_connected = eobool_true;
        }
        else
        {
            host_connected = eobool_false;
            //printf("not connecetd after %d ms. i shall try again at next reception\n\r", eok_reltime1sec/1000);
        }
    }
    
    if(eobool_true == host_connected)
    {
        embot::hw::led::on(embot::hw::LED::one);
    }

    return(host_connected);
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

