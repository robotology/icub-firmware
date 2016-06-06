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

/* @file       eupdater_cangtw.c
	@brief      This file keeps the can gateway
	@author     marco.accame@iit.it
    @date       08/23/2013
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

// abslayer 
#include "hal.h"
#include "hal_trace.h"
#include "osal_system.h"



#include "EoCommon.h"
#include "EOtheErrormanager.h"
#include "EOMtask.h"

#include "osal_semaphore.h"

#include "EOaction.h"
#include "EOpacket.h"
#include "EOMmutex.h"
#include "EOsocketDatagram.h"
#include "EOtimer.h"
#include "EOMtheCallbackmanager.h"

// sm
#include "eOcfg_sm_CanGtw.h"


#include "eupdater-info.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eupdater_cangtw.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eupdater_cangtw_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

EOMtask*                 eupdater_task_cangateway           = NULL;
EOsocketDatagram*        eupdater_sock_cangateway           = NULL;



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_cangateway_startup(EOMtask *p, uint32_t t);
static void s_cangateway_run(EOMtask *p, uint32_t t);

static void s_isr_alert_cangtwtask_can1(void* p);
static void s_isr_alert_cangtwtask_can2(void* p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOipv4port_t       s_cangtw_port               = 3334; 

static eOipv4addr_t             s_cangtw_remaddress         = 0;

static uint8_t s_status = 0;

static EOsm*                    s_sm_cangtw                 = NULL;

static osal_semaphore_t *startup_done_semaphore = NULL; 



static const cangtw_parameters_t default_params =
{
    .t_can_stabilisation    = 900,
    .t_wait_ff_reply        = 0,
    .send_ff                = eobool_false,
    .clear_can_onentry_gtw  = eobool_false,
    .send_ack               = eobool_false    
};


static cangtw_parameters_t eupdater_cantw_parameters = {0};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOresult_t eupdater_cangtw_block_until_startup(void)
{
    if(NULL == startup_done_semaphore)
    {
        return(eores_NOK_generic);
    }
    // wait w/out timeout until s_cangateway_startup() has incremented it.  
    osal_semaphore_decrement(startup_done_semaphore, osal_reltimeINFINITE); 
    // increment it again so that next time you call it we dont stop forever
    osal_semaphore_increment(startup_done_semaphore, osal_callerTSK);

    return(eores_OK);    
}

extern const cangtw_parameters_t * eupdater_cangtw_get_parameters(void)
{
    return(&eupdater_cantw_parameters);
}

extern void eupdater_cangtw_init(void)
{
    if(0 != s_status)
    {
        return;
    }
    
    memcpy(&eupdater_cantw_parameters, &default_params, sizeof(eupdater_cantw_parameters));
    
    startup_done_semaphore = osal_semaphore_new(2, 0); // but as it is used as a binary sempahore, 1 would be ok. what is important is the second arg: 0
    
    // init the gtw task
    eupdater_task_cangateway = eom_task_New(eom_mtask_EventDriven, 100, 2*1024, s_cangateway_startup, s_cangateway_run,  32, 
                                    eok_reltimeINFINITE, NULL, 
                                    task_cangateway, "cangateway");    
    
    s_status = 1;    
}

extern void eupdater_cangtw_start(eOipv4addr_t remipaddr, const cangtw_parameters_t *params)
{     
    if(NULL == params)
    {
       params = &default_params;      
    }
    
    memcpy(&eupdater_cantw_parameters, params, sizeof(eupdater_cantw_parameters)); 
    
    if(1 != s_status)
    {
        // already started ... we force an enter run again ...
        eom_task_SetEvent(eupdater_task_cangateway, event_cangtw_go2run);     
        return;
    }    
    
    
    // eupdater_info_trace(NULL, "sending event_cangtw_start = %d", event_cangtw_start);

    eupdater_cangtw_set_remote_addr(remipaddr);
    
    // just emit the start event. the sm embedded in the can gateway task shall process it
    eom_task_SetEvent(eupdater_task_cangateway, event_cangtw_start);
    
    
    s_status = 2;
}

extern eOipv4addr_t eupdater_cangtw_get_remote_addr(void)
{
    return(s_cangtw_remaddress);
}

extern void eupdater_cangtw_set_remote_addr(eOipv4addr_t remaddr)
{
    s_cangtw_remaddress = remaddr;
}

extern eOipv4port_t eupdater_cangtw_get_remote_port(void)
{
    return(s_cangtw_port);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void task_cangateway(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
}  

//#if     defined(HAL_USE_VERSION_2) || defined(HAL_IS_VERSION_2)
static void errcallback(void* p) {};
//#endif
extern void cangateway_hid_hal_init(void)
{  
    static uint8_t can_initted = 0;
    
    if(1 == can_initted)
    {
        return;
    }
    
    
    can_initted = 1;
    
//#if     defined(HAL_USE_VERSION_2) || defined(HAL_IS_VERSION_2)
    hal_can_cfg_t canxcfg = 
    {
        .runmode                    = hal_can_runmode_isr_1txq1rxq, 
        .baudrate                   = hal_can_baudrate_1mbps, 
        .priorx                     = hal_int_priorityNONE,
        .priotx                     = hal_int_priorityNONE,
        .capacityofrxfifoofframes   = 16,
        .capacityoftxfifoofframes   = 16,
        .capacityoftxfifohighprio   = 2,
        .callback_on_rx             = NULL,
        .arg_cb_rx                  = NULL,
        .callback_on_tx             = NULL,
        .arg_cb_tx                  = NULL,
        .callback_on_err            = errcallback,
        .arg_cb_err                 = NULL
    };   

    // init can1
    canxcfg.priorx          = hal_int_priority07;
    canxcfg.priotx          = hal_int_priority08;
    canxcfg.callback_on_rx  = s_isr_alert_cangtwtask_can1;    
    hal_can_init(hal_can_port1, &canxcfg);
    
    // init can2
    canxcfg.priorx          = hal_int_priority09;
    canxcfg.priotx          = hal_int_priority10;
    canxcfg.callback_on_rx  = s_isr_alert_cangtwtask_can2;     
    hal_can_init(hal_can_port2, &canxcfg);   
//#else    
//    hal_can_cfg_t canxcfg = 
//    {
//        .runmode        = hal_can_runmode_isr_1txq1rxq, 
//        .baudrate       = hal_can_baudrate_1mbps, 
//        .priorx         = hal_int_priorityNONE,
//        .priotx         = hal_int_priorityNONE,
//        .callback_on_rx = NULL,
//        .arg_cb_rx      = NULL,
//        .callback_on_tx = NULL,
//        .arg_cb_tx      = NULL
//    };   
//
//    // init can1
//    canxcfg.priorx          = hal_int_priority07;
//    canxcfg.priotx          = hal_int_priority08;
//    canxcfg.callback_on_rx  = s_isr_alert_cangtwtask_can1;    
//    hal_can_init(hal_can_port1, &canxcfg);
//    
//    // init can2
//    canxcfg.priorx          = hal_int_priority09;
//    canxcfg.priotx          = hal_int_priority10;
//    canxcfg.callback_on_rx  = s_isr_alert_cangtwtask_can2;     
//    hal_can_init(hal_can_port2, &canxcfg);   
//#endif    
}


extern void cangateway_hid_hal_enable(void)
{
    // enable can1 and can2 
    hal_can_enable(hal_can_port1);
    hal_can_enable(hal_can_port2);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_cangateway_startup(EOMtask *p, uint32_t t)
{

    // initialise the socket
    // up to 16 packets of 64 bytes ...    
    eupdater_sock_cangateway = eo_socketdtg_New (   16, eupdater_cangtw_udp_packet_maxsize, eom_mutex_New(), // input queue
                                                    16, eupdater_cangtw_udp_packet_maxsize, eom_mutex_New()  // output queue
                                                );   
    
    eupdater_info_trace("canGTW", "opening a txrx socket on port %d", s_cangtw_port);

    EOaction_strg action_strg;
    EOaction* act = (EOaction*)&action_strg;  
    // set the rx action on socket to be an event s_event_cangtw_sock_rec to this task object
    eo_action_SetEvent(act, event_cangtw_sock_rec, p);
    eo_socketdtg_Open(eupdater_sock_cangateway, s_cangtw_port, eo_sktdir_TXRX, eobool_false, NULL, act, NULL);
    
    // init and start the sm. also pass to it the task and teh socket 
    s_sm_cangtw = eo_sm_New(eo_cfg_sm_cangtw_Get());
    eo_sm_Start(s_sm_cangtw);
    eOsmDynamicDataCanGtw_t* smdata = (eOsmDynamicDataCanGtw_t*) eo_sm_GetDynamicData(s_sm_cangtw); 
    smdata->taskCANgtw = eupdater_task_cangateway;
    smdata->sockCANgtw = eupdater_sock_cangateway; 

    // ok, now the startup is over.
    osal_semaphore_increment(startup_done_semaphore, osal_callerTSK);
}

static void s_cangateway_run(EOMtask *p, uint32_t t)
{
    eOevent_t evt = (eOevent_t) t;
    
#if defined(_DEBUG_MODE_FULL_)    
    eupdater_info_trace(NULL, "received event %d", evt);    
#endif    
    
    if(eobool_true == eo_common_event_check(evt, event_cangtw_start))
    {
        eo_sm_ProcessEvent(s_sm_cangtw, eo_sm_cangtw_evstart);        
    }

    if(eobool_true == eo_common_event_check(evt, event_cangtw_canstable))
    {
        eo_sm_ProcessEvent(s_sm_cangtw, eo_sm_cangtw_evcanstable);        
    }    
    
    if(eobool_true == eo_common_event_check(evt, event_cangtw_go2run))
    {
        eo_sm_ProcessEvent(s_sm_cangtw, eo_sm_cangtw_evgo2run);        
    }
    
    if(eobool_true == eo_common_event_check(evt, event_cangtw_sock_rec))
    {
        eo_sm_ProcessEvent(s_sm_cangtw, eo_sm_cangtw_evrxeth);        
    }
    
    if(eobool_true == eo_common_event_check(evt, event_cangtw_can1_rec))
    {
        eo_sm_ProcessEvent(s_sm_cangtw, eo_sm_cangtw_evrxcan1);        
    }    
    
    if(eobool_true == eo_common_event_check(evt, event_cangtw_can2_rec))
    {
        eo_sm_ProcessEvent(s_sm_cangtw, eo_sm_cangtw_evrxcan2);        
    }  
  
}

static void s_isr_alert_cangtwtask_can1(void* p)
{
    eom_task_isrSetEvent(eupdater_task_cangateway, event_cangtw_can1_rec);   
}

static void s_isr_alert_cangtwtask_can2(void* p)
{
    eom_task_isrSetEvent(eupdater_task_cangateway, event_cangtw_can2_rec);   
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



