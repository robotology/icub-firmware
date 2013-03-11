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

#ifndef _HAL_CPU_NAM_STM32F407_H_
#define _HAL_CPU_NAM_STM32F407_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_cpu_nam_stm32f407.h
    @brief      This header file keeps public interface to the hal parts which are specific of cpu family: stm32f407
    @author     marco.accame@iit.it
    @date       02/27/2013
**/

/** @defgroup doxy_group_hal_cpu_nam_stm32f407 HAL CPU STM32F107

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


/** @typedef    typedef enum hal_cpu_nam_stm32f407_irqn_t
    @brief      contains the irq numbers for the stm32f1x family of processors.
 **/
typedef enum 
{
/******  Cortex-M4 Processor Exceptions Numbers ****************************************************************/
  hal_cpu_nam_stm32f407_NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                          */
  hal_cpu_nam_stm32f407_MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M4 Memory Management Interrupt                           */
  hal_cpu_nam_stm32f407_BusFault_IRQn               = -11,    /*!< 5 Cortex-M4 Bus Fault Interrupt                                   */
  hal_cpu_nam_stm32f407_UsageFault_IRQn             = -10,    /*!< 6 Cortex-M4 Usage Fault Interrupt                                 */
  hal_cpu_nam_stm32f407_SVCall_IRQn                 = -5,     /*!< 11 Cortex-M4 SV Call Interrupt                                    */
  hal_cpu_nam_stm32f407_DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M4 Debug Monitor Interrupt                              */
  hal_cpu_nam_stm32f407_PendSV_IRQn                 = -2,     /*!< 14 Cortex-M4 Pend SV Interrupt                                    */
  hal_cpu_nam_stm32f407_SysTick_IRQn                = -1,     /*!< 15 Cortex-M4 System Tick Interrupt                                */
/******  STM32 specific Interrupt Numbers **********************************************************************/
  hal_cpu_nam_stm32f407_WWDG_IRQn                   = 0,      /*!< Window WatchDog Interrupt                                         */
  hal_cpu_nam_stm32f407_PVD_IRQn                    = 1,      /*!< PVD through EXTI Line detection Interrupt                         */
  hal_cpu_nam_stm32f407_TAMP_STAMP_IRQn             = 2,      /*!< Tamper and TimeStamp interrupts through the EXTI line             */
  hal_cpu_nam_stm32f407_RTC_WKUP_IRQn               = 3,      /*!< RTC Wakeup interrupt through the EXTI line                        */
  hal_cpu_nam_stm32f407_FLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                                            */
  hal_cpu_nam_stm32f407_RCC_IRQn                    = 5,      /*!< RCC global Interrupt                                              */
  hal_cpu_nam_stm32f407_EXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                              */
  hal_cpu_nam_stm32f407_EXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                              */
  hal_cpu_nam_stm32f407_EXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                              */
  hal_cpu_nam_stm32f407_EXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                              */
  hal_cpu_nam_stm32f407_EXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                              */
  hal_cpu_nam_stm32f407_DMA1_Stream0_IRQn           = 11,     /*!< DMA1 Stream 0 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA1_Stream1_IRQn           = 12,     /*!< DMA1 Stream 1 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA1_Stream2_IRQn           = 13,     /*!< DMA1 Stream 2 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA1_Stream3_IRQn           = 14,     /*!< DMA1 Stream 3 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA1_Stream4_IRQn           = 15,     /*!< DMA1 Stream 4 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA1_Stream5_IRQn           = 16,     /*!< DMA1 Stream 5 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA1_Stream6_IRQn           = 17,     /*!< DMA1 Stream 6 global Interrupt                                    */
  hal_cpu_nam_stm32f407_ADC_IRQn                    = 18,     /*!< ADC1, ADC2 and ADC3 global Interrupts                             */
  hal_cpu_nam_stm32f407_CAN1_TX_IRQn                = 19,     /*!< CAN1 TX Interrupt                                                 */
  hal_cpu_nam_stm32f407_CAN1_RX0_IRQn               = 20,     /*!< CAN1 RX0 Interrupt                                                */
  hal_cpu_nam_stm32f407_CAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                                */
  hal_cpu_nam_stm32f407_CAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                                */
  hal_cpu_nam_stm32f407_EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
  hal_cpu_nam_stm32f407_TIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
  hal_cpu_nam_stm32f407_TIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
  hal_cpu_nam_stm32f407_TIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
  hal_cpu_nam_stm32f407_TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
  hal_cpu_nam_stm32f407_TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
  hal_cpu_nam_stm32f407_TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
  hal_cpu_nam_stm32f407_TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
  hal_cpu_nam_stm32f407_I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
  hal_cpu_nam_stm32f407_I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
  hal_cpu_nam_stm32f407_I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
  hal_cpu_nam_stm32f407_I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */
  hal_cpu_nam_stm32f407_SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
  hal_cpu_nam_stm32f407_SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
  hal_cpu_nam_stm32f407_USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
  hal_cpu_nam_stm32f407_USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
  hal_cpu_nam_stm32f407_USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                                           */
  hal_cpu_nam_stm32f407_EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
  hal_cpu_nam_stm32f407_RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
  hal_cpu_nam_stm32f407_OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
  hal_cpu_nam_stm32f407_TIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
  hal_cpu_nam_stm32f407_TIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
  hal_cpu_nam_stm32f407_TIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
  hal_cpu_nam_stm32f407_TIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
  hal_cpu_nam_stm32f407_FSMC_IRQn                   = 48,     /*!< FSMC global Interrupt                                             */
  hal_cpu_nam_stm32f407_SDIO_IRQn                   = 49,     /*!< SDIO global Interrupt                                             */
  hal_cpu_nam_stm32f407_TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
  hal_cpu_nam_stm32f407_SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
  hal_cpu_nam_stm32f407_UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                                            */
  hal_cpu_nam_stm32f407_UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                                            */
  hal_cpu_nam_stm32f407_TIM6_DAC_IRQn               = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
  hal_cpu_nam_stm32f407_TIM7_IRQn                   = 55,     /*!< TIM7 global interrupt                                             */
  hal_cpu_nam_stm32f407_DMA2_Stream0_IRQn           = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA2_Stream1_IRQn           = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA2_Stream2_IRQn           = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA2_Stream3_IRQn           = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
  hal_cpu_nam_stm32f407_DMA2_Stream4_IRQn           = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
  hal_cpu_nam_stm32f407_ETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                                         */
  hal_cpu_nam_stm32f407_ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
  hal_cpu_nam_stm32f407_CAN2_TX_IRQn                = 63,     /*!< CAN2 TX Interrupt                                                 */
  hal_cpu_nam_stm32f407_CAN2_RX0_IRQn               = 64,     /*!< CAN2 RX0 Interrupt                                                */
  hal_cpu_nam_stm32f407_CAN2_RX1_IRQn               = 65,     /*!< CAN2 RX1 Interrupt                                                */
  hal_cpu_nam_stm32f407_CAN2_SCE_IRQn               = 66,     /*!< CAN2 SCE Interrupt                                                */
  hal_cpu_nam_stm32f407_OTG_FS_IRQn                 = 67,     /*!< USB OTG FS global Interrupt                                       */
  hal_cpu_nam_stm32f407_DMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
  hal_cpu_nam_stm32f407_DMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
  hal_cpu_nam_stm32f407_DMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
  hal_cpu_nam_stm32f407_USART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
  hal_cpu_nam_stm32f407_I2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
  hal_cpu_nam_stm32f407_I2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
  hal_cpu_nam_stm32f407_OTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
  hal_cpu_nam_stm32f407_OTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
  hal_cpu_nam_stm32f407_OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
  hal_cpu_nam_stm32f407_OTG_HS_IRQn                 = 77,     /*!< USB OTG HS global interrupt                                       */
  hal_cpu_nam_stm32f407_DCMI_IRQn                   = 78,     /*!< DCMI global interrupt                                             */
  hal_cpu_nam_stm32f407_CRYP_IRQn                   = 79,     /*!< CRYP crypto global interrupt                                      */
  hal_cpu_nam_stm32f407_HASH_RNG_IRQn               = 80,      /*!< Hash and Rng global interrupt                                     */
  hal_cpu_nam_stm32f407_FPU_IRQn                    = 81      /*!< FPU global interrupt                                              */
 
} hal_cpu_nam_stm32f407_irqn_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group doxy_group_hal_cpu_nam_stm32f407  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



