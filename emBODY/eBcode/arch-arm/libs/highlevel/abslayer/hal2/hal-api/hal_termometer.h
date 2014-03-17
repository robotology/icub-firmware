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

#ifndef _HAL_TERMOMETER_H_
#define _HAL_TERMOMETER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_termometer.h
    @brief      This header file implements interface to a generic hal termometer
    @author     marco.accame@iit.it
    @date       10/24/2012
**/

/** @defgroup doxy_group_hal_termometer HAL TERMOMETER

    The HAL SNSR_TEMP is a high level module whcih manages a temperature sensor
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_termometer_t 
    @brief      hal_termometer_t contains the possible ids for a temperature sensor
 **/
typedef enum
{
    hal_termometer1 = 0
} hal_termometer_t;

enum { hal_termometers_number = 1 };


/** @typedef    typedef int8_t hal_termometer_degrees_t 
    @brief      expresses temperature in celsius degrees with range [-128, +127] and 1 degree resolution.
 **/
typedef int16_t hal_termometer_degrees_t;


/** @typedef    typedef enum hal_termometer_cfg_t 
    @brief      hal_termometer_cfg_t contains the configuration for the temperature port.
 **/
typedef struct
{
    uint8_t         dummy;
} hal_termometer_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_termometer_cfg_t hal_termometer_cfg_default; // = { .dummy = 0 };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_termometer_init(hal_termometer_t id, const hal_termometer_cfg_t *cfg)
    @brief  	this function initializes a temperature port on a given port
    @param  	port	        the port
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_termometer_init(hal_termometer_t id, const hal_termometer_cfg_t *cfg);


/** @fn			extern hal_result_t hal_termometer_read(hal_termometer_t id, hal_termometer_degrees_t* degrees)
    @brief  	this function reads temperatures on a given port.
    @param  	id	            the id
    @param  	degrees 	    the read degrees
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_termometer_read(hal_termometer_t id, hal_termometer_degrees_t* degrees);


/** @}            
    end of group doxy_group_hal_termometer  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



