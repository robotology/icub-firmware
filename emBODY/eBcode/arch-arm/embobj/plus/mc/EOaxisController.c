
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
        o->calibration_type = eomc_calibration_typeUndefined;
        o->pwm_limit_calib = 0;
        o->calib_count = 0;
        o->calib_stable = 0;
        o->old_pos = o->pos_max;
        o->pos_to_reach = 0;
        o->offset = 0;
        o->isvirtuallycoupled = 0;
        o->hardwarelimitisreached = 0;
    }

    return o;
}

extern void eo_axisController_SetItIsVirtuallyCoupled(EOaxisController* o)
{   // imposes that this axis is virtually coupled (on the head v3 only rigth and left eyes are virtually coupled)
    if (!o) return;
    
    o->isvirtuallycoupled = 1;    
    
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
    //ale disabled this...but it seems clearer to me for the high-level user...discuss
    o->control_mode = eomc_controlmode_calib;
}
extern void eo_axisController_StartCalibration_type6(EOaxisController *o, int32_t position, int32_t velocity, int32_t maxencoder)
{
    if (!o) return;
    
    if (NOT_READY() && (o->state_mask !=  AC_NOT_CALIBRATED))
    {
        o->control_mode = eomc_controlmode_hwFault;
        return;
    }
    
    SET_BITS(o->state_mask, AC_NOT_CALIBRATED);
    
    /*
    //init the traj object
    eo_trajectory_Init(o->trajectory, GET_AXIS_POSITION(), GET_AXIS_VELOCITY(), 0);
    eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION());
    
    //new trajectory using params1 and params2
    eo_axisController_SetPosRef(o, position, velocity);
    
    //what should I do with param3???
    
    o->control_mode = eomc_controlmode_calib;
    */
}

extern void eo_axisController_StartCalibration_type5(EOaxisController *o, int32_t pwmlimit, int32_t final_position)
{
    if (!o) return;
    
    if (NOT_READY() && (o->state_mask !=  AC_NOT_CALIBRATED))
    {
        o->control_mode = eomc_controlmode_hwFault;
        return;
    }
    
    SET_BITS(o->state_mask, AC_NOT_CALIBRATED);
    
    //set the values to start procedure
    o->pwm_limit_calib = pwmlimit;
    
    if (o->pwm_limit_calib > 0)
        o->pos_to_reach = final_position;
    else
        o->pos_to_reach = -final_position;
 
    //reset the offset
    o->offset = 0;
    
    o->control_mode = eomc_controlmode_calib;
    
    // reset hwlimit reached
    o->hardwarelimitisreached =0;
}

extern void eo_axisController_SetCalibrated(EOaxisController *o)
{
    if (!o) return;
    
    RST_BITS(o->state_mask, AC_NOT_CALIBRATED);
}

extern void eo_axisController_ResetCalibration(EOaxisController *o)
{
    if (!o) return;
    
    axisMotionReset(o);
    SET_BITS(o->state_mask, AC_NOT_CALIBRATED);
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

extern int32_t eo_axisController_GetAxisPos (EOaxisController *o)
{
    if (o) 
        return o->position;
    else 
        return 0;
}

extern int32_t eo_axisController_GetAxisVel (EOaxisController *o)
{
    if (o) 
        return o->velocity;
    else 
        return 0;
}

extern void eo_axisController_SetEncPos(EOaxisController *o, int32_t pos)
{
    if (!o) return;
    
    if((o->calibration_type == eomc_calibration_type5_hard_stops_mc4plus) && !(eo_axisController_IsCalibrated(o)))
        o->position = pos;
    else 
        o->position = pos - eo_axisController_GetAxisCalibrationZero(o);
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
        //if the AXIS is not calibrated (not configured or in calib phase), I go back (or remain in) to notConfigured (safe) state (probably an EXTfault has occurred)
        //N.B. at the moment only for calibration5 cause calibration3 applies only an offset to the encoder raw values (no risks)
        if ((!IS_CALIBRATED()))
        {    
            if (o->calibration_type == eomc_calibration_type5_hard_stops_mc4plus)
            {
                //if the joint was calibrating, I reset its calibration values
                if (o->control_mode == eomc_controlmode_calib)
                {
                    eo_emsController_ResetCalibrationValues(o->axisID);
                }
                o->control_mode = eomc_controlmode_notConfigured;
            }
            
            else if (o->calibration_type == eomc_calibration_type3_abs_sens_digital)
            {
                o->control_mode = eomc_controlmode_idle;
            }
            
        }
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
            //calib type 5
            if (o->calibration_type == eomc_calibration_type5_hard_stops_mc4plus)
            {
                if(/*(1 == o->isvirtuallycoupled) && */(1 == o->hardwarelimitisreached))
                {   
                    // if in here, then it means that the calibration for one eye is terminated but the calib for the
                    // other eye is not terminated yet.
                    // TAG-XXX
                    return 0;                    
                }
                
                if (s_eo_axisController_isHardwareLimitReached(o))
                {
                    // here I should only set the final position (the one in which the hardware limit is reached) to the value indicated in param3 of the calib
                    // the setting of this offset should be done only once!                  
                    eo_pid_Reset(o->pidP);
                    
                    o->offset = pos - o->pos_to_reach;
                    
                    int32_t new_pos = pos - o->offset;
                    
                    //out of bound protection (redundant, but protect from typo inside XML)
                    if ( new_pos < (o->pos_min - TICKS_PER_HALF_REVOLUTION))
                        new_pos += TICKS_PER_REVOLUTION;
                    else if (new_pos > (o->pos_max + TICKS_PER_HALF_REVOLUTION))
                        new_pos -= TICKS_PER_REVOLUTION;
                    
                    //update axis, trajectory pos
                    //eo_axisController_SetEncPos(o, new_pos);
                    o->position = new_pos -  o->calibration_zero;
                    eo_axisController_SetEncVel(o, vel);
                
                    eo_trajectory_Init(o->trajectory, new_pos - o->calibration_zero, vel, 0);
                    eo_trajectory_Stop(o->trajectory, new_pos - o->calibration_zero);
                    o->err = 0;
                    
                    // we set true this flag, so that for teh case of virtually coupled joint 
                    // the next time we call the function we just return 0 (see above in TAG-XXX)
                    // even if we stay in calibration control mode.

                    o->hardwarelimitisreached = 1;
                    
                    //not setting the axis as calibrated and going to position...eo_emsController_CheckCalibrations is doing this now
                    /*
                    if(0 == o->isvirtuallycoupled)
                    {   // we finish the calibration only if there is no virtual coupling (i.e.: if the axis is NOT rigth or left eye).
                        eo_axisController_SetCalibrated (o);
                        o->control_mode = eomc_controlmode_position;
                    }
                    */
                    
                    //but the first time also RobotInterface try to set the pos to 0... (it does not seem to be a problem)
                    #if defined(CALIB5_GOTO_ZERO)
                    eo_axisController_SetPosRef(o, 0, vel); // go to 0 with velocity from calib param2   
                    #endif                    
                          
                    return 0;                    
                }
                //if still not calibrated I need to continue my search for the hardware limit
                //important! need to do that only if the encoder has been already initialized
                if (!eo_axisController_IsCalibrated(o)) 
                {
                    o->interact_mode = eOmc_interactionmode_stiff;
                    *stiff = eobool_true;
                    o->err = 0;
                    return o->pwm_limit_calib;
                }
            }
            // calib type 6
            else if (o->calibration_type == eomc_calibration_type6_mais_mc4plus)
            {
                if (IS_CALIBRATED())
                {
                    eo_pid_Reset(o->pidP);
                    eo_trajectory_Init(o->trajectory, pos - o->calibration_zero, vel, 0);
                    eo_trajectory_Stop(o->trajectory, pos - o->calibration_zero); 
                    o->control_mode = eomc_controlmode_position;
                    
                    o->err = 0;
                    
                    return 0;
                }
                o->interact_mode = eOmc_interactionmode_stiff;
                *stiff = eobool_true;
                
                float pos_ref;
                float vel_ref;
                float acc_ref;
                eo_trajectory_Step(o->trajectory, &pos_ref, &vel_ref, &acc_ref);
            
                int32_t err = pos_ref - pos;

                o->err = err;
                
                //when the trajectory is ended I can can safely set the axis as calibrated
                if (eo_trajectory_IsDone(o->trajectory))
                {
                    //Set the axis calibrated cause the calibration procedure has ended
                    eo_axisController_SetCalibrated (o);
                }
                return eo_pid_PWM_pid(o->pidP, o->err);
            }
            // calib type 3
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

extern EOtrajectory* eo_axisController_GetTraj (EOaxisController *o)
{
    if (o) return o->trajectory;
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

extern void eo_axisController_RescaleAxisPosition(EOaxisController *o, int32_t current_pos)
{
    if (!o) return;
    
    int32_t pos = current_pos - o->offset;
    
    // out of bound protections
    if (pos < (o->pos_min - TICKS_PER_HALF_REVOLUTION))
        pos += TICKS_PER_REVOLUTION;
    else if (pos > (o->pos_max + TICKS_PER_HALF_REVOLUTION))
        pos -= TICKS_PER_REVOLUTION;

    //update axis pos
    o->position = pos - eo_axisController_GetAxisCalibrationZero(o);
    return;
}


extern void eo_axisController_RescaleAxisPositionToVersionVergence(EOaxisController *o2, EOaxisController *o3, int32_t current_pos2, int32_t current_pos3, int joint)
{    
    //    joint can be 2 or 3.
    // marco.accame: this matrix maps independent position of eyes into version and vergence as follows:
    // ( o )( o ) into vers = 0, verg = 0
    // (  o)(o  ) into vers = 0, verg = 45 deg
    // (o  )(o  ) into vers = 45 deg, verg = 0
    // given that independent position of eyes is measured as follows:
    // ( o ) gives 0 deg
    // (o  ) gives 45 deg
    // (  o) gives -45 deg
    const float eyesDirectMatrix[] = {+0.500, +0.500, -0.500, +0.500};
    // const float eyesInverseMatrix[] = {+1.000, -1.000, +1.000, +1.000};
    
    // apply the offset, so that we are in range [-45, +45]
    int32_t pos2 = current_pos2 - o2->offset;
    int32_t pos3 = current_pos3 - o3->offset;
    
    
    // now apply the transformation which maps pos2 and pos3 into virtual joints version and vergence        
    int32_t pos = 0; // pos is the virtual position of teh joint
    EOaxisController *o = NULL;
    
    if(2 == joint)
    {   
        // version: i apply ...
        pos = (eyesDirectMatrix[0])*pos2 + (eyesDirectMatrix[1])*pos3;    
        o = o2;
    }
    else
    {   // vergence: i apply ...
        pos = (eyesDirectMatrix[2])*pos2 + (eyesDirectMatrix[3])*pos3; 
        o = o3;
    }
    
    
    if (pos < (o->pos_min - TICKS_PER_HALF_REVOLUTION))
        pos += TICKS_PER_REVOLUTION;
    else if (pos > (o->pos_max + TICKS_PER_HALF_REVOLUTION))
        pos -= TICKS_PER_REVOLUTION;     
    
    //update axis pos
    o->position = pos - eo_axisController_GetAxisCalibrationZero(o);    
    
    return;
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
    //if for 20 consecutive times (~20ms) I'm in the same position (but let the calibration start before...), it means that I reached the hardware limit
    o->calib_count += 1;
    if (o->calib_count > 1200)
    {
        if (GET_AXIS_POSITION() == o->old_pos)
        {
            o->calib_stable += 1;
            if (o->calib_stable == 20)
            {
                o->calib_stable = 0;
                o->calib_count = 0;
                return eobool_true;
            }
        }
        else
        {
            o->calib_stable = 0;
        }
    }
    o->old_pos = GET_AXIS_POSITION();
    return eobool_false;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




