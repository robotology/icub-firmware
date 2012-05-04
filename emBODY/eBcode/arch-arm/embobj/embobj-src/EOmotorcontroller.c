
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

static float limit(float x, float min, float max);
static void compute_velocity_ref(EOmotorcontroller *o);
static void compute_torque_ref(EOmotorcontroller *o);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOmotorcontroller";

static const float PERIOD    = 0.001f;       // 1 ms
//static const float FREQUENCY = 1.0f/PERIOD;  // 1 kHz

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOmotorcontroller* eo_motorcontroller_New(void) 
{
    EOmotorcontroller *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOmotorcontroller), 1);

    if (o)
    {
        o->pidP = eo_pid_New();
        o->pidT = eo_pid_New();
        o->pidI = eo_pid_New();
        
        o->trajectory = eo_trajectory_New(); 

        o->pos_min = 0.0f;
        o->pos_max = 0.0f;
        o->vel_max = 0.0f;

        o->encpos_meas = 0.0f;
        o->torque_meas = 0.0f;

        o->pos_vel_bias = 0.0f;
        o->vel_ref      = 0.0f;
        o->acc_ref_step = 0.0f;
        o->vel_timer    = 0.0f;
        o->vel_timeout  = 0.1f;

        o->pos_out      = 0.0f;
        o->vel_out      = 0.0f;
        o->vel_out_step = 0.0f;

        o->torque_ref = 0.0f;
        o->torque_out = 0.0f;
        o->torque_off = 0.0f;
        o->torque_last_pos_err = 0.0f;
        o->torque_damp_lp_filt = 0.0f;

        o->stiffness = 0.0f;
        o->damping   = 0.0f;

        o->control_mode = CM_IDLE;
    }

    return o;
}

extern void eo_motorcontroller_SetPosLimits(EOmotorcontroller *o, float pos_min, float pos_max)
{
    o->pos_min = pos_min;
    o->pos_max = pos_max;
}

extern void eo_motorcontroller_SetVelMax(EOmotorcontroller *o, float vel_max)
{
    o->vel_max = vel_max;
}

extern void eo_motorcontroller_SetStiffness(EOmotorcontroller *o, float stiffness)
{
    o->stiffness = stiffness;
}

extern void eo_motorcontroller_ReadEncoder(EOmotorcontroller *o, float encpos_meas)
{
    o->encpos_meas = encpos_meas;
}

extern void eo_motorcontroller_ReadTorqueSensor(EOmotorcontroller *o, float torque_meas)
{
    o->torque_meas = torque_meas;
}

extern uint8_t eo_motorcontroller_SetRefPos(EOmotorcontroller *o, float pos, float speed)
{
    if (o->control_mode == CM_IDLE || o->control_mode == CM_TORQUE  || o->control_mode == CM_OPENLOOP) return 0;

    if (o->control_mode == CM_IMPEDANCE_POS || o->control_mode == CM_IMPEDANCE_VEL)
    {
        o->control_mode = CM_IMPEDANCE_POS;
    }
    else
    {
        o->control_mode = CM_POSITION;
    }

    eo_trajectory_SetReference(o->trajectory, 
                               o->encpos_meas, 
                               limit(pos,  o->pos_min, o->pos_max), 
                               o->vel_out, 
                               limit(speed, -o->vel_max, o->vel_max));

    return 1;
}

extern uint8_t eo_motorcontroller_SetRefVel(EOmotorcontroller *o, float vel_ref, float acc_ref)
{
    if (o->control_mode == CM_IDLE || o->control_mode == CM_TORQUE || o->control_mode == CM_OPENLOOP) return 0;

    if (o->control_mode == CM_IMPEDANCE_POS || o->control_mode == CM_IMPEDANCE_VEL)
    { 
        o->control_mode = CM_IMPEDANCE_VEL;
    }
    else
    {
        o->control_mode = CM_VELOCITY;
    }

    o->vel_ref = limit(vel_ref, -o->vel_max, o->vel_max); 
    o->acc_ref_step = acc_ref * ( acc_ref > 0.0f ? PERIOD:-PERIOD);
    //o->pos_vel_bias = 0.0f;
    o->vel_timer = 0.0f;

    return 1;
}

extern uint8_t eo_motorcontroller_SetControlMode(EOmotorcontroller *o, control_mode_t control_mode)
{
    if (o->control_mode == control_mode) return 1;

    switch (control_mode)
    {
        case CM_POSITION:
        {
            o->control_mode = CM_POSITION;

            eo_trajectory_Stop(o->trajectory, o->encpos_meas);

            break;
        }

        case CM_VELOCITY:
        {
            o->control_mode = CM_VELOCITY;

            o->vel_ref = 0.0f;

            break;
        }
    };

    return 1;
}

extern float eo_motorcontroller_PWM(EOmotorcontroller *o)
{
    switch (o->control_mode)
    {
        case CM_IDLE:
            return 0.0f;

        case CM_OPENLOOP:
            return eo_pid_GetOffset(o->pidP);

        case CM_VELOCITY:
        {
            compute_velocity_ref(o);  

            return eo_pid_PWM(o->pidP, o->pos_out - o->encpos_meas);
        }

        case CM_IMPEDANCE_VEL:
        {
            compute_velocity_ref(o);
            
            compute_torque_ref(o);
            
            return eo_pid_PWM(o->pidT, o->torque_out - o->torque_meas);    
        }

        case CM_CALIB_ABS_POS_SENS:
        {
            // not implemented
            return 0.0f;
        }

        case CM_POSITION:
        case CM_CALIB_ABS_AND_INC:
        {
            eo_trajectory_Step(o->trajectory, &(o->pos_out), &(o->vel_out));

            return eo_pid_PWM(o->pidP, o->pos_out - o->encpos_meas);
        }

        case CM_IMPEDANCE_POS:
        {
            eo_trajectory_Step(o->trajectory, &(o->pos_out), &(o->vel_out));

            compute_torque_ref(o);

            return eo_pid_PWM(o->pidT, o->torque_out - o->torque_meas);
        }

        case CM_TORQUE:
        {
            float pwm = eo_pid_PWM(o->pidT, o->torque_ref - o->torque_meas);

            // to add: speed damping

            return pwm;
        }       
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

static float limit(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;    
}

static void compute_velocity_ref(EOmotorcontroller *o)
{
    float last_pos_out = o->pos_out;

    float vel_err = o->vel_ref - o->vel_out;

    if (vel_err != 0.0f)
    {
        if (vel_err < -o->acc_ref_step)
        {
            o->vel_out = limit(o->vel_out - o->acc_ref_step, -o->vel_max, o->vel_max);
        }
        else if (vel_err > o->acc_ref_step)
        {
            o->vel_out = limit(o->vel_out + o->acc_ref_step, -o->vel_max, o->vel_max);
        }
        else
        {
            o->vel_out = limit(o->vel_ref, -o->vel_max, o->vel_max);
        }

        o->vel_out_step = o->vel_out * PERIOD;
    }

    eo_trajectory_Step(o->trajectory, &(o->pos_out), &(o->vel_out));            
    o->pos_vel_bias += o->vel_out_step;
    o->pos_out += o->pos_vel_bias;

    if (o->pos_out < o->pos_min && o->pos_out < last_pos_out)
    {
        o->pos_out = o->pos_min;
        o->vel_ref = 0.0f;
    }
    else if (o->pos_out > o->pos_max && o->pos_out > last_pos_out)
    {
        o->pos_out = o->pos_max;
        o->vel_ref = 0.0f;
    }

    o->vel_timer += PERIOD;

    if (o->vel_timer >= o->vel_timeout)
    {
        o->vel_timer = 0.0f;
        o->control_mode = CM_POSITION;
                
        eo_trajectory_Stop(o->trajectory, o->pos_out);
    }  
}

static void compute_torque_ref(EOmotorcontroller *o)
{
    float pos_err = o->pos_out - o->encpos_meas;
    o->torque_damp_lp_filt = 0.9f * o->torque_damp_lp_filt + 0.1f * o->damping * (pos_err - o->torque_last_pos_err);
    o->torque_last_pos_err = pos_err;
    o->torque_out = o->torque_off + o->stiffness * pos_err + o->torque_damp_lp_filt;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




