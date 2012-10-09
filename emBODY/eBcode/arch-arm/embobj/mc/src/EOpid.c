
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
#define LIMIT(x,L) if (x>L) x=L; else if (x<-L) x=-L

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

        o->En = 0.0f;        
        o->KIn = 0.0f;
         
        o->pwm = 0.0f;

        o->Yoff = 0.0f;
        o->Ymax = 0.0f;
        o->Imax = 0.0f;
        o->KImax = 0.0f;

        o->initialized = eobool_false;
    }

    return o;
}

extern void eo_pid_Init(EOpid *o, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff)
{
    eo_pid_SetPid(o, K, Kd, Ki, Ymax, Imax, Yoff);

    eo_pid_Reset(o);

    o->initialized = eobool_true;
}

extern void eo_pid_SetPid(EOpid *o, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff)
{
    o->K  = K;
    o->Kd = Kd*EMS_PERIOD;
    o->Ki = Ki;
	
    o->Ymax = Ymax;
    o->Imax = Imax;
    o->Yoff = Yoff;

    o->KImax = o->Ki!=0.0f ? o->Imax/o->Ki : 0.0f;
}

extern void eo_pid_GetStatus(EOpid *o, int32_t *pwm, int32_t *err)
{
    *pwm = (int32_t)o->pwm;
    *err = (int32_t)o->En;
}

extern eObool_t eo_pid_IsInitialized(EOpid *o)
{
    return o->initialized;
}

extern void eo_pid_Reset(EOpid *o)
{
    if (!o) return;
	
    o->En = 0.0f;
    o->pwm = 0.0f;
    o->KIn = 0.0f; 
}

extern int16_t eo_pid_PWM2(EOpid *o, float En, float Vref,float Venc)
{
    float Xn = o->K*(En+o->Kd*(Vref-Venc));

    //if ((o->KIn<0.0f) ^ (Xn<0.0f)) o->KIn = 0.0f;
   
    o->KIn += Xn;

    LIMIT(o->KIn, o->KImax);

    o->pwm = Xn + o->Ki*o->KIn + o->Yoff;
     
    // dead zone suppression
    // motor moves with pwm > 680
    if (Vref>0.0f) 
        o->pwm += 680.0f;
    else 
    if (Vref<0.0f)
        o->pwm -= 680.0f; 

    LIMIT(o->pwm, o->Ymax);

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




