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
    float Kbemf;
    float Ktau;
    
    float Dn;
    float En;
    float In;
    float Imax;
    
    float A,B;
    
    float stiction_up;
    float stiction_down;

    float viscous_pos_val;
    float viscous_neg_val;
    float coulomb_pos_val;
    float coulomb_neg_val;
    float velocityThres_val;

#ifdef FINGER_MK3
    float out_max_open;
    float out_max_close;
#else
    float out_max;
#endif

    float out_lpf;
    float out;
    
    uint8_t filter;
} PID; 

PID* PID_new(uint8_t n);
extern void PID_init(PID* o);
extern void PID_config(PID* o, eOmc_PID_t* config);
extern void PID_config_friction(PID *o, float Kbemf, float Ktau, eOmc_FrictionParams_t friction);
extern void PID_config_filter(PID *o, uint8_t filter);

extern void PID_reset(PID* o);
extern void PID_get_state(PID* o, float *out, float *err);

extern float PID_do_out(PID* o, float En);
extern float PID_do_friction_comp(PID *o, float vel, float Tr);


 
#endif  // include-guard


