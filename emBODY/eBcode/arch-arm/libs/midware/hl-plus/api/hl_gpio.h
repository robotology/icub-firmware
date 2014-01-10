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


/** @typedef    typedef struct hl_gpio_t 
    @brief      hl_gpio_t specifies the gpio (port, pin). It is 4 bytes long on most compilers, thus it can be 
                safely used as a single function argument.
 **/
typedef struct
{
    hl_gpio_port_t  port;       /**< the port */
    hl_gpio_pin_t   pin;        /**< the pin */
} hl_gpio_t;

 
/** @typedef    typedef enum hl_gpio_val_t 
    @brief      hl_gpio_val_t contains the values that a pin can have.
 **/
typedef enum  
{ 
    hl_gpio_valRESET    = 0,    /**< The reset value  */
    hl_gpio_valSET      = 1,    /**< The set value    */
    hl_gpio_valUNDEF    = 3     /**< Not defined value. It must be used very carefully and NEVER as function argument   */
} hl_gpio_val_t; 
 


/** @typedef    typedef struct hl_gpio_map_t 
    @brief      hl_gpio_map_t is used to map a peripheral to relevant gpio and alternate function.                
 **/
typedef struct 
{
    hl_gpio_t           gpio;       /**< the gpio */
    uint32_t            af32;       /**< the alternate function. use hl_NA32 for none, else what is specified by stm32fx */
} hl_gpio_map_t;
 

/** @typedef    typedef struct hl_gpio_init_opaque_t hl_gpio_init_t
    @brief      contains the opaque declaration of the initialisation parameters of a gpio. As such, hl_gpio_init_t
                can be used only as a pointer to a struct hl_gpio_init_opaque_t which is defined in hl_arch.h.
                The reason is that the initialisation mode depends on the architecture. 
 **/
typedef struct hl_gpio_init_opaque_t hl_gpio_init_t;


/** @typedef    typedef struct hl_gpio_altf_opaque_t hl_gpio_altf_t
    @brief      contains the opaque declaration of the alternate function parameters of a gpio. As such, hl_gpio_altf_t
                can be used only as a pointer to a struct hl_gpio_altf_opaque_t which is defined in hl_arch.h.
                The reason is that the alternate function mode depends on the architecture. 
 **/
typedef struct hl_gpio_altf_opaque_t hl_gpio_altf_t;


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
extern hl_result_t hl_gpio_pin_toggle(hl_gpio_t gpio);


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



/** @fn         extern hl_result_t hl_gpio_fill_init(hl_gpio_init_t* init, const hl_gpio_map_t* gpiomap)
    @brief      fills the init data structure with info coming from gpiomap. it extracts from gpiomap
                the port and the pin and copies it in relevant part of init.
    @param      init            The gpio init pointer.  
    @param      gpiomap         The gpio map pointer. 
    @return     If successful hl_res_OK
 **/
extern hl_result_t hl_gpio_fill_init(hl_gpio_init_t* init, const hl_gpio_map_t* gpiomap);


/** @fn         extern hl_result_t hl_gpio_fill_altf(hl_gpio_altf_t* altf, const hl_gpio_map_t* gpiomap)
    @brief      fills the altf data structure with info coming from gpiomap. it extracts from gpiomap
                the af32 and copies it in relevant part of altf.
    @param      altf            The gpio altf pointer.  
    @param      gpiomap         The gpio map pointer. 
    @return     If successful hl_res_OK
 **/
extern hl_result_t hl_gpio_fill_altf(hl_gpio_altf_t* altf, const hl_gpio_map_t* gpiomap);


/** @fn         extern hl_result_t hl_gpio_pin_output_init(hl_gpio_t gpio)
    @brief      initialises a pin as output.
    @param      gpio            The gpio. 
    @return     If successful hl_res_OK
 **/
extern hl_result_t hl_gpio_pin_output_init(hl_gpio_t gpio);



/** @}            
    end of group doxy_group_hl_gpio  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



