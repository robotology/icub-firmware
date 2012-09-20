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


#define stm32ee_BASICTIMEOUT                ((uint32_t)0x00111000)
const uint32_t          stm32ee_hid_timeout_flag            = stm32ee_BASICTIMEOUT;
const uint32_t          stm32ee_hid_timeout_long            = 10 * stm32ee_BASICTIMEOUT;
const uint32_t          stm32ee_hid_ackaddress_maxtrials    = 300;


const uint8_t           stm32ee_hid_hwaddressbits1and2      = (0 << 2) | (0 << 1);


#if     defined(USE_STM32F4)

// -- i2c

const uint8_t           stm32ee_hid_i2c_portnum             = 1;
I2C_TypeDef *           stm32ee_hid_i2c_port                = I2C1;
const uint32_t          stm32ee_hid_i2c_clock               = RCC_APB1Periph_I2C1;  

// INTERNAL: static non const or ..  const but use a temporary var 
const I2C_InitTypeDef   stm32ee_hid_i2c_cfg                 =
{
    .I2C_Mode                   = I2C_Mode_I2C,
    .I2C_DutyCycle              = I2C_DutyCycle_2,
    .I2C_OwnAddress1            = 0,
    .I2C_Ack                    = I2C_Ack_Enable,
    .I2C_AcknowledgedAddress    = I2C_AcknowledgedAddress_7bit,
    .I2C_ClockSpeed             = 400000            // PARAMETER
};

const uint32_t          stm32ee_hid_i2c_gpio_scl_clock      = RCC_AHB1Periph_GPIOB;       
GPIO_TypeDef *          stm32ee_hid_i2c_gpio_scl_port       = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_scl_pinnum     = GPIO_PinSource8;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_scl_pin        =
{
    .GPIO_Pin       = GPIO_Pin_8,
    .GPIO_Mode      = GPIO_Mode_AF,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_OType     = GPIO_OType_OD,
    .GPIO_PuPd      = GPIO_PuPd_NOPULL
};
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_scl_floatingpin =
{
    .GPIO_Pin       = GPIO_Pin_8,
    .GPIO_Mode      = GPIO_Mode_IN,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_OType     = GPIO_OType_PP,
    .GPIO_PuPd      = GPIO_PuPd_NOPULL
};

const uint32_t          stm32ee_hid_i2c_gpio_sda_clock      = RCC_AHB1Periph_GPIOB;       
GPIO_TypeDef *          stm32ee_hid_i2c_gpio_sda_port       = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_sda_pinnum     = GPIO_PinSource9;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_sda_pin        =
{
    .GPIO_Pin       = GPIO_Pin_9,
    .GPIO_Mode      = GPIO_Mode_AF,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_OType     = GPIO_OType_OD,
    .GPIO_PuPd      = GPIO_PuPd_NOPULL
};
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_sda_floatingpin =
{
    .GPIO_Pin       = GPIO_Pin_9,
    .GPIO_Mode      = GPIO_Mode_IN,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_OType     = GPIO_OType_PP,
    .GPIO_PuPd      = GPIO_PuPd_NOPULL
};

const uint32_t          stm32ee_hid_i2c_gpio_remap_clock    = RCC_APB2Periph_SYSCFG;  
const uint32_t          stm32ee_hid_i2c_gpio_remap          = GPIO_AF_I2C1;


// -- dma

// PARAM
//const uint8_t  stm32ee_hid_dma_useit    = 0;

const NVIC_InitTypeDef  stm32ee_hid_dma_nvic_tx_enable  =
{
    .NVIC_IRQChannel                    = DMA1_Stream6_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

const NVIC_InitTypeDef  stm32ee_hid_dma_nvic_rx_enable  =
{
    .NVIC_IRQChannel                    = DMA1_Stream0_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

const NVIC_InitTypeDef  stm32ee_hid_dma_nvic_tx_disable =
{
    .NVIC_IRQChannel                    = DMA1_Stream6_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = DISABLE
};

const NVIC_InitTypeDef  stm32ee_hid_dma_nvic_rx_disable =
{
    .NVIC_IRQChannel                    = DMA1_Stream0_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = DISABLE
};

const uint32_t          stm32ee_hid_dma_clock       = RCC_AHB1Periph_DMA1; 

DMA_TypeDef *           stm32ee_hid_dma             = DMA1;
DMA_Stream_TypeDef *    stm32ee_hid_dma_stream_tx   = DMA1_Stream6;
DMA_Stream_TypeDef *    stm32ee_hid_dma_stream_rx   = DMA1_Stream0;

#define stm32ee_hid_dma_I2C_DMA_TX_IRQHandler    DMA1_Stream6_IRQHandler
#define stm32ee_hid_dma_I2C_DMA_RX_IRQHandler    DMA1_Stream0_IRQHandler   

const DMA_InitTypeDef         stm32ee_hid_dma_cfg_init    =
{
    .DMA_Channel            = DMA_Channel_1,
    .DMA_PeripheralBaseAddr = ((uint32_t)0x40005410),
    .DMA_Memory0BaseAddr    = (uint32_t)0,                  // to be configured during communication 
    .DMA_DIR                = DMA_DIR_MemoryToPeripheral,   // to be configured during communication 
    .DMA_BufferSize         = 0xFFFF,                       // to be configured during communication 
    .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
    .DMA_MemoryInc          = DMA_MemoryInc_Enable,
    .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
    .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
    .DMA_Mode               = DMA_Mode_Normal,
    .DMA_Priority           = DMA_Priority_VeryHigh,
    .DMA_FIFOMode           = DMA_FIFOMode_Enable,
    .DMA_FIFOThreshold      = DMA_FIFOThreshold_Full,
    .DMA_MemoryBurst        = DMA_MemoryBurst_Single,
    .DMA_PeripheralBurst    = DMA_PeripheralBurst_Single
};

const uint32_t          stm32ee_hid_dma_flags_tx                = (DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6);
const uint32_t          stm32ee_hid_dma_flags_rx                = (DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0 | DMA_FLAG_TCIF0);

const uint32_t          stm32ee_hid_dma_flags_tx_completed      = (DMA_FLAG_TCIF6);
const uint32_t          stm32ee_hid_dma_flags_rx_completed      = (DMA_FLAG_TCIF0);

// INTERNAL
const uint32_t          stm32ee_hid_dma_dir_MEMORY2PERIPHERAL   = DMA_DIR_MemoryToPeripheral;
const uint32_t          stm32ee_hid_dma_dir_PERIPHERAL2MEMORY   = DMA_DIR_PeripheralToMemory;



#elif   defined(USE_STM32F1)

// -- i2c

const uint8_t           stm32ee_hid_i2c_portnum             = 1;
I2C_TypeDef *           stm32ee_hid_i2c_port                = I2C1;
const uint32_t          stm32ee_hid_i2c_clock               = RCC_APB1Periph_I2C1;  

// INTERNAL: static non const or ..  const but use a temporary var 
const I2C_InitTypeDef   stm32ee_hid_i2c_cfg                 =
{
    .I2C_Mode                   = I2C_Mode_I2C,
    .I2C_DutyCycle              = I2C_DutyCycle_2,
    .I2C_OwnAddress1            = 0,
    .I2C_Ack                    = I2C_Ack_Enable,
    .I2C_AcknowledgedAddress    = I2C_AcknowledgedAddress_7bit,
    .I2C_ClockSpeed             = 400000            // PARAMETER
};

const uint32_t          stm32ee_hid_i2c_gpio_scl_clock      = RCC_APB2Periph_GPIOB;       
GPIO_TypeDef *          stm32ee_hid_i2c_gpio_scl_port       = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_scl_pinnum     = 8;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_scl_pin        =
{
    .GPIO_Pin       = GPIO_Pin_8,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_Mode      = GPIO_Mode_AF_OD
};
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_scl_floatingpin =
{
    .GPIO_Pin       = GPIO_Pin_8,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_Mode      = GPIO_Mode_IN_FLOATING,
};

const uint32_t          stm32ee_hid_i2c_gpio_sda_clock      = RCC_APB2Periph_GPIOB;       
GPIO_TypeDef *          stm32ee_hid_i2c_gpio_sda_port       = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_sda_pinnum     = 9;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_sda_pin        =
{
    .GPIO_Pin       = GPIO_Pin_9,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_Mode      = GPIO_Mode_AF_OD,
};
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_sda_floatingpin =
{
    .GPIO_Pin       = GPIO_Pin_9,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_Mode      = GPIO_Mode_IN_FLOATING,
};

const uint32_t          stm32ee_hid_i2c_gpio_remap_clock    = RCC_APB2Periph_AFIO;  
const uint32_t          stm32ee_hid_i2c_gpio_remap          = GPIO_Remap_I2C1;

// -- dma

// PARAM
//const uint8_t  stm32ee_hid_dma_useit    = 0;

const NVIC_InitTypeDef  stm32ee_hid_dma_nvic_tx_enable  =
{
    .NVIC_IRQChannel                    = DMA1_Channel6_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

const NVIC_InitTypeDef  stm32ee_hid_dma_nvic_rx_enable  =
{
    .NVIC_IRQChannel                    = DMA1_Channel7_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

const NVIC_InitTypeDef  stm32ee_hid_dma_nvic_tx_disable =
{
    .NVIC_IRQChannel                    = DMA1_Channel6_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = DISABLE
};

const NVIC_InitTypeDef  stm32ee_hid_dma_nvic_rx_disable =
{
    .NVIC_IRQChannel                    = DMA1_Channel7_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = DISABLE
};

const uint32_t          stm32ee_hid_dma_clock       = RCC_AHBPeriph_DMA1; 

DMA_TypeDef *           stm32ee_hid_dma             = DMA1;
DMA_Channel_TypeDef *   stm32ee_hid_dma_stream_tx   = DMA1_Channel6;
DMA_Channel_TypeDef *   stm32ee_hid_dma_stream_rx   = DMA1_Channel7;

#define stm32ee_hid_dma_I2C_DMA_TX_IRQHandler    DMA1_Channel6_IRQHandler
#define stm32ee_hid_dma_I2C_DMA_RX_IRQHandler    DMA1_Channel7_IRQHandler   

const DMA_InitTypeDef         stm32ee_hid_dma_cfg_init    =
{
    .DMA_PeripheralBaseAddr = ((uint32_t)0x40005410),
    .DMA_MemoryBaseAddr     = (uint32_t)0,              /* This parameter will be configured durig communication */
    .DMA_DIR                = DMA_DIR_PeripheralDST,    /* This parameter will be configured durig communication */
    .DMA_BufferSize         = 0xFFFF,                   /* This parameter will be configured durig communication */
    .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
    .DMA_MemoryInc          = DMA_MemoryInc_Enable,
    .DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte,
    .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
    .DMA_Mode               = DMA_Mode_Normal,
    .DMA_Priority           = DMA_Priority_VeryHigh,
    .DMA_M2M                = DMA_M2M_Disable    
};

//const uint32_t          stm32ee_hid_dma_flags_tx                = (DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6);
//const uint32_t          stm32ee_hid_dma_flags_rx                = (DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0 | DMA_FLAG_TCIF0);

const uint32_t          stm32ee_hid_dma_flags_tx_completed      = (DMA1_IT_TC6);
const uint32_t          stm32ee_hid_dma_flags_rx_completed      = (DMA1_IT_TC7);

const uint32_t          stm32ee_hid_dma_flags_tx_all            = (DMA1_IT_GL6);
const uint32_t          stm32ee_hid_dma_flags_rx_all            = (DMA1_IT_GL7);

// INTERNAL
const uint32_t          stm32ee_hid_dma_dir_MEMORY2PERIPHERAL   = DMA_DIR_PeripheralDST;
const uint32_t          stm32ee_hid_dma_dir_PERIPHERAL2MEMORY   = DMA_DIR_PeripheralSRC;


#elif   defined(SAFE_USE_STM32F1)

const uint8_t           stm32ee_hid_i2c_portnum             = 1;
I2C_TypeDef *           stm32ee_hid_i2c_port                = I2C1;
const uint32_t          stm32ee_hid_i2c_clock               = RCC_APB1Periph_I2C1;  

const I2C_InitTypeDef   stm32ee_hid_i2c_cfg                 =
{
    .I2C_Mode                   = I2C_Mode_I2C,
    .I2C_DutyCycle              = I2C_DutyCycle_2,
    .I2C_OwnAddress1            = 0,
    .I2C_Ack                    = I2C_Ack_Enable,
    .I2C_AcknowledgedAddress    = I2C_AcknowledgedAddress_7bit,
    .I2C_ClockSpeed             = 400000
};

const uint32_t          stm32ee_hid_i2c_gpio_scl_clock      = RCC_APB2Periph_GPIOB;       
GPIO_TypeDef *          stm32ee_hid_i2c_gpio_scl_port       = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_scl_pinnum     = 8;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_scl_pin        =
{
    .GPIO_Pin       = GPIO_Pin_8,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_Mode      = GPIO_Mode_AF_OD
};
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_scl_floatingpin =
{
    .GPIO_Pin       = GPIO_Pin_8,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_Mode      = GPIO_Mode_IN_FLOATING,
};

const uint32_t          stm32ee_hid_i2c_gpio_sda_clock      = RCC_APB2Periph_GPIOB;       
GPIO_TypeDef *          stm32ee_hid_i2c_gpio_sda_port       = GPIOB;
const uint16_t          stm32ee_hid_i2c_gpio_sda_pinnum     = 9;
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_sda_pin        =
{
    .GPIO_Pin       = GPIO_Pin_9,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_Mode      = GPIO_Mode_AF_OD,
};
const GPIO_InitTypeDef  stm32ee_hid_i2c_gpio_sda_floatingpin =
{
    .GPIO_Pin       = GPIO_Pin_9,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_Mode      = GPIO_Mode_IN_FLOATING,
};

const uint32_t          stm32ee_hid_i2c_gpio_remap_clock    = RCC_APB2Periph_AFIO;  
const uint32_t          stm32ee_hid_i2c_gpio_remap          = GPIO_Remap_I2C1;

#if 1
//    #define sEE_I2C                          stm32ee_hid_i2c_port
//    #define sEE_I2C_CLK                      stm32ee_hid_i2c_clock
//    #define sEE_I2C_SCL_PIN                  stm32ee_hid_i2c_gpio_scl_pin.GPIO_Pin                  /* PB.08 */
//    #define sEE_I2C_SCL_GPIO_PORT            stm32ee_hid_i2c_gpio_scl_port                       /* GPIOB */
//    #define sEE_I2C_SCL_GPIO_CLK             stm32ee_hid_i2c_gpio_scl_clock
///   #define sEE_I2C_SDA_PIN                  stm32ee_hid_i2c_gpio_sda_pin.GPIO_Pin                  /* PB.09 */
//    #define sEE_I2C_SDA_GPIO_PORT            stm32ee_hid_i2c_gpio_sda_port                       /* GPIOB */
//    #define sEE_I2C_SDA_GPIO_CLK             stm32ee_hid_i2c_gpio_sda_clock
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

//#define I2C_SPEED                           400000
//#define I2C_SLAVE_ADDRESS7                  0x00
//0xA0

#warning --> ma come mai serve I2C_SLAVE_ADDRESS7 ??? non serve. metti zero.

// -- dma

#if defined(USE_STM32F4)



#if 0

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

#endif

#elif   defined(SAFE_USE_STM32F1)

NVIC_InitTypeDef        stm32ee_hid_dma_nvic_tx_cfg     =
{
    .NVIC_IRQChannel                    = DMA1_Channel6_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

NVIC_InitTypeDef        stm32ee_hid_dma_nvic_rx_cfg     =
{
    .NVIC_IRQChannel                    = DMA1_Channel7_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

const uint32_t          stm32ee_hid_dma_clock       = RCC_AHBPeriph_DMA1;  


#define sEE_I2C_DMA                             DMA1 
// channel is missing
#define stm32ee_I2C_DMA_STREAMorCHANNEL_TX      DMA1_Channel6
#define stm32ee_I2C_DMA_STREAMorCHANNEL_RX      DMA1_Channel7 
#define sEE_I2C_DMA_CLK                         RCC_AHBPeriph_DMA1
#define sEE_I2C_DR_Address                      ((uint32_t)0x40005410)

#define sEE_I2C_DMA_TX_IRQn              DMA1_Channel6_IRQn
#define sEE_I2C_DMA_RX_IRQn              DMA1_Channel7_IRQn
#define stm32ee_I2C_DMA_TX_IRQHandler    DMA1_Channel6_IRQHandler
#define stm32ee_I2C_DMA_RX_IRQHandler    DMA1_Channel7_IRQHandler
#define sEE_I2C_DMA_PREPRIO              0
#define sEE_I2C_DMA_SUBPRIO              0   

// flags are different
#define sEE_I2C_DMA_FLAG_TX_TC           DMA1_IT_TC6   
#define sEE_I2C_DMA_FLAG_TX_GL           DMA1_IT_GL6 
#define sEE_I2C_DMA_FLAG_RX_TC           DMA1_IT_TC7 
#define sEE_I2C_DMA_FLAG_RX_GL           DMA1_IT_GL7    



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




