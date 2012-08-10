
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOEMSCONTROLLER_HID_H_
#define _EOEMSCONTROLLER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  @file       EOemsController_hid.h
    @brief      This header file implements hidden interface to motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOdecoupler.h"
#include "EOaxisController.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#define MAX_MOTORS 4

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/*  @struct     EOemsController_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOemsController_hid 
{
    EOdecoupler *decoupler[3];

    EOaxisController *axis_controller[MAX_MOTORS];

    float encoder_pos[MAX_MOTORS];
    float torque_meas[MAX_MOTORS];

    float pos_ref[MAX_MOTORS];
    float vel_ref[MAX_MOTORS];
    float acc_ref[MAX_MOTORS];
    float trq_ref[MAX_MOTORS];
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




