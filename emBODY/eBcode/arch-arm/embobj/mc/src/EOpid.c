
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

static int32_t limit(int32_t val, int32_t lim);


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
        o->Kp = 0;
        o->Ki = 0;
        o->Kd = 0;

        o->shift = 0;
        o->shift_fact_by_2 = 0;

        o->En = 0;        
        o->KiIn = 0;
        o->Dn = 0;
        
        o->pwm = 0;

        o->Ymax = 0;
        o->Imax = 0;
        o->Imax_shift = 0;

        o->initialized = eobool_false;
    }

    return o;
}

extern void eo_pid_Init(EOpid *o, int32_t Kp, int32_t Kd, int32_t Ki, int32_t Ymax, int32_t Imax, uint8_t shift)
{
    eo_pid_SetPid(o, Kp, Kd, Ki, shift);

    eo_pid_SetPidLimits(o, Ymax, Imax);

    eo_pid_Reset(o);

    o->initialized = eobool_true;
}

extern void eo_pid_SetPid(EOpid *o, int32_t Kp, int32_t Kd, int32_t Ki, uint8_t shift)
{
    o->Kp = Kp;
    o->Kd = Kd;
    o->Ki = Ki;

    o->shift = shift;

    o->shift_fact_by_2 = shift ? (1<<(shift-1)) : 0;

    o->Imax_shift = o->Imax<<shift;
}

extern void eo_pid_SetPidLimits(EOpid *o, int32_t Ymax, int32_t Imax)
{
    o->Ymax = Ymax;
    o->Imax = Imax;

    o->Imax_shift = Imax<<o->shift;
}

extern void eo_pid_GetStatus(EOpid *o, int16_t *pwm, int32_t *err)
{
    *pwm = (int16_t)o->pwm;
    *err = o->En;
}

extern eObool_t eo_pid_IsInitialized(EOpid *o)
{
    return o->initialized;
}

extern void eo_pid_Reset(EOpid *o)
{
    o->En = 0;
    o->KiIn = 0; 
    o->Dn = 0;

    o->pwm = 0;
}

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

    o->Dn = (15*o->Dn+(En-o->En))/16; ///

    o->En = En;

    o->KiIn += o->Ki*En;

    if (o->KiIn > o->Imax_shift) 
        o->KiIn =  o->Imax_shift;
    else if (o->KiIn < -o->Imax_shift) 
        o->KiIn = -o->Imax_shift;

    return (int16_t)o->pwm;
}

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

    o->En = En;

    o->KiIn += o->Ki*En;

    if (o->KiIn > o->Imax_shift) 
        o->KiIn =  o->Imax_shift;
    else if (o->KiIn < -o->Imax_shift) 
        o->KiIn = -o->Imax_shift;

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

static int32_t limit(int32_t val, int32_t lim)
{
    if (val >=  lim) return  lim;

    if (val <= -lim) return -lim;

    return val;
} 

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




