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

#ifndef _STM32I2C_H_
#define _STM32I2C_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       stm32i2c.h
    @brief      This header file contains public interface for the init/deinit of i2c using stm32 apis.
    @author     marco.accame@iit.it
    @date       09/24/2012
**/

/** @defgroup arm_hal_stm32i2c STM32 I2C

    The STM32 I2C ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum stm32i2c_result_t 
    @brief      stm32i2c_result_t contains function results for this module.
 **/ 
typedef enum  
{ 
    stm32i2c_res_OK          = 0,  
    stm32i2c_res_NOK         = 1
} stm32i2c_result_t;



// keep same values as hal_gpio_port_t
typedef enum 
{
    stm32i2c_gpio_portA = 0,
    stm32i2c_gpio_portB,
    stm32i2c_gpio_portC,
    stm32i2c_gpio_portD,
    stm32i2c_gpio_portE,
    stm32i2c_gpio_portF,
    stm32i2c_gpio_portG,
    stm32i2c_gpio_portH,
    stm32i2c_gpio_portI,
    stm32i2c_gpio_portNONE = 255
} stm32i2c_gpio_port_t;

typedef enum 
{
    stm32i2c_gpio_pin0 = 0,
    stm32i2c_gpio_pin1,
    stm32i2c_gpio_pin2,
    stm32i2c_gpio_pin3,
    stm32i2c_gpio_pin4,
    stm32i2c_gpio_pin5,
    stm32i2c_gpio_pin6,
    stm32i2c_gpio_pin7,
    stm32i2c_gpio_pin8,
    stm32i2c_gpio_pin9,
    stm32i2c_gpio_pin10,  
    stm32i2c_gpio_pin11,
    stm32i2c_gpio_pin12,
    stm32i2c_gpio_pin13,
    stm32i2c_gpio_pin14,
    stm32i2c_gpio_pin15,
    stm32i2c_gpio_pinNONE = 255    
} stm32i2c_gpio_pin_t; 

typedef struct
{
    stm32i2c_gpio_port_t     port;
    stm32i2c_gpio_pin_t      pin;
} stm32i2c_gpio_t;  


/** @typedef    typedef struct stm32i2c_cfg_t;
    @brief      contains configuration data of i2c.
 **/
typedef struct
{
    uint8_t                 speed;      /**< in multiples of 100K. valid values are only 1 and 4. */
    stm32i2c_gpio_t         scl;
    stm32i2c_gpio_t         sda;
} stm32i2c_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//extern const stm32i2c_cfg_t stm32i2c_cfg_default;   // = { .device = stm32i2c_device_st_m24lr64, .i2cbus = 0, .functioni2cinit = NULL, .parameteri2cinit = NULL, .functioni2cdeinit = NULL };

// port is 0, 1, 2

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn    	    xtern stm32i2c_result_t stm32i2c_init(uint8_t port, const stm32i2c_cfg_t *cfg)
    @brief      This function initialises i2c. Initialisation can be done only once.
                If one wants to initialise once more, he/she must call stm32i2c_deinit() first.
    @param      cfg             The configuration. If NULL an error is returned. 
    @return     In case of successful initialisation is stm32i2c_res_OK, in case of error stm32i2c_res_NOK.
  */
extern stm32i2c_result_t stm32i2c_init(uint8_t port, const stm32i2c_cfg_t *cfg);


/** @fn    	    extern stm32i2c_result_t stm32i2c_deinit(uint8_t i2cport, const stm32i2c_cfg_t *cfg)
    @brief      This function de-initialises i2c.
    @return     In case of successful initialisation is stm32i2c_res_OK, in case of error stm32i2c_res_NOK.
  */
extern stm32i2c_result_t stm32i2c_deinit(uint8_t port, const stm32i2c_cfg_t *cfg);



/** @}            
    end of group arm_hal_stm32i2c  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



