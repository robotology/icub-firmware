/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HAL_BRDCFG_EMS001_H_
#define _HAL_BRDCFG_EMS001_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       hal_brdcfg_ems001.h
    @brief      This header file defines Interface Pins, eval-board depend macro and low level function prototypes.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       11/16/2010
 **/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"


#ifdef  HAL_USE_CAN
    #include "hal_stm32_can_hid.h"
#endif//HAL_USE_CAN

#ifdef  HAL_USE_CRC
    #include "hal_stm32_crc_hid.h"
#endif//HAL_USE_CRC

#ifdef HAL_USE_ETH
    #include "hal_stm32_eth_hid.h"
#endif//HAL_USE_ETH

#ifdef HAL_USE_FLASH
    #include "hal_stm32_flash_hid.h"
#endif//HAL_USE_FLASH

#ifdef HAL_USE_GPIO
    #include "hal_stm32_gpio_hid.h"
#endif//HAL_USE_GPIO

#ifdef HAL_USE_I2C
    #include "hal_stm32_i2c_hid.h"
#endif//HAL_USE_I2C

#ifdef HAL_USE_SPI
    #include "hal_spi.h"
#endif//HAL_USE_SPI

#ifdef HAL_USE_SYS
    #include "hal_stm32_sys_hid.h"
#endif//HAL_USE_SYS

#ifdef  HAL_USE_TIMER
    #include "hal_stm32_timer_hid.h"  
#endif//HAL_USE_TIMER

#ifdef  HAL_USE_WATCHDOG
    #include "hal_stm32_watchdog_hid.h"  
#endif//HAL_USE_WATCHDOG



#ifdef  HAL_USE_ACTUATOR_LED
    #include "hal_actuator_led_hid.h"
#endif//HAL_USE_ACTUATOR_LED


#ifdef  HAL_USE_DEVICE_CANTRANSCEIVER
    #include "hal_device_cantransceiver_hid.h" 
#endif//HAL_USE_DEVICE_CANTRANSCEIVER

#ifdef  HAL_USE_DEVICE_EEPROM
    #include "hal_device_eeprom_hid.h" 
//    #include "utils/hal_chip_xx_eeprom.h"
#endif//HAL_USE_DEVICE_EEPROM

#ifdef  HAL_USE_DEVICE_ETHTRANSCEIVER
    #include "hal_device_ethtransceiver_hid.h" 
#endif//HAL_USE_DEVICE_ETHTRANSCEIVER

#ifdef  HAL_USE_DEVICE_SWITCH
    #include "hal_device_switch_hid.h" 
#endif//HAL_USE_DEVICE_SWITCH

#ifdef  HAL_USE_SENSOR_ACCEL
    #include "hal_sensor_accel_hid.h"
#endif//HAL_USE_SENSOR_ACCEL

#ifdef  HAL_USE_SENSOR_GYRO
    #include "hal_sensor_gyro_hid.h"
#endif//HAL_USE_SENSOR_GYRO
    
#ifdef  HAL_USE_SENSOR_TEMP
    #include "hal_sensor_temp_hid.h" 
#endif//HAL_USE_SENSOR_TEMP


#include "hal_stm32xx_include.h"




// - public #define  --------------------------------------------------------------------------------------------------


#if defined(HAL_USE_ETH) || defined(HAL_USE_DEVICE_ETHTRANSCEIVER) || defined(HAL_USE_DEVICE_SWITCH)
    #define HAL_ETH_PHYMODE_THEONE2USE     hal_eth_phymode_fullduplex100mbps
#endif

//#ifdef HAL_USE_SPI4ENCODER
//    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOD
//    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOD
//    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOD  
//#endif//HAL_USE_SPI4ENCODER




// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

#ifdef  HAL_USE_CAN
    extern const hal_can_hid_brdcfg_t hal_brdcfg_can__theconfig;
#endif//HAL_USE_CAN

#ifdef  HAL_USE_CRC
    extern const hal_crc_hid_brdcfg_t hal_brdcfg_crc__theconfig;
#endif//HAL_USE_CRC

#ifdef  HAL_USE_ETH
    extern const hal_eth_hid_brdcfg_t hal_brdcfg_eth__theconfig;
#endif//HAL_USE_ETH

#ifdef  HAL_USE_FLASH
    extern const hal_flash_hid_brdcfg_t hal_brdcfg_flash__theconfig;
#endif//HAL_USE_FLASH

#ifdef  HAL_USE_GPIO
    extern const hal_gpio_hid_brdcfg_t hal_brdcfg_gpio__theconfig;
#endif//HAL_USE_GPIO

#ifdef  HAL_USE_I2C
    extern const hal_i2c_hid_brdcfg_t hal_brdcfg_i2c__theconfig;
#endif//HAL_USE_I2C

#ifdef  HAL_USE_SPI
    extern const hal_spi_hid_brdcfg_t hal_brdcfg_spi__theconfig;
    tobedone
    extern const uint8_t hal_brdcfg_spi__supported_mask;
    extern const hal_gpio_cfg_t hal_brdcfg_spi__miso[];
    extern const hal_gpio_cfg_t hal_brdcfg_spi__mosi[];
    extern const hal_gpio_cfg_t hal_brdcfg_spi__sck[];
#endif//HAL_USE_SPI


#ifdef  HAL_USE_SYS
    extern const hal_sys_hid_brdcfg_t hal_brdcfg_sys__theconfig;
#endif//HAL_USE_SYS            

#ifdef  HAL_USE_TIMER
    extern const hal_timer_hid_brdcfg_t hal_brdcfg_timer__theconfig;  
#endif//HAL_USE_TIMER

#ifdef  HAL_USE_WATCHDOG
    extern const hal_watchdog_hid_brdcfg_t hal_brdcfg_watchdog__theconfig;
#endif//HAL_USE_WATCHDOG

#ifdef  HAL_USE_ACTUATOR_LED
    extern const hal_actuator_led_hid_brdcfg_t hal_brdcfg_actuator_led__theconfig;
#endif//HAL_USE_ACTUATOR_LED 

#ifdef  HAL_USE_DEVICE_CANTRANSCEIVER
    extern const hal_device_cantransceiver_hid_brdcfg_t hal_brdcfg_device_cantransceiver__theconfig;
#endif//HAL_USE_DEVICE_CANTRANSCEIVER 

#ifdef  HAL_USE_DEVICE_DISPLAY
    extern const hal_device_display_hid_brdcfg_t hal_brdcfg_display_encoder__theconfig
#endif//HAL_USE_DEVICE_DISPLAY

#ifdef  HAL_USE_DEVICE_EEPROM
    extern const hal_device_eeprom_hid_brdcfg_t hal_brdcfg_device_eeprom__theconfig;   
#endif//HAL_USE_DEVICE_EEPROM 

#ifdef  HAL_USE_DEVICE_ETHTRANSCEIVER
    extern const hal_device_ethtransceiver_hid_brdcfg_t hal_brdcfg_device_ethtransceiver__theconfig;
#endif//HAL_USE_DEVICE_ETHTRANSCEIVER

#ifdef HAL_USE_DEVICE_SWITCH
    extern const hal_device_switch_hid_brdcfg_t hal_brdcfg_device_switch__theconfig;
#endif//HAL_USE_DEVICE_SWITCH


#ifdef  HAL_USE_SENSOR_ACCEL
    extern const hal_sensor_accel_hid_brdcfg_t hal_brdcfg_sensor_accel__theconfig;
#endif//HAL_USE_SENSOR_ACCEL


#ifdef  HAL_USE_SENSOR_ENCODER
    extern const hal_sensor_encoder_hid_brdcfg_t hal_brdcfg_sensor_encoder__theconfig;
#endif//HAL_USE_SENSOR_ENCODER


#ifdef  HAL_USE_SENSOR_GYRO
    extern const hal_sensor_gyro_hid_brdcfg_t hal_brdcfg_sensor_gyro__theconfig;
#endif//HAL_USE_SENSOR_GYRO

#ifdef  HAL_USE_SENSOR_TEMP
    extern const hal_sensor_temp_hid_brdcfg_t hal_brdcfg_sensor_temp__theconfig;
#endif//HAL_USE_SENSOR_TEMP




// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint32_t hal_brdcfg_chips__getsize(const hal_cfg_t *cfg);
extern hal_result_t hal_brdcfg_chips__setmem(const hal_cfg_t *cfg, uint32_t *memory);



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





