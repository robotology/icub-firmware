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

#include "Pid.h"

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
    o->viscous_pos_val = friction.viscous_pos_val;
    o->viscous_neg_val = friction.viscous_neg_val;
    o->coulomb_pos_val = friction.coulomb_pos_val;
    o->coulomb_neg_val = friction.coulomb_neg_val;
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

float PID_do_friction_comp(PID *o, float vel_fbk, float trq_ref)
{
    // TODO: return value using the new friction parameters (do we need to put it under macro?)
    //return o->Ktau*(o->coulomb_pos_val + o->viscous_pos_val*vel_fbk + o->Kff*trq_ref);
    return o->Ktau*(o->Kbemf*vel_fbk+o->Kff*trq_ref);
}
