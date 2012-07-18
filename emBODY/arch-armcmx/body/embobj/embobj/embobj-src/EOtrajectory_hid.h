
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
    float Pos;
    float Vel;
    float PAcc;
    float P2Jerk;
    float P3Snap;
    float P4Crackle;

    float biPAcc;
    float biP2Jerk;

    float PosF;
    float VelF;

    int32_t pos_steps_to_end;
    int32_t vel_steps_to_end;

    float pos_min;
    float pos_max;
    float vel_max;

    int32_t acc_stop_hybrid;

    eObool_t hybrid;

    float hybridVel;
    float hybridPAcc;    
    float hybridP2Jerk;
    float hybridP3Snap;

    int32_t hybrid_steps_to_end;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




