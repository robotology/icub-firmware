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
#ifndef _EOMTHELEDPULSER_HID_H_
#define _EOMTHELEDPULSER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheLEDpulser_hid.h
    @brief      This header file implements .... singleton used in MEE
    @author     marco.accame@iit.it
    @date       02/05/2013
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtimer.h"
#include "EOMtask.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheLEDpulser.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section



// - definition of the hidden struct implementing the object ----------------------------------------------------------


/** @struct     EOMtheLEDpulser_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOMtheLEDpulser_hid 
{
    // base object
    EOtimer                     *timer;
    eOmledpulser_cfg_t          config;
    EOaction                    *action;
    uint16_t                    ticks;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

// name used by u-vision
void eom_gpioman(void *p);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




