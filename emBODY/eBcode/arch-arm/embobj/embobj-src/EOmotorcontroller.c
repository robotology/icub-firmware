
/*  @file       EOmotorcontroller.c
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

#include "EOmotorcontroller.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOmotorcontroller_hid.h" 


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

static const char s_eobj_ownname[] = "EOmotorcontroller";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOmotorcontroller* eo_motorcontroller_New(void) 
{
    EOmotorcontroller *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOmotorcontroller), 1);

    if (o)
    {
        o->control_mode = CM_UNINIT;

        o->torque_meas  = 0.0f;
        o->encoder_meas = 0.0f;
        o->stiffness = 0.0f;
        o->speed_ref = 0.0f;

        o->pid[CM_POSITION]  = eo_pid_New();
        o->pid[CM_TORQUE]    = eo_pid_New();
        o->pid[CM_VELOCITY]  = eo_pid_New();
        o->pid[CM_IMPEDANCE] = eo_pid_New();
        
        o->trajectory = eo_trajectory_New(); 
    }

    return o;
}

extern uint8_t eo_motorcontroller_SetControlMode(EOmotorcontroller *o, control_mode_t cm)
{
    if (eo_pid_IsInitialized(o->pid[cm]))
    {
        o->control_mode = cm;
        
        return 1;
    }
    else
    {
        return 0;
    }
}

extern void eo_motorcontroller_ReadEncoder(EOmotorcontroller *o, float position)
{
    o->encoder_meas = position;
}

extern void eo_motorcontroller_ReadTorque(EOmotorcontroller *o, float torque)
{
    o->torque_meas = torque;
}

extern void eo_motorcontroller_SetReference(EOmotorcontroller *o, float reference, float speed)
{
    if (o->control_mode == CM_IMPEDANCE)
    {
        // reference is the desired torque
        eo_trajectory_SetReference(o->trajectory, 
                                   o->encoder_meas, 
                                   o->encoder_meas + reference / o->stiffness, 
                                   eo_trajectory_GetSpeed(o->trajectory), 
                                   speed);
    }
    else if (o->control_mode == CM_VELOCITY)
    {
    }
    else
    {
        eo_trajectory_SetReference(o->trajectory, 
                                   o->encoder_meas, 
                                   reference,
                                   eo_trajectory_GetSpeed(o->trajectory), 
                                   speed);
    }    
}

extern void eo_motorcontroller_SetStiffness(EOmotorcontroller *o, float stiffness)
{
    o->stiffness = stiffness;
}

extern float eo_motorcontroller_Step(EOmotorcontroller *o, float En)
{
    if (o->control_mode == CM_VELOCITY)
    {
        return 0.0f;
    }
    else
    {
        return eo_pid_Step(o->pid[o->control_mode], En);
    }
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




