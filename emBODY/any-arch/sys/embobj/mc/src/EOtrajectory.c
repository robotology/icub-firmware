
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

static const int32_t FREQUENCY = 1000;         // 1 kHz

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

        o->pf = 0;
        o->vf = 0;

        o->DT = 0.0f;

        o->steps_to_end = 0;

        o->Kw = 0.0f;
        o->Wn = 0.0f;
        o->Yn = 0.0f;
    }

    return o;
}

extern void eo_trajectory_SetReference(EOtrajectory *o, int32_t p0, int32_t pf, int32_t v0, int32_t vf, int32_t speed)
{
    if (!speed)
    {
        eo_trajectory_Abort(o, p0);

        return;
    }    

    o->p = (float)p0;
    o->v = (float)v0;
    o->a = 0.0f;

    o->pf = pf;
    o->vf = vf;

    int32_t pfp0 = pf-p0;

    int32_t A =  120*pfp0-60*(v0+vf);
    int32_t B = -180*pfp0+96*v0+63*vf;
  //int32_t C =   60*pfp0-36*v0-24*vf;

    o->steps_to_end = (pfp0*FREQUENCY)/speed;

    if (o->steps_to_end < 0) o->steps_to_end = -o->steps_to_end;

    float DT = 1.0f/((float)o->steps_to_end);
    float DT2 = DT*DT;
    float BDT2 = ((float)B)*DT2;
    float ADT3 = ((float)A)*DT2*DT;

    o->DT = DT;

    o->Kw =       6.0f*ADT3;
    o->Wn = o->Kw+2.0f*BDT2;
    o->Yn = ADT3+BDT2+((float)(60*pfp0-36*v0-24*vf))*DT;
}

/** @fn         extern float eo_trajectory_Step(EOtrajectory *o)
    @brief      Executes a trajectory step.
    @param      o               The pointer to the trajectory object.
    @return     The actual trajectory point value.
 **/

extern void eo_trajectory_Abort(EOtrajectory *o, int32_t p)
{
    o->pf = p;
    o->vf = 0;

    o->steps_to_end = 0;
}

extern eObool_t eo_trajectory_IsDone(EOtrajectory* o)
{
    return o->steps_to_end == 0;
}

extern int32_t eo_trajectory_GetPos(EOtrajectory* o)
{
    return (int32_t)o->p;
}

extern int32_t eo_trajectory_GetVel(EOtrajectory* o)
{
    return (int32_t)o->v;
}

extern void eo_trajectory_Step(EOtrajectory* o, int32_t *p, int32_t *v, int32_t *a)
{
    if (o->steps_to_end == 0)
    {
        if (a) *a = 0;
        if (v) *v = o->vf;
        if (p) *p = o->pf;

        return;
    }
    
    --(o->steps_to_end);

    o->a  += o->Yn;
    o->Yn += o->Wn;
    o->Wn += o->Kw;

    o->v += o->DT*o->a;
    o->p += o->DT*o->v;

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
