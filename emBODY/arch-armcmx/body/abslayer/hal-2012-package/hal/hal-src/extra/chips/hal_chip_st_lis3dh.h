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

#ifndef _HAL_CHIP_ST_LIS3DH_H_
#define _HAL_CHIP_ST_LIS3DH_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_chip_st_lis3dh.h
    @brief      This header file implements interface to a generic hal sensor angular rate module (gyroscope)
    @author     marco.accame@iit.it
    @date       10/24/2012
**/

/** @defgroup arm_hal_chip_st_lis3dh HAL DEV_ACC

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
    hal_chip_st_lis3dh_range_2g       = 0,
    hal_chip_st_lis3dh_range_4g       = 1,
    hal_chip_st_lis3dh_range_8g       = 2,   
    hal_chip_st_lis3dh_range_16g      = 3 
} hal_chip_st_lis3dh_range_t;



/** @typedef    typedef enum hal_chip_st_lis3dh_cfg_t 
    @brief      hal_chip_st_lis3dh_cfg_t contains the configuration for the sensor.
 **/
typedef struct
{
    hal_i2c_t                       i2cid;
    hal_chip_st_lis3dh_range_t      range;
} hal_chip_st_lis3dh_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_chip_st_lis3dh_cfg_t hal_chip_st_lis3dh_cfg_default; // = { .i2cid = hal_i2c1, .range = hal_chip_st_lis3dh_range_2g };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_chip_st_lis3dh_init(const hal_chip_st_lis3dh_cfg_t *cfg)
    @brief  	this function initializes a lis3dh gyro sensor 
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_chip_st_lis3dh_init(const hal_chip_st_lis3dh_cfg_t *cfg);



/** @fn			extern hal_result_t hal_chip_st_lis3dh_temp_get(int16_t* temp)
    @brief  	this function reads temperature.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_chip_st_lis3dh_temp_get(int16_t* temp);


/** @fn			extern hal_result_t hal_chip_st_lis3dh_angrate_get(int16_t* xac, int16_t* yac, int16_t* zac)
    @brief  	this function reads acceleration on x, y, and z axis.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_chip_st_lis3dh_accel_get(int32_t* xac, int32_t* yac, int32_t* zac);


/** @}            
    end of group arm_hal_chip_st_lis3dh  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



