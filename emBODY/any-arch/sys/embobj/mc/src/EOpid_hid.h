
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOPID_HID_H_
#define _EOPID_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  @file       EOpid_hid.h
    @brief      This header file implements hidden interface to PID controller.
    @author     alessandro.scalzo@iit.it
    @date       23/03/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOpid.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------



// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOpid_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOpid_hid 
{
    int32_t Kp;
    int32_t Ki;
    int32_t Kd;

    int32_t En;
    int32_t KiIn;
    int32_t Dn;

    int32_t Ymax;
    int32_t Imax;
    int32_t Imax_shift;

    int32_t pwm;
    int32_t shift_fact_by_2;

    eObool_t initialized;

    uint8_t shift;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




