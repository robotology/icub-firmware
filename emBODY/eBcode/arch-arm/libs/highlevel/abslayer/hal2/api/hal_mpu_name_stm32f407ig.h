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

#ifndef _HAL_MPU_NAME_STM32F407IG_H_
#define _HAL_MPU_NAME_STM32F407IG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_mpu_name_stm32f407ig.h
    @brief      This header file keeps public interface to the hal parts which are specific of cpu stm32f407ig
    @author     marco.accame@iit.it
    @date       02/27/2013
**/

/** @defgroup doxy_group_hal_mpu_name_stm32f407ig HAL CPU STM32F407IG

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/



// - public #define  --------------------------------------------------------------------------------------------------

//#warning --> maybe change it 

// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hal_mpu.h"
#include "hal_mpu_arch_armcmx.h"



// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_mpu_name_stm32f407ig_irqn_t
    @brief      contains the irq numbers for the stm32f407ig processor
 **/
typedef enum 
{
    // Cortex-M4 Processor Exceptions Numbers 
    hal_mpu_name_stm32f407ig_NonMaskableInt_IRQn        = hal_mpu_arch_armcmx_NonMaskableInt_IRQn,     /**< -14: Non Maskable Interrupt                    */
    hal_mpu_name_stm32f407ig_MemoryManagement_IRQn      = hal_mpu_arch_armcmx_MemoryManagement_IRQn,   /**< -12: Cortex-Mx Memory Management Interrupt     */
    hal_mpu_name_stm32f407ig_BusFault_IRQn              = hal_mpu_arch_armcmx_BusFault_IRQn,           /**< -11: Cortex-Mx Bus Fault Interrupt             */
    hal_mpu_name_stm32f407ig_UsageFault_IRQn            = hal_mpu_arch_armcmx_UsageFault_IRQn,         /**< -10: Cortex-Mx Usage Fault Interrupt           */
    hal_mpu_name_stm32f407ig_SVCall_IRQn                = hal_mpu_arch_armcmx_SVCall_IRQn,             /**<  -5: Cortex-Mx SV Call Interrupt               */
    hal_mpu_name_stm32f407ig_DebugMonitor_IRQn          = hal_mpu_arch_armcmx_DebugMonitor_IRQn,       /**<  -4: Cortex-Mx Debug Monitor Interrupt         */
    hal_mpu_name_stm32f407ig_PendSV_IRQn                = hal_mpu_arch_armcmx_PendSV_IRQn,             /**<  -2: Cortex-Mx Pend SV Interrupt               */
    hal_mpu_name_stm32f407ig_SysTick_IRQn               = hal_mpu_arch_armcmx_SysTick_IRQn,            /**<  -1: Cortex-Mx System Tick Interrupt           */
    // STM32F4 specific Interrupt Numbers 
    hal_mpu_name_stm32f407ig_WWDG_IRQn                  = 0,      /*!< Window WatchDog Interrupt                                         */
    hal_mpu_name_stm32f407igPVD_IRQn                    = 1,      /*!< PVD through EXTI Line detection Interrupt                         */
    hal_mpu_name_stm32f407igTAMP_STAMP_IRQn             = 2,      /*!< Tamper and TimeStamp interrupts through the EXTI line             */
    hal_mpu_name_stm32f407igRTC_WKUP_IRQn               = 3,      /*!< RTC Wakeup interrupt through the EXTI line                        */
    hal_mpu_name_stm32f407igFLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                                            */
    hal_mpu_name_stm32f407igRCC_IRQn                    = 5,      /*!< RCC global Interrupt                                              */
    hal_mpu_name_stm32f407igEXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                              */
    hal_mpu_name_stm32f407igEXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                              */
    hal_mpu_name_stm32f407igEXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                              */
    hal_mpu_name_stm32f407igEXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                              */
    hal_mpu_name_stm32f407igEXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                              */
    hal_mpu_name_stm32f407igDMA1_Stream0_IRQn           = 11,     /*!< DMA1 Stream 0 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA1_Stream1_IRQn           = 12,     /*!< DMA1 Stream 1 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA1_Stream2_IRQn           = 13,     /*!< DMA1 Stream 2 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA1_Stream3_IRQn           = 14,     /*!< DMA1 Stream 3 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA1_Stream4_IRQn           = 15,     /*!< DMA1 Stream 4 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA1_Stream5_IRQn           = 16,     /*!< DMA1 Stream 5 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA1_Stream6_IRQn           = 17,     /*!< DMA1 Stream 6 global Interrupt                                    */
    hal_mpu_name_stm32f407igADC_IRQn                    = 18,     /*!< ADC1, ADC2 and ADC3 global Interrupts                             */  
    // STM32F40_41xxx specific Interrupt Numbers 
    hal_mpu_name_stm32f407igCAN1_TX_IRQn                = 19,     /*!< CAN1 TX Interrupt                                                 */
    hal_mpu_name_stm32f407igCAN1_RX0_IRQn               = 20,     /*!< CAN1 RX0 Interrupt                                                */
    hal_mpu_name_stm32f407igCAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                                */
    hal_mpu_name_stm32f407igCAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                                */
    hal_mpu_name_stm32f407igEXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
    hal_mpu_name_stm32f407igTIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
    hal_mpu_name_stm32f407igTIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
    hal_mpu_name_stm32f407igTIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
    hal_mpu_name_stm32f407igTIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
    hal_mpu_name_stm32f407igTIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
    hal_mpu_name_stm32f407igTIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
    hal_mpu_name_stm32f407igTIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
    hal_mpu_name_stm32f407igI2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
    hal_mpu_name_stm32f407igI2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
    hal_mpu_name_stm32f407igI2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
    hal_mpu_name_stm32f407igI2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */
    hal_mpu_name_stm32f407igSPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
    hal_mpu_name_stm32f407igSPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
    hal_mpu_name_stm32f407igUSART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
    hal_mpu_name_stm32f407igUSART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
    hal_mpu_name_stm32f407igUSART3_IRQn                 = 39,     /*!< USART3 global Interrupt                                           */
    hal_mpu_name_stm32f407igEXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
    hal_mpu_name_stm32f407igRTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
    hal_mpu_name_stm32f407igOTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
    hal_mpu_name_stm32f407igTIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
    hal_mpu_name_stm32f407igTIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
    hal_mpu_name_stm32f407igTIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    hal_mpu_name_stm32f407igTIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
    hal_mpu_name_stm32f407igDMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
    hal_mpu_name_stm32f407igFSMC_IRQn                   = 48,     /*!< FSMC global Interrupt                                             */
    hal_mpu_name_stm32f407igSDIO_IRQn                   = 49,     /*!< SDIO global Interrupt                                             */
    hal_mpu_name_stm32f407igTIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
    hal_mpu_name_stm32f407igSPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
    hal_mpu_name_stm32f407igUART4_IRQn                  = 52,     /*!< UART4 global Interrupt                                            */
    hal_mpu_name_stm32f407igUART5_IRQn                  = 53,     /*!< UART5 global Interrupt                                            */
    hal_mpu_name_stm32f407igTIM6_DAC_IRQn               = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
    hal_mpu_name_stm32f407igTIM7_IRQn                   = 55,     /*!< TIM7 global interrupt                                             */
    hal_mpu_name_stm32f407igDMA2_Stream0_IRQn           = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA2_Stream1_IRQn           = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA2_Stream2_IRQn           = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA2_Stream3_IRQn           = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
    hal_mpu_name_stm32f407igDMA2_Stream4_IRQn           = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
    hal_mpu_name_stm32f407igETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                                         */
    hal_mpu_name_stm32f407igETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
    hal_mpu_name_stm32f407igCAN2_TX_IRQn                = 63,     /*!< CAN2 TX Interrupt                                                 */
    hal_mpu_name_stm32f407igCAN2_RX0_IRQn               = 64,     /*!< CAN2 RX0 Interrupt                                                */
    hal_mpu_name_stm32f407igCAN2_RX1_IRQn               = 65,     /*!< CAN2 RX1 Interrupt                                                */
    hal_mpu_name_stm32f407igCAN2_SCE_IRQn               = 66,     /*!< CAN2 SCE Interrupt                                                */
    hal_mpu_name_stm32f407igOTG_FS_IRQn                 = 67,     /*!< USB OTG FS global Interrupt                                       */
    hal_mpu_name_stm32f407igDMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
    hal_mpu_name_stm32f407igDMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
    hal_mpu_name_stm32f407igDMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
    hal_mpu_name_stm32f407igUSART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
    hal_mpu_name_stm32f407igI2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
    hal_mpu_name_stm32f407igI2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
    hal_mpu_name_stm32f407igOTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
    hal_mpu_name_stm32f407igOTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
    hal_mpu_name_stm32f407igOTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
    hal_mpu_name_stm32f407igOTG_HS_IRQn                 = 77,     /*!< USB OTG HS global interrupt                                       */
    hal_mpu_name_stm32f407igDCMI_IRQn                   = 78,     /*!< DCMI global interrupt                                             */
    hal_mpu_name_stm32f407igCRYP_IRQn                   = 79,     /*!< CRYP crypto global interrupt                                      */
    hal_mpu_name_stm32f407igHASH_RNG_IRQn               = 80,      /*!< Hash and Rng global interrupt                                     */
    hal_mpu_name_stm32f407igFPU_IRQn                    = 81      /*!< FPU global interrupt                                              */
 
} hal_mpu_name_stm32f407ig_irqn_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group doxy_group_hal_mpu_name_stm32f407ig  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



