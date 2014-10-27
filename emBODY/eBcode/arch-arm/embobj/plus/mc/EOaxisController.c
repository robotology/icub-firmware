
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
//#include "EOtheErrorManager.h"
//#include "EOVtheSystem.h"
#include "EOemsControllerCfg.h"
//#include "hal_led.h"

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

#define GET_AXIS_POSITION() (o->position)
#define GET_AXIS_VELOCITY() (o->velocity)

#define NOT_READY()     (o->state_mask)
#define IS_CALIBRATED() (!(o->state_mask & AC_NOT_CALIBRATED))

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

static void axisMotionReset(EOaxisController *o);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOaxisController";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOaxisController* eo_axisController_New(uint8_t id) 
{
    EOaxisController *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOaxisController), 1);

    if (o)
    {
        o->axisID = id;
        
        o->pidP = eo_pid_New();    
        o->pidT = eo_pid_New();
        
        o->trajectory = eo_trajectory_New();
        
        o->rot_sign = 0;
        
        o->velocity_timer = 0;
        o->torque_timer = 0;

        ///////////////////////////
        o->pos_min = o->pos_max = TICKS_PER_HALF_REVOLUTION;
        
        o->torque_ref = 0;
        o->torque_out = 0;
        o->torque_off = 0;
        o->torque_meas = 0;
        o->torque_wdog = TORQUE_SENSOR_TIMEOUT;

        o->stiffness = 0;
        o->damping   = 0;
        ///////////////////////////
        
        o->openloop_out = 0;

        o->control_mode  = eomc_controlmode_notConfigured;
        o->interact_mode = eOmc_interactionmode_stiff;

        o->err = 0;
        
        o->position = 0;
        o->velocity = 0;
        
        o->state_mask = AC_NOT_READY;
    }

    return o;
}

extern eObool_t eo_axisController_IsOk(EOaxisController* o)
{
    if (!o) return eobool_false;
    
    return o->state_mask == AC_OK;
}

extern eObool_t eo_axisController_IsTorqueSensorFault(EOaxisController* o)
{
    return (o->state_mask & AC_TORQUE_SENS_FAULT) != 0;
}

extern void eo_axisController_StartCalibration(EOaxisController *o)
{
    if (!o) return;
    
    if (NOT_READY() && (o->state_mask !=  AC_NOT_CALIBRATED))
    {
        o->control_mode = eomc_controlmode_hwFault;
        return;
    }
    
    SET_BITS(o->state_mask, AC_NOT_CALIBRATED);
    
    o->control_mode = eomc_controlmode_calib;
}

extern void eo_axisController_SetCalibrated(EOaxisController *o)
{
    if (!o) return;
    
    RST_BITS(o->state_mask, AC_NOT_CALIBRATED);
}

extern eObool_t eo_axisController_IsCalibrated(EOaxisController *o)
{
    if (!o) return eobool_false;
    
    return IS_CALIBRATED();
}

extern int32_t eo_axisController_EncRangeAdj(EOaxisController *o, int32_t* ep)
{
    if (!o) return eobool_false;
    
    int32_t jp = ep[0]-ep[1]+(40*ep[2])/65;
    
    if (o->pos_min <= jp && jp <= o->pos_max) return jp;
    
    int32_t err = o->pos_min - jp;
    int32_t min_err = err;
    int32_t jret = jp;
    
    err = jp - o->pos_max;
    if (min_err > err) min_err = err;
    
    jp += 40330; // 40330 = 40*65536/65;
        
    if (o->pos_min <= jp && jp <= o->pos_max) return jp;
        
    err = o->pos_min - jp;
    if (min_err > err)
    {
        min_err = err;
        jret = jp;
    }
    
    err = jp - o->pos_max;
    if (min_err > err)
    {
        min_err = err;
        jret = jp;
    }
    
    jp -= 80660; // 40330 = 40*65536/65;
        
    if (o->pos_min <= jp && jp <= o->pos_max) return jp;
        
    err = o->pos_min - jp;
    if (min_err > err)
    {
        min_err = err;
        jret = jp;
    }
    
    err = jp - o->pos_max;
    if (min_err > err)
    {
        min_err = err;
        jret = jp;
    }
    
    return jret;
}

extern void eo_axisController_SetLimits(EOaxisController *o, int32_t pos_min, int32_t pos_max, int32_t vel_max)
{
    if (!o) return;
    
    RST_BITS(o->state_mask, AC_NO_MIN_LIMIT|AC_NO_MAX_LIMIT|AC_NO_VEL_LIMIT);

    o->pos_min = pos_min;
    o->pos_max = pos_max;
    
    eo_trajectory_SetPosMin(o->trajectory, pos_min);
    eo_trajectory_SetPosMax(o->trajectory, pos_max);
  //eo_trajectory_SetVelMax(o->trajectory, vel_max);
}

extern void eo_axisController_SetPosMin(EOaxisController *o, int32_t pos_min)
{
    if (!o) return;
    
    RST_BITS(o->state_mask, AC_NO_MIN_LIMIT);
    
    o->pos_min = pos_min;

    eo_trajectory_SetPosMin(o->trajectory, pos_min);
}
extern void eo_axisController_SetPosMax(EOaxisController *o, int32_t pos_max)
{
    if (!o) return;
    
    RST_BITS(o->state_mask, AC_NO_MAX_LIMIT);
    
    o->pos_max = pos_max;

    eo_trajectory_SetPosMax(o->trajectory, pos_max);
}
extern void eo_axisController_SetVelMax(EOaxisController *o, int32_t vel_max)
{
    if (!o) return;
    
    RST_BITS(o->state_mask, AC_NO_VEL_LIMIT);

  //eo_trajectory_SetVelMax(o->trajectory, vel_max);
}

extern void eo_axisController_SetVelTimeout(EOaxisController *o, int32_t vel_timeout)
{
    if (!o) return;
    
    RST_BITS(o->state_mask, AC_NO_VEL_TOUT);

  //o->velocity_timeout = vel_timeout;
}

extern void eo_axisController_SetImpedance(EOaxisController *o, int32_t stiffness, int32_t damping, int32_t offset)
{
    if (o)
    {
        o->stiffness  = stiffness;
        o->damping    = (damping*EMS_FREQUENCY_INT32)/1000;
        o->torque_off = offset;
    }
}                 

extern void eo_axisController_GetImpedance(EOaxisController *o, int32_t *stiffness, int32_t *damping, int32_t *offset)
{
    if (o)
    {
        *stiffness = o->stiffness;
        *damping   = (o->damping*1000)/EMS_FREQUENCY_INT32;
        *offset    = o->torque_off;
    }
}

extern void eo_axisController_SetEncPos(EOaxisController *o, int32_t pos)
{
    if (o) o->position = pos;
} 

extern void eo_axisController_SetEncVel(EOaxisController *o, int32_t vel)
{
    if (o) o->velocity = vel;
} 

extern void eo_axisController_SetTorque(EOaxisController *o, int16_t trq)
{
    if (o)
    {
        if (o->torque_meas != trq)
        {
            o->torque_meas = trq;
            o->torque_wdog = TORQUE_SENSOR_TIMEOUT;
        }
    }
}

extern eObool_t eo_axisController_SetInteractionMode(EOaxisController *o, eOmc_interactionmode_t mode)
{
    o->interact_mode = mode;
 
    if (mode == eOmc_interactionmode_stiff)
    {
        axisMotionReset(o);
    }
    
    return eobool_true;
}

/*
extern eOmc_interactionmode_t eo_axisController_GetInteractionMode(EOaxisController *o)
{
    return o->interact_mode;
}
*/

extern eOmc_controlmode_t eo_axisController_GetControlMode(EOaxisController *o)
{
    if (!o) return eomc_controlmode_idle;
    
    return o->control_mode;
}

extern eObool_t eo_axisController_SetOutput(EOaxisController *o, int16_t out)
{
    //if (!o) return eObool_false; 
    
    if (o->control_mode != eomc_controlmode_openloop) return eobool_false;
    
    o->openloop_out = out;
    
    return eobool_true;     
}

extern eObool_t eo_axisController_SetPosRef(EOaxisController *o, int32_t pos, int32_t avg_vel)
{
    //if (!o) return eobool_false;
    
    if (NOT_READY()) return eobool_false;

    if ((o->control_mode != eomc_controlmode_position) &&
        (o->control_mode != eomc_controlmode_mixed)) 
        return eobool_false;
    
    eo_trajectory_SetPosReference(o->trajectory, GET_AXIS_POSITION(), pos, avg_vel);
    
    return eobool_true;  
}

extern eObool_t eo_axisController_SetPosRaw(EOaxisController *o, int32_t pos)
{
    //if (!o) return eobool_false;
    
    if (NOT_READY()) return eobool_false;
    
    if (o->control_mode != eomc_controlmode_direct) return eobool_false;

    eo_trajectory_SetPosRaw(o->trajectory, pos);

    return eobool_true;
}

extern eObool_t eo_axisController_SetVelRef(EOaxisController *o, int32_t vel, int32_t avg_acc)
{
    //if (!o) return eobool_false;
    
    if (NOT_READY()) return eobool_false;
    
    if ((o->control_mode != eomc_controlmode_velocity) && 
        (o->control_mode != eomc_controlmode_mixed)) 
        return eobool_false;
    
    o->velocity_timer = VELOCITY_CMD_TIMEOUT;
       
    eo_trajectory_SetVelReference(o->trajectory, vel, avg_acc);
    
    return eobool_true;
}

extern eObool_t eo_axisController_SetTrqRef(EOaxisController *o, int32_t trq)
{
    //if (!o) return eobool_false;
    
    if (NOT_READY())
    {
        o->torque_ref = 0;
        return eobool_false;
    }
    
    if (o->control_mode != eomc_controlmode_torque)
    {
        o->torque_ref = 0;
        return eobool_false;
    }
    
    o->torque_timer = TORQUE_CMD_TIMEOUT;
    
    o->torque_ref = trq;
    
    return eobool_true;
}

extern void eo_axisController_SetHardwareFault(EOaxisController *o)
{
		o->control_mode = eomc_controlmode_hwFault;
}

extern eObool_t eo_axisController_IsHardwareFault(EOaxisController *o)
{
		return o->control_mode == eomc_controlmode_hwFault;
}

extern eObool_t eo_axisController_SetControlMode(EOaxisController *o, eOmc_controlmode_command_t cmc)
{
    //if (!o) return eobool_false;
    
    if (o->control_mode == eomc_controlmode_hwFault)
    {
        if (cmc == eomc_controlmode_cmd_force_idle)
        {
            o->state_mask &= 0x00FF; // reset errors
            axisMotionReset(o);
            o->control_mode = eomc_controlmode_idle;
            return eobool_true;
        }
        
        return eobool_false;
    }
    
    if (NOT_READY())
    {
        //o->control_mode = eomc_controlmode_notConfigured;
        
        return eobool_false;
    }
    
    /*
    if (o->control_mode == eomc_controlmode_calib)
    {
        if (cmc == eomc_controlmode_cmd_idle)
        {
            o->control_mode = eomc_controlmode_idle;
            
            return eobool_true;
        }
        
        return eobool_false;
    }
    */
    
    //o->control_mode = eomc_controlmode_idle;
    
    switch (cmc)
    {
    case eomc_controlmode_cmd_force_idle:
    case eomc_controlmode_cmd_idle:
        axisMotionReset(o);
        o->control_mode = eomc_controlmode_idle;
        return eobool_true;
    
    case eomc_controlmode_direct:
        o->control_mode = eomc_controlmode_direct;
        eo_pid_Reset(o->pidP);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
        o->velocity_timer = 0; //VELOCITY_TIMEOUT;
        o->err = 0;
        return eobool_true;
    
    case eomc_controlmode_cmd_position:
        o->control_mode = eomc_controlmode_position;
        eo_pid_Reset(o->pidP);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
        o->velocity_timer = 0; //VELOCITY_TIMEOUT;
        o->err = 0;
        return eobool_true;
    
    case eomc_controlmode_cmd_velocity:
        o->control_mode = eomc_controlmode_velocity;
        eo_pid_Reset(o->pidP);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
        o->velocity_timer = 0; //VELOCITY_TIMEOUT;
        o->err = 0;
        return eobool_true;
    
    case eomc_controlmode_cmd_mixed:
        o->control_mode = eomc_controlmode_mixed;
        eo_pid_Reset(o->pidP);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
        o->velocity_timer = 0; //VELOCITY_TIMEOUT;
        o->err = 0;
        return eobool_true;

    case eomc_controlmode_cmd_torque:
        eo_pid_Reset(o->pidT);
        o->torque_timer = 0;
        o->torque_wdog = TORQUE_SENSOR_TIMEOUT;
        o->torque_ref = 0;
        o->torque_meas = 0;
        o->err = 0;
        o->control_mode = eomc_controlmode_torque;
        return eobool_true;
    
    case eomc_controlmode_cmd_openloop:
        o->control_mode = eomc_controlmode_openloop;
        o->openloop_out = 0;
        o->err = 0;
        return eobool_true;
    }
    
    return eobool_false;
}

extern int16_t eo_axisController_PWM(EOaxisController *o, eObool_t *stiff)
{
    //if (!o) return 0;
    
    int32_t pos = GET_AXIS_POSITION();
    int32_t vel = GET_AXIS_VELOCITY();
    
    switch (o->control_mode)
    {
        case eomc_controlmode_hwFault:
            return 0;
        
        case eomc_controlmode_calib:
        {
            if (IS_CALIBRATED())
            {
                eo_pid_Reset(o->pidP);
                eo_trajectory_Init(o->trajectory, pos, vel, 0);
                eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION()); 
                o->control_mode = eomc_controlmode_position;
            }
            
            o->interact_mode = eOmc_interactionmode_stiff;
            *stiff = eobool_true;
            o->err = 0;
            return 0;
        }
        case eomc_controlmode_idle:
        {
            eo_trajectory_Init(o->trajectory, pos, vel, 0);
            *stiff = o->interact_mode == eOmc_interactionmode_stiff;
            o->err = 0;
            return 0;
        }
        case eomc_controlmode_openloop:
        {
            eo_trajectory_Init(o->trajectory, pos, vel, 0);
            *stiff = eobool_true;
            o->err = 0;
            
            if (pos <= o->pos_min || o->pos_max <= pos)
            {
                return 0;
            }
            
            return o->openloop_out;
        }

        
        // pos,vel,direct,mixed modes
        
        case eomc_controlmode_mixed:
        case eomc_controlmode_velocity:
            if (o->velocity_timer)
            {
                --o->velocity_timer;
            }
            else
            {
                eo_trajectory_VelocityStop(o->trajectory);
            }
            
        case eomc_controlmode_direct:
        case eomc_controlmode_position:
        {
            float pos_ref;
            float vel_ref;
            float acc_ref;
            
            eo_trajectory_Step(o->trajectory, &pos_ref, &vel_ref, &acc_ref);
            
            int32_t err = pos_ref - pos;

            if (o->interact_mode == eOmc_interactionmode_stiff)
            {
                *stiff = eobool_true;
                
                o->err = err;
                
                //return eo_pid_PWM_piv(o->pidP, o->err, vel_ref-vel);
                
                #ifdef EXPERIMENTAL_SPEED_CONTROL
                return (int16_t)(vel_ref / 10 + err / 2);
                #else
                return eo_pid_PWM_pid(o->pidP, o->err);
                #endif
            }
            else // compliant
            {
                *stiff = eobool_false;
                
                if (o->torque_wdog)
                {
                    --o->torque_wdog;
                }
                else
                {
                    SET_BITS(o->state_mask,AC_TORQUE_SENS_FAULT); 
                    o->control_mode = eomc_controlmode_hwFault;
                    o->torque_ref = o->torque_meas = 0;
                    return 0;
                }
        
                o->torque_ref = o->torque_off + (o->stiffness*err)/1000 + o->damping*(err - o->err);

                o->err = err;
                
                #if defined(ANKLE_BOARD) 
                    int16_t pwm_out = eo_pid_PWM_pi_3_0Hz_1stLPF(o->pidT, o->torque_ref, o->torque_meas, vel);
                #elif defined(SHOULDER_BOARD) 
                    int16_t pwm_out = eo_pid_PWM_pi_1_1Hz_1stLPF(o->pidT, o->torque_ref, o->torque_meas, vel);
                #elif defined(UPPERLEG_BOARD) || defined(WAIST_BOARD)
                    int16_t pwm_out = eo_pid_PWM_pi_0_8Hz_1stLPF(o->pidT, o->torque_ref, o->torque_meas, vel);
                #else
                    int16_t pwm_out = 0;
                #endif
                
                if (pos < o->pos_min)
                {
                    int16_t pwm_lim = eo_pid_PWM_p(o->pidP, o->pos_min - pos);
                    
                    if ((pwm_lim > 0) ^ (pwm_out > pwm_lim)) pwm_out = pwm_lim;
                }
                else if (pos > o->pos_max)
                {
                    int16_t pwm_lim = eo_pid_PWM_p(o->pidP, o->pos_max - pos);
                    
                    if ((pwm_lim > 0) ^ (pwm_out > pwm_lim)) pwm_out = pwm_lim;
                }
                
                return pwm_out;
            }
        }
        case eomc_controlmode_torque:
        {
            *stiff = eobool_false;
            
            o->err = 0;
            
            if (o->torque_wdog)
            {
                --o->torque_wdog;
            }
            else
            {
                SET_BITS(o->state_mask,AC_TORQUE_SENS_FAULT);
                o->control_mode = eomc_controlmode_hwFault;
                o->torque_ref = o->torque_meas = 0;
                return 0;
            }
            
            if (o->torque_timer)
            {
                --o->torque_timer;
            }
            else
            {
                o->torque_ref = 0;
                
                //return 0;
            }
            
            #if defined(ANKLE_BOARD) 
                int16_t pwm_out = eo_pid_PWM_pi_3_0Hz_1stLPF(o->pidT, o->torque_ref, o->torque_meas, vel);
            #elif defined(SHOULDER_BOARD) 
                int16_t pwm_out = eo_pid_PWM_pi_1_1Hz_1stLPF(o->pidT, o->torque_ref, o->torque_meas, vel);
            #elif defined(UPPERLEG_BOARD) || defined(WAIST_BOARD)
                int16_t pwm_out = eo_pid_PWM_pi_0_8Hz_1stLPF(o->pidT, o->torque_ref, o->torque_meas, vel);
            #else
                int16_t pwm_out = 0;
            #endif
            
            if (pos < o->pos_min)
            {
                int16_t pwm_lim = eo_pid_PWM_p(o->pidP, o->pos_min - pos);
                    
                if ((pwm_lim > 0) ^ (pwm_out > pwm_lim)) pwm_out = pwm_lim;
            }
            else if (pos > o->pos_max)
            {
                int16_t pwm_lim = eo_pid_PWM_p(o->pidP, o->pos_max - pos);
                    
                if ((pwm_lim > 0) ^ (pwm_out > pwm_lim)) pwm_out = pwm_lim;
            }
            
            return pwm_out;
        } 
    }
    
    return 0;
}

extern eObool_t eo_axisController_GetMotionDone(EOaxisController *o)
{
    //if (!o) return eobool_false;
    
    if (o->control_mode == eomc_controlmode_position || o->control_mode == eomc_controlmode_mixed)
    {
        return eo_trajectory_IsDone(o->trajectory);
    }
    
    return eobool_false;
}

extern void eo_axisController_Stop(EOaxisController *o)
{
    if (o) eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
}

extern EOpid* eo_axisController_GetPosPidPtr(EOaxisController *o)
{
    return o ? o->pidP : NULL;
}

extern EOpid* eo_axisController_GetTrqPidPtr(EOaxisController *o)
{
    return o ? o->pidT : NULL;
}

extern void eo_axisController_SetBemf(EOaxisController *o, float Kbemf)
{
    //eo_pid_SetPidBemf(o->pidP, Kbemf);
    eo_pid_SetPidBemf(o->pidT, Kbemf);
}

extern void eo_axisController_SetPosPid(EOaxisController *o, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    //if (!o) return;
    
    RST_BITS(o->state_mask, AC_NO_POS_PID);

    eo_pid_SetPidBase(o->pidP, Kp, Kd, Ki, Imax, Ymax, Yoff);
}
extern void eo_axisController_SetTrqPid(EOaxisController *o, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff, float Kff)
{
    //if (!o) return;
    
    RST_BITS(o->state_mask, AC_NO_TRQ_PID);

    o->rot_sign = (Kp>=0.f)?1:-1;
    
    eo_pid_SetPidTorq(o->pidT, Kp, Kd, Ki, Imax, Ymax, Yoff, Kff);
}

extern void eo_axisController_GetJointStatus(EOaxisController *o, eOmc_joint_status_t* jointStatus)
{
    if (!o) return;
    
    jointStatus->interactionmodestatus =  o->interact_mode;
    
    jointStatus->basic.controlmodestatus   = o->control_mode;
    jointStatus->basic.position            = GET_AXIS_POSITION();           
    jointStatus->basic.velocity            = GET_AXIS_VELOCITY();        
    
    #warning acceleration to be implemented
    jointStatus->basic.acceleration        = 0; //eo_speedometer_GetAcceleration(o->speedmeter);       
    
    jointStatus->basic.torque              = o->torque_meas;
}

extern void eo_axisController_GetActivePidStatus(EOaxisController *o, eOmc_joint_status_ofpid_t* pidStatus)
{
    if (o->control_mode == eomc_controlmode_idle)
    {
        pidStatus->positionreference = 0;
        pidStatus->torquereference = 0;
        pidStatus->output    = 0;
        pidStatus->error     = 0;
        
        return;
    }
    
    if (o->control_mode == eomc_controlmode_openloop)
    {
        pidStatus->positionreference = 0;
        pidStatus->torquereference = 0;
        pidStatus->output    = o->openloop_out;
        pidStatus->error     = 0;
        
        return;
    }
    
    if (o->control_mode == eomc_controlmode_torque)
    {
        pidStatus->positionreference = 0;
        pidStatus->torquereference = o->torque_ref;
        eo_pid_GetStatus(o->pidT, &(pidStatus->output), &(pidStatus->error));
        
        return;
    }
    
    if (o->interact_mode == eOmc_interactionmode_compliant)
    {
        pidStatus->positionreference = eo_trajectory_GetPos(o->trajectory);
        pidStatus->torquereference = o->torque_ref;
        eo_pid_GetStatus(o->pidT, &(pidStatus->output), &(pidStatus->error));
        
        return;
    }
    
    // stiff position modes
    
    pidStatus->positionreference = eo_trajectory_GetPos(o->trajectory);
    pidStatus->torquereference = 0;
    eo_pid_GetStatus(o->pidP, &(pidStatus->output), &(pidStatus->error));    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

static void axisMotionReset(EOaxisController *o)
{
        eo_pid_Reset(o->pidP);
        eo_pid_Reset(o->pidT);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
        o->torque_timer = 0;
        o->torque_ref = 0;
        o->err = 0;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




