/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or  FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License foFITNESSr more details
*/

#include "EoCommon.h"

#include "EOtheMemoryPool.h"

#include "EOemsControllerCfg.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOappEncodersReader.h"

#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"
#include "Pid.h"

#include "JointSet.h"

#include "Calibrators.h"

static void JointSet_set_inner_control_flags(JointSet* o);

JointSet* JointSet_new(uint8_t n) //
{
    JointSet* o = NEW(JointSet, n);
    
    for (int i=0; i<n; ++i)
    {   
        JointSet_init(o+i);
    }
    
    return o;
}

void JointSet_init(JointSet* o) //
{   
    o->pN = NULL;
    o->pE = NULL;

    o->joints_of_set = NULL;
    o->motors_of_set = NULL;
    
    o->joint = NULL;
    o->motor = NULL;
       
    o->Jjm = NULL;
    o->Jmj = NULL;
    
    o->Sje = NULL;
    o->Sjm = NULL;
    
    o->absEncoder = NULL;
    
    o->control_mode     = eomc_controlmode_notConfigured;
    o->interaction_mode = eOmc_interactionmode_stiff;
    
    o->MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR; 
    
    o->pos_control_active = TRUE;
    o->trq_control_active = FALSE; 
    o->CAN_DO_TRQ_CTRL = TRUE;
    o->USE_SPEED_FBK_FROM_MOTORS = TRUE;
    
    o->is_calibrated = FALSE;
    
    o->special_constraint = NO_CONSTRAINT;
    
    o->calibration_in_progress = eomc_calibration_typeUndefined;
}

void JointSet_config //
(
    JointSet* o,
    uint8_t* pN,
    uint8_t* pE,
    uint8_t* joints_of_set,
    uint8_t* motors_of_set,
    uint8_t* encoders_of_set,
    Joint* joint, 
    Motor* motor,
    AbsEncoder *absEncoder,
    float** Jjm,
    float** Jmj,
    float** Sje,
    float** Sjm
)
{
    o->pN = pN;
    o->pE = pE;
    o->joints_of_set = joints_of_set;
    o->motors_of_set = motors_of_set;
    o->encoders_of_set = encoders_of_set;
    o->joint = joint;
    o->motor = motor;
    o->absEncoder = absEncoder;
    o->Jjm = Jjm;
    o->Jmj = Jmj;
    o->Sje = Sje;
    o->Sjm = Sjm;
}

void JointSet_do_odometry(JointSet* o) //
{
    int js, j;
    int ms, m;

    int N = *(o->pN);
            
    float **Sjm = o->Sjm;
    
    if (Sjm)
    {
        for (js=0; js<N; ++js)
        {
            j = o->joints_of_set[js];

            o->joint[j].pos_fbk_from_motors = ZERO;
            o->joint[j].vel_fbk_from_motors = ZERO;
            
            for (ms=0; ms<N; ++ms)
            {
                m = o->motors_of_set[ms];
        
                o->joint[j].pos_fbk_from_motors += Sjm[j][m] * o->motor[m].pos_fbk;
                o->joint[j].vel_fbk_from_motors += Sjm[j][m] * o->motor[m].vel_fbk;
            }
        }
    }
    else
    {
        for (js=0; js<N; ++js)
        {
            j = o->joints_of_set[js];
            
            o->joint[j].pos_fbk_from_motors = o->motor[j].pos_fbk;
            o->joint[j].vel_fbk_from_motors = o->motor[j].vel_fbk;
        }
    }
    
    if (!o->Sje) // no encoder coupling
    {
        for (js=0; js<N; ++js)
        {    
            j = o->encoders_of_set[js];
        
            if (AbsEncoder_is_fake(o->absEncoder+j))
            {
                o->joint[j].pos_fbk = o->joint[j].pos_fbk_from_motors;
                o->joint[j].vel_fbk = o->joint[j].vel_fbk_from_motors;
            }
            else
            {
                o->joint[j].pos_fbk = AbsEncoder_position(o->absEncoder+j);
                
                if (o->USE_SPEED_FBK_FROM_MOTORS)
                {
                    o->joint[j].vel_fbk = o->joint[j].vel_fbk_from_motors;
                }
                else
                {
                    o->joint[j].vel_fbk = AbsEncoder_velocity(o->absEncoder+j);
                }
            }
        }
        
        return;
    }
    
    // encoder coupling
    {
        int E = *(o->pE);
        
        float pos[MAX_ENCODS_PER_BOARD];
        float vel[MAX_ENCODS_PER_BOARD];
        
        int es, e;
        
        for (es=0; es<E; ++es)
        {    
            e = o->encoders_of_set[es];
            
            if (AbsEncoder_is_fake(o->absEncoder+e))
            {
                pos[e] = o->joint[e].pos_fbk_from_motors;
                vel[e] = o->joint[e].vel_fbk_from_motors;
            }
            else
            {
                pos[e] = AbsEncoder_position(o->absEncoder+e);
                
                if (o->USE_SPEED_FBK_FROM_MOTORS)
                {
                    vel[e] = o->joint[e].vel_fbk_from_motors;
                }
                else
                {
                    vel[e] = AbsEncoder_velocity(o->absEncoder+e);
                }
            }
        }
    
        float **Sje = o->Sje;
        
        for (js=0; js<N; ++js)
        {
            j = o->joints_of_set[js];
        
            o->joint[j].pos_fbk = ZERO;
            o->joint[j].vel_fbk = ZERO;
            
            for (es=0; es<E; ++es)
            {
                e = o->encoders_of_set[es];
                
                o->joint[j].pos_fbk += Sje[j][e] * pos[e];
                o->joint[j].vel_fbk += Sje[j][e] * vel[e];
            }
        }
    }
}

BOOL JointSet_do_check_faults(JointSet* o)
{
    int N = *(o->pN);
    int E = *(o->pE);
    
    BOOL fault = FALSE;
    o->external_fault = FALSE;
    
    for (int k=0; k<N; ++k)
    {
        if (Joint_check_faults(o->joint+o->joints_of_set[k]))
        {
            fault = TRUE;
        }
        
        if (Motor_check_faults(o->motor+o->motors_of_set[k]))
        {
            fault = TRUE;
            
            // traditional confusion among joints, motors and encoders: I don't like it
            o->joint[o->joints_of_set[k]].control_mode = eomc_controlmode_hwFault;
        }
        
        if (Motor_is_external_fault(o->motor+o->motors_of_set[k]))
        {
            o->external_fault = TRUE;
        }
    }
    
    for (int k=0; k<E; ++k)
    {
        if (AbsEncoder_is_in_fault(o->absEncoder+o->encoders_of_set[k]))
        {
            fault = TRUE;
            
            // traditional confusion among joints, motors and encoders: I don't like it
            o->joint[o->joints_of_set[k]].control_mode = eomc_controlmode_hwFault;
        }
    }
    
    if (fault)
    {
        for (int k=0; k<N; ++k)
        {
            Joint_set_control_mode(o->joint+o->joints_of_set[k], eomc_controlmode_cmd_idle);
            
            if (Motor_is_running(o->motor+o->motors_of_set[k]))
            {
                Motor_set_idle(o->motor+o->motors_of_set[k]);
            }
        }
        
        o->control_mode = eomc_controlmode_hwFault;
        
        JointSet_set_inner_control_flags(o);
    }
    else if (o->external_fault)
    {
        for (int k=0; k<N; ++k)
        {
            Joint_set_control_mode(o->joint+o->joints_of_set[k], eomc_controlmode_cmd_idle);
            
            if (Motor_is_running(o->motor+o->motors_of_set[k]))
            {
                Motor_set_idle(o->motor+o->motors_of_set[k]);
            }
        }
        
        o->control_mode = eomc_controlmode_idle;
        
        JointSet_set_inner_control_flags(o);
    }
    
    return fault;
}

static void JointSet_do_vel_control(JointSet* o);

void JointSet_do_control(JointSet* o)
{
    switch (o->MOTOR_CONTROL_TYPE)
    {
    case PWM_CONTROLLED_MOTOR:
        JointSet_do_pwm_control(o);
        return;
    
    case VEL_CONTROLLED_MOTOR:
        JointSet_do_vel_control(o);
        return;
    
    case IQQ_CONTROLLED_MOTOR:
        //JointSet_do_trq_control(o);
        return;
    }
}

static void JointSet_do_wait_calibration(JointSet* o);

void JointSet_do(JointSet* o)
{
    JointSet_do_odometry(o);
    JointSet_do_check_faults(o);
    
    if (o->is_calibrated)
    {                
        JointSet_do_control(o);
    }
    else
    {
        JointSet_do_wait_calibration(o);
    }
}

BOOL JointSet_set_control_mode(JointSet* o, eOmc_controlmode_command_t control_mode)
{
    if (control_mode == o->control_mode) return TRUE;
    
    if (o->control_mode == eomc_controlmode_calib) return FALSE;
    
    if (o->control_mode == eomc_controlmode_notConfigured) return FALSE;
    
    if (control_mode == eomc_ctrlmval_openloop && o->MOTOR_CONTROL_TYPE != PWM_CONTROLLED_MOTOR) return FALSE;
    
    if (control_mode == eomc_controlmode_torque && !o->CAN_DO_TRQ_CTRL) return FALSE;
    
    int N = *(o->pN);
    int E = *(o->pE);
    
    if (o->control_mode == eomc_controlmode_hwFault)
    {
        if (control_mode == eomc_controlmode_cmd_force_idle)
        {
            for (int k=0; k<E; ++k)
            {   
                AbsEncoder_clear_faults(o->absEncoder+o->encoders_of_set[k]);
            }
            
            for (int k=0; k<N; ++k)
            {   
                Motor_force_idle(o->motor+o->motors_of_set[k]);
                
                Joint_set_control_mode(o->joint+o->joints_of_set[k], eomc_controlmode_cmd_force_idle);
            }
        }
        else
        {
            return FALSE;
        }
    }
    
    switch (control_mode)
    {
    case eomc_controlmode_cmd_force_idle:
    case eomc_controlmode_cmd_idle:
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
            
            Motor_set_idle(o->motor+o->motors_of_set[k]);
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode);
        }
        break;
    
    case eomc_controlmode_cmd_openloop:
        //if (o->external_fault) return FALSE;
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
            
            Motor_set_run(o->motor+o->motors_of_set[k]);
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode);
        }
        break;
        
    case eomc_controlmode_cmd_torque:
        //if (o->external_fault) return FALSE;
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
            
            Motor_set_run(o->motor+o->motors_of_set[k]);
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode);
        }    
        break;
    
    case eomc_controlmode_direct:
    case eomc_controlmode_cmd_mixed:
    case eomc_controlmode_cmd_position:
    case eomc_controlmode_cmd_velocity:
        //if (o->external_fault) return FALSE;
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
            
            Motor_set_run(o->motor+o->motors_of_set[k]);
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode);
        }
        break;
        
    default:
        return FALSE;
    }
    
    o->control_mode = (eOmc_controlmode_t)control_mode;
    
    JointSet_set_inner_control_flags(o);
    
    return TRUE;
}

void JointSet_set_interaction_mode(JointSet* o, eOmc_interactionmode_t interaction_mode)
{
    if (interaction_mode == o->interaction_mode) return;
    
    o->interaction_mode = interaction_mode;
    
    JointSet_set_inner_control_flags(o);
    
    int N = *(o->pN);
    
    if (interaction_mode == eOmc_interactionmode_stiff)
    {        
        for (int k=0; k<N; ++k)
        {
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
        }
    }
    
    for (int k=0; k<N; ++k)
    {
        Joint_set_interaction_mode(o->joint+o->joints_of_set[k], interaction_mode);
    }
}

//////////////////////////////////////////////////////////////////////////
// statics

static void JointSet_manage_cable_constraint(JointSet* o)
{
    for (int k=0; k<*(o->pN); ++k)
    {
        if (Joint_manage_cable_constraint(o->joint+o->joints_of_set[k]))
        {
            o->motor[o->motors_of_set[k]].output = 0; 
        }
    }
}

static void JointSet_manage_trifid_constraint(JointSet* o)
{
    int j0 = o->joints_of_set[0];
    int j1 = o->joints_of_set[1];
    int j2 = o->joints_of_set[2];

    int m0 = o->motors_of_set[0];
    int m1 = o->motors_of_set[1];
    int m2 = o->motors_of_set[2];
    
    int32_t rho0 = o->joint[j0].pos_fbk;
    int32_t rho1 = o->joint[j1].pos_fbk;
    int32_t rho2 = o->joint[j2].pos_fbk;

    if (rho0*rho0+rho1*rho1+rho2*rho2-rho0*rho1-rho1*rho2-rho2*rho0 < o->special_limit)
    {
        return;
    }
            
    if ( !(rho0<=rho1 && rho0<=rho2 && o->motor[m0].output>=0) 
      && !(rho0>=rho1 && rho0>=rho2 && o->motor[m0].output<=0))
    {
        o->motor[j0].output = 0;
        Joint_stop(o->joint+j0);
    }
                
    if ( !(rho1<=rho2 && rho1<=rho0 && o->motor[m1].output>=0) 
      && !(rho1>=rho2 && rho1>=rho0 && o->motor[m1].output<=0))
    {
        o->motor[j1].output = 0;
        Joint_stop(o->joint+j1);
    }
                
    if ( !(rho2<=rho0 && rho2<=rho1 && o->motor[m2].output>=0) 
      && !(rho2>=rho0 && rho2>=rho1 && o->motor[m2].output<=0))
    {
        o->motor[j2].output = 0;
        Joint_stop(o->joint+j2);
    }
}

void JointSet_do_pwm_control(JointSet* o)
{
    int N = *(o->pN);
        
    BOOL limits_torque_protection = FALSE;
        
    for (int js=0; js<N; ++js)
    {
        Joint *pJoint = o->joint+o->joints_of_set[js];
        
        Joint_do_pwm_control(pJoint);
       
        if (o->trq_control_active && Joint_pushing_limit(pJoint))
        {
            limits_torque_protection = TRUE;
        }
    }
    
    CTRL_UNITS motor_pwm_ref = ZERO;
    
    for (int ms=0; ms<N; ++ms)
    {
        int m = o->motors_of_set[ms];
    
        if (o->trq_control_active)
        {   
            if (o->Jjm)
            {
                CTRL_UNITS motor_trq_ref = ZERO;
                CTRL_UNITS motor_trq_fbk = ZERO;
            
                for (int js=0; js<N; ++js)
                {
                    int j = o->joints_of_set[js];
                
                    // mu = Jt Tau 
                    // transposed direct Jacobian
                    motor_trq_ref += o->Jjm[j][m]*o->joint[j].trq_ref;
                    motor_trq_fbk += o->Jjm[j][m]*o->joint[j].trq_fbk;
                }
                
                motor_pwm_ref = Motor_do_trq_control(o->motor+m, motor_trq_ref, motor_trq_fbk);
            }
            else
            {
                motor_pwm_ref = Motor_do_trq_control(o->motor+m, o->joint[m].trq_ref, o->joint[m].trq_fbk);
            }
        }
        else
        {
            if (o->Jmj)
            {
                motor_pwm_ref = ZERO;
            
                for (int js=0; js<N; ++js)
                {
                    int j = o->joints_of_set[js];
                
                    // inverse Jacobian
                    motor_pwm_ref += o->Jmj[m][j]*o->joint[j].output;
                }
            }
            else
            {
                motor_pwm_ref = o->joint[m].output;
            }
        }

        Motor_set_pwm_ref(o->motor+m, motor_pwm_ref);
    }
    
    if (limits_torque_protection)
    {
        CTRL_UNITS joint_pwm_ref[MAX_JOINTS_PER_BOARD];
        
        for (int js=0; js<N; ++js)
        {
            int j = o->joints_of_set[js];
            
            if (o->Jmj)
            {
                joint_pwm_ref[j] = ZERO;
            
                for (int ms=0; ms<N; ++ms)
                {
                    int m = o->motors_of_set[ms];
                
                    // transposed inverse Jacobian
                    joint_pwm_ref[j] += o->Jmj[m][j]*o->motor[m].pwm_ref;
                }
            }
            else
            {
                joint_pwm_ref[j] = o->motor[j].pwm_ref;
            }
            
            if (Joint_pushing_limit(o->joint+j))
            {
                if ((o->joint[j].output_lim > ZERO) ^ (o->joint[j].output_lim < joint_pwm_ref[j]))
                {
                    joint_pwm_ref[j] = o->joint[j].output_lim; 
                }
            }
        }
        
        for (int ms=0; ms<N; ++ms)
        {
            int m = o->motors_of_set[ms];
        
            if (o->Jjm)
            {
                motor_pwm_ref = ZERO;
            
                for (int js=0; js<N; ++js)
                {
                    int j = o->joints_of_set[js];
                
                    // transposed jacobian
                    motor_pwm_ref += o->Jjm[j][m]*joint_pwm_ref[j];
                }
            
                Motor_set_pwm_ref(o->motor+m, motor_pwm_ref);
            }
            else
            {
                Motor_set_pwm_ref(o->motor+m, joint_pwm_ref[m]);
            }
        }
    }
    
    switch (o->special_constraint)
    {
        case TRIFID_CONSTRAINT:
            JointSet_manage_trifid_constraint(o);
            break;
        
        case CER_HAND_CONSTRAINT:
            JointSet_manage_cable_constraint(o);
            break;

        default:
            break;
    }
}

static void JointSet_do_vel_control(JointSet* o)
{
    int N = *(o->pN);

    for (int js=0; js<N; ++js)
    {
        Joint_do_vel_control(o->joint+o->joints_of_set[js]);
    }
    
    for (int ms=0; ms<N; ++ms)
    {
        int m = o->motors_of_set[ms];
    
        if (o->Jmj)
        {
            float **Ji = o->Jmj; // inverse Jacobian
            
            CTRL_UNITS motor_vel_ref = ZERO;
        
            for (int js=0; js<N; ++js)
            {
                int j = o->joints_of_set[js];
                
                motor_vel_ref += Ji[m][j]*o->joint[j].output;
            }
        
            Motor_set_vel_ref(o->motor+m, motor_vel_ref);
        }
        else
        {
            Motor_set_vel_ref(o->motor+m, o->joint[m].output);
        }
    }
    
    switch (o->special_constraint)
    {
        case TRIFID_CONSTRAINT:
            JointSet_manage_trifid_constraint(o);
            break;
        
        default:
            break;
    }
}

static void JointSet_set_inner_control_flags(JointSet* o)
{
    switch (o->control_mode)
    {
        case eomc_controlmode_position:
        case eomc_controlmode_velocity:
        case eomc_controlmode_mixed:
        case eomc_controlmode_direct:
            o->pos_control_active = TRUE;
        break;
        
        default:
            o->pos_control_active = FALSE;
    }
    
    if (o->control_mode==eomc_controlmode_torque)
    {
        o->trq_control_active = TRUE;
    }
    else if (o->pos_control_active && (o->interaction_mode==eOmc_interactionmode_compliant))
    {
        o->trq_control_active = TRUE;
    }
    else
    {
        o->trq_control_active = FALSE;
    }
}

static void JointSet_do_wait_calibration(JointSet* o)
{
    int N = *(o->pN);
    int E = *(o->pE);
    
    if (o->calibration_in_progress != eomc_calibration_typeUndefined)
    {
        if (o->calibration_timeout < CALIBRATION_TIMEOUT)
        {
            ++o->calibration_timeout;
        }
        else
        {
            o->calibration_in_progress = eomc_calibration_typeUndefined;
        
            o->control_mode = eomc_controlmode_notConfigured;
            
            o->is_calibrated = FALSE;
            
            for (int k=0; k<N; ++k)
            {
                o->joint[o->joints_of_set[k]].control_mode = eomc_controlmode_notConfigured;
            
                Motor_set_pwm_ref(o->motor+o->motors_of_set[k], 0);
                Motor_set_idle(o->motor+o->motors_of_set[k]);
                Motor_uncalibrate(o->motor+o->motors_of_set[k]);
            }    
            
            return;
        }
    }

    o->is_calibrated = TRUE;
    
    switch (o->calibration_in_progress)
    {
        case eomc_calibration_type3_abs_sens_digital:
            o->is_calibrated = JointSet_do_wait_calibration_3(o);
            break;
        
        case eomc_calibration_type5_hard_stops:
            o->is_calibrated = JointSet_do_wait_calibration_5(o);
            break;
            
//        case eomc_calibration_type6_mais: ==> managed in mixed
//            o->is_calibrated = JointSet_do_wait_calibration_6(o);
//            break;
//        
//        case eomc_calibration_type7_hall_sensor: ==> managed in mixed
//            o->is_calibrated = JointSet_do_wait_calibration_7(o);
//            break;

        case eomc_calibration_type8_tripod_internal_hard_stop:
            o->is_calibrated = JointSet_do_wait_calibration_8(o);
            break;
        
        case eomc_calibration_type9_tripod_external_hard_stop:
            o->is_calibrated = JointSet_do_wait_calibration_9(o);
            break;

        case eomc_calibration_type10_abs_hard_stop:
            o->is_calibrated = JointSet_do_wait_calibration_10(o);
            break;
            
        case eomc_calibration_type11_cer_hands:
            o->is_calibrated = JointSet_do_wait_calibration_11(o);
            break;
            
        case eomc_calibration_typeMixed:
            o->is_calibrated = JointSet_do_wait_calibration_mixed(o);
            break;
        
        default:
            o->is_calibrated = FALSE;
            break;
    }
    
    if (!o->is_calibrated) return;
    
    for (int es=0; es<E; ++es)
    {
        if (!AbsEncoder_is_calibrated(o->absEncoder+o->encoders_of_set[es])) 
        {
            o->is_calibrated = FALSE;
            return;
        }
    }
    
    o->calibration_in_progress = eomc_calibration_typeUndefined;
    
    JointSet_do_odometry(o);
    
    o->control_mode = eomc_controlmode_idle;

    for (int js=0; js<N; ++js)
    {
        o->joint[o->joints_of_set[js]].control_mode = eomc_controlmode_idle;
    }
    
    JointSet_set_control_mode(o, eomc_controlmode_cmd_position);
}

void JointSet_calibrate(JointSet* o, uint8_t e, eOmc_calibrator_t *calibrator)
{
//    for (int js=0; js<*(o->pN); ++js)
//    {
//        o->joint[o->joints_of_set[js]].control_mode = eomc_controlmode_calib;
//    }
    
    eOmc_controlmode_t joint_controlMode_old = o->joint[e].control_mode;
    eOmc_controlmode_t jointSet_controlMode_old = o->control_mode;
    
    o->joint[e].control_mode = eomc_controlmode_calib;
    
    o->control_mode = eomc_controlmode_calib;
    
    o->is_calibrated = FALSE;
    
    switch (calibrator->type)
    {
        case eomc_calibration_type3_abs_sens_digital:
        {
            AbsEncoder_calibrate_absolute(o->absEncoder+e, calibrator->params.type3.offset, calibrator->params.type3.calibrationZero);
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            break;
        }
        case eomc_calibration_type5_hard_stops:
        {
//            //Debug code
//            char message[150];
//            snprintf(message, sizeof(message), "calib cmd rec: pwm%d cz%d", calibrator->params.type5.pwmlimit, calibrator->params.type5.calibrationZero);
//            JointSet_send_debug_message(message, e);
            
            BOOL ret = Motor_calibrate_moving2Hardstop(o->motor+e, calibrator->params.type5.pwmlimit, calibrator->params.type5.calibrationZero);
            
            if(!ret)
            {
                o->joint[e].control_mode = joint_controlMode_old;
                o->control_mode = jointSet_controlMode_old;
                return;
            }
            AbsEncoder_calibrate_fake(o->absEncoder+e);
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            break;
        }
        
        case eomc_calibration_type6_mais:
        {
            // 1) check params are ok

            CTRL_UNITS target_pos;
            
            if (calibrator->params.type6.current == 1 )
            {
                target_pos = calibrator->params.type6.vmax;
            }
            else if (calibrator->params.type6.current == -1)
            {
                target_pos = calibrator->params.type6.vmin;
            }
            else
            {
                ////debug code
                char info[50];
                snprintf(info, 50, "error type6.current=%d",calibrator->params.type6.current);
                JointSet_send_debug_message(info, e);
                ////debug code ended
                return;
            }
            
                
            //if I'm here I can perform calib type 6.
            
            // 2) set state
            
            o->calibration_in_progress = eomc_calibration_typeMixed;
            o->joint[e].running_calibration.type = (eOmc_calibration_type_t)calibrator->type;
            o->joint[e].running_calibration.data.type6.is_active = TRUE;
            o->joint[e].running_calibration.data.type6.state = calibtype6_st_inited;
            
            
//            o->joint[e].calib_type6_data.is_active = TRUE;
//            o->joint[e].calib_type6_data.state = calibtype6_st_inited;
//            o->joint[e].calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            
            
            // 3) calculate new joint encoder factor and param_zero
            eOmc_joint_config_t *jconfig = &o->joint[e].eo_joint_ptr->config;
           
            float computedJntEncoderResolution = (float)(calibrator->params.type6.vmax - calibrator->params.type6.vmin) / (float) (jconfig->limitsofjoint.max  - jconfig->limitsofjoint.min);
            
            eOresult_t res = eo_appEncReader_UpdatedMaisConversionFactors(eo_appEncReader_GetHandle(), e, computedJntEncoderResolution);
            if(eores_OK != res)
            {    
                ////debug code
                char info[70];
                snprintf(info, 70, "calib6: error updating Mais conversion factor j%d", e);
                JointSet_send_debug_message(info, e);
                ////debug code ended
                return;
            }

            AbsEncoder_config_resolution(o->absEncoder+e, EOK_CLIP_INT32(computedJntEncoderResolution));
            
            //Now I need to re-init absEncoder because I chenged maisConversionFactor, therefore the values returned by EOappEncoreReder are changed.
            o->absEncoder[e].state.bits.not_initialized = TRUE;

            float computedJntEncoderZero =  - (float)(jconfig->limitsofjoint.min) + ((float)(calibrator->params.type6.vmin) / computedJntEncoderResolution);
            o->joint[e].running_calibration.data.type6.computedZero = computedJntEncoderZero;

            o->joint[e].running_calibration.data.type6.targetpos = target_pos / computedJntEncoderResolution - computedJntEncoderZero; //convert target pos from mais unit to icub deegre

            o->joint[e].running_calibration.data.type6.velocity = calibrator->params.type6.velocity;
            
            o->joint[e].running_calibration.data.type6.state = calibtype6_st_jntEncResComputed;
            
        }
        break;

        case eomc_calibration_type7_hall_sensor:
        {
            //1) check params: nothinh to do
            
            // 2) set state
            o->calibration_in_progress = eomc_calibration_typeMixed;
            o->joint[e].running_calibration.type = (eOmc_calibration_type_t)calibrator->type;
            o->joint[e].running_calibration.data.type7.state = calibtype7_st_inited;
            o->joint[e].running_calibration.data.type7.is_active = TRUE;
            
//            o->joint[e].calib_type7_data.is_active = TRUE;
//            o->joint[e].calib_type7_data.state = calibtype7_st_inited;
//            o->joint[e].calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            
            // 2) calculate new joint encoder factor and param_zero
            eOmc_joint_config_t *jconfig = &o->joint[e].eo_joint_ptr->config;
            float computedJntEncoderResolution = (float)(calibrator->params.type7.vmax - calibrator->params.type7.vmin) / (float) (jconfig->limitsofjoint.max  - jconfig->limitsofjoint.min);
            
            
            //In some cases, position returned by encoder reader is bigger than 65535, therefore I need to rescale this value into range [0, 65535] in order to work with AbsEncoder object.
            float jmin = ((float)calibrator->params.type7.vmin)/computedJntEncoderResolution;
            float jmax = ((float)calibrator->params.type7.vmax)/computedJntEncoderResolution;
            
            #define ROUND_ANGLE_IDEGREE 65535.0f
            int32_t offset;
            if((jmin > ROUND_ANGLE_IDEGREE) && (jmax > ROUND_ANGLE_IDEGREE))
            {
                offset = 65535;
                
            }
            else if((jmin < ROUND_ANGLE_IDEGREE) && (jmax < ROUND_ANGLE_IDEGREE))
            {
                offset = 0;
            }
            else
            {
                ////debug code 
                char info[50];
                snprintf(info, 50, "error calib 7 computing encoder factor");
                JointSet_send_debug_message(info, e);
                ////debug code ended
                return;
            }
            
            eOresult_t res = eo_appEncReader_UpdatedHallAdcOffset(eo_appEncReader_GetHandle(), e, offset);
            if(eores_OK != res)
            {    
                ////debug code 
                char info[70];
                snprintf(info, 70, "calib7: error updating HallADC offset j%d", e);
                JointSet_send_debug_message(info, e);
                ////debug code ended
                return;
            }
            
            
            res = eo_appEncReader_UpdatedHallAdcConversionFactors(eo_appEncReader_GetHandle(), e, computedJntEncoderResolution);
            if(eores_OK != res)
            {    
                ////debug code 
                char info[70];
                snprintf(info, 70, "calib7: error updating HallADC conversion factor j%d", e);
                JointSet_send_debug_message(info, e);
                ////debug code ended
                return;
            }

            AbsEncoder_config_resolution(o->absEncoder+e, EOK_CLIP_INT32(computedJntEncoderResolution));
            
            //Now I need to re init absEncoder because I chenged hallADCConversionFactor, therefore the values returned by EOappEncoreReder are changed.
            o->absEncoder[e].state.bits.not_initialized = TRUE;
            
            float computedJntEncoderZero =  (((float)calibrator->params.type7.vmin) / computedJntEncoderResolution) - ((float)(jconfig->limitsofjoint.min)) - offset;

            
            o->joint[e].running_calibration.data.type7.computedZero = computedJntEncoderZero;
            o->joint[e].running_calibration.data.type7.state = calibtype7_st_jntEncResComputed;
            
        }
        break;

        case eomc_calibration_type8_tripod_internal_hard_stop:
        case eomc_calibration_type9_tripod_external_hard_stop:
        {
            if (o->calibration_in_progress == calibrator->type) return;
            
            o->calibration_wait = 0;
            o->calibration_timeout = 0;
            
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            o->joint[o->joints_of_set[0]].control_mode = eomc_controlmode_calib;
            o->joint[o->joints_of_set[1]].control_mode = eomc_controlmode_calib;
            o->joint[o->joints_of_set[2]].control_mode = eomc_controlmode_calib;
            
            AbsEncoder_calibrate_fake(o->absEncoder+o->encoders_of_set[0]);
            AbsEncoder_calibrate_fake(o->absEncoder+o->encoders_of_set[1]);
            AbsEncoder_calibrate_fake(o->absEncoder+o->encoders_of_set[2]);
            
            o->tripod_calib.pwm       = calibrator->params.type9.pwmlimit;
            o->tripod_calib.max_delta = calibrator->params.type9.max_delta;
            o->tripod_calib.zero      = calibrator->params.type9.calibrationZero;
            
            Motor_uncalibrate(o->motor+o->motors_of_set[0]);
            Motor_uncalibrate(o->motor+o->motors_of_set[1]);
            Motor_uncalibrate(o->motor+o->motors_of_set[2]);
            
            Motor_set_run(o->motor+o->motors_of_set[0]);
            Motor_set_run(o->motor+o->motors_of_set[1]);
            Motor_set_run(o->motor+o->motors_of_set[2]);
            
            o->tripod_calib.start_pos[0] = o->motor[o->motors_of_set[0]].pos_fbk;
            o->tripod_calib.start_pos[1] = o->motor[o->motors_of_set[1]].pos_fbk;
            o->tripod_calib.start_pos[2] = o->motor[o->motors_of_set[2]].pos_fbk;
            
            break;
        }
        
        case eomc_calibration_type10_abs_hard_stop:
        {
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            o->calibration_timeout = 0;
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            Motor_set_run(o->motor+e);
            o->motor[e].calib_pwm = calibrator->params.type10.pwmlimit;
            
            AbsEncoder_still_check_reset(o->absEncoder+e);
            AbsEncoder_start_hard_stop_calibrate(o->absEncoder+e, calibrator->params.type10.calibrationZero);
            break;
        }
        
        case eomc_calibration_type11_cer_hands:
        {   
            AbsEncoder* enc = o->absEncoder + 3*e;
            AbsEncoder_calibrate_absolute(enc  , calibrator->params.type11.offset0, enc[0].mul*32767);
            AbsEncoder_calibrate_absolute(enc+1, calibrator->params.type11.offset1, enc[1].mul*32767);
            AbsEncoder_calibrate_absolute(enc+2, calibrator->params.type11.offset2, enc[2].mul*32767);
            
            JointSet_do_odometry(o);
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            Motor_set_run(o->motor+e);
            Motor_uncalibrate(o->motor+e);
            
            o->joint[e].cable_constr.max_tension = SPRING_MAX_TENSION;
            
            o->joint[e].cable_calib.pwm         = calibrator->params.type11.pwm;
            o->joint[e].cable_calib.cable_range = calibrator->params.type11.cable_range;
            
            o->joint[e].cable_calib.delta       = 900;//calibrator->params.type11.delta;
            o->joint[e].cable_calib.target      = o->joint[e].pos_fbk + o->joint[e].cable_calib.delta;
            
            if (o->joint[e].cable_calib.target > o->joint[e].pos_max) o->joint[e].cable_calib.target = o->joint[e].pos_max;  
            if (o->joint[e].cable_calib.target < o->joint[e].pos_min) o->joint[e].cable_calib.target = o->joint[e].pos_min;
            
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            o->calibration_wait = TRUE;
            
            o->calibration_timeout = 0;
            
            break;
        }
        
        default:
            break;
    }
}

void JointSet_send_debug_message(char *message, uint8_t jid)
{

    eOerrmanDescriptor_t errdes = {0};

    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = jid;
    errdes.par16            = 0;
    errdes.par64            = 0;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, message, NULL, &errdes);

}
