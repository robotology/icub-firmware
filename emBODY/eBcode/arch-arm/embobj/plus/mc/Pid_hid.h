
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef MC_PID_HID_H___
#define MC_PID_HID_H___


#include "EoCommon.h"

struct PID_hid 
{
    float Ko;
    float Kp;
    float Kd;
    float Ki;
    
    float Kff;
    float Kbemf;
    float Ktau;
    
    float Dn;
    float En;
    float In;
    float Imax;
    
    float A,B;
    
    float stiction_up;
    float stiction_down;

    float viscous_pos_val;
    float viscous_neg_val;
    float coulomb_pos_val;
    float coulomb_neg_val;
    float velocityThres_val;

#ifdef FINGER_MK3
    float out_max_open;
    float out_max_close;
#else
    float out_max;
#endif

    float out_lpf;
    float out;
    
    uint8_t filter;
}; 

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

