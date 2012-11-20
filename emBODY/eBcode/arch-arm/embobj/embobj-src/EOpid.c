
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

#include "EOspeedmeter.h"

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

#define SAFE_MAX_CURRENT    1000

#define ZERO_ROTATION_TORQUE 832 //680

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
        o->KKiIn = 0.0f;
        
        o->Imax = 0.0f;

        o->Yoff    = 0;
        o->pwm     = 0;
        o->pwm_max = 0;
        
        o->A = 0.0f;
        o->B = 0.0f;
        
        o->zero_rotation_torque_neg = ZERO_ROTATION_TORQUE;
        o->zero_rotation_torque_pos = ZERO_ROTATION_TORQUE;
        
        o->safe_mode = eobool_true;
    }

    return o;
}

extern void eo_pid_SafeMode(EOpid *o, eObool_t on_off)
{
    if (!o) return;
    
    o->safe_mode = on_off;
}

extern void eo_pid_SetPid(EOpid *o, float K, float Kd, float Ki, int32_t pwm_max, float Imax, int32_t Yoff)
{
    if (!o) return;
    
    o->K  = K;
    o->Kd = Kd*EMS_PERIOD;
    o->Ki = Ki;
	
    o->Yoff = Yoff;
    o->Imax = Imax;
    
    o->pwm_max = pwm_max;
    
    float N = 10.0;
    float T = o->Kd/N;
    float Q = 2.0f*T*EMS_FREQUENCY_FLOAT; 

    o->A = (Q-1.0f)/(Q+1.0f);
    o->B = EMS_FREQUENCY_FLOAT*(1.0f-o->A);
}

extern void eo_pid_SetZRT(EOpid *o, int16_t zrt_neg, int16_t zrt_pos)
{
    if (!o) return;
    
    o->zero_rotation_torque_neg = zrt_neg;
    o->zero_rotation_torque_pos = zrt_pos;
}

extern void eo_pid_GetStatus(EOpid *o, int32_t *pwm, int32_t *err)
{
    if (!o)
    {
        *pwm = *err = 0;
        
        return;
    }
    
    *pwm = o->pwm;
    *err = (int32_t)o->En;
}

extern void eo_pid_Reset(EOpid *o)
{
    if (!o) return;
	
    o->Dn   = 0.0f;
    o->En   = 0.0f;
    o->KKiIn  = 0.0f;
    
    o->pwm  = 0;
}

extern int16_t eo_pid_PWM(EOpid *o, float En, float Vref)
{
    if (!o) return 0;
                
    float Dn = o->A*o->Dn + o->B*(En - o->En);
    
    o->En = En;
    o->Dn = Dn;

    float Xn = o->K*En;
    
    o->KKiIn += o->Ki*Xn;

    LIMIT(o->KKiIn, o->Imax);

    int32_t pwm = o->Yoff + (int32_t)(Xn + o->K*Dn + o->KKiIn);
    
    if (Vref>0.0f)
    {
        pwm += o->zero_rotation_torque_pos;
    }
    else if (Vref<0.0f)
    {
        pwm += o->zero_rotation_torque_neg;
    }
    
    if (o->safe_mode)
    {
        LIMIT(pwm, SAFE_MAX_CURRENT);
    }
    else
    {
        LIMIT(pwm, o->pwm_max);
    }

    //#ifdef USE_2FOC_FAST_ENCODER
    return (int16_t)(o->pwm = pwm);
    //#else
    //return (int16_t)(o->pwm = (o->pwm + pwm + 1)/2);
    //#endif
}

extern int16_t eo_pid_PWM2(EOpid *o, float En, float Vref, float Venc)
{
    if (!o) return 0;
    
    o->En = En;
    
    float Xn = (Vref == 0.0f) ? (o->K*En) : (o->K*(En+o->Kd*(Vref-Venc))); 
    
    o->KKiIn += o->Ki*Xn;

    LIMIT(o->KKiIn, o->Imax);

    int32_t pwm = o->Yoff + (int32_t)(Xn + o->KKiIn);
    
    if (Vref>0.0f)
    {
        pwm += o->zero_rotation_torque_pos;
    }
    else if (Vref<0.0f)
    {
        pwm += o->zero_rotation_torque_neg; 
    }
    
    if (o->safe_mode)
    {
        LIMIT(pwm, SAFE_MAX_CURRENT);
    }
    else
    {
        LIMIT(pwm, o->pwm_max);
    }

    //#ifdef USE_2FOC_FAST_ENCODER
    return (int16_t)(o->pwm = pwm);
    //#else
    //return (int16_t)(o->pwm = (o->pwm + pwm + 1)/2);
    //#endif
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




