
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

        o->pf = 0.0f;
        o->vf = 0.0f;
        o->pos_set = eobool_false;

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

extern void eo_trajectory_SetPosReference(EOtrajectory *o, int32_t p1, int32_t avg_vel)
{
    o->pos_set = eobool_true;
    o->vel_steps_to_end = 0;
    o->vf = 0.0f;

    if (!avg_vel)
    {
        o->pos_steps_to_end = 0;
        o->pf = o->p;
 
        return;    
    }

    if (p1 < o->pos_min)
    { 
        o->pf = o->pos_min;
    } 
    else if (p1 > o->pos_max)
    {
        o->pf = o->pos_max;
    }
    else
    {
        o->pf = p1;
    }

    float D = o->pf-o->p;

    float V = (float)avg_vel;

    if ((D<0.0f) ^ (V<0.0f)) V = -V;

    float T = D/V;

    o->pos_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);
    
    float lbyT = 1.0f/T;
    float PbyT = EMS_PERIOD*lbyT;

    float DbyT2 = V*lbyT;
    float VbyT = o->v*lbyT; 

    float A3 = ( 120.0f*DbyT2-60.0f*VbyT-10.0f*o->a)*PbyT*PbyT*PbyT;
    float A2 = (-180.0f*DbyT2+96.0f*VbyT+18.0f*o->a)*PbyT*PbyT;

    o->Kw = 6.0f*A3;
    o->Wn = 2.0f*A2+o->Kw;
    o->Xn = (60.0f*DbyT2-36.0f*VbyT-9.0f*o->a)*PbyT+A2+A3;
}

extern void eo_trajectory_SetVelReference(EOtrajectory *o, int32_t v1, int32_t avg_acc, eObool_t reset)
{
    float D;

    o->vf = (float)v1;
    
    if (reset)
    {
        o->pos_steps_to_end = 0;

        D = o->vf-o->v;
    }
    else
    {
        D = o->vf;
    }

    o->pos_set = eobool_false;

    if (!avg_acc)
    {
        o->vel_steps_to_end = 0;
        o->vf = o->v;

        return;
    }
   
    float A = (float)avg_acc;

    if ((D<0.0f) ^ (A<0.0f)) A = -A;

    float T = D/A;   
   
    o->vel_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);

    float PbyT = EMS_PERIOD/T;

    if (reset)
    {
        o->Kz = (-12.0f*A+6.0f*o->a)*PbyT*PbyT;
        o->Zn = (  6.0f*A-4.0f*o->a)*PbyT+0.5f*o->Kz;
    }
    else
    {
        o->Kz = -12.0f*A*PbyT*PbyT;
        o->Zn =   6.0f*A*PbyT+0.5f*o->Kz;
    }
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
    o->pos_set = eobool_true;
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

extern int8_t eo_trajectory_Step(EOtrajectory* o, float *p, float *v, float *a)
{
    int8_t limit = 0;

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

        if (o->pos_set)
        {
            o->p = o->pf;        
        }
        else
        {
            o->p += EMS_PERIOD*o->v;
        }
    }        

    if (o->p <= o->pos_min)
    { 
        o->p = o->pos_min;
        if (o->v < 0.0f) o->v = 0.0f;
        limit = -1;
    } 
    else if (o->p >= o->pos_max)
    {
        o->p = o->pos_max;
        if (o->v > 0.0f) o->v = 0.0f;
        limit =  1;
    }

    if (a) *a = o->a;
    if (p) *p = o->p;
    if (v) *v = o->v;

    /*
        if (!limit)
        {
            int32_t pp = (int32_t)o->p + ((int32_t)o->v)/10;

            if (pp <= o->pos_min)
            {
                if (*v < 0) *v = 0;
                limit = -1;
            }
            else if (pp >= o->pos_max)
            {
                if (*v > 0) *v = 0;
                limit =  1;
            }
        }
    */

    return limit;
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
