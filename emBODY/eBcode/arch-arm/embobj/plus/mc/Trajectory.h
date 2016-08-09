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

#ifndef MC_TRAJECTORY_H___
#define MC_TRAJECTORY_H___

#include "EoCommon.h"

#include "EOemsControllerCfg.h"

typedef struct // Trajectory 
{
    float pos_min;
    float pos_max;
    
    float vel_max;
    float acc_max;
    
    //////////////////
    // position
    float xX;
    float xV;
    float xA;
    
    float xT;
    float xTimer;
    
    //////////////////
    // velocity
    BOOL bVelocityMove;
    
    float vX;
    float vV;
    float vA;
    
    //float vVf;
   
    float vT;
    float vTimer;
    
    float xInvT;
    float vInvT;
    
    float xK0,xK1,xK2,xK3;
    float vK0,vK1,vK2;
} Trajectory; 

extern Trajectory* Trajectory_new(uint8_t n);

extern void Trajectory_config_limits(Trajectory *o, float pos_min, float pos_max, float vel_max, float acc_max);

extern void Trajectory_set_pos_end(Trajectory *o, float p1, float avg_vel);
extern void Trajectory_set_vel_end(Trajectory *o, float v1, float avg_acc);
extern void Trajectory_set_pos_raw(Trajectory *o, float p0);
extern void Trajectory_set_vel_raw(Trajectory *o, float v0);

extern int8_t Trajectory_do_step(Trajectory *o, float *p, float *v, float *a);

extern void Trajectory_init(Trajectory *o, int32_t p0, int32_t v0, int32_t a0);
extern void Trajectory_stop(Trajectory *o, int32_t pos);
extern void Trajectory_velocity_stop(Trajectory *o);

extern int32_t Trajectory_get_pos_ref(Trajectory *o);
extern int32_t Trajectory_get_vel_ref(Trajectory *o);
extern int32_t Trajectory_get_acc_ref(Trajectory *o);

extern BOOL Trajectory_is_done(Trajectory* o);

#endif

