
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EODECOUPLER_HID_H_
#define _EODECOUPLER_HID_H_


/*  @file       EOdecoupler_hid.h
    @brief      This header file implements hidden interface to motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOdecoupler.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#define DEC_MAX_SIZE 4

#define IS_COUPLED_HID(decoupler,joint,motor) (decoupler->M[joint][motor] != 0.0f)

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/*  @struct     EOdecoupler_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOdecoupler_hid 
{
    float M[4][4];
    uint8_t n;
}; 

// - declaration of extern hidden functions ---------------------------------------------------------------------------


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




