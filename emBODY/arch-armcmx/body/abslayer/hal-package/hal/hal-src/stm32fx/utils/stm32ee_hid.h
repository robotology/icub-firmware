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

// -- these are the irq handler of the dma tx and rx transfers. they have to be changed in here and also in _cfg.c

#if   defined(USE_STM32F4)

#define stm32ee_hid_dma_I2C_DMA_TX_IRQHandler    DMA1_Stream6_IRQHandler
#define stm32ee_hid_dma_I2C_DMA_RX_IRQHandler    DMA1_Stream0_IRQHandler  

#elif   defined(USE_STM32F1)

#define stm32ee_hid_dma_I2C_DMA_TX_IRQHandler    DMA1_Channel6_IRQHandler
#define stm32ee_hid_dma_I2C_DMA_RX_IRQHandler    DMA1_Channel7_IRQHandler   

#endif

// - definition of hidden structs -------------------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section


// -- delays 

extern const uint32_t           stm32ee_hid_timeout_flag;
extern const uint32_t           stm32ee_hid_timeout_long;
extern const uint32_t           stm32ee_hid_ackaddress_maxtrials;

// -- the three bit address which depends on how the A2 A1 A0 pins of eeprom are connected to the board

//extern const uint8_t            stm32ee_hid_hwaddressa2a1a0;


// -- i2c ports

extern const I2C_TypeDef *      stm32ee_hid_i2cx_port[3];
extern const uint32_t           stm32ee_hid_i2cx_clock[3];  
extern const uint32_t           stm32ee_hid_i2cx_gpio_remap_clock[3];  
extern const uint32_t           stm32ee_hid_i2cx_gpio_remap[3];


// -- i2c pins: not used anymore ...
//
// extern const uint32_t           stm32ee_hid_i2c_gpio_scl_clock;       
// extern const GPIO_TypeDef *     stm32ee_hid_i2c_gpio_scl_port;
// extern const uint16_t           stm32ee_hid_i2c_gpio_scl_pinnum;
// extern const GPIO_InitTypeDef   stm32ee_hid_i2c_gpio_scl_pin;

// extern const GPIO_InitTypeDef   stm32ee_hid_i2c_gpio_scl_floatingpin;

// extern const uint32_t           stm32ee_hid_i2c_gpio_sda_clock;       
// extern const GPIO_TypeDef *     stm32ee_hid_i2c_gpio_sda_port;
// extern const uint16_t           stm32ee_hid_i2c_gpio_sda_pinnum;
// extern const GPIO_InitTypeDef   stm32ee_hid_i2c_gpio_sda_pin;
// extern const GPIO_InitTypeDef   stm32ee_hid_i2c_gpio_sda_floatingpin;


// -- dma

extern const NVIC_InitTypeDef   stm32ee_hid_dma_nvic_tx_enable;

extern const NVIC_InitTypeDef   stm32ee_hid_dma_nvic_rx_enable;

extern const NVIC_InitTypeDef   stm32ee_hid_dma_nvic_tx_disable;

extern const NVIC_InitTypeDef   stm32ee_hid_dma_nvic_rx_disable;

extern const uint32_t           stm32ee_hid_dma_clock; 

extern DMA_TypeDef *            stm32ee_hid_dma;

#if     defined(USE_STM32F1)
extern DMA_Channel_TypeDef *    stm32ee_hid_dma_stream_tx;
extern DMA_Channel_TypeDef *    stm32ee_hid_dma_stream_rx;
#elif   defined(USE_STM32F4)
extern DMA_Stream_TypeDef *     stm32ee_hid_dma_stream_tx;
extern DMA_Stream_TypeDef *     stm32ee_hid_dma_stream_rx;
#endif

extern const DMA_InitTypeDef    stm32ee_hid_dma_cfg_init;

extern const uint32_t           stm32ee_hid_dma_flags_tx;
extern const uint32_t           stm32ee_hid_dma_flags_rx;

extern const uint32_t           stm32ee_hid_dma_flags_tx_completed;
extern const uint32_t           stm32ee_hid_dma_flags_rx_completed;

extern const uint32_t           stm32ee_hid_dma_flags_tx_all;
extern const uint32_t           stm32ee_hid_dma_flags_rx_all;



// - declaration of extern hidden functions ---------------------------------------------------------------------------

//extern uint32_t stm32ee_hid_myfunction(void);

// - definition of extern hidden inline functions ---------------------------------------------------------------------
// empty-section

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




