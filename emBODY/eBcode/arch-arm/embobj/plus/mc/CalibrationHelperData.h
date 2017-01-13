/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
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
 * MERCHANTABILITY or  FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License foFITNESSr more details
*/

#ifndef MC_CALIBRATION_HELPER_DATA_H___
#define MC_CALIBRATION_HELPER_DATA_H___

#ifdef __cplusplus
extern "C" {
#endif

#include "EOemsControllerCfg.h"

typedef struct // TripodCalib
{
    int32_t pwm;
    int32_t zero;
    int32_t max_delta;
    int32_t start_pos[3];
    
} TripodCalib;

typedef struct // CableCalib
{
    int32_t pwm;
    int32_t delta;
    int32_t target;
    int32_t cable_range;
} CableCalib;


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


