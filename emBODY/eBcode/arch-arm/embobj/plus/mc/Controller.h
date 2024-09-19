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

#ifndef MC_CONTROLLER___
#define MC_CONTROLLER___

// this file Controller.h is the entry point for the usage of this MController module.
// the module can be used by C modules and also by C++ modules as long as:
// - we keep the following extern "C" { clause,
// - we dont add in here any C++ include files
// We surely can use C++ code inside any other .h and .c files, such as AbsEncoder.[c, h], etc.
// and we MUST compile the files using C++

#ifdef __cplusplus
extern "C" {
#endif


// to see basic types
#include "EoCommon.h"
    
// to see eOmn_serv_configuration_t etc
#include "EoManagement.h"    

// to see eOmc_joint_status_t etc
#include "EoMotionControl.h"    
    
// to see CTRL_UNITS
#include "EOemsControllerCfg.h"

// need to use this opaque struct in order to 
// remove from the interface any possible .h files, such as Joint.h etc, containing C++ code.
typedef struct MController_hid MController;

extern MController* MController_new(uint8_t nJoints, uint8_t nEncoders); //

extern void MController_init(void); //
extern void MController_deinit(void); //
extern void MController_config_board(const eOmn_serv_configuration_t* brd_cfg);

extern void MController_config_joint(int j, eOmc_joint_config_t* config); //
extern void MController_config_motor(int m, eOmc_motor_config_t* config); //
extern void MController_config_Jjm(float **Jjm); //
extern void MController_config_Jje(float **Jje); //

extern void MController_update_motor_state_fbk(uint8_t m, void* state);
extern void MController_update_joint_torque_fbk(uint8_t j, CTRL_UNITS trq_fbk); //
extern void MController_update_absEncoder_fbk(uint8_t e, uint32_t* positions); //

extern void MController_invalid_absEncoder_fbk(uint8_t e, uint8_t error_type); // use ae_errortype_t or similar type

extern void MController_timeout_absEncoder_fbk(uint8_t e);

extern int32_t MController_get_absEncoder(uint8_t j); //

extern void MController_do(void); //

extern BOOL MController_set_control_mode(uint8_t j, eOmc_controlmode_command_t control_mode);
extern void MController_set_interaction_mode(uint8_t j, eOmc_interactionmode_t interaction_mode);

extern void MController_calibrate(uint8_t e, eOmc_calibrator_t *calibrator);

extern void MController_go_idle(void);

extern void MController_get_joint_state(int j, eOmc_joint_status_t* joint_state);
extern void MController_get_pid_state(int j, eOmc_joint_status_ofpid_t* pid_state, BOOL decoupled_pwm);
extern void MController_get_motor_state(int m, eOmc_motor_status_t* motor_status);
extern void MController_update_joint_targets(int j);
extern void MController_update_motor_pos_fbk(int m, int32_t position_raw);
extern void MController_update_motor_current_fbk(int m, int16_t current);
extern void MController_update_motor_temperature_fbk(int m, int16_t temperature);
extern void MController_config_motor_friction(int m, eOmc_motor_params_t* params); //
extern void MController_config_LuGre_params(int m, eOmc_LuGre_params_t* params);
extern void MController_config_joint_impedance(int j, eOmc_impedance_t* impedance); //

extern void MController_config_minjerk_pid(int j, eOmc_PID_t *pid_conf); //
extern void MController_config_direct_pid(int j, eOmc_PID_t *pid_conf); //

extern void MController_motor_config_current_PID(int m, eOmc_PID_t* pid);
extern void MController_motor_config_torque_PID(int m, eOmc_PID_t *pid);
extern void MController_motor_config_speed_PID(int m, eOmc_PID_t *pid);

extern void MController_config_joint_pos_limits(int j, int32_t pos_min, int32_t pos_max);
extern void MController_config_joint_vel_ref_timeout(int j, int32_t timeout_ms);
extern void MController_motor_config_max_currents(int m, eOmc_current_limits_params_t* current_params);
extern void MController_motor_config_max_temperature(int m, eOmeas_temperature_t* temperature);

extern BOOL MController_set_joint_pos_ref(int j, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref);
extern BOOL MController_set_joint_vel_ref(int j, CTRL_UNITS vel_ref, CTRL_UNITS acc_ref);
extern BOOL MController_set_joint_pos_raw(int j, CTRL_UNITS pos_ref);
extern BOOL MController_set_joint_vel_raw(int j, CTRL_UNITS vel_ref);
extern BOOL MController_set_joint_trq_ref(int j, CTRL_UNITS trq_ref);
extern BOOL MController_set_joint_pwm_ref(int j, CTRL_UNITS pwm_ref);
extern BOOL MController_set_joint_cur_ref(int j, CTRL_UNITS cur_ref);

extern void MController_stop_joint(int j);
extern void MController_config_motor_gearbox_M2J(int m, float32_t gearbox_M2J);
extern void MController_config_motor_encoder(int m, int32_t resolution);
extern int16_t MController_config_motor_pwm_limit(int m, int16_t pwm_limit);
extern void MController_update_motor_odometry_fbk_can(int m, void* data);

extern void MController_motor_raise_fault_i2t(int m);



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


