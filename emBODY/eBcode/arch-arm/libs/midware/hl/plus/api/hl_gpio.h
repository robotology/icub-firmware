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

#ifndef _HL_GPIO_H_
#define _HL_GPIO_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_gpio.h
    @brief      This header file implements gpio utilities for hl library
    @author     marco.accame@iit.it
    @date       10/21/2013
**/

/** @defgroup doxy_group_hl_gpio HL GPIO utilities

    The HL GPIO ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hl_gpio_port_t 
    @brief      hl_gpio_port_t contains all possible ports managed by a wide range of MPUs.
 **/   
typedef enum 
{
    hl_gpio_portA       = 0,
    hl_gpio_portB,
    hl_gpio_portC,
    hl_gpio_portD,
    hl_gpio_portE,
    hl_gpio_portF,
    hl_gpio_portG,
    hl_gpio_portH,
    hl_gpio_portI,
    hl_gpio_portJ,
    hl_gpio_portK,
    hl_gpio_portL,
    hl_gpio_portNONE    = hl_NA08       /**< use when no port can be specified. it does not count in hl_gpio_ports_number */
} hl_gpio_port_t;

enum { hl_gpio_ports_number = 12 };

    
/** @typedef    typedef enum hl_gpio_pin_t 
    @brief      hl_gpio_pin_t contains all possible pins that a GPIO can have.
 **/    
typedef enum 
{
    hl_gpio_pin0        = 0x0001 << 0,
    hl_gpio_pin1        = 0x0001 << 1,
    hl_gpio_pin2        = 0x0001 << 2,
    hl_gpio_pin3        = 0x0001 << 3,
    hl_gpio_pin4        = 0x0001 << 4,
    hl_gpio_pin5        = 0x0001 << 5,
    hl_gpio_pin6        = 0x0001 << 6,
    hl_gpio_pin7        = 0x0001 << 7,
    hl_gpio_pin8        = 0x0001 << 8,
    hl_gpio_pin9        = 0x0001 << 9,
    hl_gpio_pin10       = 0x0001 << 10,  
    hl_gpio_pin11       = 0x0001 << 11,
    hl_gpio_pin12       = 0x0001 << 12,
    hl_gpio_pin13       = 0x0001 << 13,
    hl_gpio_pin14       = 0x0001 << 14,
    hl_gpio_pin15       = 0x0001 << 15,
    hl_gpio_pinNONE     = 0x0000,       /**< use when no pin can be specified. it does not count in hl_gpio_pins_number */   
    hl_gpio_pinALL      = 0xffff        /**< use when every pin is to be specified. it does not count in hl_gpio_pins_number */
} hl_gpio_pin_t;   

enum { hl_gpio_pins_number = 16 };

typedef struct
{
    hl_gpio_port_t  port;
    hl_gpio_pin_t   pin;
} hl_gpio_t;

 
/** @typedef    typedef enum hl_gpio_val_t 
    @brief      hl_gpio_val_t contains the values that a  pin can have.
 **/
typedef enum  
{ 
    hl_gpio_valRESET    = 0,    /**< The reset value  */
    hl_gpio_valSET      = 1,    /**< The set value    */
    hl_gpio_valUNDEF    = 3     /**< Not defined value. It must be used very carefully and NEVER as function argument   */
} hl_gpio_val_t; 
 

/** @typedef    typedef struct hl_gpio_initmode_f1_t 
    @brief      contains the gpio initialisation mode for stm32f1 library.
 **/
typedef struct
{
    uint16_t        gpio_pins;      /**< the mask of pins. use one hl_gpio_pin_t value or an OR combination of them */
    uint8_t         gpio_speed;     /**< the speed. use GPIOSpeed_TypeDef values (GPIO_Speed_10MHz = 1, GPIO_Speed_2MHz = 2, GPIO_Speed_50MHz = 3) */
    uint8_t         gpio_mode;      /**< the mode. use GPIOMode_TypeDef values (GPIO_Mode_AIN = 0x0, GPIO_Mode_IN_FLOATING = 0x04, GPIO_Mode_IPD = 0x28, etc.) */
} hl_gpio_initmode_f1_t;


/** @typedef    typedef struct hl_gpio_altfmode_f1_t 
    @brief      contains the alternate function initialisation mode for stm32f1 library.
 **/
typedef struct
{
    uint32_t        gpio_remap;     /**< the remap mode. use hl_NA32 for no remap, else use a GPIO_Remap*, GPIO_PartialRemap*, GPIO_FullRemap* macro (e.g., GPIO_Remap1_CAN1 = 0x001D4000, etc.) */
} hl_gpio_altfmode_f1_t;


/** @typedef    typedef struct hl_gpio_initmode_fx_t 
    @brief      contains the gpio initialisation mode for stm32f2/f4 library.
 **/
typedef struct
{
    uint32_t        gpio_pins;      /**< the mask of pins. use one hl_gpio_pin_t value or an OR combination of them */  
    uint8_t         gpio_mode;      /**< the mode. use GPIOMode_TypeDef values (GPIO_Mode_IN = 0x00, GPIO_Mode_OUT = 0x01, GPIO_Mode_AF = 0x02, GPIO_Mode_AN = 0x03) */
    uint8_t         gpio_speed;     /**< the speed. use GPIOSpeed_TypeDef values (GPIO_Speed_2MHz = 0x00, GPIO_Speed_25MHz = 0x01, GPIO_Speed_50MHz = 0x03, GPIO_Speed_100MHz = 0x04) */
    uint8_t         gpio_otype;     /**< the output type. use GPIOOType_TypeDef (GPIO_OType_PP = 0x00, GPIO_OType_OD = 0x01) */
    uint8_t         gpio_pupd;      /**< the pull-up/pull-down mode. use GPIOPuPd_TypeDef (GPIO_PuPd_NOPULL = 0x00, GPIO_PuPd_UP = 0x01, GPIO_PuPd_DOWN = 0x02) */
} hl_gpio_initmode_fx_t;


/** @typedef    typedef struct hl_gpio_altfmode_fx_t 
    @brief      contains the alternate function initialisation mode for stm32f2/f4 library.
 **/
typedef struct
{
    hl_gpio_t       gpio;           /**< the pair port-pin to remap in alternate mode */
    uint8_t         gpio_af;        /**< the af mode. use hl_NA08 for no remap, else use a GPIO_AF_* macro with values from 0 to 15 (e.g., GPIO_AF_CAN1 = 0x09, GPIO_AF_ETH = 0x0B, etc.) */       
} hl_gpio_altfmode_fx_t;


/** @typedef    typedef union hl_gpio_initmode_t 
    @brief      contains the gpio initialisation mode. It is a union so that it can be used with both stm32f1 and stm32f2/4
                standard peripheral libraries.
 **/
typedef union
{
    hl_gpio_initmode_f1_t   f1;     /**< to be used with stm32f1 peripheral library */
    hl_gpio_initmode_fx_t   fx;     /**< to be used with stm32f2/4 peripheral library */
} hl_gpio_initmode_t;


/** @typedef    typedef struct hl_gpio_init_t 
    @brief      contains the gpio initialisation parameters.
 **/
typedef struct
{
    hl_gpio_port_t      port;       /**< the port */
    hl_gpio_initmode_t  mode;       /**< the mode */
} hl_gpio_init_t;


/** @typedef    typedef union hl_gpio_altf_t 
    @brief      contains the alternate function mode. It is an union so that it can be used with both stm32f1 and stm32f2/4
                standard peripheral libraries.
 **/
typedef union
{
    hl_gpio_altfmode_f1_t  f1;      /**< to be used with stm32f1 peripheral library */
    hl_gpio_altfmode_fx_t  fx;      /**< to be used with stm32f2/4 peripheral library */
} hl_gpio_altf_t;
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern hl_result_t hl_gpio_init(hl_gpio_init_t* init)
    @brief      It initialises the gpio. 
    @param      init            The gpio init configuration. 
    @return     If successful hl_res_OK
 **/
extern hl_result_t hl_gpio_init(hl_gpio_init_t* init);


/** @fn         extern hl_result_t hl_gpio_altf(hl_gpio_altf_t* altf)
    @brief      It initialises the alternate function. 
    @param      altf            The gpio altf configuration. 
    @return     If successful hl_res_OK
 **/
extern hl_result_t hl_gpio_altf(hl_gpio_altf_t* altf);


/** @fn         extern hl_result_t hl_gpio_pin_write(hl_gpio_t gpio, hl_gpio_val_t val)
    @brief      Sets the value of the given gpio
    @param      gpio            The gpio. 
    @param      value           The target value.
    @return     If successful hl_res_OK
 **/
extern hl_result_t hl_gpio_pin_write(hl_gpio_t gpio, hl_gpio_val_t val);


/** @fn         extern hl_result_t hl_gpio_pin_toggle(hl_gpio_t gpio)
    @brief      Toggles the value of the given gpio
    @param      gpio            The gpio. 
    @return     If successful hl_res_OK
 **/
//extern hl_result_t hl_gpio_pin_toggle(hl_gpio_t gpio);


/** @fn         extern hl_gpio_val_t hl_gpio_pin_input_read(hl_gpio_t gpio)
    @brief      Gets the value of the given gpio if configured as input
    @param      gpio            The gpio.  
    @return     The value
 **/
extern hl_gpio_val_t hl_gpio_pin_input_read(hl_gpio_t gpio);


/** @fn         extern hl_gpio_val_t hl_gpio_pin_output_read(hl_gpio_t gpio)
    @brief      Gets the value of the given gpio if configured as output
    @param      gpio            The gpio.  
    @return     The value
 **/
extern hl_gpio_val_t hl_gpio_pin_output_read(hl_gpio_t gpio);



extern hl_result_t hl_gpio_fill_init(hl_gpio_init_t* init, hl_gpio_t* gpio);

extern hl_result_t hl_gpio_fill_altf(hl_gpio_init_t* init, hl_gpio_t* gpio);



/** @}            
    end of group doxy_group_hl_gpio  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



