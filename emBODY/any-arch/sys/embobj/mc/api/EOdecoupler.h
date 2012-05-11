
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

#define IS_COUPLED(decoupler,joint,motor) IS_COUPLED_HID(decoupler,joint,motor)

// - declaration of public user-defined types ------------------------------------------------------------------------- 

 

/* @typedef    typedef struct EOtrajectory_hid EOtrajectory
    @brief      EOtrajectory is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOdecoupler_hid EOdecoupler;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

extern float decoupler_shoulder_trq[4][4];
extern float decoupler_shoulder_pwm[4][4];
extern float decoupler_waist_pwm[4][4];

// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/* @fn         extern EOtrajectory* eo_trajectory_New(void)
    @brief      Creates a new trajectory object 
    @return     The pointer to the required object.
 **/
extern EOdecoupler* eo_decoupler_New(uint8_t dim, float matrix[4][4]);

/* @fn         extern float eo_trajectory_Step(EOtrajectory *o)
    @brief      Executes a trajectory step.
    @param      o  The pointer to the trajectory object.
    @return     The actual trajectory point value.
 **/
extern void eo_decoupler_Mult(EOdecoupler *o, float *src, float *dst);

extern uint8_t eo_decoupler_IsCoupled(EOdecoupler *o, uint8_t joint, uint8_t motor);

/** @}            
    end of group eo_decopler  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

