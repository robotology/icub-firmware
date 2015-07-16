
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOEMSCONTROLLER_H_
#define _EOEMSCONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EOemsController.h
    @brief      This header file implements public interface to joint-motor decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       18/04/2012
**/

/** @defgroup eo_emsController Object EOemsController
    Does something.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOmcController.h"
#include "EOdecoupler.h"
#include "EOaxisController.h"
#include "EoMotionControl.h"


// - public #define  --------------------------------------------------------------------------------------------------

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/* @typedef    typedef struct EOtrajectory_hid EOtrajectory
    @brief      EOtrajectory is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOemsController_hid EOemsController;



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/* @fn         extern EOtrajectory* eo_trajectory_New(void)
    @brief      Creates a new trajectory object 
    @return     The pointer to the required object.
 **/
extern EOemsController* eo_emsController_Init(eOemscontroller_board_t board, eOemscontroller_actuation_t act, uint8_t nax);
extern void eo_emsController_set_Jacobian(int32_t **Ji32);

/* @fn         extern void eo_trajectory_Set(EOtrajectory *o, float p0, float pf, float v0, float speed)
    @brief      Sets minimum jerk trajectory new start and end points, initial and average speed. When the time is 
                expired, the function always returns the trajectory end point. 
    @param      o      The pointer to the trajectory object.
    @param      p0     The start position of the trajectory.         
    @param      pf     The end position of the trajectory.
    @param      v0     The inital velocity.
    @param      speed  The average speed.
 **/

// encoders

extern void eo_emsController_SetAbsEncoderSign(uint8_t joint, int32_t sign);
extern void eo_emsController_AcquireAbsEncoders(int32_t *abs_enc_pos, uint8_t error_mask);
extern void eo_emsController_AcquireMotorEncoder(uint8_t motor, int16_t current, int32_t velocity, int32_t position);

extern void set_2FOC_idle(uint8_t joint);
extern void set_2FOC_running(uint8_t joint);

extern void eo_emsController_ReadTorque(uint8_t joint, eOmeas_torque_t trq_measure);
extern void eo_emsController_CheckFaults(void);
extern void eo_emsController_PWM(int16_t* pwm);

extern void eo_emsController_SetOutput(uint8_t joint, int16_t out);
extern void eo_emsController_SetPosRef(uint8_t joint, int32_t pos, int32_t avg_vel);
extern void eo_emsController_SetVelRef(uint8_t joint, int32_t vel, int32_t avg_acc);
extern void eo_emsController_SetPosRaw(uint8_t joint, int32_t pos);
extern void eo_emsController_SetTrqRef(uint8_t joint, int32_t trq);

// asynchronous

extern void eo_emsController_SetControlMode(uint8_t joint, eOmc_controlmode_command_t mode);
extern void eo_emsController_SetControlModeGroupJoints(uint8_t joint, eOmc_controlmode_command_t mode);
extern eOmc_controlmode_t eo_emsController_GetControlMode(uint8_t joint);

extern eObool_t eo_emsController_SetInteractionModeGroupJoints(uint8_t joint, eOmc_interactionmode_t mode);
extern eObool_t eo_emsController_SetInteractionMode(uint8_t joint, eOmc_interactionmode_t mode);
//extern eOmc_interactionmode_t eo_emsController_GetInteractionMode(uint8_t joint);

extern void eo_emsController_SetMotorConfig(uint8_t joint, eOmc_motor_config_t motorconfig);
extern void eo_emsController_SetRotorEncoder(uint8_t joint, int32_t rotorencoder);
extern void eo_emsController_ReadMotorstatus(uint8_t motor, uint8_t* state);
extern void eo_emsController_GetMotorStatus(uint8_t mId, eOmc_motor_status_t* motor_status);

extern void eo_emsController_CheckCalibrations(void);
extern void eo_emsController_StartCalibration_type3(uint8_t joint, int32_t pos, int32_t vel, int32_t offset);
extern void eo_emsController_StartCalibration_type0(uint8_t joint, int16_t pwmlimit, int16_t vel);

extern void eo_emsController_ResetPosPid(uint8_t joint);
extern void eo_emsController_ResetTrqPid(uint8_t joint);
extern void eo_emsController_Stop(uint8_t joint);

extern void eo_emsController_GetPWMOutput(uint8_t joint, int32_t* pwm);
extern void eo_emsController_GetActivePidStatus(uint8_t joint, eOmc_joint_status_ofpid_t* pidStatus);
extern void eo_emsController_GetJointStatus(uint8_t joint, eOmc_joint_status_t* jointStatus);
extern eObool_t eo_emsController_GetMotionDone(uint8_t joint);

// configuration
extern void eo_emsController_SetMotorParams(uint8_t joint, eOmc_motor_params_t params);
extern void eo_emsController_SetTcFilterType(uint8_t joint, uint8_t filterType);
extern void eo_emsController_SetPosPid(uint8_t joint, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff, float stiction_up, float stiction_down);
extern void eo_emsController_SetTrqPid(uint8_t joint, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff, float Kff, float stiction_up, float stiction_down);

extern void eo_emsController_SetImpedance(uint8_t joint, int32_t stiffness, int32_t damping, int32_t offset);
extern void eo_emsController_GetImpedance(uint8_t joint, int32_t *stiffness, int32_t *damping, int32_t *offset);

extern void eo_emsController_SetLimits(uint8_t joint, int32_t pos_min, int32_t pos_max, int32_t vel_max);
extern void eo_emsController_SetPosMin(uint8_t joint, int32_t pos_min);
extern void eo_emsController_SetPosMax(uint8_t joint, int32_t pos_max);
extern void eo_emsController_SetVelMax(uint8_t joint, int32_t vel_max);
extern void eo_emsController_SetVelTimeout(uint8_t joint, int32_t vel_timeout);
extern void eo_emsController_SetGearboxRatio(uint8_t joint, int32_t gearboxratio);
extern void eo_emsController_GetDecoupledMeasuredTorque (uint8_t joint, int32_t * Trq);
extern void eo_emsController_GetDecoupledReferenceTorque (uint8_t joint, int32_t * Trq);
extern void eo_emsMotorController_GoIdle(void);

/** @}            
    end of group eo_emsController  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

