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

struct hl_i2c_advcfg_full_opaque_t
{
    uint8_t     nothingsofar;
};

// --
// -- i2c section: end



// -- can section: begin
// --


/** @typedef    struct hl_can_advcfg_bitsampling_opaque_t 
    @brief      contains the quantisation bit timing for stm32f1/2/4 library. The bit is divided in N quanta, N = bs1 + 1 + bs2.
                value bs1 is the number of quanta before the sampling quanta, and bs2 the number after. value sjw is the number
                of quanta that are possible to stretch to perform resynchronization.
    @warning    the can clock (slow APB bus) divided by 1000 must be multiple of value N = bs1+1+bs2.
                for can clock at 36MHz good values are: (CAN_BS1_5tq, CAN_BS2_3tq, CAN_SJW_3tq) because 5+1+3=9    
                for can clock at 42MHz good values are: (CAN_BS1_4tq, CAN_BS2_2tq, CAN_SJW_3tq) because 4+1+2=7 
 **/
struct hl_can_advcfg_bitsampling_opaque_t
{
    uint8_t     bs1;    /**< use CAN_BS1_1tq -> CAN_BS1_16tq */
    uint8_t     bs2;    /**< use CAN_BS2_1tq -> CAN_BS1_8tq */
    uint8_t     sjw;    /**< use CAN_SJW_1tq -> CAN_SJW_4tq */
};


struct hl_can_advcfg_full_opaque_t
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



