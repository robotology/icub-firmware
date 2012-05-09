
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMOTORCONTROLLER_HID_H_
#define _EOMOTORCONTROLLER_HID_H_

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
#include "EOpid.h"
#include "EOtrajectory.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOmotorcontroller.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOmotorcontroller_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOmotorcontroller_hid 
{
    EOpid *pidP;
    EOpid *pidT;
    EOpid *pidI;

    EOtrajectory *trajectory;

    // axis limits
    float pos_min;
    float pos_max;
    float vel_max;
    
    // measures (read only)
    float encpos_meas;
    float torque_meas;

    // velocity control
    float pos_vel_bias;
    float vel_ref;
    float acc_ref_step;
    float vel_timer;
    float vel_timeout;

    float pos_out;
    float vel_out;
    float vel_out_step;

    // torque control
    float torque_ref;
    float torque_out;
    float torque_off;
    float torque_last_pos_err;
    float torque_damp_lp_filt;

    // parameters
    float stiffness;
    float damping;

    uint8_t control_mode;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




