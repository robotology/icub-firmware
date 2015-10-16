
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
//#include "EOVtheSystem.h"
#include "EOemsControllerCfg.h"
//#include "hal_led.h"
#include "EoError.h"
#include "EoemsController.h"

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
static eObool_t s_eo_axisController_isHardwareLimitReached(EOaxisController *o);

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
        
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        o->torque_out = 0;
        o->torque_off = 0;
        o->torque_meas_jnt = 0;
        o->torque_meas_mot = 0;
        o->torque_wdog = TORQUE_SENSOR_TIMEOUT;

        o->stiffness = 0;
        o->damping   = 0;
        ///////////////////////////
        
        o->openloop_out = 0;
        o->openloop_limitreached = 0;
        o->controller_output = 0;

        o->control_mode  = eomc_controlmode_notConfigured;
        o->interact_mode = eOmc_interactionmode_stiff;
        o->tcFilterType  = 3;

        o->err = 0;
        
        o->position = 0;
        o->velocity = 0;
        
        o->state_mask = AC_NOT_READY;
        o->calibration_zero = 0;
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

extern void eo_axisController_StartCalibration_type3(EOaxisController *o)
{
    if (!o) return;
    
    if (NOT_READY() && (o->state_mask !=  AC_NOT_CALIBRATED))
    {
        o->control_mode = eomc_controlmode_hwFault;
        return;
    }
    
    SET_BITS(o->state_mask, AC_NOT_CALIBRATED);
    //o->control_mode = eomc_controlmode_calib;
}

extern void eo_axisController_StartCalibration_type0(EOaxisController *o, int16_t pwmlimit, int16_t vel)
{
    if (!o) return;
    
    if (NOT_READY() && (o->state_mask !=  AC_NOT_CALIBRATED))
    {
        o->control_mode = eomc_controlmode_hwFault;
        return;
    }
    
    SET_BITS(o->state_mask, AC_NOT_CALIBRATED);
    
    //set the value to start procedure
    o->pwm_limit_calib = pwmlimit;
    //what about velocity? is it useful?
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


extern void eo_axisController_SetAxisCalibrationZero(EOaxisController *o, int32_t zero)
{
    if (!o) return;
    o->calibration_zero = zero;
}

extern int32_t eo_axisController_GetAxisCalibrationZero(EOaxisController *o)
{
    if (!o) return 0;
    return o->calibration_zero;
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
    if (o) o->position = pos - eo_axisController_GetAxisCalibrationZero(o);
} 

extern void eo_axisController_SetEncVel(EOaxisController *o, int32_t vel)
{
    if (o) o->velocity = vel;
} 

extern void eo_axisController_SetTorque(EOaxisController *o, int16_t trq)
{
    if (o)
    {
       o->torque_meas_jnt = trq;
       o->torque_wdog = TORQUE_SENSOR_TIMEOUT;
    }
}

extern eObool_t eo_axisController_SetInteractionMode(EOaxisController *o, eOmc_interactionmode_t mode)
{
    if (o)
    {
       if (o->interact_mode == mode) return eobool_true;
      
       o->interact_mode = mode;
       if (mode == eOmc_interactionmode_stiff)
       {
          axisMotionReset(o);
       }
       return eobool_true;
    }

    return eobool_true; //eobool_false?
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
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        return eobool_false;
    }
    
    if (o->control_mode != eomc_controlmode_torque)
    {
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        return eobool_false;
    }
    
    o->torque_timer = TORQUE_CMD_TIMEOUT;
    o->torque_ref_jnt = trq;
    
    return eobool_true;
}

extern void eo_axisController_SetHardwareFault(EOaxisController *o)
{
        axisMotionReset(o);
    
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
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        return eobool_true;
    
    case eomc_controlmode_direct:
        if (o->control_mode == eomc_controlmode_direct) return eobool_true;
        o->control_mode = eomc_controlmode_direct;
        eo_pid_Reset(o->pidP);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
        o->velocity_timer = 0; //VELOCITY_TIMEOUT;
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        o->err = 0;
        return eobool_true;
    
    case eomc_controlmode_cmd_position:
        if (o->control_mode == eomc_controlmode_position) return eobool_true;
        o->control_mode = eomc_controlmode_position;
        eo_pid_Reset(o->pidP);
        eo_trajectory_Init(o->trajectory, GET_AXIS_POSITION(), 0, 0);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
        o->velocity_timer = 0; //VELOCITY_TIMEOUT;
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        o->err = 0;
        return eobool_true;
    
    case eomc_controlmode_cmd_velocity:
        if (o->control_mode == eomc_controlmode_cmd_velocity) return eobool_true;
        o->control_mode = eomc_controlmode_velocity;
        eo_pid_Reset(o->pidP);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
        o->velocity_timer = 0; //VELOCITY_TIMEOUT;
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        o->err = 0;
        return eobool_true;
    
    case eomc_controlmode_cmd_mixed:
        if (o->control_mode == eomc_controlmode_cmd_mixed) return eobool_true;
        o->control_mode = eomc_controlmode_mixed;
        eo_pid_Reset(o->pidP);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
        o->velocity_timer = 0; //VELOCITY_TIMEOUT;
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        o->err = 0;
        return eobool_true;

    case eomc_controlmode_cmd_torque:
        if (o->control_mode == eomc_controlmode_cmd_torque) return eobool_true;
        eo_pid_Reset(o->pidT);
        o->torque_timer = 0;
        o->torque_wdog = TORQUE_SENSOR_TIMEOUT;
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        o->torque_meas_jnt = 0;
        o->torque_meas_mot = 0;
        o->err = 0;
        o->control_mode = eomc_controlmode_torque;
        return eobool_true;
    
    case eomc_controlmode_cmd_openloop:
        if (o->control_mode == eomc_controlmode_cmd_openloop) return eobool_true;
        o->control_mode = eomc_controlmode_openloop;
        o->openloop_out = 0;
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        o->err = 0;
        return eobool_true;
    }
    
    return eobool_false;
}

extern float eo_axisController_FrictionCompensation(EOaxisController *o, float input_pwm, int32_t motor_velocity )
{
    float pwm_out = input_pwm;
    if ((o->control_mode==eomc_controlmode_torque) ||
        (o->interact_mode==eOmc_interactionmode_compliant &&
        (o->control_mode==eomc_controlmode_position ||
         o->control_mode==eomc_controlmode_velocity ||
         o->control_mode==eomc_controlmode_mixed ||
         o->control_mode==eomc_controlmode_direct)))
    {
      pwm_out=eo_pid_PWM_friction(o->pidT, input_pwm, motor_velocity, o->torque_ref_mot); //here torque_ref_mot has already been computed
      //MR: This is here only temporary: it has to be replaced with limit of the motor, not the limit of the pid
      LIMIT(pwm_out,eo_pid_GetPwmMax(o->pidT));
    
    }
    else
    {
      //MR: This is here only temporary: it has to be replaced with limit of the motor, not the limit of the pid
      LIMIT(pwm_out,eo_pid_GetPwmMax(o->pidP));
    }
    return pwm_out;
}

extern float eo_axisController_PWM(EOaxisController *o, eObool_t *stiff)
{
    //if (!o) return 0;
    
    int32_t pos = GET_AXIS_POSITION();
    int32_t vel = GET_AXIS_VELOCITY();
    
    switch (o->control_mode)
    {
        case eomc_controlmode_notConfigured:
        case eomc_controlmode_hwFault:
            return 0;
        
        case eomc_controlmode_calib:
        {
            //calib type 0
            if (o->calibration_type == eomc_calibration_type0_hard_stops)
            {
                if (s_eo_axisController_isHardwareLimitReached(o))
                {
                    eo_pid_Reset(o->pidP);
                    eo_trajectory_Init(o->trajectory, pos, vel, 0);
                    eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
                    
                    eo_axisController_SetCalibrated (o);
                    o->control_mode = eomc_controlmode_position;
                    // acemor: define how to go back                    
                    //Set position reference and return
                    eo_axisController_SetPosRef(o, 0, (pos-0)/2); //how to know the right return position and velocity?
                    return 0;
                    
                }
                //if still not calibrated I need to continue my search for the hardware limit
                if (!eo_axisController_IsCalibrated(o))
                {
                    o->interact_mode = eOmc_interactionmode_stiff;
                    *stiff = eobool_true;
                    o->err = 0;
                    return o->pwm_limit_calib;
                }
            }
            /*
            else if (o->calibration_type == eomc_calibration_type3_abs_sens_digital)
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
            */
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
                //limit reached, store the PWMvalue
                if (o->openloop_limitreached == 0)
                    o->openloop_limitreached = o->openloop_out;
                   
                //check if the last PWM output has the same sign of the last openloop
                if ((o->openloop_limitreached >= 0) ^ (o->openloop_out < 0))
                    return 0;
            }
            //inside safe band, reset value
            else
            {
                o->openloop_limitreached = 0;
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
                return (0.04f*vel_ref+0.2f*(float)err);
                //return eo_pid_experimentalPWM(o->pidP, (float)err, vel_ref);
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
                    o->torque_ref_jnt = o->torque_ref_mot = o->torque_meas_jnt = o->torque_meas_mot = 0;
                    return 0;
                }
        
                o->torque_ref_jnt = o->torque_off + (o->stiffness*err)/1000 + o->damping*(err - o->err);
                o->err = err;
                
                eo_emsController_GetDecoupledMeasuredTorque (o->axisID,&o->torque_meas_mot);
                eo_emsController_GetDecoupledReferenceTorque(o->axisID,&o->torque_ref_mot);
                
                float pwm_out = 0;
                if      (o->tcFilterType==3) 
                    pwm_out = eo_pid_PWM_pi_3_0Hz_1stLPF(o->pidT, o->torque_ref_mot, o->torque_meas_mot);
                else if (o->tcFilterType==1) 
                    pwm_out = eo_pid_PWM_pi_1_1Hz_1stLPF(o->pidT, o->torque_ref_mot, o->torque_meas_mot);
                else if (o->tcFilterType==2) 
                    pwm_out = eo_pid_PWM_pi_0_8Hz_1stLPF(o->pidT, o->torque_ref_mot, o->torque_meas_mot);
                else if (o->tcFilterType==0)
                    pwm_out = eo_pid_PWM_pi(o->pidT, o->torque_ref_mot, o->torque_meas_mot);
                else
                    {
                        //invalid tcFilterType, do not use it
                        pwm_out = 0;
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
                o->torque_ref_mot = o->torque_ref_jnt = o->torque_meas_jnt = o->torque_meas_mot = 0;
                return 0;
            }
            
            if (o->torque_timer)
            {
                --o->torque_timer;
            }
            else
            {
                //MR: temporary disabled
                //o->torque_ref_mot = o->torque_ref_jnt= 0;
                //return 0;
            }
           
            eo_emsController_GetDecoupledMeasuredTorque (o->axisID,&o->torque_meas_mot);
            eo_emsController_GetDecoupledReferenceTorque(o->axisID,&o->torque_ref_mot);
            
            float pwm_out = 0;
            if      (o->tcFilterType==3) 
                pwm_out = eo_pid_PWM_pi_3_0Hz_1stLPF(o->pidT, o->torque_ref_mot, o->torque_meas_mot);
            else if (o->tcFilterType==1) 
                pwm_out = eo_pid_PWM_pi_1_1Hz_1stLPF(o->pidT, o->torque_ref_mot, o->torque_meas_mot);
            else if (o->tcFilterType==2) 
                pwm_out = eo_pid_PWM_pi_0_8Hz_1stLPF(o->pidT, o->torque_ref_mot, o->torque_meas_mot);
            else if (o->tcFilterType==0)
                pwm_out = eo_pid_PWM_pi(o->pidT, o->torque_ref_mot, o->torque_meas_mot);
            else
                {
                    //invalid tcFilterType, do not use it
                    pwm_out = 0;
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

extern void eo_axisController_SetKtau(EOaxisController *o, float Ktau)
{
    //eo_pid_SetPidKtau(o->pidP, Ktau);
    eo_pid_SetPidKtau(o->pidT, Ktau);
}

extern void eo_axisController_SetTcFilterType(EOaxisController *o, uint8_t filterType)
{
    if (o)
    {
        o->tcFilterType=filterType;
    }
}

extern void eo_axisController_SetPosPid(EOaxisController *o, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff, float stiction_up, float stiction_down)
{
    //if (!o) return;
    
    RST_BITS(o->state_mask, AC_NO_POS_PID);

    eo_pid_SetPidBase(o->pidP, Kp, Kd, Ki, Imax, Ymax, Yoff, stiction_up, stiction_down);
}
extern void eo_axisController_SetTrqPid(EOaxisController *o, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff, float Kff, float stiction_up, float stiction_down)
{
    //if (!o) return;
    
    RST_BITS(o->state_mask, AC_NO_TRQ_PID);

    o->rot_sign = (Kp>=0.f)?1:-1;
    
    eo_pid_SetPidTorq(o->pidT, Kp, Kd, Ki, Imax, Ymax, Yoff, Kff, stiction_up, stiction_down);
}

extern void eo_axisController_GetJointStatus(EOaxisController *o, eOmc_joint_status_t* jointStatus)
{
    if (!o) return;
    
    jointStatus->interactionmodestatus =  o->interact_mode;
    
    jointStatus->basic.controlmodestatus   = o->control_mode;
    jointStatus->basic.jnt_position        = GET_AXIS_POSITION();           
    jointStatus->basic.jnt_velocity        = GET_AXIS_VELOCITY();        
    
    #warning acceleration to be implemented
    jointStatus->basic.jnt_acceleration    = 0; //eo_speedometer_GetAcceleration(o->speedmeter);       
    
    jointStatus->basic.jnt_torque          = o->torque_meas_jnt;
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
        pidStatus->torquereference = o->torque_ref_jnt;
        #warning marco.randazzo: pidStatus->output is wrongly obtained before joints decoupling, fixed in s_eom_emsrunner_hid_UpdateJointstatus()
        eo_pid_GetStatusInt32(o->pidT, &(pidStatus->output), &(pidStatus->error));
        
        return;
    }
    
    if (o->interact_mode == eOmc_interactionmode_compliant)
    {
        pidStatus->positionreference = eo_trajectory_GetPos(o->trajectory);
        pidStatus->torquereference = o->torque_ref_jnt;
        #warning marco.randazzo: pidStatus->output is wrongly obtained before joints decoupling, fixed in s_eom_emsrunner_hid_UpdateJointstatus()
        eo_pid_GetStatusInt32(o->pidT, &(pidStatus->output), &(pidStatus->error));
        
        return;
    }
    
    // stiff position modes
    
    pidStatus->positionreference = eo_trajectory_GetPos(o->trajectory);
    pidStatus->torquereference = 0;
    #warning marco.randazzo: pidStatus->output is wrongly obtained before joints decoupling, fixed in s_eom_emsrunner_hid_UpdateJointstatus()
    eo_pid_GetStatusInt32(o->pidP, &(pidStatus->output), &(pidStatus->error));    
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
        o->torque_ref_jnt = 0;
        o->torque_ref_mot = 0;
        o->err = 0;
}

static eObool_t s_eo_axisController_isHardwareLimitReached(EOaxisController *o)
{
    static int32_t current_pos = 0, veryold_pos = 0;
    static uint16_t count_equal = 0, count_calib = 0;
    
    count_calib++;
    current_pos = GET_AXIS_POSITION();
    if (current_pos == veryold_pos)
    {
        count_equal++;
        //if for 20 consecutive times (~20ms) I'm in the same position (but let the calibration start before...), it means that I reached the hardware limit
        if ((count_equal == 20) && (count_calib > 1200))
        {
            count_equal = 0;
            count_calib = 0;
            return eobool_true;
        }
    }
    else
    {
        count_equal = 0;
    }
    veryold_pos = current_pos;
    return eobool_false;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




