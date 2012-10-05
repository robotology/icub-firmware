
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
#include "EOdecoupler.h"
#include "EOaxisController.h"
#include "EoMotionControl.h"


// - public #define  --------------------------------------------------------------------------------------------------

#define ENC_INVALID 0xFFFFFFFF

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum {
    DECOUPLER_POS = 0,
    DECOUPLER_TRQ = 1,
    DECOUPLER_PWM = 2
} emsMotorDecoupler_t;

typedef enum {
    EMS_NULL     = 0,
    EMS_GENERIC  = 1,
    EMS_WAIST    = 2,
    EMS_SHOULDER = 3
} emsBoardType_t;

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
extern EOemsController* eo_emsController_Init(emsBoardType_t board_type);
extern eObool_t eo_emsController_AddAxis(uint8_t naxis);


/* @fn         extern void eo_trajectory_Set(EOtrajectory *o, float p0, float pf, float v0, float speed)
    @brief      Sets minimum jerk trajectory new start and end points, initial and average speed. When the time is 
                expired, the function always returns the trajectory end point. 
    @param      o      The pointer to the trajectory object.
    @param      p0     The start position of the trajectory.         
    @param      pf     The end position of the trajectory.
    @param      v0     The inital velocity.
    @param      speed  The average speed.
 **/

// synchronous
extern void eo_emsController_SkipEncoders(void);
extern void eo_emsController_ReadEncoders(int32_t *pos);
extern void eo_emsController_ReadSpeed(uint8_t axis, int32_t speed);
extern void eo_emsController_ReadTorques(int32_t *torque);

//extern float eo_emsController_GetVelocity(uint8_t joint);

extern void eo_emsController_SetOutput(uint8_t joint, int16_t out);
extern void eo_emsController_SetPosRef(uint8_t joint, int32_t pos, int32_t avg_vel);
extern void eo_emsController_SetVelRef(uint8_t joint, int32_t vel, int32_t avg_acc);
extern void eo_emsController_SetTrqRef(uint8_t joint, int32_t trq);

extern /*int16_t*/ void eo_emsController_PWM(int16_t* pwm);

// asynchronous
extern void eo_emsController_StartCalibration(uint8_t joint, int32_t pos, int32_t vel, int32_t offset);
extern void eo_emsController_SetControlMode(uint8_t joint, control_mode_t mode);
extern void eo_emsController_ResetPosPid(uint8_t joint);
extern void eo_emsController_ResetTrqPid(uint8_t joint);
extern void eo_emsController_Stop(uint8_t joint);
//extern void eo_emsGetActivePidStatus(uint8_t joint, int16_t *pwm, int32_t *err);
extern void eo_emsController_GetActivePidStatus(uint8_t joint, eOmc_joint_status_ofpid_t* pidStatus);
extern void eo_emsController_GetJointStatus(uint8_t joint, eOmc_joint_status_basic_t* jointStatus);
extern eObool_t eo_emsController_GetMotionDone(uint8_t joint);

// configuration
extern void eo_emsController_SetPosPid(uint8_t joint, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff);
extern void eo_emsController_SetTrqPid(uint8_t joint, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff);


extern void eo_emsController_SetStiffness(uint8_t joint, int32_t stiffeness);

extern void eo_emsController_SetLimits(uint8_t joint, int32_t pos_min, int32_t pos_max, int32_t vel_max);
extern void eo_emsController_SetPosMin(uint8_t joint, int32_t pos_min);
extern void eo_emsController_SetPosMax(uint8_t joint, int32_t pos_max);
extern void eo_emsController_SetVelMax(uint8_t joint, int32_t vel_max);

extern void eo_emsController_SetVelTimeout(uint8_t joint, int32_t vel_timeout);

extern void eo_emsController_SetDecoupler(emsMotorDecoupler_t dec_type, float matrix[4][4]);

/** @}            
    end of group eo_emsController  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

