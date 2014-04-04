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

#ifndef _HAL_MPU_NAM_STM32F107VC_H_
#define _HAL_MPU_NAM_STM32F107VC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_mpu_name_stm32f107vc.h
    @brief      This header file keeps public interface to the hal parts which are specific of cpu family: stm32f107
    @author     marco.accame@iit.it
    @date       02/27/2013
**/

/** @defgroup doxy_group_hal_mpu_name_stm32f107vc HAL CPU STM32F107VC

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/



// - public #define  --------------------------------------------------------------------------------------------------

//#warning --> maybe change it 

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"

#include "hal_mpu.h"
#include "hal_mpu_arch_armcmx.h"


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_mpu_name_stm32f107vc_irqn_t
    @brief      contains the irq numbers for the stm32f107vc family of processors.
 **/
typedef enum 
{
    // Cortex-M3 Processor Exceptions Numbers 
    hal_mpu_name_stm32f107vc_NonMaskableInt_IRQn         = hal_mpu_arch_armcmx_NonMaskableInt_IRQn,     /**< -14: Non Maskable Interrupt                    */
    hal_mpu_name_stm32f107vc_MemoryManagement_IRQn       = hal_mpu_arch_armcmx_MemoryManagement_IRQn,   /**< -12: Cortex-Mx Memory Management Interrupt     */
    hal_mpu_name_stm32f107vc_BusFault_IRQn               = hal_mpu_arch_armcmx_BusFault_IRQn,           /**< -11: Cortex-Mx Bus Fault Interrupt             */
    hal_mpu_name_stm32f107vc_UsageFault_IRQn             = hal_mpu_arch_armcmx_UsageFault_IRQn,         /**< -10: Cortex-Mx Usage Fault Interrupt           */
    hal_mpu_name_stm32f107vc_SVCall_IRQn                 = hal_mpu_arch_armcmx_SVCall_IRQn,             /**<  -5: Cortex-Mx SV Call Interrupt               */
    hal_mpu_name_stm32f107vc_DebugMonitor_IRQn           = hal_mpu_arch_armcmx_DebugMonitor_IRQn,       /**<  -4: Cortex-Mx Debug Monitor Interrupt         */
    hal_mpu_name_stm32f107vc_PendSV_IRQn                 = hal_mpu_arch_armcmx_PendSV_IRQn,             /**<  -2: Cortex-Mx Pend SV Interrupt               */
    hal_mpu_name_stm32f107vc_SysTick_IRQn                = hal_mpu_arch_armcmx_SysTick_IRQn,            /**<  -1: Cortex-Mx System Tick Interrupt           */
    // STM32F1 specific Interrupt Numbers 
    hal_mpu_name_stm32f107vc_WWDG_IRQn                   = 0,      /**< Window WatchDog Interrupt                            */
    hal_mpu_name_stm32f107vc_PVD_IRQn                    = 1,      /*!< PVD through EXTI Line detection Interrupt            */
    hal_mpu_name_stm32f107vc_TAMPER_IRQn                 = 2,      /*!< Tamper Interrupt                                     */
    hal_mpu_name_stm32f107vc_RTC_IRQn                    = 3,      /*!< RTC global Interrupt                                 */
    hal_mpu_name_stm32f107vc_FLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                               */
    hal_mpu_name_stm32f107vc_RCC_IRQn                    = 5,      /*!< RCC global Interrupt                                 */
    hal_mpu_name_stm32f107vc_EXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                 */
    hal_mpu_name_stm32f107vc_EXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                 */
    hal_mpu_name_stm32f107vc_EXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                 */
    hal_mpu_name_stm32f107vc_EXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                 */
    hal_mpu_name_stm32f107vc_EXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                 */
    hal_mpu_name_stm32f107vc_DMA1_Channel1_IRQn          = 11,     /*!< DMA1 Channel 1 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA1_Channel2_IRQn          = 12,     /*!< DMA1 Channel 2 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA1_Channel3_IRQn          = 13,     /*!< DMA1 Channel 3 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA1_Channel4_IRQn          = 14,     /*!< DMA1 Channel 4 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA1_Channel5_IRQn          = 15,     /*!< DMA1 Channel 5 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA1_Channel6_IRQn          = 16,     /*!< DMA1 Channel 6 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA1_Channel7_IRQn          = 17,     /*!< DMA1 Channel 7 global Interrupt                      */
    // STM32F10X_CL (stm32f107) specific Interrupt Numbers
    hal_mpu_name_stm32f107vc_ADC1_2_IRQn                 = 18,     /*!< ADC1 and ADC2 global Interrupt                       */
    hal_mpu_name_stm32f107vc_CAN1_TX_IRQn                = 19,     /*!< USB Device High Priority or CAN1 TX Interrupts       */
    hal_mpu_name_stm32f107vc_CAN1_RX0_IRQn               = 20,     /*!< USB Device Low Priority or CAN1 RX0 Interrupts       */
    hal_mpu_name_stm32f107vc_CAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                   */
    hal_mpu_name_stm32f107vc_CAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                   */
    hal_mpu_name_stm32f107vc_EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                        */
    hal_mpu_name_stm32f107vc_TIM1_BRK_IRQn               = 24,     /*!< TIM1 Break Interrupt                                 */
    hal_mpu_name_stm32f107vc_TIM1_UP_IRQn                = 25,     /*!< TIM1 Update Interrupt                                */
    hal_mpu_name_stm32f107vc_TIM1_TRG_COM_IRQn           = 26,     /*!< TIM1 Trigger and Commutation Interrupt               */
    hal_mpu_name_stm32f107vc_TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                       */
    hal_mpu_name_stm32f107vc_TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                */
    hal_mpu_name_stm32f107vc_TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                */
    hal_mpu_name_stm32f107vc_TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                */
    hal_mpu_name_stm32f107vc_I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                 */
    hal_mpu_name_stm32f107vc_I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                 */
    hal_mpu_name_stm32f107vc_I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                 */
    hal_mpu_name_stm32f107vc_I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                 */
    hal_mpu_name_stm32f107vc_SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                */
    hal_mpu_name_stm32f107vc_SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                */
    hal_mpu_name_stm32f107vc_USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                              */
    hal_mpu_name_stm32f107vc_USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                              */
    hal_mpu_name_stm32f107vc_USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                              */
    hal_mpu_name_stm32f107vc_EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                      */
    hal_mpu_name_stm32f107vc_RTCAlarm_IRQn               = 41,     /*!< RTC Alarm through EXTI Line Interrupt                */
    hal_mpu_name_stm32f107vc_OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS WakeUp from suspend through EXTI Line Interrupt */
    hal_mpu_name_stm32f107vc_TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                */
    hal_mpu_name_stm32f107vc_SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                */
    hal_mpu_name_stm32f107vc_UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                               */
    hal_mpu_name_stm32f107vc_UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                               */
    hal_mpu_name_stm32f107vc_TIM6_IRQn                   = 54,     /*!< TIM6 global Interrupt                                */
    hal_mpu_name_stm32f107vc_TIM7_IRQn                   = 55,     /*!< TIM7 global Interrupt                                */
    hal_mpu_name_stm32f107vc_DMA2_Channel1_IRQn          = 56,     /*!< DMA2 Channel 1 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA2_Channel2_IRQn          = 57,     /*!< DMA2 Channel 2 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA2_Channel3_IRQn          = 58,     /*!< DMA2 Channel 3 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA2_Channel4_IRQn          = 59,     /*!< DMA2 Channel 4 global Interrupt                      */
    hal_mpu_name_stm32f107vc_DMA2_Channel5_IRQn          = 60,     /*!< DMA2 Channel 5 global Interrupt                      */
    hal_mpu_name_stm32f107vc_ETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                            */
    hal_mpu_name_stm32f107vc_ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt          */
    hal_mpu_name_stm32f107vc_CAN2_TX_IRQn                = 63,     /*!< CAN2 TX Interrupt                                    */
    hal_mpu_name_stm32f107vc_CAN2_RX0_IRQn               = 64,     /*!< CAN2 RX0 Interrupt                                   */
    hal_mpu_name_stm32f107vc_CAN2_RX1_IRQn               = 65,     /*!< CAN2 RX1 Interrupt                                   */
    hal_mpu_name_stm32f107vc_CAN2_SCE_IRQn               = 66,     /*!< CAN2 SCE Interrupt                                   */
    hal_mpu_name_stm32f107vc_OTG_FS_IRQn                 = 67      /*!< USB OTG FS global Interrupt                          */
 
} hal_mpu_name_stm32f107vc_irqn_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group doxy_group_hal_mpu_name_stm32f107vc  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



