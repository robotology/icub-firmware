/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       eom_applipnet_specialise.c
	@brief      This file keeps the vcececew
	@author     marco.accame@iit.it
    @date       01/11/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

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



#include "eventviewer.h"

#include "EOtimer.h"
#include "EOaction.h"
#include "EOMtheCallbackmanager.h"
#include "EOMtheIPnet.h"

#include "EOtheLEDpulser.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eom_applipnet_specialise.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

extern void task_udpserver(void *p);



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// if not echoer, then it is a-symmetric
#define USE_RECEIVER_ECHOER
//#define DONT_PULSE_LEDS



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_udpserver_startup(EOMtask *p, uint32_t t);

static void s_udpserver_run(EOMtask *p, uint32_t t);


static void s_eom_applipnet_specialise_transceiver_init(void);

#if defined(USE_RECEIVER_ECHOER)
static void s_eom_applipnet_specialise_echoer_receive(EOpacket* rxpkt);
#else
static void s_eom_applipnet_specialise_asimm_receive(EOpacket* rxpkt);
#endif

static void s_eom_applipnet_specialise_ipnet_start(void);

static void s_eom_applipnet_specialise_updserver_start(void);

static void s_eom_applipnet_specialise_onpktreceived_set( void (*cbk)(EOpacket*) );

static void s_eom_applipnet_specialise_leds(void);

static void s_eom_applipnet_specialise_transmit(EOpacket *txpkt);

static eObool_t s_eom_applipnet_specialise_connect(eOipv4addr_t remotehostaddr);


void getfromEOsocket(void)
{

}

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOpacket* s_mytxpkt = NULL;


static EOsocketDatagram*        s_skt_rops          = NULL;
static EOpacket*                s_rxpkt             = NULL;
static EOpacket*                s_txpkt             = NULL;
static EOMtask*                 s_task_udpserver    = NULL;


static void (*s_eom_applipnet_specialise_on_ropframe_received)(EOpacket*) = NULL;


static const eOevent_t s_event_from_skt_rops            = 0x00000004;

static const eOipv4port_t s_server_port                 = 33333; 

static volatile eObool_t s_host_connected               = eobool_false;

static eOipv4addr_t s_host_ipaddress                    = 0;

static const uint32_t s_max_packet_size                     = 512;


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern const ipal_cfg_t    ipal_cfg;

extern void eom_applipnet_specialise_system(void)
{

    // -- service 1:    init the leds.
    //                  led0, led1, led2 with the LEDpulser. led3, led4, and led5 manually
    //                  led0 will blink forever at 1Hz
    //                  led1 will pulse 5 times at 5Hz at pkt reception
    //                  led2 will pulse 10 times at 10Hz at pkt transmission
    
    s_eom_applipnet_specialise_leds();
    
    
    // -- service 2:    starts the ipnet.
    //                  it has two internal tasks: one periodic and the other one triggered by network events or tx requests
    
    s_eom_applipnet_specialise_ipnet_start();


    // -- service 3:    starts an event-driven task-upd-server. 
    //                  it which starts the listening socket and manages reception of packets.

    s_eom_applipnet_specialise_updserver_start();  

}



static void s_eom_applipnet_specialise_onpktreceived_set( void (*cbk)(EOpacket*)  )
{
    s_eom_applipnet_specialise_on_ropframe_received = cbk;
}

static void s_eom_applipnet_specialise_leds(void)
{
   
    // init the ledpulser with leds 0 and 1 and 2
    
    eOledpulser_cfg_t ledpulsercfg = 
    {
        .led_enable_mask    = (1 << eo_ledpulser_led_zero) | (1 << eo_ledpulser_led_one) | (1 << eo_ledpulser_led_two),
        .led_init           = (eOint8_fp_uint8_cvoidp_t)hal_led_init,
        .led_on             = (eOint8_fp_uint8_t)hal_led_on,
        .led_off            = (eOint8_fp_uint8_t)hal_led_off,
        .led_toggle         = (eOint8_fp_uint8_t)hal_led_toggle
    };
    
    // each active led will have at timer. we can set it on, off, pulse. 
    eo_ledpulser_Initialise(&ledpulsercfg);  

#if !defined(DONT_PULSE_LEDS)    
    // start a pulse on led0: infinite with 1 Hz period
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, 1*1000*1000, 0);
#endif    
    // led1 will be driven by the receiver 
    // led2 will be driven by teh transmitter
           
    // init the other leds
    hal_led_init(hal_led3, NULL);
    hal_led_init(hal_led4, NULL);
    hal_led_init(hal_led5, NULL);
    hal_led_init(hal_led6, NULL);
    hal_led_init(hal_led7, NULL);
               
}


static void s_eom_applipnet_specialise_transmit(EOpacket *txpkt)
{

   // copy the pkt pointer
    s_txpkt = txpkt;

    eo_packet_Addressing_Set(s_txpkt, s_host_ipaddress, s_server_port);

    eo_socketdtg_Put(s_skt_rops, s_txpkt);

}


static eObool_t s_eom_applipnet_specialise_connect(eOipv4addr_t remotehostaddr)
{
    if((eobool_false == s_host_connected) || (remotehostaddr != s_host_ipaddress))
    {
        // attempt connection for 1 second. no more. 
        if(eores_OK == eo_socketdtg_Connect(s_skt_rops, remotehostaddr, eok_reltime1sec))
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

    return(s_host_connected);
}


static void s_eom_applipnet_specialise_ipnet_start(void)
{
    const ipal_cfg_t* used_ipal_cfg = &ipal_cfg;
    
    const eOmipnet_cfg_dtgskt_t used_dtgskt_cfg = 
    {   
        .numberofsockets            = 2,        // max 2 sockets in IPnet 
        .maxdatagramenqueuedintx    = 2         // max 2 datagrams in tx queue of IPnet
    };
 

    eom_ipnet_Initialise(&eom_ipnet_DefaultCfg,     // the properties of the two tasks of IPnet
                         used_ipal_cfg,             // the ipal config
                         NULL,                      // the ip/mac addresses. if NULL ... they are taken from ipal_cfg
                         &used_dtgskt_cfg           // the datagrams   
                         );

}


static void s_eom_applipnet_specialise_updserver_start(void)
{
    s_host_connected = eobool_false;

    // the function s_udpserver_startup() is executed only one and ... starts a listening socket
    // which will send and event to the task if a new packet arrives.
    // the function s_udpserver_run() is triggered when an event arrives from the attached socket
     s_task_udpserver = eom_task_New(eom_mtask_EventDriven, 100, 2*1024, s_udpserver_startup, s_udpserver_run,  6, 
                                    eok_reltimeINFINITE, NULL, 
                                    task_udpserver, "udpserver");

    s_task_udpserver = s_task_udpserver;

}  



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern void task_udpserver(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





static void s_udpserver_startup(EOMtask *p, uint32_t t)
{
    EOaction_strg s_fullaction_stg;
    EOaction* s_fullaction = (EOaction*)s_fullaction_stg;


    // init the rx packet 
    s_rxpkt = eo_packet_New(s_max_packet_size);  
    
 

    // initialise the socket 
    s_skt_rops = eo_socketdtg_New(  2, s_max_packet_size, eom_mutex_New(), // input queue
                                    2, s_max_packet_size, eom_mutex_New()  // output queue
                                 );

    // set the rx action on socket to be a message s_message_from_skt_rops to this task object
    eo_action_SetEvent(s_fullaction, s_event_from_skt_rops, p);
    eo_socketdtg_Open(s_skt_rops, s_server_port, eo_sktdir_TXRX, eobool_false, NULL, s_fullaction, NULL);
    
    // init the transceiver
    s_eom_applipnet_specialise_transceiver_init();

    //eventviewer_load(ev_ID_first_usrdef+0, getfromEOsocket);
        
}


static void s_udpserver_run(EOMtask *p, uint32_t t)
{
    eOresult_t res;
    uint16_t numberof = 0;

    // the event that we have received
    eOevent_t evt = (eOevent_t)t;

    if(eobool_true == eo_common_event_check(evt, s_event_from_skt_rops))
    {
        //evEntityId_t prev = eventviewer_switch_to(ev_ID_first_usrdef+0);
        // retrieve the pkt.
        res = eo_socketdtg_Get(s_skt_rops, s_rxpkt, eok_reltimeINFINITE);

        //eventviewer_switch_to(prev);
#if !defined(DONT_PULSE_LEDS)   
        eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_one, 200*1000, 5);
#endif
        // call a function for it
        if((eores_OK == res) && (NULL != s_eom_applipnet_specialise_on_ropframe_received))
        {
            s_eom_applipnet_specialise_on_ropframe_received(s_rxpkt);
        }

        // if any other pkt in socket then send event again
        res = eo_socketdtg_Received_NumberOf(s_skt_rops, &numberof);
        if((0 != numberof) && (eores_OK == res))
        {
            eom_task_SetEvent(p, s_event_from_skt_rops);
        }
    }

}


static void s_eom_applipnet_specialise_transceiver_init(void)
{
    
    // init transceiver
    s_mytxpkt = eo_packet_New(s_max_packet_size);

    eo_packet_Size_Set(s_mytxpkt, s_max_packet_size);
        
    // impose that on rx pkt the task udp calls ...

#if defined(USE_RECEIVER_ECHOER)    
    s_eom_applipnet_specialise_onpktreceived_set(s_eom_applipnet_specialise_echoer_receive);
#else    
    s_eom_applipnet_specialise_onpktreceived_set(s_eom_applipnet_specialise_asimm_receive);    
#endif
    
}


#if defined(USE_RECEIVER_ECHOER)
static void s_eom_applipnet_specialise_echoer_receive(EOpacket* rxpkt)
{
    eOipv4addr_t remaddr;
    eOipv4port_t remport;

    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);

    // at first attempt to connect.

    if(eobool_false == s_eom_applipnet_specialise_connect(remaddr))
    {
        return;
    }


    // then send back teh very same pkt

    s_eom_applipnet_specialise_transmit(rxpkt);
    
#if !defined(DONT_PULSE_LEDS)       
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_two, 100*1000, 10);
#endif
}
#else
static void s_eom_applipnet_specialise_asimm_receive(EOpacket* rxpkt)
{
    eOipv4addr_t remaddr;
    eOipv4port_t remport;

    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);

    // at first attempt to connect.

    if(eobool_false == s_eom_applipnet_specialise_connect(remaddr))
    {
        return;
    }


    // then send back the full pkt

    s_eom_applipnet_specialise_transmit(s_mytxpkt);

#if !defined(DONT_PULSE_LEDS)       
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_two, 100*1000, 10);
#endif
}
#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



