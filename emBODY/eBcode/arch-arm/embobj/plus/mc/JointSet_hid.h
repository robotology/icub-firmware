
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef MC_JOINTSET_HID___
#define MC_JOINTSET_HID___

#ifdef WRIST_MK2
#include "Trajectory_hid.h"
#include "wrist_decoupler.h"
#endif

#include "EoCommon.h"
#include "EOemsControllerCfg.h"
#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"
#include "CalibrationHelperData.h" 

#if defined(MC_use_embot_app_mc_Trajectory)
#include "embot_app_mc_Trajectory.h"
#endif

#if defined(MC_use_Trajectory)
#include "Trajectory.h"
#include "Trajectory_hid.h"
#endif




#ifdef WRIST_MK2

enum wrist_mk_version_t {WRIST_MK_VER_2_0 = 20,  WRIST_MK_VER_2_1 = 21};

#if defined(MC_use_embot_app_mc_Trajectory)
    struct Trajectories
    {
        static constexpr size_t njoints {3};    
        embot::app::mc::Trajectory *ypr_trajectory[njoints] {nullptr, nullptr, nullptr};
        embot::app::mc::Trajectory *prk_trajectory[njoints] {nullptr, nullptr, nullptr};
    };
#endif

/* The struct wristMK2_t contains all the data related to the wrist MK2*/
struct wristMK2_t
{
    // The warmup flag is == 2 at startup and makes the wrist start a first parking move.
    // warmup == 1 means first parking in progress, and warmup == 0 means first parking done.
    // Thanks to the new direct kinematics solver all the parking procedures will be likely removed
    // after some time of use of the robot without problems because no more necessary.
    BOOL is_parking;
    int warmup;
    
    int watchdog;
    
    wrist_mk_version_t mk_version;
    BOOL is_right_wrist;
    
    wrist_decoupler wristDecoupler;

#if defined(MC_use_embot_app_mc_Trajectory)
//    static constexpr size_t njoints {3};    
//    embot::app::mc::Trajectory *ypr_traj[njoints] {nullptr, nullptr, nullptr};
//    embot::app::mc::Trajectory *prk_traj[njoints] {nullptr, nullptr, nullptr};
    Trajectories trajectories {};
#endif
    
#if defined(MC_use_Trajectory)    
    Trajectory ypr_trajectory[3];
    Trajectory prk_trajectory[3];
#endif
    
    CTRL_UNITS ypr_pos_ref[3];
    CTRL_UNITS ypr_vel_ref[3];
    CTRL_UNITS ypr_acc_ref[3];
    CTRL_UNITS ypr_pos_fbk[3];
    
    CTRL_UNITS prk_pos_ref[3];
    CTRL_UNITS prk_vel_ref[3];
    CTRL_UNITS prk_acc_ref[3];
    
    CTRL_UNITS last_valid_pos[3];

};

#endif

struct JointSet_hid 
{    
    uint8_t* pN;
    uint8_t* pE;
    
    uint8_t* joints_of_set;
    uint8_t* motors_of_set;
    uint8_t* encoders_of_set;
    
    // the following 8 do not hold data and are just the pointers to the same variables in MController 
    // and every instance of JointSet has them so that it can retrieve its own joints, motors, encoders etc
    Joint* joint;           
    Motor* motor;           
    AbsEncoder* absEncoder; 
    float** Jjm;
    float** Jmj;     
    float** Sjm;
    float** Smj;    
    float** Sje; // this Sje is either NULL (case of no encoder coupling) or it is the same pointer inside MController
    
    int torque_ctrl_out_type;
    int olooop_ctrl_out_type;
    int curent_ctrl_out_type;
    int postrj_ctrl_out_type;
    int veltrj_ctrl_out_type;
    int mixtrj_ctrl_out_type;
    int posdir_ctrl_out_type;
    int veldir_ctrl_out_type;
        
    uint32_t calibration_wait;
    
 
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
    wristMK2_t wristMK2;
    #endif

    TripodCalib tripod_calib;
    HardStopCalib hard_stop_calib;    
};

 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
