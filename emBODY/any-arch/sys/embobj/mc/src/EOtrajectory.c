
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
        
        eo_trajectory_Init(o, 0, 0, 0);
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
    
    o->xX = p0;
    o->xV = v0;
    o->xA = a0;
    
    o->xTimer = o->xT = 0.0f;
    
    o->vX = 0.0f;
    o->vV = 0.0f;
    o->vA = 0.0f;
    
    o->vTimer = o->vT = 0.0f;
    
    //o->Xdebug = 0;
    //o->Vdebug = 0;
    
    o->bVelocityMove = eobool_false;
}


extern void eo_trajectory_SetPosRaw(EOtrajectory *o, int32_t p0)
{
    if (!o) return;
    
    LIMIT(o->pos_min, p0, o->pos_max)
    
    o->xX = p0;
    o->xV = 0.0f;
    o->xA = 0.0f;
    
    o->xTimer = o->xT = 0.0f;
    
    o->vX = 0.0f;
    o->vV = 0.0f;
    o->vA = 0.0f;
    
    o->vTimer = o->vT = 0.0f;
    
    //o->Xdebug = 0;
    //o->Vdebug = 0;
    
    o->bVelocityMove = eobool_false;
}

/*
extern void eo_trajectory_GetPosReference(EOtrajectory *o, int32_t *xStar, int32_t *velAvg)
{
    *xStar  = o->Xdebug;
    *velAvg = o->Vdebug;
}
*/

extern void eo_trajectory_SetPosReference(EOtrajectory *o, int32_t x0, int32_t xStar, int32_t velAvg)
{
    if (!o) return;
    
    if (o->bVelocityMove || (o->vTimer < o->vT)) eo_trajectory_VelocityStop(o);

    LIMIT(o->pos_min, xStar, o->pos_max)

    if (!velAvg) velAvg = o->vel_max;
    
    LIMIT(-o->vel_max, velAvg, o->vel_max)
    
    ///////////
    o->xX = x0;
    ///////////
    
    float D = (float)xStar - o->xX;
    
    if ((velAvg>0) ^ (D>0.0f)) velAvg = -velAvg;
    
    float V = (float)(2*velAvg);

    o->xT = D/V;
    
    //o->Xdebug = xStar;
    //o->Vdebug = velAvg;
    
    if (o->xT < EMS_PERIOD)
    {
        o->xTimer = o->xT = 0.0f;
        
        o->xX = (float)xStar;
		
		o->xV = o->xA = 0.0f;
        
        //o->Vdebug = 0;
        
        return;
    }
    
    o->xTimer = -o->xT;
    
    o->xInvT = 1.0f/o->xT;
    
    o->xK2 =  0.75f*(o->xV*o->xInvT+o->xA);
    o->xK3 = -3.75f*(o->xV-V)*o->xInvT-1.25f*o->xA;
    float O5A = 0.5f*o->xA;
    o->xK0 = -o->xK2+O5A;
    o->xK1 = -o->xK3-O5A;
}

extern void eo_trajectory_SetVelReference(EOtrajectory *o, int32_t vStar, int32_t accAvg)
{
    if (!o) return;
    
    o->bVelocityMove = eobool_true;

    LIMIT(-o->vel_max, vStar, o->vel_max)
    
    if (!accAvg) accAvg = o->acc_max;
    
    LIMIT(-o->acc_max, accAvg, o->acc_max)

    float D = (float)vStar - o->vV;
    
    if ((accAvg>0) ^ (D>0.0f)) accAvg=-accAvg;

    float A = (float)(2*accAvg);

    o->vT = D/A;

    if (o->vT < EMS_PERIOD)
    {
        o->vTimer = o->vT = 0.0f;
        
        o->vV = (float)vStar;
		
		o->vA = 0.0f;  
        
        return;
    }
    
    o->vTimer = -o->vT;
    o->vInvT  = 1.0f/o->vT;
    
    o->vK1 = -0.5f*o->vA;
    o->vK0 = 0.75f*A+0.5f*o->vK1;
    o->vK2 = -o->vK0-o->vK1;
}

extern void eo_trajectory_Stop(EOtrajectory *o, int32_t pos, int32_t stop_acc)
{
    eo_trajectory_SetPosRaw(o, pos);
 
    /*
    if (!o) return;
    
    if (o->bVelocityMove || (o->vTimer < o->vT)) eo_trajectory_VelocityStop(o);

    o->xX = (float)pos;
    
    LIMIT(o->pos_min, o->xX, o->pos_max)
    
    if (!stop_acc) stop_acc = o->acc_max;

    o->xT = o->xV/stop_acc;
    
    if (o->xT < 0.0f) o->xT = -o->xT;
    
    if (o->xT < EMS_PERIOD)
    {
        o->xTimer = o->xT = 0.0f;
		
		o->xV = o->xA = 0.0f;
        
        return;
    }
    
    o->xTimer = -o->xT;
    o->xInvT  = 1.0f/o->xT;
    
    float ViT = o->xV*o->xInvT;
    o->xK2 = 0.75f*(ViT+o->xA);
    o->xK3 =-3.75f*ViT-1.25f*o->xA;
    float O5A = 0.5f*o->xA;
    o->xK0 = -o->xK2+O5A;
    o->xK1 = -o->xK3-O5A;
    */
}

extern void eo_trajectory_VelocityStop(EOtrajectory *o)
{
    if (!o) return;
    
    o->bVelocityMove = eobool_false;
    
    o->vTimer = o->vT = 0.0f;

    o->xX += o->vX;
    //o->xV += o->vV;
    //o->xA += o->vA;

    o->vX = o->vV = o->vA = 0.0f;    
}

//////////////////////////////////////////////////////////////////////////////////

extern int8_t eo_trajectory_Step(EOtrajectory* o, float *p, float *v, float *a)
{
    if (!o) return 0;
    
    if (o->xTimer < o->xT)
    {
        float t = o->xTimer*o->xInvT;
        
        o->xA = (((o->xK3*t+o->xK2)*t+o->xK1)*t)+o->xK0;
        o->xV += EMS_PERIOD*o->xA;
        o->xX += EMS_PERIOD*o->xV;
        
        o->xTimer += EMS_PERIOD;
    }
    else
    {
        o->xV = o->xA = 0.0f;
    }
    
    if (o->vTimer < o->vT)
    {
        float t = o->vTimer*o->vInvT;
        
        o->vA = ((o->vK2*t+o->vK1)*t)+o->vK0;
        o->vV += EMS_PERIOD*o->vA;
        o->vX += EMS_PERIOD*o->vV;

        *p = o->xX + o->vX;
        *v = o->xV + o->vV;
        *a = o->xA + o->vA;
        
        o->vTimer += EMS_PERIOD;
    }
    else if (o->bVelocityMove)
    {
        o->vA = 0.0f;
        o->vX += EMS_PERIOD*o->vV;

        *p = o->xX + o->vX;
        *v = o->xV + o->vV;
        *a = o->xA;
    }
    else
    {
        *p = o->xX;
        *v = o->xV;
        *a = o->xA;
    }
    
    if (*p <= o->pos_min)
    {
        if (*v < 0.0f)
        {
            if (o->bVelocityMove || (o->vTimer < o->vT)) eo_trajectory_VelocityStop(o);

            *v = 0.0f;
        }
		
		if (*a < 0.0f) *a = 0.0f;
        
        *p = o->pos_min;
        
        return -1;
    }
    else if (*p >= o->pos_max)
    {
        if (*v > 0.0f)
        {
            if (o->bVelocityMove || (o->vTimer < o->vT)) eo_trajectory_VelocityStop(o);
          
            *v = 0.0f;
        }
		
		if (*a > 0.0f) *a = 0.0f;
             
        *p = o->pos_max;
        
        return  1;
    }
    
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////

extern eObool_t eo_trajectory_IsDone(EOtrajectory* o)
{
    if (!o) return eobool_false;
    
    return o->xTimer >= o->xT;
}

extern int32_t eo_trajectory_GetPos(EOtrajectory* o)
{
    if (!o) return 0;
    
    return (int32_t)(o->xX + o->vX);
}

extern int32_t eo_trajectory_GetVel(EOtrajectory* o)
{
    if (!o) return 0;
    
    return (int32_t)(o->xV + o->vV);
}

extern int32_t eo_trajectory_GetAcc(EOtrajectory* o)
{
    if (!o) return 0;
    
    return (int32_t)(o->xA + o->vA);
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
