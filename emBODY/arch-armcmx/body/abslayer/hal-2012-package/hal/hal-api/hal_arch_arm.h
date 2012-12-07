/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HAL_ARCH_ARM_H_
#define _HAL_ARCH_ARM_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_arch_arm.h
    @brief      This header file keeps public interface to the hal parts which are specific of arm architecture.
    @author     marco.accame@iit.it
    @date       09/16/2011
**/

/** @defgroup doxy_group_hal_arch_arm HAL ARCH ARM

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/



// - public #define  --------------------------------------------------------------------------------------------------


#if !defined(HAL_USE_ARCH_ARM)
#define HAL_USE_ARCH_ARM
#endif

// -- HAL_HAS_ are macros used to include inside hal.h the proper include files of the defined modules

// the base of hal
#define HAL_HAS_BASE

// mpu peripherals supported for arm architecture (stm32fx)
#define HAL_HAS_CAN
#define HAL_HAS_CRC
#define HAL_HAS_ETH
#define HAL_HAS_FLASH
#define HAL_HAS_GPIO
#define HAL_HAS_I2C
#define HAL_HAS_SPI
#define HAL_HAS_SYS
#define HAL_HAS_TIMER
#define HAL_HAS_TRACE
#define HAL_HAS_WATCHDOG

// external devices supported for the boards using arm architecture
#define HAL_HAS_ACCELEROMETER
#define HAL_HAS_CANTRANSCEIVER
#define HAL_HAS_DISPLAY
#define HAL_HAS_EEPROM
#define HAL_HAS_ENCODER
#define HAL_HAS_ETHTRANSCEIVER
#define HAL_HAS_GYROSCOPE
#define HAL_HAS_LED
#define HAL_HAS_SWITCH
#define HAL_HAS_TERMOMETER


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hal_base.h"




// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef uint64_t hal_arch_arm_uniqueid64_t
    @brief      expresses a ID of 64 bits which is unique, i.e., it is different in every micro-processor-unit.
 **/
typedef uint64_t hal_arch_arm_uniqueid64_t;


/** @typedef    typedef enum hal_arch_arm_irqn_t
    @brief      expresses the irq number for the cm3-stm32 architecture.
 **/
typedef enum 
{
    hal_arch_arm_NonMaskableInt_IRQn         = -14,    /**< Non Maskable Interrupt                             */
    hal_arch_arm_MemoryManagement_IRQn       = -12,    /**<  Cortex-M3 Memory Management Interrupt              */
    hal_arch_arm_BusFault_IRQn               = -11,    /**<  Cortex-M3 Bus Fault Interrupt                      */
    hal_arch_arm_UsageFault_IRQn             = -10,    /**<  Cortex-M3 Usage Fault Interrupt                    */
    hal_arch_arm_SVCall_IRQn                 = -5,     /**<  Cortex-M3 SV Call Interrupt                       */
    hal_arch_arm_DebugMonitor_IRQn           = -4,     /**<  Cortex-M3 Debug Monitor Interrupt                 */
    hal_arch_arm_PendSV_IRQn                 = -2,     /**<  Cortex-M3 Pend SV Interrupt                       */
    hal_arch_arm_SysTick_IRQn                = -1,     /**<  Cortex-M3 System Tick Interrupt                   */
    
    
    hal_arch_arm_WWDG_IRQn                   = 0,      /**< Window WatchDog Interrupt                            */
    
    hal_arch_arm_CAN1_TX_IRQn                = 19,     /**< USB Device High Priority or CAN1 TX Interrupts       */
    hal_arch_arm_CAN1_RX0_IRQn               = 20,     /**< USB Device Low Priority or CAN1 RX0 Interrupts       */
    hal_arch_arm_CAN1_RX1_IRQn               = 21,     /**< CAN1 RX1 Interrupt                                   */
    hal_arch_arm_CAN1_SCE_IRQn               = 22,     /**< CAN1 SCE Interrupt                                   */
    
    hal_arch_arm_TIM1_UP_IRQn                = 25,     /**< TIM1 Update Interrupt                                */
    
    hal_arch_arm_TIM2_IRQn                   = 28,     /**< TIM2 global Interrupt                                */
    hal_arch_arm_TIM3_IRQn                   = 29,     /**< TIM3 global Interrupt                                */
    hal_arch_arm_TIM4_IRQn                   = 30,     /**< TIM4 global Interrupt                                */
    
    hal_arch_arm_SPI1_IRQn                   = 35,     /**< SPI1 global Interrupt                                */
    hal_arch_arm_SPI2_IRQn                   = 36,     /**< SPI2 global Interrupt                                */
    
    hal_arch_arm_TIM5_IRQn                   = 50,     /**< TIM5 global Interrupt                                */
    hal_arch_arm_SPI3_IRQn                   = 51,     /**< SPI3 global Interrupt                                */
    
    hal_arch_arm_TIM6_IRQn                   = 54,     /**< TIM6 global Interrupt                                */
    hal_arch_arm_TIM7_IRQn                   = 55,     /**< TIM7 global Interrupt                                */
    
    hal_arch_arm_ETH_IRQn                    = 61,     /**< Ethernet global Interrupt                            */
    hal_arch_arm_CAN2_TX_IRQn                = 63,     /**< CAN2 TX Interrupt                                    */
    hal_arch_arm_CAN2_RX0_IRQn               = 64,     /**< CAN2 RX0 Interrupt                                   */
    hal_arch_arm_CAN2_RX1_IRQn               = 65,     /**< CAN2 RX1 Interrupt                                   */
    hal_arch_arm_CAN2_SCE_IRQn               = 66      /**< CAN2 SCE Interrupt                                   */
 
} hal_arch_arm_irqn_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

/** @fn         extern hal_arch_arm_uniqueid64_t hal_arch_arm_uniqueid64_get(void)
    @brief      This function gives back an ID of 64 bits which is unique and can be used for instance for
                deriving a MAC address, etc.
    @return     The ID. If valid it must be different from hal_NA64
  */
extern hal_arch_arm_uniqueid64_t hal_arch_arm_uniqueid64_get(void);



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group doxy_group_hal_arch_arm  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



