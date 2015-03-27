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

// - declaration of extern public functions ---------------------------------------------------------------------------
extern void hal_quad_enc_Init(void);

extern void hal_quad_enc_single_init(uint8_t encoder_number);

extern uint32_t hal_quad_enc_getCounter(uint8_t encoder_number);

extern void hal_quad_enc_reset_counter(uint8_t encoder_number);
#endif
