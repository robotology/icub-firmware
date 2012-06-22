
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
        o->KKiIn = 0.0f;
         
        o->pwm = 0.0f;

        o->Ymax = 0.0f;
        o->Imax = 0.0f;

        o->initialized = eobool_false;
    }

    return o;
}

extern void eo_pid_Init(EOpid *o, float K, float Kd, float Ki, float Ymax, float Imax)
{
    eo_pid_SetPid(o, K, Kd, Ki);

    eo_pid_SetPidLimits(o, Ymax, Imax);

    eo_pid_Reset(o);

    o->initialized = eobool_true;
}

extern void eo_pid_SetPid(EOpid *o, float K, float Kd, float Ki)
{
    o->K  = K;
    o->Kd = Kd*EMS_PERIOD;
    o->Ki = Ki;
}

extern void eo_pid_SetPidLimits(EOpid *o, float Ymax, float Imax)
{
    o->Ymax = Ymax;
    o->Imax = Imax;
}

extern void eo_pid_GetStatus(EOpid *o, int16_t *pwm, int32_t *err)
{
    *pwm = (int16_t)o->pwm;
    *err = (int32_t)o->En;
}

extern eObool_t eo_pid_IsInitialized(EOpid *o)
{
    return o->initialized;
}

extern void eo_pid_Reset(EOpid *o)
{
    o->En = 0.0f;
    o->pwm = 0.0f;
    o->KKiIn = 0.0f; 
}

extern int16_t eo_pid_PWM2(EOpid *o, float En, float Vn)
{
    float Xn = o->K*(En+o->Kd*Vn);

    o->KKiIn += o->Ki*Xn;
    
    LIMIT(o->KKiIn, o->Imax);

    o->pwm = Xn+o->KKiIn;

    LIMIT(o->pwm, o->Ymax);

    o->En = En;

    return (int16_t)o->pwm;
}

/*
extern int16_t eo_pid_PWM2(EOpid *o, int32_t En, int32_t Vn)
{
    o->pwm = o->Kp*En+o->Kd*Vn+o->KiIn;

    if (o->pwm >= o->shift_fact_by_2)
    {
        o->pwm =  ((o->shift_fact_by_2+o->pwm)>>o->shift);

        if (o->pwm >  o->Ymax) o->pwm =  o->Ymax;        
    }
    else if (o-> pwm <= -o->shift_fact_by_2)
    {
        o->pwm = -((o->shift_fact_by_2-o->pwm)>>o->shift);

        if (o->pwm < -o->Ymax) o->pwm = -o->Ymax;
    }
    else
    {
        o->pwm = 0;
    }

    int32_t KiEn = o->Ki*En;

    if (o->KiIn<0 ^ KiEn<0) o->KiIn += KiEn;
    
    o->KiIn += KiEn;

    o->En = En;

    if (o->KiIn > o->Imax_shift) 
        o->KiIn =  o->Imax_shift;
    else if (o->KiIn < -o->Imax_shift) 
        o->KiIn = -o->Imax_shift;

    return (int16_t)o->pwm;
}
*/

/*
extern int16_t eo_pid_PWM(EOpid *o, int32_t En)
{
    o->pwm = o->Kp*En+o->Kd*o->Dn+o->KiIn;

    if (o->pwm >= o->shift_fact_by_2)
    {
        o->pwm =  ((o->shift_fact_by_2+o->pwm)>>o->shift);
        
        if (o->pwm >  o->Ymax) o->pwm =  o->Ymax;        
    }
    else if (o-> pwm <= -o->shift_fact_by_2)
    {
        o->pwm = -((o->shift_fact_by_2-o->pwm)>>o->shift);

        if (o->pwm < -o->Ymax) o->pwm = -o->Ymax;
    }
    else
    {
        o->pwm = 0;
    }

    int32_t KiEn = o->Ki*En;

    if (o->KiIn<0 ^ KiEn<0) o->KiIn += KiEn;

    o->KiIn += KiEn;

    o->Dn = (15*o->Dn+(En-o->En))/16; ///

    o->En = En;

    if (o->KiIn > o->Imax_shift) 
        o->KiIn =  o->Imax_shift;
    else if (o->KiIn < -o->Imax_shift) 
        o->KiIn = -o->Imax_shift;

    return (int16_t)o->pwm;
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

static int32_t limit(int32_t val, int32_t lim)
{
    if (val >=  lim) return  lim;

    if (val <= -lim) return -lim;

    return val;
} 

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




