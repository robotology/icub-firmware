/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HAL_CPU_FAM_STM32F1_H_
#define _HAL_CPU_FAM_STM32F1_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_cpu_fam_stm32f1.h
    @brief      This header file keeps public interface to the hal parts which are specific of cpu family: stm32f1
    @author     marco.accame@iit.it
    @date       02/27/2013
**/

/** @defgroup doxy_group_hal_cpu_fam_stm32f1 HAL CPU STM32F1 FAMILY

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/



// - public #define  --------------------------------------------------------------------------------------------------



// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"

#include "hal_base.h"
#include "hal_sys.h"
#include "hal_cpu.h"



// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_cpu_fam_stm32f1_irqn_t
    @brief      contains the irq numbers for the stm32f1x family of processors.
 **/
typedef enum 
{
    hal_cpu_fam_stm32f1_NonMaskableInt_IRQn         = -14,    /**< Non Maskable Interrupt                             */
    hal_cpu_fam_stm32f1_MemoryManagement_IRQn       = -12,    /**<  Cortex-M3 Memory Management Interrupt              */
    hal_cpu_fam_stm32f1_BusFault_IRQn               = -11,    /**<  Cortex-M3 Bus Fault Interrupt                      */
    hal_cpu_fam_stm32f1_UsageFault_IRQn             = -10,    /**<  Cortex-M3 Usage Fault Interrupt                    */
    hal_cpu_fam_stm32f1_SVCall_IRQn                 = -5,     /**<  Cortex-M3 SV Call Interrupt                       */
    hal_cpu_fam_stm32f1_DebugMonitor_IRQn           = -4,     /**<  Cortex-M3 Debug Monitor Interrupt                 */
    hal_cpu_fam_stm32f1_PendSV_IRQn                 = -2,     /**<  Cortex-M3 Pend SV Interrupt                       */
    hal_cpu_fam_stm32f1_SysTick_IRQn                = -1,     /**<  Cortex-M3 System Tick Interrupt                   */
    
    
    hal_cpu_fam_stm32f1_WWDG_IRQn                   = 0,      /**< Window WatchDog Interrupt                            */
    
    hal_cpu_fam_stm32f1_CAN1_TX_IRQn                = 19,     /**< USB Device High Priority or CAN1 TX Interrupts       */
    hal_cpu_fam_stm32f1_CAN1_RX0_IRQn               = 20,     /**< USB Device Low Priority or CAN1 RX0 Interrupts       */
    hal_cpu_fam_stm32f1_CAN1_RX1_IRQn               = 21,     /**< CAN1 RX1 Interrupt                                   */
    hal_cpu_fam_stm32f1_CAN1_SCE_IRQn               = 22,     /**< CAN1 SCE Interrupt                                   */
    
    hal_cpu_fam_stm32f1_TIM1_UP_IRQn                = 25,     /**< TIM1 Update Interrupt                                */
    
    hal_cpu_fam_stm32f1_TIM2_IRQn                   = 28,     /**< TIM2 global Interrupt                                */
    hal_cpu_fam_stm32f1_TIM3_IRQn                   = 29,     /**< TIM3 global Interrupt                                */
    hal_cpu_fam_stm32f1_TIM4_IRQn                   = 30,     /**< TIM4 global Interrupt                                */
    
    hal_cpu_fam_stm32f1_SPI1_IRQn                   = 35,     /**< SPI1 global Interrupt                                */
    hal_cpu_fam_stm32f1_SPI2_IRQn                   = 36,     /**< SPI2 global Interrupt                                */
    
    hal_cpu_fam_stm32f1_TIM5_IRQn                   = 50,     /**< TIM5 global Interrupt                                */
    hal_cpu_fam_stm32f1_SPI3_IRQn                   = 51,     /**< SPI3 global Interrupt                                */
    
    hal_cpu_fam_stm32f1_TIM6_IRQn                   = 54,     /**< TIM6 global Interrupt                                */
    hal_cpu_fam_stm32f1_TIM7_IRQn                   = 55,     /**< TIM7 global Interrupt                                */
    
    hal_cpu_fam_stm32f1_ETH_IRQn                    = 61,     /**< Ethernet global Interrupt                            */
    hal_cpu_fam_stm32f1_CAN2_TX_IRQn                = 63,     /**< CAN2 TX Interrupt                                    */
    hal_cpu_fam_stm32f1_CAN2_RX0_IRQn               = 64,     /**< CAN2 RX0 Interrupt                                   */
    hal_cpu_fam_stm32f1_CAN2_RX1_IRQn               = 65,     /**< CAN2 RX1 Interrupt                                   */
    hal_cpu_fam_stm32f1_CAN2_SCE_IRQn               = 66      /**< CAN2 SCE Interrupt                                   */
 
} hal_cpu_fam_stm32f1_irqn_t;


#if 0
typedef enum
{   // values from stm32f10x_gpio.h
    hal_cpu_fam_stm32f1_gpio_speed_10mhz    = 1,
    hal_cpu_fam_stm32f1_gpio_speed_2mhz,
    hal_cpu_fam_stm32f1_gpio_speed_50mhz
} hal_cpu_fam_stm32f1_gpio_speed_t;
typedef enum
{   // values from stm32f10x_gpio.h
    hal_cpu_fam_stm32f1_gpio_mode_AIN           = 0x0,
    hal_cpu_fam_stm32f1_gpio_mode_IN_FLOATING   = 0x04,
    hal_cpu_fam_stm32f1_gpio_mode_IPD           = 0x28,
    hal_cpu_fam_stm32f1_gpio_mode_IPU           = 0x48,
    hal_cpu_fam_stm32f1_gpio_mode_Out_OD        = 0x14,
    hal_cpu_fam_stm32f1_gpio_mode_Out_PP        = 0x10,
    hal_cpu_fam_stm32f1_gpio_mode_AF_OD         = 0x1C,
    hal_cpu_fam_stm32f1_gpio_mode_AF_PP         = 0x18   
} hal_cpu_fam_stm32f1_gpio_mode_t;
typedef struct
{   // from stm32f10x_gpio.h
    uint16_t                                gpiopin;
    hal_cpu_fam_stm32f1_gpio_speed_t        gpiospeed;
    hal_cpu_fam_stm32f1_gpio_mode_t         gpiomode;
} hal_cpu_fam_stm32f1_gpio_altcfg_t;
#endif
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group doxy_group_hal_cpu_fam_stm32f1  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



