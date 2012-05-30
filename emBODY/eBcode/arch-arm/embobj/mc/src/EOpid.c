
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
// empty-section


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
        o->Ko = 0.0f;
        o->Kp = 0.0f;
        o->Ki = 0.0f;
        o->Kd = 0.0f;

        o->En = 0.0f;        
        o->In = 0.0f;
        o->Dn = 0.0f;
        
        o->Ymax = 0.0f;
        o->Imax = 0.0f;

        o->pwm = 0.0f;

        o->initialized = 0;
    }

    return o;
}

extern void eo_pid_Init(EOpid *o, float Kp, float Kd, float Ki, float Ko, float Ymax, float Imax)
{
    o->Ko = Ko;
    o->Kp = Kp;
    o->Kd = Kd;
    o->Ki = Ki;

    o->En = 0.0f;
    o->In = 0.0f;
    o->Dn = 0.0f;

    o->Ymax = Ymax;
    o->Imax = Imax;

    o->pwm = 0.0f;

    o->initialized = 1;
}

extern void eo_pid_SetPid(EOpid *o, float Kp, float Kd, float Ki)
{
    o->Kp = Kp;
    o->Kd = Kd;
    o->Ki = Ki; 
}

extern void eo_pid_SetPidLimits(EOpid *o, float Ymax, float Imax)
{
    o->Ymax = Ymax;
    o->Imax = Imax; 
}

extern void eo_pid_SetOffset(EOpid *o, float Ko)
{
    o->Ko = Ko;
}

extern float eo_pid_GetOffset(EOpid *o)
{
    return o->Ko;
}

extern void eo_pid_GetStatus(EOpid *o, float *pwm, float *err)
{
    *pwm = o->pwm;
    *err = o->En;
}

extern uint8_t eo_pid_IsInitialized(EOpid *o)
{
    return o->initialized;
}

extern void eo_pid_Reset(EOpid *o)
{
    o->In = 0.0f;
    o->En = 0.0f; 
    o->Dn = 0.0f; 
}

extern float eo_pid_PWM(EOpid *o, float En)
{
    o->pwm = o->Ko + o->Kp*En + o->In + o->Kd*o->Dn;

    o->In += o->Ki*En;

    if (o->In > o->Imax) 
        o->In = o->Imax; 
    else if (o->In< -o->Imax) 
        o->In = -o->Imax;

    o->Dn = 0.9f*o->Dn + 0.1f*(En - o->En);
    o->En = En;
    
    if (o->pwm > o->Ymax)
    {
        o->pwm = o->Ymax;
    }
    else if (o->pwm < -o->Ymax)
    {
        o->pwm = -o->Ymax;
    }

    return o->pwm;
}

extern float eo_pid_PWM2(EOpid *o, float En, float Vn)
{
    o->In += o->Ki*En;
    
    o->pwm = o->Kp*En + o->Kd*Vn + o->In;
    
    if (o->In > o->Imax) 
        o->In = o->Imax; 
    else if (o->In < -o->Imax) 
        o->In = -o->Imax;
    
    if (o->pwm > o->Ymax)
    {
        o->pwm = o->Ymax;
    }
    else if (o->pwm < -o->Ymax)
    {
        o->pwm = -o->Ymax;
    }

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




