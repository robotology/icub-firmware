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

#ifndef _HAL_SENSOR_ACCEL_H_
#define _HAL_SENSOR_ACCEL_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_sensor_accel.h
    @brief      This header file implements interface to a generic hal acceleration sensor 
    @author     marco.accame@iit.it
    @date       10/26/2012
**/

/** @defgroup arm_hal_sensor_accel HAL SNSR_ACCEL

    The HAL SNSR_TEMP is a high level module whcih manages ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_sensor_accel_t 
    @brief      hal_sensor_accel_t contains the possible sensors
 **/
typedef enum
{
    hal_sensor_accel1 = 0
} hal_sensor_accel_t;

enum { hal_sensor_accel_number = 1 };


/** @typedef    typedef int8_t hal_sensor_accel_acceleration_t 
    @brief      expresses the acceleration in milli-G.
 **/
typedef struct
{
    int32_t     xac;
    int32_t     yac;
    int32_t     zac;
} hal_sensor_accel_acceleration_t;


/** @typedef    typedef enum hal_sensor_accel_cfg_t 
    @brief      hal_sensor_accel_cfg_t contains the configuration for the sensor.
 **/
typedef struct
{
    uint8_t         dummy;
} hal_sensor_accel_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_sensor_accel_cfg_t hal_sensor_accel_cfg_default; // = { .dummy = 0 };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_sensor_accel_init(hal_sensor_accel_t sensor, const hal_sensor_accel_cfg_t *cfg)
    @brief  	this function initializes a given accel sensor 
    @param  	sensor	        the sensor
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_sensor_accel_init(hal_sensor_accel_t sensor, const hal_sensor_accel_cfg_t *cfg);


/** @fn			extern hal_result_t hal_sensor_accel_read(hal_sensor_accel_t sensor, hal_sensor_accel_acceleration_t* acceler)
    @brief  	this function reads temperatures on a given sensor.
    @param  	sensor	        the sensor
    @param  	acceler 	    the read accelration
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_sensor_accel_read(hal_sensor_accel_t sensor, hal_sensor_accel_acceleration_t* acceler);


/** @}            
    end of group arm_hal_sensor_accel  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



