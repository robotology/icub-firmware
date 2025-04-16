
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef MC_TRAJECTORY_HID_H__
#define MC_TRAJECTORY_HID_H__

#include "EoCommon.h"
#include "EOemsControllerCfg.h"

// - OPAQUE STRUCT
struct Trajectory_hid 
{
    float pos_min;
    float pos_max;
    
    float vel_max;
    float acc_max;
    
    //////////////////
    // position
    float target_pos;
    float xX;
    float xV;
    float xA;
    
    float xT;
    float xTimer;
    
    //////////////////
    // velocity
    BOOL bVelocityMove;
    float target_vel;
    
    float vX;
    float vV;
    float vA;
    
    //float vVf;
   
    float vT;
    float vTimer;
    
    float xInvT;
    float vInvT;
    
    float xK0,xK1,xK2,xK3;
    float vK0,vK1,vK2;
}; 


 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

