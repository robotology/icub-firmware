
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
#include "EOspeedcurve.h"
#include "EOspeedmeter.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOaxisController.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#define eo_axisController_ReadTorque_hid(axis_controller, torque) axis_controller->torque_meas = torque

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOmotorcontroller_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOaxisController_hid 
{
    EOpid *pidP;
    EOpid *pidC;
    EOpid *pidT;

    EOspeedmeter *speedmeter;
    EOtrajectory *trajectory;

    eObool_t is_calibrated;
    int32_t calib_timeout_ms;
    float   calib_max_error;

    // axis limits
    int32_t pos_min;
    int32_t pos_max;
    int32_t vel_max;
    
    // measures (read only)
    int32_t torque_meas;

    int32_t vel_timer;
    int32_t vel_timeout;    

    // torque control
    int32_t torque_ref;
    int32_t torque_out;
    int32_t torque_off;
    int32_t torque_last_pos_err;
    int32_t torque_damp_lp_filt;

    // parameters
    int32_t stiffness;
    int32_t damping;

    int16_t pwm_offset;

    uint8_t control_mode;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




