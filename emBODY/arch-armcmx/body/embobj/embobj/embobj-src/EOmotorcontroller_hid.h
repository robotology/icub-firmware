
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMOTORCONTROLLER_HID_H_
#define _EOMOTORCONTROLLER_HID_H_


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

    EOtrajectory *trajectory;

    float stiffness;
    
    float encpos_meas;
    float torque_meas;
    
    float pos_now;
    float vel_now;
    float vel_now_PERIOD;

    float vel_pos_ref;
    float vel_timer;
    float vel_timeout;

    float pos_ref;
    float speed;

    float vel_ref;
    float acc_ref;

    float pos_min;
    float pos_max;
    float vel_max;

    float torque_ref;
    
    uint8_t control_mode;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




