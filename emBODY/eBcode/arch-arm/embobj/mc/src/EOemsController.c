
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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

const float   EMS_PERIOD = 0.001f;
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

static const char s_eobj_ownname[] = "EOemsController";

static EOemsController *s_emsc = NULL;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOemsController* eo_emsController_Init(emsBoardType_t board_type) 
{
    if (board_type == EMS_NULL) return NULL;

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
    if (!s_emsc) return eobool_false;

    if (naxis >= MAX_MOTORS) return eobool_false;

    if (s_emsc->axis_controller[naxis] != NULL) return eobool_false;

    s_emsc->axis_controller[naxis] = eo_axisController_New();

    return eobool_true;
}

extern void eo_emsController_SkipEncoders(void)
{
    if (!s_emsc) return;

    for (uint8_t i=0; i<MAX_MOTORS; ++i)
    {
        if (s_emsc->axis_controller[i])
        {
            eo_axisController_SkipEncPos(s_emsc->axis_controller[i]);
        }
    }
}

extern void eo_emsController_ReadEncoders(int32_t *enc)
{
    static float encs_pos[MAX_MOTORS];
    static float axis_pos[MAX_MOTORS];

    if (!s_emsc) return;

    if (s_emsc->decoupler[DECOUPLER_POS])
    {
        for (uint8_t i=0; i<MAX_MOTORS; ++i)
        {
            if (enc[i]==(int32_t)ENC_INVALID)
            {
                eo_emsController_SkipEncoders();

                return;
            }
            
            encs_pos[i]=(float)enc[i];
        }

        eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_POS], encs_pos, axis_pos);
     
        for (uint8_t i=0; i<MAX_MOTORS; ++i)
        {
            if (s_emsc->axis_controller[i])
            {
                eo_axisController_ReadEncPos(s_emsc->axis_controller[i], (int32_t)axis_pos[i]);
            }
        }     
    }
    else
    {
        for (uint8_t i=0; i<MAX_MOTORS; ++i)
        {
            if (s_emsc->axis_controller[i])
            {
                if (enc[i]==(int32_t)ENC_INVALID)
                {
                    eo_axisController_SkipEncPos(s_emsc->axis_controller[i]);
                }
                else
                {
                    eo_axisController_ReadEncPos(s_emsc->axis_controller[i], enc[i]);
                }
            }
        }
    }
}

extern void eo_emsController_ReadSpeed(uint8_t axis, int32_t speed)
{
    if (!s_emsc) return;

    eo_axisController_ReadSpeed(s_emsc->axis_controller[axis], speed);    
}

extern void eo_emsController_ReadTorques(int32_t *torque)
{
    if (!s_emsc) return;

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

/*
extern float eo_emsController_GetVelocity(uint8_t joint)
{
    if (!s_emsc) return 0.0f;

    return eo_axisController_GetVelocity(s_emsc->axis_controller[joint]);                
}
*/

extern void eo_emsController_SetOutput(uint8_t joint, int16_t out)
{
    if (!s_emsc) return;

    eo_axisController_SetOutput(s_emsc->axis_controller[joint], out);    
}

extern void eo_emsController_SetPosRef(uint8_t joint, int32_t pos, int32_t avg_vel)
{
    if (!s_emsc) return;

    eo_axisController_SetPosRef(s_emsc->axis_controller[joint], pos, avg_vel);
}

extern void eo_emsController_SetVelRef(uint8_t joint, int32_t vel, int32_t avg_acc)
{
    if (!s_emsc) return;

    eo_axisController_SetVelRef(s_emsc->axis_controller[joint], vel, avg_acc);
}

extern void eo_emsController_SetTrqRef(uint8_t joint, int32_t trq)
{
    if (!s_emsc) return;

    eo_axisController_SetTrqRef(s_emsc->axis_controller[joint], trq);
}

extern void /*int16_t*/ eo_emsController_PWM(int16_t* pwm)
{
    if (!s_emsc) return;// NULL;

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
        memcpy(pwm, pwm_motor, 2*MAX_MOTORS);
        //return pwm_motor;    
    }

		memcpy(pwm, pwm_axis, 2*MAX_MOTORS);
    //return pwm_axis;
}


extern void eo_emsController_SetControlMode(uint8_t joint, control_mode_t mode)
{
    if (!s_emsc) return;

    eo_axisController_SetControlMode(s_emsc->axis_controller[joint], mode);
}

extern void eo_emsController_ResetPosPid(uint8_t joint)
{
    if (!s_emsc) return;

    eo_pid_Reset(eo_axisController_GetPosPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsController_StartCalibration(uint8_t joint, int32_t pos, int32_t vel, int32_t offset)
{
    if (!s_emsc) return;

    eo_axisController_StartCalibration(s_emsc->axis_controller[joint], pos, vel, offset);
}

extern void eo_emsController_Stop(uint8_t joint)
{
    if (!s_emsc) return;

    eo_axisController_Stop(s_emsc->axis_controller[joint]);
}

extern void eo_emsController_ResetTrqPid(uint8_t joint)
{
    if (!s_emsc) return;

    eo_pid_Reset(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]));
}

/*
extern void eo_emsGetActivePidStatus(uint8_t joint, int16_t *pwm, int32_t *err)
{
    eo_axisController_GetActivePidStatus(s_emsc->axis_controller[joint], pwm, err);    
}
*/

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
        jointStatus->controlmodestatus   = CM_IDLE;  // use eOmc_controlmode_t. it is a readonly shadow copy of jconfig.controlmode used to remind the host of teh current controlmode
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

extern void eo_emsController_SetDecoupler(emsMotorDecoupler_t dec_type, float matrix[4][4])
{
    if (!s_emsc) return;

    s_emsc->decoupler[dec_type] = eo_decoupler_New(MAX_MOTORS, matrix);
}

///////////////////////
// PID configurations
extern void eo_emsController_SetPosPid(uint8_t joint, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff)
{
    if (!s_emsc) return;

    eo_axisController_SetPosPid(s_emsc->axis_controller[joint], K, Kd, Ki, Ymax, Imax, Yoff);    
}   
extern void eo_emsController_SetTrqPid(uint8_t joint, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff)
{
    if (!s_emsc) return;

    eo_axisController_SetTrqPid(s_emsc->axis_controller[joint], K, Kd, Ki, Ymax, Imax, Yoff);    
}   
// PID configurations
///////////////////////


extern void eo_emsController_SetStiffness(uint8_t joint, int32_t stiffness)
{
    if (!s_emsc) return;

    eo_axisController_SetStiffness(s_emsc->axis_controller[joint], stiffness);
}

extern void eo_emsController_SetVelTimeout(uint8_t joint, int32_t vel_timeout)
{
    if (!s_emsc) return;

    eo_axisController_SetVelTimeout(s_emsc->axis_controller[joint], vel_timeout);
}

extern void eo_emsController_SetLimits(uint8_t joint, int32_t pos_min, int32_t pos_max, int32_t vel_max)
{
    if (!s_emsc) return;

    eo_axisController_SetLimits(s_emsc->axis_controller[joint], pos_min, pos_max, vel_max);
}

extern void eo_emsController_SetPosMin(uint8_t joint, int32_t pos_min)
{
    if (!s_emsc) return;

    eo_axisController_SetPosMin(s_emsc->axis_controller[joint], pos_min);
}

extern void eo_emsController_SetPosMax(uint8_t joint, int32_t pos_max)
{
    if (!s_emsc) return;

    eo_axisController_SetPosMax(s_emsc->axis_controller[joint], pos_max);
}

extern void eo_emsController_SetVelMax(uint8_t joint, int32_t vel_max)
{
    if (!s_emsc) return;

    eo_axisController_SetVelMax(s_emsc->axis_controller[joint], vel_max);
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




