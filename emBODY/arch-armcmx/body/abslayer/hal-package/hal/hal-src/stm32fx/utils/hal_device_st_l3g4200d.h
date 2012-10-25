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

#ifndef _HAL_DEVICE_ST_L3G4200D_H_
#define _HAL_DEVICE_ST_L3G4200D_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_device_st_l3g4200d.h
    @brief      This header file implements interface to a generic hal sensor angular rate module (gyroscope)
    @author     marco.accame@iit.it
    @date       10/24/2012
**/

/** @defgroup arm_hal_device_st_l3g4200d HAL SNSR_GYRO

    The HAL SNSR_TEMP is a high level module whcih manages a angular rate sensor
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 



/** @typedef    typedef int8_t hal_device_st_l3g4200d_angular_rate_t 
    @brief      expresses the angular rate in degrees per second with range [-250, +250] and 1 degree/sec resulution.
 **/
typedef struct
{
    int16_t     xar;
    int16_t     yar;
    int16_t     zar;
} hal_device_st_l3g4200d_angular_rate_t;


/** @typedef    typedef enum hal_device_st_l3g4200d_cfg_t 
    @brief      hal_device_st_l3g4200d_cfg_t contains the configuration for the sensor.
 **/
typedef struct
{
    hal_i2c_port_t      i2cport;
} hal_device_st_l3g4200d_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_device_st_l3g4200d_cfg_t hal_device_st_l3g4200d_cfg_default; // = { .i2cport = hal_i2c_port1 };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_device_st_l3g4200d_init(const hal_device_st_l3g4200d_cfg_t *cfg)
    @brief  	this function initializes a l3g4200d gyro sensor 
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_device_st_l3g4200d_init(const hal_device_st_l3g4200d_cfg_t *cfg);



/** @fn			extern hal_result_t hal_hal_device_st_l3g4200d_temp_get(int8_t* temp)
    @brief  	this function reads temperature.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_hal_device_st_l3g4200d_temp_get(int8_t* temp);


/** @fn			extern hal_result_t hal_hal_device_st_l3g4200d_angrate_get(int16_t* xar, int16_t* yar, int16_t* zar)
    @brief  	this function reads angular rates on x, y, and z axis.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_hal_device_st_l3g4200d_angrate_get(int16_t* xar, int16_t* yar, int16_t* zar);


/** @}            
    end of group arm_hal_device_st_l3g4200d  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



