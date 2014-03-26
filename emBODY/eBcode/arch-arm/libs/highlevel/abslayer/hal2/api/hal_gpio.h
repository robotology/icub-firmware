/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_gpio.h
    @brief      This header file implements public interface to the hal_gpio.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup doxy_group_hal_gpio HAL GPIO

    The HAL GPIO ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_gpio_port_t 
    @brief      hal_gpio_pin_t contains all possible ports that a silicon GPIO can have.
 **/   
typedef enum 
{
    hal_gpio_portA = 0,
    hal_gpio_portB,
    hal_gpio_portC,
    hal_gpio_portD,
    hal_gpio_portE,
    hal_gpio_portF,
    hal_gpio_portG,
    hal_gpio_portH,
    hal_gpio_portI,
    hal_gpio_portJ,
    hal_gpio_portK,
    hal_gpio_portL,
    hal_gpio_portNONE = hal_NA08    /**< use when no port can be specified. it does not count in hal_gpio_ports_number */
} hal_gpio_port_t;

enum { hal_gpio_ports_number = 12 };

    
/** @typedef    typedef enum hal_gpio_pin_t 
    @brief      hal_gpio_pin_t contains all possible pins that a silicon GPIO can have. It it correct make the id start
                from 0 rather than 1, as usual in HAL, because silicon vendors make it start from 0.
 **/    
typedef enum 
{
    hal_gpio_pin0       = 0x0001 << 0,
    hal_gpio_pin1       = 0x0001 << 1,
    hal_gpio_pin2       = 0x0001 << 2,
    hal_gpio_pin3       = 0x0001 << 3,
    hal_gpio_pin4       = 0x0001 << 4,
    hal_gpio_pin5       = 0x0001 << 5,
    hal_gpio_pin6       = 0x0001 << 6,
    hal_gpio_pin7       = 0x0001 << 7,
    hal_gpio_pin8       = 0x0001 << 8,
    hal_gpio_pin9       = 0x0001 << 9,
    hal_gpio_pin10      = 0x0001 << 10,  
    hal_gpio_pin11      = 0x0001 << 11,
    hal_gpio_pin12      = 0x0001 << 12,
    hal_gpio_pin13      = 0x0001 << 13,
    hal_gpio_pin14      = 0x0001 << 14,
    hal_gpio_pin15      = 0x0001 << 15,
    hal_gpio_pinALL     = 0xffff,
    hal_gpio_pinNONE    = 0x0000     /**< use when no pin can be specified. it does not count in hal_gpio_pinss_number */   
} hal_gpio_pin_t; 

enum { hal_gpio_pins_number = 16 };

/** @typedef    typedef struct hal_gpio_t 
    @brief      hal_gpio_t specifies a pair port-pin whcih identifies a gpio.
 **/    
typedef struct 
{
    hal_gpio_port_t     port;
    hal_gpio_pin_t      pin;    
} hal_gpio_t;   

/** @typedef    typedef enum hal_gpio_speed_t 
    @brief      hal_gpio_val_t contains speed values that an output pin can have. The values used inside HAL
                depend on the micro. Typical values for a stm32f1xx micro are 2Mhz, 10MHz and 50Mhz respectively.
 **/
typedef enum
{ 
    hal_gpio_speed_default  = 0,        /**< in case we dont care about driving clock of the gpio */
    hal_gpio_speed_low      = 1,        /**< use a driving clock with low speed */
    hal_gpio_speed_medium   = 2,        /**< use a driving clock with medium speed */
    hal_gpio_speed_high     = 3,        /**< use a driving clock with fast speed */
    hal_gpio_speed_max      = 4,        /**< use a driving clock with maximum speed */
    hal_gpio_speed_NONE     = hal_NA08  /**< use when no speed can be specified. it does not count in hal_gpio_speeds_number */
} hal_gpio_speed_t;

enum { hal_gpio_speeds_number = 5 };

/** @typedef    typedef enum hal_gpio_dir_t 
    @brief      hal_gpio_dir_t contains all possible directions that a silicon pin can have.
 **/
typedef enum  
{
    hal_gpio_dirINP  = 0,       /**< Input direction                */
    hal_gpio_dirOUT  = 1,       /**< Output direction               */
    hal_gpio_dirALT  = 2,       /**< Alternate mode configuration   */
    hal_gpio_dirNONE = 255,     /**< use when no direction can be specified. it does not count in hal_gpio_speeds_number */
} hal_gpio_dir_t;

enum { hal_gpio_dirs_number = 3 };
 
/** @typedef    typedef enum hal_gpio_val_t 
    @brief      hal_gpio_val_t contains the values that a silicon pin can have.
 **/
typedef enum  
{ 
    hal_gpio_valLOW  = 0,          /**< Logical 0 value        */
    hal_gpio_valHIGH = 1,          /**< Logical 1 value        */
    hal_gpio_valNONE = 3           /**< Not defined value. It must be used very carefully and NEVER as function argument   */
} hal_gpio_val_t; 
 

/** @typedef    typedef enum hal_gpio_cfg_t 
    @brief      hal_gpio_cfg_t contains the configuration for a gpio. it can be a normal input or output or even an alternate
                function. in this latter case, we use altcfg.
 **/
typedef struct 
{
    hal_gpio_dir_t      dir;
    hal_gpio_speed_t    speed;
    void*               altcfg;
} hal_gpio_cfg_t;



/** @typedef    typedef enum hal_gpio_map_t 
    @brief      hal_gpio_map_t contains the mapping of gpio with dir and speed and its alternate config.
                It is used in mapping a peripheral into a given gpio
 **/
typedef struct 
{
    hal_gpio_t          gpio;
    uint32_t            af32;
} hal_gpio_maP_t;

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_gpio_cfg_t hal_gpio_cfg_default; // = {.dir = hal_gpio_dirOUT, .speed = hal_gpio_speed_low};


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern hal_result_t hal_gpio_init(hal_gpio_t gpio, const hal_gpio_cfg_t* cfg)
    @brief      It initialises the gpio. If cfg->dir is hal_gpio_dirINP or hal_gpio_dirINP then the gpio is
                configured using the cfg->dir and cfg->speed only. Else if cfg->dir is hal_gpio_dirALT, then
                it is used also the field cfg->altcfg.
    @param      gpio            The gpio. 
    @param      cfg             The cfg. 
    @return     If successful hal_res_OK
 **/
extern hal_result_t hal_gpio_init(hal_gpio_t gpio, const hal_gpio_cfg_t* cfg);



/** @fn         extern hal_result_t hal_gpio_setval(hal_gpio_t gpio, hal_gpio_val_t val)
    @brief      Sets the value of the given gpio 
    @param      gpio            The gpio. 
    @param      value           The target value.
    @return     If successful hal_res_OK
 **/
extern hal_result_t hal_gpio_setval(hal_gpio_t gpio, hal_gpio_val_t val);


/** @fn         extern hal_gpio_val_t hal_gpio_getval(hal_gpio_t gpio)
    @brief      Gets the value of the given gpio 
    @param      gpio            The gpio.  
    @return     The value.
 **/
extern hal_gpio_val_t hal_gpio_getval(hal_gpio_t gpio);


// /** @fn         extern void hal_gpio_quickest_setval(hal_gpio_t gpio hal_gpio_val_t val)
//     @brief      Sets the value of the given pin in the given port in the quickest possible mode. To pursue speed,
//                 all safety checks which are available in hal_gpio_setval are here removed. Thus, pay particular
//                 attention to manipulate gpios already initted as output. And never and never use hal_gpio_portNONE
//                 or hal_gpio_pinNONE otherwise a SW crash is inevitable. needless to say don't ever use hal_gpio_valNONE. 
//                 On STM32F107 @ 72MHz, the execution time is around 1 usec vs. about 1.7 usec of hal_gpio_setval().
//                 On STM32F407 @ 168MHz, the execution time is around 0.2 usec vs. about 0.4 usec of hal_gpio_setval().
//     @param      gpio            The gpio. 
//     @param      value           The target value.
//  **/
// extern void hal_gpio_quickest_setval(hal_gpio_t gpio, hal_gpio_val_t val);



/** @}            
    end of group doxy_group_hal_gpio  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



