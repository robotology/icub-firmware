
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
        o->pwm_offset = 0;
        
        o->K  = 0.f;
        o->Ki = 0.f;
        o->Kd = 0.f;
        
        o->Xn = 0.f;
        o->Yn = 0.f;
        
        o->Dn    = 0.f;
        o->En    = 0.f;        
        o->KKiIn = 0.f;        
        o->Imax  = 0.f;
        o->Ymax  = 0.f;

        o->pwm     = 0;
        
        //o->A = 0.f;
        //o->B = 0.f;
    }

    return o;
}

extern void eo_pid_SetPid(EOpid *o, float K, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    if (!o) return;
    
    o->K  = K;
    o->Kd = Kd*EMS_PERIOD;
    o->Ki = Ki;
	
    o->Imax = Imax;
    o->Ymax = Ymax;
    
    o->pwm_offset = Yoff;
    
    //float N = 10.0;
    //float T = o->Kd/N;
    //float Q = 2.0f*T*EMS_FREQUENCY_FLOAT; 

    //o->A = (Q-1.0f)/(Q+1.0f);
    //o->B = EMS_FREQUENCY_FLOAT*(1.0f-o->A);
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
    
    o->Xn = 0.f;
    o->Yn = 0.f;
    
    o->pwm = 0;
}
/*
extern int32_t eo_pid_PWM(EOpid *o, float En)
{
    if (!o) return 0;
                
    float Dn = o->A*o->Dn + o->B*(En - o->En);
    
    o->En = En;
    o->Dn = Dn;

    float Xn = o->K*o->En;
    
    o->KKiIn += o->Ki*Xn;

    LIMIT(o->KKiIn, o->Imax);

    o->pwm = o->Yoff + (int32_t)(Xn + o->K*Dn + o->KKiIn);
    
    LIMIT(o->pwm, o->pwm_max);

    return o->pwm;
}
*/

#define ZRT 500.f

extern int32_t eo_pid_PWM2(EOpid *o, float En, float Vref, float Aref, float Venc)
{
    if (!o) return 0;
    
    o->En = En;
    
    o->Yn += o->Ki*(o->K*(En + o->Kd*(Vref-Venc)) - o->Yn);
    
    LIMIT(o->Yn, o->Ymax);
    
    return o->pwm = o->pwm_offset + (int32_t)o->Yn;
}



/*
extern int32_t eo_pid_PWM2(EOpid *o, float En, float Vref, float Venc)
{
    if (!o) return 0;
    
    o->En = En;
    
    //float Xn = (Vref == 0.0f) ? (o->K*o->En) : (o->K*(o->En+o->Kd*(Vref-Venc)));

    float Xn = o->K*(o->En+o->Kd*(Vref-Venc));
    
    o->KKiIn += o->Ki*Xn;

    LIMIT(o->KKiIn, o->Imax);

    o->pwm = o->Yoff + (int32_t)(Xn+o->KKiIn);
        
    LIMIT(o->pwm, o->pwm_max);
    
    return o->pwm;
}
*/



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




