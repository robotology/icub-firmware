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

/* @file       eom_applbasic_specialise.c
	@brief      This file keeps the vcececew
	@author     marco.accame@iit.it
    @date       01/11/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEmemorymap.h"
#include "EOVtheSystem.h"
#include "EOMtask.h"
#include "EOpacket.h"
#include "EOsocketDatagram.h"
#include "EOMmutex.h"
#include "EOaction_hid.h"
#include "EOtheErrorManager.h"

#include "osal.h"
#include "hal.h"
#include "ipal.h"

#include "stdlib.h"

#include "hal_eth.h"


#include "eom_applbasic_extra.h"


#include "eventviewer.h"

#include "EOtimer.h"
#include "EOaction.h"
#include "EOMtheCallbackmanager.h"

#include "EOtheLEDpulser.h"

extern const ipal_cfg_t    ipal_cfg;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eom_applbasic_specialise.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

extern void task_udpserver(void *p);
extern void task_periodic(void *p);



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t* caller, const eOerrmanDescriptor_t* des);

static void s_udpserver_startup(EOMtask *p, uint32_t t);

static void s_udpserver_run(EOMtask *p, uint32_t t);

static void s_periodic_startup(EOMtask *p, uint32_t t);

static void s_periodic_run(EOMtask *p, uint32_t t);

static void s_specialise_blink(void *param);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtimer*     s_timer_blink = NULL;
static EOaction*    s_action_blink = NULL;


static osal_semaphore_t*        s_sem_tx_pkt        = NULL;

static EOsocketDatagram*        s_skt_rops          = NULL;
static EOpacket*                s_rxpkt             = NULL;
static EOpacket*                s_txpkt             = NULL;
static EOMtask*                 s_task_udpserver    = NULL;


static void (*s_eom_applbasic_specialise_on_ropframe_received)(EOpacket*) = NULL;



static const eOevent_t s_event_tx_into_skt_rops         = 0x00000001;
static const eOevent_t s_event_connect_to_host          = 0x00000002;
static const eOevent_t s_event_from_skt_rops            = 0x00000004;

static const eOipv4port_t s_server_port                 = 33333; 

static volatile eObool_t s_host_connected               = eobool_false;

static eOipv4addr_t s_host_ipaddress                    = 0;


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const ipal_cfg_t* eom_applbasic_specialise_ipal_cfg = &ipal_cfg;


extern const eOerrman_cfg_t  eom_applbasic_specialise_errcfg = 
{
    .extfn.usr_on_error = s_OnError
};

extern const eOmipnet_cfg_dtgskt_t eom_applbasic_specialise_dtgskt_cfg = 
{   
    .numberofsockets            = 2, 
    .maxdatagramenqueuedintx    = 2
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

void getfromEOsocket(void)
{

}


extern void eom_applbasic_specialise_updserver_start(void)
{
    s_sem_tx_pkt =  osal_semaphore_new(255, 0);

    s_host_connected = eobool_false;

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_sem_tx_pkt), "semaphore is NULL", "applbasic_specialise", NULL);

    s_task_udpserver = eom_task_New(eom_mtask_EventDriven, 100, 2*1024, s_udpserver_startup, s_udpserver_run,  6, 
                                    eok_reltimeINFINITE, NULL, 
                                    task_udpserver, "udpserver");

    s_task_udpserver = s_task_udpserver;

}  

extern void eom_applbasic_specialise_onpktreceived_set( void (*cbk)(EOpacket*)  )
{
    s_eom_applbasic_specialise_on_ropframe_received = cbk;
}

extern void eom_applbasic_specialise_otherthings(void)
{

    eom_applbasic_extra_transceiver_init();
    
    // init the 5 leds and blink every second
    hal_led_init(hal_led0, NULL);
    hal_led_init(hal_led1, NULL);
    hal_led_init(hal_led2, NULL);
    hal_led_init(hal_led3, NULL);
    hal_led_init(hal_led4, NULL);
    //hal_led_init(hal_led5, NULL);
    
    s_timer_blink = eo_timer_New();
    s_action_blink = eo_action_New();
    eo_action_SetCallback(s_action_blink, 
                          s_specialise_blink, NULL,
                          eom_callbackman_GetTask(eom_callbackman_GetHandle())
                         );
    
    eo_timer_Start(s_timer_blink, eok_abstimeNOW, 500*1000, eo_tmrmode_FOREVER, s_action_blink);    
    
    
    
    // init teh ledpulser with led 5
    
    eOledpulser_cfg_t ledpulsercfg = 
    {
        .led_enable_mask    = 1 << eo_ledpulser_led_five,
        .led_init           = (eOint8_fp_uint8_cvoidp_t)hal_led_init,
        .led_on             = (eOint8_fp_uint8_t)hal_led_on,
        .led_off            = (eOint8_fp_uint8_t)hal_led_off,
        .led_toggle         = (eOint8_fp_uint8_t)hal_led_toggle
    };
    
    eo_ledpulser_Initialise(&ledpulsercfg);    
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_five, 2*1000*1000, 0);
    
    //eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_five);
    
    
    eom_task_New(eom_mtask_Periodic, 100, 2*1024, NULL, s_periodic_run,  0, 
                eok_reltime1sec, NULL, 
                task_periodic, "periodic");
}


extern void eom_applbasic_specialise_transmit(EOpacket *txpkt)
{

   // copy teh pkt pointer
    s_txpkt = txpkt;

    eo_packet_Addressing_Set(s_txpkt, s_host_ipaddress, s_server_port);

    eo_socketdtg_Put(s_skt_rops, s_txpkt);

//    // alert the task upd server
//    eom_task_SetEvent(s_task_udpserver, s_event_tx_into_skt_rops);
//
//    // decrement the semaphore to wait for end of operation
//    osal_semaphore_decrement(s_sem_tx_pkt, OSAL_reltimeINFINITE);
   
}


extern eObool_t eom_applbasic_specialise_connect(eOipv4addr_t remotehostaddr, uint32_t tout)
{
    if((eobool_false == s_host_connected) || (remotehostaddr != s_host_ipaddress))
    {
        // attempt connection for 1 second. no more. 
        if(eores_OK == eo_socketdtg_Connect(s_skt_rops, remotehostaddr, tout))
        {
            s_host_ipaddress = remotehostaddr;
            s_host_connected = eobool_true;
        }
        else
        {
            s_host_connected = eobool_false;
            //printf("not connecetd after %d ms. i shall try again at next reception\n\r", eok_reltime1sec/1000);
        }
    }
    
    if(eobool_true == s_host_connected)
    {
        hal_led_on(hal_led0);
    }
    else
    {
        hal_led_off(hal_led0);
    }

    return(s_host_connected);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern void task_udpserver(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

extern void task_periodic(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t* caller, const eOerrmanDescriptor_t* des)
{
    const char err[5][16] = {"info", "debug", "warning", "error", "fatal"};
    char str[128];

    if(NULL != caller)
    {
        snprintf(str, sizeof(str), "[eobj: %s, tsk: %d] %s: %s", caller->eobjstr, caller->taskid, err[(uint8_t)errtype], info);
    }
    hal_trace_puts(str);

    if(errtype <= eo_errortype_warning)
    {
        return;
    }

    for(;;);
}


static const eOipv4addr_t hostpc104ipaddr = EO_COMMON_IPV4ADDR(10, 0, 1, 104);

static void s_udpserver_startup(EOMtask *p, uint32_t t)
{
    EOaction s_fullaction;

    // init the rx packet 
    s_rxpkt = eo_packet_New(512);  
 

    // initialise the socket 
#if 1
    s_skt_rops = eo_socketdtg_New(  2, 512, eom_mutex_New(), // input queue
                                    2, 512, eom_mutex_New()  // output queue
                                 );
#else
    s_skt_rops = eo_socketdtg_New(  2, 512, NULL, // input queue
                                    2, 512, NULL  // output queue
                                 );
#endif

    // set the rx action on socket to be a message s_message_from_skt_rops to this task object
    eo_action_SetEvent(&s_fullaction, s_event_from_skt_rops, p);
    eo_socketdtg_Open(s_skt_rops, s_server_port, eo_sktdir_TXRX, eobool_false, NULL, &s_fullaction, NULL);
    
    eom_applbasic_specialise_connect(hostpc104ipaddr, 2*eok_reltime1sec);

    //eventviewer_load(ev_ID_first_usrdef+0, getfromEOsocket);
        
}

static void s_udpserver_run(EOMtask *p, uint32_t t)
{
    // read the packet.
    eOresult_t res;
    uint16_t numberof = 0;

    // the event that we have received
    eOevent_t evt = (eOevent_t)t;

    if(s_event_from_skt_rops == (evt & s_event_from_skt_rops))
    {

        //evEntityId_t prev = eventviewer_switch_to(ev_ID_first_usrdef+0);
        // retrieve the pkt.
        res = eo_socketdtg_Get(s_skt_rops, s_rxpkt, eok_reltimeINFINITE);

        //eventviewer_switch_to(prev);

        // call a function for it
        if((eores_OK == res) && (NULL != s_eom_applbasic_specialise_on_ropframe_received))
        {
            s_eom_applbasic_specialise_on_ropframe_received(s_rxpkt);
        }

        // if any other pkt in socket then send event again
        res = eo_socketdtg_Received_NumberOf(s_skt_rops, &numberof);
        if((0 != numberof) && (eores_OK == res))
        {
            eom_task_SetEvent(p, s_event_from_skt_rops);
        }
    }
    
//    if(s_event_tx_into_skt_rops == (evt & s_event_tx_into_skt_rops))
//    {
//        // transmit the pkt.
//        res = eo_socketdtg_Put(s_skt_rops, s_txpkt);
//
//        // increment the semaphore so taht teh caller can return
//        osal_semaphore_increment(s_sem_tx_pkt);
//    }
//
//    if(s_event_connect_to_host == (evt & s_event_connect_to_host))
//    {
//
//        // attempt connection for 1 second. no more. 
//        if(eores_OK == eo_socketdtg_Connect(s_skt_rops, s_host_ipaddress, eok_reltime1sec))
//        {
//            s_host_connected = eobool_true;
//        }
//        else
//        {
//            s_host_connected = eobool_false;
//            //printf("not connecetd after %d ms. i shall try again at next reception\n\r", eok_reltime1sec/1000);
//        }
// 
//        // increment the semaphore so taht teh caller can return
//        osal_semaphore_increment(s_sem_tx_pkt);
//    }

}

static void s_periodic_startup(EOMtask *p, uint32_t t)
{
    
}

static void s_periodic_run(EOMtask *p, uint32_t t)
{
    
    // read the eth status
    
    hal_eth_phy_status_t stat[2];
    
    hal_result_t res = hal_res_NOK_generic;
    
    res = hal_eth_get_links_status(stat, 2);
    
    char str[128];

    if(hal_res_OK == res)
    {
        snprintf(str, sizeof(str)-1, "stat[0] is: up = %d, autoneg = %d, link = %d, speed100 = %d, duplexfull = %d", 
                                        stat[0].linkisup, 
                                        stat[0].autoNeg_done, 
                                        stat[0].linkisgood, 
                                        stat[0].linkspeed, 
                                        stat[0].linkduplex);
        //hal_trace_puts(str);    
        
        snprintf(str, sizeof(str)-1, "stat[1] is: up = %d, autoneg = %d, link = %d, speed100 = %d, duplexfull = %d", 
                                        stat[1].linkisup, 
                                        stat[1].autoNeg_done, 
                                        stat[1].linkisgood, 
                                        stat[1].linkspeed, 
                                        stat[1].linkduplex);
        //hal_trace_puts(str);  
    } 
    else
    {
        hal_trace_puts("hal_eth_get_links_status() fails");  
    }    
}

static void s_specialise_blink(void *param)
{
    static uint8_t pos = 0;
    
    switch(pos)
    {
        case 0: hal_led_toggle(hal_led1); break;
        case 1: hal_led_toggle(hal_led2); break;
        case 2: hal_led_toggle(hal_led3); break;
        //case 3: hal_led_toggle(hal_led3); break;
        //case 4: hal_led_toggle(hal_led4); break;
        //case 5: hal_led_toggle(hal_led5); break;
    }
    
    pos = (pos+1)%3;
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



