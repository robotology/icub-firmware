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

#ifdef __cplusplus
extern "C" {
#endif

#include "EoCommon.h"
#include "EoMotionControl.h"
#include "EOtheCANprotocol.h"
#include "EOemsControllerCfg.h"

#include "WatchDog.h"
#include "Pid.h"

typedef struct //CanOdometry2FocMsg
{
    int16_t current;
    int16_t velocity;
    int32_t position;
} CanOdometry2FocMsg; 

typedef union
{
    struct
    {
        //B0 L
        unsigned ExternalFaultAsserted  :1;
        unsigned UnderVoltageFailure    :1;      
        unsigned OverVoltageFailure     :1;
        unsigned OverCurrentFailure     :1;
        //B0 H
        unsigned DHESInvalidValue       :1;
        unsigned AS5045CSumError        :1;
        unsigned DHESInvalidSequence    :1;
        unsigned CANInvalidProtocol     :1;
        //B1 L
        unsigned CAN_BufferOverRun      :1;
        unsigned SetpointExpired        :1;
        unsigned CAN_TXIsPasv           :1;
        unsigned CAN_RXIsPasv           :1;
        //B1 H
        unsigned CAN_IsWarnTX           :1;
        unsigned CAN_IsWarnRX           :1;
        unsigned unused                 :1;
        unsigned OverHeating            :1;
        //B2 L
        unsigned ADCCalFailure          :1; 
        unsigned I2TFailure             :1;                     
        unsigned EMUROMFault            :1;
        unsigned EMUROMCRCFault         :1;
        //B2 H
        unsigned EncoderFault           :1;
        unsigned FirmwareSPITimingError :1;		
        unsigned AS5045CalcError        :1;
        unsigned FirmwarePWMFatalError  :1;
        //B3 L
        unsigned CAN_TXWasPasv          :1;
        unsigned CAN_RXWasPasv          :1;
        unsigned CAN_RTRFlagActive      :1;
        unsigned CAN_WasWarn            :1;
        //B3 H
        unsigned CAN_DLCError           :1;
        unsigned SiliconRevisionFault   :1;
        unsigned PositionLimitUpper     :1; 
        unsigned PositionLimitLower     :1; 
    } bits;
        
    uint32_t bitmask;
        
} MotorFaultState;

typedef union
{
    struct
    {
        unsigned dirty           :1;
        unsigned stuck           :1;
        unsigned index_broken    :1;
        unsigned phase_broken    :1;
        
        unsigned not_calibrated  :1;
        unsigned unused          :3;
    } bits;
    uint8_t bitmask;
} QEState;

typedef struct //State2FocMsg
{
    uint8_t control_mode;
    
    //QEState qe_state;
    uint8_t qe_state;
    
    uint16_t pwm_fbk;
    
    //FaultState fault_state;
    uint32_t fault_state;
    
} State2FocMsg;

typedef struct //HardStopCalibData
{
    union
    {
        struct
        {
            unsigned iscalibrating   :1;
            unsigned hwlimitreached  :1;
            unsigned unused          :6;
        } bits;
        uint8_t bitmask;
    }u;
    uint8_t posStable_counter;
    uint16_t waitcalib_counter;
    int32_t limited_pwm;
    int32_t last_pos;
    int32_t zero;
    
} HardStopCalibData;

typedef struct //Motor
{
    // consts
    uint8_t ID;
    uint8_t actuatorPort;

    // UNKNOWN              0
    // HARDWARE_2FOC        1
    // HARDWARE_MC4p        2
    uint8_t HARDWARE_TYPE;

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

    // 2FOC specific data
    WatchDog can_2FOC_alive_wdog;
    uint8_t can_motor_config[7];
    //BOOL outOfLimitsSignaled;

} Motor;

extern Motor* Motor_new(uint8_t n);  //
extern void Motor_init(Motor* o);  //
extern void Motor_config(Motor* o, uint8_t ID, eOmc_motor_config_t* config); //
extern void Motor_destroy(Motor* o); //

extern void Motor_config_encoder(Motor* o, int32_t resolution);
extern void Motor_config_max_currents(Motor* o, eOmc_current_limits_params_t* current_params);

extern void Motor_config_current_PID(Motor* o, eOmc_PID_t* pid);
extern void Motor_config_torque_PID(Motor* o, eOmc_PID_t* pid);
extern void Motor_config_speed_PID(Motor* o, eOmc_PID_t* pid);

extern void Motor_config_filter(Motor* o, uint8_t filter); //
extern void Motor_config_friction(Motor* o, float Bemf, float Ktau); //
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

extern CTRL_UNITS Motor_do_trq_control(Motor* o, CTRL_UNITS trq_ref, CTRL_UNITS trq_fbk); //
extern void Motor_update_state_fbk(Motor* o, void* state_msg); //
extern void Motor_update_odometry_fbk_can(Motor* o, CanOdometry2FocMsg* data); //
extern void Motor_do_calibration_hard_stop(Motor* o); //

extern void Motor_actuate(Motor* o, uint8_t N); //

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

extern void Motor_set_i2t_fault(Motor* o);

extern void Motor_raise_fault_i2t(Motor* o);
//extern void Motor_raise_fault_external(Motor* o);
extern BOOL Motor_is_external_fault(Motor* o);
extern BOOL Motor_is_in_fault(Motor* o);
extern BOOL Motor_is_running(Motor* o);

//BOOL Motor_clear_ext_fault(Motor *o);

////////////////////////////////////////////////////////////////////////////
extern void Motor_config_gearbox_M2J(Motor* o, float32_t gearbox_M2J);
extern int16_t Motor_config_pwm_limit(Motor* o, int16_t pwm_limit);
////////////////////////////////////////////////////////////////////////////
/*
extern void Motor_update_temperature_fbk(Motor* o, int16_t temperature_fbk);
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


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard



// Motor
/////////////////////////////////////////////////////////
