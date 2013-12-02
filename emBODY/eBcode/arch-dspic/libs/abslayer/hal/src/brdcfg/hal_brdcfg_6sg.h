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
  
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _HAL_BRDCFG_6SG_H_
#define _HAL_BRDCFG_6SG_H_

/* @file       hal_brdcfg_6sg.h
    @brief      This header file defines Interface Pins, eval-board depend macro and low level function prototype.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/09/2010
 **/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"

#ifdef HAL_USE_CAN
    #include "hal_can.h"
#endif//HAL_USE_CAN

#ifdef HAL_USE_GPIO
    #include "hal_gpio.h"
#endif//HAL_USE_GPIO

#ifdef HAL_USE_LED
    #include "hal_led.h"
#endif//HAL_USE_LED

#ifdef HAL_USE_EEPROM
    #include "hal_eeprom.h"
#endif





// - public #define  --------------------------------------------------------------------------------------------------

//#ifdef HAL_USE_CAN
//    // empty-section
//#endif//HAL_USE_CAN
//

//#ifdef HAL_USE_FLASH
//    // empty-section
//#endif//HAL_USE_FLASH  

#ifdef HAL_USE_GPIO
    #define HAL_BRDCFG_GPIO__maxnumber                          2
#endif//HAL_USE_GPIO


#ifdef HAL_USE_LED
    #define HAL_BRDCFG_LED__maxnumber                           2
#endif//HAL_USE_LED            
 
//#ifdef HAL_USE_SYS
//    // empty-section
//#endif//HAL_USE_SYS 

#ifdef HAL_USE_TIMER
    #define HAL_BRDCFG_TIMER__maxnumber                         5
#endif//HAL_USE_TIMER       




// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

#ifdef HAL_USE_CAN
    extern const uint8_t hal_brdcfg_can__supported_mask; //                     = 0x03;
    extern const uint8_t hal_brdcfg_can__rxfilter_masknum_supported; //         = 3;
    extern const uint8_t hal_brdcfg_can__rxfilter_idnum_supported; //           = 16;
#endif//HAL_USE_CAN

#ifdef HAL_USE_CRC
    extern const uint8_t hal_brdcfg_crc__supported_mask; //           = 0x03;
#endif//HAL_USE_CRC


#ifdef HAL_USE_GPIO
    extern const uint16_t hal_brdcfg_gpio__supported_mask[];
#endif//HAL_USE_GPIO

#ifdef HAL_USE_LED
    extern const hal_gpio_val_t hal_brdcfg_led__value_on;
    extern const hal_gpio_val_t hal_brdcfg_led__value_off;
    extern const uint8_t hal_brdcfg_led__supported_mask;
    extern const hal_gpio_cfg_t hal_brdcfg_led__cfg[];    
#endif//HAL_USE_LED 

#ifdef HAL_USE_TIMER
    extern const uint8_t hal_brdcfg_timer__supported_mask;
#endif//HAL_USE_TIMER


// - declaration of extern public functions ---------------------------------------------------------------------------

#ifdef HAL_USE_CAN
    extern void hal_brdcfg_can__phydevices_enable(hal_can_port_t port);
    extern void hal_brdcfg_can__phydevices_disable(hal_can_port_t port);
#endif//HAL_USE_CAN


#ifdef HAL_USE_EEPROM
    extern const uint8_t hal_brdcfg_eeprom__supported_mask;
    extern const uint32_t hal_brdcfg_eeprom__emflash_baseaddress;
    extern const uint32_t hal_brdcfg_eeprom__emflash_totalsize;
#endif//HAL_USE_EEPROM 

  

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


