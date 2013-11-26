
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EODECOUPLER_HID_H_
#define _EODECOUPLER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  @file       EOdecoupler_hid.h
    @brief      This header file implements hidden interface to motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

//#include "EOmotorControl_hid.h"
    
// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOdecoupler.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/*  @struct     EOdecoupler_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
#define MAX_MOTORS 4
 
struct EOmotors_hid 
{
    uint8_t n_motors;
    
    uint8_t motor_error_mask;
    int16_t zero_rotation_torque[MAX_MOTORS];
}; 

// - declaration of extern hidden functions ---------------------------------------------------------------------------

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




