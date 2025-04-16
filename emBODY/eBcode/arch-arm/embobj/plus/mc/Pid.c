/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or  FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License foFITNESSr more details
*/

#if !defined(__cplusplus)
    #error this Pid.c file must be compiled in C++
#endif

// - API
#include "Pid.h"  


// - HIDDEN DATA
#include "PID_hid.h"

// - dependencies    
//#include "EOtheErrorManager.h"
//#include "EoError.h"
//    
//static void send_debug_message(const char *message, uint8_t jid, uint16_t par16, uint64_t par64)
//{
//    eOerrmanDescriptor_t errdes = {0};
//    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
//    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//    errdes.sourceaddress    = jid;
//    errdes.par16            = par16;
//    errdes.par64            = par64;
//    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, message, NULL, &errdes);
//    //eo_errman_Info(eo_errman_GetHandle(), message, "", &errdes);
//}


PID* PID_new(uint8_t n)
{
    PID *o = NEW(PID, n);

    for (uint8_t i=0; i<n; ++i)
    {
        PID_init(&(o[i]));
    }
    
    return o;
}

void PID_init(PID* o)
{
    memset(o, 0, sizeof(PID));
}

void PID_config(PID* o, eOmc_PID_t* config)
{
    float rescaler = 1.0f/(float)(1<<config->scale);
    
    o->Ko = config->offset;
    o->Kp = rescaler*config->kp;
    o->Kd = rescaler*config->kd;
    o->Ki = rescaler*config->ki;
    
    o->Kff = rescaler*config->kff;
    
    o->Kbemf = 0.0f;
    o->Ktau  = 0.0f;
    
    o->Dn = 0.0f;
    o->En = 0.0f;
    o->In = 0.0f;
    o->Imax = config->limitonintegral;
    
    o->stiction_up   = rescaler*config->stiction_up_val;
    o->stiction_down = rescaler*config->stiction_down_val;

#ifdef FINGER_MK3  
    o->out_max_close = config->limitonoutput;
    o->out_max_open = 0.2f*o->out_max_close;
#else
    o->out_max = config->limitonoutput;
#endif

    o->out_lpf = 0.0f;
    o->out = 0.0f;
    
    o->filter = 0; 
    
    o->Ki *= 0.5f*CTRL_LOOP_PERIOD;
    o->Kd *= CTRL_LOOP_FREQUENCY;
    
    if (o->Kd != 0.0f && o->Kp != 0.0f)
    {
        static const float N=10.f;
        
        o->A = o->Kd / (o->Kd + o->Kp*N);
        o->B = (1.0f - o->A)*o->Kd;
    }
    else
    {
        o->A = o->B = 0.0f;
    }
    
}

void PID_config_friction(PID *o, float Kbemf, float Ktau, eOmc_FrictionParams_t friction)
{
    o->Kbemf = Kbemf;
    o->Ktau  = Ktau;
    // Printa ktau
    o->viscous_pos_val = friction.viscous_pos_val;
    o->viscous_neg_val = friction.viscous_neg_val;
    o->coulomb_pos_val = friction.coulomb_pos_val;
    o->coulomb_neg_val = friction.coulomb_neg_val;
    o->velocityThres_val = friction.velocityThres_val;
}

void PID_config_filter(PID *o, uint8_t filter)
{
    o->filter = filter;
}

void PID_reset(PID* o)
{
    o->Dn = 0.0f;
    o->En = 0.0f;
    o->In = 0.0f;
    
    o->out_lpf = 0.0f;
    o->out = 0.0f;
}

void PID_get_state(PID* o, float *out, float *err)
{
    *out = o->out_lpf;
    *err = o->En;
}

float PID_do_out(PID* o, float En)
{   
    // proportional
    float out = o->Kp*En;
    
    // integral
    o->In += o->Ki*(En + o->En);
    LIMIT(o->In, o->Imax);
    out += o->In;
    
    // derivative
    o->Dn *= o->A;
    o->Dn += o->B*(En - o->En);
    out += o->Dn;
    
    o->En = En;

    // offset
    out += o->Ko;
    
#ifdef FINGER_MK3
    if (o->Kp > ZERO)
    {
        if (out > ZERO)
        {
            LIMIT(out, o->out_max_close);
        }
        else
        {
            LIMIT(out, o->out_max_open);
        }
    }
    else
    {
        if (out < ZERO)
        {
            LIMIT(out, o->out_max_close);
        }
        else
        {
            LIMIT(out, o->out_max_open);
        }
    }
#else
    LIMIT(out, o->out_max);
#endif
    
    switch (o->filter)
    {
    case 0: o->out_lpf = out;                                                      break;
    case 1: o->out_lpf = 0.9813258905f*o->out_lpf + 0.00933705470f*(o->out + out); break; // 3.0 Hz
    case 2: o->out_lpf = 0.9931122710f*o->out_lpf + 0.00344386440f*(o->out + out); break; // 1.1 Hz
    case 3: o->out_lpf = 0.9949860427f*o->out_lpf + 0.00250697865f*(o->out + out); break; // 0.8 Hz
    case 4: o->out_lpf = 0.9968633318f*o->out_lpf + 0.00156833410f*(o->out + out); break; // 0.5 Hz
    default: o->out_lpf = out; break;
    }
    
    o->out = out;
    
    return o->out_lpf;
}

float PID_do_friction_comp(PID *o, float32_t vel_fbk, float32_t trq_ref)
{
    // A threshold is necessary to handle velocities close to zero and higher velocities.
    // When the velocity is close to zero the model used for friction compensation is cubic
    // (this avoids discontinuous control for low velocities).
    // In fact, when the velocity is close to zero, it can continuosly switch from positive to negative values due 
    // to the effects of noise. As a result, the friction torque also switches from negative values to 
    // positive values if a threshold is not used.
    
    const float32_t MICRO { 1000000.0 }; // conversion from Nm into microNm
    const float32_t coulomb_pos_converted = o->coulomb_pos_val * MICRO;
    const float32_t coulomb_neg_converted = o->coulomb_neg_val * MICRO;
    
    const float32_t viscous_neg = o->viscous_neg_val * vel_fbk;
    const float32_t viscous_pos = o->viscous_pos_val * vel_fbk;
    
    if (vel_fbk <= -o->velocityThres_val)
    {
        return o->Ktau*(-coulomb_neg_converted + viscous_neg + o->Kff*trq_ref);
    }
    else if (vel_fbk > o->velocityThres_val)
    {
        return o->Ktau*(coulomb_pos_converted + viscous_pos + o->Kff*trq_ref);
    }
    else
    {
        return o->Ktau*(o->Kff*trq_ref);
    }
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
