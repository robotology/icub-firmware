
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

extern EOemsController* eo_emsController_Init(uint8_t nmotors, emsBoardType_t board_type) 
{
    if (nmotors > MAX_MOTORS) return NULL;

    s_emsc = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsController), 1);

    if (s_emsc)
    {
        s_emsc->nmotors = nmotors;
        
        while (nmotors)
        {
            s_emsc->axis_controller[--nmotors] = eo_axisController_New();     
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

    s_emsc->flag_encoder_pos_dirty = eobool_false;
    s_emsc->flag_torque_meas_dirty = eobool_false;

    return s_emsc;
}

extern void eo_emsController_AxisConfigure(uint8_t joint, eOmc_joint_config_t *c)
{
    eo_axisController_Configure(s_emsc->axis_controller[joint], c);
}

extern void eo_emsController_AxisCALIBRATE(uint8_t joint, eOmc_calibrator_t *c)
{
    s_emsc->flag_encoder_pos_dirty = eobool_true;

    eo_axisController_Calibrate(s_emsc->axis_controller[joint], c);
}

extern void eo_emsController_ReadEncoder(uint8_t joint, int32_t pos)
{
    s_emsc->encoder_pos[joint] = (float)pos;
    
    s_emsc->flag_encoder_pos_dirty = eobool_true;
}

extern void eo_emsController_ReadTorque(uint8_t joint, int32_t torque)
{
    s_emsc->torque_meas[joint] = (float)torque;

    s_emsc->flag_torque_meas_dirty = eobool_true;
}

extern void eo_emsController_SetPosRef(uint8_t joint, float pos, float vel)
{
    eo_axisController_SetPosRef(s_emsc->axis_controller[joint], pos, vel);
}

extern void eo_emsController_SetVelRef(uint8_t joint, float vel, float acc)
{
    eo_axisController_SetVelRef(s_emsc->axis_controller[joint], vel, acc);
}

extern void eo_emsController_SetTrqRef(uint8_t joint, float trq)
{
    eo_axisController_SetTrqRef(s_emsc->axis_controller[joint], trq);
}

extern float* eo_emsController_PWM()
{
    static float axis_pos[MAX_MOTORS];
    static float axis_trq[MAX_MOTORS];

    static float pwm_axis[MAX_MOTORS];
    static float pwm_motor[MAX_MOTORS];

    /*
    if (s->emsc->flag_encoder_pos_dirty)
    {
        float *pos = s_emsc->encoder_pos;
        s_emsc->flag_encoder_pos_dirty = 0;

        if (s_emsc->board_type == EMS_SHOULDER)
        {
            
        }    
    }
    */

    if (s_emsc->flag_encoder_pos_dirty)
    {
        float *pos = s_emsc->encoder_pos;

        s_emsc->flag_encoder_pos_dirty = eobool_false;

        if (s_emsc->decoupler[DECOUPLER_POS])
        {
            eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_POS], s_emsc->encoder_pos, axis_pos);
            pos = axis_pos;       
        }

        for (uint8_t i=0; i<s_emsc->nmotors; ++i)
        {
            eo_axisController_ReadEncPos(s_emsc->axis_controller[i], pos[i]);
        }
    }

    if (s_emsc->flag_torque_meas_dirty)
    { 
        float *trq = s_emsc->torque_meas;

        s_emsc->flag_torque_meas_dirty = eobool_false;

        if (s_emsc->decoupler[DECOUPLER_TRQ])
        {
            eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_TRQ], s_emsc->torque_meas, axis_trq);
            trq = axis_trq;
        }

        for (uint8_t i=0; i<s_emsc->nmotors; ++i)
        {
            eo_axisController_ReadTorque(s_emsc->axis_controller[i], trq[i]);
        }
    }

    for (uint8_t i=0; i<s_emsc->nmotors; ++i)
    {
        pwm_axis[i] = eo_axisController_PWM(s_emsc->axis_controller[i]);
    }

    if (s_emsc->decoupler[DECOUPLER_PWM])
    {
        eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_PWM], pwm_axis, pwm_motor);
        
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

extern void eo_emsController_ResetTrqPid(uint8_t joint)
{
    eo_pid_Reset(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsGetActivePidStatus(uint8_t joint, float *pwm, float *err)
{
    eo_axisController_GetActivePidStatus(s_emsc->axis_controller[joint], pwm, err);    
}

extern void eo_emsController_SetDecoupler(emsMotorDecoupler_t dec_type, float matrix[4][4])
{
    s_emsc->decoupler[dec_type] = eo_decoupler_New(s_emsc->nmotors, matrix);
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




