
/* @file       EOpid.c
   @brief      This file implements internal implementation of a PID controller.
   @author     alessandro.scalzo@iit.it
   @date       27/03/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOVtheSystem.h"

extern const float   EMS_PERIOD;
extern const float   EMS_FREQUENCY_FLOAT;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOpid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOpid_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define LIMIT(x,L) if (x>(L)) x=(L); else if (x<-(L)) x=-(L)

#define SAFE_MAX_CURRENT 900.0f

#define ZERO_ROTATION_TORQUE      680.0f
#define ZERO_ROTATION_TORQUE_SLOW 720.0f

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOpid";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOpid* eo_pid_New(void) 
{
    EOpid *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOpid), 1);

    if (o)
    {
        o->K  = 0.0f;
        o->Ki = 0.0f;
        o->Kd = 0.0f;

        o->Dn  = 0.0f;
        o->En  = 0.0f;        
        o->KIn = 0.0f;
        o->pwm = 0.0f;
        
        o->Yoff  = 0.0f;
        o->Ymax  = 0.0f;
        o->Imax  = 0.0f;
        o->KImax = 0.0f;

        o->safe_mode = eobool_false;
        
        o->A = 0.0f;
        o->B = 0.0f;
    }

    return o;
}

extern void eo_pid_SafeMode(EOpid *o, eObool_t on_off)
{
    if (!o) return;
    
    o->safe_mode = on_off;
}

extern void eo_pid_SetPid(EOpid *o, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff)
{
    if (!o) return;
    
    o->K  = K;
    o->Kd = Kd*EMS_PERIOD;
    o->Ki = Ki;
	
    o->Ymax = Ymax;
    o->Imax = Imax;
    o->Yoff = Yoff;

    o->KImax = o->Ki!=0.0f ? o->Imax/o->Ki : 0.0f;
    
    //float N = 10.0;
    //float T = o->Kd/N;
    //float Q = 2.0f*T*EMS_FREQUENCY_FLOAT; 

    o->A = 39.0/41.0;
    //o->A = (Q-1.0f)/(Q+1.0f);
    o->B = EMS_FREQUENCY_FLOAT*(1.0f-o->A);
}

extern void eo_pid_GetStatus(EOpid *o, int32_t *pwm, int32_t *err)
{
    if (!o)
    {
        *pwm = *err = 0;
        
        return;
    }
    
    *pwm = (int32_t)o->pwm;
    *err = (int32_t)o->En;
}

extern void eo_pid_Reset(EOpid *o)
{
    if (!o) return;
	
    o->Dn   = 0.0f;
    o->En   = 0.0f;
    o->pwm  = 0.0f;
    o->KIn  = 0.0f;
}

extern int16_t eo_pid_PWM(EOpid *o, float En, float Vref)
{
    if (!o) return 0;
                
    //float Dn = o->A*o->Dn + o->B*(En - o->En); 
    
    o->En = En;
    //o->Dn = Dn;
    
    //float Xn = o->K*(En + Dn);

    float Xn = o->K*En;
    
    o->KIn += Xn;

    LIMIT(o->KIn, o->KImax);

    o->pwm = Xn + o->Ki*o->KIn;
    
    if (Vref>0.0f)
    {
        o->pwm += ZERO_ROTATION_TORQUE;
    }
    else if (Vref<0.0f)
    {
        o->pwm -= ZERO_ROTATION_TORQUE; 
    }
    
    if (o->safe_mode)
    {
        LIMIT(o->pwm, SAFE_MAX_CURRENT);
    }
    else
    {
        LIMIT(o->pwm, o->Ymax);
    }

    return (int16_t)o->pwm;
}

extern int16_t eo_pid_PWM2(EOpid *o, float En, float Vref, float Venc)
{
    if (!o) return 0;
    
    float Xn = o->K*(En+o->Kd*(Vref-Venc));
    
    o->KIn += Xn;

    LIMIT(o->KIn, o->KImax);

    o->pwm = Xn + o->Ki*o->KIn + o->Yoff;
    
    if (Vref>0.0f)
    {
        o->pwm += ZERO_ROTATION_TORQUE;
    }
    else if (Vref<0.0f)
    {
        o->pwm -= ZERO_ROTATION_TORQUE; 
    }
    
    if (o->safe_mode)
    {
        LIMIT(o->pwm, SAFE_MAX_CURRENT);
    }
    else
    {
        LIMIT(o->pwm, o->Ymax);
    }

    o->En = En;

    return (int16_t)o->pwm;
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




