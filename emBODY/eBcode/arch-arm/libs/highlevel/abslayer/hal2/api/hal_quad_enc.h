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


/* @file       hal_quad_enc.h
	@brief      This file implements the quadrature encoder interface.
	@author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/

#ifndef _HAL_QUAD_ENC_H_
#define _HAL_QUAD_ENC_H_

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"

/** @typedef    typedef enum hal_quad_enc_t 
    @brief      hal_quad_enc_t contains the port to use.
 **/ 
typedef enum  
{ 
    hal_quad_enc1       = 0,        
    hal_quad_enc2       = 1,          
	hal_quad_enc3       = 2,        
    hal_quad_enc4       = 3       
} hal_quad_enc_t;

// - declaration of extern public functions ---------------------------------------------------------------------------
/** @fn         extern void hal_quad_enc_Init(void)
    @brief      This function initialize all the quadrature encoders for the MC4 plus boards.
                It uses the ports from P2 to P5 (included) and HW TIMERS 2 to 5.
   @return      none
  */
extern void hal_quad_enc_Init(void);

/** @fn         extern void hal_quad_single_init(void)
    @brief      This function initialize the quadrature choosen as a parameter.
    Every encoder is associated to a port and an hw timer. In particular:
    - 0 : P2, TIM2
    - 1 : P3, TIM3
    - 2 : P4, TIM4
    - 3 : P5. TIM5
   @param       encoder_number  encoder to initialize
   @retval      none
  */
extern void hal_quad_enc_single_init(uint8_t encoder_number);

/** @fn         uint32_t hal_quad_enc_getCounter(uint8_t encoder_number)
    @brief      This function returns the angular position of the encoder,
                reading the counter of the associated timer
   @param       encoder_number  encoder to be read
   @retval      uint32_t        position read
  */
extern uint32_t hal_quad_enc_getCounter(uint8_t encoder_number);

/** @fn         hal_quad_enc_reset_counter(uint8_t encoder_number)
    @brief      This function reset the value of the timer associated to the encoder,
                setting it to 0.
   @param       encoder_number  encoder to be reset
   @retval      none
  */
extern void hal_quad_enc_reset_counter(uint8_t encoder_number);
#endif
