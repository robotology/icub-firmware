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

#ifndef _HAL_ENCODER_H_
#define _HAL_ENCODER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_encoder.h
    @brief      This header file implements interface to a generic hal encoder sensor 
    @author     marco.accame@iit.it
    @date       02/07/2013
**/

/** @defgroup doxy_group_hal_encoder HAL SENSOR ENCODER

    The HAL ENCODER ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_encoder_t 
    @brief      hal_encoder_t contains every possible encoder sensor. the way the encoder is read from the peripherals
                of the MPU depends on internals of the encoder file. in case of encoders using spi and mux, then the mapping
                between encoders and peripherals is contained in the brdcfg file.
 **/ 
typedef enum  
{ 
    hal_encoder1 = 0,
	hal_encoder2 = 1,
    hal_encoder3 = 2,         
    hal_encoder4 = 3,
	hal_encoder5 = 4,
    hal_encoder6 = 5
} hal_encoder_t;

enum { hal_encoders_number = 6 };


/** @typedef    typedef uint32_t hal_encoder_position_t
    @brief      contains value of encoder
 **/
typedef uint32_t hal_encoder_position_t;



/** @typedef    typedef struct hal_encoder_cfg_t;
    @brief      contains configuration data of encoder.
 **/
typedef struct
{
    hal_interrupt_priority_t    priority;       /**< the priority if the ISR underlying the encoder */
    void (*callback_on_rx)(void *arg);          /**< callback called when a new value for the encoder is available   */
    void*                       arg;            /**< argument of the callback: contains pointer to latest encoder value  */
} hal_encoder_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_encoder_cfg_t hal_encoder_cfg_default;   // = { .priority = hal_int_priority15, .callback_on_rx = NULL, .arg = NULL };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_encoder_init(hal_encoder_t id, const hal_encoder_cfg_t *cfg)
    @brief  	This function initializes the encoder and whatever is required for its behaviour (mux and spi).
    @param  	encoder 	    the encoder
    @param  	cfg 	        The configuration of the encoder. 
    @return 	hal_res_NOK_generic in case the encoder is not supported, else hal_res_OK
  */
extern hal_result_t hal_encoder_init(hal_encoder_t id, const hal_encoder_cfg_t *cfg);


/** @fn			extern hal_result_t hal_encoder_read_start(hal_encoder_t id)
    @brief  	This function starts reading of data from the encoder in a non-blocking way.
                When reading is finished, then the callback on reception is invoked, which can
                retrieve the value with hal_encoder_read().
    @param  	encoder 	    the encoder
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_encoder_read_start(hal_encoder_t id);


/** @fn			extern uint32_t hal_encoder_get_value(hal_encoder_t id, hal_encoder_position_t* value);
    @brief  	This function reads data previously acquired by a call of hal_encoder_start().
    @param  	encoder 	    the encoder
    @param  	value 	        keeps the value.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_encoder_get_value(hal_encoder_t id, hal_encoder_position_t* value);



/** @}            
    end of group doxy_group_hal_encoder  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



