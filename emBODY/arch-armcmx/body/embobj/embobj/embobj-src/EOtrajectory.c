
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
        o->Acc = 0.0f;
        o->Jerk = 0.0f;
        o->Snap = 0.0f;
        o->Crackle = 0.0f;

        o->biAcc = 0.0f;
        o->biJerk = 0.0f;

        o->PosF = 0.0f;
        o->VelF = 0.0f;

        o->pos_steps_to_end = 0;
        o->vel_steps_to_end = 0;

        o->pos_min =    0;
        o->pos_max = 4095;
        o->vel_max = 2048;

        o->acc_stop_hybrid = 4096;
        //o->acc_stop_cmode = 4096;
        //o->acc_stop_alarm = 8192;

        o->hybrid = eobool_false;
        o->hybridVel = 0.0f;
        o->hybridAcc = 0.0f;
        o->hybridSnap = 0.0f;
        o->hybridJerk = 0.0f;
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
    o->Acc = a0;
}

extern void eo_trajectory_SetPosReference(EOtrajectory *o, int32_t p1, int32_t avg_vel)
{
    if (o->hybrid) eo_trajectory_StopHybrid(o);

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

    float Va = (float)avg_vel;

    if ((D<0.0f) ^ (Va<0.0f)) Va = -Va;

    float T = D/Va;

    o->pos_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);
    
    float lbyT = 1.0f/T;
    float PbyT = EMS_PERIOD*lbyT;

    float DbyT2 =    Va*lbyT;
    float VbyT  = o->Vel*lbyT; 

    //float A3 = ( 120.0f*DbyT2-60.0f*VbyT-10.0f*o->Acc)*PbyT*PbyT*PbyT;
    //float A2 = (-180.0f*DbyT2+96.0f*VbyT+18.0f*o->Acc)*PbyT*PbyT;

    //o->Crackle = 6.0f*A3;
    //o->Snap = 2.0f*A2+o->Crackle;
    //o->Jerk = (60.0f*DbyT2-36.0f*VbyT-9.0f*o->Acc)*PbyT+A2+A3;

    static const float Fby3  =  4.0f/3.0f;
    static const float TWby3 = 20.0f/3.0f;

    float A3 = (  80.0f*DbyT2-40.0f*VbyT-TWby3*o->Acc)*PbyT*PbyT*PbyT;
    float A2 = (-120.0f*DbyT2+64.0f*VbyT+12.0f*o->Acc)*PbyT*PbyT;

    o->Crackle = 6.0f*A3;
    o->Snap = 2.0f*A2+o->Crackle;
    o->Jerk = (40.0f*DbyT2-24.0f*VbyT-6.0f*o->Acc)*PbyT+A2+A3;

    o->biJerk = (-4.0f*DbyT2+2.0f*VbyT)*PbyT;
    o->biAcc  = ( 2.0f*DbyT2-Fby3*VbyT);
}

extern void eo_trajectory_Stop(EOtrajectory *o, int32_t stop_acc)
{
    if (o->hybrid) eo_trajectory_StopHybrid(o);

    o->pos_steps_to_end = 0;
    o->vel_steps_to_end = 0;

    o->PosF = o->Pos;
    o->VelF = 0.0f;

    if (!stop_acc) return;

    float T = o->Vel/(float)stop_acc;

    if (T < 0.0f) T = -T;

    o->pos_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);
    
    float lbyT = 1.0f/T;
    float PbyT = EMS_PERIOD*lbyT;

    float VbyT  = o->Vel*lbyT; 

    float A3 = (-60.0f*VbyT-10.0f*o->Acc)*PbyT*PbyT*PbyT;
    float A2 = (+96.0f*VbyT+18.0f*o->Acc)*PbyT*PbyT;

    o->Crackle = 6.0f*A3;
    o->Snap = 2.0f*A2+o->Crackle;
    o->Jerk = (-36.0f*VbyT-9.0f*o->Acc)*PbyT+A2+A3;

    o->biJerk = 0.0f;
    o->biAcc  = 0.0f;
}

extern void eo_trajectory_SetVelReference(EOtrajectory *o, int32_t v1, int32_t avg_acc)
{
    if (o->hybrid) eo_trajectory_StopHybrid(o);

    o->pos_steps_to_end = 0;

    o->VelF = (float)v1;

    LIMIT(-o->vel_max, o->VelF, o->vel_max)

    if (!avg_acc)
    {
        o->vel_steps_to_end = 0;
       
        return;
    }

    float D = o->VelF-o->Vel;
   
    float Aa = (float)avg_acc;

    if ((D<0.0f) ^ (Aa<0.0f)) Aa = -Aa;

    float T = D/Aa;   
   
    o->vel_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);

    float PbyT = EMS_PERIOD/T;

    o->Crackle = 0.0f;
    //o->Snap = (-12.0f*Aa+6.0f*o->Acc)*PbyT*PbyT;
    //o->Jerk = (  6.0f*Aa-4.0f*o->Acc)*PbyT+0.5f*o->Snap;

    static const float lby3 = 1.0f/3.0f;
    static const float Fby3 = 4.0f/3.0f;

    o->Snap = (-8.0f*Aa+4.0f*o->Acc)*PbyT*PbyT;
    o->Jerk = ( 4.0f*Aa-Fby3*o->Acc)*PbyT+0.5f*o->Snap;
    
    o->biJerk = 0.0f;
    o->biAcc  = lby3*Aa;
}

extern void eo_trajectory_AddVelHybrid(EOtrajectory *o, int32_t v1, int32_t avg_acc)
{
    o->hybrid = eobool_true;

    float VelF = (float)v1;

    LIMIT(-o->vel_max, VelF, o->vel_max)

    if (!avg_acc)
    {
        o->vel_steps_to_end = 0;
       
        return;
    }

    float D = VelF - o->hybridVel;
   
    float Aa = (float)avg_acc;

    if ((D<0.0f) ^ (Aa<0.0f)) Aa = -Aa;

    float T = D/Aa;   
   
    o->hybrid_steps_to_end = (int32_t)(T*EMS_FFREQUENCY);

    float PbyT = EMS_PERIOD/T;

    o->hybridSnap = (-12.0f*Aa+6.0f*o->hybridAcc)*PbyT*PbyT;
    o->hybridJerk = (  6.0f*Aa-4.0f*o->hybridAcc)*PbyT+0.5f*o->hybridSnap;
}

extern void eo_trajectory_StopHybrid(EOtrajectory *o)
{
    o->hybrid = eobool_false;
    o->hybrid_steps_to_end = 0;

    o->Vel += o->hybridVel;
    o->Acc += o->hybridAcc;

    o->hybridVel = 0.0f;
    o->hybridAcc = 0.0f;
}

extern void eo_trajectory_TimeoutVelReference(EOtrajectory *o)
{
    if (o->hybrid)
    {
        o->hybrid = eobool_false;
        eo_trajectory_AddVelHybrid(o, 0, o->acc_stop_hybrid);
    }
}

/*
extern void eo_trajectory_Stop(EOtrajectory *o)
{
    if (o->hybrid) eo_trajectory_StopHybrid(o);

    o->pos_steps_to_end = 0;
    o->vel_steps_to_end = 0;

    o->PosF = o->Pos;
    o->VelF = 0.0f;
}
*/

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
    return (int32_t)o->Acc;
}

extern int8_t eo_trajectory_Step(EOtrajectory* o, float *p, float *v)
{
    if (o->pos_steps_to_end)
    {
        --o->pos_steps_to_end;

        o->Acc  += o->Jerk;
        o->Jerk += o->Snap;
        o->Snap += o->Crackle;

        o->biAcc += o->biJerk;
        
        o->Vel += EMS_PERIOD*(o->Acc+o->biAcc);
        o->Pos += EMS_PERIOD*o->Vel;   
    }
    else if (o->vel_steps_to_end)
    {
        --o->vel_steps_to_end;

        o->Acc  += o->Jerk;
        o->Jerk += o->Snap;

        o->Vel += EMS_PERIOD*(o->Acc+o->biAcc);
        o->Pos += EMS_PERIOD*o->Vel;

        o->PosF = o->Pos; 
    }
    else
    {
        if (o->VelF != 0.0f) o->PosF += EMS_PERIOD*o->VelF;

        o->Vel = o->VelF;        
        o->Pos = o->PosF;
        o->Acc = 0.0f;
    }

    if (o->hybrid_steps_to_end)
    {
        --o->hybrid_steps_to_end;

        o->hybridAcc += o->hybridJerk;
        o->hybridJerk += o->hybridSnap;
        o->hybridVel += EMS_PERIOD*o->hybridAcc;

        float dP = EMS_PERIOD*o->hybridVel;
        o->Pos += dP;
        o->PosF += dP;
    }
    else if (o->hybrid)
    {
        o->hybridAcc = 0.0f;

        float dP = EMS_PERIOD*o->hybridVel;
        o->Pos += dP;
        o->PosF += dP;
    }
    else
    {
        o->hybridVel = 0.0f;
    }
        
    if (o->Pos < o->pos_min)
    {
        if (o->hybrid && o->hybridVel < 0.0f) eo_trajectory_StopHybrid(o);

        o->pos_steps_to_end = 0;
        o->vel_steps_to_end = 0;

        o->PosF = o->Pos = o->pos_min;
        o->VelF = o->Vel = o->Acc = 0.0f;

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
        o->VelF = o->Vel = o->Acc = 0.0f;

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
