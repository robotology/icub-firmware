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
#ifndef _EOSM_HID_H_
#define _EOSM_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOeo_sm_hid.h
    @brief      This header file implements hidden interface to a state machine.
    @author     marco.accame@iit.it
    @date       10/06/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOsm.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/* @struct     EOsmStateQuickInfo_t
    @brief      Keeps info about current state which are used in runtime to speed up the processing of events.
 **/ 
typedef struct
{
    uint32_t                evtmask;                // mask of valid events for the state
    uint8_t                 *transindices;          // indices of transitions in rom-mapped config for each event 
} EOsmStateQuickInfo_t;

/* @struct     EOeo_sm_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  

struct EOsm_hid 
{
    const eOsm_cfg_t        *cfg;                   // pointer to rom-mapped configuration
    uint8_t                 started; 
    uint8_t                 activestate;            // current state of the state machine 
    uint8_t                 latestevent;            // the latest event received by the state machine 
    EOsmStateQuickInfo_t    *statequickinfo;        // keeps an array of states in ram.
    void                    *ram;                   // private ram
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





