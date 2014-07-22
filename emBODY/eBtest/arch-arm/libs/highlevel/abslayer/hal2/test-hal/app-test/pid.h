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


/* @file        pid.h
	@brief      This file implements the PID.
	@author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/
// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _PID_H_
#define _PID_H_

#include "hal_base.h"


typedef struct 
{  
  int16_t hKp_Gain;
  uint16_t hKp_Divisor;
  int16_t hKi_Gain;
  uint16_t hKi_Divisor;  
  int16_t hLower_Limit_Output;     //Lower Limit for Output limitation
  int16_t hUpper_Limit_Output;     //Lower Limit for Output limitation
  int32_t wLower_Limit_Integral;   //Lower Limit for Integral term limitation
  int32_t wUpper_Limit_Integral;   //Lower Limit for Integral term limitation
  int32_t wIntegral;
  // Actually used only if DIFFERENTIAL_TERM_ENABLED is enabled in
  //stm32f10x_MCconf.h
  int16_t hKd_Gain;
  uint16_t hKd_Divisor;
  int32_t wPreviousError;
} PID_Struct_t;

void PID_Init (PID_Struct_t *,PID_Struct_t *);
void PID_Speed_Coefficients_update(int16_t, PID_Struct_t *);
int16_t PID_Regulator(int16_t, int16_t, PID_Struct_t *);

#endif
