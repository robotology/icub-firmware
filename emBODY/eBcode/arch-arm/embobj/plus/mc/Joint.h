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

#ifndef MC_JOINT___
#define MC_JOINT___

#ifdef __cplusplus
extern "C" {
#endif

#include "EoCommon.h"

#include "EOemsControllerCfg.h"

#include "Pid.h"
#include "Trajectory.h"
#include "WatchDog.h"

#include "CalibrationHelperData.h"

typedef union
{
    struct
    {
        uint8_t torque_sensor_timeout:1;
        uint8_t hard_limit_reached:1;
    } bits;
        
    uint8_t bitmask;
} JointFaultState;


typedef struct
{
    int32_t motor_pos_min;
    int32_t motor_pos_max;
    //CTRL_UNITS last_joint_pos;
    int32_t max_tension;
} CableConstraintData;

typedef enum
{
    calibtype6_st_inited =0,
    calibtype6_st_jntEncResComputed = 1,
    calibtype6_st_absEncoderCalibrated = 2,
    calibtype6_st_trajectoryStarted = 3,
    calibtype6_st_finished = 4
} calibtype6_states;

 typedef struct
 {
     BOOL is_active;
     CTRL_UNITS targetpos;
     CTRL_UNITS velocity;
     calibtype6_states state;
     int32_t computedZero;
     int32_t rotorposmin;
     int32_t rotorposmax;
 } jointCalibType6Data;




typedef enum
{
    calibtype7_st_inited =0,
    calibtype7_st_jntEncResComputed = 1,
    calibtype7_st_jntCheckLimits = 2,
    calibtype7_st_finished = 3
} calibtype7_states;

typedef struct
{
    BOOL is_active;
    calibtype7_states state;
    int32_t computedZero; 
} jointCalibType7Data;



typedef struct
{
    union
    {
        jointCalibType6Data type6;
        jointCalibType7Data type7;
    }data;
    eOmc_calibration_type_t type;
} jointCalibrationData;

typedef struct Joint_hid Joint;

extern Joint* Joint_new(uint8_t n);
extern void Joint_init(Joint* o);
extern void Joint_destroy(Joint* o);

extern void Joint_reset_calibration_data(Joint* o);

extern void Joint_config(Joint* o, uint8_t ID, eOmc_joint_config_t* config);

extern void Joint_motion_reset(Joint *o);

extern void Joint_update_odometry_fbk(Joint* o, CTRL_UNITS pos_fbk, CTRL_UNITS vel_fbk);
extern void Joint_update_torque_fbk(Joint* o, CTRL_UNITS trq_fbk);
extern void Joint_update_status_reference(Joint* o);

extern BOOL Joint_check_faults(Joint* o);
extern void Joint_clear_faults(Joint* o);

extern int8_t Joint_check_limits(Joint* o);
extern int8_t Joint_pushing_limit(Joint* o);

extern void Joint_set_limits(Joint* o, CTRL_UNITS pos_min, CTRL_UNITS pos_max);
extern void Joint_set_hardware_limit(Joint* o);

extern BOOL Joint_manage_cable_constraint(Joint* o);
extern BOOL Joint_manage_R1_finger_tension_constraint(Joint* o);

extern CTRL_UNITS Joint_do_pwm_or_current_control(Joint* o);
extern CTRL_UNITS Joint_do_vel_control(Joint* o);

extern BOOL Joint_set_control_mode(Joint* o, eOmc_controlmode_command_t control_mode);
extern BOOL Joint_set_interaction_mode(Joint* o, eOmc_interactionmode_t interaction_mode);

extern void Joint_set_impedance(Joint* o, eOmc_impedance_t* impedance);
extern void Joint_get_impedance(Joint* o, eOmc_impedance_t* impedance);

extern void Joint_get_state(Joint* o, eOmc_joint_status_t* joint_state);

extern BOOL Joint_get_pid_state(Joint* o, eOmc_joint_status_ofpid_t* pid_state);

extern BOOL Joint_set_pos_ref(Joint* o, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref);
extern BOOL Joint_set_vel_ref(Joint* o, CTRL_UNITS vel_ref, CTRL_UNITS acc_ref);
extern BOOL Joint_set_pos_raw(Joint* o, CTRL_UNITS pos_ref);
extern BOOL Joint_set_vel_raw(Joint* o, CTRL_UNITS vel_ref);
extern BOOL Joint_set_trq_ref(Joint* o, CTRL_UNITS trq_ref);
extern BOOL Joint_set_pwm_ref(Joint* o, CTRL_UNITS pwm_ref);
extern BOOL Joint_set_cur_ref(Joint* o, CTRL_UNITS cur_ref);
extern void Joint_stop(Joint* o);

extern BOOL Joint_set_pos_ref_in_calibType6(Joint* o, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref);

//VALE: debug function. I'll remove it ASAP
//extern void Joint_update_debug_current_info(Joint *o, int32_t avgCurrent, int32_t accum_Ep);

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


