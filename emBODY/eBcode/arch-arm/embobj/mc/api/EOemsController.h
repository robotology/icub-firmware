
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
 
extern EOemsController* eo_emsController_Init(uint8_t nmotors, emsBoardType_t board_type);

// synchronous
extern void eo_emsController_ReadEncoder(uint8_t joint, int32_t pos);
extern void eo_emsController_ReadTorque(uint8_t joint, int32_t torque);
extern void eo_emsController_ReadStatus(uint8_t joint, int32_t pos, int32_t torque);

extern void eo_emsController_SetPosRef(uint8_t joint, float pos, float vel);
extern void eo_emsController_SetVelRef(uint8_t joint, float vel, float acc);
extern void eo_emsController_SetTrqRef(uint8_t joint, float trq);

extern float* eo_emsController_PWM(void);

// asynchronous
extern void eo_emsController_SetControlMode(uint8_t joint, control_mode_t mode);
extern void eo_emsController_ResetPosPid(uint8_t joint);
extern void eo_emsController_ResetTrqPid(uint8_t joint);

extern void eo_emsGetActivePidStatus(uint8_t joint, float *pwm, float *err);

// configuration
extern void eo_emsController_SetDecoupler(emsMotorDecoupler_t dec_type, float matrix[4][4]);

/** @}            
    end of group eo_emsController  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

