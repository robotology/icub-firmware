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

#include "stm32gpio.h"



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


/** @typedef    typedef void (*stm32i2c_void_fp_void_t) (void)
    @brief      pointer to a function which returns void and has a void argument.
 **/
typedef void (*stm32i2c_void_fp_void_t) (void);



/** @typedef    typedef struct stm32i2c_cfg_t;
    @brief      contains configuration data of i2c.
 **/
typedef struct
{
    uint32_t                speed;      /**< valid values are only 100k, 200k, and 400k. */
    stm32gpio_gpio_t        scl;
    stm32gpio_gpio_t        sda;
    uint8_t                 usedma;     /**< so far only i2c1 can have dma */
    stm32i2c_void_fp_void_t ontimeout;
} stm32i2c_cfg_t;           

typedef struct 
{
    uint8_t     numofbytes;         // only 1, 2, 3
    union
    {
        uint8_t     three[3];
        uint16_t    two;
        uint8_t     one;
    } bytes;
} stm32i2c_regaddr_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//extern const stm32i2c_cfg_t stm32i2c_cfg_default;   // = { .device = stm32i2c_device_st_m24lr64, .i2cbus = 0, .functioni2cinit = NULL, .parameteri2cinit = NULL, .functioni2cdeinit = NULL };

// port is 1, 2, 3

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn    	    extern stm32i2c_result_t stm32i2c_init(uint8_t port, const stm32i2c_cfg_t *cfg)
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



extern stm32i2c_result_t stm32i2c_read(uint8_t port, uint8_t devaddr, stm32i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


extern stm32i2c_result_t stm32i2c_write(uint8_t port, uint8_t devaddr, stm32i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


extern stm32i2c_result_t stm32i2c_standby(uint8_t port, uint8_t devaddr);

/** @}            
    end of group arm_hal_stm32i2c  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



