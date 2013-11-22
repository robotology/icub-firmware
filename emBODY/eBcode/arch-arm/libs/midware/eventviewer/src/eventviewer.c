
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


/* @file       eventviewer.c
	@brief      This file implements internal implementation of the event viewer module.
	@author     marco.accame@iit.it
    @date       02/28/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eventviewer.h"

extern void evIDLE(void);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define EVENTVIEWER_USE_CORTEX_M3M4

#define ENTITY_ID_IDLE  ev_ID_idle


// -- specific for cortex m3/m4 ----

#if defined(EVENTVIEWER_USE_CORTEX_M3M4)

#define DEMCR_TRCENA    0x01000000
#define ITM_ITMENA      0x00000001

/* Core Debug registers */
#define DEMCR           (*((volatile uint32_t *)0xE000EDFC))

/* ITM registers */
#define ITM_CONTROL     (*((volatile uint32_t *)0xE0000E80))
#define ITM_ENABLE      (*((volatile uint32_t *)0xE0000E00))
#define ITM_PORT30_U32  (*((volatile uint32_t *)0xE0000078))
#define ITM_PORT31_U32  (*((volatile uint32_t *)0xE000007C))
#define ITM_PORT31_U16  (*((volatile uint16_t *)0xE000007C))
#define ITM_PORT31_U8   (*((volatile uint8_t  *)0xE000007C))

#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static evEntityId_t s_eventviewer_currentid = ev_ID_idle;
static uint8_t s_eventviewer_initted = 0;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern evResult_t eventviewer_init(void)
{
    if(1 == s_eventviewer_initted)
    {
        return(ev_res_OK);
    }

    if((DEMCR & DEMCR_TRCENA)       && 
       (ITM_CONTROL & ITM_ITMENA)   &&
       (ITM_ENABLE & (1UL << 31))
      ) 
    {
        s_eventviewer_initted = 1;
        eventviewer_load(ev_ID_idle, evIDLE);
        eventviewer_switch_to(ev_ID_idle);
        s_eventviewer_currentid = ev_ID_idle;
        return(ev_res_OK);
    }
    return(ev_res_NOK_generic);
}


extern void eventviewer_load(evEntityId_t id, evEntityName_t name)
{
    if(0 == s_eventviewer_initted)
    {
        eventviewer_init();
    }

    while (ITM_PORT31_U32 == 0);
    ITM_PORT31_U32 = (uint32_t)name;
    while (ITM_PORT31_U32 == 0);
    ITM_PORT31_U16 = (1 << 8) | (uint8_t)id;
}


extern void eventviewer_unload(evEntityId_t id, evEntityName_t name)
{
    if(0 == s_eventviewer_initted)
    {
        eventviewer_init();
    }
    
    while (ITM_PORT31_U32 == 0);
    ITM_PORT31_U32 = (uint32_t)name;
    while (ITM_PORT31_U32 == 0);
    ITM_PORT31_U16 = (0 << 8) | (uint8_t)id;
}


extern evEntityId_t eventviewer_switch_to(evEntityId_t id)
{
    evEntityId_t prev = s_eventviewer_currentid;
    s_eventviewer_currentid = id;
    
    if(0 == s_eventviewer_initted)
    {
        return(prev);
    }
    
    while (ITM_PORT31_U32 == 0);
    ITM_PORT31_U8 = (uint8_t)s_eventviewer_currentid;
        
    return(prev);
}








// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern void evIDLE(void)
{

}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



