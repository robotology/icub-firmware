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

typedef float float_fsticks_per_s_t;

typedef struct //PID
{
    float Kc;
    float Kp;
    float Kd;
    float Ki;

    float Kff;
    float Kdith;
    float Kbemf;
    float Ktau;

    float Dn;
    float En;
    float In;
    float Imax;

    float A,B;

    float stiction_up;
    float stiction_down;
    float ditheringVal;
	  float_fsticks_per_s_t ditheringMotorVel;

    float out_max;
    float out_lpf;
    float out;

    uint8_t filter;
} PID;

#define MIN_VEL_OBSERVABLE 10 /* in degrees/s */
#define PWM_FULLSCALE 32000
#define TAU_REF_SCALE 1000000
#define STICTION_INPUT_SCALE (TAU_REF_SCALE*100/PWM_FULLSCALE)
#define VEL_FULLSCALE 0x10000
#define DITHERING_MOTOR_VEL_DFLT 100 /* in motor deg/s */

PID* PID_new(uint8_t n);
extern void PID_init(PID* o);
extern void PID_config(PID* o, eOmc_PID_t* config);
extern void PID_config_friction(PID *o, CTRL_UNITS gearbox, float Kbemf, float Ktau);
extern void PID_config_filter(PID *o, uint8_t filter);

extern void PID_reset(PID* o);
extern void PID_get_state(PID* o, float *out, float *err);

extern float PID_do_out(PID* o, float En);
extern float PID_do_friction_comp(PID *o, float vel_raw, float vel, float Tr);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif

#endif  // include-guard
