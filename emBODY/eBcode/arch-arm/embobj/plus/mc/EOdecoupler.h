
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EODECOUPLER_H_
#define _EODECOUPLER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOdecoupler.h
    @brief      This header file implements public interface to joint-motor decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       18/04/2012
**/

/** @defgroup eo_decoupler Object EOdecoupler
    Does something.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------

// - declaration of public user-defined types ------------------------------------------------------------------------- 

#define MOTOR_EXTERNAL_FAULT     0x0001
#define MOTOR_OVERCURRENT_FAULT  0x0002
#define MOTOR_I2T_LIMIT_FAULT    0x0004
#define MOTOR_HALLSENSORS_FAULT  0x0008
#define MOTOR_QENCODER_FAULT     0x0010
#define MOTOR_CAN_INVALID_PROT   0x0020
#define MOTOR_CAN_GENERIC_FAULT  0x0040
#define MOTOR_CAN_NOT_RESPONDING 0x0080

//#define AXIS_AEA_TOUT_FAULT     0x1000
//#define AXIS_AEA_HARD_FAULT     0x2000
//#define AXIS_TORQUE_SENS_FAULT  0x4000

#define MOTOR_HARDWARE_FAULT     0xFFFE

/* @typedef    typedef struct EOtrajectory_hid EOtrajectory
    @brief      EOtrajectory is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOmotors_hid EOmotors;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/* @fn         extern EOtrajectory* eo_trajectory_New(void)
    @brief      Creates a new trajectory object 
    @return     The pointer to the required object.
 **/
extern EOmotors* eo_motors_New(uint8_t nmotors);

extern void eo_motors_PWM(int32_t *pwm_joint, int16_t *pwm_motor, eObool_t *stiff);

extern eObool_t eo_motors_CableLimitAlarm(int32_t j0, int32_t j1, int32_t j2);

extern void eo_motor_set_motor_status(EOmotors *o, uint8_t m, uint8_t* state);
//extern uint8_t eo_motor_are_motors_in_fault(EOmotors *o, uint8_t mask);

extern uint16_t eo_get_motor_fault_mask(EOmotors *o, uint8_t m);
extern eObool_t eo_is_motor_hard_fault(EOmotors *o, uint8_t m);
extern eObool_t eo_is_motor_ext_fault(EOmotors *o, uint8_t m);

/** @}            
    end of group eo_decopler  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

