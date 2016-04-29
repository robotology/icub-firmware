#include "Calibrators.h"
#include "JointSet.h"
#include "CalibrationHelperData.h"
#include "hal_adc.h"

BOOL JointSet_do_wait_calibration_3(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        if (!Motor_is_calibrated(o->motor+o->motors_of_set[ms])) 
        {
            calibrated = FALSE;
        }
    }
    
    return calibrated;
}

BOOL JointSet_do_wait_calibration_5(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        if (!Motor_is_calibrated(o->motor+o->motors_of_set[ms])) 
        {
            calibrated = FALSE;
            
            Motor_do_calibration_hard_stop(o->motor+o->motors_of_set[ms]);
        }
    }
    
    return calibrated;
}

BOOL JointSet_do_wait_calibration_8(JointSet* o)
{    
    BOOL calibrated = TRUE;
    
    int32_t pos[3];
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        int m = o->motors_of_set[ms];

        if (hal_adc_get_hall_sensor_analog_input_mV(o->motor[m].actuatorPort) < 1500)
        {
            o->motor[m].not_calibrated = FALSE;
        }
        
        if (Motor_is_calibrated(o->motor+m))
        {
            Motor_set_pwm_ref(o->motor+m, 0);
            
            pos[ms] = 0x7FFFFFFF;
        }
        else
        {
            pos[ms] = o->motor[m].pos_fbk - o->tripod_calib.start_pos[ms];
            
            //Motor_set_pwm_ref(o->motor+m, o->tripod_calib.pwm);
            
            calibrated = FALSE;
        }
    }
    
    if (calibrated)
    {        
        o->motor[o->motors_of_set[0]].pos_calib_offset += o->motor[o->motors_of_set[0]].pos_fbk - o->tripod_calib.zero;
        o->motor[o->motors_of_set[1]].pos_calib_offset += o->motor[o->motors_of_set[1]].pos_fbk - o->tripod_calib.zero;
        o->motor[o->motors_of_set[2]].pos_calib_offset += o->motor[o->motors_of_set[2]].pos_fbk - o->tripod_calib.zero;
        
        o->motor[o->motors_of_set[0]].pos_fbk = o->tripod_calib.zero;
        o->motor[o->motors_of_set[1]].pos_fbk = o->tripod_calib.zero;
        o->motor[o->motors_of_set[2]].pos_fbk = o->tripod_calib.zero;

        o->motor[o->motors_of_set[0]].pos_fbk_old = o->motor[o->motors_of_set[0]].pos_fbk;
        o->motor[o->motors_of_set[1]].pos_fbk_old = o->motor[o->motors_of_set[1]].pos_fbk;
        o->motor[o->motors_of_set[2]].pos_fbk_old = o->motor[o->motors_of_set[2]].pos_fbk;
        
        return TRUE;
    }
    
    float pwm[3];
    
    if (pos[0]<0) pos[0] = -pos[0];
    if (pos[1]<0) pos[1] = -pos[1];
    if (pos[2]<0) pos[2] = -pos[2];
    
    int32_t posL = pos[0];
    if (pos[1] < posL) posL = pos[1];
    if (pos[2] < posL) posL = pos[2];
    
    float pwm_calib = (float)o->tripod_calib.pwm;
    
    if (pwm_calib < 0.0f) pwm_calib = -pwm_calib;
    
    pwm[0] = pwm_calib * (1.0f - (float)(pos[0] - posL)/(float)o->tripod_calib.max_delta);
    pwm[1] = pwm_calib * (1.0f - (float)(pos[1] - posL)/(float)o->tripod_calib.max_delta);
    pwm[2] = pwm_calib * (1.0f - (float)(pos[2] - posL)/(float)o->tripod_calib.max_delta);
    
    if (pwm[0] < 0.0f) pwm[0] = 0.0f;
    if (pwm[1] < 0.0f) pwm[1] = 0.0f;
    if (pwm[2] < 0.0f) pwm[2] = 0.0f;
    
    float pwm_min = pwm[0];
    if (pwm[1] < pwm_min) pwm_min = pwm[1];
    if (pwm[2] < pwm_min) pwm_min = pwm[2];

    if (o->tripod_calib.pwm < 0)
    {
        pwm[0] = -pwm[0];
        pwm[1] = -pwm[1];
        pwm[2] = -pwm[2];
    }
    
    Motor_set_pwm_ref(o->motor+o->motors_of_set[0], (int32_t)pwm[0]);
    Motor_set_pwm_ref(o->motor+o->motors_of_set[1], (int32_t)pwm[1]);
    Motor_set_pwm_ref(o->motor+o->motors_of_set[2], (int32_t)pwm[2]);

    return FALSE;
}

BOOL JointSet_do_wait_calibration_9(JointSet* o)
{
    if (o->calibration_wait<250)
    {
        Motor_set_pwm_ref(o->motor+o->motors_of_set[0], o->tripod_calib.pwm);
        Motor_set_pwm_ref(o->motor+o->motors_of_set[1], o->tripod_calib.pwm);
        Motor_set_pwm_ref(o->motor+o->motors_of_set[2], o->tripod_calib.pwm);
        ++o->calibration_wait;
        return FALSE;
    }
    
    BOOL calibrated = TRUE;
    
    int32_t pos[3];
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        int m = o->motors_of_set[ms];
        
        if (Motor_is_calibrated(o->motor+m))
        {
            Motor_set_pwm_ref(o->motor+m, 0);
            
            pos[ms] = 0x7FFFFFFF;
        }
        else
        {
            pos[ms] = o->motor[m].pos_fbk - o->tripod_calib.start_pos[ms];
            
            //Motor_set_pwm_ref(o->motor+m, o->tripod_calib.pwm);
            
            calibrated = FALSE;
        }
    }
    
    if (calibrated)
    {        
        o->motor[o->motors_of_set[0]].pos_calib_offset += o->motor[o->motors_of_set[0]].pos_fbk - o->tripod_calib.zero;
        o->motor[o->motors_of_set[1]].pos_calib_offset += o->motor[o->motors_of_set[1]].pos_fbk - o->tripod_calib.zero;
        o->motor[o->motors_of_set[2]].pos_calib_offset += o->motor[o->motors_of_set[2]].pos_fbk - o->tripod_calib.zero;
        
        o->motor[o->motors_of_set[0]].pos_fbk = o->tripod_calib.zero;
        o->motor[o->motors_of_set[1]].pos_fbk = o->tripod_calib.zero;
        o->motor[o->motors_of_set[2]].pos_fbk = o->tripod_calib.zero;

        o->motor[o->motors_of_set[0]].pos_fbk_old = o->motor[o->motors_of_set[0]].pos_fbk;
        o->motor[o->motors_of_set[1]].pos_fbk_old = o->motor[o->motors_of_set[1]].pos_fbk;
        o->motor[o->motors_of_set[2]].pos_fbk_old = o->motor[o->motors_of_set[2]].pos_fbk;
        
        return TRUE;
    }
    
    float pwm[3];
    
    if (pos[0]<0) pos[0] = -pos[0];
    if (pos[1]<0) pos[1] = -pos[1];
    if (pos[2]<0) pos[2] = -pos[2];
    
    int32_t posL = pos[0];
    if (pos[1] < posL) posL = pos[1];
    if (pos[2] < posL) posL = pos[2];
    
    float pwm_calib = (float)o->tripod_calib.pwm;
    
    if (pwm_calib < 0.0f) pwm_calib = -pwm_calib;
    
    pwm[0] = pwm_calib * (1.0f - (float)(pos[0] - posL)/(float)o->tripod_calib.max_delta);
    pwm[1] = pwm_calib * (1.0f - (float)(pos[1] - posL)/(float)o->tripod_calib.max_delta);
    pwm[2] = pwm_calib * (1.0f - (float)(pos[2] - posL)/(float)o->tripod_calib.max_delta);
    
    if (pwm[0] < 0.0f) pwm[0] = 0.0f;
    if (pwm[1] < 0.0f) pwm[1] = 0.0f;
    if (pwm[2] < 0.0f) pwm[2] = 0.0f;
    
    float pwm_min = pwm[0];
    if (pwm[1] < pwm_min) pwm_min = pwm[1];
    if (pwm[2] < pwm_min) pwm_min = pwm[2];

    if (pwm_min < 3500.0f)
    {
        pwm[0] += 3500.0f - pwm_min;
        pwm[1] += 3500.0f - pwm_min;
        pwm[2] += 3500.0f - pwm_min;
    }

    if (o->tripod_calib.pwm < 0)
    {
        pwm[0] = -pwm[0];
        pwm[1] = -pwm[1];
        pwm[2] = -pwm[2];
    }
    
    Motor_set_pwm_ref(o->motor+o->motors_of_set[0], (int32_t)pwm[0]);
    Motor_set_pwm_ref(o->motor+o->motors_of_set[1], (int32_t)pwm[1]);
    Motor_set_pwm_ref(o->motor+o->motors_of_set[2], (int32_t)pwm[2]);

    return FALSE;
}

BOOL JointSet_do_wait_calibration_10(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int k=0; k<*(o->pN); ++k)
    {
        int m = o->motors_of_set[k];
        
        int e = o->encoders_of_set[k];
        
        if (AbsEncoder_is_hard_stop_calibrating(o->absEncoder+e))
        {
            Motor_set_pwm_ref(o->motor+m, o->motor[m].calib_pwm);
        
            if (AbsEncoder_is_still(o->absEncoder+e, 12000, 500))
            {
                AbsEncoder_calibrate_in_hard_stop(o->absEncoder+e);
            }
            else
            {
                calibrated = FALSE;
            }
        }
        else
        {
            Motor_set_pwm_ref(o->motor+m, 0);
        }
    }
    
    return calibrated;
}

BOOL JointSet_do_wait_calibration_11(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int k=0; k<*(o->pN); ++k)
    {
        Joint* pj = o->joint+o->joints_of_set[k];
        Motor* pm = o->motor+o->motors_of_set[k];
        
        if (pm->not_calibrated)
        {
            if (o->calibration_wait)
            {
                if (pj->pos_fbk_from_motors > -2*pj->cable_calib.delta)
                {
                    Motor_set_pwm_ref(pm, -pj->cable_calib.pwm);
                }
                else
                {
                    Motor_set_pwm_ref(pm, 0);
                    pj->cable_calib.target = pj->pos_fbk + pj->cable_calib.delta;
                    o->calibration_wait = FALSE;
                }
                
                calibrated = FALSE;
            }
            else if (pj->pos_fbk < pj->cable_calib.target)
            {
                Motor_set_pwm_ref(pm, pj->cable_calib.pwm);
            
                calibrated = FALSE;
            }
            else
            {
                Motor_set_pwm_ref(pm, 0);
            
                pm->not_calibrated = FALSE;
                
                float L =  (float)(pj->pos_fbk - pj->pos_min) / (float)(pj->pos_max - pj->pos_min);
                
                pj->cable_constr.motor_pos_min = pj->pos_fbk_from_motors -       L *pj->cable_calib.cable_range;
                pj->cable_constr.motor_pos_max = pj->pos_fbk_from_motors + (1.0f-L)*pj->cable_calib.cable_range;
                
                pj->cable_constr.last_motor_closing_pos = pj->pos_fbk_from_motors;
                pj->cable_constr.last_joint_closing_pos = pj->pos_fbk;
            }
        }
        else
        {
            Motor_set_pwm_ref(pm, 0);
        }
    }
    
    return calibrated;
}
