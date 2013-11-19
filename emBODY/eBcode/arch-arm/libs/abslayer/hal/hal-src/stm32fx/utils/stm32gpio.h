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

#ifndef _STM32GPIO_H_
#define _STM32GPIO_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       stm32gpio.h
    @brief      This header file contains public interface for the init/deinit of i2c using stm32 apis.
    @author     marco.accame@iit.it
    @date       09/24/2012
**/

/** @defgroup stm32gpio STM32 GPIO

    The STM32 I2C ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


// keep same values as hal_gpio_port_t
typedef enum 
{
    stm32gpio_portA = 0,
    stm32gpio_portB,
    stm32gpio_portC,
    stm32gpio_portD,
    stm32gpio_portE,
    stm32gpio_portF,
    stm32gpio_portG,
    stm32gpio_portH,
    stm32gpio_portI,
    stm32gpio_portNONE = 255
} stm32gpio_port_t;

// keep same values as hal_gpio_pin_t
typedef enum 
{
    stm32gpio_pin0 = 0,
    stm32gpio_pin1,
    stm32gpio_pin2,
    stm32gpio_pin3,
    stm32gpio_pin4,
    stm32gpio_pin5,
    stm32gpio_pin6,
    stm32gpio_pin7,
    stm32gpio_pin8,
    stm32gpio_pin9,
    stm32gpio_pin10,  
    stm32gpio_pin11,
    stm32gpio_pin12,
    stm32gpio_pin13,
    stm32gpio_pin14,
    stm32gpio_pin15,
    stm32gpio_pinNONE = 255    
} stm32gpio_pin_t; 

// keep same values as hal_gpio_val_t
typedef enum  
{ 
    stm32gpio_valLOW    = 0,          
    stm32gpio_valHIGH   = 1,          
    stm32gpio_valNONE   = 3          
} stm32gpio_val_t;

typedef struct
{
    stm32gpio_port_t    port;
    stm32gpio_pin_t     pin;
} stm32gpio_gpio_t;     // 2B  
  

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group stm32gpio  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



