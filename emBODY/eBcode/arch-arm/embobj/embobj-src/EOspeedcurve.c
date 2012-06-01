
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

static const float PERIOD = 0.001f;
//static const int32_t FREQUENCY = 1000;         // 1 kHz

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOspeedcurve* eo_speedcurve_New(void) 
{
    EOspeedcurve *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOspeedcurve), 1);

    if (o)
    {
        o->v = 0.0f;
        o->acc = 0.0f;

        o->vf = 0.0f;

        o->done = eobool_true;
    }

    return o;
}

extern void eo_speedcurve_SetReference(EOspeedcurve *o, int32_t v0, int32_t vf, int32_t acc)
{
    if (acc == 0.0f)
    {
        o->v = o->vf = (float)v0;

        return;
    }    

    o->acc = PERIOD*(float)acc;
    o->v  = (float)v0;
    o->vf = (float)vf;

    o->done = eobool_false;

    o->rabbit = 0.0f;
}

/** @fn         extern float eo_trajectory_Step(EOtrajectory *o)
    @brief      Executes a trajectory step.
    @param      o               The pointer to the trajectory object.
    @return     The actual trajectory point value.
 **/

extern void eo_speedcurve_Stop(EOspeedcurve *o, int32_t p)
{
    o->vf = 0.0f;

    o->done = eobool_true;
}

extern eObool_t eo_speedcurve_IsDone(EOspeedcurve* o)
{
    return o->done;
}

extern int32_t eo_speedcurve_GetVel(EOspeedcurve* o)
{
    return (int32_t)o->v;
}

extern int32_t eo_speedcurve_Step(EOspeedcurve* o, int32_t delta, int32_t* v)
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

    o->rabbit += o->v*PERIOD-(float)delta;

    if (v) *v = (int32_t)o->v;

    return (int32_t)(o->rabbit);
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
