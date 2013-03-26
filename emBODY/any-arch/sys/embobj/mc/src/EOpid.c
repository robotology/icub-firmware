
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

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOpid";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOpid* eo_pid_New(void) 
{
    EOpid *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOpid), 1);

    if (o)
    {
        o->K  = 0.f;
        o->Ki = 0.f;
        o->Kd = 0.f;
        
        o->Kff = 0.f;
        
        o->xv0 = o->xv1 = o->xv2 = 0.0f;
        o->yv0 = o->yv1 = o->yv2 = 0.0f;
       
        o->Dn    = 0.f;
        o->En    = 0.f;        
        o->KKiIn = 0.f;        
        o->Imax  = 0.f;
     
        o->pwm_offset = 0;
        o->pwm_max = 0;
        o->pwm = 0;
        
        o->A = 0.f;
        o->B = 0.f;
    }

    return o;
}

extern void eo_pid_SetPid(EOpid *o, float K, float Kd, float Ki, float Imax, int32_t pwm_max, int32_t pwm_offset)
{
    if (!o) return;
    
    o->K  = K;
    o->Kd = Kd*EMS_PERIOD;
    o->Ki = Ki;
    
    o->Kff = (K>0.0f) ? 0.16f : -0.16f;
	
    o->Imax = Imax;
    
    o->pwm_max = pwm_max;
    o->pwm_offset = pwm_offset;
    
    float N = 10.0;
    float T = o->Kd/N;
    float Q = 2.0f*T*EMS_FREQUENCY_FLOAT; 

    o->A = (Q-1.0f)/(Q+1.0f);
    o->B = EMS_FREQUENCY_FLOAT*(1.0f-o->A);
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
	
    o->Dn   = 0.f;
    o->En   = 0.f;
    o->KKiIn  = 0.f;
    
    o->xv0 = o->xv1 = o->xv2 = 0.0f;
    o->yv0 = o->yv1 = o->yv2 = 0.0f;
    
    o->pwm = 0;
}

extern int32_t eo_pid_PWM_pid(EOpid *o, float En)
{
    if (!o) return 0;
                
    o->Dn = o->A*o->Dn + o->B*(En - o->En);
    
    o->En = En;

    En *= o->K;
    
    o->KKiIn += o->Ki*En;

    LIMIT(o->KKiIn, o->Imax);
    
    o->pwm = o->pwm_offset + (int32_t)(En + o->K*o->Dn + o->KKiIn);
    
    LIMIT(o->pwm, o->pwm_max);

    return o->pwm;
}

extern int32_t eo_pid_PWM_pi(EOpid *o, float Tr, float Tm)
{
    if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->En = En;

    En *= o->K;
    
    //o->KKiIn += o->Ki*En;
    //LIMIT(o->KKiIn, o->Imax); 
    //En += o->KKiIn;
    
    o->pwm = o->pwm_offset + (int32_t)(En + o->Kff*Tr);
    
    LIMIT(o->pwm, o->pwm_max);

    return o->pwm;
}


extern int32_t eo_pid_PWM_pi_1_1Hz_2ndLPF(EOpid *o, float Tr, float Tm)
{
    if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->En = En;
   
    En *= o->K;
    
    //o->KKiIn += o->Ki*En;
    //LIMIT(o->KKiIn, o->Imax);
    //En += o->KKiIn;
    
    o->xv0 = o->xv1; o->xv1 = o->xv2; 
    //o->xv2 = 9.8259168159e-6f*En; // 1 Hz
    //o->xv2 = 1.4136793977e-5f*En; // 1.2 Hz
    o->xv2 = 1.1884094643e-5f*En; // 1.1 Hz
    
    o->yv0 = o->yv1; o->yv1 = o->yv2;
    //o->yv2 = o->xv0 +2.0f*o->xv1 +o->xv2 -0.9911535959f*o->yv0 +1.9911142922f*o->yv1; // 1 Hz
    //o->yv2 = o->xv0 +2.0f*o->xv1 +o->xv2 -0.9893937283f*o->yv0 +1.9893371812f*o->yv1; // 1.2 Hz
    o->yv2 = o->xv0 +2.0f*o->xv1 +o->xv2 -0.9902732712f*o->yv0 +1.9902257348f*o->yv1; // 1.1 Hz
    
    o->pwm = o->pwm_offset + (int32_t)(o->yv2 + o->Kff*Tr);
    
    LIMIT(o->pwm, o->pwm_max);

    return o->pwm;
}

extern int32_t eo_pid_PWM_pi_3_0Hz_2ndLPF(EOpid *o, float Tr, float Tm)
{
    if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->En = En;
   
    En *= o->K;
    
    //o->KKiIn += o->Ki*En;
    //LIMIT(o->KKiIn, o->Imax);
    //En += o->KKiIn;
    
    o->xv0 = o->xv1; o->xv1 = o->xv2; 
    o->xv2 = 8.7655548748e-5f*En; // 3.0 Hz
    
    o->yv0 = o->yv1; o->yv1 = o->yv2;
    o->yv2 = o->xv0 +2.0f*o->xv1 +o->xv2 -0.9736948720f*o->yv0 +1.9733442498f*o->yv1; // 3.0 Hz
    
    o->pwm = o->pwm_offset + (int32_t)(o->yv2 + o->Kff*Tr);
    
    LIMIT(o->pwm, o->pwm_max);

    return o->pwm;
}

extern int32_t eo_pid_PWM_pi_1_1Hz_1stLPF(EOpid *o, float Tr, float Tm)
{
    if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->En = En;
   
    En *= o->K;
    
    //o->KKiIn += o->Ki*En;
    //LIMIT(o->KKiIn, o->Imax);
    //En += o->KKiIn;
    
    o->xv0 = o->xv1; 
    o->xv1 = 0.0034438644f*En; // 3.0 Hz
    
    o->yv0 = o->yv1;
    o->yv1 = o->xv0+o->xv1+0.9931122710f*o->yv0; // 3.0 Hz
    
    o->pwm = o->pwm_offset + (int32_t)(o->yv1 + o->Kff*Tr);
    
    LIMIT(o->pwm, o->pwm_max);

    return o->pwm;
}

extern int32_t eo_pid_PWM_pi_3_0Hz_1stLPF(EOpid *o, float Tr, float Tm)
{
    if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->En = En;
   
    En *= o->K;
    
    //o->KKiIn += o->Ki*En;
    //LIMIT(o->KKiIn, o->Imax);
    //En += o->KKiIn;
    
    o->xv0 = o->xv1; 
    o->xv1 = 0.0093370547f*En; // 3.0 Hz
    
    o->yv0 = o->yv1;
    o->yv1 = o->xv0+o->xv1+0.9813258905f*o->yv0; // 3.0 Hz
    
    o->pwm = o->pwm_offset + (int32_t)(o->yv1 + o->Kff*Tr);
    
    LIMIT(o->pwm, o->pwm_max);

    return o->pwm;
}

extern int32_t eo_pid_PWM_piv(EOpid *o, float En, float Venc, float Vref, float Aref)
{
    if (!o) return 0;
    
    o->En = En;
    
    En = o->K*(En+o->Kd*(Vref-Venc));
    
    //o->KKiIn += o->Ki*En;
    //LIMIT(o->KKiIn, o->Imax);
    //En += o->KKiIn;
    
    o->pwm = o->pwm_offset + (int32_t)En;
        
    LIMIT(o->pwm, o->pwm_max);
    
    return o->pwm;
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




