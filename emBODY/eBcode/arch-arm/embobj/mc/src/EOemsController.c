
/*  @file       EOdecoupler.c
    @brief      This file implements internal implementation of motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "hal_led.h"

#include <string.h>

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOemsController.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOemsController_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define TICKS_PER_REVOLUTION 0x10000

#define JOINTS(j)   for (uint8_t j=0; j<s_emsc->n_joints; ++j)
#define MOTORS(m)   for (uint8_t m=0; m<s_emsc->n_joints; ++m)
#define ENCODERS(e) for (uint8_t e=0; e<s_emsc->n_joints; ++e)

#define SAFE_MAX_CURRENT 1500
#define LIMIT(x,L) if (x>(L)) x=(L); else if (x<-(L)) x=-(L)

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

const float   EMS_PERIOD          = 0.001f;
const int32_t EMS_FREQUENCY_INT32 = 1000;
const float   EMS_FREQUENCY_FLOAT = 1000.0f;


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

//static const char s_eobj_ownname[] = "EOemsController";

static EOemsController *s_emsc = NULL;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOemsController* eo_emsController_Init(emsBoardType_t board_type) 
{    
    //board_type = EMS_UPPERLEG;
    
    if (board_type == EMS_NULL) return NULL;

    s_emsc = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsController), 1);

    if (s_emsc)
    {
        s_emsc->boardType  = board_type;
        s_emsc->defcon     = EMS_PRUDENT;
        s_emsc->n_joints   = MAX_JOINTS;
        s_emsc->is_coupled = eobool_false;
        
        s_emsc->cable_length_alarm = eobool_false;
        
        JOINTS(j)
        {
            s_emsc->axis_controller[j] = NULL;
            s_emsc->enc_speedometer[j] = NULL;
        }
        
        switch (s_emsc->boardType)
        {
            case EMS_GENERIC:            
                break;
            
            case EMS_SHOULDER: 
                s_emsc->n_joints   = 4;
                s_emsc->is_coupled = eobool_true;
                break;
            
            case EMS_WAIST:    
                s_emsc->n_joints   = 3;          
                s_emsc->is_coupled = eobool_true;
                break;

            case EMS_UPPERLEG:
                s_emsc->is_coupled = eobool_false;
                s_emsc->n_joints   = 4;          
                break;
            
            case EMS_ANKLE:
                s_emsc->is_coupled = eobool_false;
                s_emsc->n_joints   = 2;          
                break;
            
            default:           
                s_emsc->n_joints = 0;
        }
        
        s_emsc->motors = eo_motors_New(s_emsc->n_joints);
        
        JOINTS(j)
        {
            s_emsc->axis_controller[j] = eo_axisController_New(TICKS_PER_REVOLUTION);
            s_emsc->enc_speedometer[j] = eo_speedmeter_New();
        }
    }
    
    return s_emsc;
}

extern void eo_emsController_SetEncSign(uint16_t jxx, int32_t enc_sign)
{
    eo_speedometer_SetEncSign(s_emsc->enc_speedometer[jxx], enc_sign);
}

extern void eo_emsController_ReadEncoders(int32_t *enc_pos)
{
    if (!s_emsc) return;
    
    int32_t enc_vel[MAX_JOINTS];
    
    ENCODERS(e)
    {
        eo_speedometer_SlowEncoderRead(s_emsc->enc_speedometer[e], enc_pos[e]);
        
        enc_pos[e] = eo_speedometer_GetDistance(s_emsc->enc_speedometer[e]);
        enc_vel[e] = eo_speedometer_GetVelocity(s_emsc->enc_speedometer[e]);
    }

    switch (s_emsc->boardType)
    {
    case EMS_SHOULDER:
    {
        
        // |J0|   |  1     0    0   |   |E0|     
		// |J1| = |  0     1    0   | * |E1|     with a=40/65
		// |J2|   |  1    -1    a   |   |E2|
    
        int32_t jnt_posI2I = enc_pos[0]-enc_pos[1]+(40*enc_pos[2])/65;
    
        //s_emsc->cable_length_alarm = eo_motors_CableLimitAlarm(enc_pos[0], enc_pos[1], jnt_posI2I);
        
        eo_axisController_SetEncPos(s_emsc->axis_controller[0], enc_pos[0]);
        eo_axisController_SetEncPos(s_emsc->axis_controller[1], enc_pos[1]);
        eo_axisController_SetEncPos(s_emsc->axis_controller[2], jnt_posI2I);
        eo_axisController_SetEncPos(s_emsc->axis_controller[3], enc_pos[3]);

        #ifdef USE_2FOC_FAST_ENCODER
        ERROR: fast encoder shoulder geometry TBD!!!
        #else
        eo_axisController_SetEncVel(s_emsc->axis_controller[0], enc_vel[0]);
        eo_axisController_SetEncVel(s_emsc->axis_controller[1], enc_vel[1]);
        eo_axisController_SetEncVel(s_emsc->axis_controller[2], enc_vel[0]-enc_vel[1]+(40*enc_vel[2])/65);
        eo_axisController_SetEncVel(s_emsc->axis_controller[3], enc_vel[3]);
        #endif
    
        break;
    }
    case EMS_UPPERLEG:
    case EMS_WAIST:
    case EMS_ANKLE:
    default:
        JOINTS(j)
        {
            eo_axisController_SetEncPos(s_emsc->axis_controller[j], enc_pos[j]);
            eo_axisController_SetEncVel(s_emsc->axis_controller[j], enc_vel[j]);
        }
    }
}

extern void eo_emsController_ReadTorques(int32_t *trq_measures)
{
    if (!s_emsc) return;
    
    JOINTS(j)
    {
        eo_axisController_SetTorque(s_emsc->axis_controller[j], trq_measures[j]);
    }
}

#ifdef USE_2FOC_FAST_ENCODER
extern void eo_emsController_ReadSpeed(uint8_t axis, int32_t speed)
{
    if (s_emsc) eo_speedometer_FastEncoderRead(s_emsc->enc_speedometer[axis], speed);
}
#endif

extern void eo_emsController_PWM(int16_t* pwm_motor)
{
    if (!s_emsc)
    {
        for (uint8_t m=0; m<4; ++m) pwm_motor[m] = 0;
        
        return;
    }
    
    if (s_emsc->defcon == EMS_ALARM)
    {
        static uint8_t time = 0;
        if (!++time) hal_led_toggle(hal_led0); // flash red light
        
        MOTORS(m) pwm_motor[m] = 0;
        
        return;
    }
    
    EMSdefcon_t defcon = EMS_ALL_OK;
    
    uint8_t alarm_mask = s_emsc->cable_length_alarm ?  0x07 : 0x00;
    
    JOINTS(j)
    {
        if (eo_speedometer_IsHardFault(s_emsc->enc_speedometer[j]))
        {
            s_emsc->defcon = EMS_ALARM;
            MOTORS(m) pwm_motor[m] = 0;
            return;
        }
        
        if (!eo_speedometer_IsStarted(s_emsc->enc_speedometer[j]) || !eo_axisController_IsReady(s_emsc->axis_controller[j]))
        {            
            alarm_mask |= 1<<j;
            s_emsc->defcon = defcon = EMS_PRUDENT;
        }
    }
    
    if (s_emsc->defcon == EMS_PRUDENT && defcon == EMS_ALL_OK)
    {
        JOINTS(j) eo_axisController_Stop(s_emsc->axis_controller[j]);
        
        s_emsc->defcon = EMS_ALL_OK;
    }
       
    int32_t pwm_joint[MAX_JOINTS];

    JOINTS(j)
    {
        eObool_t big_error_flag = eobool_false;
        
        pwm_joint[j] = eo_axisController_PWM(s_emsc->axis_controller[j], &big_error_flag);
        
        if (big_error_flag) alarm_mask |= 1<<j;
    }
    
    eo_motors_PWM(s_emsc->boardType, pwm_joint, pwm_motor, alarm_mask);
}





    /*
    for (uint8_t m=0; m<4; ++m)
    {
        if (!eo_motors_is_motorON(s_emsc->motors, m))
        {
            JOINTS(j)
            {
                eo_axisController_SetControlMode(s_emsc->axis_controller[j], eomc_controlmode_cmd_switch_everything_off);
            }
            
            break;
        }
    }
    */

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


extern void eo_emsController_SetOutput(uint8_t joint, int16_t out)
{
    if (s_emsc) eo_axisController_SetOutput(s_emsc->axis_controller[joint], out);    
}

extern void eo_emsController_SetPosRef(uint8_t joint, int32_t pos, int32_t avg_vel)
{
    if (joint == 0) hal_led_toggle(hal_led1); // green light
        
    if (s_emsc) eo_axisController_SetPosRef(s_emsc->axis_controller[joint], pos, avg_vel);
}

extern void eo_emsController_SetVelRef(uint8_t joint, int32_t vel, int32_t avg_acc)
{
    if (s_emsc) eo_axisController_SetVelRef(s_emsc->axis_controller[joint], vel, avg_acc);
}

extern void eo_emsController_SetTrqRef(uint8_t joint, int32_t trq)
{
    if (s_emsc) eo_axisController_SetTrqRef(s_emsc->axis_controller[joint], trq);
}

extern void eo_emsController_SetControlMode(uint8_t joint, eOmc_controlmode_command_t mode)
{
    if (s_emsc) eo_axisController_SetControlMode(s_emsc->axis_controller[joint], mode);
}

extern eOmc_controlmode_t eo_emsController_GetControlMode(uint8_t joint)
{
    if (!s_emsc) return eomc_controlmode_idle;

    return eo_axisController_GetControlMode(s_emsc->axis_controller[joint]);
}

extern void eo_emsController_ResetPosPid(uint8_t joint)
{
    if (s_emsc) eo_pid_Reset(eo_axisController_GetPosPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsController_StartCalibration(uint8_t joint, int32_t pos, int32_t vel, int32_t offset)
{
    if (s_emsc)
    {
        eo_speedometer_Calibrate(s_emsc->enc_speedometer[joint], offset);
        
        eo_axisController_StartCalibration(s_emsc->axis_controller[joint], pos, vel);
    }
}

extern void eo_emsController_Stop(uint8_t joint)
{
    if (s_emsc) eo_axisController_Stop(s_emsc->axis_controller[joint]);
}

extern void eo_emsController_ResetTrqPid(uint8_t joint)
{
    if (s_emsc) eo_pid_Reset(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsController_GetActivePidStatus(uint8_t joint, eOmc_joint_status_ofpid_t* pidStatus)
{
    if (s_emsc && s_emsc->axis_controller[joint])
    {
        eo_axisController_GetActivePidStatus(s_emsc->axis_controller[joint], pidStatus);
    }
    else
    {
        pidStatus->error     = 0;
        pidStatus->output    = 0;
        pidStatus->reference = 0;
    }
}

extern void eo_emsController_GetJointStatus(uint8_t joint, eOmc_joint_status_basic_t* jointStatus)
{
    if (s_emsc && s_emsc->axis_controller[joint])
    {
        eo_axisController_GetJointStatus(s_emsc->axis_controller[joint], jointStatus);
    }
    else
    {
        jointStatus->position            = 0;  // the position of the joint           
        jointStatus->velocity            = 0;  // the velocity of the joint          
        jointStatus->acceleration        = 0;  // the acceleration of the joint       
        jointStatus->torque              = 0;  // the torque of the joint when locally measured
        jointStatus->motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  // use eOmc_motionmonitorstatus_t. it is eomc_motionmonitorstatus_notmonitored unless the monitor is activated in jconfig.motionmonitormode  
        jointStatus->controlmodestatus   = eomc_controlmode_idle;  // use eOmc_controlmode_t. it is a readonly shadow copy of jconfig.controlmode used to remind the host of teh current controlmode
    }
}

extern eObool_t eo_emsController_GetMotionDone(uint8_t joint)
{
    if (s_emsc && s_emsc->axis_controller[joint])
    {
        return eo_axisController_GetMotionDone(s_emsc->axis_controller[joint]);
    }
    else
    {
        return eobool_false;
    }
}

///////////////////////
// PID configurations
extern void eo_emsController_SetPosPid(uint8_t joint, float K, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    if (s_emsc) eo_axisController_SetPosPid(s_emsc->axis_controller[joint], K, Kd, Ki, Imax, Ymax, Yoff);    
}   
extern void eo_emsController_SetTrqPid(uint8_t joint, float K, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    if (s_emsc) eo_axisController_SetTrqPid(s_emsc->axis_controller[joint], K, Kd, Ki, Imax, Ymax, Yoff);    
}   
// PID configurations
///////////////////////


extern void eo_emsController_SetStiffness(uint8_t joint, int32_t stiffness)
{
    if (s_emsc) eo_axisController_SetStiffness(s_emsc->axis_controller[joint], stiffness);
}

extern void eo_emsController_SetVelTimeout(uint8_t joint, int32_t vel_timeout)
{
    if (s_emsc) eo_axisController_SetVelTimeout(s_emsc->axis_controller[joint], vel_timeout);
}

extern void eo_emsController_SetLimits(uint8_t joint, int32_t pos_min, int32_t pos_max, int32_t vel_max)
{
    if (s_emsc) eo_axisController_SetLimits(s_emsc->axis_controller[joint], pos_min, pos_max, vel_max);
}

extern void eo_emsController_SetPosMin(uint8_t joint, int32_t pos_min)
{
    if (s_emsc) eo_axisController_SetPosMin(s_emsc->axis_controller[joint], pos_min);
}

extern void eo_emsController_SetPosMax(uint8_t joint, int32_t pos_max)
{
    if (s_emsc) eo_axisController_SetPosMax(s_emsc->axis_controller[joint], pos_max);
}

extern void eo_emsController_SetVelMax(uint8_t joint, int32_t vel_max)
{
    if (s_emsc) eo_axisController_SetVelMax(s_emsc->axis_controller[joint], vel_max);
}

extern void eo_emsController_ReadMotorstatus(uint8_t motor, uint8_t motorerror, uint8_t canerror, eOmc_controlmode_t controlmode)
{
    //if (s_emsc) eo_motor_set_motor_status(s_emsc->motors, motor, !motorerror && !canerror && controlmode!=eomc_controlmode_idle);
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




