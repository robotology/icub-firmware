
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

#include "EOemsControllerCfg.h"

#include "EOspeedmeter.h"

#include "EoError.h"
#include "EOtheErrorManager.h"
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
        //o->moving = eobool_false;
        
        o->Kp = 0.f;
        o->Kd = 0.f;
        o->Ki = 0.f;
        
        o->Kff   = 0.f;
        o->Kbemf = 0.f;
        o->Ktau = 0.f;
        
        o->stiction_up_val = 0.0f;
        o->stiction_down_val = 0.0f;
      
        o->xv0 = o->xv1 = 0.0f;
        o->yv0 = o->yv1 = 0.0f;
       
        o->Dn = 0.f;
        o->En = 0.f;        
        o->In = 0.f;          
        o->Imax = 0.f;
     
        o->pwm_offset = 0;
        o->pwm_max = 0;
        o->pwm = 0;
        
        o->A = 0.f;
        o->B = 0.f;
    }

    return o;
}

extern void eo_pid_SetPidBase(EOpid *o, float Kp, float Kd, float Ki, float Imax, float pwm_max, float pwm_offset, float stiction_up, float stiction_down)
{
    static const float N=10.f;

    o->pwm_max = pwm_max;
    o->pwm_offset = pwm_offset;
    
    o->Imax = Imax;
    
    o->Dn = 0.f;
    o->In = 0.f;
    o->En = 0.f;
    
    o->stiction_down_val = stiction_up;
    o->stiction_up_val = stiction_down;
  
    o->Kp = Kp;
    o->Kd = Kd;
    //o->Ki = Ki;
    o->Ki = 0.5f*Ki*EMS_PERIOD;
    
    //o->A = 0.9f;
    //o->B = (1.f - o->A)*Kd;
    if (Kd != 0.f && Kp != 0.f)
    {
    o->A = Kd / (Kd + Kp*N*EMS_PERIOD);
    o->B = (1.f - o->A)*Kd*EMS_FREQUENCY_FLOAT;
}
    else
    {
        o->A = o->B = 0.f;
    }
}

extern void eo_pid_SetPidTorq(EOpid *o, float Kp, float Kd, float Ki, float Imax, float pwm_max, float pwm_offset, float Kff, float stiction_up, float stiction_down)
{    
    eo_pid_SetPidBase(o, Kp, Kd, Ki, Imax, pwm_max, pwm_offset, stiction_up, stiction_down);
   
    o->Kff = Kff;
}

extern void eo_pid_SetPidBemf(EOpid *o, float Kbemf)
{
    o->Kbemf = Kbemf;
}

extern void eo_pid_SetPidKtau(EOpid *o, float Ktau)
{
    o->Ktau = Ktau;
}

extern float eo_pid_GetPwmMax(EOpid *o)
{
    return o->pwm_max;
}

extern float eo_pid_PWM_p(EOpid *o, float En)
{    
    o->pwm = o->Kp*En;
    
    LIMIT(o->pwm, o->pwm_max);

#ifdef FORCE_ZERO_PWM_OUT
    return 0;
#else
    return o->pwm;
#endif
}

extern float eo_pid_PWM_pid(EOpid *o, float En)
{    
    //if (!o) return 0;
    
    o->Dn *= o->A;
    o->Dn += o->B*(En - o->En);
    
    //o->In += o->Ki*En;
    o->In += o->Ki*(En + o->En);
    LIMIT(o->In, o->Imax);
    
    o->En = En;
    
    o->pwm = o->pwm_offset + o->Kp*En + o->In + o->Dn;
    LIMIT(o->pwm, o->pwm_max);

#ifdef FORCE_ZERO_PWM_OUT
    return 0;
#else
    return o->pwm;
#endif
}

/*
extern float eo_pid_PWM_pid(EOpid *o, float En)
{     
    o->pwm += (o->Kp*(En-o->En) + o->Ki*(En+o->En));
    
    o->En = En;
    
    LIMIT(o->pwm, o->pwm_max);

#ifdef FORCE_ZERO_PWM_OUT
    return 0;
#else
    return o->pwm;
#endif
}
*/

extern float eo_pid_PWM_piv(EOpid *o, float En, float Vn)
{
    //if (!o) return 0;
       
    //o->In += o->Ki*En;
    o->In += o->Ki*(En + o->En);
    LIMIT(o->In, o->Imax);
    
    o->En = En;
    
    o->pwm = o->pwm_offset + o->Kp*En + o->In + o->Kd*Vn;
    LIMIT(o->pwm, o->pwm_max);
    
#ifdef FORCE_ZERO_PWM_OUT
    return 0;
#else
    return o->pwm;
#endif
}

extern void eo_pid_Reset(EOpid *o)
{
    //if (!o) return;
	
    //o->moving = eobool_false;
    
    o->Dn = 0.f;
    o->En = 0.f;
    o->In = 0.f;
    
    o->xv0 = o->xv1 = 0.0f;
    o->yv0 = o->yv1 = 0.0f;
}

extern void eo_pid_GetStatus(EOpid *o, float *pwm, float *err)
{
    if (!o)
    {
        *pwm = *err = 0;
        
        return;
    }
    
    *pwm = o->pwm;
    *err = o->En;
}

extern void eo_pid_GetStatusInt32 (EOpid *o, int32_t *pwm, int32_t *err)
{
    if (!o)
    {
        *pwm = *err = 0;
        
        return;
    }
    
    *pwm = (int32_t) o->pwm;
    *err = (int32_t) o->En;
}

extern float eo_pid_PWM_pi(EOpid *o, float Tr, float Tm)
{
    //if (!o) return 0;
    
    float En = Tr - Tm;

    //o->In += o->Ki*En;
    o->In += o->Ki*(En + o->En);
    LIMIT(o->In, o->Imax);
    
    o->En = En;
    
    o->pwm = o->pwm_offset + o->Kp*En + o->In;
    LIMIT(o->pwm, o->pwm_max);

#ifdef FORCE_ZERO_PWM_OUT
    return 0;
#else
    return o->pwm;
#endif
}

extern float eo_pid_PWM_pi_0_5Hz_1stLPF(EOpid *o, float Tr, float Tm)
{
    //if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->Dn *= o->A;
    o->Dn += o->B*(En - o->En);
    
    //o->In += o->Ki*En;
    o->In += o->Ki*(En + o->En);
    LIMIT(o->In, o->Imax);
    
    o->En = En;
    
    En *= o->Kp;
    En += o->In + o->Dn;
    
    o->yv0 = 0.0015683341f*(o->xv0 + En)+0.9968633318f*o->yv0; // 0.5 Hz 
    o->xv0 = En;
    
    o->pwm = o->pwm_offset + o->yv0;
    
    LIMIT(o->pwm, o->pwm_max);

#ifdef FORCE_ZERO_PWM_OUT
    return 0;
#else
    return o->pwm;
#endif
}

extern float eo_pid_PWM_friction(EOpid *o, float pwm_input, float vel, float Tr)
{
    //viscous friction
    pwm_input=pwm_input + o->Ktau * o->Kbemf*vel;
    
    //torque feedforward
    pwm_input=pwm_input + o->Ktau * o->Kff * Tr;

    //coulomb friction
    /*float coulomb = 0;
    if (vel>910) coulomb = 1; //910 corresponds to 5deg/s (65535/360*5)
    else if (vel<-910) coulomb = -1;
    else
    {
      coulomb = vel*vel*vel/753571000; //753571000 = 910^3
    }
    
    if (vel>0)
    {coulomb = coulomb * o->stiction_up_val;}
    else
    {coulomb = coulomb * o->stiction_down_val;} 
    pwm_input=pwm_input+coulomb;
    */
  

    return pwm_input;
}

extern float eo_pid_PWM_pi_0_8Hz_1stLPF(EOpid *o, float Tr, float Tm)
{
    //if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->Dn *= o->A;
    o->Dn += o->B*(En - o->En);
    
    //o->In += o->Ki*En;
    o->In += o->Ki*(En + o->En);
    LIMIT(o->In, o->Imax);
    
    o->En = En;
    
    En *= o->Kp;
    En += o->In + o->Dn;
    
    o->yv0 = 0.00250697865f*(o->xv0 + En)+0.9949860427f*o->yv0; // 0.8 Hz 
    o->xv0 = En;
    
    o->pwm = o->pwm_offset + o->yv0;
    
    LIMIT(o->pwm, o->pwm_max);

#ifdef FORCE_ZERO_PWM_OUT
    return 0;
#else
    return o->pwm;
#endif
}

extern float eo_pid_PWM_pi_1_1Hz_1stLPF(EOpid *o, float Tr, float Tm)
{
    //if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->Dn *= o->A;
    o->Dn += o->B*(En - o->En);
    
    //o->In += o->Ki*En;
    o->In += o->Ki*(En + o->En);
    LIMIT(o->In, o->Imax);
    
    o->En = En;
    
    En *= o->Kp;
    En += o->In + o->Dn;
    
    o->yv0 = 0.0034438644f*(o->xv0 + En)+0.9931122710f*o->yv0; // 1.1 Hz 
    o->xv0 = En;
    
    o->pwm = o->pwm_offset + o->yv0;
    
    LIMIT(o->pwm, o->pwm_max);

#ifdef FORCE_ZERO_PWM_OUT
    return 0;
#else
    return o->pwm;
#endif
}

extern float eo_pid_PWM_pi_3_0Hz_1stLPF(EOpid *o, float Tr, float Tm)
{
    //if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->Dn *= o->A;
    o->Dn += o->B*(En - o->En);
    
    //o->In += o->Ki*En;
    o->In += o->Ki*(En + o->En);
    LIMIT(o->In, o->Imax);
    
    o->En = En;
    
    En *= o->Kp;
    En += o->In + o->Dn;
    
    o->yv0 = 0.0093370547f*(o->xv0 + En)+0.9813258905f*o->yv0; // 3.0 Hz
    o->xv0 = En;
    
    o->pwm = o->pwm_offset + o->yv0;
    
    LIMIT(o->pwm, o->pwm_max);

#ifdef FORCE_ZERO_PWM_OUT
    return 0;
#else
    return o->pwm;
#endif
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




#if 0
extern float eo_pid_PWM_pi_1_1Hz_2ndLPF(EOpid *o, float Tr, float Tm)
{
    if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->En = En;
    
    //o->In += o->Kih*En;
    //LIMIT(o->In, o->Imax);
    En *= o->Kp;
    //En += o->In;
    
    o->yv1 = 1.1884094643e-5f*(o->xv0 +2.0f*o->xv1 + En) -0.9902732712f*o->yv0 +1.9902257348f*o->yv1; // 1.1 Hz
    o->yv0 = o->yv1;
    o->xv0 = o->xv1;
    o->xv1 = En;
    
    o->pwm = o->pwm_offset + o->yv1;
    
    LIMIT(o->pwm, o->pwm_max);

    return o->pwm;
}

extern float eo_pid_PWM_pi_3_0Hz_2ndLPF(EOpid *o, float Tr, float Tm)
{
    if (!o) return 0;
    
    float En = Tr - Tm;
    
    o->En = En;
   
    //o->In += o->Kih*En;
    //LIMIT(o->In, o->Imax);
    En *= o->Kp;
    //En += o->In;
    
    o->yv1 = 8.7655548748e-5f*(o->xv0 +2.0f*o->xv1 + En) -0.9736948720f*o->yv0 +1.9733442498f*o->yv1; // 3.0 Hz
    o->yv0 = o->yv1;
    o->xv0 = o->xv1;
    o->xv1 = En;
    
    o->pwm = o->pwm_offset + o->yv1;
    
    LIMIT(o->pwm, o->pwm_max);

    return o->pwm;
}

#endif
