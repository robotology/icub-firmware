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
#include "EOVtheCallbackManager_hid.h"

#include "hal.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheLEDpulser.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheLEDpulser_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

static int8_t s_eo_ledpulser_fake_led_init(int8_t id, const void* cfg);
static int8_t s_eo_ledpulser_fake_led_on(int8_t id);
static int8_t s_eo_ledpulser_fake_led_off(int8_t id);
static int8_t s_eo_ledpulser_fake_led_toggle(int8_t id);


const eOledpulser_cfg_t eo_ledpulser_DefaultCfg = 
{ 
    .led_enable_mask    = 1 << eo_ledpulser_led_zero,
    .led_init           = s_eo_ledpulser_fake_led_init,
    .led_on             = s_eo_ledpulser_fake_led_on,
    .led_off            = s_eo_ledpulser_fake_led_off,
    .led_toggle         = s_eo_ledpulser_fake_led_toggle,
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_ledpulser_callback(void* p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOtheLEDpulser";

static EOtheLEDpulser s_theledpulser = 
{
    .config         = 
    {
        .led_enable_mask = 1 << eo_ledpulser_led_zero 
    },
    .action         = NULL,
    .timer          = {NULL},
    .ticks          = {0},
    .initted        = 0   
}; 



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheLEDpulser * eo_ledpulser_Initialise(const eOledpulser_cfg_t *ledpulsercfg) 
{
    // already initialised ?
    if(0 != s_theledpulser.initted) 
    {
        // already initialised !
        return(&s_theledpulser);
    }
    
 
    if(NULL == ledpulsercfg)
    {
        ledpulsercfg = &eo_ledpulser_DefaultCfg;
    }
    
    if((0 == ledpulsercfg->led_enable_mask) || 
       (NULL == ledpulsercfg->led_init) || 
       (NULL == ledpulsercfg->led_on) ||
       (NULL == ledpulsercfg->led_off) || 
       (NULL == ledpulsercfg->led_toggle))
    {
        return(NULL);
    }
    
    // dont do any control over coherency of leds is ...
    
    memcpy(&s_theledpulser.config, ledpulsercfg, sizeof(eOledpulser_cfg_t));
    
    memset(&s_theledpulser.ticks[0], 0, eo_ledpulser_leds_number);
 
    // i get the timers and init teh leds
    uint8_t i = 0;
    for(i=0; i<eo_ledpulser_leds_number; i++)
    {
        uint8_t ledbit = 0x1 << i;
        if((ledpulsercfg->led_enable_mask & ledbit) == ledbit)
        {            
            s_theledpulser.timer[i] = eo_timer_New(); 
            s_theledpulser.config.led_init((hal_led_t)i, NULL);
            s_theledpulser.config.led_off((hal_led_t)i);
        }
        else
        {
            s_theledpulser.timer[i] = NULL;
        }
    }
    
    // i get the action
    s_theledpulser.action = eo_action_New();
    
    eo_action_SetCallback(s_theledpulser.action, s_eo_ledpulser_callback, (void*)0, eov_callbackman_GetTask(eov_callbackman_GetHandle()));

    s_theledpulser.initted = 1;
    
    // return the singleton handler
    return(&s_theledpulser);
    
}    


extern EOtheLEDpulser * eo_ledpulser_GetHandle(void) 
{ 
    return( (NULL != s_theledpulser.timer) ? (&s_theledpulser) : (NULL) );   
}

extern void eo_ledpulser_On(EOtheLEDpulser* p, eOledpulser_led_t id)
{
    if(NULL == p)
    {
        return;
    }
    
    if((eo_ledpulser_led_none == id) || (NULL == s_theledpulser.timer[id]))
    {
        return;
    }
    
    if(eo_tmrstat_Running == eo_timer_GetStatus(s_theledpulser.timer[id]))
    {
        eo_timer_Stop(s_theledpulser.timer[id]);
    }
    
    s_theledpulser.ticks[id] = 0;
    
    s_theledpulser.config.led_on((hal_led_t)id);      
}

extern void eo_ledpulser_Off(EOtheLEDpulser* p, eOledpulser_led_t id)
{
    if(NULL == p)
    {
        return;
    }
    
    if((eo_ledpulser_led_none == id) || (NULL == s_theledpulser.timer[id]))
    {
        return;
    }
    
    if(eo_tmrstat_Running == eo_timer_GetStatus(s_theledpulser.timer[id]))
    {
        eo_timer_Stop(s_theledpulser.timer[id]);
    }
    
    s_theledpulser.ticks[id] = 0;
    
    s_theledpulser.config.led_off((hal_led_t)id);      
}

extern void eo_ledpulser_Toggle(EOtheLEDpulser* p, eOledpulser_led_t id)
{
    if(NULL == p)
    {
        return;
    }
    
    if((eo_ledpulser_led_none == id) || (NULL == s_theledpulser.timer[id]))
    {
        return;
    }    
    
    if(eo_tmrstat_Running == eo_timer_GetStatus(s_theledpulser.timer[id]))
    {
        eo_timer_Stop(s_theledpulser.timer[id]);
    }
    
    s_theledpulser.ticks[id] = 0;
    
    s_theledpulser.config.led_toggle((hal_led_t)id);      
}

extern void eo_ledpulser_Start(EOtheLEDpulser* p, eOledpulser_led_t id, eOreltime_t pulseperiod, uint16_t pulsesnumberof)
{
    if(NULL == p)
    {
        return;
    }
    
    if((eo_ledpulser_led_none == id) || (NULL == s_theledpulser.timer[id]))
    {
        return;
    }    
    
    if(eo_tmrstat_Running == eo_timer_GetStatus(s_theledpulser.timer[id]))
    {
        eo_timer_Stop(s_theledpulser.timer[id]);
    }
    
    if(0 == pulsesnumberof)
    {
        s_theledpulser.ticks[id] = EOK_uint32dummy;
    }
    else
    {   
        s_theledpulser.ticks[id] = 2*pulsesnumberof;
    }
    
    s_theledpulser.config.led_on((hal_led_t)id);
    
    eo_action_SetCallback(s_theledpulser.action, s_eo_ledpulser_callback, (void*)id, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
    
    eo_timer_Start(s_theledpulser.timer[id], eok_abstimeNOW, pulseperiod/2, eo_tmrmode_FOREVER, s_theledpulser.action);       
}


extern void eo_ledpulser_Stop(EOtheLEDpulser* p, eOledpulser_led_t id)
{
    if(NULL == p)
    {
        return;
    }
    
    if((eo_ledpulser_led_none == id) || (NULL == s_theledpulser.timer[id]))
    {
        return;
    }    
    
    eo_timer_Stop(s_theledpulser.timer[id]);
 
    s_theledpulser.config.led_off((hal_led_t)id);      
}



/* ------------------------------------------------------------------------------------
   definition of extern protected functions
   ------------------------------------------------------------------------------------
*/



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

 
static void s_eo_ledpulser_callback(void* p)
{
    EOtheLEDpulser* pulser = &s_theledpulser;
    uint32_t dummy = (uint32_t) p; // to remove compiler warning 
    eOledpulser_led_t id = (eOledpulser_led_t) dummy;
    
    if(EOK_uint32dummy == pulser->ticks[id])
    {
        // dont decrement because we go on forever
    }
    else
    {
        pulser->ticks[id] --;
    }
    
    
    if(0 == pulser->ticks[id])
    {
        s_theledpulser.config.led_off((hal_led_t)id);
        eo_timer_Stop(pulser->timer[id]);
    }
    else
    {
        s_theledpulser.config.led_toggle((hal_led_t)id);
    }
    
}

static int8_t s_eo_ledpulser_fake_led_init(int8_t id, const void* cfg)
{
    return(-1);
}

static int8_t s_eo_ledpulser_fake_led_on(int8_t id)
{
    return(-1);
}

static int8_t s_eo_ledpulser_fake_led_off(int8_t id)
{
    return(-1);
}

static int8_t s_eo_ledpulser_fake_led_toggle(int8_t id)
{
    return(-1);
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




