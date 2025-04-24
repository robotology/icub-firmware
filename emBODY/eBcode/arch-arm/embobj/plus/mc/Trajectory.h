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

// opaque struct

typedef struct Trajectory_hid Trajectory;


// API

// marco.accame on 16apr2025: not used
// extern Trajectory* Trajectory_new(uint8_t n);

// embot::app::mc::Trajectory::config()
extern void Trajectory_config_limits(Trajectory *o, float pos_min, float pos_max, float vel_max, float acc_max);

// embot::app::mc::Trajectory::set() for all of them
extern void Trajectory_set_pos_end(Trajectory *o, float p1, float avg_vel);
extern void Trajectory_set_vel_end(Trajectory *o, float v1, float avg_acc);
extern void Trajectory_set_pos_raw(Trajectory *o, float p0);
extern void Trajectory_set_vel_raw(Trajectory *o, float v0);

// marco.accame on 16apr2025: used only if WRIST_MK2 is defined
#if defined(WRIST_MK2)
extern void Trajectory_start2end(Trajectory *o, int32_t start, float end, float velAvg);
#endif

// embot::app::mc::Trajectory::tick() once to advance simulation step + embot::app::mc::Trajectory::get() as may times we need to get the latest {p, v, a}
extern int8_t Trajectory_do_step(Trajectory *o, float *p, float *v, float *a);

// embot::app::mc::Trajectory::reset() as it is always used w/ 0 arguments
extern void Trajectory_init(Trajectory *o, int32_t p0, int32_t v0, int32_t a0);
// embot::app::mc::Trajectory::stop()
extern void Trajectory_stop(Trajectory *o, int32_t pos);
// embot::app::mc::Trajectory::stop()
extern void Trajectory_velocity_stop(Trajectory *o);

// marco.accame on 16apr2025: not used
// extern int32_t Trajectory_get_pos_ref(Trajectory *o);
// extern int32_t Trajectory_get_vel_ref(Trajectory *o);
// extern int32_t Trajectory_get_acc_ref(Trajectory *o);

// embot::app::mc::Trajectory::isdone()
extern BOOL Trajectory_is_done(Trajectory* o);

// embot::app::mc::Trajectory::get() for both of them
extern float Trajectory_get_target_position(Trajectory* o);
extern float Trajectory_get_target_velocity(Trajectory* o);


#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

