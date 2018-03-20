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

#ifndef MC_PID_H___
#define MC_PID_H___

#ifdef __cplusplus
extern "C" {
#endif

#include "EoCommon.h"
#include "EoMotionControl.h"

#include "EOemsControllerCfg.h"

typedef struct //PID 
{
    float Ko;
    float Kp;
    float Kd;
    float Ki;
    float Kff;

    float Ko__delta;
    float Kp__delta;
    float Kd__delta;
    float Ki__delta; 
    float Kff_delta;
    
    float Ko__final;
    float Kp__final;
    float Kd__final;
    float Ki__final;
    float Kff_final;

    float Kbemf;
    float Ktau;
    
    float Dn;
    float En;
    float In;
    float Imax;
    
    float A,B;
    
    float stiction_up;
    float stiction_down;
  
    float out_max;
    float out_lpf;
    float out;
    
    uint16_t slope_time_ms;
    
    uint8_t filter;
} PID; 

PID* PID_new(uint8_t n);
extern void PID_init(PID* o);
extern void PID_config(PID* o, eOmc_PID_t* config);
extern void PID_config_friction(PID *o, float Kbemf, float Ktau);
extern void PID_config_filter(PID *o, uint8_t filter);

extern void PID_reset(PID* o);
extern void PID_get_state(PID* o, float *out, float *err);

extern float PID_do_out(PID* o, float En);
extern float PID_do_friction_comp(PID *o, float vel, float Tr);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


