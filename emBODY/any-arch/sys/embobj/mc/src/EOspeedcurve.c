
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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOspeedcurve.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOspeedcurve_hid.h" 


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

static const char s_eobj_ownname[] = "EOspeedcurve";

static const int32_t FREQUENCY = 1000;         // 1 kHz
static const float   frequency = (float)FREQUENCY;
static const float   PERIOD = 1.0f/frequency;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOspeedcurve* eo_speedcurve_New(void) 
{
    EOspeedcurve *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOspeedcurve), 1);

    if (o)
    {
        o->p = 0.0f;
        o->v = 0.0f;
        o->a = 0.0f;

        o->Ky = 0.0f;
        o->Yn = 0.0f;

        o->period_by_time = 0.0f;

        o->vf = 0;

        o->steps_to_end = 0;
    }

    return o;
}

/*
extern void eo_speedcurve_SetReference(EOspeedcurve *o, int32_t p0, int32_t v0, int32_t vf, int32_t acc)
{
    if (acc == 0.0f)
    {
        o->p = (float)p0;
        o->v = o->vf = (float)v0;

        return;
    }    

    o->p  = (float)p0;
    o->v  = (float)v0;
    o->vf = (float)vf;
    o->acc = PERIOD*(float)acc;

    o->done = eobool_false;
}
*/

extern void eo_speedcurve_SetReference(EOspeedcurve *o, int32_t p0, int32_t v0, int32_t vf, int32_t acc)
{
    if (!acc || vf==v0)
    {
        o->steps_to_end = 0;

        return;
    }    

    o->p = (float)p0;
    o->v = (float)v0;
    o->a = 0.0f;

    o->vf = vf;

    int32_t vfv0 = vf-v0;

    o->steps_to_end = (vfv0*FREQUENCY)/acc;

    if (o->steps_to_end < 0) o->steps_to_end = -o->steps_to_end;

    float DT = 1.0f/((float)o->steps_to_end);
    float DT2 = DT*DT;

    o->period_by_time = DT;

    o->Yn = (DT-DT2)*(float)(6*vfv0);
    o->Ky = DT2*(float)(-12*vfv0);
}

/** @fn         extern float eo_trajectory_Step(EOtrajectory *o)
    @brief      Executes a trajectory step.
    @param      o               The pointer to the trajectory object.
    @return     The actual trajectory point value.
 **/

extern void eo_speedcurve_Stop(EOspeedcurve *o, int32_t p)
{
    o->p = p;
    o->v = o->vf = 0;
    o->steps_to_end = 0;
}

extern eObool_t eo_speedcurve_IsDone(EOspeedcurve* o)
{
    return o->steps_to_end == 0;
}

extern int32_t eo_speedcurve_GetVel(EOspeedcurve* o)
{
    return (int32_t)o->v;
}
/*
extern void eo_speedcurve_Step(EOspeedcurve* o, int32_t* p, int32_t* v)
{
    if (o->done)
    {
        o->v = o->vf;
    }
    else
    {
        float acc = o->vf - o->v;

        if (-o->acc <= acc && acc <= o->acc)
        {
            o->v = o->vf;

            o->done = eobool_true;
        }
        else if (acc < -o->acc)
        {
            o->v -= o->acc;
        }
        else if (acc >  o->acc)
        {
            o->v += o->acc;
        }
    }

    o->p += o->v*PERIOD;

    if (v) *v = (int32_t)o->v;
    if (p) *p = (int32_t)o->p;
}
*/

extern void eo_speedcurve_Step(EOspeedcurve* o, int32_t *p, int32_t *v, int32_t *a)
{
    if (o->steps_to_end)
    {
        --o->steps_to_end;

        o->a  += o->Yn;
        o->Yn += o->Ky;

        o->v += o->period_by_time*o->a;

        if (a) *a = (int32_t)o->a;
        if (v) *v = (int32_t)o->v;
    }
    else
    {
        o->v = o->vf;

        if (a) *a = 0;
        if (v) *v = o->vf;        
    }
    
    o->p += PERIOD*o->v;

    if (p) *p = (int32_t)o->p;
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
