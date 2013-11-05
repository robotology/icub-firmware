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

#ifndef _HL_CHIP_ST_LIS3DH_H_
#define _HL_CHIP_ST_LIS3DH_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_chip_st_lis3x.h
    @brief      This header file implements interface to a ST LIS3DH or LIS331DLH accelerometer
    @author     marco.accame@iit.it
    @date       10/29/2013
**/

/** @defgroup arm_hl_chip_st_lis3x HL DEV_ACC

    The HL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_i2c.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hl_chip_st_lis3x_chip_t 
    @brief      contains every supported LIS3X chip
 **/ 
typedef enum  
{ 
    hl_chip_st_lis3x_chip_lis3dh            = 0,    /**< it has some extra features such as temperature sensor and wider range */
    hl_chip_st_lis3x_chip_lis331dlh         = 1,    /**< it offers basic behaviour */
    hl_chip_st_lis3x_chip_autodetect        = 15,   /**< it detects the chip by reading its whoami register */
    hl_chip_st_lis3x_chip_none              = 255   /**< just no device ... yet. */
} hl_chip_st_lis3x_chip_t;


/** @typedef    typedef enum hl_chip_st_lis3x_range_t 
    @brief      hl_chip_st_lis3x_range_t contains the possible ranges for the sensor.
 **/
typedef enum
{
    hl_chip_st_lis3x_range_2g       = 0,
    hl_chip_st_lis3x_range_4g       = 1,
    hl_chip_st_lis3x_range_8g       = 2 
} hl_chip_st_lis3x_range_t;



/** @typedef    typedef struct hl_chip_st_lis3x_cfg_t 
    @brief      hl_chip_st_lis3x_cfg_t contains the configuration for the accelerometer.
 **/
typedef struct
{
    hl_chip_st_lis3x_chip_t         chip;
    hl_i2c_t                        i2cid;
    hl_chip_st_lis3x_range_t        range;
} hl_chip_st_lis3x_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_chip_st_lis3x_cfg_t hl_chip_st_lis3x_cfg_default; // = { .chip = hl_chip_st_lis3x_chip_autodetect, .i2cid = hl_i2c1, .range = hl_chip_st_lis3x_range_2g };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hl_result_t hl_chip_st_lis3x_init(const hl_chip_st_lis3x_cfg_t *cfg)
    @brief  	this function initializes a lis3x accelerometer. It auto-detects if it is a LIS331DLH or a LIS3DH
    @param  	cfg 	        pointer to configuration data
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_st_lis3x_init(const hl_chip_st_lis3x_cfg_t *cfg);



/** @fn			extern hl_result_t hl_chip_st_lis3x_temp_get(int16_t* temp)
    @brief  	this function reads temperature. It is possible only for the LIS3DH chip.
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_st_lis3x_temp_get(int16_t* temp);


/** @fn			extern hl_result_t hl_chip_st_lis3x_angrate_get(int16_t* xac, int16_t* yac, int16_t* zac)
    @brief  	this function reads acceleration on x, y, and z axis. the measures are in mG.
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_st_lis3x_accel_get(int32_t* xac, int32_t* yac, int32_t* zac);


/** @}            
    end of group arm_hl_chip_st_lis3x  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



