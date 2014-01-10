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

#ifndef _HL_CHIP_ST_L3G4200D_H_
#define _HL_CHIP_ST_L3G4200D_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_chip_st_l3g4200d.h
    @brief      This header file implements interface to a ST L3G4200D gyroscope
    @author     marco.accame@iit.it
    @date       10/25/2013
**/

/** @defgroup arm_hl_chip_st_l3g4200d HAL SNSR_GYRO

 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_i2c.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hl_chip_st_l3g4200d_range_t 
    @brief      hl_chip_st_l3g4200d_cfg_t contains possible ranges for the sensor.
 **/
typedef enum
{
    hl_chip_st_l3g4200d_range_250dps     = 0,
    hl_chip_st_l3g4200d_range_500dps     = 1,
    hl_chip_st_l3g4200d_range_2000dps    = 2   
} hl_chip_st_l3g4200d_range_t;


/** @typedef    typedef struct hl_chip_st_l3g4200d_cfg_t 
    @brief      hl_chip_st_l3g4200d_cfg_t contains the configuration for the chip.
 **/
typedef struct
{
    hl_i2c_t                           i2cid;
    hl_chip_st_l3g4200d_range_t        range;
} hl_chip_st_l3g4200d_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_chip_st_l3g4200d_cfg_t hl_chip_st_l3g4200d_cfg_default; // = { .i2cport = hl_i2c_port1, .range = hl_chip_st_l3g4200d_range_250dps };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hl_result_t hl_chip_st_l3g4200d_init(const hl_chip_st_l3g4200d_cfg_t *cfg)
    @brief  	this function initializes a l3g4200d gyro chip 
    @param  	cfg 	        pointer to configuration data
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_st_l3g4200d_init(const hl_chip_st_l3g4200d_cfg_t *cfg);



/** @fn			extern hl_result_t hl_chip_st_l3g4200d_temp_get(int8_t* temp)
    @brief  	this function reads temperature.
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_st_l3g4200d_temp_get(int8_t* temp);


/** @fn			extern hl_result_t hl_chip_st_l3g4200d_angrate_get(int32_t* xar, int32_t* yar, int32_t* zar)
    @brief  	this function reads angular rates on x, y, and z axis in milli-deg/sec
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_st_l3g4200d_angrate_get(int32_t* xar, int32_t* yar, int32_t* zar);


/** @}            
    end of group arm_hl_chip_st_l3g4200d  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



