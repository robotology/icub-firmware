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

/////////////////////////////////////////////////////////
// Motor

#ifndef MC_MOTOR_H___
#define MC_MOTOR_H___


#include "EoCommon.h"
#include "EoMotionControl.h"

#include "WatchDog.h"
#include "Pid.h"

typedef struct
{
    int16_t current;
    int16_t velocity;
    int32_t position;
} CanOdometry2FocMsg; 


typedef struct Motor_hid Motor;


extern Motor* Motor_new(uint8_t n);  //
extern void Motor_init(Motor* o);  //
extern void Motor_config(Motor* o, uint8_t ID, eOmc_motor_config_t* config); //
extern void Motor_destroy(Motor* o); //

extern BOOL Motor_is_still(Motor* o, int32_t space_window, int32_t time_window);
extern void Motor_still_check_reset(Motor* o);

extern void Motor_config_encoder(Motor* o, int32_t resolution);
extern void Motor_config_max_currents(Motor* o, eOmc_current_limits_params_t* current_params);
extern void Motor_config_max_temperature(Motor* o, eOmeas_temperature_t* motor_temperature_limit);

extern void Motor_config_current_PID(Motor* o, eOmc_PID_t* pid);
extern void Motor_config_torque_PID(Motor* o, eOmc_PID_t* pid);
extern void Motor_config_speed_PID(Motor* o, eOmc_PID_t* pid);

extern void Motor_config_filter(Motor* o, uint8_t filter); //
extern void Motor_config_friction(Motor* o, float Bemf, float Ktau, eOmc_FrictionParams_t friction); //
extern void Motor_config_LuGre(Motor* o, float Km, float Kw, float S0, float S1, float Vth, float Fc, float Fs);
extern void Motor_calibrate_withOffset(Motor* o, int32_t offset); //
extern BOOL Motor_calibrate_moving2Hardstop(Motor* o, int32_t pwm, int32_t zero); //
extern void Motor_uncalibrate(Motor* o);

extern BOOL Motor_set_run(Motor* o, int16_t low_lev_ctrl_type); //
extern void Motor_set_idle(Motor* o); //
extern void Motor_force_idle(Motor* o); //

extern void Motor_motion_reset(Motor *o); //
extern BOOL Motor_is_calibrated(Motor* o); //
extern BOOL Motor_check_faults(Motor* o); //
extern void Motor_reset(Motor *o);//

extern CTRL_UNITS Motor_do_trq_control(Motor* o, CTRL_UNITS trq_ref, CTRL_UNITS trq_fbk, CTRL_UNITS motor_vel_kf_icubdeg_sec); //
extern CTRL_UNITS Motor_do_trq_control_EXPERIMENTAL(Motor* o, CTRL_UNITS trq_ref, CTRL_UNITS trq_fbk, CTRL_UNITS motor_vel_icubdeg_sec);
extern void Motor_update_state_fbk(Motor* o, void* state_msg); //
extern void Motor_update_odometry_fbk_can(Motor* o, CanOdometry2FocMsg* data); //
extern void Motor_do_calibration_hard_stop(Motor* o); //

// marco.accame on 08aug2023: in here o is used as an array of MAX_MOTORS_PER_BOARD Motor*  
extern void Motor_actuate(Motor* o, uint8_t N, MC_ACTUATION_t act); //

extern void Motor_set_pwm_ref(Motor* o, int32_t pwm_ref);
extern void Motor_set_Iqq_ref(Motor* o, int32_t Iqq_ref);
//extern void Motor_set_pos_ref(Motor* o, int32_t vel_ref);
extern void Motor_set_vel_ref(Motor* o, int32_t vel_ref);
//extern void Motor_set_trq_ref(Motor* o, CTRL_UNITS trq_ref);

//extern uint32_t Motor_get_fault_mask(Motor* o);
extern void Motor_get_pid_state(Motor* o, eOmc_joint_status_ofpid_t* pid_state);
extern void Motor_get_state(Motor* o, eOmc_motor_status_t* motor_status);
extern void Motor_update_pos_fbk(Motor* o, int32_t position_raw);
extern void Motor_update_current_fbk(Motor* o, int16_t current);
extern void Motor_update_temperature_fbk(Motor* o, int16_t temperature);

extern void Motor_set_i2t_fault(Motor* o);

extern void Motor_raise_fault_i2t(Motor* o);
//extern void Motor_raise_fault_external(Motor* o);
extern BOOL Motor_is_external_fault(Motor* o);
extern BOOL Motor_is_in_fault(Motor* o);
extern BOOL Motor_is_running(Motor* o);

extern BOOL Motor_is_motor_joint_fault_over(Motor* o);

//BOOL Motor_clear_ext_fault(Motor *o);

////////////////////////////////////////////////////////////////////////////
extern void Motor_config_gearbox_M2J(Motor* o, float32_t gearbox_M2J);
extern int16_t Motor_config_pwm_limit(Motor* o, int16_t pwm_limit);
////////////////////////////////////////////////////////////////////////////
/*
extern void Motor_update_pos_raw_fbk(Motor* o, int32_t pos_raw_fbk);
extern void Motor_update_vel_raw_fbk(Motor* o, int32_t vel_raw_fbk);
extern void Motor_update_pwm_fbk(Motor* o, int16_t pwm_fbk);
extern void Motor_update_Iqq_fbk(Motor* o, int16_t Iqq_fbk);
extern void Motor_update_pos_fbk(Motor* o, int32_t pos_fbk);
extern void Motor_update_vel_fbk(Motor* o, int32_t vel_fbk);
extern void Motor_update_trq_fbk(Motor* o, CTRL_UNITS trq_fbk);

extern int16_t Motor_get_temperature_fbk(Motor* o);
extern int32_t Motor_get_pos_raw_fbk(Motor* o);
extern int32_t Motor_get_vel_raw_fbk(Motor* o);
extern int16_t Motor_get_pwm_fbk(Motor* o);
extern int16_t Motor_get_Iqq_fbk(Motor* o);
extern int32_t Motor_get_pos_fbk(Motor* o);
extern int32_t Motor_get_vel_fbk(Motor* o);
extern CTRL_UNITS Motor_get_trq_fbk(Motor* o);
*/

 
 
#endif  // include-guard



// Motor
/////////////////////////////////////////////////////////
