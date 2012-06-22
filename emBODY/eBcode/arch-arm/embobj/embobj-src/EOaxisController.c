
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

extern int16_t posref_can;
extern int16_t encoder_can;

extern const int32_t EMS_IFREQUENCY;
extern const float   EMS_FFREQUENCY;
extern const float   EMS_PERIOD;

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

static int32_t limit(int32_t x, int32_t min, int32_t max);
//static void check_first_run(EOaxisController *o);

//static void compute_torque_ref(EOaxisController *o);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOaxisController";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOaxisController* eo_axisController_New(void) 
{
    EOaxisController *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOaxisController), 1);

    if (o)
    {
        o->pidP = eo_pid_New();
        o->pidC = eo_pid_New();

        eo_pid_SetPid(o->pidC, 20.0f, 20.0f, 0.0125f);
        eo_pid_SetPidLimits(o->pidC, 2000.0f, 500.0f);
        
        //o->pidV = eo_pid_New();
        //o->pidT = eo_pid_New();
        
        o->speedmeter = eo_speedmeter_New(4096); // ticks per revolution
        o->trajectory = eo_trajectory_New();

        o->pos_min = 0.0f;
        o->pos_max = 0.0f;
        o->vel_max = 2048.0f;

        o->vel_timer   = 0;
        o->vel_timeout = 10*EMS_IFREQUENCY;

        ///////////////////////////
        
        o->torque_ref = 0;
        o->torque_out = 0;
        o->torque_off = 0;
        o->torque_meas = 0;
        o->torque_last_pos_err = 0;
        o->torque_damp_lp_filt = 0;

        o->stiffness = 0;
        o->damping   = 0;
        ///////////////////////////
        
        o->pwm_offset = 0;

        o->control_mode = CM_IDLE;

        o->is_calibrated = eobool_false;
        o->calib_timeout_ms = 0;
        o->calib_max_error  = 1;
    }

    return o;
}

extern void eo_axisController_StartCalibration(EOaxisController *o, int32_t pos, int32_t offset, int32_t timeout_ms, int32_t max_error)
{
    eo_speedometer_EncoderCalibrate(o->speedmeter, offset);
    eo_trajectory_Init(o->trajectory, eo_speedometer_GetDistance(o->speedmeter), eo_speedometer_GetVelocity(o->speedmeter), 0);

    o->control_mode = CM_CALIB_ABS_POS_SENS;
    o->calib_timeout_ms = timeout_ms;
    o->is_calibrated = eobool_false;
    o->calib_max_error = max_error;

    eo_trajectory_SetPosReference(o->trajectory, pos, 1024);
}

extern void eo_axisController_SetPosLimits(EOaxisController *o, int32_t pos_min, int32_t pos_max)
{
    o->pos_min = pos_min;
    o->pos_max = pos_max;

    eo_trajectory_SetLimits(o->trajectory, pos_min, pos_max);
}

extern void eo_axisController_SetVelMax(EOaxisController *o, int32_t vel_max)
{
    o->vel_max = vel_max;
}

extern void eo_axisController_SetStiffness(EOaxisController *o, int32_t stiffness)
{
    o->stiffness = stiffness;
}                 

extern void eo_axisController_ReadEncPos(EOaxisController *o, int32_t pos)
{
    eo_speedometer_EncoderValid(o->speedmeter, pos);
} 

extern void eo_axisController_SkipEncPos(EOaxisController *o)
{
    eo_speedometer_EncoderError(o->speedmeter);
}

extern int32_t eo_axisController_GetVelocity(EOaxisController *o)
{
    return eo_speedometer_GetVelocity(o->speedmeter);
}

extern void eo_axisController_SetPosRef(EOaxisController *o, int32_t pos, int32_t avg_vel)
{
    if (o->control_mode == CM_IDLE || 
        o->control_mode == CM_TORQUE || 
        o->control_mode == CM_OPENLOOP) return;

    if (!o->is_calibrated) return;

    //if (o->control_mode == CM_IMPEDANCE_VEL) o->control_mode = CM_IMPEDANCE_POS;
    
    switch (o->control_mode)
    {
    case CM_POS_VEL:
    case CM_VELOCITY:
        o->control_mode = CM_POSITION;
    case CM_POSITION:
    case CM_CALIB_ABS_POS_SENS:
        eo_trajectory_SetPosReference(o->trajectory, pos, avg_vel);
        break;
        
    case CM_IMPEDANCE_VEL:
        o->control_mode = CM_IMPEDANCE_POS;
    case CM_IMPEDANCE_POS:
        // to be implemented
        break;            
    }    
}

extern void eo_axisController_SetVelRef(EOaxisController *o, int32_t vel, int32_t avg_acc)
{
    if (o->control_mode == CM_IDLE || 
        o->control_mode == CM_TORQUE || 
        o->control_mode == CM_OPENLOOP) return;

    if (!o->is_calibrated) return;

    //if (o->control_mode == CM_IMPEDANCE_POS) o->control_mode = CM_IMPEDANCE_VEL;

    switch (o->control_mode)
    {
    case CM_POSITION:
        o->control_mode = CM_POS_VEL;
    case CM_POS_VEL:
        o->vel_timer = o->vel_timeout;
        eo_trajectory_SetVelReference(o->trajectory, limit(vel, -o->vel_max, o->vel_max), avg_acc, eobool_false);
        break;
    
    case CM_VELOCITY:
        eo_trajectory_SetVelReference(o->trajectory, limit(vel, -o->vel_max, o->vel_max), avg_acc, eobool_true);
        break;
        
    case CM_IMPEDANCE_POS:
        o->control_mode = CM_IMPEDANCE_VEL;
    case CM_IMPEDANCE_VEL:
        // to be implemented
        break;          
    }
}

extern void eo_axisController_SetTrqRef(EOaxisController *o, int32_t trq)
{
    o->torque_ref = trq;
}

extern eObool_t eo_axisController_SetControlMode(EOaxisController *o, control_mode_t control_mode)
{
    if (o->control_mode == control_mode) return eobool_true;

    switch (control_mode)
    {
        case CM_POSITION:
        {
            if (o->control_mode == CM_VELOCITY)
            {
                eo_pid_Reset(o->pidP);
                eo_trajectory_SetPosReference(o->trajectory, eo_speedometer_GetDistance(o->speedmeter), 0);            
            }
           
            break;
        }

        case CM_VELOCITY:
        {
            if (o->control_mode == CM_POSITION || o->control_mode == CM_POS_VEL)
            {
                eo_pid_Reset(o->pidP);
                eo_trajectory_SetVelReference(o->trajectory, eo_speedometer_GetVelocity(o->speedmeter), 0, eobool_true); 
            }

            break;
        }
    }

    o->control_mode = control_mode;

    return eobool_true;
}

extern void eo_axisController_GetActivePidStatus(EOaxisController *o, int16_t *pwm, int32_t *err)
{
    switch (o->control_mode)
    {
        case CM_IDLE:
            *pwm = 0;
            *err = 0;
            break;

        case CM_OPENLOOP:
            *pwm = o->pwm_offset;
            *err = 0;
            break;

        case CM_VELOCITY:
            eo_pid_GetStatus(o->pidP, pwm, err);
            break;
        case CM_POSITION:
            eo_pid_GetStatus(o->pidP, pwm, err);
            break;

        case CM_TORQUE:
            eo_pid_GetStatus(o->pidT, pwm, err);
            break;
    }
}

extern int16_t eo_axisController_PWM(EOaxisController *o)
{
    /*
    if (o->vel_timer)
    {
        if (!--o->vel_timer)
        {
            eo_trajectory_TimeoutVelReference(o->trajectory);    
        }
    }
    */

    switch (o->control_mode)
    {
        case CM_IDLE:
            return 0;

        case CM_OPENLOOP:
            return o->pwm_offset;

        case CM_CALIB_ABS_POS_SENS:
        {
            int32_t pos = eo_speedometer_GetDistance(o->speedmeter);
            int32_t vel = eo_speedometer_GetVelocity(o->speedmeter);
            float pos_ref;
            float vel_ref;

            eo_trajectory_Step(o->trajectory, &pos_ref, &vel_ref, NULL);
            
            encoder_can = pos_ref;
            posref_can = pos_ref;           

            float err = pos_ref - pos;

            if (eo_trajectory_IsDone(o->trajectory))
            {
                if (!o->calib_timeout_ms)
                {
                    o->control_mode = CM_IDLE;
                
                    return 0;
                }

                --o->calib_timeout_ms;
                
                if (-o->calib_max_error<=err && err<=o->calib_max_error)
                {
                    o->is_calibrated = eobool_true;
                    o->control_mode = CM_POSITION;
                }
            } 

            return eo_pid_PWM2(o->pidC, err, vel_ref - vel);
        }

        case CM_POS_VEL:
        case CM_POSITION:
        case CM_VELOCITY:
        {
            int32_t pos = eo_speedometer_GetDistance(o->speedmeter);
            int32_t vel = eo_speedometer_GetVelocity(o->speedmeter);
            float pos_ref;
            float vel_ref;

            if (eo_trajectory_Step(o->trajectory, &pos_ref, &vel_ref, NULL))
            {
                // position limit reached
                //eo_pid_Reset(o->pidP);
            }

            encoder_can = vel_ref;
            posref_can = pos_ref;

            return eo_pid_PWM2(o->pidP, pos_ref - pos, vel_ref - vel);
        }
       
        //case CM_IMPEDANCE_POS:
        //case CM_TORQUE:
        //case CM_IMPEDANCE_VEL:     
    }
    
    return 0;   
}

extern void eo_axisController_Stop(EOaxisController *o)
{
    eo_trajectory_Stop(o->trajectory, eo_speedometer_GetDistance(o->speedmeter));
}

extern EOpid* eo_axisController_GetPosPidPtr(EOaxisController *o)
{
    return o->pidP;
}

/*
extern EOpid* eo_axisController_GetVelPidPtr(EOaxisController *o)
{
    return o->pidV;
}
*/

extern EOpid* eo_axisController_GetTrqPidPtr(EOaxisController *o)
{
    return o->pidT;
}

extern void eo_axisController_SetOffset(EOaxisController *o, int16_t offset)
{
    o->pwm_offset = offset;
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static int32_t limit(int32_t x, int32_t min, int32_t max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;    
}

/*
static void check_first_run(EOaxisController *o)
{
    if (o->first_run)
    {
        o->first_run = eobool_false;

        o->pos_ref = eo_speedometer_GetDistance(o->speedmeter);
        o->vel_ref = eo_speedometer_GetSpeed(o->speedmeter);
    }
}
*/

/*
static void compute_torque_ref(EOaxisController *o)
{
    float pos_err = o->pos_out - o->encpos_meas;
    o->torque_damp_lp_filt = 0.9f * o->torque_damp_lp_filt + 0.1f * o->damping * (pos_err - o->torque_last_pos_err);
    o->torque_last_pos_err = pos_err;
    o->torque_out = o->torque_off + o->stiffness * pos_err + o->torque_damp_lp_filt;
}
*/

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




