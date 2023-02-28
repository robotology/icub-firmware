#ifndef MC_MOTOR_HID___
#define MC_MOTOR_HID___

#if defined(ERGOJOINT)
#include "TorqueModel.h"
#else
#ifdef __cplusplus
extern "C" {
#endif
#endif    
    
#include "Motor.h"

#if defined(ERGOJOINT)
struct TrqCtrlParams_t
{
    CTRL_UNITS Ko;  // Static friction coefficient  [A]
    CTRL_UNITS Kw;  // Viscous friction coefficient [A/(rad/s)]
    CTRL_UNITS Ifriction_max; // Limits the two contributions above [A] 
    
    CTRL_UNITS Kw2; // Quadratic term for non linear characteristic [A/(rad/s)^2]
                    // It can also be negative to prevent runaway.
    CTRL_UNITS Iquadratic_max; // Limits the quadratic contribution above [A]
    
    CTRL_UNITS Kff; // Feed forward term, equal to 1/(torque constant * gearbox ratio * efficiency) [A/Nm]
    CTRL_UNITS Iff_max; // Limits the feed forward contribution above [A]
    
    CTRL_UNITS Kp; // Proportional term [A/Nm]
    CTRL_UNITS Iproportional_max; // Limits the proportional contribution above [A]
    
    CTRL_UNITS Iout_max; // Limits the final output [A] 
};
#endif

struct Motor_hid
{
    // consts
    uint8_t ID;
    uint8_t actuatorPort;
#if !defined(EOTHESERVICES_customize_handV3_7joints)
    // UNKNOWN              0
    // HARDWARE_2FOC        1
    // HARDWARE_MC4p        2
    uint8_t HARDWARE_TYPE;
#else    
    eObrd_canlocation_t canloc; // it keeps the can location of the motor 
    
    // HARDWARE_UNKNOWN     0
    // HARDWARE_2FOC        1
    // HARDWARE_MC4p        2
    // HARDWARE_PMC         3
    uint8_t HARDWARE_TYPE; // it can have the above values
#endif


    CTRL_UNITS GEARBOX;

    BOOL HAS_TEMP_SENSOR;
    int16_t temperature_max;
    int16_t temperature_fbk;

    int32_t pwm_max;
    int32_t pwm_fbk;
    int32_t pwm_ref;
    
    int32_t calib_pwm;
    
    int32_t pos_raw_cal_fbk;
    int32_t pos_raw_fbk;
    int32_t vel_raw_fbk;
    
    int32_t pos_calib_offset;
    
    int32_t pos_min;
    int32_t pos_max;
    int32_t pos_ref;
    int32_t pos_fbk;
    int32_t pos_err;
    
    int32_t pos_fbk_old;
    int32_t enc_sign;
    float32_t enc_tolerance;
    
    int32_t vel_max;
    int32_t vel_ref;
    int32_t vel_fbk;
    int32_t vel_err;
    
    int32_t Iqq_max;
    int32_t Iqq_ref;
    int32_t Iqq_fbk;
    int32_t Iqq_ovl;
    int32_t Iqq_err;
    
    CTRL_UNITS trq_max;
    CTRL_UNITS trq_ref;
    CTRL_UNITS trq_fbk;
    CTRL_UNITS trq_err;
    
    int32_t output;
    
    PID trqPID;
    
    BOOL not_init;
    BOOL not_calibrated;
    BOOL hardware_fault;
    
    BOOL can_dead;
    BOOL wrong_ctrl_mode;
    
    uint16_t diagnostics_refresh;
    uint16_t diagnostics_refresh_warning;
    MotorFaultState fault_state_prec;
    MotorFaultState fault_state;
    QEState qe_state;
    
    icubCanProto_controlmode_t  control_mode;
    icubCanProto_controlmode_t  control_mode_req;
    WatchDog control_mode_req_wdog;

    HardStopCalibData hardstop_calibdata;

    uint16_t qencoder_err_counter;

    // 2FOC specific data
    WatchDog can_2FOC_alive_wdog;
    uint8_t can_motor_config[7];
    //BOOL outOfLimitsSignaled;
    
#if defined(ERGOJOINT)
    TorqueModel torque_model;
    int32_t joint_position_raw;

    int32_t joint_full_res;
    int16_t torsion;
    int16_t torsion_raw;
    int16_t torsion_offset;
    int16_t torsion_fine_tuning[256];
    
    int32_t joint_encoder_last;
    int32_t joint_encoder_sure;
    
    int16_t offmax;
    int16_t offmin;
        
    int16_t angmax;
    int16_t angmin;
    int16_t angle;
    
    TrqCtrlParams_t trq_ctrl;
#endif 
};

#if !defined(ERGOJOINT)
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
#endif

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
