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
#ifndef _EOUMLSM_HID_H_
#define _EOUMLSM_HID_H_


/* @file       EOeo_umlsm_hid.h
    @brief      This header file implements hidden interface to a uml state machine.
    @author     marco.accame@iit.it
    @date       09/02/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOfifoByte.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOumlsm.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/* @struct     EOeo_umlsm_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  

struct EOumlsm_hid 
{
    void                        *ram;
    eOumlsm_cfg_t               *cfg;
    uint8_t                     initialised;            /**< set to true first time eo_umlsm_Init() is called to avoid re-init again */
    uint8_t                     activestate;            /**< index inside states_table for the active state */
    EOfifoByte                  *internal_event_fifo;   /**< fifo queue of internal events */
//    const sm_state_t    *state;                 /**< pointer to active state */        
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





