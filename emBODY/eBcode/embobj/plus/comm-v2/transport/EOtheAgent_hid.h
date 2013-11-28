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
#ifndef _EOTHEAGENT_HID_H_
#define _EOTHEAGENT_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOtheAgent_hid.h
    @brief      This header file implements hidden interface to the EOtheAgent singleton.
    @author     marco.accame@iit.it
    @date       09/03/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheAgent.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section



// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOtheAgent_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOtheAgent_hid 
{
    uint8_t                 initted;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

// to be called only once just before transmission
//extern eOresult_t eo_agent_hid_OutROPonTransmission(EOtheAgent *p, EOrop *rop);

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







