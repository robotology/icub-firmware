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

	  /* All the input values Ktau, Kbemf, stiction_up, stiction_down
	   * are positive values.
     * Kc input value is considered the value identified for positive
	   * joint velocities. It is a priori positive, but could be negative	
	   * depending on Stribeck effect and the fitting of the friction
	   * training data used in the identification.
     * The final Kc and stiction parameters set in the model get the same
	   * sign as the joint velocity.
     * The final Ktau set in the model gets the same	sign as the GEARBOX
	   * ratio. */
	  
	  // PID parameters
    o->Kp = fabsf(rescaler*config->kp);
    o->Kd = fabsf(rescaler*config->kd);
    o->Ki = fabsf(rescaler*config->ki);

    o->Dn = 0.0f;
    o->En = 0.0f;
    o->In = 0.0f;
    o->Imax = config->limitonintegral;

    // Feed-forward parameters
		o->Kff = rescaler*config->kff;
    o->Kdith = 0.0f; // Dithering scale is off for now, until the proper interface is implemented on yarpmotorgui.
    o->Kc = fabsf(rescaler*config->offset);
    o->Kbemf = 0.0f;
    o->Ktau  = 0.0f;

	  /* Stiction: input values are absolute. The model parameter then gets the same
	   * sign as the joint velocity. */
    o->stiction_up   = fabsf(rescaler*config->stiction_up_val);
    o->stiction_down = fabsf(rescaler*config->stiction_down_val);
		
    o->ditheringVal = o->Kdith*EO_MAX(o->stiction_up,o->stiction_down);
	  o->ditheringMotorVel = (float)DITHERING_MOTOR_VEL_DFLT*(float)TICKS_PER_REVOLUTION/360;
	  o->stictionJointVel  = (float)STICTION_JOINT_VEL_DFLT;

    // Further PID configuration
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

void PID_config_friction(PID *o, float Kbemf, float Ktau)
{
    o->Kbemf = fabsf(Kbemf);
    o->Ktau  = fabsf(Ktau);
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

float PID_do_friction_comp(PID *o, float vel_raw_fbk, float vel_fbk, float vel_fbk_from_joint_enc, float trq_ref, float trq_fbk)
{
    static float signDithering = 1.0f;
	  volatile float dither,stiction,coulViscFriction,signVel,velIsPos,velIsNeg,torqIsPos,torqIsNeg;
	  
	  // Valid velocity for stiction and viscous friction compensation
    if (fabsf(vel_fbk_from_joint_enc/100)<=o->stictionJointVel)
		{
			vel_fbk_from_joint_enc = 0.0f;
			vel_fbk = 0.0f;
    }
		
	  // vel_fbk>0 --> signVel=1;  vel_fbk<0 --> signVel=-1; vel_fbk==0 --> signVel=0;
	  velIsNeg = (float)(vel_fbk_from_joint_enc<0);
	  velIsPos = (float)(vel_fbk_from_joint_enc>0);
	  signVel = velIsPos-velIsNeg;
	  
	  torqIsNeg = (float)(trq_ref<0);
	  torqIsPos = (float)(trq_ref>0);
		
		// Stiction
		stiction = (velIsPos+!signVel*torqIsPos)*o->stiction_up - (velIsNeg+!signVel*torqIsNeg)*o->stiction_down;
		
		// Coulomb + Viscous friction
	  coulViscFriction = o->Kc*signVel + o->Kbemf*vel_fbk;
		
		// Dithering
    if (fabsf(vel_raw_fbk)<=o->ditheringMotorVel)
    {
			// unobservable joint velocity (Apply Dithering)
      dither = signDithering*(o->ditheringVal);
      signDithering *= -1;
    }
		else
		{
			dither = 0.0f;
		}
		
    float totalFriction = dither + ((fabsf(stiction) > fabsf(coulViscFriction) ? stiction : coulViscFriction));
    return o->Ktau*(o->Kff*trq_ref + totalFriction);
}
