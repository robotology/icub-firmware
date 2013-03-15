
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
    float pos_min;
    float pos_max;
    
    int32_t vel_max;
    int32_t acc_max;

    //////////////////
    // position
    float xPos;
    float xVel;
    float xPAcc;

    float xPosF;
    float xVelF;
    
    int32_t PosTimer;
    
    //////////////////
    // velocity
    eObool_t bVelocityMove;
    eObool_t bIsBraking;
    
    float    vPos;
    float    vVel;
    float    vPAcc;
    
    float    vVelF;
    
    int32_t  VelTimer;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




