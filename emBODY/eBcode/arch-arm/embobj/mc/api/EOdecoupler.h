
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

 typedef enum {
    EMS_NULL     = 0,
    EMS_GENERIC  = 1,
    EMS_WAIST    = 2,
    EMS_SHOULDER = 3,
    EMS_ANKLE    = 4,
    EMS_UPPERLEG = 5
} emsBoardType_t;

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

extern uint8_t eo_motors_PWM(EOmotors *o, emsBoardType_t board_type, int32_t *pwm_joint, int16_t *pwm_motor, uint8_t* alarm_mask);

extern eObool_t eo_motors_CableLimitAlarm(int32_t j0, int32_t j1, int32_t j2);

//extern eObool_t eo_motors_is_motorON(EOmotors *o, uint8_t m);
extern void eo_motor_set_motor_error_status(EOmotors *o, uint8_t m, eObool_t on_off);


/** @}            
    end of group eo_decopler  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

