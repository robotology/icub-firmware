
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMOTORCONTROLLER_H_
#define _EOMOTORCONTROLLER_H_


/** @file       EOmotorcontroller.h
    @brief      This header file implements public interface to a motor controller.
    @author     alessandro.scalzo@iit.it
    @date       27/03/2012
**/

/** @defgroup eo_motorcontroller Object EOmotorcontroller
    Does something.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum { 
    CM_IDLE                 =0x00, 
    CM_POSITION             =0x01,
    CM_VELOCITY             =0x02, 
    CM_TORQUE               =0x03, 
    CM_IMPEDANCE_POS        =0x04, 
    CM_IMPEDANCE_VEL        =0x05,

    CM_CALIB_ABS_POS_SENS   =0x10,
    CM_CALIB_HARD_STOPS     =0x20,
    CM_HANDLE_HARD_STOPS    =0x30,
    CM_MARGIN_REACHED       =0x40,
    CM_CALIB_ABS_AND_INC    =0x41,
    CM_OPENLOOP             =0x50
} control_mode_t;

/** @typedef    typedef struct EOmotorcontroller_hid EOmotorcontroller
    @brief      EOtrajectory is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOmotorcontroller_hid EOmotorcontroller;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOmotorcontroller* eo_motorcontroller_New(void)
    @brief      Creates a new motor controller object 
    @return     The pointer to the required object.
 **/
extern EOmotorcontroller* eo_motorcontroller_New(void);

extern uint8_t eo_motorcontroller_SetControlMode(EOmotorcontroller *o, control_mode_t cm);

extern void eo_motorcontroller_ReadEncPos(EOmotorcontroller *o, float encpos_meas);
extern void eo_motorcontroller_ReadTorque(EOmotorcontroller *o, float torque);

extern float eo_motorcontroller_PWM(EOmotorcontroller *o);

extern void eo_motorcontroller_SetStiffness(EOmotorcontroller *o, float stiffness);

extern void eo_motorcontroller_SetPosRef(EOmotorcontroller *o, float pos_ref, float speed);
extern void eo_motorcontroller_SetVelRef(EOmotorcontroller *o, float vel_ref, float acc_ref);
extern void eo_motorcontroller_SetVelMax(EOmotorcontroller *o, float vel_max);

/** @}            
    end of group eo_motorcontroller  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

