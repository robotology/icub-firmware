
/*  @file       EOaxisController.c
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

#include "EOaxisController.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOaxisController_hid.h" 


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
static void compute_velocity_ref(EOaxisController *o);
static void compute_torque_ref(EOaxisController *o);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOaxisController";

static const float PERIOD    = 0.001f;       // 1 ms
//static const float FREQUENCY = 1.0f/PERIOD;  // 1 kHz

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOaxisController* eo_axisController_New(void) 
{
    EOaxisController *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOaxisController), 1);

    if (o)
    {
        o->pidP = eo_pid_New(); //
        o->pidT = eo_pid_New(); //
        
        o->trajectory = eo_trajectory_New(); 

        o->pos_min = 0.0f; //
        o->pos_max = 0.0f; //
        //o->vel_max = 0.0f;

        o->encpos_meas = 0.0f;
        o->torque_meas = 0.0f;

        o->pos_vel_bias = 0.0f;
        o->vel_ref      = 0.0f;
        o->acc_ref_step = 0.0f;
        o->vel_timer    = 0.0f;
        o->vel_timeout  = 0.1f; //

        o->pos_out      = 0.0f;
        o->vel_out      = 0.0f;
        o->vel_out_step = 0.0f;

        o->torque_ref = 0.0f;
        o->torque_out = 0.0f;
        o->torque_last_pos_err = 0.0f;
        o->torque_damp_lp_filt = 0.0f;

        o->stiffness  = 0.0f; //
        o->damping    = 0.0f; //
        o->torque_off = 0.0f; //

        o->enc_offset = 0.0f;

        o->is_configured = eobool_false;
        o->is_calibrated = eobool_false;

        o->calibrating = eobool_false;

        o->control_mode = CM_IDLE;
    }

    return o;
}

extern void eo_axisController_Configure(EOaxisController *o, eOmc_joint_config_t *c)
{
    eo_pid_Configure(eo_axisController_GetPosPidPtr(o), &(c->pidposition));
    eo_pid_Configure(eo_axisController_GetTrqPidPtr(o), &(c->pidtorque));

    o->stiffness  = c->impedance.stiffness;
    o->damping    = c->impedance.damping;
    o->torque_off = c->impedance.offset;

    o->pos_min = c->minpositionofjoint;
    o->pos_max = c->maxpositionofjoint;
    o->vel_timeout = 0.001f*c->velocitysetpointtimeout;

    o->is_configured = eobool_true;    
}

extern eObool_t eo_axisController_Calibrate(EOaxisController *o, eOmc_calibrator_t *c)
{
    EOpid *pidP = eo_axisController_GetPosPidPtr(o);

    if (!eo_pid_IsConfigured(pidP))
    {
        return eobool_false;
    }
    
    if (c->type != 3)
    {
        return eobool_false;
    }

    o->control_mode = CM_POSITION;
    o->calibrating = eobool_true;

    o->enc_offset   = c->params.type3.offset;

    eo_axisController_SetPosRef(o, c->params.type3.position, c->params.type3.velocity);

    return 1;
}

//////////////////////////////////////////////////////////////////////////////////////

extern void eo_axisController_SetPosRef(EOaxisController *o, float pos, float vel)
{
    if (o->control_mode == CM_IDLE || o->control_mode == CM_TORQUE  || o->control_mode == CM_OPENLOOP) return;

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
                               vel);
}

extern void eo_axisController_SetVelRef(EOaxisController *o, float vel, float acc)
{
    if (o->control_mode == CM_IDLE || o->control_mode == CM_TORQUE || o->control_mode == CM_OPENLOOP) return;

    if (o->control_mode == CM_IMPEDANCE_POS || o->control_mode == CM_IMPEDANCE_VEL)
    { 
        o->control_mode = CM_IMPEDANCE_VEL;
    }
    else
    {
        o->control_mode = CM_VELOCITY;
    }

    o->vel_ref = vel; 
    o->acc_ref_step = acc * ( acc > 0.0f ? PERIOD:-PERIOD);
    //o->pos_vel_bias = 0.0f;
    o->vel_timer = 0.0f;
}

extern void eo_axisController_SetTrqRef(EOaxisController *o, float trq)
{
    o->torque_ref = trq;
}

extern uint8_t eo_axisController_SetControlMode(EOaxisController *o, control_mode_t control_mode)
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

extern void eo_axisController_GetActivePidStatus(EOaxisController *o, float *pwm, float *err)
{
    switch (o->control_mode)
    {
        case CM_IDLE:
            *pwm = 0.0f;
            *err = 0.0f;
            break;

        case CM_OPENLOOP:
            *pwm = eo_pid_GetOffset(o->pidP);
            *err = 0.0f;
            break;

        case CM_VELOCITY:
        case CM_POSITION:
            eo_pid_GetStatus(o->pidP, pwm, err);
            break;

        case CM_TORQUE:
            eo_pid_GetStatus(o->pidT, pwm, err);
            break;
    }
}

extern float eo_axisController_PWM(EOaxisController *o)
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



/*
extern EOpid* eo_axisController_GetPosPidPtr(EOaxisController *o)
{
    return o->pidP;
}

extern EOpid* eo_axisController_GetTrqPidPtr(EOaxisController *o)
{
    return o->pidT;
}
*/

/*
extern void eo_axisController_ReadEncPos(EOaxisController *o, float encpos)
{
    o->encpos_meas = encpos - o->enc_offset;
}

extern void eo_axisController_ReadTorque(EOaxisController *o, float torque)
{
    o->torque_meas = torque;
}
*/

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

static void compute_velocity_ref(EOaxisController *o)
{
    float last_pos_out = o->pos_out;

    float vel_err = o->vel_ref - o->vel_out;

    if (vel_err != 0.0f)
    {
        if (vel_err < -o->acc_ref_step)
        {
            o->vel_out -= o->acc_ref_step;
        }
        else if (vel_err > o->acc_ref_step)
        {
            o->vel_out += o->acc_ref_step;
        }
        else
        {
            o->vel_out = o->vel_ref;
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

static void compute_torque_ref(EOaxisController *o)
{
    float pos_err = o->pos_out - o->encpos_meas;
    o->torque_damp_lp_filt = 0.9f * o->torque_damp_lp_filt + 0.1f * o->damping * (pos_err - o->torque_last_pos_err);
    o->torque_last_pos_err = pos_err;
    o->torque_out = o->torque_off + o->stiffness * pos_err + o->torque_damp_lp_filt;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




