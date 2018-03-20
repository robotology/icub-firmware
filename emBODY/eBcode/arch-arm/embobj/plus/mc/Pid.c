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

#include <math.h>
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
    
    o->Ko__final = config->offset;
    o->Kp__final = rescaler*config->kp;
    o->Kd__final = rescaler*config->kd;
    o->Ki__final = rescaler*config->ki;
    o->Kff_final = rescaler*config->kff;
    
    o->Ki__final *= 0.5f*CTRL_LOOP_PERIOD;
    o->Kd__final *= CTRL_LOOP_FREQUENCY;
    
    o->slope_time_ms = config->slope_time_ms;
    
    if (o->slope_time_ms < 1) o->slope_time_ms = 1;
    
    float step = 1.0f/(float)o->slope_time_ms;
    
    o->Ko__delta = step*(o->Ko__final - o->Ko );
    o->Kp__delta = step*(o->Kp__final - o->Kp );
    o->Kd__delta = step*(o->Kd__final - o->Kd );
    o->Ki__delta = step*(o->Ki__final - o->Ki );
    o->Kff_delta = step*(o->Kff_final - o->Kff);
    
    o->Imax = config->limitonintegral;
    
    o->stiction_up   = rescaler*config->stiction_up_val;
    o->stiction_down = rescaler*config->stiction_down_val;
  
    o->out_max = config->limitonoutput;
}

#if 0
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
  
    o->out_max = config->limitonoutput;
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
#endif

void PID_config_friction(PID *o, float Kbemf, float Ktau)
{
    o->Kbemf = Kbemf;
    o->Ktau  = Ktau;
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
    if (o->slope_time_ms)
    {
        if (o->slope_time_ms == 1)
        {
            o->Kp  = o->Kp__final;
            o->Ki  = o->Ki__final;
            o->Kd  = o->Kd__final;
            o->Ko  = o->Ko__final;
            o->Kff = o->Kff_final;
        }
        else
        {
            o->Kp  += o->Kp__delta;
            o->Ki  += o->Ki__delta;
            o->Kd  += o->Kd__delta;
            o->Ko  += o->Ko__delta;
            o->Kff += o->Kff_delta;
        }
        
        --o->slope_time_ms;
        
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
    
    LIMIT(out, o->out_max);
    
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
    return o->Ktau*(o->Kbemf*vel_fbk+o->Kff*trq_ref);
}
