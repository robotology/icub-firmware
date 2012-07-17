
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
#include "EOtheErrorManager.h"
#include "EOVtheSystem.h"



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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

const int32_t EMS_IFREQUENCY = 1000;
const float   EMS_FFREQUENCY = 1000.0f;
const float   EMS_PERIOD     = 0.001f;

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

static const char s_eobj_ownname[] = "EOemsController";

static EOemsController *s_emsc = NULL;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOemsController* eo_emsController_Init(emsBoardType_t board_type) 
{
    s_emsc = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsController), 1);

    if (s_emsc)
    {
        for (uint8_t m=0; m<MAX_MOTORS; ++m)
        {
            s_emsc->axis_controller[m] = NULL;     
        }

        s_emsc->decoupler[DECOUPLER_POS] = NULL;
        s_emsc->decoupler[DECOUPLER_TRQ] = NULL;
        s_emsc->decoupler[DECOUPLER_PWM] = NULL;

        switch (board_type)
        {
            case EMS_GENERIC:
                break;

            case EMS_WAIST:
                eo_emsController_SetDecoupler(DECOUPLER_PWM, decoupler_waist_pwm);
                break;

            case EMS_SHOULDER:
                eo_emsController_SetDecoupler(DECOUPLER_POS, decoupler_shoulder_pos);
                eo_emsController_SetDecoupler(DECOUPLER_TRQ, decoupler_shoulder_trq);
                eo_emsController_SetDecoupler(DECOUPLER_PWM, decoupler_shoulder_pwm);
                break;
        }
    }

    return s_emsc;
}

extern eObool_t eo_emsController_AddAxis(uint8_t naxis)
{
    if (naxis >= MAX_MOTORS) return eobool_false;

    if (s_emsc->axis_controller[naxis] != NULL) return eobool_false;

    s_emsc->axis_controller[naxis] = eo_axisController_New();

    return eobool_true;
}

extern void eo_emsController_ReadEncoders(int32_t *enc)
{
    static float axis_pos[MAX_MOTORS];

    float *pos = s_emsc->encoder_pos;

    for (int i=0; i<MAX_MOTORS; ++i)
    {
        s_emsc->encoder_pos[i] = (float)enc[i];
    }

    if (s_emsc->decoupler[DECOUPLER_POS])
    {
        eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_POS], s_emsc->encoder_pos, axis_pos);
     
        pos = axis_pos;       
    }

    for (uint8_t i=0; i<MAX_MOTORS; ++i)
    {
        if (s_emsc->axis_controller[i])
        {
            eo_axisController_ReadEncPos(s_emsc->axis_controller[i], pos[i]);
        }
    }
}

extern void eo_emsController_SkipEncoders(void)
{
    for (uint8_t i=0; i<MAX_MOTORS; ++i)
    {
        if (s_emsc->axis_controller[i])
        {
            eo_axisController_SkipEncPos(s_emsc->axis_controller[i]);
        }
    }
}

extern void eo_emsController_ReadSpeed(uint8_t axis, int32_t speed)
{
    eo_axisController_ReadSpeed(s_emsc->axis_controller[axis], speed);    
}

extern float eo_emsController_GetVelocity(uint8_t joint)
{
    return eo_axisController_GetVelocity(s_emsc->axis_controller[joint]);                
}

extern void eo_emsController_ReadTorques(int32_t *torque)
{
    for (int i=0; i<MAX_MOTORS; ++i)
    {
        s_emsc->torque_meas[i] = (float)torque[i];
    }

    float *trq = s_emsc->torque_meas;
    static float axis_trq[MAX_MOTORS];

    if (s_emsc->decoupler[DECOUPLER_TRQ])
    {
        eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_TRQ], s_emsc->torque_meas, axis_trq);
        trq = axis_trq;
    }

    for (uint8_t i=0; i<MAX_MOTORS; ++i)
    {
        if (s_emsc->axis_controller[i])
        {
            eo_axisController_ReadTorque(s_emsc->axis_controller[i], trq[i]);
        }
    }
}

extern void eo_emsController_SetOffset(uint8_t joint, int16_t off)
{
    eo_axisController_SetOffset(s_emsc->axis_controller[joint], off);    
}

extern void eo_emsController_SetPosRef(uint8_t joint, int32_t pos, int32_t avg_vel)
{
    eo_axisController_SetPosRef(s_emsc->axis_controller[joint], pos, avg_vel);
}

extern void eo_emsController_SetVelRef(uint8_t joint, int32_t vel, int32_t avg_acc)
{
    eo_axisController_SetVelRef(s_emsc->axis_controller[joint], vel, avg_acc);
}

extern void eo_emsController_SetTrqRef(uint8_t joint, int32_t trq)
{
    eo_axisController_SetTrqRef(s_emsc->axis_controller[joint], trq);
}

extern int16_t* eo_emsController_PWM()
{
    static int16_t pwm_axis[MAX_MOTORS];
    static int16_t pwm_motor[MAX_MOTORS];

    for (uint8_t i=0; i<MAX_MOTORS; ++i)
    {
        if (s_emsc->axis_controller[i])
        {
            pwm_axis[i] = eo_axisController_PWM(s_emsc->axis_controller[i]);
        }
    }

    if (s_emsc->decoupler[DECOUPLER_PWM])
    {
        //eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_PWM], pwm_axis, pwm_motor);
        
        return pwm_motor;    
    }

    return pwm_axis;
}


extern void eo_emsController_SetControlMode(uint8_t joint, control_mode_t mode)
{
    eo_axisController_SetControlMode(s_emsc->axis_controller[joint], mode);
}

extern void eo_emsController_ResetPosPid(uint8_t joint)
{
    eo_pid_Reset(eo_axisController_GetPosPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsController_StartCalibration(uint8_t joint, int32_t pos, int32_t offset, int32_t timeout_ms, int32_t max_error)
{
    eo_axisController_StartCalibration(s_emsc->axis_controller[joint], pos, offset, timeout_ms, max_error);
}

extern void eo_emsController_Stop(uint8_t joint, int32_t stop_acc)
{
    eo_axisController_Stop(s_emsc->axis_controller[joint], stop_acc);
}

extern void eo_emsController_ResetTrqPid(uint8_t joint)
{
    eo_pid_Reset(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsGetActivePidStatus(uint8_t joint, int16_t *pwm, int32_t *err)
{
    eo_axisController_GetActivePidStatus(s_emsc->axis_controller[joint], pwm, err);    
}

extern void eo_emsController_SetDecoupler(emsMotorDecoupler_t dec_type, float matrix[4][4])
{
    s_emsc->decoupler[dec_type] = eo_decoupler_New(MAX_MOTORS, matrix);
}

///////////////////////
// PID configurations

extern void eo_emsController_SetPosPid(uint8_t joint, float k, float kd, float ki)
{
    eo_pid_SetPid(eo_axisController_GetPosPidPtr(s_emsc->axis_controller[joint]), k, kd, ki);    
}   
extern void eo_emsController_SetPosPidLimits(uint8_t joint, float Ymax, float Imax)
{
    eo_pid_SetPidLimits(eo_axisController_GetPosPidPtr(s_emsc->axis_controller[joint]), Ymax, Imax);    
}

extern void eo_emsController_SetTrqPid(uint8_t joint, float k, float kd, float ki)
{
    eo_pid_SetPid(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]), k, kd, ki);    
}   
extern void eo_emsController_SetTrqPidLimits(uint8_t joint, float Ymax, float Imax)
{
    eo_pid_SetPidLimits(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]), Ymax, Imax);    
}

// PID configurations
///////////////////////


extern void eo_emsController_SetStiffness(uint8_t joint, int32_t stiffness)
{
    eo_axisController_SetStiffness(s_emsc->axis_controller[joint], stiffness);
}

extern void eo_emsController_SetVelTimeout(int32_t vel_timeout)
{
    for (int joint=0; joint<MAX_MOTORS; ++joint)
    {
        if (s_emsc->axis_controller[joint])
        {
            eo_axisController_SetVelTimeout(s_emsc->axis_controller[joint], vel_timeout);
        }
    }
}

extern void eo_emsController_SetLimits(uint8_t joint, int32_t pos_min, int32_t pos_max, int32_t vel_max)
{
    eo_axisController_SetLimits(s_emsc->axis_controller[joint], pos_min, pos_max, vel_max);
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




