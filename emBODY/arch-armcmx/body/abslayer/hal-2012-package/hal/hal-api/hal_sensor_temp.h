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

#ifndef _HAL_SENSOR_TEMP_H_
#define _HAL_SENSOR_TEMP_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_sensor_temp.h
    @brief      This header file implements interface to a generic hal sensor temperature module
    @author     marco.accame@iit.it
    @date       10/24/2012
**/

/** @defgroup arm_hal_sensor_temp HAL SNSR_TEMP

    The HAL SNSR_TEMP is a high level module whcih manages a temperature sensor
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_sensor_temp_t 
    @brief      hal_sensor_temp_t contains the possible SNSR_TEMP ports 
 **/
typedef enum
{
    hal_sensor_temp1 = 0
} hal_sensor_temp_t;

enum { hal_sensor_temps_number = 1 };


/** @typedef    typedef int8_t hal_sensor_temp_degree_t 
    @brief      expresses temperature in celsius degrees with range [-128, +127] and 1 degree resulution.
 **/
typedef int16_t hal_sensor_temp_degree_t;


/** @typedef    typedef enum hal_sensor_temp_cfg_t 
    @brief      hal_sensor_temp_cfg_t contains the configuration for the temperature sensor.
 **/
typedef struct
{
    uint8_t         dummy;
} hal_sensor_temp_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_sensor_temp_cfg_t hal_sensor_temp_cfg_default; // = { .dummy = 0 };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_sensor_temp_init(hal_sensor_temp_t sensor, const hal_sensor_temp_cfg_t *cfg)
    @brief  	this function initializes a temperature sensor on a given sensor
    @param  	sensor	        the sensor
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_sensor_temp_init(hal_sensor_temp_t sensor, const hal_sensor_temp_cfg_t *cfg);


/** @fn			extern hal_result_t hal_sensor_temp_read(hal_sensor_temp_t sensor, hal_sensor_temp_degree_t* degrees)
    @brief  	this function reads temperatures on a given sensor.
    @param  	sensor	        the sensor
    @param  	degrees 	    the read degrees
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_sensor_temp_read(hal_sensor_temp_t sensor, hal_sensor_temp_degree_t* degrees);


/** @}            
    end of group arm_hal_sensor_temp  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



