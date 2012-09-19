/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _STM32EE_HID_H_
#define _STM32EE_HID_H_


/* @file       stm32ee_hid.h
    @brief      This header file contains hidden interface for the management of some i2c EEPROMs using stm32 apis.
    @author     marco.accame@iit.it
    @date       09/14/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_stm32xx_include.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "stm32ee.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------
// empty-section

// -- constants
#define sEE_FLAG_TIMEOUT                ((uint32_t)0x00111000)
#define sEE_LONG_TIMEOUT                ((uint32_t)(10 * sEE_FLAG_TIMEOUT))
#define sEE_MAX_TRIALS_NUMBER           300



// -- i2c pinout

#if     defined(USE_STM32F4)

const uint8_t           stm32ee_hid_i2c_portnum         = 1;
const I2C_TypeDef *     stm32ee_hid_i2c_port            = I2C1;
const uint32_t          stm32ee_hid_i2c_clock           = RCC_APB1Periph_I2C1; 

const uint32_t          stm32ee_hid_i2c_gpio_scl_clock  = RCC_AHB1Periph_GPIOB;       
const GPIO_TypeDef *    stm32ee_hid_i2c_gpio_scl_port   = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_scl_pinnum = GPIO_PinSource8;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_scl_pin    =
{
    .GPIO_Pin       = GPIO_Pin_8,
    .GPIO_Mode      = GPIO_Mode_AF,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_OType     = GPIO_OType_OD,
    .GPIO_PuPd      = GPIO_PuPd_NOPULL
};

const uint32_t          stm32ee_hid_i2c_gpio_sda_clock  = RCC_AHB1Periph_GPIOB;       
const GPIO_TypeDef *    stm32ee_hid_i2c_gpio_sda_port   = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_sda_pinnum = GPIO_PinSource9;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_sda_pin    =
{
    .GPIO_Pin       = GPIO_Pin_9,
    .GPIO_Mode      = GPIO_Mode_AF,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_OType     = GPIO_OType_OD,
    .GPIO_PuPd      = GPIO_PuPd_NOPULL
};

const uint32_t          stm32ee_hid_i2c_gpio_remap      = GPIO_AF_I2C1;

#define sEE_I2C                          I2C1
#define sEE_I2C_CLK                      RCC_APB1Periph_I2C1
#define sEE_I2C_SCL_PIN                  GPIO_Pin_8                  /* PB.08 */
#define sEE_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sEE_I2C_SCL_SOURCE               GPIO_PinSource8
#define sEE_I2C_SCL_AF                   GPIO_AF_I2C1
#define sEE_I2C_SDA_PIN                  GPIO_Pin_9                  /* PB.09 */
#define sEE_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB

#define sEE_I2C_SDA_SOURCE               GPIO_PinSource9
#define sEE_I2C_SDA_AF                   GPIO_AF_I2C1

#elif   defined(USE_STM32F1)

const uint8_t           stm32ee_hid_i2c_portnum         = 1;
I2C_TypeDef *           stm32ee_hid_i2c_port            = I2C1;
const uint32_t          stm32ee_hid_i2c_clock           = RCC_APB1Periph_I2C1;  

const uint32_t          stm32ee_hid_i2c_gpio_scl_clock  = RCC_APB2Periph_GPIOB;       
GPIO_TypeDef *          stm32ee_hid_i2c_gpio_scl_port   = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_scl_pinnum = 8;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_scl_pin    =
{
    .GPIO_Pin       = GPIO_Pin_8,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_Mode      = GPIO_Mode_AF_OD
};

const uint32_t          stm32ee_hid_i2c_gpio_sda_clock  = RCC_APB2Periph_GPIOB;       
GPIO_TypeDef *          stm32ee_hid_i2c_gpio_sda_port   = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_sda_pinnum = 9;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_sda_pin    =
{
    .GPIO_Pin       = GPIO_Pin_9,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_Mode      = GPIO_Mode_AF_OD,
};

const uint32_t          stm32ee_hid_i2c_gpio_remap      = GPIO_Remap_I2C1;

#if 1
    #define sEE_I2C                          stm32ee_hid_i2c_port
    #define sEE_I2C_CLK                      stm32ee_hid_i2c_clock
    #define sEE_I2C_SCL_PIN                  stm32ee_hid_i2c_gpio_scl_pin.GPIO_Pin                  /* PB.08 */
    #define sEE_I2C_SCL_GPIO_PORT            stm32ee_hid_i2c_gpio_scl_port                       /* GPIOB */
    #define sEE_I2C_SCL_GPIO_CLK             stm32ee_hid_i2c_gpio_scl_clock
    #define sEE_I2C_SDA_PIN                  stm32ee_hid_i2c_gpio_sda_pin.GPIO_Pin                  /* PB.09 */
    #define sEE_I2C_SDA_GPIO_PORT            stm32ee_hid_i2c_gpio_sda_port                       /* GPIOB */
    #define sEE_I2C_SDA_GPIO_CLK             stm32ee_hid_i2c_gpio_sda_clock
#else
    #define sEE_I2C                          I2C1
    #define sEE_I2C_CLK                      RCC_APB1Periph_I2C1
    #define sEE_I2C_SCL_PIN                  GPIO_Pin_8                  /* PB.08 */
    #define sEE_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
    #define sEE_I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOB
    #define sEE_I2C_SDA_PIN                  GPIO_Pin_9                  /* PB.09 */
    #define sEE_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
    #define sEE_I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOB
#endif

#endif   

// -- i2c generics

#define I2C_SPEED                           400000
#define I2C_SLAVE_ADDRESS7                  0x00
//0xA0

#warning --> ma come mai serve I2C_SLAVE_ADDRESS7 ??? non serve. metti zero.

// -- dma

#if defined(USE_STM32F4)

#define sEE_I2C_DMA                      DMA1   
#define sEE_I2C_DMA_CHANNEL              DMA_Channel_1
#define stm32ee_I2C_DMA_STREAMorCHANNEL_TX            DMA1_Stream6
#define stm32ee_I2C_DMA_STREAMorCHANNEL_RX            DMA1_Stream0   
#define sEE_I2C_DMA_CLK                  RCC_AHB1Periph_DMA1
#define sEE_I2C_DR_Address               ((uint32_t)0x40005410)

#define sEE_I2C_DMA_TX_IRQn              DMA1_Stream6_IRQn
#define sEE_I2C_DMA_RX_IRQn              DMA1_Stream0_IRQn
#define stm32ee_I2C_DMA_TX_IRQHandler    DMA1_Stream6_IRQHandler
#define stm32ee_I2C_DMA_RX_IRQHandler    DMA1_Stream0_IRQHandler   
#define sEE_I2C_DMA_PREPRIO              0
#define sEE_I2C_DMA_SUBPRIO              0   
   
#define sEE_TX_DMA_FLAG_FEIF             DMA_FLAG_FEIF6
#define sEE_TX_DMA_FLAG_DMEIF            DMA_FLAG_DMEIF6
#define sEE_TX_DMA_FLAG_TEIF             DMA_FLAG_TEIF6
#define sEE_TX_DMA_FLAG_HTIF             DMA_FLAG_HTIF6
#define sEE_TX_DMA_FLAG_TCIF             DMA_FLAG_TCIF6
#define sEE_RX_DMA_FLAG_FEIF             DMA_FLAG_FEIF0
#define sEE_RX_DMA_FLAG_DMEIF            DMA_FLAG_DMEIF0
#define sEE_RX_DMA_FLAG_TEIF             DMA_FLAG_TEIF0
#define sEE_RX_DMA_FLAG_HTIF             DMA_FLAG_HTIF0
#define sEE_RX_DMA_FLAG_TCIF             DMA_FLAG_TCIF0

#elif   defined(USE_STM32F1)

#define sEE_I2C_DMA                      DMA1   
#define stm32ee_I2C_DMA_STREAMorCHANNEL_TX           DMA1_Channel6
#define stm32ee_I2C_DMA_STREAMorCHANNEL_RX           DMA1_Channel7 
#define sEE_I2C_DMA_FLAG_TX_TC           DMA1_IT_TC6   
#define sEE_I2C_DMA_FLAG_TX_GL           DMA1_IT_GL6 
#define sEE_I2C_DMA_FLAG_RX_TC           DMA1_IT_TC7 
#define sEE_I2C_DMA_FLAG_RX_GL           DMA1_IT_GL7    
#define sEE_I2C_DMA_CLK                  RCC_AHBPeriph_DMA1
#define sEE_I2C_DR_Address               ((uint32_t)0x40005410)

//#define stm32ee_I2C_DMA_STREAMorCHANNEL_RX           sEE_I2C_DMA_CHANNEL_TX
//#define stm32ee_I2C_DMA_STREAMorCHANNEL_TX           sEE_I2C_DMA_CHANNEL_TX
   
#define sEE_I2C_DMA_TX_IRQn              DMA1_Channel6_IRQn
#define sEE_I2C_DMA_RX_IRQn              DMA1_Channel7_IRQn
#define stm32ee_I2C_DMA_TX_IRQHandler    DMA1_Channel6_IRQHandler
#define stm32ee_I2C_DMA_RX_IRQHandler    DMA1_Channel7_IRQHandler   
#define sEE_I2C_DMA_PREPRIO              0
#define sEE_I2C_DMA_SUBPRIO              0   

#endif

// - definition of hidden structs -------------------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------

//extern uint32_t stm32ee_hid_myfunction(void);

// - definition of extern hidden inline functions ---------------------------------------------------------------------
// empty-section

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




