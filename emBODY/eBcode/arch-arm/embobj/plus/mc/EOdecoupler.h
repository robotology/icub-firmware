
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
#include "EOmcController.h"
    
#include "EOemsControllerCfg.h"
#include "EOemsController.h" 

// - public #define  --------------------------------------------------------------------------------------------------

// - declaration of public user-defined types ------------------------------------------------------------------------- 

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
extern EOmotors* eo_motors_New(uint8_t nmotors, eOemscontroller_board_t board);
extern EOmotors* eo_motors_GetHandle(void);
extern uint32_t eo_motors_getQEError(EOmotors *o, uint8_t motor);
extern void eo_motor_get_motor_status(EOmotors *o, uint8_t m, uint8_t *state, uint8_t *state_req);
extern void eo_motors_new_state_req(EOmotors *o, uint8_t motor, uint8_t control_mode);
extern eObool_t eo_motor_check_state_req(EOmotors *o, uint8_t m);
extern void eo_motors_reset_wdog(EOmotors *o, uint8_t motor);
extern void eo_motors_rearm_wdog(EOmotors *o, uint8_t motor);
extern void eo_motors_check_wdog(EOmotors *o);
extern eObool_t eo_get_motor_timeout(EOmotors *o, uint8_t m);
extern void eo_motors_set_Jacobian(EOmotors *o, int32_t **Ji32);
extern eObool_t eo_motors_are_coupled(EOmotors *o, uint8_t ma, uint8_t mb);
    
extern void eo_motors_decouple_PWM(EOmotors *o, float *pwm_joint, float *pwm_motor, eObool_t* stiff);
extern eObool_t eo_motors_CableLimitAlarm(int32_t j0, int32_t j1, int32_t j2);

extern void eo_motor_set_motor_status(EOmotors *o, uint8_t m, uint8_t* state);
//extern uint8_t eo_motor_are_motors_in_fault(EOmotors *o, uint8_t mask);

extern uint32_t eo_get_motor_fault_mask(EOmotors *o, uint8_t m);
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

