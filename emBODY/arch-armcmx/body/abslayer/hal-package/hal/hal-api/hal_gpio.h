/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/** @defgroup arm_hal_gpio HAL GPIO

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
    hal_gpio_portNONE = hal_NA08
} hal_gpio_port_t;

enum { hal_gpio_ports_number = 9 };

    
/** @typedef    typedef enum hal_gpio_pin_t 
    @brief      hal_gpio_pin_t contains all possible pins that a silicon GPIO can have.
 **/    
typedef enum 
{
    hal_gpio_pin0 = 0,
    hal_gpio_pin1,
    hal_gpio_pin2,
    hal_gpio_pin3,
    hal_gpio_pin4,
    hal_gpio_pin5,
    hal_gpio_pin6,
    hal_gpio_pin7,
    hal_gpio_pin8,
    hal_gpio_pin9,
    hal_gpio_pin10,  
    hal_gpio_pin11,
    hal_gpio_pin12,
    hal_gpio_pin13,
    hal_gpio_pin14,
    hal_gpio_pin15,
    hal_gpio_pinNONE = hal_NA08    
} hal_gpio_pin_t;   

enum { hal_gpio_pins_number = 16 };


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
    hal_gpio_speed_max      = 4         /**< use a driving clock with maximum speed */
} hal_gpio_speed_t;

enum { hal_gpio_speeds_number = 5 };

/** @typedef    typedef enum hal_gpio_dir_t 
    @brief      hal_gpio_dir_t contains all possible directions that a silicon pin can have.
 **/
typedef enum  
{
    hal_gpio_dirNONE = 0,      /**< Not defined direction. It must be used very carefully */
    hal_gpio_dirINP  = 1,      /**< Input direction            */
    hal_gpio_dirOUT  = 2,      /**< Output direction           */
    hal_gpio_dirALT  = 3
} hal_gpio_dir_t;

 
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
    @brief      hal_gpio_cfg_t contains the configuration for a gpio
 **/
typedef struct 
{
    hal_gpio_port_t     port;
    hal_gpio_pin_t      pin;
    hal_gpio_dir_t      dir;
    hal_gpio_speed_t    speed;
} hal_gpio_cfg_t;

typedef struct hal_gpio_hid_altcfg_t hal_gpio_altcfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


extern void pippo(void);

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t hal_gpio_init(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_dir_t dir)
    @brief      Inits the given pin in the given port with the given direction and a given speed 
    @param      pin             The pin. 
    @param      port            The port. 
    @param      dir             The direction.
    @param      dir             The speed.
    @return     Always hal_res_OK
 **/
extern hal_result_t hal_gpio_init(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_dir_t dir, hal_gpio_speed_t speed);

extern hal_result_t hal_gpio_configure(hal_gpio_cfg_t cfg, const hal_gpio_altcfg_t* altcfg);


/** @fn         extern hal_result_t hal_gpio_setval(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_val_t val)
    @brief      Sets the value of the given pin in the given port 
    @param      pin             The pin. 
    @param      port            The port. 
    @param      value           The target value.
    @return     Always hal_res_OK
 **/
extern hal_result_t hal_gpio_setval(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_val_t val);


/** @fn         extern hal_gpio_val_t hal_gpio_getval(hal_gpio_port_t port, hal_gpio_pin_t pin)
    @brief      Gets the value of the given pin in the given port 
    @param      pin             The pin. 
    @param      port            The port. 
    @return     The value.
 **/
extern hal_gpio_val_t hal_gpio_getval(hal_gpio_port_t port, hal_gpio_pin_t pin);


/** @fn         extern void hal_gpio_quickest_setval(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_val_t val)
    @brief      Sets the value of the given pin in the given port in the quickest possible mode. To pursue speed,
                all safety checks which are available in hal_gpio_setval are here removed. Thus, pay particular
                attention to manipulate gpios already initted as output. And never and never use hal_gpio_portNONE
                or hal_gpio_pinNONE otherwise a SW crash is inevitable. needless to say don't ever use hal_gpio_valNONE. 
                On STM32F107 @ 72MHz, the execution time is around 1 usec vs. about 1.7 usec of hal_gpio_setval().
                On STM32F407 @ 168MHz, the execution time is around 0.2 usec vs. about 0.4 usec of hal_gpio_setval().
    @param      pin             The pin. 
    @param      port            The port. 
    @param      value           The target value.
 **/
extern void hal_gpio_quickest_setval(hal_gpio_port_t port, hal_gpio_pin_t pin, hal_gpio_val_t val);



/** @}            
    end of group arm_hal_gpio  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



