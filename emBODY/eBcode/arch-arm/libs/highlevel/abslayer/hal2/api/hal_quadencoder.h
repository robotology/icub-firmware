/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Maggiali
 * email:   marco.maggiali@iit.it
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


/* @file       hal_quadencoder.h
	@brief      This file implements the quadrature encoder interface.
	@author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/

#ifndef _HAL_QUADENCODER_H_
#define _HAL_QUADENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"

/** @typedef    typedef enum hal_quadencoder_t 
    @brief      hal_quadencoder_t contains the port to use.
 **/ 
typedef enum  
{ 
    hal_quadencoder1       = 0,        
    hal_quadencoder2       = 1,          
	hal_quadencoder3       = 2,        
    hal_quadencoder4       = 3,
    hal_quadencoderALL     = 254,      /**< all supported  */
    hal_quadencoderNONE    = 255       /**< none */        
} hal_quadencoder_t;

enum { hal_quadencoders_number = 4 };

// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hal_boolval_t hal_quadencoder_supported_is(hal_quadencoder_t id)
    @brief      This function tells if the specified quad encoder is supported by the board. 
    @param      id              identifies encoder id 
    @return     hal_true or hal_false
  */
extern hal_boolval_t hal_quadencoder_supported_is(hal_quadencoder_t id);


/** @fn         extern void hal_quadencoder_init(hal_quadencoder_t id)
    @brief      This function initialize the chosen quadrature encoder 
                In mc4plus: every encoder is associated to a port and an hw timer. In particular:
                - 0 : P2, TIM2
                - 1 : P3, TIM3
                - 2 : P4, TIM4
                - 3 : P5. TIM5
   @param       id      encoder to initialize
   @retval      none
  */
extern hal_result_t hal_quadencoder_init(hal_quadencoder_t id);


/** @fn         uint32_t hal_quadencoder_get_counter(hal_quadencoder_t id)
    @brief      This function returns the angular position of the encoder,
                reading the counter of the associated timer
   @param       id  encoder to be read
   @retval      uint32_t        position read (hal_NA32 if invalid or id is not supported)
  */
extern uint32_t hal_quadencoder_get_counter(hal_quadencoder_t id);


/** @fn         hal_quadencoder_reset_counter(hal_quadencoder_t id)
    @brief      This function reset the value of the timer associated to the encoder,
                setting it to 0.
   @param       id  encoder to be reset
   @retval      none
  */
extern void hal_quadencoder_reset_counter(hal_quadencoder_t id);


/** @fn         hal_quadencoder_init_indexes_flags(void)
    @brief      This function initialize the interrupt line (which is not mandatory) of the index for the quadrature encoder 
   @param       none
   @retval      none
  */
extern void hal_quadencoder_init_indexes_flags(void);


/** @fn         hal_quadencoder_is_index_found(hal_quadencoder_t id)
    @brief      This function checks if the encoder has reached (no matter when in the past) the index. If it returns true, it also clears the
                flag set by the interrupt line, so that it will be set again only when the index will be reached again 
   @param       id  encoder flag to be read
   @retval      none
  */
extern hal_bool_t hal_quadencoder_is_index_found(hal_quadencoder_t id);

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


