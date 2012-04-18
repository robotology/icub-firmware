
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTRAJECTORY_HID_H_
#define _EOTRAJECTORY_HID_H_


/* @file        EOtrajectory_hid.h
    @brief      This header file implements hidden interface to motor minumum jerk trajectory generation.
    @author     alessandro.scalzo@iit.it
    @date       23/03/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtrajectory.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOtrajectory_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOtrajectory_hid 
{
    float Ai; /**< fast incremental algorithm register */
    float Bi; /**< fast incremental algorithm register */
    float Ci; /**< fast incremental algorithm register */
    float Kc; /**< fast incremental algorithm register */
    float Zi; /**< fast incremental algorithm register */
    float Yi; /**< fast incremental algorithm register */
    float Ky; /**< fast incremental algorithm register */
    float Fi; /**< fast incremental algorithm register */
    float Kf; /**< fast incremental algorithm register */
    float pi; /**< trajectory actual point */
    float pf; /**< trajectory end point */

    float delta; /**< last step value */

    uint32_t steps_to_end; /**< steps required to complete the trajectory */
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

inline float eo_trajectory_Step(EOtrajectory *o)
{
    if (!o->steps_to_end) return o->pf;
        
    --(o->steps_to_end);
    
    o->Ai += o->Bi; 
    o->Bi += o->Ci;
    o->Ci += o->Kc;
    
    o->Zi += o->Yi; 
    o->Yi += o->Ky;

    o->Fi += o->Kf;
    
    float pi = o->Fi + o->Ai*o->Zi;

    o->delta = pi - o->pi;

    o->pi = pi;

    return o->pi;
}

inline float eo_trajectory_StepDelta(EOtrajectory *o)
{
    if (!o->steps_to_end) return o->pf;
        
    --(o->steps_to_end);
    
    o->Ai += o->Bi; 
    o->Bi += o->Ci;
    o->Ci += o->Kc;
    
    o->Zi += o->Yi; 
    o->Yi += o->Ky;

    o->Fi += o->Kf;
    
    float pi = o->Fi + o->Ai*o->Zi;

    o->delta = pi - o->pi;

    o->pi = pi;

    return o->delta;
}

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




