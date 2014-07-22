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

void hal_quad_enc_Init(void);
int32_t hal_quad_enc_getCounter(int16_t encoder_number);
#endif
