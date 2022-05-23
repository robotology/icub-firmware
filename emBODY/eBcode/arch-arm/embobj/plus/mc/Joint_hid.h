#ifndef MC_JOINT_HID___
#define MC_JOINT_HID___

#include "kalman_filter.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#include "Joint.h"

struct Joint_hid // Joint
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
    WatchDog vel_ref_wdog;
    
    Trajectory trajectory;
    
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

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
