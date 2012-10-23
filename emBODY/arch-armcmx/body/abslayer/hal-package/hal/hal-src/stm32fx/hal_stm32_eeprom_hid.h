/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
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
#ifndef _HAL_STM32_EEPROM_HID_H_
#define _HAL_STM32_EEPROM_HID_H_


/* @file       hal_stm32_eeprom_hid.h
    @brief      This header file implements hidden interface to eeprom.
    @author     marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_gpio.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_eeprom.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef enum
{
    hal_eeprom_device_st_m24lr64        = 0,    /**< capacity 8kbytes; page size 4 bytes; i2c speeds: 400khz. it is also wireless, but we don't care. */
    hal_eeprom_device_st_m24c64         = 1,    /**< capacity 8kbytes; page size 32 bytes; i2c speeds: 1mhz, 400khz, 100khz.  */
    hal_eeprom_device_st_m24512         = 2,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz, 100khz. */
    hal_eeprom_device_atmel_at24c512b   = 3,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz.  */
    hal_eeprom_device_atmel_at24c1024b  = 4,    /**< capacity 128kbytes; page size 256 bytes; i2c speeds: 1mhz, 400khz.  */
    hal_eeprom_device_none              = 255   /**< just no device ... yet. */        
} hal_eeprom_device_t;

typedef struct
{
    hal_eeprom_device_t     device;
//    uint8_t                 i2cport;            // it can be 1, 2, 3. */
    uint8_t                 hwaddra2a1a0;       // put a 1 in pos 2, 1, 0 if the a2, a1, a0 is high. */
//    hal_gpio_val_t          wpval;              // hal_gpio_valHIGH if protection is high, hal_gpio_valLOW if it is low, hal_gpio_valNONE if not used */
//    hal_gpio_cfg_t          wppin;
} hal_eeprom_hw_cfg_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_eeprom_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_eeprom_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);


///* @fn			extern hal_result_t hal_hid_eeprom_waitstandbystate(void);
//  * @brief  	Wait for EEPROM Standby state
//  */
//extern hal_result_t hal_eeprom_hid_waitstandbystate(hal_eeprom_t eep);



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




