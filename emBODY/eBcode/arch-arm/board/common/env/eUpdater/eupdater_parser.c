/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Alessandro Scalzo
 * email:   marco.accame@iit.it, alessandro.scalzo@iit.it
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

/* @file       eupdater-parser.c
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

#include "stdlib.h"

#include "updater-core.h"

#include "eupdater_cangtw.h"

#include "osal_system.h"
#include "EOtimer.h"
#include "EOaction.h"
#include "eEsharedServices.h"
#include "EOMtheCallbackmanager.h"

#include "hal.h"

#include "hal_5v.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eupdater_parser.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    eOnanotime_t    client_tx_time;
    eOnanotime_t    server_tx_time;
    uint32_t        client_id;
} pkt_payload_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//static eObool_t s_eupdater_parser_process_ethcmd_dummy(EOpacket *rxpkt, EOpacket *txpkt);
static eObool_t s_eupdater_parser_process_ethcmd_alessandro(EOpacket *rxpkt, EOpacket *txpkt);

static void s_eupdater_jump2app(void *par);
static void s_toggle_led(void *p);


static void s_jump2app_countdown_start(eOreltime_t countdown);
static eObool_t s_jump2app_countdown_isrunning(void);
static void s_jump2app_countdown_stop(void);

static void s_led_countdown_start(void);
static void s_led_countdown_stop(void);

static void s_stayforever(eOipv4addr_t remaddress);

static void s_led_stayforever_start(void);


static void s_toggle_led_cangtw(void *p);

static void s_led_cangtw_start(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtimer* s_timer_jump2app = NULL;
static EOaction *s_action_jump2app = NULL;
static eObool_t s_isrunning_jump2app = eobool_false;

static EOtimer* s_timer_led = NULL;
static EOaction *s_action_led = NULL;


static eObool_t s_stay_in_here_forever = eobool_false;

#if defined(_DEBUG_MODE_FULL_)
    #warning --> we are in _DEBUG_MODE_FULL_ and use: mostrecent_host_ipaddress = EO_COMMON_IPV4ADDR(10, 255, 72, 114)
    static eOipv4addr_t mostrecent_host_ipaddress = EO_COMMON_IPV4ADDR(10, 255, 72, 114);
#else
    static eOipv4addr_t mostrecent_host_ipaddress = EO_COMMON_IPV4ADDR_LOCALHOST;
#endif



static uint8_t s_cangtw_is_active = 0;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// evaluates if to stay in updated or to jump to application now or after a timeout
extern void eupdater_parser_evalwhenjumping(void)
{
    eOreltime_t time2stay = 0;
    eEprocess_t pr = ee_procNone;
    
    s_stay_in_here_forever = eobool_false;
    
#if !defined(_MAINTAINER_APPL_ )    

//    uncomment only if you want to debug    
//    ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
    
    // if the loader has set the goto and it is updater ... then we stay in here forever
    if(ee_res_OK == ee_sharserv_ipc_gotoproc_get(&pr))
    {
        ee_sharserv_ipc_gotoproc_clr();
        
        if(ee_procUpdater == pr)
        {   // stay in this process forever
            s_stayforever(mostrecent_host_ipaddress);
            return;
        }
    }    
    
    // then we retrieve how long we have to stay in here   
    ee_sharserv_part_proc_startuptime_get(&time2stay);
    
//    uncomment one of the two only if you want to debug      
//    time2stay = EOK_reltimeINFINITE;
//    time2stay = EOK_reltimeZERO;
    
    if(EOK_reltimeZERO == time2stay)
    {
        // if zero time ... then we immediately jump to default application
        s_eupdater_jump2app(NULL);
    }
    else if(EOK_reltimeINFINITE == time2stay)
    {
        s_stayforever(mostrecent_host_ipaddress);
        return;
    }
    else
    {
        // otherwise start a jump2app timer
        s_jump2app_countdown_start(time2stay);
        
        // and also start the countdown led
        s_led_countdown_start();
    }

#elif defined(_MAINTAINER_APPL_)
 
    // the maintainer just stays in here forever. thus init the stayforever leds    
    s_stayforever(mostrecent_host_ipaddress);
    
#endif    
}



// return true if there is a pkt to transmit back
extern eObool_t eupdater_parser_process_ethcmd(EOpacket *rxpkt, EOpacket *txpkt)
{
    //return(s_eupdater_parser_process_ethcmd_dummy(rxpkt, txpkt));
    return(s_eupdater_parser_process_ethcmd_alessandro(rxpkt, txpkt));
}





extern void eupdater_parser_download_blinkled_start(void)
{
    eo_timer_Stop(s_timer_led);
    
//     if(NULL == s_timer_led)
//     {
//         s_timer_led = eo_timer_New();
//     }

//     if(NULL == s_action_led)
//     {
//         s_action_led = eo_action_New();
//     }
//     
//     eo_action_SetCallback(s_action_led, s_toggle_led, NULL, eom_callbackman_GetTask(eom_callbackman_GetHandle()));
//     eo_timer_Start(s_timer_led, eok_abstimeNOW, 100*1000, eo_tmrmode_FOREVER, s_action_led);
}


extern void eupdater_parser_download_blinkled_stop(void)
{
//    eo_timer_Stop(s_timer_led);
    
    s_led_stayforever_start();
}

extern void eupdater_parser_download_toggleled(void)
{
    s_toggle_led(NULL);
}

extern eObool_t eupdater_parser_stay_forever(void)
{
    return(s_stay_in_here_forever);
}

extern void eupdater_parser_cangtw_activate(void)
{
    s_cangtw_is_active = 1;
    s_cangtw_is_active = s_cangtw_is_active;
    
    s_led_cangtw_start();
}

extern eObool_t eupdater_parser_cangtw_isactivated(void)
{
    return(s_cangtw_is_active);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_eupdater_parser_process_ethcmd_alessandro(EOpacket *rxpkt, EOpacket *txpkt)
{
    uint8_t *datarx;
    uint8_t *datatx;
    uint16_t sizerx;
    uint16_t sizetx;
    uint16_t capacitytx;

    eOipv4addr_t remaddr = 0;
    eOipv4port_t remport = 0;
    
    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);
    
    mostrecent_host_ipaddress = remaddr;
    
    // if we receive any packet from the correct socket, then we stop the countdown
    if(eobool_true == s_jump2app_countdown_isrunning())
    {
        s_jump2app_countdown_stop();
        
        // and also we change the led frequency
        s_led_countdown_stop();
        s_stayforever(mostrecent_host_ipaddress);
    }

    eo_packet_Payload_Get(rxpkt, &datarx, &sizerx);
    eo_packet_Payload_Get(txpkt, &datatx, &sizetx);
    eo_packet_Capacity_Get(txpkt, &capacitytx);

    if(1 == upd_core_manage_cmd(datarx, sizerx, remaddr, datatx, capacitytx, &sizetx))
    {
        eo_packet_Payload_Set(txpkt, (uint8_t*)datatx, sizetx);
        //eo_packet_Destination_Set(txpkt, remaddr, remport);
        eo_packet_Destination_Set(txpkt, remaddr, 3333);

        return(eobool_true);
    }

    return(eobool_false);

}

// static eObool_t s_eupdater_parser_process_ethcmd_dummy(EOpacket *rxpkt, EOpacket *txpkt)
// {
//     pkt_payload_t *pktdata = NULL;
//     uint8_t *data;
//     uint16_t size;
//     eOnanotime_t nano = 0;

//     eOipv4addr_t remaddr = 0;
//     eOipv4port_t remport = 0;

//     eo_packet_Destination_Get(rxpkt, &remaddr, &remport);


//     eo_packet_Payload_Get(rxpkt, &data, &size);
//     //ipaddr = ((uint8_t*)&remaddr);
//     pktdata = (pkt_payload_t*)data;


//     // prepare tx packet
//     pktdata->client_id = 0x10101010;
//     eov_sys_NanoTimeGet(eov_sys_GetHandle(), &nano); 

//     pktdata->client_tx_time = nano;

//     eo_packet_Payload_Set(txpkt, (uint8_t*)pktdata, sizeof(pkt_payload_t));

//     eo_packet_Destination_Set(txpkt, remaddr, 3333);

//     return(eobool_true);
// }

static void s_eupdater_jump2app(void *par)
{
    // retrieve the default process
    eEprocess_t defproc = ee_procNone;
    eObool_t attemptjump = eobool_false;
    
    // we dont need a led countdown anymore
    s_led_countdown_stop();
    

    if(ee_res_OK == ee_sharserv_part_proc_def2run_get(&defproc))
    {

//      uncomment only if you want to debug         
//        defproc = ee_procUpdater;
        
        // we go to the default process only if it is not the updater ... else we stay in here
        if(ee_procUpdater != defproc)
        {
            attemptjump = eobool_true;

        }
        else
        {
            attemptjump = eobool_false;        
        }
    }

    if(eobool_true == attemptjump)
    {
        // we dont need to verify if the system can jump to defproc because eloader shall do it.
        //osal_system_scheduling_suspend();
        ee_sharserv_ipc_gotoproc_set(defproc);
        ee_sharserv_sys_restart();        
    }
    
    // if in here ... we stay in eupdater forever

    s_stayforever(mostrecent_host_ipaddress);      
}

static void s_jump2app_countdown_start(eOreltime_t countdown)
{
    if(NULL == s_timer_jump2app)
    {
        s_timer_jump2app = eo_timer_New();
    }
    
    if(NULL == s_action_jump2app)
    {
        
        s_action_jump2app = eo_action_New();
    }
    
    eo_action_SetCallback(s_action_jump2app, 
                          s_eupdater_jump2app, NULL,
                          eom_callbackman_GetTask(eom_callbackman_GetHandle())
                         );
    
    eo_timer_Start(s_timer_jump2app, eok_abstimeNOW, countdown, eo_tmrmode_ONESHOT, s_action_jump2app);
    
    s_isrunning_jump2app = eobool_true;
} 

static eObool_t s_jump2app_countdown_isrunning(void)
{
    return(s_isrunning_jump2app);
}


static void s_jump2app_countdown_stop(void)
{
    eo_timer_Stop(s_timer_jump2app);
    s_isrunning_jump2app = eobool_false;
}


static void s_led_countdown_start(void)
{
    if(NULL == s_timer_led)
    {
        s_timer_led = eo_timer_New();
    }
    
    if(NULL == s_action_led)
    {
        s_action_led = eo_action_New();
    }
    
    eo_action_SetCallback(s_action_led, s_toggle_led, NULL, eom_callbackman_GetTask(eom_callbackman_GetHandle()));
    eo_timer_Start(s_timer_led, eok_abstimeNOW, 500*1000, eo_tmrmode_FOREVER, s_action_led);
}

static void s_led_countdown_stop(void)
{
    eo_timer_Stop(s_timer_led);
}

static void s_stayforever(eOipv4addr_t remaddress)
{
    s_led_stayforever_start();    
    s_stay_in_here_forever = eobool_true;       
#if !defined(_MAINTAINER_APPL_)
#ifdef _START_CANGTW_WHEN_STAY_FOREVER_    
    // if updater we also start the can gateway
    eupdater_cangtw_start(remaddress);
#endif  
    // i init the 5v0 as soon as i know i stay in here forever.
    // BUT it dont init the can so that things are not changed with respect to prev version
    hal_5v_init(NULL);
    hal_5v_on();
#endif       
}

static void s_led_stayforever_start(void)
{
    if(NULL == s_timer_led)
    {
        s_timer_led = eo_timer_New();
    }

    if(NULL == s_action_led)
    {
        s_action_led = eo_action_New();
    }
    
    eo_action_SetCallback(s_action_led, s_toggle_led, NULL, eom_callbackman_GetTask(eom_callbackman_GetHandle()));
    eo_timer_Start(s_timer_led, eok_abstimeNOW, 1000*1000, eo_tmrmode_FOREVER, s_action_led);
}



static void s_toggle_led(void *p)
{
#if !defined(_MAINTAINER_APPL_) 
    
    //hal_led_toggle(hal_led0);
    hal_led_toggle(hal_led1);
    hal_led_toggle(hal_led2);
    hal_led_toggle(hal_led3);
    hal_led_toggle(hal_led4);
    hal_led_toggle(hal_led5);

#else
    static uint8_t num = 0;
    
    switch(num)
    {
        case 0:     /*hal_led_toggle(hal_led0);*/ hal_led_toggle(hal_led1);     break; 
        case 1:     hal_led_toggle(hal_led2); hal_led_toggle(hal_led3);     break;   
        case 2:     hal_led_toggle(hal_led4); hal_led_toggle(hal_led5);     break;        
    }
    
    num = (num+1)%3;

#endif    
}

static void s_toggle_led_cangtw(void *p)
{  
    //hal_led_toggle(hal_led0);
    hal_led_toggle(hal_led1);
    hal_led_toggle(hal_led2);
    hal_led_toggle(hal_led3);
}


static void s_led_cangtw_start(void)
{
    if(NULL == s_timer_led)
    {
        s_timer_led = eo_timer_New();
    }
    
    if(NULL == s_action_led)
    {
        s_action_led = eo_action_New();
    }
    
    eo_timer_Stop(s_timer_led);
    
    //hal_led_off(hal_led0);
    hal_led_off(hal_led1);
    hal_led_off(hal_led2);
    hal_led_off(hal_led3);    
    hal_led_off(hal_led4);
    hal_led_off(hal_led5);
    
    eo_action_SetCallback(s_action_led, s_toggle_led_cangtw, NULL, eom_callbackman_GetTask(eom_callbackman_GetHandle()));
    eo_timer_Start(s_timer_led, eok_abstimeNOW, 2000*1000, eo_tmrmode_FOREVER, s_action_led);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



