/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "EoCommon.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOtimer.h"
#include "EOlist.h"

#include "EOVtheIPnet.h"
#include "EOVtheIPnet_hid.h"
#include "EOsocketDatagram_hid.h"
#include "EOVtask.h"
#include "EOMmutex.h"
#include "EOMtask_hid.h"

#include "EOpacket_hid.h"
#include "EOsocket_hid.h"

#include "osal.h"
#include "ipal.h"
#include "hal_sys.h"


#define ARP_NEWMODE
//#define TEST_ARP
#undef TEST_ARP_EVIEW
#undef TEST_ARP_PRINT

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheIPnet.h"


#if defined(TEST_ARP_EVIEW)

#include "eventviewer.h"

void fn_mark1(void){}
void fn_mark2(void){}
void fn_ipnetarp(void){}
static evEntityId_t ev_mark1 = ev_ID_first_usrdef+1;    
static evEntityId_t ev_mark2 = ev_ID_first_usrdef+2;   
static evEntityId_t ev_ipnetarp = ev_ID_first_usrdef+3;

evEntityId_t evprev_mark1;
evEntityId_t evprev_mark2;
evEntityId_t evprev_ipnetarp;
   
#endif


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheIPnet_hid.h" 
#include "EOMmutex_hid.h" 
//#include "OPCprotocolManager_Cfg.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#undef SLOWERMODEONRXDATAGRAM
//#define SLOWERMODEONRXDATAGRAM

#define _MAXTOKENS_SEM_CMD_  2  // but as it is used as a binary semaphore, then 1 would be enough 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

eOmipnet_diagnosticsinfo_t eom_ipnet_diagnosticsInfo  =
{
    .datagrams_failed_to_go_in_rxfifo                       = 0,
    .datagrams_failed_to_go_in_txosalqueue                  = 0,
    .datagrams_failed_to_be_retrieved_from_txfifo           = 0,
    .datagrams_failed_to_be_sent_by_ipal                    = 0    
};


const eOmipnet_cfg_t eom_ipnet_DefaultCfg = 
{ 
    .procpriority               = 220, 
    .procstacksize              = 1024, 
    .procmaxidletime            = 20000, 
    .procwakeuponrxframe        = eobool_true, 
    .tickpriority               = 219, 
    .tickstacksize              = 128
};

const eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_DefaultCfg = 
{   
    .numberofsockets            = 2, 
    .maxdatagramenqueuedintx    = 8
};
 
const eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_NOsocketCfg = 
{
    .numberofsockets            = 0, 
    .maxdatagramenqueuedintx    = 0
};

const eOmipnet_cfg_addr_t eom_ipnet_addr_DefaultCfg = 
{
    .macaddr                    = 0,
    .ipaddr                     = 0,
    .ipmask                     = 0
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

#undef START_IPAL_INSIDE_eom_ipnet_Initialise


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_ipnet_ipal_start(void);

static void e_eom_ipnet_signal_new_frame_is_available(void);

static void s_eom_ipnet_tsktick_forever(EOMtask *rt, uint32_t n);

static void s_eom_ipnet_tskproc_startup(EOMtask *rt, uint32_t n);

static void s_eom_ipnet_tskproc_forever(EOMtask *rt, uint32_t msg);

static eOresult_t s_eom_ipnet_ARP(EOVtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout);
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
static eOresult_t s_eom_ipnet_non_blocking_ARP(EOVtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout);
#endif
static eOresult_t s_eom_ipnet_AttachSocket(EOVtheIPnet* ip, EOsocketDerived *s);
static eOresult_t s_eom_ipnet_DetachSocket(EOVtheIPnet* ip, EOsocketDerived *s);
static eOresult_t s_eom_ipnet_Alert(EOVtheIPnet* ip, void *eobjcaller, eOevent_t evt);
static eOresult_t s_eom_ipnet_WaitPacket(EOVtheIPnet* ip, EOsocketDerived *s, eOreltime_t tout);

static void s_eom_ipnet_OnReceptionDatagram(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t por);



static void s_eom_ipnet_process_command(void);
static void s_eom_ipnet_stop_command(void);
static void s_eom_ipnet_repeat_command(void);

static void s_eom_ipnet_process_transmission_datagram(void);


static eObool_t s_eom_ipnet_attach_rqst_dtgsocket(EOVtheIPnet *vip, EOsocketDatagram *dgmskt);
static void s_eom_ipnet_attach_proc_dtgsocket(EOsocketDatagram *dtgs);
static eObool_t s_eom_ipnet_detach_rqst_dtgsocket(EOVtheIPnet *vip, EOsocketDatagram *dgmskt);
static void s_eom_ipnet_detach_proc_dtgsocket(EOsocketDatagram *dtgs);

#ifdef SLOWERMODEONRXDATAGRAM
static eOresult_t s_eom_ipnet_DatagramSocketHas(void *item, void *param);
#endif




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
 
static const char s_eobj_ownname[] = "EOMtheIPnet";

static EOMtheIPnet s_eom_theipnet = 
{
    .ipnet                  = NULL,                               // ipnet
    .tskproc                = NULL,                               // tskproc
    .tsktick                = NULL,                               // tsktick
    .cmd                    =
                            {
                                .mtxcaller      = NULL, 
                                .opcode         = cmdDoNONE, 
                                .repeatcmd      = 0,
                                .result         = 0,
                                .par16b         = 0,
                                .par32b         = 0,
                                .par32x         = 0,
                                .par64x         = 0,
                                .tout           = 0, 
                                .blockingsemaphore = NULL, 
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
                                .busysemaphore  = NULL,
#endif
                                .stoptmr        = NULL,
                                .stopact        = NULL
                            },   // cmd
    .rxpacket               = NULL,                                
    .maxwaittime            = 0,                                  // maxwaittime 
    .dgramsocketready2tx    = NULL,                               // dgramsocketready2tx  
//    .ipcfg                  = {0},                                // ipcfg
    .taskwakeuponrxframe    = eobool_false
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOMtheIPnet * eom_ipnet_Initialise(const eOmipnet_cfg_t *ipnetcfg,
                                          const ipal_cfg_t *ipcfg, 
                                          const eOmipnet_cfg_addr_t *addrcfg,
                                          const eOmipnet_cfg_dtgskt_t *dtgskcfg 
                                         ) 
{
    if(NULL != s_eom_theipnet.ipnet) 
    {
        // already initialised
        return(&s_eom_theipnet);
    }
    

    if(NULL == ipnetcfg)
    {
        ipnetcfg = &eom_ipnet_DefaultCfg;
    }
   
    // trying to initialise with no ipcfg ?? error
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != ipcfg), "eom_ipnet_Initialise(): NULL ipcfg", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);   

    // trying to initialise with wrong params error
    eo_errman_Assert(eo_errman_GetHandle(), (0 != ipnetcfg->procmaxidletime), "eom_ipnet_Initialise(): 0 procmaxidletime", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    eo_errman_Assert(eo_errman_GetHandle(), (0 != ipnetcfg->procstacksize), "eom_ipnet_Initialise(): 0 procstacksize", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    eo_errman_Assert(eo_errman_GetHandle(), (0 != ipnetcfg->procpriority), "eom_ipnet_Initialise(): procpriority", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    
    
        
    // we can initialise w/out datagram sockets. we do so by using dtgskcfg  which contains zero and zero
    if(NULL == dtgskcfg)
    {
        dtgskcfg = &eom_ipnet_dtgskt_DefaultCfg;
    }

    // cannot have maxdatagramenqueuedintx equal to zero if we have non-zero numberofsockets
    eo_errman_Assert(eo_errman_GetHandle(), 
                     (0 != dtgskcfg->maxdatagramenqueuedintx) || (0 == dtgskcfg->numberofsockets), 
                     "eom_ipnet_Initialise(): 0 maxdatagramenqueuedintx",
                     s_eobj_ownname, &eo_errman_DescrWrongParamLocal); 


   
    
//    #warning the mutex mutexactivedgram is used to protect multiple access to the activedgramlist.
//    #warning but it may be not needed. the caller is unique (protected by another mutex) and the ipnet use the list only when the caller is waiting for its action
    // i get a basic ip net with proper osal mutex and attach and detach functions which are proper for rtos
    s_eom_theipnet.ipnet = eov_ipnet_hid_Initialise(dtgskcfg->numberofsockets, NULL, //eom_mutex_New(), 
                                                    s_eom_ipnet_AttachSocket, 
                                                    s_eom_ipnet_DetachSocket, 
                                                    s_eom_ipnet_Alert,
                                                    s_eom_ipnet_ARP,
                                                    s_eom_ipnet_WaitPacket
                                                    //s_eom_ipnet_GetTask // removed as it is not necessary anymore
                                                    );

                                             
    // i get cmd.blockingsemaphore, initted with zero tokens
    s_eom_theipnet.cmd.blockingsemaphore = osal_semaphore_new(_MAXTOKENS_SEM_CMD_, 0);
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_eom_theipnet.cmd.blockingsemaphore), "eom_ipnet_Initialise(): osal cant give blockingsemaphore", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);

#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
    s_eom_theipnet.cmd.busysemaphore = osal_semaphore_new(_MAXTOKENS_SEM_CMD_, 1);
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_eom_theipnet.cmd.busysemaphore),  "eom_ipnet_Initialise(): osal cant give busysemaphore", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
#endif
    
    s_eom_theipnet.cmd.mtxcaller = osal_mutex_new();
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_eom_theipnet.cmd.mtxcaller), "eom_ipnet_Initialise(): osal cant give mtxcaller", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);

    s_eom_theipnet.cmd.stoptmr = eo_timer_New();
    s_eom_theipnet.cmd.stopact = eo_action_New();


    // i get a messagequeue which will keep addresses of datagramsockets
    if(0 != dtgskcfg->maxdatagramenqueuedintx)
    {
        s_eom_theipnet.dgramsocketready2tx = osal_messagequeue_new(dtgskcfg->maxdatagramenqueuedintx);
        eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_eom_theipnet.dgramsocketready2tx), "eom_ipnet_Initialise(): osal cant give a dgramsocketready2tx", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
    }
    else
    {
        s_eom_theipnet.dgramsocketready2tx = NULL;
    }

    // i init the ipconfig
    memcpy((uint8_t*)&s_eom_theipnet.ipcfg, (uint8_t*)ipcfg, sizeof(ipal_cfg_t)); 
     
    // change s_eom_theipnet.ipcfg to reflect changes passed as parameters, such as macaddr, ipaddr, ipmask
    if(NULL != addrcfg)
    {
        if(0x0000000000000000 != addrcfg->macaddr)
        {
            s_eom_theipnet.ipcfg.eth_mac = addrcfg->macaddr;
        }
        
        if(0x00000000 != addrcfg->ipaddr)
        {
            s_eom_theipnet.ipcfg.eth_ip = addrcfg->ipaddr;       
        }    
    
        if(0x00000000 != addrcfg->ipmask)
        {
            s_eom_theipnet.ipcfg.eth_mask = addrcfg->ipmask;
        } 
    }
    
    
    // complete other things    
    s_eom_theipnet.rxpacket     = eo_packet_New(0); // zero capacity .... because we want to assign data later on
    s_eom_theipnet.maxwaittime  = eok_reltimeINFINITE; //(eok_reltimeINFINITE == maxidle) ? (eok_reltimeINFINITE) : (4*maxidle); 

    
    s_eom_theipnet.taskwakeuponrxframe = ipnetcfg->procwakeuponrxframe;
    
    
    eom_ipnet_Activate(&s_eom_theipnet);


    // and finally, i prepare the task able to process the ip net
    s_eom_theipnet.tskproc = eom_task_New(eom_mtask_EventDriven, ipnetcfg->procpriority, ipnetcfg->procstacksize,
                                          s_eom_ipnet_tskproc_startup, s_eom_ipnet_tskproc_forever,
                                          0, ipnetcfg->procmaxidletime,
                                          NULL,
                                          eom_ipnetproc, 
                                          "ipnet.proc");

#define IPNET_TICK_PERIODIC                                          
#if defined(IPNET_TICK_PERIODIC) 
    // and task which ticks the timers. it is a periodic task
    s_eom_theipnet.tsktick = eom_task_New(eom_mtask_Periodic, ipnetcfg->tickpriority, ipnetcfg->tickstacksize,
                                          NULL, s_eom_ipnet_tsktick_forever,
                                          0, s_eom_theipnet.ipcfg.sys_timetick,
                                          NULL, 
                                          eom_ipnettick,
                                          "ipnet.tick");
#else
    // and task which ticks the timers. it is an event based task with timeout. 
    // this solution is  not really periodic, but it can be good enough because we just need to increment some delays for tcp/ip retransmission.
    // we use this solution in the particular case of the object EOMtheEMSrunner with hw timer (but not with osaltimer) to avoid the ipnettick to delay start of rx task
    s_eom_theipnet.tsktick = eom_task_New(eom_mtask_EventDriven, ipnetcfg->tickpriority, ipnetcfg->tickstacksize,
                                          NULL, s_eom_ipnet_tsktick_forever,
                                          0, s_eom_theipnet.ipcfg.sys_timetick,
                                          NULL, 
                                          eom_ipnettick,
                                          "ipnet.tick");
#endif



#if     defined(START_IPAL_INSIDE_eom_ipnet_Initialise)
    s_eom_ipnet_ipal_start();
#endif

#if defined(TEST_ARP_EVIEW)
    eventviewer_load(ev_mark1, fn_mark1);
    eventviewer_load(ev_mark2, fn_mark2);
    eventviewer_load(ev_ipnetarp, fn_ipnetarp);
#endif                                          
    return(&s_eom_theipnet);
}    

    
extern EOMtheIPnet* eom_ipnet_GetHandle(void) 
{
    if(NULL == s_eom_theipnet.ipnet) 
    {
        return(NULL);
    }
    
    return(&s_eom_theipnet);
}


extern eOipv4addr_t eom_ipnet_GetIPaddress(EOMtheIPnet *ip)
{
    if(NULL == ip)
    {
        return(0);
    }
    
    return(s_eom_theipnet.ipcfg.eth_ip);
}

extern eOresult_t eom_ipnet_Activate(EOMtheIPnet *ip)
{
    if(NULL == ip)
    {
        return(eores_NOK_nullpointer);
    }
    
    return(eov_ipnet_Activate(s_eom_theipnet.ipnet));
}

extern eOresult_t eom_ipnet_Deactivate(EOMtheIPnet *ip)
{
    if(NULL == ip)
    {
        return(eores_NOK_nullpointer);
    }
    
    return(eov_ipnet_Deactivate(s_eom_theipnet.ipnet));
}

#if defined(TEST_ARP)
#include "hal.h"
#include "osal.h"
#include "stdio.h"

#include "EOtheLEDpulser.h"

    static uint64_t t0 =0;
    static uint64_t t1 =0;
#endif


#if defined(TEST_ARP) 

extern eOresult_t eom_ipnet_ResolveIP_TEST(EOMtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout)
{
    static uint8_t alreadycalled = 0;
    if(NULL == ip)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(1 == alreadycalled)
    {
        return(eores_OK);
    }
    
    alreadycalled = 1;
    
    // blink a led at 1 hz

    // init the ledpulser with leds 0 and 1 and 2 and 3
    
    eOledpulser_cfg_t ledpulsercfg = 
    {
        .led_enable_mask    = (1 << eo_ledpulser_led_zero)      | 
                              (1 << eo_ledpulser_led_one)       | 
                              (1 << eo_ledpulser_led_two)       |
                              (1 << eo_ledpulser_led_three)     |
                              (1 << eo_ledpulser_led_four)      |
                              (1 << eo_ledpulser_led_five),
        .led_init           = (eOint8_fp_uint8_cvoidp_t)hal_led_init,
        .led_on             = (eOint8_fp_uint8_t)hal_led_on,
        .led_off            = (eOint8_fp_uint8_t)hal_led_off,
        .led_toggle         = (eOint8_fp_uint8_t)hal_led_toggle
    };
    
    // each active led will have at timer. we can set it on, off, pulse. 
    eo_ledpulser_Initialise(&ledpulsercfg);  
        
    // start a pulse on led0: infinite with 1 Hz period
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_five, 1*eok_reltime1sec, 0);       
 
    //tout *= 4;
    tout = 40*1000*1000;
    
    eOresult_t r = eores_OK;
#if defined(TEST_ARP_PRINT)    
    char str[64];
#endif
    uint64_t tt = 0;
    uint8_t mac[6] = {0};
    uint8_t *ad = (uint8_t*)&ipaddr;
    
    //osal_task_wait(10000000);
    
    //osal_task_wait(10*eok_reltime1ms);
    
    //eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_five); //, 2*eok_reltime1sec, 0);
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_five, 100*eok_reltime1ms, 0);
    

    
//    if(ipal_res_OK == ipal_arp_iscached(ipaddr, mac))
//    {
//        snprintf(str, sizeof(str), "about IP %d.%d.%d.%d: we already have mac = %x:%x:%x:%x:%x:%x", 
//                                    ad[0], ad[1], ad[2], ad[3],
//                                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);    
//        hal_trace_puts(str);        
//    }
    
    //t0 = osal_system_abstime_get();
    //osal_task_wait(1*1000*1000);

#if defined(TEST_ARP_EVIEW)
    evprev_mark1 = eventviewer_switch_to(ev_mark1);
#endif    
    t0 = osal_system_abstime_get();    
    r = s_eom_ipnet_ARP(ip->ipnet, ipaddr, tout);
    t1 = osal_system_abstime_get();
#if defined(TEST_ARP_EVIEW)
    eventviewer_switch_to(evprev_mark1);
#endif    
    tt = t1 - t0;
    uint32_t s = tt/1000000;
    uint32_t m = tt/1000;
    m %= 1000;
    uint32_t u = tt%1000;
    uint32_t usec = (uint32_t)tt;

#if defined(TEST_ARP_PRINT)
    snprintf(str, sizeof(str), "doing ARP to %d.%d.%d.%d: res = %d in %d.%.3d sec + %.3d usec (usec = %d)", 
                               ad[0], ad[1], ad[2], ad[3],
                                r, s, m, u, usec);    
    hal_trace_puts(str);
#endif    
    
    // now switch leds one to four in on/off
    if(eores_OK != r)
    {   // if ko then they are all off and i pulse the number 
        eo_ledpulser_Off(eo_ledpulser_GetHandle(), eo_ledpulser_led_five); 
        eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_five, 500*eok_reltime1ms, 0);            
    }
    else
    {
        eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_five, 2000*eok_reltime1ms, 0);
        
        uint32_t ss = 1000*s+m;
        ss /= 1000;
        if(ss>=31)
        {
            eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
            eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_one);
            eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_two);
            eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_three);
            eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_four);
        }
        else
        {
        
            if((ss&0x1))
            {
                eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_four);
            }
            if((ss&0x2))
            {
                eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
            }
            if((ss&0x4))
            {
                eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_one);
            } 
            if((ss&0x8))
            {
                eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_two);
            }  
            if((ss&0x10))
            {
                eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_three);
            }             
        }        
        
        
    }
    
    return(r);

}

#endif


extern eOresult_t eom_ipnet_ResolveIP(EOMtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout)
{
    if(NULL == ip)
    {
        return(eores_NOK_nullpointer);
    }
#if defined(TEST_ARP)
    eom_ipnet_ResolveIP_TEST(ip, ipaddr, tout);
#else
    return(s_eom_ipnet_ARP(ip->ipnet, ipaddr, tout));
#endif    
}

#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
extern eOresult_t eom_ipnet_ResolveIPnonblocking(EOMtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout)
{
    if(NULL == ip)
    {
        return(eores_NOK_nullpointer);
    }

    return(s_eom_ipnet_non_blocking_ARP(ip->ipnet, ipaddr, tout));
}
#endif


extern eOresult_t eom_ipnet_IGMPgroupJoin(EOMtheIPnet *ip, eOipv4addr_t igmp)
{
    if(NULL == ip)
    {
        return(eores_NOK_nullpointer);
    }

    return(eov_ipnet_IGMPgroupJoin(ip->ipnet, igmp));
}


extern eOresult_t eom_ipnet_IGMPgroupLeave(EOMtheIPnet *ip, eOipv4addr_t igmp)
{
    if(NULL == ip)
    {
        return(eores_NOK_nullpointer);
    }

    return(eov_ipnet_IGMPgroupLeave(ip->ipnet, igmp));
}

extern EOMtask* eom_ipnet_GetTask(EOMtheIPnet *ip, eOmipnet_taskid_t tskid)
{
    if(NULL == ip)
    {
        return(NULL);
    }
    return((eomipnet_task_proc == tskid) ? (ip->tskproc) : (ip->tsktick));
}

extern eOresult_t eom_ipnet_GetDiagnosticsInfo(EOMtheIPnet *ip, eOmipnet_diagnosticsinfo_t *dgn_ptr)
{
    if((NULL == ip) || (NULL == dgn_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    memcpy(dgn_ptr, &eom_ipnet_diagnosticsInfo, sizeof(eOmipnet_diagnosticsinfo_t));
    
    return(eores_OK);
}


extern  eOmipnet_diagnosticsinfo_t * eom_ipnet_GetDiagnosticsInfoHandle(EOMtheIPnet *ip)
{
    if(NULL == ip)
    {
        return(NULL);
    }
    return(&eom_ipnet_diagnosticsInfo);

}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

/*  @brief      called by an external task, via eom_ipnet_ResolveIP(), to resolve IP address
    @details    it sends a command to the tskproc, which sends several ARP requests until
                it receives a reply or until a maximum number of requests are sent or a maximum timeout
                is reached.
 **/
static eOresult_t s_eom_ipnet_ARP(EOVtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout)
{
    eOresult_t res = eores_NOK_generic;

    static const uint16_t maxarptx = 65000;
    
    if(NULL == ip) 
    {
        return(eores_NOK_nullpointer);    
    } 

    // verify we dont have a zero ip address
    if(0x00000000 == ipaddr)
    {
        return(eores_NOK_generic);
    } 

    // - block other possible tasks from sending a command to the ipnet and also to execute this function from here onward
    osal_mutex_take(s_eom_theipnet.cmd.mtxcaller, osal_reltimeINFINITE);

#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)   
    if(osal_res_OK != osal_semaphore_decrement(s_eom_theipnet.cmd.busysemaphore, osal_reltimeINFINITE))
    {   // the semaphore could not be decremented because someone else before has decremented it: quit
        osal_mutex_release(s_eom_theipnet.cmd.mtxcaller);
        return(eores_NOK_generic);
    }    
#endif
    
    // - build the command to be sent to the proc task

    // copy the wanted command: doarp
    s_eom_theipnet.cmd.par16b = maxarptx;
    s_eom_theipnet.cmd.par32b = ipaddr;
    s_eom_theipnet.cmd.par32x = 100*1000;   // max frequency of tx of arp. the ipnet is called at every timeout or asynchronously at pkt rx or tx request
    s_eom_theipnet.cmd.par64x = 0;          // time of last arp pkt sent
    s_eom_theipnet.cmd.result  = 0;
    s_eom_theipnet.cmd.tout  = tout;
    s_eom_theipnet.cmd.opcode = cmdDoARP;

 
    // - tell the task that there is a command to process, so that a switch context is forced.
    s_eom_ipnet_Alert(s_eom_theipnet.ipnet, &s_eom_theipnet, eov_ipnet_evt_CMD2process);

    // - now the calling task is placed in hold until this semaphore is incremented  
    // - by the ipnet task when it has finished executing the command or its time has expired
    osal_semaphore_decrement(s_eom_theipnet.cmd.blockingsemaphore, osal_reltimeINFINITE);
    

    // - verify operation success
    if(1 == s_eom_theipnet.cmd.result)
    {
        res = eores_OK;
    }
    else
    {
        res = eores_NOK_timeout;
    }

    // - enable again another task to send a command to the ipnet
    osal_mutex_release(s_eom_theipnet.cmd.mtxcaller);

    return(res);
}

#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
static eOresult_t s_eom_ipnet_non_blocking_ARP(EOVtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout)
{
    eOresult_t res = eores_NOK_generic;

    static const uint16_t maxarptx = 65000;
    
    if(NULL == ip) 
    {
        return(eores_NOK_nullpointer);    
    } 

    // verify we dont have a zero ip address
    if(0x00000000 == ipaddr)
    {
        return(eores_NOK_generic);
    } 

    // - block other possible tasks from sending a command to the ipnet and also to execute this function from here onward
    osal_mutex_take(s_eom_theipnet.cmd.mtxcaller, osal_reltimeINFINITE);
    
    // if in here, then it means that: no other task has asked arping. but the same task may have asked a arp not yet expired
    
    // i understand it by attempting to get the busysemaphore w/ 0 timeout
    if(osal_res_OK != osal_semaphore_decrement(s_eom_theipnet.cmd.busysemaphore, osal_reltimeZERO))
    {   // the semaphore could not be decremented beacuse someone else before has decremented it: quit
        osal_mutex_release(s_eom_theipnet.cmd.mtxcaller);
        return(eores_NOK_generic);
    }
//    else
//    {   // nobody has taken the semaphore, thus we can go on
//        // in order to respect the same operations as in blocking mode, ... we increment
//        osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect);
//    }
    

    // - build the command to be sent to the proc task

    // copy the wanted command: doarp
    s_eom_theipnet.cmd.par16b = maxarptx;
    s_eom_theipnet.cmd.par32b = ipaddr;
    s_eom_theipnet.cmd.par32x = 100*1000;   // max frequency of tx of arp. the ipnet is called at evry timeout or asynchronously at pkt rx or tx request
    s_eom_theipnet.cmd.par64x = 0; // time of last arp pkt sent
    s_eom_theipnet.cmd.result  = 0;
    s_eom_theipnet.cmd.tout  = tout;
    s_eom_theipnet.cmd.opcode = cmdDoARP;

 
    // - tell the task that there is a command to process, so that a switch context is forced.
    s_eom_ipnet_Alert(s_eom_theipnet.ipnet, &s_eom_theipnet, eov_ipnet_evt_CMD2process);

#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)    
    // - now the calling task is placed in hold until this semaphore is incremented  
    // - by the ipnet task when it has finished executing the command or its time has expired
    osal_semaphore_decrement(s_eom_theipnet.cmd.blockingsemaphore, osal_reltimeINFINITE);

    // - verify operation success
    if(1 == s_eom_theipnet.cmd.result)
    {
        res = eores_OK;
    }
    else
    {
        res = eores_NOK_timeout;
    }

#endif    
    // - enable again another task to send a command to the ipnet
    osal_mutex_release(s_eom_theipnet.cmd.mtxcaller);

    return(eores_OK);
}
#endif

/*  @brief      called by an external task, via some methods of a socket, to add a socket to the management of the ipnet
    @details    it sends a command to the tskproc, which adds the sockets and then unblocks the caller.
 **/
static eOresult_t s_eom_ipnet_AttachSocket(EOVtheIPnet* ip, EOsocketDerived *s) 
{
    eOresult_t res = eores_NOK_generic;
    eObool_t canattach = eobool_false;;
    eOsocketType_t t;
    
    if((NULL == ip) || (NULL == s)) 
    {
        return(eores_NOK_nullpointer);    
    }

    t = eo_socket_hid_derived_Get_Type(s);
    
    // for now i only do datagrams  ...
    eo_errman_Assert(eo_errman_GetHandle(), (eo_skttyp_datagram == t), "s_eom_ipnet_AttachSocket(): only datagram socket", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);


    // - block other possible tasks from sending a command to the ipnet and also to execute this function from here onward
    osal_mutex_take(s_eom_theipnet.cmd.mtxcaller, osal_reltimeINFINITE);
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
    if(osal_res_OK != osal_semaphore_decrement(s_eom_theipnet.cmd.busysemaphore, osal_reltimeINFINITE))
    {   // the semaphore could not be decremented because someone else before has decremented it: quit
        osal_mutex_release(s_eom_theipnet.cmd.mtxcaller);
        return(eores_NOK_generic);
    }    
#endif
    // - fill the command and also verify if the socket can be attached
    canattach = s_eom_ipnet_attach_rqst_dtgsocket(ip, (EOsocketDatagram*)s);
 

    // - if the socket can be attached, send a command to ipnet main task and wait for the end of command
    if(eobool_true == canattach)
    {   
        // - tell the task that there is a command to process, so that a switch context is forced.
        s_eom_ipnet_Alert(s_eom_theipnet.ipnet, &s_eom_theipnet, eov_ipnet_evt_CMD2process);

        // - now the calling task is placed in hold until this semaphore is incremented 
        // - by the ipnet task when it has finished executing the command or its time has expired
        osal_semaphore_decrement(s_eom_theipnet.cmd.blockingsemaphore, osal_reltimeINFINITE);

        // verify the success of the operation
        if(1 == s_eom_theipnet.cmd.result)
        {
            res = eores_OK;
        }
        else
        {
            res = eores_NOK_timeout;
        }
    }
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
    else
    {
        osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect);
    }
#endif

    // - enable again another task to send a command to the ipnet
    osal_mutex_release(s_eom_theipnet.cmd.mtxcaller);

    return(res);    
}


/*  @brief      called by an external task, via some methods of a socket, to remove a socket from the management of the ipnet
    @details    it sends a command to the tskproc, which removes the sockets and then unblocks the caller.
 **/
static eOresult_t s_eom_ipnet_DetachSocket(EOVtheIPnet* ip, EOsocketDerived *s) 
{
    eOresult_t res = eores_NOK_generic;
    eObool_t candetach = eobool_false;;
    eOsocketType_t t;
    
    if((NULL == ip) || (NULL == s)) 
    {
        return(eores_NOK_nullpointer);    
    }

    t = eo_socket_hid_derived_Get_Type(s);
    
    // for now i only do datagrams  ...
    eo_errman_Assert(eo_errman_GetHandle(), (eo_skttyp_datagram == t), "s_eom_ipnet_DetachSocket(): only datagram socket", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);


    // - block other possible tasks from sending a command to the ipnet and also to execute this function from here onward
    osal_mutex_take(s_eom_theipnet.cmd.mtxcaller, osal_reltimeINFINITE);

#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
    if(osal_res_OK != osal_semaphore_decrement(s_eom_theipnet.cmd.busysemaphore, osal_reltimeINFINITE))
    {   // the semaphore could not be decremented because someone else before has decremented it: quit
        osal_mutex_release(s_eom_theipnet.cmd.mtxcaller);
        return(eores_NOK_generic);
    }  
#endif        
    // - fill the command and also verify if the socket can be detached
    candetach = s_eom_ipnet_detach_rqst_dtgsocket(ip, (EOsocketDatagram*)s);
    
    
    // - if the socket can be detached, send a command to ipnet main task and wait for the end of command
    if(eobool_true == candetach)
    {   
        // - tell the task that there is a command to process, so that a switch context is forced.
        s_eom_ipnet_Alert(s_eom_theipnet.ipnet, &s_eom_theipnet, eov_ipnet_evt_CMD2process);

        // - now the calling task is placed in hold until this semaphore is incremented 
        // - by the ipnet task when it has finished executing the command or its time has expired
        osal_semaphore_decrement(s_eom_theipnet.cmd.blockingsemaphore, osal_reltimeINFINITE);

        // verify the success of the operation
        if(1 == s_eom_theipnet.cmd.result)
        {
            res = eores_OK;
        }
        else
        {
            res = eores_NOK_timeout;
        }

    } 
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)  
    else
    {
        osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect);
    }
#endif

    // - enable again another task to send a command to the ipnet
    osal_mutex_release(s_eom_theipnet.cmd.mtxcaller);

    return(res);    
}

/*  @brief      sends an event to the tskproc
 **/
static eOresult_t s_eom_ipnet_Alert(EOVtheIPnet* ip, void *eobjcaller, eOevent_t evt) 
{
   
    if(NULL == ip) 
    {
        return(eores_NOK_nullpointer);    
    }
    
    // - simply send an event to the ip task. if the caller is an isr use the correct method. if a socket ...

    switch(evt)
    {
        case eov_ipnet_evt_RXethframe:
        {
            // caller is the eth isr
            eom_task_isrSetEvent(s_eom_theipnet.tskproc, eov_ipnet_evt_RXethframe);
        } break;

        case eov_ipnet_evt_TXdatagram:
        {
            // caller is a socket object which ... wants to tx a datagram packet. we need to put inside a fifo the reference to the socket
            // contained in eobjcaller
            if(osal_res_OK == osal_messagequeue_put(s_eom_theipnet.dgramsocketready2tx, (osal_message_t)eobjcaller, osal_reltimeINFINITE, osal_callerTSK))
            {
                // send event
                eom_task_SetEvent(s_eom_theipnet.tskproc, eov_ipnet_evt_TXdatagram);
            }
            else
            {
                eom_ipnet_diagnosticsInfo.datagrams_failed_to_go_in_txosalqueue ++;
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "s_eom_ipnet_Alert(): cant put in tx fifo", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);  
            }

        } break;

        default:
        {
            // caller is ... any other object from inside a task
            eom_task_SetEvent(s_eom_theipnet.tskproc, evt);
        } break;
    }

    return(eores_OK);    
}


/*  @brief      called by a socket configured in blocking mode when it attempts to retrieve a packet
    @details    the base socket only has a void* variable (blkgethandle) because it is osal-agnostic
                and does not know about osal_semaphore_t type.    
 **/
static eOresult_t s_eom_ipnet_WaitPacket(EOVtheIPnet* ip, EOsocketDerived *s, eOreltime_t tout) 
{
    eOresult_t res = eores_NOK_generic;
    EOsocket *bs = (EOsocket*)eo_common_getbaseobject(s);
    eOsocketType_t t;
    
    if((NULL == ip) || (NULL == s)) 
    {
        return(eores_NOK_nullpointer);    
    }

    t = eo_socket_hid_derived_Get_Type(s);

    
    // for now i only do datagrams and synchros ...
    eo_errman_Assert(eo_errman_GetHandle(), (eo_skttyp_datagram == t), "s_eom_ipnet_WaitPacket(): only datagram socket", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);

    
    // - waits in here but only if the semaphore really exists
    // - the semaphore is incremented by one each time the ipnet received a packet for this particular socket.
    if(NULL != bs->blkgethandle)
    {
        if(osal_res_OK == osal_semaphore_decrement(bs->blkgethandle, tout))
        {
            res = eores_OK;
        }
        else
        {
            res = eores_NOK_timeout;
        }
    }

    return(res);    
}


///*  @brief      used by eov_ipnet_GetTask() to give a pointer to the EOMtaskDerived object which runs teh ipnet.
//    @details    so far it is used by teh synchro-socket to send an event to the task ...     
// **/
//static EOVtaskDerived * s_eom_ipnet_GetTask(EOVtheIPnet* ip)
//{
//    if(NULL == ip) 
//    {
//        return(NULL);    
//    }
//
//    return(s_eom_theipnet.tskproc);
//}


// name of the task as it is shown in uvision
void eom_ipnetproc(void *p)
{
    eom_task_Start(p);
}

// name of the task as it is shown in uvision
void eom_ipnettick(void *p)
{
     eom_task_Start(p);
}



static void e_eom_ipnet_signal_new_frame_is_available(void)
{
    // the presence of this wakeup event which the ethernet isr sends 
    // to the task whcih process the tcp/ip stack improved response time of
    // ping from values around 200-300 ms or more down to values always <1ms.
    // the test is done on the iit intranet.
    
    if(eobool_false == s_eom_theipnet.ipnet->active)
    {
        return;
    }

    //eom_task_isrSetEvent(s_eom_theipnet.tskproc, eov_ipnet_evt_RXethframe);   
    osal_eventflag_set(eov_ipnet_evt_RXethframe, s_eom_theipnet.tskproc->osaltask, osal_callerAUTOdetect);
}


/*  @brief      the body of tsktick.
 **/
static void s_eom_ipnet_tsktick_forever(EOMtask *rt, uint32_t n)
{
// we call the ipal_sys_timetick_increment() at regular intervals even if teh ipnet is not active. the reason is that the timeout of teh stack must be computed anyway 
//    if(eobool_false == s_eom_theipnet.ipnet->active)
//    {
//        return;
//    }
    
    ipal_sys_timetick_increment();
}

static void s_eom_ipnet_ipal_start(void)
{
    uint32_t ram32sizeip;
    uint32_t *ram32dataip = NULL;

    // initialise the ipal
    ipal_base_memory_getsize(&s_eom_theipnet.ipcfg, &ram32sizeip);
    if(0 != ram32sizeip)
    {
        ram32dataip = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, ram32sizeip, 1);
    }

    if(eobool_true == s_eom_theipnet.taskwakeuponrxframe)
    {
        s_eom_theipnet.ipcfg.extfn.usr_on_ethframe_received = e_eom_ipnet_signal_new_frame_is_available;
    }
    else
    {
        s_eom_theipnet.ipcfg.extfn.usr_on_ethframe_received = NULL;
    }

    ipal_base_initialise(&s_eom_theipnet.ipcfg, ram32dataip);
    
    // start the ipal
    ipal_sys_start();       
}


/*  @brief      the startup of tsktproc.
    @details    in here we initialise and start the ipal, 
 **/
static void s_eom_ipnet_tskproc_startup(EOMtask *rt, uint32_t n)
{
#if     !defined(START_IPAL_INSIDE_eom_ipnet_Initialise)
    s_eom_ipnet_ipal_start();
#endif
}

/*  @brief      the body of tsktproc.
    @details    in here we: (a) process any incoming packet from the network, (b) transmit any packet of a synchro socket
                whose time has come  because the timer of the synsock has sent an event, (c) transmit any packet from datagrams
                with something in the output queues, (d) process a command request, (e) process a command stop, and (f) process
                a repetition of a command.                
 **/
static void s_eom_ipnet_tskproc_forever(EOMtask *rt, uint32_t evtmsk)
{
    static uint8_t force_transmission_when_reactivated = 0;
    
    // evtmask contains the events which the task has just received.
    
    if(eobool_false == s_eom_theipnet.ipnet->active)
    {
        if(eov_ipnet_evt_TXdatagram == (evtmsk & eov_ipnet_evt_TXdatagram))
        {    
            force_transmission_when_reactivated = 1;
        }
        
        return;
    }

        
    // - (a) process the tcp/ip stack.
    
    ipal_sys_process_communication();



//    if(eov_ipnet_evt_RXethframe == (evtmsk & eov_ipnet_evt_RXethframe))
//    {   // the ethernet isr has just received and processed an ethernet frame
//
//        // i could maybe return if i have this event and only this ????  no: i go on  
//    }


    // - (b) process transmission of a synchronised packet
    
//    if((0 != s_eom_theipnet.ipnet->synchsock_txmask) && 0 != (evtmsk & EOK_ipnet_evt_TXsynchroMASK))
//    {
//        s_eom_ipnet_process_transmission_synchro(evtmsk & EOK_ipnet_evt_TXsynchroMASK);
//    }


    // - (c) process transmission of a datagram packet
    
    // if we keep the tx of datagrams in here and before of teh processing of commands, then we
    // allow the following mechanism: transmit one or mor packets (by a very high priority task)
    // and then ... the same task closes teh socket .... well: the IPnet before transmit every packet
    // in the fifooutput and then closes the socket.
    if((eov_ipnet_evt_TXdatagram == (evtmsk & eov_ipnet_evt_TXdatagram)) || (1 == force_transmission_when_reactivated))
    {
        force_transmission_when_reactivated = 0;
        s_eom_ipnet_process_transmission_datagram();
    }

    
    // - (d) process request of a command
    
    if(eov_ipnet_evt_CMD2process == (evtmsk & eov_ipnet_evt_CMD2process))
    {
        s_eom_ipnet_process_command();
    }

    
    // - (e) process termination of processing of a command (the termination is internally issued)
    
    if(eov_ipnet_evt_CMD2stop == (evtmsk & eov_ipnet_evt_CMD2stop))
    {
        s_eom_ipnet_stop_command();
    }
    
    
    // - (e) process repetion of a command (the repetition is internally issued)
    
    if(1 == s_eom_theipnet.cmd.repeatcmd)
    {
        s_eom_ipnet_repeat_command();
    }

}

#ifdef SLOWERMODEONRXDATAGRAM
static eOresult_t s_eom_ipnet_DatagramSocketHas(void *item, void *param)
{
    EOsocketDatagram* s   = *((EOsocketDatagram**)item);
    ipal_udpsocket_t* skt = (ipal_udpsocket_t*)param;

    if(skt == eo_socket_hid_derived_Get_Handle(s))
    {
        return(eores_OK);
    }

    return(eores_NOK_generic);
}
#endif


/*  @brief      called by ipal_sys_process_communication() on reception of a datagram socket
    @details    the ipal calls a function with this parameters. we search for the correct EOsocketDatagram and then
                we link the received data inside a temporary packet which is then copied inside the input queue of
                the datagram socket.
                finally we alert the caller or unblock its wait                
 **/
static void s_eom_ipnet_OnReceptionDatagram(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t por)
{
    EOsocketDatagram *dtgskt = NULL;
    volatile eOresult_t res = eores_NOK_timeout;
   

#ifdef SLOWERMODEONRXDATAGRAM
    EOlistIter *li = NULL;
    // we are sure that the s_eom_theipnet.ipnet->activedgramsocksptrlist is non NULL because we setup this callback
    // only if we have support for the datagramsockets, hence we have this list.

    // find the socket with descriptor soc inside the list s_eom_theipnet.ipnet->activedgramsocksptrlist
    // the function s_eom_ipnet_DatagramSocketHas() returns eores_OK and thus the search is ok when an item
    // of the list (a pointer to a EOsocketDatagram) has a ->socket->skthandle which is uqual to skt

    li = eo_list_Find(s_eom_theipnet.ipnet->activedgramsocksptrlist, s_eom_ipnet_DatagramSocketHas, skt);

    if(NULL == li)
    {   // did not find the socket ... quit
        return;
    }

    // ok ... dtgskt is the datagram socket
    dtgskt = *(EOsocketDatagram**)eo_list_At(s_eom_theipnet.ipnet->activedgramsocksptrlist, li);
#else
    // as specified by ipal_udpsocket_recv(), the variable arg keeps the socketdatagram  
    dtgskt = (EOsocketDatagram *)arg;
#endif

    if(eo_sktdir_TXonly == dtgskt->socket->dir)
    {
        // it is transmitting-only socket ... quit
        return;
    }

            
    // build a packet, s_eom_theipnet.rxpacket, with rx payload, its size, address and port
    // and then use it to copy info into the input queue.
    
    // for now, i use the uint32_t in the simplest way: as a holder for a uint8_t addr[4].
    eo_packet_Full_LinkTo(s_eom_theipnet.rxpacket, adr, por, pkt->size, pkt->data);
   
    res = eo_fifo_Put(dtgskt->dgramfifoinput, s_eom_theipnet.rxpacket, s_eom_theipnet.maxwaittime);

    if(eores_OK != res)
    {
        eom_ipnet_diagnosticsInfo.datagrams_failed_to_go_in_rxfifo ++;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "s_eom_ipnet_OnReceptionDatagram(): cant put in rx fifo", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
        // return because ... we did not put the message in the queue and thus ... we dont want do any action on reception
        return;
    }


    // do registered action on reception
    eo_action_Execute(dtgskt->socket->onreception, eok_reltimeZERO);

    if(eobool_true == dtgskt->socket->block2wait4packet)
    {
        // unblock the reception
        osal_semaphore_increment(dtgskt->socket->blkgethandle, osal_callerTSK);
    }

  

    return;
}




static void s_eom_ipnet_process_command(void)
{
//    EOsocket *s = NULL;
    EOsocketDerived *sdrv = NULL;
    eOsizecntnr_t size = 0;


    switch(s_eom_theipnet.cmd.opcode)
    {
        case cmdDoARP:
        {
            // par16b: max number of arp packets sent
            // par32b: ip address
            // in here we are sure that we have valid params (non-zero so far)
            if(0 != s_eom_theipnet.cmd.par16b)
            { 
                s_eom_theipnet.cmd.par16b --;
                s_eom_theipnet.cmd.repeatcmd = 1;

                s_eom_theipnet.cmd.par64x = osal_system_abstime_get();
#if defined(TEST_ARP_EVIEW)
                evprev_ipnetarp = eventviewer_switch_to(ev_ipnetarp);
                eventviewer_switch_to(evprev_ipnetarp);
#endif 
#if defined(TEST_ARP_PRINT)
                char st[60] = {0};
                uint32_t x = s_eom_theipnet.cmd.par64x/1000;
                snprintf(st, sizeof(st), "arp@ms=%d", x);
                hal_trace_puts(st);
#endif
                const uint8_t forcearpframe = 1;
                if(ipal_res_OK == ipal_arp_resolve(s_eom_theipnet.cmd.par32b, ipal_arp_cache_permanently, forcearpframe))
                { 
                    // ok at the first time .....  
                    s_eom_theipnet.cmd.result = 1;
                    s_eom_theipnet.cmd.par16b = 0;
                    s_eom_theipnet.cmd.repeatcmd = 0;
                    s_eom_theipnet.cmd.tout = 0;
                    // release the caller
#if defined(TEST_ARP_EVIEW)
                    eventviewer_switch_to(ev_mark2);
#endif
                    osal_semaphore_increment(s_eom_theipnet.cmd.blockingsemaphore, osal_callerTSK);
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
                    osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect);
#endif                    
                }
                else if(0 != s_eom_theipnet.cmd.tout)
                {
#if !defined(ARP_NEWMODE)
                    // unlucky. retry with a timeout
                    // start a one-shot timer with tout and which sends an event of stop to this task
                    eo_action_SetEvent(s_eom_theipnet.cmd.stopact, eov_ipnet_evt_CMD2stop, s_eom_theipnet.tskproc);
                    eo_timer_Start(s_eom_theipnet.cmd.stoptmr, eok_abstimeNOW, s_eom_theipnet.cmd.tout, eo_tmrmode_ONESHOT, s_eom_theipnet.cmd.stopact);
#else          
                    
                    if(ipal_res_OK == ipal_arp_isresolved(s_eom_theipnet.cmd.par32b))
                    {
                        // ok at the seconf time .....  
                        s_eom_theipnet.cmd.result = 1;
                        s_eom_theipnet.cmd.par16b = 0;
                        s_eom_theipnet.cmd.repeatcmd = 0;
                        s_eom_theipnet.cmd.tout = 0;
                        // release the caller
#if defined(TEST_ARP_EVIEW)
                        eventviewer_switch_to(ev_mark2);
#endif
                        osal_semaphore_increment(s_eom_theipnet.cmd.blockingsemaphore, osal_callerTSK);  
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)                        
                        osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect); 
#endif                        
                    }
                    else
                    {                    
                        // unlucky. retry with a timeout
                        // start a one-shot timer with tout and which sends an event of stop to this task
                        eo_action_SetEvent(s_eom_theipnet.cmd.stopact, eov_ipnet_evt_CMD2stop, s_eom_theipnet.tskproc);
                        eo_timer_Start(s_eom_theipnet.cmd.stoptmr, eok_abstimeNOW, s_eom_theipnet.cmd.tout, eo_tmrmode_ONESHOT, s_eom_theipnet.cmd.stopact);
                        
                        // - tick the task in order to produce max throughput.
                        //s_eom_ipnet_Alert(s_eom_theipnet.ipnet, &s_eom_theipnet, eov_ipnet_evt_tick);

                    }
#endif                
                }

            }

 
        } break;

        case cmdAttachDTG:
        {
            // get the derived socket
            sdrv = (((void*) s_eom_theipnet.cmd.par32b)); 
            
            // initialise the base socket, commit the derived socket in proper data structure of ipnet
            s_eom_ipnet_attach_proc_dtgsocket((EOsocketDatagram*)sdrv);

            // increment the semaphore to allow execution of the caller
            osal_semaphore_increment(s_eom_theipnet.cmd.blockingsemaphore, osal_callerTSK); 
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
            osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect);
#endif
        } break;

        case cmdDetachDTG:
        {
 
            // get the derived socket
            sdrv = (((void*) s_eom_theipnet.cmd.par32b)); 

            // initialise the base socket, commit the derived socket in proper data stratcure of ipnet

            // if we are asked to close a datagram socket which has some more packets to transmit, then
            // we decide to empty the fifo first. we do that by sending a eov_ipnet_evt_TXdatagram
            // event but also a eov_ipnet_evt_CMD2process to get here inside again. 

            if(eores_OK == eo_fifo_Size(((EOsocketDatagram*)sdrv)->dgramfifooutput, &size, s_eom_theipnet.maxwaittime))
            {
                if(0 != size)
                {
                    // sends an event of kind tx-pkt and cmd
                    eom_task_SetEvent(s_eom_theipnet.tskproc, eov_ipnet_evt_TXdatagram | eov_ipnet_evt_CMD2process);
                    // and then return ....
                    return;
                }
            }

            // if we have not returned, then we dont have any more datagram to send, thus ... detach the datagram socket
            s_eom_ipnet_detach_proc_dtgsocket((EOsocketDatagram*)sdrv);


            // increment the semaphore to allow execution of the caller
            osal_semaphore_increment(s_eom_theipnet.cmd.blockingsemaphore, osal_callerTSK); 
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
            osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect);
#endif                              
        } break;


        default:
        {
        } break;

    }
}

static void s_eom_ipnet_repeat_command(void)
{
#if defined(ARP_NEWMODE)
    //static uint8_t mux = 0;
    uint8_t stopit = 0;
#endif    
    switch(s_eom_theipnet.cmd.opcode)
    {
        case cmdDoARP:
        {
            // par16b: max number of arp packets sent
            // par32b: ip address
            // in here we are sure that we have valid params (non-zero so far)
            if(0 != s_eom_theipnet.cmd.par16b)
            { 
                if(0 == s_eom_theipnet.cmd.tout)
                {
                    // with this control, the exit is done not with par16 being decremented but with timeout expiry
                    s_eom_theipnet.cmd.par16b --;
                }
                s_eom_theipnet.cmd.repeatcmd = 1;
#if !defined(ARP_NEWMODE)
                const uint8_t forcearpframe = 1;
                if(ipal_res_OK == ipal_arp_resolve(s_eom_theipnet.cmd.par32b, ipal_arp_cache_permanently, forcearpframe))
                { 
                    // ok at the first time .....  
                    s_eom_theipnet.cmd.result = 1;
                    s_eom_theipnet.cmd.par16b = 0;
                    s_eom_theipnet.cmd.repeatcmd = 0;
                    s_eom_theipnet.cmd.tout = 0;
                    // release the timer (if any)
                    if(0 != s_eom_theipnet.cmd.tout)
                    {
                        eo_timer_Stop(s_eom_theipnet.cmd.stoptmr);
                        s_eom_theipnet.cmd.tout = 0;
                    }
                    // release the caller
                    osal_semaphore_increment(s_eom_theipnet.cmd.blockingsemaphore, osal_callerTSK);
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
                    osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect);
#endif
                }
#else

                if(ipal_res_OK == ipal_arp_isresolved(s_eom_theipnet.cmd.par32b))
                {
                    stopit = 1;
                }
                else 
                {
                    uint64_t now = osal_system_abstime_get();
#if defined(TEST_ARP_PRINT)
                    uint32_t delta = now - s_eom_theipnet.cmd.par64x;
#endif                    
                    if(now >= (s_eom_theipnet.cmd.par64x+s_eom_theipnet.cmd.par32x-10))
                    {
                        s_eom_theipnet.cmd.par64x = osal_system_abstime_get();   
#if defined(TEST_ARP_EVIEW)
                        evprev_ipnetarp = eventviewer_switch_to(ev_ipnetarp);
                        eventviewer_switch_to(evprev_ipnetarp);
#endif
#if defined(TEST_ARP_PRINT)
                        char st[60] = {0};
                        uint32_t x = s_eom_theipnet.cmd.par64x/1000;
                        snprintf(st, sizeof(st), "arp@ms=%d (d=%d)", x, delta/1000);
                        hal_trace_puts(st);
#endif
                        const uint8_t forcearpframe = 1;
                        if(ipal_res_OK == ipal_arp_resolve(s_eom_theipnet.cmd.par32b, ipal_arp_cache_permanently, forcearpframe))
                        {
                            stopit = 1;
                        }
                    }
                }
//                else if(0 != s_eom_theipnet.cmd.tout)
//                {
//                    // - tick the task in order to produce max throughput.
//                    //s_eom_ipnet_Alert(s_eom_theipnet.ipnet, &s_eom_theipnet, eov_ipnet_evt_tick);
//                    
//                    // allow 1 ms of wait
//                    //osal_task_wait(2000);
//                    //hal_sys_delay(1000);
//                }
                
                if(1 == stopit)
                {
#if defined(TEST_ARP_EVIEW)
                    eventviewer_switch_to(ev_mark2);
#endif
                    // ok now .....  
                    s_eom_theipnet.cmd.result = 1;
                    s_eom_theipnet.cmd.par16b = 0;
                    s_eom_theipnet.cmd.par32x = 0;
                    s_eom_theipnet.cmd.par64x = 0;
                    s_eom_theipnet.cmd.repeatcmd = 0;
                    s_eom_theipnet.cmd.tout = 0;
                    // release the timer (if any)
                    if(0 != s_eom_theipnet.cmd.tout)
                    {
                        eo_timer_Stop(s_eom_theipnet.cmd.stoptmr);
                        s_eom_theipnet.cmd.tout = 0;
                    }
                    
                    // release the caller
                    osal_semaphore_increment(s_eom_theipnet.cmd.blockingsemaphore, osal_callerTSK);
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
                    osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect);
#endif
                }
#endif           

            }
            else
            {
                // reset command, give result ko, clear params, release semaphore
                s_eom_theipnet.cmd.result = 0;
                s_eom_theipnet.cmd.par16b = 0;
                s_eom_theipnet.cmd.opcode = cmdDoNONE;
                s_eom_theipnet.cmd.par32b = 0;
                s_eom_theipnet.cmd.par32x = 0;
                s_eom_theipnet.cmd.par64x = 0;                
                s_eom_theipnet.cmd.repeatcmd = 0;
 
                // release the timer (if any)
                if(0 != s_eom_theipnet.cmd.tout)
                {
                    eo_timer_Stop(s_eom_theipnet.cmd.stoptmr);
                    s_eom_theipnet.cmd.tout = 0;
                }
#if defined(TEST_ARP_EVIEW)
                    eventviewer_switch_to(ev_mark2);
#endif
                // release the caller
                osal_semaphore_increment(s_eom_theipnet.cmd.blockingsemaphore, osal_callerTSK); 
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
                osal_semaphore_increment(s_eom_theipnet.cmd.busysemaphore, osal_callerAUTOdetect);                
#endif
            } 
 
        } break;


        default:
        {
            s_eom_theipnet.cmd.repeatcmd = 0;
        } break;

    }
}

static void s_eom_ipnet_stop_command(void)
{
    s_eom_theipnet.cmd.par16b = 0;
    s_eom_ipnet_repeat_command();
}

static void s_eom_ipnet_process_transmission_datagram(void)
{
    EOsocketDatagram *s = NULL;
    eOresult_t res;
    const void *vitem = NULL;
    EOpacket *ditem = NULL;
    ipal_packet_t ipalpkt;

#if 0
// we can use a getrem only if we have a pre-allocated and big enough packet where to copy
    static EOpacket *txpkt = NULL;
    txpkt = eo_packet_New(bignumber); // bif number depends on the socket.
#endif
    // get the socket which has something to transmit
    s = (EOsocketDatagram*) osal_messagequeue_getquick(s_eom_theipnet.dgramsocketready2tx, osal_reltimeINFINITE, osal_callerTSK);

    if(NULL == s)
    {
        return;
    }
        
    // do action depending on status of the socket
    switch(s->socket->status)
    {
        case STATUS_SOCK_OPENED:
        {
            // get dgram from output queue. 
            // VERY IMPORTANT: it may be that we have a periodic
            // transmission of the socket and the user does not have a packet to transmit, thus fifooutput is empty.
            // well: we dont want to block in there. we can:
            // 1. retriev the size, 2. if seize is not zero the we retrieve the packet with infinite wait (so that we are sure to get it)

            uint16_t sizefifoout = 0;
            res = eo_fifo_Size(s->dgramfifooutput, &sizefifoout, eok_reltimeINFINITE);
            
            if(0 != sizefifoout)
            {
                res = eo_fifo_Get(s->dgramfifooutput, &vitem, eok_reltimeINFINITE);
                ditem = ((EOpacket *)vitem);
                // if i use a quicker get-rem ... uncomments the following two line and comment the eo_fifo_Rem()
                //res = eo_fifo_GetRem(s->dgramfifooutput, txpkt, eok_reltimeZERO);
                //ditem = ((EOpacket *)txpkt);
                if(eores_OK == res) 
                {   // transmit the datagram
                    ipalpkt.data = ditem->data;
                    ipalpkt.size = ditem->size;
                    if(ipal_res_OK == ipal_udpsocket_sendto(s->socket->skthandle, &ipalpkt, ditem->remoteaddr, ditem->remoteport))
                    {
                        // remove the datagram being transmitted
                        eo_fifo_Rem(s->dgramfifooutput, s_eom_theipnet.maxwaittime);

                        // do action on tx-done
                        if((NULL != s->socket->ontransmission) && (eo_actypeNONE != eo_action_GetType(s->socket->ontransmission)))
                        {
                            eo_action_Execute(s->socket->ontransmission, eok_reltimeZERO);
                        }
                    }
                    else
                    {
                         // remove the datagram being transmitted also if you fail to tx it.
                        eo_fifo_Rem(s->dgramfifooutput, s_eom_theipnet.maxwaittime);
                        // but put a warning on it.
                        eom_ipnet_diagnosticsInfo.datagrams_failed_to_be_sent_by_ipal ++;
                        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "s_eom_ipnet_process_transmission_datagram(): ipal fails in tx", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);

                    }
                }
                else
                {
                    eom_ipnet_diagnosticsInfo.datagrams_failed_to_be_retrieved_from_txfifo ++;       
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "s_eom_ipnet_process_transmission_datagram(): dgramfifooutput is empty", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);                    
                }
            
            }

            // if message queue is not empty, then sends another tx event because there is another socket which needs transmission
            if(0 != osal_messagequeue_size(s_eom_theipnet.dgramsocketready2tx, osal_callerTSK))
            {
                eom_task_SetEvent(s_eom_theipnet.tskproc, eov_ipnet_evt_TXdatagram);
            } 
            
        } break;
            
        default:
        {
        } break;            
        
    }
        

}




/*  @brief      this function is executed by teh calling task.
                if the socket can be attached: completes the OSAL-related initialisation of socket, 
                fills the command to be sent to tskproc with opcode = cmdAttachDTG and returns eobool_true. 
 **/
static eObool_t s_eom_ipnet_attach_rqst_dtgsocket(EOVtheIPnet *vip, EOsocketDatagram *dgmskt)
{

    EOlist *socklist = vip->activedgramsocksptrlist;

    if(NULL == socklist)
    {
        // teh ipnet was initted without datagram sockets ... return false
        return(eobool_false);
    }
    
    // block socklist ... no need
    //eom_mutex_Take(s_eom_theipnet.ipnet->mutexactivedgram, eok_reltimeINFINITE);

    // if list is already full then return false, otherwise go on with building up a command.
    if(eobool_true == eo_list_Full(socklist)) 
    {
        // unblock ... no need
        //eom_mutex_Release(s_eom_theipnet.ipnet->mutexactivedgram);
        return(eobool_false);
    }

    // unblock socklist ... no need
    //eom_mutex_Release(s_eom_theipnet.ipnet->mutexactivedgram);
    
    // create the semaphore for the socket if we have blocking mode and the semaphore is still NULL.
    // we init the sempahore with maxtokens equal to the capacity of the input fifo + 1.
    if((NULL == dgmskt->socket->blkgethandle) && (eobool_true == dgmskt->socket->block2wait4packet))
    {
        eOsizecntnr_t maxtokens;
        eo_fifo_Capacity(dgmskt->dgramfifoinput, &maxtokens, eok_reltimeINFINITE);
        dgmskt->socket->blkgethandle = osal_semaphore_new(maxtokens+1, 0);
        eo_errman_Assert(eo_errman_GetHandle(), (NULL != dgmskt->socket->blkgethandle), "s_eom_ipnet_attach_rqst_dtgsocket(): blkgethandle is not created by osal", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
    }    
    
    // build the command
    s_eom_theipnet.cmd.par32b = (uint32_t)dgmskt;
    s_eom_theipnet.cmd.tout  = 0;
    s_eom_theipnet.cmd.opcode = cmdAttachDTG;
 
  
    return(eobool_true);  
}


/*  @brief      this function is executed by taskproc at reception of a command with opcode = cmdAttachDTG. 
                it just create the ipal udp socket and store it inside the skthandle
 **/
static void s_eom_ipnet_attach_proc_dtgsocket(EOsocketDatagram *dtgs)
{
    const ipal_tos_t tos = {.precedence = ipal_prec_priority, .lowdelay = 1, .highthroughput = 1, .highreliability = 1, .unused = 0};
    
    EOsocket *s = eo_common_getbaseobject(dtgs);
 
    // reset the command
    s_eom_theipnet.cmd.result = 0;
    s_eom_theipnet.cmd.opcode = cmdDoNONE;
    s_eom_theipnet.cmd.par32b = 0; 

    // create the ipal socket, bind it, set the callback on receive
    s->skthandle = ipal_udpsocket_new(tos);

    if(NULL == s->skthandle)
    {
        return;
    }

    if(ipal_res_OK != ipal_udpsocket_bind(s->skthandle, IPAL_ipv4addr_INADDR_ANY, s->localport))
    {
        return;
    }

    if(ipal_res_OK != ipal_udpsocket_recv(s->skthandle, s_eom_ipnet_OnReceptionDatagram, dtgs))
    {
        return;
    }


    // we are ok ... set other things
    s->status = STATUS_SOCK_OPENED;
    // take the list of active datagrams
    //eom_mutex_Take(s_eom_theipnet.ipnet->mutexactivedgram, eok_reltimeINFINITE); 

    // add the pointer of the datagram socket inside the list of active datagrams managed by the ipnet
    eo_list_PushFront(s_eom_theipnet.ipnet->activedgramsocksptrlist, &dtgs);

    // release it
    //eom_mutex_Release(s_eom_theipnet.ipnet->mutexactivedgram); 
    
    s_eom_theipnet.cmd.result = 1;
}


/*  @brief      this function is executed by the calling task.
                if the socket can be detached: fills the command to be sent to tskproc with opcode = cmdDetachDTG 
                and returns eobool_true. 
 **/
static eObool_t s_eom_ipnet_detach_rqst_dtgsocket(EOVtheIPnet *vip, EOsocketDatagram *dgmskt)
{
    // returns eobool_true if the socket can be detached

    EOlist *socklist = vip->activedgramsocksptrlist;

    if(NULL == socklist)
    {
        // the ipnet was initted without datagram sockets ... return false
        return(eobool_false);
    }

    
    // block socklist ... no need
    //eom_mutex_Take(s_eom_theipnet.ipnet->mutexactivedgram, eok_reltimeINFINITE);

    // if cannot find the socket in list of active datagram managed by the ipnet then return false, 
    // otherwise go on with building up a command.
    if(NULL == eo_list_FindItem(socklist, &dgmskt)) 
    {
        // unblock ... no need
        //eom_mutex_Release(s_eom_theipnet.ipnet->mutexactivedgram);
        return(eobool_false);
    }

    // unblock socklist ... no need
    //eom_mutex_Release(s_eom_theipnet.ipnet->mutexactivedgram);
    
    // build the command
    s_eom_theipnet.cmd.par32b = (uint32_t)dgmskt;
    s_eom_theipnet.cmd.tout  = 0;
    s_eom_theipnet.cmd.opcode = cmdDetachDTG;
  
    return(eobool_true);  
}


/*  @brief      this function is executed by taskproc at reception of a command with opcode = cmdDetachDTG. 
                ...
 **/
static void s_eom_ipnet_detach_proc_dtgsocket(EOsocketDatagram *dtgs)
{
    EOlist *socks = s_eom_theipnet.ipnet->activedgramsocksptrlist;
    
    EOsocket *s = eo_common_getbaseobject(dtgs);

    // reset the command
    s_eom_theipnet.cmd.result = 0;
    s_eom_theipnet.cmd.opcode = cmdDoNONE;
    s_eom_theipnet.cmd.par32b = 0; 


    // close and delete the osal socket
    ipal_udpsocket_delete(s->skthandle);

    // clear data structure
    s->skthandle = NULL;
    s->status = STATUS_SOCK_NONE;

    // remove the datagram socket from list
    eo_list_Erase(socks, eo_list_FindItem(socks, &dtgs));  

    // result is ok now
    s_eom_theipnet.cmd.result = 1;
}







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





