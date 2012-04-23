
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

static const float PERIOD    = 0.001f;       // 1 ms
static const float FREQUENCY = 1.0f/PERIOD;  // 1 kHz

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOmotorcontroller* eo_motorcontroller_New(void) 
{
    EOmotorcontroller *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOmotorcontroller), 1);

    if (o)
    {
        o->control_mode = CM_IDLE;

        o->torque_meas = 0.0f;
        o->encpos_meas = 0.0f;
        
        o->stiffness = 0.0f;

        o->vel = 0.0f;
        o->vel_max = 0.0f;
        o->fake_pos_ref = 0.0f;

        o->pos_ref = 0.0f;
        o->vel_ref = 0.0f;
        o->acc_ref = 0.0f;

        o->pidP = eo_pid_New();
        o->pidT = eo_pid_New();
        
        o->trajectory = eo_trajectory_New(); 
    }

    return o;
}

extern void eo_motorcontroller_SetVelMax(EOmotorcontroller *o, float vel_max)
{
    if (vel_max < 0.0f) vel_max = -vel_max;

    o->vel_max = vel_max;
}

extern uint8_t eo_motorcontroller_SetControlMode(EOmotorcontroller *o, control_mode_t control_mode)
{
    o->control_mode = control_mode;

    return 1;
}

extern void eo_motorcontroller_SetStiffness(EOmotorcontroller *o, float stiffness)
{
    o->stiffness = stiffness;
}

extern void eo_motorcontroller_ReadEncPos(EOmotorcontroller *o, float encpos_meas)
{
    o->encpos_meas = encpos_meas;
}

extern void eo_motorcontroller_ReadTorque(EOmotorcontroller *o, float torque_meas)
{
    o->torque_meas = torque_meas;
}

extern void eo_motorcontroller_SetRefPos(EOmotorcontroller *o, float pos_ref, float avg_speed)
{
}
extern void eo_motorcontroller_SetRefVel(EOmotorcontroller *o, float vel_ref, float acc_ref)
{
    if (vel_ref < -o->vel_max) 
        o->vel_ref = -o->vel_max;
    else if (vel_ref > o->vel_max) 
        o->vel_ref =  o->vel_max;
    else 
        o->vel_ref = vel_ref;
        
    if (o->acc_ref < 0.0f) o->acc_ref = -o->acc_ref;
    
    acc_ref *= PERIOD;  
}

extern float eo_motorcontroller_PWM(EOmotorcontroller *o)
{
    switch (o->control_mode)
    {
        case CM_IDLE:
            return 0.0f;
            
        case CM_POSITION:
        {
            o->pos_ref = eo_trajectory_Step(o->trajectory);
            
            return eo_pid_PWM(o->pidP, o->pos_ref - o->encpos_meas);
        }

        case CM_VELOCITY:
        {
            float vel_err = o->vel_ref - o->vel;

            if (vel_err < -o->acc_ref)
                o->vel -= o->acc_ref;
            else if (vel_err > o->acc_ref)
                o->vel += o->acc_ref;
            else
                o->vel  = o->vel_ref;

            o->fake_pos_ref += o->vel*PERIOD;

            o->pos_ref = eo_trajectory_Step(o->trajectory) + o->fake_pos_ref;   

            return eo_pid_PWM(o->pidP, o->pos_ref - o->encpos_meas);
        }         

        case CM_TORQUE:
        {
            float pwm = eo_pid_PWM(o->pidT, o->torque_ref - o->torque_meas);

            // to add: speed damping

            return pwm;
        }       
        case CM_IMPEDANCE_POS: 
        case CM_IMPEDANCE_VEL:
        ;
    }
    
    return 0.0f;   
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




