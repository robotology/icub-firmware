
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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EVENTVIEWER_H_
#define _EVENTVIEWER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eventviewer.h
    @brief      This header file implements public interface to the hal trace module.
    @author     marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup eventviewer Event Viewer

    The HAL TRACE ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef uint8_t evEntityId_t;
typedef void (*evEntityName_t)(void); 

// they are the reserved IDs
typedef enum
{
    ev_ID_idle              = 1,
    ev_ID_first_usrdef      = 2,
    ev_ID_first_ostask      = 64, 
    ev_ID_pendsv            = 128,    
    ev_ID_systick           = 129,
    ev_ID_svc               = 130,
    ev_ID_first_isr         = 131     
} ev_IDs_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section





// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern uint8_t eventviewer_init(void)
    @brief      Initialises the trace peripheral for use with the event viewer of uv4.
    @return     1 upon success
    @warning    The function requires a proper .ini file for the toolchain.
 **/
extern uint8_t eventviewer_init(void);


/** @fn         extern void eventviewer_load(evEntityId_t id, evEntityName_t name)
    @brief      fegfergvervg
 **/
extern void eventviewer_load(evEntityId_t id, evEntityName_t name);


/** @fn         extern void eventviewer_unload(evEntityId_t id, evEntityName_t name)
    @brief      crvcrever
 **/
extern void eventviewer_unload(evEntityId_t id, evEntityName_t name);


/** @fn         extern evEntityId_t eventviewer_switch_to(evEntityId_t id)
    @brief      crvcrever
 **/
extern evEntityId_t eventviewer_switch_to(evEntityId_t id);
 
 
 
 /** @}            
    end of group eventviewer  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



