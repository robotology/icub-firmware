#include "Calibrators.h"
#include "JointSet.h"
#include "CalibrationHelperData.h"
#include "hal_adc.h"

#define CALIB_TYPE_6_POS_TRHESHOLD 546 //546=3 degree //91.02f // = 0.5 degree

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

static BOOL JointSet_calibType6_check_reached_pos(Joint *j)
{
//    char info[70];

    CTRL_UNITS delta = j->running_calibration.data.type6.targetpos - j->pos_fbk;
    
//    ///// debug code
//    int32_t t_ref_pos = Trajectory_get_pos_ref(&j->trajectory);
//    int32_t t_ref_vel = Trajectory_get_vel_ref(&j->trajectory);
//    snprintf(info, 70, "e=%.1f rp=%d rv=%d d=%.1f", j->pos_err, t_ref_pos, t_ref_vel, delta);
//    send_debug_message(info, j->ID);
//    ///// debug code end
    
    if( (delta>CALIB_TYPE_6_POS_TRHESHOLD ) || (delta<-CALIB_TYPE_6_POS_TRHESHOLD ) )
        return FALSE;
    
    return TRUE;
}

static eOresult_t JointSet_do_wait_calibration_6_singleJoint(JointSet *o, int indexSet, BOOL* calibrationCompleted)
{
    
    /* When i'm here i sure that:
       - state = calibtype6_st_jntEncResComputed
     */
    
    *calibrationCompleted = FALSE;   
    //get poiter to the joint to calibrate
    Joint* j_ptr = o->joint + o->joints_of_set[indexSet];
    
    //get the encoder of joint to calibrate
    AbsEncoder* e_ptr = o->absEncoder+ o->encoders_of_set[indexSet];
    
    jointCalibType6Data *jCalib6Data_ptr = &(j_ptr->running_calibration.data.type6);
    
    switch(jCalib6Data_ptr->state)
    {
        case calibtype6_st_jntEncResComputed:
        {
            AbsEncoder_calibrate_absolute(e_ptr, 0, jCalib6Data_ptr->computedZero);
            jCalib6Data_ptr->state = calibtype6_st_absEncoderCalibrated;
        }
        break;
        
        case calibtype6_st_absEncoderCalibrated:
        {
            //if the current position (computed with calib param of abs encoder) is out of limits range, I'll put joint in fault
            int32_t curr_pos = AbsEncoder_position(e_ptr);
            if((curr_pos > j_ptr->pos_max+CALIB_TYPE_6_POS_TRHESHOLD) || (curr_pos < j_ptr->pos_min-CALIB_TYPE_6_POS_TRHESHOLD))
            {
                
                jCalib6Data_ptr->is_active = FALSE;
                char info[80];
                sprintf(info,"calib 6: outLim: cp%d mx%.1f mn%.1f",curr_pos, j_ptr->pos_max, j_ptr->pos_min);
                JointSet_send_debug_message(info, j_ptr->ID);

                return(eores_NOK_generic);
            }
            
            for (int k=0; k<*(o->pN); ++k)
            {
                int m = o->motors_of_set[k];
                int j = o->joints_of_set[k];
                Motor_motion_reset(o->motor+ m);
                Joint_motion_reset(o->joint+ j);
                Motor_set_run(o->motor+ m); //clear ext fault bit if it is not pressed
            }
        
            if(j_ptr->control_mode == eomc_controlmode_hwFault)
            {
                return(eores_NOK_generic);
            }
            
            BOOL ret = Joint_set_pos_ref_in_calibType6(j_ptr, jCalib6Data_ptr->targetpos, jCalib6Data_ptr->velocity);
            if(!ret)
            {
                char info[50];
                snprintf(info, 50,"error in Joint_set_pos_ref_in_calibType6");
                JointSet_send_debug_message(info, j_ptr->ID);

                return(eores_NOK_generic);
            }
            
//            ///// debug code
//            char info[70];
//            snprintf(info, 70, "init traj: cpos=%.2f target=%.2f lim%.2f %.2f", j_ptr->pos_fbk, j_ptr->calib_type6_data.targetpos, j_ptr->pos_max, j_ptr->pos_min);
//            send_debug_message(info, j_ptr->ID);
//            //// debug code ended
            
            jCalib6Data_ptr->state = calibtype6_st_trajectoryStarted;
        }    
        break;
        
        case calibtype6_st_trajectoryStarted:
        {
            JointSet_do_pwm_control(o);
            
            
            if(JointSet_calibType6_check_reached_pos(j_ptr))
            {
                jCalib6Data_ptr->state = calibtype6_st_finished;
                jCalib6Data_ptr->is_active = FALSE;
                *calibrationCompleted = TRUE;
                
                Motor* m_ptr = o->motor + o->motors_of_set[indexSet];
                Motor_calibrate_withOffset(m_ptr, m_ptr->pos_fbk);//the offset is the current position
                
                for (int k=0; k<*(o->pN); ++k)
                {
                    int m = o->motors_of_set[k];
                    int j = o->joints_of_set[k];
                    Motor_motion_reset(o->motor+ m);
                    Joint_motion_reset(o->joint+ j);
                    Motor_set_idle(o->motor+ m);
                }
            }
        }
        break;
        
        case calibtype6_st_finished:
        {
            *calibrationCompleted = TRUE;
        }break;
        
        default:
            return(eores_NOK_generic);

    }

    return(eores_OK);

}


static eOresult_t JointSet_do_wait_calibration_7_singleJoint(Joint *j, Motor* m, AbsEncoder* e, BOOL* calibrationCompleted)
{
    
    /* When i'm here i sure that:
       - state = calibtype7_st_jntEncResComputed
    */
    
    *calibrationCompleted = FALSE;
    
    jointCalibType7Data* jCalib7data_ptr = &(j->running_calibration.data.type7);

    switch(jCalib7data_ptr->state)
    {
        case calibtype7_st_jntEncResComputed:
        {
            AbsEncoder_calibrate_absolute(e, 0, j->running_calibration.data.type7.computedZero);
            jCalib7data_ptr->state = calibtype7_st_jntCheckLimits;
        }    
        break;
        
        case calibtype7_st_jntCheckLimits:
        {
            int32_t curr_pos = AbsEncoder_position(e);
            if((curr_pos > j->pos_max+CALIB_TYPE_6_POS_TRHESHOLD) || (curr_pos < j->pos_min-CALIB_TYPE_6_POS_TRHESHOLD))
            {
                //// debug code
                char info[80];
                sprintf(info,"calib7:outLim: cp%d mx%.1f mn%.1f",curr_pos, j->pos_max, j->pos_min);
                JointSet_send_debug_message(info, j->ID);
                ////debug code ended

                return(eores_NOK_generic);
            
            }
            else
            {
                *calibrationCompleted = TRUE;
                jCalib7data_ptr->state = calibtype7_st_finished;
            }
         }    
        break;
        case calibtype7_st_finished:
        {
            *calibrationCompleted = TRUE;
        }break;
    }
    return(eores_OK);
}

BOOL JointSet_do_wait_calibration_mixed(JointSet* o)
{
    JointSet_set_interaction_mode(o, eOmc_interactionmode_stiff);
    
    BOOL calibrationCompleted = TRUE;
    eOresult_t res = eores_OK;
    
    for (int k=0; ( (k<*(o->pN)) && (eores_OK == res) ); ++k)
    {
        int m = o->motors_of_set[k];
        int e = o->encoders_of_set[k];
        int j = o->joints_of_set[k];
        Joint* j_ptr = o->joint+j;
        BOOL calibrated_single_joint = FALSE;
        
       switch (j_ptr->running_calibration.type)
       {
        case eomc_calibration_type6_mais:
            res = JointSet_do_wait_calibration_6_singleJoint(o, k, &calibrated_single_joint);
            break;
        
        case eomc_calibration_type7_hall_sensor:
            res = JointSet_do_wait_calibration_7_singleJoint(j_ptr, o->motor+m, o->absEncoder+e, &calibrated_single_joint);
            break;

        case eomc_calibration_typeUndefined:
            res = eores_OK; //if I'm here means joint j didn't receive calib command or is already calibrated. Both cases are ok! 
            
            break;
        default://If i'm here means joint has a calib type that not support calibration for single joint.
            res = eores_NOK_generic;
            break;
        }
       
       calibrationCompleted = calibrationCompleted & calibrated_single_joint;

    }//end for
    
    
    if(res != eores_OK)
    {
        o->calibration_in_progress = eomc_calibration_typeUndefined;
        o->control_mode = eomc_controlmode_hwFault;
        
        for (int k=0; k<*(o->pN); ++k)
        {
            Joint* j_ptr = o->joint + o->joints_of_set[k];
            j_ptr->control_mode = eomc_controlmode_hwFault;
            //j_ptr->calibration_in_progress = eomc_calibration_typeUndefined;
            Joint_reset_calibration_data(j_ptr);
            Motor_set_idle(o->motor+o->motors_of_set[k]);
        }
        
    }
    if(calibrationCompleted)
    {
        for (int k=0; k<*(o->pN); ++k)
        {
            Joint* j_ptr = o->joint + o->joints_of_set[k];
            Joint_reset_calibration_data(j_ptr);
        }
    
    }
    return(calibrationCompleted);

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
                
                pj->cable_constr.last_joint_pos = pj->pos_fbk;
            }
        }
        else
        {
            Motor_set_pwm_ref(pm, 0);
        }
    }
    
    return calibrated;
}
