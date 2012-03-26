/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

/* @file       hal_brdcfg_2foc.c
	@brief      This file implements low level functions which depend on board.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       11/16/2010
**/


// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
  
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "p33fxxxx.h"
#include <libpic30.h>

#include "hal_base.h"
#include "hal_dspic_base_hid.h"

#include "hal_timer.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_brdcfg_2foc.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#ifdef HAL_USE_CAN
    const uint8_t hal_brdcfg_can__supported_mask                     = 0x01;
    const uint8_t hal_brdcfg_can__rxfilter_masknum_supported         = 3;
    const uint8_t hal_brdcfg_can__rxfilter_idnum_supported           = 16;
#endif//HAL_USE_CAN


#ifdef HAL_USE_CRC
    const uint8_t hal_brdcfg_crc__supported_mask                    = 0x03;
#endif//HAL_USE_CRC


#ifdef HAL_USE_EEPROM

    const uint8_t hal_brdcfg_eeprom__supported_mask         = (1 << hal_eeprom_emulatedflash);
    // from 0x015000 to 0x015800 in dspic address space, where in a location we can put 2 data bytes
    const uint32_t hal_brdcfg_eeprom__emflash_baseaddress           = 0x014C00; //0x2A000;     // 0x015000 = 86016 -> 2*86016 = 172032 = 168*1024 
    const uint32_t hal_brdcfg_eeprom__emflash_totalsize             = 3L*1024L;       // 0x000800 = 2048  -> 2*2048  = 4096   = 4*1024

#endif//HAL_USE_EEPROM 


#ifdef HAL_USE_GPIO
    const uint16_t hal_brdcfg_gpio__supported_mask[hal_gpio_ports_number]    =
    {   // ok, i enable every pin of every port. however i should enable only those used a true gpio
        0x001f,     // port a
        0xffff,     // port b
        0x0000,     // port c
        0x0000,     // port d
        0x0000,     // port e
        0x0000,     // port f
        0x0000,     // port g
        0x0000,     // port h
        0x0000      // port i
    };
#endif//HAL_USE_GPIO

#ifdef HAL_USE_LED

    const hal_gpio_val_t hal_brdcfg_led__value_on          = hal_gpio_valHIGH;
    const hal_gpio_val_t hal_brdcfg_led__value_off         = hal_gpio_valLOW;
    const uint8_t hal_brdcfg_led__supported_mask           = 0x01;
    const hal_gpio_cfg_t hal_brdcfg_led__cfg[HAL_BRDCFG_LED__maxnumber] = 
    {
        {   // hal_led0 
            .port     = hal_gpio_portA,
            .pin      = hal_gpio_pin4,        
            .speed    = hal_gpio_speed_default,
            .dir      = hal_gpio_dirOUT
        }
    };

#endif//HAL_USE_LED

#ifdef HAL_USE_TIMER
    const uint8_t hal_brdcfg_timer__supported_mask           = 
    (1 << hal_timer1) | (1 << hal_timer2) | (1 << hal_timer3) | (1 << hal_timer4) |
    (1 << hal_timer5); //0x1F
#endif//HAL_USE_TIMER



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#ifdef HAL_USE_CAN

extern void hal_brdcfg_can__phydevices_enable(hal_can_port_t port)
{
    //I don't check param port, because this function is called after can port supported check

    RPOR2bits.RP4R = 0b10000;   // TX: Set RP4 as output CANTX
    RPINR26bits.C1RXR = 2; //8;      // RX: Connect ECAN1 Receive to RP8
    //TRISAbits.TRISA2 = 0;
    
}

extern void hal_brdcfg_can__phydevices_disable(hal_can_port_t port)
{
   ;
}

#endif//HAL_USE_CAN




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


