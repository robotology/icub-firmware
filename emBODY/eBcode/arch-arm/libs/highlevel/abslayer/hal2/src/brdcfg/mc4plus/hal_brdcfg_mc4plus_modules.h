/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Maggiali, Valentina Gaggero, Marco Accame
 * email:   marco.maggiali@iit.it, valentina.gaggero@iit.it, marco.accame@iit.it
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

#ifndef _HAL_BRDCFG_MC4PLUS_MODULES_H_
#define _HAL_BRDCFG_MC4PLUS_MODULES_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       hal_brdcfg_mc4plus_modules.h
    @brief      This header file defines the modules to be compiled in hal
    @author     marco.accame@iit.it
    @date       7/11/2014
 **/

// # HAL section: begin -----------------------------------------------------------------------------------------------

// - porting across multiple mpus -------------------------------------------------------------------------------------

// -- mpu: choose one NAME: STM32F103RB, STM32F107VC, STM32F407IG. 

#define     HAL_USE_MPU_NAME_STM32F407IG
// #define     HAL_USE_MPU_TYPE_STM32F4
// #define     HAL_USE_MPU_ARCH_ARMCM4

// -- convert NAME into whatever else is needed (TYPE, ARCH). just include hal_mpuname2macros.h

#include "hal_mpuname2macros.h"

// - HAL modules to be built ------------------------------------------------------------------------------------------


// -- core: these moduled must be always defined

#define     HAL_USE_BASE
#define     HAL_USE_CORE
#define     HAL_USE_MPU
#define     HAL_USE_FLASH
#define     HAL_USE_HEAP
#define     HAL_USE_MIDDLEWARE_INTERFACE
#define     HAL_USE_SYS




// -- whatever else 

// -- peripherals: define what you need (beware of cross dependencies)

#undef     HAL_USE_CAN
#define     HAL_USE_ETH
#define     HAL_USE_GPIO
#define     HAL_USE_I2C
#undef     HAL_USE_SPI
#define     HAL_USE_TIMER
#define     HAL_USE_TRACE
#define     HAL_USE_UNIQUEID
#undef     HAL_USE_WATCHDOG


// -- devices: define what you need (beware of cross dependencies)

#undef  HAL_USE_ACCELEROMETER
#undef     HAL_USE_CANTRANSCEIVER
#define     HAL_USE_EEPROM
#undef     HAL_USE_ENCODER
#define     HAL_USE_ETHTRANSCEIVER
#undef  HAL_USE_GYROSCOPE
#define     HAL_USE_LED
#undef     HAL_USE_MUX
#define     HAL_USE_SWITCH
#undef  HAL_USE_TERMOMETER


// -- behaviour: define how the code is shaped
// validity check verifies if the module has been initted before executing something 
// argument check verifies that the arguments are corrects
//#define  HAL_BEH_REMOVE_RUNTIME_INITTED_CHECK
//#define  HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK



// # HAL section: end -------------------------------------------------------------------------------------------------


// # HL section: begin ------------------------------------------------------------------------------------------------
// # this section is required so that we propagate what specified above in HAL section also to HL.

// - hl_cfg_plus_target.h ---------------------------------------------------------------------------------------------


// - mpu name (defined even if it is already defined by the call of hal_mpuname2macros.h)

    #if !defined(HL_USE_MPU_NAME_STM32F407IG)
        #define HL_USE_MPU_NAME_STM32F407IG
    #endif

// - preparation of setsysclock

    #define HL_CFG_OVERRIDE_hl_system_stm32fx_before_setsysclock

// - mpu speed used in setsysclock
    
    #define HL_CFG_MPUSPEED_HSEBYPASS
    #define HL_CFG_MPUSPEED_INTclockspeed               ((uint32_t)16000000)
    #define HL_CFG_MPUSPEED_EXTclockspeed               ((uint32_t)50000000)
    
    #define HL_CFG_MPUSPEED_STM32F4_PLL_m               (50)
    #define HL_CFG_MPUSPEED_STM32F4_PLL_n               (336)
    #define HL_CFG_MPUSPEED_STM32F4_PLL_p               (2)
    #define HL_CFG_MPUSPEED_STM32F4_PLL_q               (7)
    

// - mpu arch:
#include "hl_mpu2arch.h"


// - mpu speed:
#include "hl_mpuspeed.h"    


// - hl_cfg_plus_modules.h --------------------------------------------------------------------------------------------

#define HL_USE_CORE_CMSIS

#define HL_USE_CORE_STM32

#define HL_USE_UTIL_SYS
#define HL_USE_UTIL_GPIO
#define HL_USE_UTIL_BITS
#define HL_USE_UTIL_FIFO
#define HL_USE_UTIL_I2C
#define HL_USE_UTIL_ETH
#define HL_USE_UTIL_ETHTRANS
#define HL_USE_CHIP_MICREL_KS8893
#undef HL_USE_UTIL_CAN
#undef HL_USE_UTIL_CAN_COMM
#define HL_USE_CHIP_XX_EEPROM
#undef HL_USE_UTIL_SPI
#define HL_USE_UTIL_TIMER
#undef HL_USE_CHIP_ST_LIS3X
#undef HL_USE_CHIP_ST_L3G4200D

#if     defined(MC4PLUS_ETHDBG)
#define MC4PLUS_USE_MICREL_AS_MANAGED_DEVICE
#endif



//#define HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK
//#define HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


