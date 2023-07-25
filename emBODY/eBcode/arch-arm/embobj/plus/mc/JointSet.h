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

#ifndef MC_JOINT_SET___
#define MC_JOINT_SET___

#ifdef WRIST_MK2
#include "wrist_decoupler.h"
#endif

#include "EoCommon.h"

#include "EOemsControllerCfg.h"

#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"
#include "Pid.h"

#include "CalibrationHelperData.h"

#include "hal_led.h"


typedef struct // JointSet
{
    hal_led_t led;
    
    uint8_t* pN;
    uint8_t* pE;
    
    uint8_t* joints_of_set;
    uint8_t* motors_of_set;
    uint8_t* encoders_of_set;
    
    Joint* joint;
    Motor* motor;

    float** Jjm;
    float** Jmj; 
    
    float** Sjm;
    float** Smj;
    
    float** Sje;
    
    int torque_ctrl_out_type;
    int olooop_ctrl_out_type;
    int curent_ctrl_out_type;
    int postrj_ctrl_out_type;
    int veltrj_ctrl_out_type;
    int mixtrj_ctrl_out_type;
    int posdir_ctrl_out_type;
    int veldir_ctrl_out_type;
        
    uint32_t calibration_wait;
    
    AbsEncoder* absEncoder;
 
    int16_t motor_input_type;
    eOmc_controlmode_t control_mode;
    eOmc_interactionmode_t interaction_mode;
    
    BOOL pos_control_active;
    BOOL trq_control_active;
    BOOL USE_SPEED_FBK_FROM_MOTORS;
    
    BOOL is_calibrated;
    
    BOOL external_fault;
    
    eOmc_jsetconstraint_t special_constraint;
    
    int32_t special_limit;
    
    eOmc_calibration_type_t calibration_in_progress;
    
    int32_t calibration_timeout;
    
#ifdef WRIST_MK2
    BOOL is_parking;
    BOOL must_park;
    
    wrist_decoupler wristDecoupler;
    
    Trajectory ypr_trajectory[3];
    
    CTRL_UNITS ypr_pos_ref[3];
    CTRL_UNITS ypr_vel_ref[3];
    CTRL_UNITS ypr_acc_ref[3];
    CTRL_UNITS ypr_pos_fbk[3];
    
    //CTRL_UNITS arm_pos_off[3];
#endif

    TripodCalib tripod_calib;
} JointSet;

extern JointSet* JointSet_new(uint8_t n); //

extern void JointSet_init(JointSet* o); //

extern void JointSet_config //
(
    JointSet* o,
    uint8_t* pN,
    uint8_t* pE,
    uint8_t* joints_of_set,
    uint8_t* motors_of_set,
    uint8_t* encoders_of_set,
    Joint* joint,
    Motor* motor,
    AbsEncoder *absEncoder,
    float** Jjm, float** Jjm_hard,
    float** Jmj,
    float** Sje,
    float** Sjm
);

extern void JointSet_do_odometry(JointSet* o); //
extern BOOL JointSet_do_check_faults(JointSet* o); //
extern void JointSet_do_control(JointSet* o); //

extern void JointSet_do(JointSet* o);
    
extern BOOL JointSet_set_control_mode(JointSet* o, eOmc_controlmode_command_t control_mode_cmd);
extern void JointSet_set_interaction_mode(JointSet* o, eOmc_interactionmode_t interaction_mode);

extern void JointSet_calibrate(JointSet* o, uint8_t e, eOmc_calibrator_t *calibrator);

extern void JointSet_do_pwm_control(JointSet* o);
    
extern void JointSet_send_debug_message(char *message, uint8_t jid, uint16_t par16, uint64_t par64);

#ifdef WRIST_MK2
extern BOOL JointSet_set_pos_ref(JointSet* o, int j, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref);
extern void JointSet_get_state(JointSet* o, int j, eOmc_joint_status_t* joint_state);
extern void JointSet_stop(JointSet* o, int j);
#endif
 
#endif  // include-guard


