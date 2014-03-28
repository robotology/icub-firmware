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
#ifndef _HAL_BRDCFG_EMS4RD_H_
#define _HAL_BRDCFG_EMS4RD_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       hal_brdcfg_ems4rd.h
    @brief      This header file defines support for board ems4rd
    @author     marco.accame@iit.it
    @date       10/03/2012
 **/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_base_hid.h"

#include "hal_mpu.h"
#include "hal_mpu_name.h"
//#include "hal_mpu_arch_armcmx.h"
//#include "hal_mpu_name_stm32f407ig.h"
#include "hal_mpu_hid.h"

#include "hal_sys.h"
#include "hal_sys_hid.h"

#include "hal_flash_hid.h"
#include "hal_heap_hid.h"


// -- mpu peripherals

#ifdef  HAL_USE_CAN
    #include "hal_can_hid.h"
#endif//HAL_USE_CAN

#ifdef  HAL_USE_CRC
    #include "hal_crc_hid.h"
#endif//HAL_USE_CRC

#ifdef  HAL_USE_DMA
    #include "hal_dma_hid.h"
#endif//HAL_USE_DMA

#ifdef HAL_USE_ETH
    #include "hal_eth_hid.h"
#endif//HAL_USE_ETH

#ifdef HAL_USE_GPIO
    #include "hal_gpio_hid.h"
#endif//HAL_USE_GPIO

#ifdef HAL_USE_I2C
    #include "hal_i2c_hid.h"
#endif//HAL_USE_I2C

#if     defined(HAL_USE_SPI) | defined(HAL_USE_SPI_DMA)
    #include "hal_spi_hid.h"
#endif//HAL_USE_SPI

#ifdef  HAL_USE_TIMER
    #include "hal_timer_hid.h"  
#endif//HAL_USE_TIMER

#ifdef  HAL_USE_TRACE
    #include "hal_trace_hid.h"  
#endif//HAL_USE_TRACE

#ifdef  HAL_USE_UNIQUEID
    #include "hal_uniqueid_hid.h"  
#endif//HAL_USE_UNIQUEID

#ifdef  HAL_USE_WATCHDOG
    #include "hal_watchdog_hid.h"  
#endif//HAL_USE_WATCHDOG

// -- devices

#ifdef  HAL_USE_ACCELEROMETER
    #include "hal_accelerometer_hid.h"
#endif//HAL_USE_ACCELEROMETER

#ifdef  HAL_USE_CANTRANSCEIVER
    #include "hal_cantransceiver_hid.h" 
#endif//HAL_USE_CANTRANSCEIVER

#ifdef  HAL_USE_DISPLAY
    #include "hal_display_hid.h" 
#endif//HAL_USE_DISPLAY

#ifdef  HAL_USE_EEPROM
    #include "hal_eeprom_hid.h" 
#endif//HAL_USE_EEPROM

#ifdef HAL_USE_ENCODER
    #include "hal_encoder_hid.h"
#endif//HAL_USE_ENCODER

#ifdef  HAL_USE_ETHTRANSCEIVER
    #include "hal_ethtransceiver_hid.h" 
#endif//HAL_USE_ETHTRANSCEIVER

#ifdef  HAL_USE_GYROSCOPE
    #include "hal_gyroscope_hid.h"
#endif//HAL_USE_GYROSCOPE
 
#ifdef  HAL_USE_LED
    #include "hal_led_hid.h"
#endif//HAL_USE_LED

#ifdef  HAL_USE_MUX
    #include "hal_mux_hid.h"
#endif//HAL_USE_MUX

#ifdef  HAL_USE_SWITCH
    #include "hal_switch_hid.h"
#endif//HAL_USE_SWITCH
 
#ifdef  HAL_USE_TERMOMETER
    #include "hal_termometer_hid.h" 
#endif//HAL_USE_TERMOMETER




// - public #define  --------------------------------------------------------------------------------------------------




// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// -- core
 

// -- mpu peripherals




// -- devices





// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section






#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


