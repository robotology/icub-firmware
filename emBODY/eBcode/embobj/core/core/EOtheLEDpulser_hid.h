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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTHELEDPULSER_HID_H_
#define _EOTHELEDPULSER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOtheLEDpulser_hid.h
    @brief      This header file implements .... singleton used in MEE
    @author     marco.accame@iit.it
    @date       02/05/2013
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOaction.h"
#include "EOtimer.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheLEDpulser.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section



// - definition of the hidden struct implementing the object ----------------------------------------------------------


 
struct EOtheLEDpulser_hid 
{    
    eOledpulser_cfg_t           config;    
    EOaction                    *action;
    EOtimer                     *timer[eo_ledpulser_leds_number];
    uint32_t                    ticks[eo_ledpulser_leds_number];
    uint8_t                     initted;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




