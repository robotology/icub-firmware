
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

#include "icub/api/EoMotionControl.h"

#include "EOpid.h"
#include "EOaxisController_hid.h"

// - public #define  --------------------------------------------------------------------------------------------------

#define eo_axisController_GetPosPidPtr(o) eo_axisController_GetPosPidPtr_hid(o)
#define eo_axisController_GetTrqPidPtr(o) eo_axisController_GetTrqPidPtr_hid(o)

#define eo_axisController_IsCalibrated(o) eo_axisController_IsCalibrated_hid(o)

#define eo_axisController_ReadEncPos(axis_controller, encpos) eo_axisController_ReadEncPos_hid(axis_controller, encpos)
#define eo_axisController_ReadTorque(axis_controller, torque) eo_axisController_ReadTorque_hid(axis_controller, torque)

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum { 
    CM_IDLE                 =0x00,   //
    CM_POSITION             =0x01,   //
    CM_VELOCITY             =0x02,   //
    CM_TORQUE               =0x03,   //
    CM_IMPEDANCE_POS        =0x04,   //
    CM_IMPEDANCE_VEL        =0x05,   //

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
extern void eo_axisController_Configure(EOaxisController *o, eOmc_joint_config_t *c);
extern eObool_t eo_axisController_Calibrate(EOaxisController *o, eOmc_calibrator_t *c);

extern void eo_axisController_SetPosRef(EOaxisController *o, float pos, float vel);
extern void eo_axisController_SetVelRef(EOaxisController *o, float vel, float acc);
extern void eo_axisController_SetTrqRef(EOaxisController *o, float trq);

extern eObool_t eo_axisController_SetControlMode(EOaxisController *o, control_mode_t cm);

extern void eo_axisController_GetActivePidStatus(EOaxisController *o, float *pwm, float *err);

extern float eo_axisController_PWM(EOaxisController *o);

//extern EOpid* eo_axisController_GetPosPidPtr(EOaxisController *o);
//extern EOpid* eo_axisController_GetTrqPidPtr(EOaxisController *o);
//extern void eo_axisController_ReadEncPos(EOaxisController *o, float encpos);
//extern void eo_axisController_ReadTorque(EOaxisController *o, float torque);

/** @}            
    end of group eo_axisController  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

