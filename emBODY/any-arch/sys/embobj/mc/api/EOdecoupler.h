
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

extern void eo_motors_PWMs_Shoulder(EOmotors *o, int32_t *pwm_joint, int32_t *vel_joint, int16_t *pwm_motor);
extern void eo_motors_PWMs_UpperLeg(EOmotors *o, int32_t *pwm_joint, int32_t *vel_joint, int16_t *pwm_motor);


/** @}            
    end of group eo_decopler  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

