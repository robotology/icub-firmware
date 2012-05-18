
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


// - public #define  --------------------------------------------------------------------------------------------------

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum {
    DECOUPLER_POS = 0,
    DECOUPLER_TRQ = 1,
    DECOUPLER_PWM = 2
} emsMotorDecoupler_t;

typedef enum {
    EMS_GENERIC  = 0,
    EMS_WAIST    = 1,
    EMS_SHOULDER = 2
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
extern EOemsController* eo_emsController_Init(uint8_t nmotors, emsBoardType_t board_type);


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
extern void eo_emsController_ReadEncoders(int32_t *pos);
extern void eo_emsController_ReadTorques(int32_t *torque);

extern void eo_emsController_SetPosRef(uint8_t joint, float pos, float vel, uint8_t reset);
extern void eo_emsController_SetPosRefSet(float *pos, float *vel);
extern void eo_emsController_SetVelRef(uint8_t joint, float vel, float acc);
extern void eo_emsController_SetVelRefSet(float *vel, float *acc);
extern void eo_emsController_SetTrqRef(uint8_t joint, float trq);
extern void eo_emsController_SetTrqRefSet(float *trq);

extern float* eo_emsController_PWM(void);

// asynchronous
extern void eo_emsController_SetControlMode(uint8_t joint, control_mode_t mode);
extern void eo_emsController_ResetPosPid(uint8_t joint);
extern void eo_emsController_ResetTrqPid(uint8_t joint);

extern void eo_emsGetActivePidStatus(uint8_t joint, float *pwm, float *err);

// configuration
extern void eo_emsController_SetDecoupler(emsMotorDecoupler_t dec_type, float matrix[4][4]);
extern void eo_emsController_SetPosPid(uint8_t joint, float kp, float ki, float kd, float Ymax, float Imax);
extern void eo_emsController_SetTrqPid(uint8_t joint, float kp, float ki, float kd, float Ymax, float Imax);
extern void eo_emsController_SetStiffness(uint8_t joint, float stiffeness);
extern void eo_emsController_SetVelMax(uint8_t joint, float vel_max);
extern void eo_emsController_SetPosLimits(uint8_t joint, float pos_min, float pos_max);

/** @}            
    end of group eo_emsController  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

