
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

extern int32_t posref_can;

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

static const float PERIOD    = 0.001f;       // 1 ms
//static const float FREQUENCY = 1.0f/PERIOD;  // 1 kHz

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOtrajectory* eo_trajectory_New(void) 
{
    EOtrajectory *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtrajectory), 1);

    if (o)
    {
//        o->Ai = 0.0f;
//        o->Bi = 0.0f;
//        o->Ci = 0.0f;
//        o->Kc = 0.0f;
//
//        o->Zi = 0.0f;
//        o->Yi = 0.0f;
//        o->Ky = 0.0f;
//            
//        o->Fi = 0.0f;
//        o->Kf = 0.0f;
//
//        o->pi = 0.0f;
//        o->pf = 0.0f;
//        o->vi = 0.0f;

//        o->steps_to_end = 0;

          o->time = 1.0f;
          o->pfp0 = 0.0f;
          o->step = 0.0f;
          o->p0 = 0.0f;
          o->v0 = 0.0f;
          o->p = 0.0f;
          o->v = 0.0f;
    }

    return o;
}

extern void eo_trajectory_SetReference(EOtrajectory *o, float p0, float pf, float v0, float speed)
{
    if (speed == 0.0f)
    {
        eo_trajectory_Abort(o);

        return;
    }    

    o->pfp0 = pf - p0;

    float T = o->pfp0/speed;
    
    if (T < 0.0f) T = -T;

    o->step = PERIOD / T;
    
    o->p = o->p0 = p0;
    o->v = o->v0 = v0;

    o->pf = pf;

    o->pfp030 = o->pfp0 * 30.0f;

    o->time = 0.0f; 

    /*
    float pf_p0;
    float tf;
    float steps;
    float D;
    float D2;
    float K5D2;
    
    if (speed == 0.0f)
    {
        eo_trajectory_Abort(o);

        return;
    }
    
    pf_p0 = pf - p0;

    tf = pf_p0/speed;
    
    if (tf < 0.0f) tf = -tf; 

    v0 *= tf;

    steps = FREQUENCY*tf;

    o->steps_to_end = (unsigned long)steps;

    if (o->steps_to_end == 0)
    { 
        o->steps_to_end = 1;

        steps = 1.0f;
    }

    D  = 1.0f / steps;
    D2 = D*D;
   
    o->Ai = 0.0f;
    o->Bi = D2*D;
    o->Ci = 6.0f*o->Bi;
    o->Kc = o->Ci;

    
    o->Zi = 10.0f*pf_p0 - 6.0f*v0;
    K5D2  = (6.0f*pf_p0 - 3.0f*v0)*D2;
    o->Yi = K5D2 - (15.0f*pf_p0 - 8.0f*v0)*D;
    o->Ky = 2.0f*K5D2;

    o->Fi = p0;
    o->Kf = v0*D;

    o->pi = p0;
    o->pf = pf;
    o->vi = v0;
    */
}

/** @fn         extern float eo_trajectory_Step(EOtrajectory *o)
    @brief      Executes a trajectory step.
    @param      o               The pointer to the trajectory object.
    @return     The actual trajectory point value.
 **/

extern void eo_trajectory_Abort(EOtrajectory *o)
{
//    o->steps_to_end = 0;
//    o->vi = 0.0f;
//    o->pf = o->pi;

    o->pf = o->p;
    o->time = 1.0f;
}

/*
extern void eo_trajectory_Stop(EOtrajectory *o, float pos)
{
    o->steps_to_end = 0;
    o->vi = 0.0f;
    o->pf = o->pi = pos;
}
*/

extern uint8_t eo_trajectory_IsDone(EOtrajectory* o)
{
//    return o->steps_to_end == 0;
    return o->time >= 1.0f;
}

extern float eo_trajectory_GetPos(EOtrajectory* o)
{
    //return o->pi;
    return o->p;
}

extern float eo_trajectory_GetVel(EOtrajectory* o)
{
    //return o->vi;
    return o->v;
}

extern void eo_trajectory_Step(EOtrajectory* o, float *p, float *v)
{
    float t = o->time;

    if (t >= 1.0f)
    {
        *p = o->p = o->pf;
        *v = 0.0f;
        
        posref_can = (int32_t)*p;

        return;
    }
    
    float t2 = t*t;

    *p = o->p0 + o->v0*t + t*t2*(o->pfp0*((6.0f*t-15.0f)*t+10.0f)+o->v0*((-3.0f*t+8.0f)*t-6.0f));
    *v = o->v0 + t2*(o->pfp030*((t-2.0f)*t+1.0f)+o->v0*((-15.0f*t+32.0f)*t-18.0f));

    posref_can = (int32_t)*p;

    o->time += o->step;

    /*
    if (!o->steps_to_end)
    {
        o->pi = *pi = o->pf;
        o->vi = *vi = 0.0f;

        posref_can = (int32_t)*pi;

        return;
    }
       
    --(o->steps_to_end);
    
    o->Ai += o->Bi; 
    o->Bi += o->Ci;
    o->Ci += o->Kc;
    
    o->Zi += o->Yi; 
    o->Yi += o->Ky;

    o->Fi += o->Kf;

    *pi = o->Fi + o->Ai*o->Zi;
    
    if (vi)
    {
        o->vi = *vi = (*pi - o->pi) * FREQUENCY;
    }

    o->pi = *pi;

    posref_can = (int32_t)*pi;
    */
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




