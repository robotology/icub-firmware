
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

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOtrajectory* eo_trajectory_New(void) 
{
    EOtrajectory *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtrajectory), 1);

    if (o)
    {
        o->Pos = 0.0f;
        o->Vel = 0.0f;
        o->PAcc = 0.0f;
        o->P2Jerk = 0.0f;
        o->P3Snap = 0.0f;
        o->P4Crackle = 0.0f;

        //o->biPAcc = 0.0f;
        //o->biP2Jerk = 0.0f;

        o->PosF = 0.0f;
        o->VelF = 0.0f;

        o->pos_steps_to_end = 0;
        o->vel_steps_to_end = 0;

        o->pos_min =    0;
        o->pos_max = 4095;
        o->vel_max = 2048;

        o->acc_stop_hybrid = 4096;

        o->hybrid = eobool_false;
        o->hybridVel = 0.0f;
        o->hybridPAcc = 0.0f;
        o->hybridP2Jerk = 0.0f;
        o->hybridP3Snap = 0.0f;
        o->hybrid_steps_to_end = 0;
    }

    return o;
}

extern void eo_trajectory_SetLimits(EOtrajectory *o, int32_t pos_min, int32_t pos_max, int32_t vel_max)
{
    o->pos_min = pos_min;
    o->pos_max = pos_max;
    o->vel_max = vel_max;
}

extern void eo_trajectory_Init(EOtrajectory *o, int32_t p0, int32_t v0, int32_t a0)
{
    o->Pos = p0;
    o->Vel = v0;
    o->PAcc = a0;
}

extern void eo_trajectory_SetPosReference(EOtrajectory *o, int32_t p1, int32_t avg_vel)
{
    if (o->hybrid || o->hybrid_steps_to_end)
    { 
        eo_trajectory_StopHybrid(o);
    }

    o->vel_steps_to_end = 0;
    o->VelF = 0.0f;

    o->PosF = (float)p1;

    LIMIT(o->pos_min, o->PosF, o->pos_max)

    if (!avg_vel)
    {
        o->pos_steps_to_end = 0;
        return;    
    }

    float D = o->PosF-o->Pos;

    if (D == 0.0f)
    {
        o->pos_steps_to_end = 0;
        return;
    }

    float Va = (float)avg_vel;

    if ((D<0.0f) ^ (Va<0.0f)) Va = -Va;

    float T = D/Va;

    o->pos_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);
    
    float PbyT = EMS_PERIOD/T;    
    
    Va*=PbyT; 
    float A3 =  120.0f*Va;
    float A2 = -180.0f*Va;
    float A1 =   60.0f*Va;

    if (o->Vel != 0.0f)
    {
        float PVbyT = o->Vel*PbyT;

        A3 -= 60.0f*PVbyT;
        A2 += 96.0f*PVbyT;
        A1 -= 36.0f*PVbyT;
    }

    if (o->PAcc != 0.0f)
    {
        A3 -= 10.0f*o->PAcc;
        A2 += 18.0f*o->PAcc;
        A1 -=  9.0f*o->PAcc;
    }

    A3 *= PbyT*PbyT*PbyT;
    A2 *= PbyT*PbyT;
    A1 *= PbyT;

    o->P4Crackle = 6.0f*A3;
    o->P3Snap    = 2.0f*A2+o->P4Crackle;
    o->P2Jerk    = A1+A2+A3;
}

/*
extern void eo_trajectory_SetPosReference(EOtrajectory *o, int32_t p1, int32_t avg_vel)
{
    //if (o->hybrid) 
    eo_trajectory_StopHybrid(o);

    o->vel_steps_to_end = 0;
    o->VelF = 0.0f;

    o->PosF = (float)p1;

    LIMIT(o->pos_min, o->PosF, o->pos_max)

    if (!avg_vel)
    {
        o->pos_steps_to_end = 0;
        return;    
    }

    float D = o->PosF-o->Pos;

    if (D == 0.0f)
    {
        o->pos_steps_to_end = 0;
        return;
    }

    float Va = (float)avg_vel;

    if ((D<0.0f) ^ (Va<0.0f)) Va = -Va;

    float T = D/Va;

    o->pos_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);
    
    float PbyT = EMS_PERIOD/T;    
    
    Va*=PbyT; 
    float A3 =   80.0f*Va;
    float A2 = -120.0f*Va;
    float A1 =   40.0f*Va;

    static const float Fby3  =  4.0f/3.0f;
    static const float TWby3 = 20.0f/3.0f;
    o->biP2Jerk = -4.0f*Va;
    o->biPAcc   =  2.0f*Va;

    if (o->Vel != 0.0f)
    {
        float PVbyT = o->Vel*PbyT;

        A3 -= 40.0f*PVbyT;
        A2 += 64.0f*PVbyT;
        A1 -= 24.0f*PVbyT;

        o->biP2Jerk += +2.0f*PVbyT;
        o->biPAcc   += -Fby3*PVbyT;
    }

    o->biP2Jerk *= PbyT;

    if (o->PAcc != 0.0f)
    {
        A3 -= TWby3*o->PAcc;
        A2 += 12.0f*o->PAcc;
        A1 -=  6.0f*o->PAcc;
    }

    A3 *= PbyT*PbyT*PbyT;
    A2 *= PbyT*PbyT;
    A1 *= PbyT;

    o->P4Crackle = 6.0f*A3;
    o->P3Snap    = 2.0f*A2+o->P4Crackle;
    o->P2Jerk    = A1+A2+A3;
}
*/

extern void eo_trajectory_Stop(EOtrajectory *o, int32_t stop_acc)
{
    if (o->hybrid || o->hybrid_steps_to_end)
    { 
        eo_trajectory_StopHybrid(o);
    }

    o->pos_steps_to_end = 0;
    o->vel_steps_to_end = 0;

    o->PosF = o->Pos;
    o->VelF = 0.0f;

    if (!stop_acc) return;

    float T = o->Vel/(float)stop_acc;

    if (T < 0.0f) T = -T;

    o->pos_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);
    
    float PbyT = EMS_PERIOD/T;

    float A3 = 0.0f;
    float A2 = 0.0f;
    float A1 = 0.0f;

    if (o->Vel != 0.0f)
    {
        float PVbyT = o->Vel*PbyT;

        A3 -= 60.0f*PVbyT;
        A2 += 96.0f*PVbyT;
        A1 -= 36.0f*PVbyT;
    }

    if (o->PAcc != 0.0f)
    {
        A3 -= 10.0f*o->PAcc;
        A2 += 18.0f*o->PAcc;
        A1 -=  9.0f*o->PAcc;
    } 

    A3 *= PbyT*PbyT*PbyT;
    A2 *= PbyT*PbyT;
    A1 *= PbyT;

    o->P4Crackle = 6.0f*A3;
    o->P3Snap    = 2.0f*A2+o->P4Crackle;
    o->P2Jerk    = A1+A2+A3;

    //o->biP2Jerk = 0.0f;
    //o->biPAcc   = 0.0f;
}

extern void eo_trajectory_SetVelReference(EOtrajectory *o, int32_t v1, int32_t avg_acc)
{
    if (o->hybrid || o->hybrid_steps_to_end)
    { 
        eo_trajectory_StopHybrid(o);
    }

    o->pos_steps_to_end = 0;

    o->VelF = (float)v1;

    LIMIT(-o->vel_max, o->VelF, o->vel_max)

    if (!avg_acc)
    {
        o->vel_steps_to_end = 0;
        return;
    }

    float D = o->VelF-o->Vel;

    if (D == 0.0f)
    {
        o->vel_steps_to_end = 0;
        return;
    }
   
    float Aa = (float)avg_acc;

    if ((D<0.0f) ^ (Aa<0.0f)) Aa = -Aa;

    float T = D/Aa;   
   
    o->vel_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);

    float PbyT = EMS_PERIOD/T;
    Aa *= EMS_PERIOD;

    o->P4Crackle = 0.0f;
    o->P3Snap = -12.0f*Aa;
    o->P2Jerk =   6.0f*Aa;

    if (o->PAcc != 0.0f)
    {
        o->P3Snap += 6.0f*o->PAcc;
        o->P2Jerk -= 4.0f*o->PAcc;
    }

    o->P3Snap *= PbyT*PbyT;
    o->P2Jerk *= PbyT;
    o->P2Jerk += 0.5f*o->P3Snap;
}

/*
extern void eo_trajectory_SetVelReference(EOtrajectory *o, int32_t v1, int32_t avg_acc)
{
    if (o->hybrid || o->hybrid_steps_to_end)
    { 
        eo_trajectory_StopHybrid(o);
    }

    o->pos_steps_to_end = 0;

    o->VelF = (float)v1;

    LIMIT(-o->vel_max, o->VelF, o->vel_max)

    if (!avg_acc)
    {
        o->vel_steps_to_end = 0;
        return;
    }

    float D = o->VelF-o->Vel;

    if (D == 0.0f)
    {
        o->vel_steps_to_end = 0;
        return;
    }
   
    float Aa = (float)avg_acc;

    if ((D<0.0f) ^ (Aa<0.0f)) Aa = -Aa;

    float T = D/Aa;   
   
    o->vel_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);

    float PbyT = EMS_PERIOD/T;
    Aa *= EMS_PERIOD;

    o->P4Crackle = 0.0f;
    o->P3Snap = -8.0f*Aa;
    o->P2Jerk =  4.0f*Aa;

    static const float lby3 = 1.0f/3.0f;
    static const float Eby3 = 8.0f/3.0f;

    if (o->PAcc != 0.0f)
    {
        o->P3Snap += 4.0f*o->PAcc;
        o->P2Jerk -= Eby3*o->PAcc;
    }

    o->P3Snap *= PbyT*PbyT;
    o->P2Jerk *= PbyT;
    o->P2Jerk += 0.5f*o->P3Snap;

    o->biP2Jerk = 0.0f;
    o->biPAcc   = lby3*Aa;
}
*/

extern void eo_trajectory_AddVelHybrid(EOtrajectory *o, int32_t v1, int32_t avg_acc)
{
    o->hybrid = eobool_true;

    float VelF = (float)v1;

    LIMIT(-o->vel_max, VelF, o->vel_max)

    if (!avg_acc)
    {
        o->hybrid_steps_to_end = 0;
        o->hybridVel = VelF;
        return;
    }

    float D = VelF - o->hybridVel;

    if (D == 0.0f)
    {
        o->hybrid_steps_to_end = 0;
        return;
    }
   
    float Aa = (float)avg_acc;

    if ((D<0.0f) ^ (Aa<0.0f)) Aa = -Aa;

    float T = D/Aa;   
   
    o->hybrid_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);

    float PbyT = EMS_PERIOD/T;
    Aa *= EMS_PERIOD;

    o->hybridP3Snap = (-12.0f*Aa+6.0f*o->hybridPAcc)*PbyT*PbyT;
    o->hybridP2Jerk = (  6.0f*Aa-4.0f*o->hybridPAcc)*PbyT+0.5f*o->hybridP3Snap;
}

extern void eo_trajectory_StopHybrid(EOtrajectory *o)
{
    o->hybrid = eobool_false;
    o->hybrid_steps_to_end = 0;

    o->Vel += o->hybridVel;
    o->PAcc += o->hybridPAcc;

    o->hybridVel = 0.0f;
    o->hybridPAcc = 0.0f;
}

extern void eo_trajectory_TimeoutVelReference(EOtrajectory *o)
{
    if (o->hybrid)
    {
        eo_trajectory_AddVelHybrid(o, 0, o->acc_stop_hybrid);
        o->hybrid = eobool_false;
    }
}

extern eObool_t eo_trajectory_IsDone(EOtrajectory* o)
{
    return !o->vel_steps_to_end && !o->pos_steps_to_end;
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
    return (int32_t)(EMS_FFREQUENCY*o->PAcc);
}

extern int8_t eo_trajectory_Step(EOtrajectory* o, float *p, float *v)
{
    if (o->pos_steps_to_end)
    {
        --o->pos_steps_to_end;

        o->PAcc += o->P2Jerk;
        o->P2Jerk += o->P3Snap;
        o->P3Snap += o->P4Crackle;

        //o->biPAcc += o->biP2Jerk;
        
        o->Vel += o->PAcc;//+o->biPAcc;
        o->Pos += EMS_PERIOD*o->Vel;   
    }
    else if (o->vel_steps_to_end)
    {
        --o->vel_steps_to_end;

        o->PAcc   += o->P2Jerk;
        o->P2Jerk += o->P3Snap;

        o->Vel += o->PAcc;//+o->biPAcc;
        o->Pos += EMS_PERIOD*o->Vel;

        o->PosF = o->Pos; 
    }
    else
    {
        if (o->VelF != 0.0f) o->PosF += EMS_PERIOD*o->VelF;

        o->Vel = o->VelF;        
        o->Pos = o->PosF;
        o->PAcc = 0.0f;
    }

    if (o->hybrid_steps_to_end)
    {
        --o->hybrid_steps_to_end;

        o->hybridPAcc += o->hybridP2Jerk;
        o->hybridP2Jerk += o->hybridP3Snap;

        o->hybridVel += o->hybridPAcc;

        float dP = EMS_PERIOD*o->hybridVel;
        o->Pos += dP;
        o->PosF += dP;
    }
    else if (o->hybrid)
    {
        o->hybridPAcc = 0.0f;

        float dP = EMS_PERIOD*o->hybridVel;
        o->Pos += dP;
        o->PosF += dP;
    }
    else
    {
        o->hybridVel  = 0.0f;
        o->hybridPAcc = 0.0f;
    }
        
    if (o->Pos < o->pos_min)
    {
        if (o->hybrid && o->hybridVel < 0.0f) eo_trajectory_StopHybrid(o);

        o->pos_steps_to_end = 0;
        o->vel_steps_to_end = 0;

        o->PosF = o->Pos = o->pos_min;
        o->VelF = o->Vel = o->PAcc = 0.0f;

        *p = (int32_t)o->Pos;
        *v = 0;

        return -1;
    }
    
    if (o->Pos > o->pos_max)
    {
        if (o->hybrid && o->hybridVel > 0.0f) eo_trajectory_StopHybrid(o);

        o->pos_steps_to_end = 0;
        o->vel_steps_to_end = 0;

        o->PosF = o->Pos = o->pos_max;
        o->VelF = o->Vel = o->PAcc = 0.0f;

        *p = (int32_t)o->Pos;
        *v = 0;

        return  1;
    }

    *p = (int32_t)o->Pos;
    *v = (int32_t)(o->Vel+o->hybridVel);

    return 0;
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
