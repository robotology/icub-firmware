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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheErrorManager.h"

#include "EOMtask.h"
#include "EOMmutex.h"
#include "EOMtheCallbackManager_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheLEDpulser.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheLEDpulser_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOmledpulser_cfg_t eom_ledpulser_DefaultCfg = 
{ 
    .numberofleds   = 1, 
    .leds           = { hal_led0 }
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_ledpulser_callback(void* p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheLEDpulser";

static EOMtheLEDpulser s_mledpulser = 
{
    .config         = 
    {
        .numberofleds   = 1, 
        .leds           = { hal_led0 }
    },
    .action         = NULL,
    .timer          = {NULL},
    .ticks          = {0}     
}; 



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheLEDpulser * eom_ledpulser_Initialise(const eOmledpulser_cfg_t *ledpulsercfg) 
{
    // already initialised ?
    if(NULL != s_mledpulser.timer[0]) 
    {
        // already initialised !
        return(&s_mledpulser);
    }
    
 
    if(NULL == ledpulsercfg)
    {
        ledpulsercfg = &eom_ledpulser_DefaultCfg;
    }
    
    if((ledpulsercfg->numberofleds > eom_ledpulser_leds_number) || (0 == ledpulsercfg->numberofleds))
    {
        return(NULL);
    }
    
    // dont do any control over coherency of leds is ...
    
    memcpy(&s_mledpulser.config, ledpulsercfg, sizeof(eOmledpulser_cfg_t));
    
    memset(&s_mledpulser.ticks[0], 0, eom_ledpulser_leds_number);
 
    // i get the timers
    uint8_t i = 0;
    for(i=0; i<s_mledpulser.config.numberofleds; i++)
    {
        s_mledpulser.timer[i] = eo_timer_New(); 
    }
    
    // i get the action
    s_mledpulser.action = eo_action_New();
    
    eo_action_SetCallback(s_mledpulser.action, s_eom_ledpulser_callback, (void*)0, eom_callbackman_GetTask(eom_callbackman_GetHandle()));

    // i init the leds
    for(i=0; i<s_mledpulser.config.numberofleds; i++)
    {
        hal_led_init(s_mledpulser.config.leds[i], NULL);
        hal_led_off(s_mledpulser.config.leds[i]);
    }

    
    // return the singleton handler
    return(&s_mledpulser);
    
}    


extern EOMtheLEDpulser * eom_ledpulser_GetHandle(void) 
{ 
    return( (NULL != s_mledpulser.timer) ? (&s_mledpulser) : (NULL) );   
}

extern void eom_ledpulser_On(EOMtheLEDpulser* p, eOmledpulser_led_t id)
{
    if(NULL == p)
    {
        return;
    }
    
    if(eo_tmrstat_Running == eo_timer_GetStatus(s_mledpulser.timer[id]))
    {
        eo_timer_Stop(s_mledpulser.timer[id]);
    }
    
    s_mledpulser.ticks[id] = 0;
    
    hal_led_on(s_mledpulser.config.leds[id]);      
}

extern void eom_ledpulser_Off(EOMtheLEDpulser* p, eOmledpulser_led_t id)
{
    if(NULL == p)
    {
        return;
    }
    
    if(eo_tmrstat_Running == eo_timer_GetStatus(s_mledpulser.timer[id]))
    {
        eo_timer_Stop(s_mledpulser.timer[id]);
    }
    
    s_mledpulser.ticks[id] = 0;
    
    hal_led_off(s_mledpulser.config.leds[id]);      
}

extern void eom_ledpulser_Toggle(EOMtheLEDpulser* p, eOmledpulser_led_t id)
{
    if(NULL == p)
    {
        return;
    }
    
    if(eo_tmrstat_Running == eo_timer_GetStatus(s_mledpulser.timer[id]))
    {
        eo_timer_Stop(s_mledpulser.timer[id]);
    }
    
    s_mledpulser.ticks[id] = 0;
    
    hal_led_toggle(s_mledpulser.config.leds[id]);      
}

extern void eom_ledpulser_Start(EOMtheLEDpulser* p, eOmledpulser_led_t id, eOreltime_t pulseperiod, uint8_t pulsesnumberof)
{
    if(NULL == p)
    {
        return;
    }
    
    if(eo_tmrstat_Running == eo_timer_GetStatus(s_mledpulser.timer[id]))
    {
        eo_timer_Stop(s_mledpulser.timer[id]);
    }
    
    s_mledpulser.ticks[id] = 2*pulsesnumberof;
    
    hal_led_on(s_mledpulser.config.leds[id]);
    
    eo_action_SetCallback(s_mledpulser.action, s_eom_ledpulser_callback, (void*)id, eom_callbackman_GetTask(eom_callbackman_GetHandle()));
    
    eo_timer_Start(s_mledpulser.timer[id], eok_abstimeNOW, pulseperiod/2, eo_tmrmode_FOREVER, s_mledpulser.action);       
}


extern void eom_ledpulser_Stop(EOMtheLEDpulser* p, eOmledpulser_led_t id)
{
    if(NULL == p)
    {
        return;
    }
    
    eo_timer_Stop(s_mledpulser.timer[id]);
 
    hal_led_off(s_mledpulser.config.leds[id]);      
}



/* ------------------------------------------------------------------------------------
   definition of extern protected functions
   ------------------------------------------------------------------------------------
*/



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

 
static void s_eom_ledpulser_callback(void* p)
{
    EOMtheLEDpulser* pulser = &s_mledpulser;
    uint32_t dummy = (uint32_t) p; // to remove compiler warning 
    eOmledpulser_led_t id = (eOmledpulser_led_t) dummy;
    
    pulser->ticks[id] --;
    
    if(0 == pulser->ticks[id])
    {
        hal_led_off(pulser->config.leds[id]);
        eo_timer_Stop(pulser->timer[id]);
    }
    else
    {
        hal_led_toggle(pulser->config.leds[id]);
    }
    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




