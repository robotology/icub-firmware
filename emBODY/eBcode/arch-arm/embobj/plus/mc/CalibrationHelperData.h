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


#include "EOemsControllerCfg.h"

typedef struct // TripodCalib
{
    int32_t pwm;          // [2FOC PWM units] (-32000 : +32000) = (-100% : +100%)
    int32_t zero;         // [millimiters]
    int32_t max_delta;    // [millimeters]
    int32_t start_pos[3]; // [millimeters]
    
} TripodCalib;

typedef struct // CableCalib
{
    int32_t pwm;          // [MC2+ PWM units] (-3360 : +3360) = (-100% : +100%)
    int32_t delta;        // [icubdegrees]
    int32_t target;       // [icubdegrees]
    int32_t cable_range;  // [icubdegrees]
} CableCalib;

typedef struct // HardStopCalib
{
    int32_t pwm;       // [2FOC PWM units] (-32000 : +32000) = (-100% : +100%)
    int32_t zero;      // [icubdegrees]
    int32_t space_thr; // [icubdegrees]  
    int32_t time_thr;  // [milliseconds]
} HardStopCalib;


 
#endif  // include-guard


