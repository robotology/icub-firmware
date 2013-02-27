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
#ifndef _HAL_BRDCFG_MCBSTM32C_H_
#define _HAL_BRDCFG_MCBSTM32C_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       hal_brdcfg_mcbstm32c.h
    @brief      This header file defines Interface Pins, eval-board depend macro and low level function prototypes.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/09/2010
 **/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_mpu_stm32xx_include.h"

#include "hal_base.h"

#include "hal_cpu.h"
#include "hal_cpu_arc_armcm3.h"
#include "hal_cpu_fam_stm32f1.h"
#include "hal_cpu_hid.h"

#include "hal_sys.h"
#include "hal_sys_hid.h"




// -- mpu peripherals

#ifdef  HAL_USE_CAN
    #include "hal_mpu_can_hid.h"
#endif//HAL_USE_CAN

#ifdef  HAL_USE_CRC
    #include "hal_mpu_crc_hid.h"
#endif//HAL_USE_CRC

#ifdef  HAL_USE_DMA
    #include "hal_mpu_dma_hid.h"
#endif//HAL_USE_DMA

#ifdef HAL_USE_ETH
    #include "hal_mpu_eth_hid.h"
#endif//HAL_USE_ETH

#ifdef HAL_USE_FLASH
    #include "hal_mpu_flash_hid.h"
#endif//HAL_USE_FLASH

#ifdef HAL_USE_GPIO
    #include "hal_mpu_gpio_hid.h"
#endif//HAL_USE_GPIO

#ifdef HAL_USE_I2C
    #include "hal_mpu_i2c_hid.h"
#endif//HAL_USE_I2C

#ifdef HAL_USE_SPI
    #include "hal_mpu_spi_hid.h"
#endif//HAL_USE_SPI

#ifdef  HAL_USE_TIMER
    #include "hal_mpu_timer_hid.h"  
#endif//HAL_USE_TIMER

#ifdef  HAL_USE_TRACE
    #include "hal_mpu_trace_hid.h"  
#endif//HAL_USE_TRACE

#ifdef  HAL_USE_UNIQUEID
    #include "hal_mpu_uniqueid_hid.h"  
#endif//HAL_USE_UNIQUEID

#ifdef  HAL_USE_WATCHDOG
    #include "hal_mpu_watchdog_hid.h"  
#endif//HAL_USE_WATCHDOG

// -- devices

#ifdef  HAL_USE_DEVICE_ACCELEROMETER
    #include "hal_device_accelerometer_hid.h"
#endif//HAL_USE_DEVICE_ACCELEROMETER

#ifdef  HAL_USE_DEVICE_CANTRANSCEIVER
    #include "hal_device_cantransceiver_hid.h" 
#endif//HAL_USE_DEVICE_CANTRANSCEIVER

#ifdef  HAL_USE_DEVICE_EEPROM
    #include "hal_device_eeprom_hid.h" 
#endif//HAL_USE_DEVICE_EEPROM

#ifdef  HAL_USE_DEVICE_ETHTRANSCEIVER
    #include "hal_device_ethtransceiver_hid.h" 
#endif//HAL_USE_DEVICE_ETHTRANSCEIVER

#ifdef  HAL_USE_DEVICE_DISPLAY
    #include "hal_device_display_hid.h"
#endif//HAL_USE_DEVICE_DISPLAY

#ifdef  HAL_USE_DEVICE_GYROSCOPE
    #include "hal_device_gyroscope_hid.h"
#endif//HAL_USE_DEVICE_GYROSCOPE
 
#ifdef  HAL_USE_DEVICE_LED
    #include "hal_device_led_hid.h"
#endif//HAL_USE_DEVICE_LED

//#ifdef  HAL_USE_DEVICE_SWITCH
//    #include "hal_device_switch_hid.h" 
//#endif//HAL_USE_DEVICE_SWITCH
 
#ifdef  HAL_USE_DEVICE_TERMOMETER
    #include "hal_device_termometer_hid.h" 
#endif//HAL_USE_DEVICE_TERMOMETER




// - public #define  --------------------------------------------------------------------------------------------------




// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// -- core

#ifdef  HAL_USE_BASE
    extern const hal_base_hid_brdcfg_t hal_brdcfg_base__theconfig;
#endif//HAL_USE_BASE  

#ifdef  HAL_USE_CPU
    extern const hal_cpu_hid_brdcfg_t hal_brdcfg_cpu__theconfig;
#endif//HAL_USE_CPU

#ifdef  HAL_USE_SYS
    extern const hal_sys_hid_brdcfg_t hal_brdcfg_sys__theconfig;
#endif//HAL_USE_SYS 

// -- mpu peripherals

#ifdef  HAL_USE_CAN
    extern const hal_can_hid_brdcfg_t hal_brdcfg_can__theconfig;
#endif//HAL_USE_CAN

#ifdef  HAL_USE_CRC
    extern const hal_crc_hid_brdcfg_t hal_brdcfg_crc__theconfig;
#endif//HAL_USE_CRC

#ifdef  HAL_USE_DMA
    extern const hal_dma_hid_brdcfg_t hal_brdcfg_dma__theconfig;
#endif//HAL_USE_DMA    

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
#endif//HAL_USE_SPI
         
#ifdef  HAL_USE_TIMER
    extern const hal_timer_hid_brdcfg_t hal_brdcfg_timer__theconfig;  
#endif//HAL_USE_TIMER

#ifdef  HAL_USE_TRACE
    extern const hal_trace_hid_brdcfg_t hal_brdcfg_trace__theconfig;  
#endif//HAL_USE_TRACE

#ifdef  HAL_USE_UNIQUEID
    extern const hal_uniqueid_hid_brdcfg_t hal_brdcfg_uniqueid__theconfig;
#endif//HAL_USE_UNIQUEID

#ifdef  HAL_USE_WATCHDOG
    extern const hal_watchdog_hid_brdcfg_t hal_brdcfg_watchdog__theconfig;
#endif//HAL_USE_WATCHDOG


// -- devices

#ifdef  HAL_USE_DEVICE_ACCELEROMETER
    extern const hal_device_accelerometer_hid_brdcfg_t hal_brdcfg_device_accelerometer__theconfig;
#endif//HAL_USE_DEVICE_ACCELEROMETER

#ifdef  HAL_USE_DEVICE_CANTRANSCEIVER
    extern const hal_device_cantransceiver_hid_brdcfg_t hal_brdcfg_device_cantransceiver__theconfig;
#endif//HAL_USE_DEVICE_CANTRANSCEIVER 

#ifdef HAL_USE_DEVICE_DISPLAY
    extern const hal_device_display_hid_brdcfg_t hal_brdcfg_device_display__theconfig;
#endif//HAL_USE_DEVICE_DISPLAY

#ifdef  HAL_USE_DEVICE_EEPROM
    extern const hal_device_eeprom_hid_brdcfg_t hal_brdcfg_device_eeprom__theconfig;   
#endif//HAL_USE_DEVICE_EEPROM 

#ifdef  HAL_USE_SENSOR_ENCODER
    extern const hal_sensor_encoder_hid_brdcfg_t hal_brdcfg_sensor_encoder__theconfig;
#endif//HAL_USE_SENSOR_ENCODER

#ifdef  HAL_USE_DEVICE_ETHTRANSCEIVER
    extern const hal_device_ethtransceiver_hid_brdcfg_t hal_brdcfg_device_ethtransceiver__theconfig;
#endif//HAL_USE_DEVICE_ETHTRANSCEIVER

#ifdef  HAL_USE_DEVICE_GYROSCOPE
    extern const hal_device_gyroscope_hid_brdcfg_t hal_brdcfg_device_gyroscope__theconfig;
#endif//HAL_USE_DEVICE_GYROSCOPE

#ifdef  HAL_USE_DEVICE_LED
    extern const hal_device_led_hid_brdcfg_t hal_brdcfg_device_led__theconfig;
#endif//HAL_USE_DEVICE_LED 

#ifdef  HAL_USE_DEVICE_MUX
    extern const hal_device_mux_hid_brdcfg_t hal_brdcfg_device_mux__theconfig;
#endif//HAL_USE_DEVICE_MUX 

#ifdef HAL_USE_DEVICE_SWITCH
   extern const hal_device_switch_hid_brdcfg_t hal_brdcfg_device_switch__theconfig 
#endif//HAL_USE_DEVICE_SWITCH

#ifdef  HAL_USE_DEVICE_TERMOMETER
    extern const hal_device_termometer_hid_brdcfg_t hal_brdcfg_device_termometer__theconfig;
#endif//HAL_USE_DEVICE_TERMOMETER



// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section






#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


