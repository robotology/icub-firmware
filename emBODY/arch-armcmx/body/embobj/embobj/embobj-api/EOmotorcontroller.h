
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

typedef enum { CM_UNINIT=0, CM_POSITION=0, CM_TORQUE=1, CM_VELOCITY=2, CM_IMPEDANCE=3 } control_mode_t;

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

extern void eo_motorcontroller_ReadEncoder(EOmotorcontroller *o, float position);

extern void eo_motorcontroller_ReadTorque(EOmotorcontroller *o, float torque);

extern float eo_motorcontroller_Step(EOmotorcontroller *o, float En);

extern void eo_motorcontroller_SetStiffness(EOmotorcontroller *o, float stiffness);

extern void eo_motorcontroller_SetReference(EOmotorcontroller *o, float ref, float speed);

/** @}            
    end of group eo_motorcontroller  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

