
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef MC_JOINT_HID___
#define MC_JOINT_HID___
 

#include "EOemsControllerCfg.h"

#include "Pid.h"
#include "Pid_hid.h"
#include "WatchDog.h"
#include "WatchDog_hid.h"
#include "CalibrationHelperData.h"
#include "kalman_filter.h"

#if defined(MC_use_embot_app_mc_Trajectory)
#include "embot_app_mc_Trajectory.h"
#endif

#if defined(MC_use_Trajectory)
#include "Trajectory.h"
#include "Trajectory_hid.h"
#endif


typedef enum
{
    calibtype6_st_inited = 0,
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


typedef enum
{
    calibtype14_st_inited = 0,
    calibtype14_st_absEncoderCalib = 1,
    calibtype14_st_hardLimitSet = 2,
    calibtype14_st_finished = 3
} calibtype14_states;

typedef struct
{
    BOOL is_active;
    calibtype14_states state;
    CTRL_UNITS hardstopPos;
    int32_t computedZero;
    int32_t rotorposmin;
    int32_t rotorposmax;
} jointCalibType14Data;

typedef struct
{
    union
    {
        jointCalibType6Data  type6;
        jointCalibType7Data  type7;
        jointCalibType14Data type14;
    }data;
    eOmc_calibration_type_t type;
} jointCalibrationData;


typedef union
{
    struct
    {
        uint8_t torque_sensor_timeout:1;
        uint8_t hard_limit_reached:1;
        uint8_t velocity_ref_timeout:1;
        uint8_t pwm_ref_timeout:1;
        uint8_t current_ref_timeout:1;
        uint8_t torque_ref_timeout;
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

struct Joint_hid 
{
    uint8_t ID;
    
    PID minjerkPID;
    PID directPID;
    
    CTRL_UNITS dead_zone;
    
    // Kalman Filter Parameters
    BOOL kalman_filter_enabled;
    float x0[3];
    float Q[3];
    float R;
    float P0;
    
    CTRL_UNITS pos_min;
    CTRL_UNITS pos_max;
    CTRL_UNITS vel_max;
    CTRL_UNITS acc_max;

    BOOL use_hard_limit;
    
    BOOL belong2WristMK2;
    
    CTRL_UNITS pos_min_soft;
    CTRL_UNITS pos_max_soft;    
    CTRL_UNITS pos_min_hard;
    CTRL_UNITS pos_max_hard;
    
    CTRL_UNITS output_lim;
    
    CTRL_UNITS out_ref;
    CTRL_UNITS pos_ref;
    CTRL_UNITS vel_ref;
    CTRL_UNITS acc_ref;
    CTRL_UNITS trq_ref;
    
    CTRL_UNITS pos_fbk;
    CTRL_UNITS vel_fbk;
    CTRL_UNITS acc_fbk;
    CTRL_UNITS trq_fbk;
    
    CTRL_UNITS vel_fbk_old;
    
    CTRL_UNITS pos_fbk_from_motors;
    CTRL_UNITS vel_fbk_from_motors;
    
    CTRL_UNITS pos_err;
    CTRL_UNITS vel_err;
    CTRL_UNITS trq_err;    
    
    CTRL_UNITS output;
        
    CTRL_UNITS tcKoffset;
    CTRL_UNITS tcKstiff;
    CTRL_UNITS tcKdamp;
    
    CTRL_UNITS Kadmitt;
    
    WatchDog trq_fbk_wdog;
    WatchDog trq_ref_wdog;
    WatchDog vel_ref_wdog;
    WatchDog pwm_ref_wdog;
    WatchDog cur_ref_wdog;
    
#if defined(MC_use_embot_app_mc_Trajectory)    
    embot::app::mc::Trajectory *traj {nullptr};
#endif    

#if defined(MC_use_Trajectory)
    Trajectory trajectory;
#endif

    
    eOmc_controlmode_t     control_mode;
    eOmc_interactionmode_t interaction_mode;
    
    int8_t pushing_limit;
    
    BOOL pos_control_active;
    BOOL trq_control_active;
    
    BOOL not_reversible;
    CableConstraintData cable_constr;
    CableCalib          cable_calib;  
    
    JointFaultState fault_state_prec;
    JointFaultState fault_state;
    uint16_t diagnostics_refresh;
    eOmc_joint_t * eo_joint_ptr; // pointer to network variable of ethernet protocol.
    
    jointCalibrationData running_calibration;
    
    // Kalman Filter
    kalmanfilter::KalmanFilter kalman_filter {};
    
#ifdef FINGER_MK3
    CTRL_UNITS ZTau;
    CTRL_UNITS Ke;
#endif 
};

 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
