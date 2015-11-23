
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
#include "EOmcService.h"

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
        o->vel_raw = 0;
        o->time_raw = 100;
        
        o->position = 0;
        o->velocity = 0;
        
        o->state_mask = AC_NOT_READY;
        o->calibration_zero = 0;
        o->calibration_type = eomc_calibration_typeUndefined;
        o->pwm_limit_calib = 0;
        o->calib_count = 0;
        o->pos_stable = 0;
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
extern void eo_axisController_StartCalibration(EOaxisController *o, uint32_t* params)
{
    if (!o) return;
    
    if (NOT_READY() && (o->state_mask !=  AC_NOT_CALIBRATED))
    {
        o->control_mode = eomc_controlmode_hwFault;
        return;
    }
    
    SET_BITS(o->state_mask, AC_NOT_CALIBRATED);
    
    switch (o->calibration_type)
    {
        case eomc_calibration_type3_abs_sens_digital:
        {
            eOmc_calibrator_params_type3_abs_sens_digital_t* p_type3 = (eOmc_calibrator_params_type3_abs_sens_digital_t*) params;
            
            o->control_mode = eomc_controlmode_calib;
            
        } break;
        case eomc_calibration_type5_hard_stops_mc4plus:
        case eomc_calibration_type8_adc_and_incr_mc4plus: //same params
        {
            eOmc_calibrator_params_type5_hard_stops_mc4plus_t* p_type5 = (eOmc_calibrator_params_type5_hard_stops_mc4plus_t*) params;
    
            //set the values to start procedure
            o->pwm_limit_calib = p_type5->pwmlimit;
    
            if (o->pwm_limit_calib >= 0)
                o->pos_to_reach = p_type5->final_pos;
            else
                o->pos_to_reach = -p_type5->final_pos;
 
            //reset the offset
            o->offset = 0;
    
            o->control_mode = eomc_controlmode_calib;
    
            // reset hwlimit reached
            o->hardwarelimitisreached =0;
              
        } break; 
        default:
            break;
    }
    
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
    if (o) o->position = pos - eo_axisController_GetAxisCalibrationZero(o);
} 

extern void eo_axisController_SetEncVel(EOaxisController *o, int32_t vel)
{
    if (o) o->velocity = vel;
} 

extern void eo_axisController_SetTorque(EOaxisController *o, eOmeas_torque_t trq)
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

	#ifdef EXPERIMENTAL_SPEED_CONTROL

    int32_t vel_raw = 38*(pos - eo_trajectory_GetPos(o->trajectory));
        
    if (o->time_raw > 25)
    {
        o->vel_raw = vel_raw / 10;
    }
    else
    {
        //if (o->time_raw) vel_raw /= o->time_raw;
        //o->vel_raw = (o->vel_raw + vel_raw) / 2;
        o->vel_raw = vel_raw / 10;
    }
    
    o->time_raw = 0;
    
	#endif

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
        o->vel_raw = 0;
        o->time_raw = 100;
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
        
        //o->torque_wdog = TORQUE_SENSOR_TIMEOUT;
        o->torque_ref_jnt = o->torque_meas_jnt;
        o->torque_timer = TORQUE_CMD_TIMEOUT; //0;
        o->torque_ref_mot = 0;
        //o->torque_meas_jnt = 0;
        //o->torque_meas_mot = 0;
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
        o->old_pos = 0;
        o->pos_stable = 0;
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
    
    if (o->torque_wdog)
    {
        --o->torque_wdog;
    }
    else
    {
        o->torque_ref_jnt = o->torque_ref_mot = o->torque_meas_jnt = o->torque_meas_mot = 0;
        
        if ((o->control_mode == eomc_controlmode_torque) ||
            (o->interact_mode == eOmc_interactionmode_compliant &&
             (
                 o->control_mode == eomc_controlmode_mixed    ||
                 o->control_mode == eomc_controlmode_velocity ||
                 o->control_mode == eomc_controlmode_direct   ||
                 o->control_mode == eomc_controlmode_position
             )
            )
           )
        {
            *stiff = eobool_false;
            SET_BITS(o->state_mask,AC_TORQUE_SENS_FAULT); 
            o->control_mode = eomc_controlmode_hwFault;
            return 0.f;
        }
    }
    
    switch (o->control_mode)
    {
        case eomc_controlmode_notConfigured:
        case eomc_controlmode_hwFault:
            return 0;
        
        case eomc_controlmode_calib:
        {
            //calib type 5 and calib type 8
            if ((o->calibration_type == eomc_calibration_type5_hard_stops_mc4plus) || (o->calibration_type == eomc_calibration_type8_adc_and_incr_mc4plus))
            {
                if(1 == o->hardwarelimitisreached)
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
                    //eo_pid_Reset(o->pidP);
                    
                    o->offset = pos - o->pos_to_reach + o->calibration_zero; //adding calib zero, so that the offset is computed only from the real axis position
                    
                    o->hardwarelimitisreached = 1;
                    
                    //validation check disabled at the moment
                    /*
                    int32_t new_pos = pos - o->offset;
                    //if offset is taking the joint out of limits, I change it
                    int32_t workrange = o->pos_max - o->pos_min;
                    if (new_pos < o->pos_min)
                        o->offset -= workrange;
                    else if (new_pos > o->pos_max)
                        o->offset += workrange;
                    */        
                          
                    return 0;                    
                }
                //if still not calibrated I need to continue my search for the hardware limit
                //important! need to do that only if the encoder has been already initialized
                if (!eo_axisController_IsCalibrated(o)) 
                {

                    return o->pwm_limit_calib;
                }
            }
            // calib type 6
            else if (o->calibration_type == eomc_calibration_type6_mais_mc4plus)
            {
                #warning TBD: MAIS CALIBRATION
                /*
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
                */
            }
            // calib type 3
            else if (o->calibration_type == eomc_calibration_type3_abs_sens_digital)
            {
                return 0;
            }
            
            return 0;
        }
        case eomc_controlmode_idle:
        {
            eo_trajectory_Init(o->trajectory, pos, vel, 0);
            *stiff = o->interact_mode == eOmc_interactionmode_stiff;
            o->err = 0;
            return 0.f;
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
            else
            {
                //to be discussed
				/*
                if (o->old_pos == pos && o->openloop_out != 0)
                {
                    o->pos_stable++;
                    //cannot reach the set limit, but it's safer to stop the PWM
                    if (o->pos_stable == 20)
                        o->openloop_limitreached = o->openloop_out;
                    
                    if ((o->openloop_limitreached >= 0) ^ (o->openloop_out < 0))
                    return 0;                    
                    
                }
                */
                
                //inside safe band, reset values
                o->openloop_limitreached = 0;
                o->pos_stable = 0;
                o->old_pos = pos;
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
			#ifdef EXPERIMENTAL_SPEEDCONTROL
            if (o->time_raw <= 25)
            {
                ++(o->time_raw);
            }
            else
            {
                o->vel_raw = 0;
            }
            #endif
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
                
                #ifdef EXPERIMENTAL_SPEED_CONTROL
                
                if (o->control_mode == eomc_controlmode_direct)
                {
                    return 0.039f*(float)(5*err)+(float)(o->vel_raw);
                }
                else
                {
                    if (eo_trajectory_IsDone(o->trajectory))
                    {
                        return 0.039f*(float)(5*err);
                    }
                    else
                    {
                        return 0.039f*(vel_ref+(float)(5*err));
                    }
                }
                #else
                //return eo_pid_PWM_piv(o->pidP, o->err, vel_ref-vel);
                return eo_pid_PWM_pid(o->pidP, o->err);
                #endif
            }
            else // compliant
            {
                *stiff = eobool_false;
                
                #ifdef EXPERIMENTAL_SPEED_CONTROL
                
                eo_emsController_GetDecoupledMeasuredTorque (o->axisID,&o->torque_meas_mot);
                
                int32_t displacement = o->stiffness ? (1000*o->torque_meas_mot/o->stiffness) : 0;
                
                o->err = err;
                
                return (0.038f*vel_ref+0.25f*(float)(err+displacement));
                
                #else
                
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
                
                #endif
            }
        }
        case eomc_controlmode_torque:
        {
            *stiff = eobool_false;
            
            o->err = 0;
            
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
            
            #ifdef EXPERIMENTAL_SPEED_CONTROL
            
            float pwm_out = (0.0001f*(float)o->damping)*(o->torque_meas_mot-o->torque_off);
            
            #else 
            
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
            #endif
            
            return pwm_out;
        } 
    }
    
    return 0.f;
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
    return o ? o->trajectory : NULL;
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
    
    jointStatus->modes.interactionmodestatus    =  o->interact_mode;
    jointStatus->modes.controlmodestatus        = o->control_mode;
    jointStatus->modes.ismotiondone             = eo_axisController_GetMotionDone(o);
    jointStatus->basic.jnt_position             = GET_AXIS_POSITION();           
    jointStatus->basic.jnt_velocity             = GET_AXIS_VELOCITY();        
    #warning acceleration to be implemented
    jointStatus->basic.jnt_acceleration         = 0; //eo_speedometer_GetAcceleration(o->speedmeter);       
    jointStatus->basic.jnt_torque               = o->torque_meas_jnt;
}

extern void eo_axisController_GetActivePidStatus(EOaxisController *o, eOmc_joint_status_ofpid_t* pidStatus)
{
    if (o->control_mode == eomc_controlmode_idle)
    {
        pidStatus->generic.reference1   = 0;
        pidStatus->generic.reference2   = 0;
        pidStatus->generic.error1       = 0;
        pidStatus->generic.error2       = 0;
        pidStatus->generic.output       = 0;
        
        return;
    }
    
    if (o->control_mode == eomc_controlmode_openloop)
    {
        //to be reviewed
        pidStatus->openloop.refolo      = o->openloop_out;
        pidStatus->openloop.dummyref2   = 0;
        pidStatus->openloop.dummyerr1   = 0;
        pidStatus->openloop.dummyerr2   = 0;
        pidStatus->openloop.output      = o->openloop_out; //could be updated inside control loop in case of exception

        return;
    }
    
    if (o->control_mode == eomc_controlmode_torque)
    {
        pidStatus->torque.dummyref1 = 0;
        pidStatus->torque.reftrq    = o->torque_ref_jnt;
        pidStatus->torque.dummyerr1 = 0;
        eo_pid_GetStatusInt32(o->pidT, &(pidStatus->torque.output), &(pidStatus->torque.errtrq));
        return;
    }
    
    if (o->interact_mode == eOmc_interactionmode_compliant)
    {
        pidStatus->complpos.refpos = eo_trajectory_GetPos(o->trajectory);
        pidStatus->complpos.reftrq = o->torque_ref_jnt;
        pidStatus->complpos.errpos = 0; //ask alessandro if it's needed or not      
        eo_pid_GetStatusInt32(o->pidT, &(pidStatus->complpos.output), &(pidStatus->complpos.errtrq));
        return;
    }
    
    // stiff position modes 
 
    pidStatus->stiffpos.refpos = eo_trajectory_GetPos(o->trajectory);
    pidStatus->stiffpos.dummyref2 = 0;
    pidStatus->stiffpos.dummyerr2 = 0;
    eo_pid_GetStatusInt32(o->pidP, &(pidStatus->stiffpos.output), &(pidStatus->stiffpos.errpos));    
    
}

extern void eo_axisController_RescaleAxisPosition(EOaxisController *o, int32_t current_pos)
{
    if (!o) return;
    
    int32_t pos = current_pos - o->offset;
    
    /* disabled */
    // out of bound protections (active only if I'm not calibrating or not configured)
    /*
    if ((o->control_mode != eomc_controlmode_calib) && (o->control_mode != eomc_controlmode_notConfigured))
    {
        if (pos < (o->pos_min - TICKS_PER_HALF_REVOLUTION))
            pos += TICKS_PER_REVOLUTION;
        else if (pos > (o->pos_max + TICKS_PER_HALF_REVOLUTION))
            pos -= TICKS_PER_REVOLUTION;
    }
    */
    //update axis pos and subtract calibration zero
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
    
    /* disabled */
    //out of bound protection
    /*
    if (pos < (o->pos_min - TICKS_PER_HALF_REVOLUTION))
        pos += TICKS_PER_REVOLUTION;
    else if (pos > (o->pos_max + TICKS_PER_HALF_REVOLUTION))
        pos -= TICKS_PER_REVOLUTION;     
    */
    
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
    if (o->calibration_type == eomc_calibration_type5_hard_stops_mc4plus)
    {
        //if for 20 consecutive times (~20ms) I'm in the same position (but let the calibration start before...), it means that I reached the hardware limit
        o->calib_count += 1;
        if (o->calib_count > 1200)
        {
            if (GET_AXIS_POSITION() == o->old_pos)
            {
                o->pos_stable += 1;
                if (o->pos_stable == 20)
                {
                    o->pos_stable = 0;
                    o->calib_count = 0;
                    return eobool_true;
                }
            }
            else
            {
                o->pos_stable = 0;
            }
        }
        o->old_pos = GET_AXIS_POSITION();
        return eobool_false;
    }
    else if (o->calibration_type == eomc_calibration_type8_adc_and_incr_mc4plus)
    {
        //DEBUG PRINTING
        /*
        static uint16_t count_deb = 0;
        count_deb++;
        
        if (count_deb == 500)
        {
            uint32_t deb_voltage = eo_mcserv_GetMotorAnalogSensor(eo_mcserv_GetHandle(), o->axisID);
            eOerrmanDescriptor_t errdes = {0};
            errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
            errdes.par16            = deb_voltage;
            errdes.par64            = 0;
            errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress    = o->axisID;     
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &errdes);
            
            count_deb = 0;
        }
        */
        
        //must be 0 or a value close to 0?
        #warning voltage threshold to set hardware-limit reached is hardcoded, but it could become a calib param
        if (eo_mcserv_GetMotorAnalogSensor(eo_mcserv_GetHandle(), o->axisID) < 500) 
            return eobool_true;
        else
            return eobool_false;
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




