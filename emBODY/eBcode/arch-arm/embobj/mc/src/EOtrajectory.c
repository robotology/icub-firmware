
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

extern const float   EMS_PERIOD;
extern const float   EMS_FREQUENCY_FLOAT;
extern const int32_t EMS_FREQUENCY_INT32;
extern const int32_t TICKS_PER_REVOLUTION;

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

#define LIMIT(min, x, max) { if (x < (min)) x = (min); else if (x > (max)) x = (max); }

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

//static const char s_eobj_ownname[] = "EOtrajectory";

static float K60byP = 0.0f;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOtrajectory* eo_trajectory_New() 
{
    EOtrajectory *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtrajectory), 1);

    if (o)
    {
        o->pos_min = 0;
        o->pos_max = TICKS_PER_REVOLUTION;
        o->vel_max = TICKS_PER_REVOLUTION/3;
        o->acc_max = TICKS_PER_REVOLUTION;
        
        // position
        o->xPos  = 0.0f;
        o->xVel  = 0.0f;
        o->xPAcc = 0.0f;

        o->xPosF = 0.0f;
        o->xVelF = 0.0f;

        o->PosTimer = 0;
        
        // velocity
        o->bVelocityMove     = eobool_false;
        o->bIsBraking = eobool_false;
         
        o->vPos  = 0.0f;
        o->vVel  = 0.0f;
        o->vPAcc = 0.0f;
        
        o->vVelF = 0.0f;
        
        o->VelTimer = 0;
    }

    if (K60byP == 0.0f)
    {
        K60byP = 60.0f*EMS_FREQUENCY_FLOAT;
    }
    
    return o;
}

extern void eo_trajectory_SetPosMin(EOtrajectory *o, int32_t pos_min)
{
    if (o) o->pos_min = (float)pos_min;
}

extern void eo_trajectory_SetPosMax(EOtrajectory *o, int32_t pos_max)
{
    if (o) o->pos_max = (float)pos_max;
}

extern void eo_trajectory_SetVelMax(EOtrajectory *o, int32_t vel_max)
{
    if (o) o->vel_max = vel_max;
}

extern void eo_trajectory_Init(EOtrajectory *o, int32_t p0, int32_t v0, int32_t a0)
{
    if (!o) return;
    
    o->xPosF = o->xPos  = p0;
    o->xVel  = v0;
    o->xPAcc = EMS_PERIOD*a0;
    o->xVelF = 0.0f;
    
    o->PosTimer = 0;
    o->VelTimer = 0;
    
    o->bVelocityMove     = eobool_false;
    o->bIsBraking = eobool_false;
    
    o->vPos  = 0.0f;
    o->vVel  = 0.0f;
    o->vPAcc = 0.0f;
    o->vVelF = 0.0f;
}

extern void eo_trajectory_SetPosReference(EOtrajectory *o, int32_t p1, int32_t avg_vel)
{
    if (!o) return;
    
    if (o->bVelocityMove || o->VelTimer) eo_trajectory_VelocityStop(o);

    LIMIT(o->pos_min, p1, o->pos_max)

    o->xPosF = (float)p1;
    o->xVelF = 0.0f;

    o->xVel = 0.0f;
    o->xPAcc = 0.f;
    
    if (!avg_vel) avg_vel = o->vel_max;
    
    LIMIT(-o->vel_max, avg_vel, o->vel_max)
    
    o->PosTimer = (EMS_FREQUENCY_INT32*(p1-(int32_t)o->xPos))/avg_vel;
    
    if (o->PosTimer<0) o->PosTimer = -o->PosTimer;
}

extern void eo_trajectory_SetVelReference(EOtrajectory *o, int32_t v1, int32_t avg_acc)
{
    if (!o) return;
    
    o->bVelocityMove = eobool_true;
    o->bIsBraking = eobool_false;

    LIMIT(-o->vel_max, v1, o->vel_max)

    o->vVelF = (float)v1;
    
    o->xPAcc = 0.f;

    if (!avg_acc) avg_acc = o->acc_max;
    
    LIMIT(-o->acc_max, avg_acc, o->acc_max)
    
    o->VelTimer = (EMS_FREQUENCY_INT32*(v1-(int32_t)o->vVel))/avg_acc;

    if (o->VelTimer<0) o->VelTimer = -o->VelTimer;
}

extern void eo_trajectory_Stop(EOtrajectory *o, int32_t pos, int32_t stop_acc)
{
    if (!o) return;
    
    if (o->bVelocityMove || o->VelTimer) eo_trajectory_VelocityStop(o);

    o->xPosF = o->xPos = pos;
    
    //LIMIT(o->pos_min, o->PosF, o->pos_max)
    
    o->xVelF = 0.0f;

    if (!stop_acc) stop_acc = o->acc_max;

    o->PosTimer = (EMS_FREQUENCY_INT32*(int32_t)o->xVel)/stop_acc;

    if (o->PosTimer<0) o->PosTimer = -o->PosTimer;
}

extern void eo_trajectory_VelocityStop(EOtrajectory *o)
{
    if (!o) return;
    
    o->bIsBraking    = eobool_false;
    o->bVelocityMove = eobool_false;
    
    o->VelTimer = 0;

    o->xPos  += o->vPos;
    o->xPosF += o->vPos;
    o->xVel  += o->vVel;
    o->xPAcc += o->vPAcc;

    o->vPos  = 0.0f;
    o->vVel  = 0.0f;
    o->vVelF = 0.0f;
    o->vPAcc = 0.0f;    
}

extern void eo_trajectory_VelocityTimeout(EOtrajectory *o)
{
    if (!o) return;
    
    if (o->bVelocityMove)
    {
        o->bVelocityMove = eobool_false;
        o->bIsBraking    = eobool_true;

        eo_trajectory_SetVelReference(o, 0, o->acc_max);
    }
}

//////////////////////////////////////////////////////////////////////////////////

extern int8_t eo_trajectory_Step(EOtrajectory* o, float *p, float *v, float *a)
{
    if (!o) return 0;
    
    if (o->PosTimer)
    {
        float PbyT = 1.0f/(float)(o->PosTimer--);

        o->xPos += EMS_PERIOD*(o->xVel += o->xPAcc += (((K60byP*(o->xPosF-o->xPos)*PbyT-36.f*o->xVel /*-24.f*o->xVelF*/)*PbyT)-9.f*o->xPAcc)*PbyT);
    }
    else
    {        
        if (o->xVelF != 0.f) o->xPosF += EMS_PERIOD*o->xVelF;

        o->xVel = o->xVelF;        
        o->xPos = o->xPosF;
        o->xPAcc = 0.f;
    }
    
    if (o->VelTimer)
    {
        float PbyT = 1.0f/(float)(o->VelTimer--);

        o->vPos += EMS_PERIOD*(o->vVel += o->vPAcc += (6.0f*(o->vVelF-o->vVel)*PbyT-4.0f*o->vPAcc)*PbyT);

        *p = o->xPos  + o->vPos;
        *v = o->xVel  + o->vVel;
        *a = o->xPAcc + o->vPAcc;
    }
    else if (o->bVelocityMove)
    {
        o->vPAcc = 0.0f;
        
        o->vPos += EMS_PERIOD*(o->vVel = o->vVelF);

        *p = o->xPos + o->vPos;
        *v = o->xVel + o->vVel;
        
        *a = o->xPAcc;
    }
    else
    {
        if (o->bIsBraking) eo_trajectory_VelocityStop(o);

        *p = o->xPos;
        *v = o->xVel;
        
        *a = o->xPAcc;
    }
    
    if (*p <= o->pos_min)
    {
        if (*v < 0.f)
        {
            if (o->bVelocityMove || o->VelTimer) eo_trajectory_VelocityStop(o);
            
            o->PosTimer = 0;
            *v = o->xVel = o->xVelF = 0.f;
        }
        
        *a = 0.0f;
        
        return -1;
    }
    else if (*p >= o->pos_max)
    {
        if (*v > 0.f)
        {
            if (o->bVelocityMove || o->VelTimer) eo_trajectory_VelocityStop(o);
            
            o->PosTimer = 0;
            *v = o->xVel = o->xVelF = 0.f;
        }
                
        *a = 0.0f;
        
        return  1;
    }
    
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////

extern eObool_t eo_trajectory_IsDone(EOtrajectory* o)
{
    if (!o) return eobool_false;
    
    return !o->PosTimer;
}

extern int32_t eo_trajectory_GetPos(EOtrajectory* o)
{
    if (!o) return 0;
    
    return (int32_t)o->xPos;
}

extern int32_t eo_trajectory_GetVel(EOtrajectory* o)
{
    if (!o) return 0;
    
    return (int32_t)o->xVel;
}

extern int32_t eo_trajectory_GetAcc(EOtrajectory* o)
{
    if (!o) return 0;
    
    return (int32_t)(EMS_FREQUENCY_FLOAT*o->xPAcc);
}

#if 0
extern int8_t eo_trajectory_Step(EOtrajectory* o, float *p, float *v, float *a)
{
    if (!o) return;
    
    if (o->PosTimer)
    {
        float PbyT = 1.0f/(float)(o->PosTimer--);

        o->Pos += EMS_PERIOD*(o->Vel += o->PAcc += (((K60byP*(o->PosF-o->Pos)*PbyT-36.f*o->Vel /*-24.f*o->VelF*/)*PbyT)-9.f*o->PAcc)*PbyT);
    }
    else if (o->VelTimer)
    {
        float PbyT = 1.f/(float)(o->VelTimer--);

        o->PosF = o->Pos += EMS_PERIOD*(o->Vel += o->PAcc += (6.f*(o->VelF-o->Vel)*PbyT-4.f*o->PAcc)*PbyT);
    }
    else
    {        
        if (o->VelF != 0.f) o->PosF += EMS_PERIOD*o->VelF;

        o->Vel = o->VelF;        
        o->Pos = o->PosF;
        o->PAcc = 0.f;
    }
    
    if (o->boostTimer)
    {
        float PbyT = 1.0f/(float)(o->boostTimer--);

        o->boostPos += EMS_PERIOD*(o->boostVel += o->boostPAcc += (6.0f*(o->boostVelF-o->boostVel)*PbyT-4.0f*o->boostPAcc)*PbyT);

        *p = o->Pos + o->boostPos;
        *v = o->Vel + o->boostVel;
        
        *a = o->PAcc + o->boostPAcc;
    }
    else if (o->boost)
    {
        o->boostPAcc = 0.0f;
        
        o->boostPos += EMS_PERIOD*(o->boostVel = o->boostVelF);

        *p = o->Pos + o->boostPos;
        *v = o->Vel + o->boostVel;
        
        *a = o->PAcc;
    }
    else
    {
        if (o->boostIsBraking) eo_trajectory_BoostStop(o);

        *p = o->Pos;
        *v = o->Vel;
        
        *a = o->PAcc;
    }
    
    if (*p < o->pos_min)
    {
        if (o->boost || o->boostTimer) eo_trajectory_BoostStop(o);
          
        if (*v < 0.f)
        {
            o->PosTimer = 0;
            o->VelTimer = 0;
            *v = o->Vel = o->VelF = 0.f;
        }
        
        *a = 0.0f;
        
        return -1;
    }
    else if (*p > o->pos_max)
    {
        if (o->boost || o->boostTimer) eo_trajectory_BoostStop(o);
        
        if (*v > 0.f)
        {
            o->PosTimer = 0;
            o->VelTimer = 0;
            *v = o->Vel = o->VelF = 0.f;
        }
                
        *a = 0.0f;
        
        return  1;
    }
    
    return 0;
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
