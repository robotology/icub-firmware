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

#ifndef _HAL_BRDCFG_EMS4RD_MODULES_H_
#define _HAL_BRDCFG_EMS4RD_MODULES_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       hal_brdcfg_ems4rd_modules.h
    @brief      This header file defines the modules to be compiled in hal
    @author     marco.accame@iit.it
    @date       10/04/2012
 **/


// - modules to be built ----------------------------------------------------------------------------------------------

//#define     HAL_BUILD_ONLYCORE
#undef      HAL_BUILD_ONLYCORE



// -- core: these values must be always defined
#define     HAL_USE_BASE
#define     HAL_USE_CORE
#define     HAL_USE_CPU
#define     HAL_USE_FLASH
#define     HAL_USE_HEAP
#define     HAL_USE_MIDDLEWARE_INTERFACE
#define     HAL_USE_SYS

// -- cpu: choose one ARC, FAM, and NAM for the cpu
#define     HAL_USE_CPU_ARC_ARMCM4
#define     HAL_USE_CPU_FAM_STM32F4
#define     HAL_USE_CPU_NAM_STM32F407

// -- the utilities, mpu peripherals, devices, chips, and external boards are used only if the build is not forced to core only 

#if     defined(HAL_BUILD_ONLYCORE)
// -- nothing else to build
#elif   !defined(HAL_BUILD_ONLYCORE) 

// -- mpu peripherals: define what you need to use (beware of cross dependencies)
#define     HAL_USE_CAN
#define     HAL_USE_ETH
#define     HAL_USE_GPIO
#define     HAL_USE_I2C
#define     HAL_USE_SPI
#define     HAL_USE_SPI_DMA
#define     HAL_USE_TIMER
#define     HAL_USE_TRACE
#define     HAL_USE_UNIQUEID
#define     HAL_USE_WATCHDOG

// -- devices: define what you need to use (beware of cross dependencies)
#define  HAL_USE_ACCELEROMETER
#define     HAL_USE_CANTRANSCEIVER
#define     HAL_USE_EEPROM
#define     HAL_USE_ENCODER
#define     HAL_USE_ETHTRANSCEIVER
#define  HAL_USE_GYROSCOPE
#define     HAL_USE_LED
#define     HAL_USE_MUX
#define     HAL_USE_SWITCH
#define  HAL_USE_TERMOMETER


#endif//!defined(HAL_BUILD_ONLYCORE)  


// - exceptions -------------------------------------------------------------------------------------------------------
// not managed so far


// - cross dependencies -----------------------------------------------------------------------------------------------
// not managed so far 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


