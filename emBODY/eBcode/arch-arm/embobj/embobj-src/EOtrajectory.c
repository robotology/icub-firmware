
/* @file       EOtrajectory.c
    @brief      This file implements internal implementation of a motor minimum jerk trajectory generator.
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

extern const int32_t EMS_IFREQUENCY;
extern const float   EMS_FFREQUENCY;
extern const float   EMS_PERIOD;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtrajectory.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtrajectory_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

#define NULL_POSITION 0x80000000

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

static const char s_eobj_ownname[] = "EOtrajectory";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOtrajectory* eo_trajectory_New(void) 
{
    EOtrajectory *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtrajectory), 1);

    if (o)
    {
        o->p = 0.0f;
        o->v = 0.0f;
        o->a = 0.0f;

        o->pf = NULL_POSITION;
        o->vf = 0.0f;

        o->Kw = 0.0f;
        o->Wn = 0.0f;
        o->Xn = 0.0f;

        o->Kz = 0.0f;
        o->Zn = 0.0f;

        o->pos_steps_to_end = 0;
        o->vel_steps_to_end = 0;

        o->pos_min =    0;
        o->pos_max = 4095;
    }

    return o;
}

extern void eo_trajectory_SetLimits(EOtrajectory *o, int32_t pos_min, int32_t pos_max)
{
    o->pos_min = pos_min;
    o->pos_max = pos_max;
}

extern void eo_trajectory_Init(EOtrajectory *o, int32_t p0, int32_t v0, int32_t a0)
{
    o->p = p0;
    o->v = v0;
    o->a = a0;
}

extern void eo_trajectory_SetPosReference(EOtrajectory *o, int32_t p1, uint32_t time_ms, eObool_t reset)
{
    if (reset)
    {
        o->vel_steps_to_end = 0;
        o->vf = 0.0f;
    }

    if (!time_ms)
    {
        o->pos_steps_to_end = 0;
        o->pf = o->p;
        
        return;    
    }

    if (p1 < o->pos_min)
    { 
        p1 = o->pos_min;
    } 
    else if (p1 > o->pos_max)
    {
        p1 = o->pos_max;
    }

    if (!o->vel_steps_to_end) o->pf = p1;

    o->pos_steps_to_end = (time_ms*EMS_IFREQUENCY)/1000;

    float lbyT = 1000.0f/(float)time_ms;
    float PbyT = EMS_PERIOD*lbyT;

    float DbyT2 = ((float)p1-o->p)*lbyT*lbyT;
    float VbyT  = o->v*lbyT; 

    float A3 = ( 120.0f*DbyT2-60.0f*VbyT-10.0f*o->a)*PbyT*PbyT*PbyT;
    float A2 = (-180.0f*DbyT2+96.0f*VbyT+18.0f*o->a)*PbyT*PbyT;

    o->Kw = 6.0f*A3;
    o->Wn = 2.0f*A2+o->Kw;
    o->Xn = (60.0f*DbyT2-36.0f*VbyT-9.0f*o->a)*PbyT+A2+A3;
}

extern void eo_trajectory_SetVelReference(EOtrajectory *o, int32_t v1, uint32_t time_ms, eObool_t reset)
{
    if (reset)
    {
        o->pos_steps_to_end = 0;
        o->pf = NULL_POSITION;
    }

    if (!time_ms)
    {
        o->vel_steps_to_end = 0;
        o->vf = o->v;

        return;
    }
    
    o->vf = (float)v1; 
    
    o->vel_steps_to_end = (time_ms*EMS_IFREQUENCY)/1000;

    float lbyT = 1000.0f/(float)time_ms;
    float PbyT = EMS_PERIOD*lbyT;

    float DbyT = (o->vf-o->v)*lbyT;
    
    o->Kz = (-12.0f*DbyT+6.0f*o->a)*PbyT*PbyT;
    o->Zn = (  6.0f*DbyT-4.0f*o->a)*PbyT+0.5f*o->Kz;
}

extern void eo_trajectory_TimeoutVelReference(EOtrajectory *o)
{
    o->vel_steps_to_end = 0;
    o->vf = 0.0f;
}

extern void eo_trajectory_Stop(EOtrajectory *o, int32_t p)
{
    o->pf = o->p = p;
    o->vf = o->v = 0;
    o->a = 0;

    o->pos_steps_to_end = 0;
    o->vel_steps_to_end = 0;
}

extern eObool_t eo_trajectory_IsDone(EOtrajectory* o)
{
    return !o->vel_steps_to_end && !o->pos_steps_to_end;
}

extern int32_t eo_trajectory_GetPos(EOtrajectory* o)
{
    return (int32_t)o->p;
}

extern int32_t eo_trajectory_GetVel(EOtrajectory* o)
{
    return (int32_t)o->v;
}

extern int32_t eo_trajectory_GetAcc(EOtrajectory* o)
{
    return (int32_t)o->a;
}

extern void eo_trajectory_Step(EOtrajectory* o, int32_t *p, int32_t *v, int32_t *a)
{
    if (o->pos_steps_to_end)
    {
        --o->pos_steps_to_end;

        o->a  += o->Xn;
        o->Xn += o->Wn;
        o->Wn += o->Kw;   
    }

    if (o->vel_steps_to_end)
    {
        --o->vel_steps_to_end;

        o->a  += o->Zn;
        o->Zn += o->Kz; 
    }

    if (o->vel_steps_to_end || o->pos_steps_to_end)
    {
        o->v += EMS_PERIOD*o->a;
        o->p += EMS_PERIOD*o->v; 
    }
    else
    {
        o->a = 0.0f;
        o->v = o->vf;

        if (o->v == 0.0f)
        { 
            if (o->pf != NULL_POSITION) o->p = o->pf;
        }
        else
        {
            o->p += EMS_PERIOD*o->v;    
        } 
    } 

    if (o->p <= o->pos_min)
    { 
        o->p = o->pos_min;
        o->v = 0.0f;
    } 
    else if (o->p >= o->pos_max)
    {
        o->p = o->pos_max;
        o->v = 0.0f;
    }

    if (a) *a = (int32_t) o->a;
    if (v) *v = (int32_t) o->v;
    if (p) *p = (int32_t) o->p;
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
