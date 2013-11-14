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

#ifndef _HL_ARCH_H_
#define _HL_ARCH_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_arch.h
    @brief      This header file implements types specific for arch mpus for use by the hl library
    @author     marco.accame@iit.it
    @date       10/21/2013
**/

/** @defgroup doxy_group_hl_arch HL specifics utilities

    The HL specifics for arch ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"

#if     defined(HL_CFG_TARGET_BY_FILE )
    #include "hl_cfg_plus_target.h"
#endif



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

// -- gpio section: begin
// --

#include "hl_gpio.h"

/** @typedef    typedef struct hl_gpio_initmode_f1_t 
    @brief      contains the gpio initialisation mode for stm32f1 library.
 **/
typedef struct
{
    uint16_t        gpio_pins;      /**< the mask of pins. use one hl_gpio_pin_t value or an OR combination of them */
    uint8_t         gpio_speed;     /**< the speed. use GPIOSpeed_TypeDef values (GPIO_Speed_10MHz = 1, GPIO_Speed_2MHz = 2, GPIO_Speed_50MHz = 3) */
    uint8_t         gpio_mode;      /**< the mode. use GPIOMode_TypeDef values (GPIO_Mode_AIN = 0x0, GPIO_Mode_IN_FLOATING = 0x04, GPIO_Mode_IPD = 0x28, etc.) */
} hl_gpio_initmode_f1_t;



/** @typedef    typedef struct hl_gpio_init_f1_t 
    @brief      contains the gpio initialisation parameters for stm32f2/f4 library.
 **/
typedef struct
{   
    hl_gpio_port_t          port;       /**< the port */
    hl_gpio_initmode_f1_t   mode;       /**< the mode */
} hl_gpio_init_f1_t;


/** @typedef    typedef struct hl_gpio_altf_f1_t 
    @brief      contains the alternate function initialisation mode for stm32f1 library.
 **/
typedef struct
{
    uint32_t        gpio_remap;     /**< the remap mode. use hl_NA32 for no remap, else use a GPIO_Remap*, GPIO_PartialRemap*, GPIO_FullRemap* macro (e.g., GPIO_Remap1_CAN1 = 0x001D4000, etc.) */
} hl_gpio_altf_f1_t;


/** @typedef    typedef struct hl_gpio_initmode_fx_t 
    @brief      contains the gpio initialisation mode for stm32f1 library.
 **/
typedef struct
{
    uint32_t        gpio_pins;      /**< the mask of pins. use one hl_gpio_pin_t value or an OR combination of them */  
    uint8_t         gpio_mode;      /**< the mode. use GPIOMode_TypeDef values (GPIO_Mode_IN = 0x00, GPIO_Mode_OUT = 0x01, GPIO_Mode_AF = 0x02, GPIO_Mode_AN = 0x03) */
    uint8_t         gpio_speed;     /**< the speed. use GPIOSpeed_TypeDef values (GPIO_Speed_2MHz = 0x00, GPIO_Speed_25MHz = 0x01, GPIO_Speed_50MHz = 0x03, GPIO_Speed_100MHz = 0x04) */
    uint8_t         gpio_otype;     /**< the output type. use GPIOOType_TypeDef (GPIO_OType_PP = 0x00, GPIO_OType_OD = 0x01) */
    uint8_t         gpio_pupd;      /**< the pull-up/pull-down mode. use GPIOPuPd_TypeDef (GPIO_PuPd_NOPULL = 0x00, GPIO_PuPd_UP = 0x01, GPIO_PuPd_DOWN = 0x02) */
} hl_gpio_initmode_fx_t;



/** @typedef    typedef struct hl_gpio_init_fx_t 
    @brief      contains the gpio initialisation parameters for stm32f2/f4 library.
 **/
typedef struct
{   
    hl_gpio_port_t          port;       /**< the port */
    hl_gpio_initmode_fx_t   mode;       /**< the mode */
} hl_gpio_init_fx_t;


/** @typedef    typedef struct hl_gpio_altf_fx_t 
    @brief      contains the alternate function initialisation mode for stm32f2/4 library.
 **/
typedef struct
{
    hl_gpio_t       gpio;           /**< the pair port-pin to remap in alternate mode */
    uint8_t         gpio_af;        /**< the af mode. use hl_NA08 for no remap, else use a GPIO_AF_* macro with values from 0 to 15 (e.g., GPIO_AF_CAN1 = 0x09, GPIO_AF_ETH = 0x0B, etc.) */       
} hl_gpio_altf_fx_t;



#if     defined(HL_USE_MPU_ARCH_STM32F1)

// use hl_gpio_init_f1_t
struct hl_gpio_init_opaque_t
{   
    hl_gpio_port_t          port;       /**< the port */
    hl_gpio_initmode_f1_t   mode;       /**< the mode */
};


// use hl_gpio_altf_f1_t
struct hl_gpio_altf_opaque_t
{
    uint32_t        gpio_remap;     /**< the remap mode. use hl_NA32 for no remap, else use a GPIO_Remap*, GPIO_PartialRemap*, GPIO_FullRemap* macro (e.g., GPIO_Remap1_CAN1 = 0x001D4000, etc.) */
};

#elif   defined(HL_USE_MPU_ARCH_STM32F4)    

// use hl_gpio_init_fx_t
struct hl_gpio_init_opaque_t
{   
    hl_gpio_port_t          port;       /**< the port */
    hl_gpio_initmode_fx_t   mode;       /**< the mode */
};

// use hl_gpio_altf_fx_t
struct hl_gpio_altf_opaque_t
{
    hl_gpio_t       gpio;           /**< the pair port-pin to remap in alternate mode */
    uint8_t         gpio_af;        /**< the af mode. use hl_NA08 for no remap, else use a GPIO_AF_* macro with values from 0 to 15 (e.g., GPIO_AF_CAN1 = 0x09, GPIO_AF_ETH = 0x0B, etc.) */       
};

#endif

// --
// -- gpio section: end


// -- i2c section: begin
// --

#include "hl_i2c.h"

struct hl_i2c_advcfg_opaque_t
{   // the same as I2C_InitTypeDef
    uint32_t    I2C_ClockSpeed;          
    uint16_t    I2C_Mode;                
    uint16_t    I2C_DutyCycle;           
    uint16_t    I2C_OwnAddress1;         
    uint16_t    I2C_Ack;                 
    uint16_t    I2C_AcknowledgedAddress; 
};

extern const hl_i2c_advcfg_t hl_i2c_advcfg_default; // =
//{   // good for 400kbps
//    .I2C_ClockSpeed             = 400000,                       // changed by cfg->speed
//    .I2C_Mode                   = I2C_Mode_I2C,
//    .I2C_DutyCycle              = I2C_DutyCycle_2,
//    .I2C_OwnAddress1            = 0,                            // changed by cfg->ownaddress
//    .I2C_Ack                    = I2C_Ack_Enable,
//    .I2C_AcknowledgedAddress    = I2C_AcknowledgedAddress_7bit
//}; 

// --
// -- i2c section: end



// -- can section: begin
// --

#include "hl_can.h"



struct hl_can_advcfg_opaque_t
{   // the same as CAN_InitTypeDef   
    uint16_t        CAN_Prescaler;    
    uint8_t         CAN_Mode;         
    uint8_t         CAN_SJW;
    uint8_t         CAN_BS1;
    uint8_t         CAN_BS2;
    FunctionalState CAN_TTCM;
    FunctionalState CAN_ABOM;
    FunctionalState CAN_AWUM; 
    FunctionalState CAN_NART;
    FunctionalState CAN_RFLM; 
    FunctionalState CAN_TXFP; 
};

extern const hl_can_advcfg_t hl_can_advcfg_default; // =
// {   // good for canclock @ 36mhz and bitrate 1mbps 
//     .CAN_Prescaler      = 4,                    // depends on: cfg->baudrate, canclock, sjw, bs1, bs2    
//     .CAN_Mode           = CAN_Mode_Normal,         
//     .CAN_SJW            = CAN_BS1_3tq,          // depends on: canclock
//     .CAN_BS1            = CAN_BS1_5tq,          // depends on: canclock
//     .CAN_BS2            = CAN_BS1_3tq,          
//     .CAN_TTCM           = DISABLE,
//     .CAN_ABOM           = DISABLE,
//     .CAN_AWUM           = DISABLE, 
//     .CAN_NART           = DISABLE,
//     .CAN_RFLM           = DISABLE, 
//     .CAN_TXFP           = ENABLE 
// };

// --
// -- can section: end



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group doxy_group_hl_stm32  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



