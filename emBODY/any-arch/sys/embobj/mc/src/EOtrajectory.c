
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

extern const float   EMS_FREQUENCY_FLOAT;
extern const int32_t EMS_FREQUENCY_INT32;
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

#define LIMIT(min, x, max) if (x < (min)) x = (min); else if (x > (max)) x = (max); 

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

static float K60byP = 0.0f;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOtrajectory* eo_trajectory_New(int32_t ticks_per_rev) 
{
    EOtrajectory *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtrajectory), 1);

    if (o)
    {
        o->Pos  = 0.0f;
        o->Vel  = 0.0f;
        o->PAcc = 0.0f;

        o->PosF = 0.0f;
        o->VelF = 0.0f;

        o->PosTimer = 0;
        o->VelTimer = 0;

        o->pos_min        = 0;
        o->pos_max        = ticks_per_rev;
        o->vel_max        = ticks_per_rev/2;
        o->acc_stop_boost = ticks_per_rev;

        // boost
        o->boost          = eobool_false;
        o->boostIsBraking = eobool_false;
         
        o->boostPos  = 0.0f;
        o->boostVel  = 0.0f;
        o->boostVelF = 0.0f;
        o->boostPAcc = 0.0f;
        o->boostTimer = 0;
    }

    if (K60byP == 0.0f) K60byP = 60.0f*EMS_FREQUENCY_FLOAT;

    return o;
}

extern void eo_trajectory_SetPosMin(EOtrajectory *o, int32_t pos_min)
{
    o->pos_min = (float)pos_min;
}

extern void eo_trajectory_SetPosMax(EOtrajectory *o, int32_t pos_max)
{
    o->pos_max = (float)pos_max;
}

extern void eo_trajectory_SetVelMax(EOtrajectory *o, int32_t vel_max)
{
    o->vel_max = vel_max;
}

extern void eo_trajectory_Init(EOtrajectory *o, int32_t p0, int32_t v0, int32_t a0)
{
    o->Pos  = p0;
    o->Vel  = v0;
    o->PAcc = EMS_PERIOD*a0;

    o->PosTimer = 0;
    o->VelTimer = 0;

    o->boostTimer = 0;
    o->boost          = eobool_false;
    o->boostIsBraking = eobool_false;
    o->boostPos  = 0.0f;
    o->boostVel  = 0.0f;
    o->boostVelF = 0.0f;
    o->boostPAcc = 0.0f;
}

extern void eo_trajectory_SetPosReference(EOtrajectory *o, int32_t p1, int32_t avg_vel)
{
    if (o->boost || o->boostTimer) eo_trajectory_BoostStop(o);

    LIMIT(o->pos_min, p1, o->pos_max)

    o->PosF = (float)p1;
    o->VelF = 0.0f;
    
    o->VelTimer = 0;

    if (!avg_vel)
    {
        o->Pos = o->PosF;
        o->PosTimer = 0;
        
        return;
    }

    o->PosTimer = (EMS_FREQUENCY_INT32*(p1-(int32_t)o->Pos))/avg_vel;

    if (o->PosTimer<0) o->PosTimer = -o->PosTimer;
}

extern void eo_trajectory_Stop(EOtrajectory *o, int32_t stop_acc)
{
    if (o->boost || o->boostTimer) eo_trajectory_BoostStop(o);

    o->PosTimer = 0;
    o->VelTimer = 0;

    o->PosF = o->Pos;
    o->VelF = 0.0f;

    if (!stop_acc) return;

    o->PosTimer = (EMS_FREQUENCY_INT32*(int32_t)o->Vel)/stop_acc;

    if (o->PosTimer<0) o->PosTimer = -o->PosTimer;
}

extern void eo_trajectory_SetVelReference(EOtrajectory *o, int32_t v1, int32_t avg_acc)
{
    if (o->boost || o->boostTimer) eo_trajectory_BoostStop(o);

    LIMIT(-o->vel_max, v1, o->vel_max)

    o->VelF = (float)v1;

    o->PosTimer = 0;

    if (!avg_acc)
    {
        o->VelTimer = 0;
        
        return;
    }

    o->VelTimer = (EMS_FREQUENCY_INT32*(v1-(int32_t)o->Vel))/avg_acc;

    if (o->VelTimer<0) o->VelTimer = -o->VelTimer;
}

extern void eo_trajectory_BoostStart(EOtrajectory *o, int32_t v1, int32_t avg_acc)
{
    o->boost = eobool_true;
    o->boostIsBraking = eobool_false;

    LIMIT(-o->vel_max, v1, o->vel_max)

    o->boostVelF = (float)v1;

    o->boostTimer = 0;

    if (!avg_acc)
    {
        o->boostVel = o->boostVelF;

        return;
    }

    o->boostTimer = (EMS_FREQUENCY_INT32*(v1-(int32_t)o->boostVel))/avg_acc;

    if (o->boostTimer<0) o->boostTimer = -o->boostTimer;
}

extern void eo_trajectory_BoostStop(EOtrajectory *o)
{
    o->boostIsBraking = eobool_false;
    o->boost = eobool_false;
    o->boostTimer = 0;

    o->Pos  += o->boostPos;
    o->PosF += o->boostPos;
    o->Vel  += o->boostVel;
    o->PAcc += o->boostPAcc;

    o->boostPos  = 0.0f;
    o->boostVel  = 0.0f;
    o->boostVelF = 0.0f;
    o->boostPAcc = 0.0f;    
}

extern void eo_trajectory_BoostTimeout(EOtrajectory *o)
{
    if (o->boost)
    {
        o->boost = eobool_false;
        o->boostIsBraking = eobool_true;

        eo_trajectory_BoostStart(o, 0, o->acc_stop_boost);
    }
}

extern int8_t eo_trajectory_PosStep(EOtrajectory* o, float *p, float *v)
{
    if (o->PosTimer)
    {
        float PbyT = 1.0f/(float)(o->PosTimer--);

        o->Pos += EMS_PERIOD*(o->Vel += o->PAcc += (((K60byP*(o->PosF-o->Pos)*PbyT-36.0f*o->Vel)*PbyT)-9.0f*o->PAcc)*PbyT);
    }
    else if (o->VelTimer)
    {
        float PbyT = 1.0f/(float)(o->VelTimer--);

        o->PosF = o->Pos += EMS_PERIOD*(o->Vel += o->PAcc += (6.0f*(o->VelF-o->Vel)*PbyT-4.0f*o->PAcc)*PbyT);
    }
    else
    {
        if (o->VelF != 0.0f) o->PosF += EMS_PERIOD*o->VelF;

        o->Vel = o->VelF;        
        o->Pos = o->PosF;
        o->PAcc = 0.0f;
    }

    if (o->boostTimer)
    {
        float PbyT = 1.0f/(float)(o->boostTimer--);

        o->boostPos += EMS_PERIOD*(o->boostVel += o->boostPAcc += (6.0f*(o->boostVelF-o->boostVel)*PbyT-4.0f*o->boostPAcc)*PbyT);

        *p = o->Pos + o->boostPos;
        *v = o->Vel + o->boostVel;
    }
    else if (o->boost)
    {
        o->boostPAcc = 0.0f;
        
        o->boostPos += EMS_PERIOD*(o->boostVel = o->boostVelF);

        *p = o->Pos + o->boostPos;
        *v = o->Vel + o->boostVel;
    }
    else
    {
        if (o->boostIsBraking) eo_trajectory_BoostStop(o);

        *p = o->Pos;
        *v = o->Vel;
    }

    if (*p < o->pos_min)
    {
        if (o->boost || o->boostTimer) eo_trajectory_BoostStop(o);

        *p = o->Pos = o->pos_min;
        if (*v < 0.0f) *v = o->Vel = 0.0f;
        
        return -1;
    }

    if (*p > o->pos_max)
    {
        if (o->boost || o->boostTimer) eo_trajectory_BoostStop(o);

        *p = o->Pos = o->pos_max;
        if (*v > 0.0f) *v = o->Vel = 0.0f;
                
        return  1;
    }

    return 0;
}

extern eObool_t eo_trajectory_IsDone(EOtrajectory* o)
{
    return !o->PosTimer && !o->VelTimer;
}

extern int32_t eo_trajectory_GetPos(EOtrajectory* o)
{
    return (int32_t)o->Pos;
}

extern int32_t eo_trajectory_GetVel(EOtrajectory* o)
{
    return (int32_t)o->Vel;
}

extern int32_t eo_trajectory_GetAcc(EOtrajectory* o)
{
    return (int32_t)(EMS_FREQUENCY_FLOAT*o->PAcc);
}


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
