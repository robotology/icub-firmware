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
#ifndef _EOTHEGPIO_HID_H_
#define _EOTHEGPIO_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EoTheGPIO_hid.h
    @brief      This header file implements hidden interface to the GPIO singleton.
    @author     marco.accame@iit.it
    @date       10/15/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOioPinInput.h"
#include "EOioPinOutput.h"
#include "EOioPinInputManaged.h"   
#include "EOioPinOutputManaged.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheGPIO.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section



// - definition of the hidden struct implementing the object ----------------------------------------------------------

/* @struct     EOtheGPIO_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOtheGPIO_hid 
{
    const eOgpio_cfg_t      *cfg;
    EOioPinInput            *inp;           /*< array of input pins                                    **/
    EOioPinOutput           *out;           /*< array of output pins                                   **/
    EOioPinInputManaged     *mnginp;        /*< array of managed input pins                            **/
    EOioPinOutputManaged    *mngout;        /*< array of managed output pins                           **/
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------


/* @fn         extern uint8_t eo_gpio_hid_GetNumMngInp(EOtheGPIO *const p)
    @brief      Gets the number of managed input pins 
    @param      The handle to the object.
    @return     The numer of managed input pins.
 **/
extern uint8_t eo_gpio_hid_GetNumMngInp(EOtheGPIO *const p);

/* @fn         extern uint8_t eo_gpio_hid_GetNumMngOut(EOtheGPIO *const p)
    @brief      Gets the number of managed output pins 
    @param      The handle to the object.
    @return     The numer of managed output pins.
 **/
extern uint8_t eo_gpio_hid_GetNumMngOut(EOtheGPIO *const p);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







