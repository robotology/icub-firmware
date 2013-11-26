
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
#include "EOspeedmeter.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOaxisController.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

// required

#define MASK_MIN_POS           0x01
#define MASK_MAX_POS           0x02
#define MASK_VEL_TOUT          0x04
#define MASK_MAX_VEL           0x08

#define MASK_POS_PID           0x10
#define MASK_TRQ_PID           0x20

#define MASK_HAS_LIMITS        0x03

#define MASK_POS_READY         0x13
#define MASK_TRQ_READY         0x23
 
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
    EOpid *pidT;

    EOtrajectory *trajectory;

    uint8_t axisID;
    
    int32_t position;
    int32_t velocity;
    
    int32_t pos_min;
    int32_t pos_max;
    
    int32_t err;
    
    // measures (read only)
    int32_t torque_meas;
    uint8_t torque_wdog;

    int32_t velocity_timer;

    // torque control
    int32_t torque_ref;
    int32_t torque_out;
    int32_t torque_off;

    // parameters
    int32_t stiffness;
    int32_t damping;

    int16_t openloop_out;

    eOmc_controlmode_t control_mode;

    uint8_t limits_mask;
    
    eObool_t calibrated;
    
    filter_cut_freq_t filter_cut_freq;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




