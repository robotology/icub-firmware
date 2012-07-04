
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOaxisController_H_
#define _EOaxisController_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOaxisController.h
    @brief      This header file implements public interface to a motor controller.
    @author     alessandro.scalzo@iit.it
    @date       27/03/2012
**/

/** @defgroup eo_axisController Object EOaxisController
    Does something.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOpid.h"
#include "EOaxisController_hid.h"

// - public #define  --------------------------------------------------------------------------------------------------

#define eo_axisController_ReadTorque(axis_controller, torque) eo_axisController_ReadTorque_hid(axis_controller, torque)

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum { 
    CM_IDLE                 =0x00,   //
    CM_POSITION             =0x01,   //
    CM_POS_VEL              =0x02,   //
    CM_TORQUE               =0x03,   //
    CM_IMPEDANCE_POS        =0x04,   //
    CM_IMPEDANCE_VEL        =0x05,   //
    CM_VELOCITY             =0x06,

    CM_CALIB_ABS_POS_SENS   =0x10,   //

    CM_CALIB_HARD_STOPS     =0x20,
    CM_HANDLE_HARD_STOPS    =0x30,
    CM_MARGIN_REACHED       =0x40,   // ?
    CM_CALIB_ABS_AND_INC    =0x41,   //

    CM_OPENLOOP             =0x50    //
} control_mode_t;

/** @typedef    typedef struct EOaxisController_hid EOaxisController
    @brief      EOtrajectory is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOaxisController_hid EOaxisController;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

extern float decoupler_shoulder_pos[4][4];
extern float decoupler_shoulder_trq[4][4];
extern float decoupler_shoulder_pwm[4][4];

extern float decoupler_waist_pwm[4][4];


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOaxisController* eo_axisController_New(void)
    @brief      Creates a new motor controller object 
    @return     The pointer to the required object.
 **/
extern EOaxisController* eo_axisController_New(void);

extern void eo_axisController_ReadEncPos(EOaxisController *o, int32_t pos); 
extern void eo_axisController_SkipEncPos(EOaxisController *o);
extern int32_t eo_axisController_GetVelocity(EOaxisController *o);

extern void eo_axisController_SetPosRef(EOaxisController *o, int32_t pos, int32_t avg_vel);
extern void eo_axisController_SetVelRef(EOaxisController *o, int32_t vel, int32_t avg_acc);
extern void eo_axisController_SetTrqRef(EOaxisController *o, int32_t trq);

extern int16_t eo_axisController_PWM(EOaxisController *o);

extern void eo_axisController_Stop(EOaxisController *o, int32_t stop_acc);

extern eObool_t eo_axisController_SetControlMode(EOaxisController *o, control_mode_t cm);

extern void eo_axisController_SetStiffness(EOaxisController *o, int32_t stiffness);

extern void eo_axisController_SetVelTimeout(EOaxisController *o, int32_t vel_timeout);
extern void eo_axisController_SetLimits(EOaxisController *o, int32_t pos_min, int32_t pos_max, int32_t vel_max);

extern void eo_axisController_SetOffset(EOaxisController *o, int16_t offset);

extern void eo_axisController_GetActivePidStatus(EOaxisController *o, int16_t *pwm, int32_t *err);

extern EOpid* eo_axisController_GetPosPidPtr(EOaxisController *o);
extern EOpid* eo_axisController_GetVelPidPtr(EOaxisController *o);
extern EOpid* eo_axisController_GetTrqPidPtr(EOaxisController *o);

extern void eo_axisController_StartCalibration(EOaxisController *o, int32_t pos, int32_t offset, int32_t timeout_ms, int32_t max_error);

/** @}            
    end of group eo_axisController  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

