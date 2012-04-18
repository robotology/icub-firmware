
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

static const float PERIOD = 0.001f;          // 1 ms
static const float FREQUENCY = 1.0f/PERIOD;  // 1 kHz

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOtrajectory* eo_trajectory_New(void) 
{
    EOtrajectory *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtrajectory), 1);

    if (o)
    {
        o->Ai = 0.0f;
        o->Bi = 0.0f;
        o->Ci = 0.0f;
        o->Kc = 0.0f;

        o->Zi = 0.0f;
        o->Yi = 0.0f;
        o->Ky = 0.0f;
            
        o->Fi = 0.0f;
        o->Kf = 0.0f;

        o->pi = 0.0f;
        o->pf = 0.0f;
        
        o->delta = 0.0f;

        o->steps_to_end = 0;
    }

    return o;
}

extern void eo_trajectory_Set(EOtrajectory *o, float p0, float pf, float v0, float speed /*float tf*/)
{
    if (speed == 0.0f)
    {
        eo_trajectory_Abort(o);

        return;
    }
    
    float pf_p0 = pf - p0;

    float tf = pf_p0/speed;
    
    if (tf < 0.0f) tf = -tf; 

    v0 *= tf;

    float steps = FREQUENCY*tf;

    o->steps_to_end = (unsigned long)steps;

    if (o->steps_to_end == 0)
    { 
        o->steps_to_end = 1;

        steps = 1.0f;
    }

    float D = 1.0f / steps;
    float D2 = D*D;
   
    o->Ai = 0.0f;
    o->Bi = D2*D;
    o->Ci = 6.0f*o->Bi;
    o->Kc = o->Ci;

    
    o->Zi = 10.0f*pf_p0 - 6.0f*v0;
    float K5D2 = (6.0f*pf_p0 - 3.0f*v0)*D2;
    o->Yi = K5D2 - (15.0f*pf_p0 - 8.0f*v0)*D;
    o->Ky = 2.0f*K5D2;

    o->Fi = p0;
    o->Kf = v0*D;

    o->pi = p0;
    o->pf = pf;

    o->delta = 0.0f;
}


/** @fn         extern float eo_trajectory_Step(EOtrajectory *o)
    @brief      Executes a trajectory step.
    @param      o               The pointer to the trajectory object.
    @return     The actual trajectory point value.
 **/

extern void eo_trajectory_Abort(EOtrajectory *o)
{
    o->steps_to_end = 0;
    o->delta = 0.0f;
    o->pf = o->pi;
}

float eo_trajectory_GetSpeed(EOtrajectory *o)
{
    return o->delta*FREQUENCY;
}

float eo_trajectory_GetPos(EOtrajectory *o)
{
    return o->pi;
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




