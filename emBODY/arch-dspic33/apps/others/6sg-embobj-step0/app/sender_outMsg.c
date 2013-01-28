/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/* @file       sender_outMsg.h
    @brief      this file implements the sender entity.
    @author     valentina.gaggero@iit.it
    @date       07/14/2011
*/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <string.h>


#ifdef _TIMER_HW_
     #include "hal_timer.h"  
#else
    #include "EOtimer.h"
    #include "EOaction.h"
#endif

    #include "EOStheFOOP.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "sender_outMSG.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define EVT_SENDERTMR_TIMEOUT       (0x1<<0)  // evt pos is 1 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

extern void send_data(void);

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    SIXsg_config_data_t *SIXsg_config_ptr;
} sender_data_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#ifdef _TIMER_HW_
static void s_sender_tmr_hw_init(void);
static void s_sender_tmr_hw_callbkp(void *arg);
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#ifndef _TIMER_HW_
static EOaction *s_action_ptr = NULL;
static EOtimer *s_timer_ptr = NULL;
#endif
static sender_data_t sender_data;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t sender_init(SIXsg_config_data_t *cfg_ptr)
{
    if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    sender_data.SIXsg_config_ptr = cfg_ptr;


#ifdef _TIMER_HW_
    s_sender_tmr_hw_init();
#else
    s_action_ptr = eo_action_New();
    s_timer_ptr = eo_timer_New();
    eo_action_SetEvent(s_action_ptr, EVT_SENDERTMR_TIMEOUT, eos_foop_GetHandle());
#endif
   
    return(hal_res_OK);

}

extern void sender_start(void)
{

    //TODO: verifica che se la funz viene invcata con il timer gia attivo non ci siano problemi
    if( tx_outMsg_off != sender_data.SIXsg_config_ptr->behaviour_cfg.tx_outMsg_mode)
    { 

#ifdef _TIMER_HW_
        hal_timer_start(hal_timer4);
#else
        eo_timer_Start(s_timer_ptr, eok_abstimeNOW, (sender_data.SIXsg_config_ptr->SIXsg_ee_data.can_msg_datarate*1000),
                       eo_tmrmode_FOREVER, s_action_ptr);
#endif
    }

}

extern void sender_stop(void)
{
#ifdef _TIMER_HW_
    hal_timer_stop(hal_timer4);
#else
    eo_timer_Stop(s_timer_ptr);
#endif
}

extern int16_t sender_is_running(void)
{

#ifdef _TIMER_HW_
    return(1); //TODO
#else
    if( eo_tmrstat_Idle == eo_timer_GetStatus(s_timer_ptr) )
    {
        return(0);
    }
    else
    {
        return(1);
    }
#endif

}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#ifdef _TIMER_HW_
static void s_sender_tmr_hw_init(void)
{

    hal_timer_cfg_t tmr_cfg = 
    {
        .prescaler = hal_timer_prescaler64,         
        .countdown = (sender_data.SIXsg_config_ptr->SIXsg_ee_data.can_msg_datarate*1000),
        .priority = hal_int_priority06,        
        .mode = hal_timer_mode_periodic,
        .callback_on_exp = s_sender_tmr_hw_callbkp,
        .arg = NULL 
    };

    hal_timer_init(hal_timer4, &tmr_cfg, NULL);
    hal_timer_offset_write(hal_timer4, 0);
    hal_timer_interrupt_enable(hal_timer4);

}
#endif

#ifdef _TIMER_HW_
static void s_sender_tmr_hw_callbkp(void *arg)
{
    eos_foop_SetEvent(eos_foop_GetHandle(), EVT_SENDERTMR_TIMEOUT);
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



