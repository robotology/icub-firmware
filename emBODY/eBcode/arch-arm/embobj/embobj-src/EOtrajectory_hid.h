
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTRAJECTORY_HID_H_
#define _EOTRAJECTORY_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

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

    float pi; /**< trajectory actual point    */
    float pf; /**< trajectory end point       */
    float vi; /**< trajectory actual velocity */

    uint32_t steps_to_end; /**< steps required to complete the trajectory */
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




