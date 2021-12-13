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
#include "EOtheEntities.h"

#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"
#include "Pid.h"

#include "JointSet.h"

#include "Calibrators.h"

static void JointSet_set_inner_control_flags(JointSet* o);

static const CTRL_UNITS DEG2ICUB = 65536.0f/360.0f;
static const CTRL_UNITS ICUB2DEG = 360.0f/65536.0f;

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
       
    o->Jjm = NULL; o->Sjm = NULL;
    o->Jmj = NULL; o->Smj = NULL;
    
    o->Sje = NULL;
    
    
    o->absEncoder = NULL;
    
    o->control_mode = eomc_controlmode_notConfigured;
    o->interaction_mode = eOmc_interactionmode_stiff;
    
    o->torque_ctrl_out_type = eomc_ctrl_out_type_n_a;
    o->olooop_ctrl_out_type = eomc_ctrl_out_type_n_a;
    o->curent_ctrl_out_type = eomc_ctrl_out_type_n_a;
    o->postrj_ctrl_out_type = eomc_ctrl_out_type_n_a;
    o->veltrj_ctrl_out_type = eomc_ctrl_out_type_n_a;
    o->mixtrj_ctrl_out_type = eomc_ctrl_out_type_n_a;
    o->posdir_ctrl_out_type = eomc_ctrl_out_type_n_a;
    o->veldir_ctrl_out_type = eomc_ctrl_out_type_n_a;
    
    o->pos_control_active = TRUE;
    o->trq_control_active = FALSE; 
    o->USE_SPEED_FBK_FROM_MOTORS = TRUE;
    
    o->is_calibrated = FALSE;
    
    o->special_constraint = eomc_jsetconstraint_none;
    
    o->calibration_in_progress = eomc_calibration_typeUndefined;

#ifdef WRIST_MK2
    o->is_parking = FALSE;
    o->must_park = TRUE;
    
    o->wrist_decoupler.initialize();
    
    Trajectory_init(&(o->ypr_trajectory[0]), 0, 0, 0);
    Trajectory_init(&(o->ypr_trajectory[1]), 0, 0, 0);
    Trajectory_init(&(o->ypr_trajectory[2]), 0, 0, 0);
    
    Trajectory_config_limits(&(o->ypr_trajectory[0]), -90.0f*DEG2ICUB, 90.0f*DEG2ICUB, 90.0f*DEG2ICUB, ZERO);
    Trajectory_config_limits(&(o->ypr_trajectory[1]), -60.0f*DEG2ICUB, 50.0f*DEG2ICUB, 90.0f*DEG2ICUB, ZERO);
    Trajectory_config_limits(&(o->ypr_trajectory[2]), -30.0f*DEG2ICUB, 30.0f*DEG2ICUB, 90.0f*DEG2ICUB, ZERO);
    
    o->ypr_pos_ref[0] = ZERO;
    o->ypr_pos_ref[1] = ZERO;
    o->ypr_pos_ref[2] = ZERO;
    
    o->ypr_vel_ref[0] = ZERO;
    o->ypr_vel_ref[1] = ZERO;
    o->ypr_vel_ref[2] = ZERO;

    o->ypr_pos_fbk[0] = ZERO;
    o->ypr_pos_fbk[1] = ZERO;
    o->ypr_pos_fbk[2] = ZERO;
    
    o->arm_pos_off[0] = 145.0f;
    o->arm_pos_off[1] = 270.0f;
    o->arm_pos_off[2] =  40.0f;
#endif
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
    float** Jjm, float** Sjm,
    float** Jmj, float** Smj,
    float** Sje
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
    o->Jjm = Jjm; o->Sjm = Sjm;
    o->Jmj = Jmj; o->Smj = Smj;
    o->Sje = Sje;
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
        
                o->joint[j].pos_fbk_from_motors += Sjm[j][m] * (float)o->motor[m].pos_fbk;
                o->joint[j].vel_fbk_from_motors += Sjm[j][m] * (float)o->motor[m].vel_fbk;
            }
        }
    }
    else
    {
        for (js=0; js<N; ++js)
        {
            j = o->joints_of_set[js];
            
            o->joint[j].pos_fbk_from_motors = (float)o->motor[j].pos_fbk;
            o->joint[j].vel_fbk_from_motors = (float)o->motor[j].vel_fbk;
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
        
        //return;
    }
    else // encoder coupling
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
    
    // accelerations
    for (js=0; js<N; ++js)
    {
        j = o->joints_of_set[js];
        o->joint[j].acc_fbk = CTRL_LOOP_FREQUENCY*(o->joint[j].vel_fbk - o->joint[j].vel_fbk_old);
        o->joint[j].vel_fbk_old = o->joint[j].vel_fbk;
    }
}

BOOL JointSet_do_check_faults(JointSet* o)
{
    int N = *(o->pN);
    int E = *(o->pE);
    
    BOOL fault = FALSE;
    o->external_fault = FALSE;
    
    // Set to dummy the fault state communicated to embObj before checking for new faults
    for(int k=0; k<N; ++k)
    {
        Motor_set_fault_state_dummy(o->motor+o->motors_of_set[k]);
    }
    
    for (int k=0; k<N; ++k)
    {
        if (Joint_check_faults(o->joint+o->joints_of_set[k]))
        {
            fault = TRUE;
        }
        
        if (Motor_check_faults(o->motor+o->motors_of_set[k]))
        {
            fault = TRUE;
            o->joint[o->joints_of_set[k]].control_mode = eomc_controlmode_hwFault;
        }
        
        if (Motor_is_external_fault(o->motor+o->motors_of_set[k]))
        {
            o->external_fault = TRUE;
        }
    }
    BOOL encoder_fault = FALSE;
    for (int k=0; k<E; ++k)
    {
        AbsEncoder* enc = o->absEncoder+o->encoders_of_set[k];
        if (AbsEncoder_is_in_fault(enc))
        {
            fault = TRUE;
            encoder_fault = TRUE;
        }
    }
    //if an encoder of this set is in fault ten set hw fault on each joint of this set.
    if(encoder_fault)
    {
        for (int k=0; k<N; ++k)
        {
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
static void JointSet_do_current_control(JointSet* o);

void JointSet_do_control(JointSet* o)
{
    switch (o->motor_input_type)
    {
    case eomc_ctrl_out_type_pwm:
        JointSet_do_pwm_control(o);
        return;
    
    case eomc_ctrl_out_type_vel:
        JointSet_do_vel_control(o);
        return;
    
    case eomc_ctrl_out_type_cur:
        JointSet_do_current_control(o);
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

#ifdef WRIST_MK2
static void JointSet_start_park(JointSet* o)
{
    o->is_parking = TRUE;
    
    Joint_set_control_mode(&(o->joint[0]), eomc_controlmode_cmd_position);
    Joint_set_control_mode(&(o->joint[1]), eomc_controlmode_cmd_position);
    Joint_set_control_mode(&(o->joint[2]), eomc_controlmode_cmd_position);
    
    Joint_set_pos_ref(&(o->joint[0]), ZERO, 45.0f*182.044f);
    Joint_set_pos_ref(&(o->joint[1]), ZERO, 45.0f*182.044f);
    Joint_set_pos_ref(&(o->joint[2]), ZERO, 45.0f*182.044f);
    
    static char msg[] = "START PARK";
    
    JointSet_send_debug_message(msg, 0, 0, 0);
}

static void JointSet_stop_park(JointSet* o)
{
    Joint_stop(&(o->joint[0]));
    Joint_stop(&(o->joint[1]));
    Joint_stop(&(o->joint[2]));
    
    Trajectory_stop(&(o->ypr_trajectory[0]),ZERO);
    Trajectory_stop(&(o->ypr_trajectory[1]),ZERO);
    Trajectory_stop(&(o->ypr_trajectory[2]),ZERO);
    
    JointSet_set_control_mode(o, (eOmc_controlmode_command_t)(o->control_mode));
    
    o->is_parking = FALSE;
    
    static char msg[] = "PARK DONE";
    
    JointSet_send_debug_message(msg, 0, 0, 0);
}

static BOOL JointSet_is_parked(JointSet* o)
{
    if (!o->is_parking) return TRUE;
    
    return (fabs(o->joint[0].pos_fbk) < 400.0f && fabs(o->joint[1].pos_fbk) < 400.0f && fabs(o->joint[2].pos_fbk) < 400.0f);
}
#endif

static int control_output_type(JointSet* o, int16_t control_mode)
{
    if (control_mode == eomc_controlmode_cmd_force_idle) return eomc_ctrl_out_type_off;
    
    if (control_mode == eomc_controlmode_cmd_idle) return eomc_ctrl_out_type_off;
    
    if (o->interaction_mode == eOmc_interactionmode_stiff)
    {
        switch (control_mode)
        {
            case eomc_controlmode_openloop:
                return o->olooop_ctrl_out_type;

            case eomc_controlmode_current:
                return o->curent_ctrl_out_type;
            
            case eomc_controlmode_torque:
                return o->torque_ctrl_out_type;
            
            case eomc_controlmode_direct:
                return o->posdir_ctrl_out_type;
            
            case eomc_ctrlmval_velocity_pos:
            case eomc_controlmode_mixed:
                return o->mixtrj_ctrl_out_type;
            
            case eomc_controlmode_position:
                return o->postrj_ctrl_out_type;
            
            case eomc_controlmode_velocity: //
                return o->veltrj_ctrl_out_type;
            
            case eomc_controlmode_vel_direct:
                return o->veldir_ctrl_out_type;
            
            default:
                return eomc_ctrl_out_type_n_a;
        }
    }
    else
    {
        return o->torque_ctrl_out_type;
    }
}

BOOL JointSet_set_control_mode(JointSet* o, eOmc_controlmode_command_t control_mode_cmd)
{
#ifdef WRIST_MK2
    Trajectory_stop(&(o->ypr_trajectory[0]), o->ypr_pos_fbk[0]);
    Trajectory_stop(&(o->ypr_trajectory[1]), o->ypr_pos_fbk[1]);
    Trajectory_stop(&(o->ypr_trajectory[2]), o->ypr_pos_fbk[2]);
#endif
    if (control_mode_cmd != eomc_controlmode_cmd_force_idle)
    {
#ifndef WRIST_MK2
        if ((eOmc_controlmode_t)control_mode_cmd == o->control_mode) return TRUE;
#endif
        if (o->control_mode == eomc_controlmode_calib) return FALSE;
    
        if (o->control_mode == eomc_controlmode_notConfigured) return FALSE;
    
        if ((control_mode_cmd == eomc_controlmode_cmd_torque) && (o->torque_ctrl_out_type == eomc_ctrl_out_type_n_a)) return FALSE;
    }
    
    int N = *(o->pN);
    int E = *(o->pE);
    
    if (o->control_mode == eomc_controlmode_hwFault)
    {
        if (control_mode_cmd == eomc_controlmode_cmd_force_idle)
        {
#ifdef WRIST_MK2
		o->must_park = TRUE;
#endif
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
    
    int motor_input_type = control_output_type(o, control_mode_cmd);
        
    if (motor_input_type == eomc_ctrl_out_type_n_a)
    {
        return FALSE;
    }
        
    o->motor_input_type = motor_input_type;
    
    switch (control_mode_cmd)
    {
    case eomc_controlmode_cmd_force_idle:
    case eomc_controlmode_cmd_idle:
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
            
            Motor_set_idle(o->motor+o->motors_of_set[k]);
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode_cmd);
        }
        break;
#ifdef WRIST_MK2    
    case eomc_controlmode_cmd_mixed:
    case eomc_controlmode_cmd_velocity_pos:
    case eomc_controlmode_cmd_position:
    case eomc_controlmode_cmd_velocity:
    {        
        //if (o->external_fault) return FALSE;
                
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);

            if (!Motor_set_run(o->motor+o->motors_of_set[k], o->motor_input_type)) return FALSE;
        }
        
        for (int k=0; k<N; ++k)
        { 
            Joint_set_control_mode(o->joint+o->joints_of_set[k], eomc_controlmode_cmd_direct);
        }
        break;
    } 
#endif
    case eomc_controlmode_cmd_openloop:
    case eomc_controlmode_cmd_current:    
    case eomc_controlmode_cmd_torque:
    case eomc_controlmode_cmd_direct:
#ifndef WRIST_MK2
    case eomc_controlmode_cmd_mixed:
    case eomc_controlmode_cmd_velocity_pos:
    case eomc_controlmode_cmd_position:
    case eomc_controlmode_cmd_velocity:
#endif
    case eomc_controlmode_cmd_vel_direct:
    {        
        //if (o->external_fault) return FALSE;
                
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);

            if (!Motor_set_run(o->motor+o->motors_of_set[k], o->motor_input_type)) return FALSE;
        }
        
        for (int k=0; k<N; ++k)
        { 
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode_cmd);
        }
        break;
    }    
    default:
        return FALSE;
    }
    
    switch (control_mode_cmd)
    {
        case eomc_controlmode_cmd_velocity_pos:
            o->control_mode = (eOmc_controlmode_t)eomc_controlmode_cmd_mixed;
            break;
        //case eomc_controlmode_cmd_velocity:
        //    o->control_mode = (eOmc_controlmode_t)eomc_controlmode_cmd_vel_direct;
        //    break;
        case eomc_controlmode_cmd_force_idle:
            o->control_mode = eomc_controlmode_idle;
            break;
        default:
            o->control_mode = (eOmc_controlmode_t)control_mode_cmd;
            break;
    }
    
    //JointSet_send_debug_message("SET CONTROLMODE", 0, control_mode_cmd, o->control_mode);
    
    JointSet_set_inner_control_flags(o);
    
    return TRUE;
}

void JointSet_set_interaction_mode(JointSet* o, eOmc_interactionmode_t interaction_mode)
{
    if (interaction_mode == o->interaction_mode) return;
    
    if ((interaction_mode == eOmc_interactionmode_compliant) && (o->torque_ctrl_out_type == eomc_ctrl_out_type_n_a)) return;
    
    o->interaction_mode = interaction_mode;
    
    int motor_input_type = control_output_type(o, o->control_mode);
    
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

static void JointSet_manage_R1_finger_tension_constraint(JointSet* o)
{
    for (int k=0; k<*(o->pN); ++k)
    {
        if (Joint_manage_R1_finger_tension_constraint(o->joint+o->joints_of_set[k]))
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
    
    int32_t out0 = o->motor[m0].output;
    int32_t out1 = o->motor[m1].output;
    int32_t out2 = o->motor[m2].output;
    
    if (o->joint[j0].minjerkPID.Kp < 0.0f)
    {
        out0 = -out0;
        out1 = -out1;
        out2 = -out2;
    }
    
    if ( !(rho0<=rho1 && rho0<=rho2 && out0>=0) 
      && !(rho0>=rho1 && rho0>=rho2 && out0<=0))
    {
        o->motor[j0].output = 0;
        Joint_stop(o->joint+j0);
    }
                
    if ( !(rho1<=rho2 && rho1<=rho0 && out1>=0) 
      && !(rho1>=rho2 && rho1>=rho0 && out1<=0))
    {
        o->motor[j1].output = 0;
        Joint_stop(o->joint+j1);
    }
                
    if ( !(rho2<=rho0 && rho2<=rho1 && out2>=0) 
      && !(rho2>=rho0 && rho2>=rho1 && out2<=0))
    {
        o->motor[j2].output = 0;
        Joint_stop(o->joint+j2);
    }
}

static CTRL_UNITS wrap180(CTRL_UNITS x)
{
    while (x >  180.0f) x -= 360.0f;
    while (x < -180.0f) x += 360.0f;

    return x;
}

void JointSet_do_pwm_control(JointSet* o)
{
    int N = *(o->pN);
        
    BOOL limits_torque_protection = FALSE;
#ifdef WRIST_MK2
    if (o->must_park)
    {
        JointSet_start_park(o);
  
        o->must_park = FALSE;
        
        static char msg[] = "BUONGIORNISSIMO";
        
        JointSet_send_debug_message(msg, 0, 0, 0);
    }
    
    if (o->is_parking)
    {
        if (JointSet_is_parked(o))
        {
            JointSet_stop_park(o);
        }
    }
    
    if (!o->is_parking)
    {
        Trajectory_do_step(o->ypr_trajectory,   o->ypr_pos_ref,   o->ypr_vel_ref,   o->ypr_acc_ref  );
        Trajectory_do_step(o->ypr_trajectory+1, o->ypr_pos_ref+1, o->ypr_vel_ref+1, o->ypr_acc_ref+1);
        Trajectory_do_step(o->ypr_trajectory+2, o->ypr_pos_ref+2, o->ypr_vel_ref+2, o->ypr_acc_ref+2);
    }
    else
    {
        o->ypr_pos_ref[0] = o->ypr_vel_ref[0] = o->ypr_acc_ref[0] = ZERO;
        o->ypr_pos_ref[1] = o->ypr_vel_ref[1] = o->ypr_acc_ref[1] = ZERO;
        o->ypr_pos_ref[2] = o->ypr_vel_ref[2] = o->ypr_acc_ref[2] = ZERO;
    }
    
//    static int noflood = 0;
//    
//    if (++noflood > 1000)
//    {
//        noflood = 0;
//        int16_t par16 = (int16_t)(o->ypr_pos_ref[0]*ICUB2DEG);
//        int32_t par32 = (int32_t)(Trajectory_get_target_position(o->ypr_trajectory)*ICUB2DEG);
//        JointSet_send_debug_message(NULL, 0, par16, par32);
//    }
    
    o->wrist_decoupler.rtU.ypr[0] = ICUB2DEG*o->ypr_pos_ref[0];
    o->wrist_decoupler.rtU.ypr[1] = ICUB2DEG*o->ypr_pos_ref[1];
    o->wrist_decoupler.rtU.ypr[2] = ICUB2DEG*o->ypr_pos_ref[2];
    
    o->wrist_decoupler.rtU.theta_meas[0] = ICUB2DEG*(o->joint[0]).pos_fbk + o->arm_pos_off[0];
    o->wrist_decoupler.rtU.theta_meas[1] = ICUB2DEG*(o->joint[1]).pos_fbk + o->arm_pos_off[1];
    o->wrist_decoupler.rtU.theta_meas[2] = ICUB2DEG*(o->joint[2]).pos_fbk + o->arm_pos_off[2];
    
    //////////////////////////
    o->wrist_decoupler.step();
    //////////////////////////
    
    if (o->wrist_decoupler.rtY.singularity)
    {
        if (!o->is_parking)
        {
            JointSet_start_park(o);
        }
    }
    
    o->ypr_pos_fbk[0] = DEG2ICUB*(o->wrist_decoupler.rtY.ypr_meas[0]);
    o->ypr_pos_fbk[1] = DEG2ICUB*(o->wrist_decoupler.rtY.ypr_meas[1]);    
    o->ypr_pos_fbk[2] = DEG2ICUB*(o->wrist_decoupler.rtY.ypr_meas[2]);
        
//    o->ypr_pos_fbk[0] = ZERO;
//    o->ypr_pos_fbk[1] = ZERO;    
//    o->ypr_pos_fbk[2] = ZERO;

    CTRL_UNITS arm_pos_ref[3];
    
    arm_pos_ref[0] = DEG2ICUB*wrap180(o->wrist_decoupler.rtY.theta_star[0] - o->arm_pos_off[0]);
    arm_pos_ref[1] = DEG2ICUB*wrap180(o->wrist_decoupler.rtY.theta_star[1] - o->arm_pos_off[1]);
    arm_pos_ref[2] = DEG2ICUB*wrap180(o->wrist_decoupler.rtY.theta_star[2] - o->arm_pos_off[2]);
    
//    arm_pos_ref[0] = ZERO;
//    arm_pos_ref[1] = ZERO;
//    arm_pos_ref[2] = ZERO;
#endif
    for (int js=0; js<N; ++js)
    {
        Joint *pJoint = o->joint+o->joints_of_set[js];
#ifdef WRIST_MK2 
        if (!o->is_parking)
        {
            Joint_set_pos_raw(pJoint, arm_pos_ref[js]);
        }
#endif
        Joint_do_pwm_or_current_control(pJoint);
       
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
                
                    // inverse FALSE Jacobian
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
            
            if (o->Smj)
            {
                joint_pwm_ref[j] = ZERO;
            
                for (int ms=0; ms<N; ++ms)
                {
                    int m = o->motors_of_set[ms];
                
                    // transposed inverse Jacobian
                    joint_pwm_ref[j] += o->Smj[m][j]*o->motor[m].pwm_ref;
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
        case eomc_jsetconstraint_trifid:
            JointSet_manage_trifid_constraint(o);
            break;
        
        case eomc_jsetconstraint_cerhand:
            JointSet_manage_cable_constraint(o);
            break;
        
        case eomc_jsetconstraint_cerhand2:
            JointSet_manage_R1_finger_tension_constraint(o);
            break;

        default:
            break;
    }
}

static void JointSet_do_current_control(JointSet* o)
{
    int N = *(o->pN);
        
    BOOL limits_torque_protection = FALSE;
        
    for (int js=0; js<N; ++js)
    {
        Joint *pJoint = o->joint+o->joints_of_set[js];
        
        Joint_do_pwm_or_current_control(pJoint);
       
        if (o->trq_control_active && Joint_pushing_limit(pJoint))
        {
            limits_torque_protection = TRUE;
        }
    }
    
    CTRL_UNITS motor_current_ref = ZERO;
    
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
                
                motor_current_ref = Motor_do_trq_control(o->motor+m, motor_trq_ref, motor_trq_fbk);
            }
            else
            {
                motor_current_ref = Motor_do_trq_control(o->motor+m, o->joint[m].trq_ref, o->joint[m].trq_fbk);
            }
        }
        else
        {
            //if (o->Jmj)
            if (o->Jjm)
            {
                motor_current_ref = ZERO;
            
                for (int js=0; js<N; ++js)
                {
                    int j = o->joints_of_set[js];
                
                    // inverse FALSE Jacobian NO!!!
                    // motor_current_ref += o->Jmj[m][j]*o->joint[j].output;
                    // transposed direct Jacobian
                    motor_current_ref += o->Jjm[j][m]*o->joint[j].output;
                }
            }
            else
            {
                motor_current_ref = o->joint[m].output;
            }
        }

        Motor_set_Iqq_ref(o->motor+m, motor_current_ref);
    }
    
    if (limits_torque_protection)
    {
        CTRL_UNITS joint_current_ref[MAX_JOINTS_PER_BOARD];
        
        for (int js=0; js<N; ++js)
        {
            int j = o->joints_of_set[js];
            
            if (o->Smj)
            {
                joint_current_ref[j] = ZERO;
            
                for (int ms=0; ms<N; ++ms)
                {
                    int m = o->motors_of_set[ms];
                
                    // transposed inverse Jacobian
                    joint_current_ref[j] += o->Smj[m][j]*o->motor[m].Iqq_ref;
                }
            }
            else
            {
                joint_current_ref[j] = o->motor[j].Iqq_ref;
            }
            
            if (Joint_pushing_limit(o->joint+j))
            {
                if ((o->joint[j].output_lim > ZERO) ^ (o->joint[j].output_lim < joint_current_ref[j]))
                {
                    joint_current_ref[j] = o->joint[j].output_lim; 
                }
            }
        }
        
        for (int ms=0; ms<N; ++ms)
        {
            int m = o->motors_of_set[ms];
        
            if (o->Jjm)
            {
                motor_current_ref = ZERO;
            
                for (int js=0; js<N; ++js)
                {
                    int j = o->joints_of_set[js];
                
                    // transposed jacobian
                    motor_current_ref += o->Jjm[j][m]*joint_current_ref[j];
                }
            
                Motor_set_Iqq_ref(o->motor+m, motor_current_ref);
            }
            else
            {
                Motor_set_Iqq_ref(o->motor+m, joint_current_ref[m]);
            }
        }
    }
    
    switch (o->special_constraint)
    {
        case eomc_jsetconstraint_trifid:
            JointSet_manage_trifid_constraint(o);
            break;
        
        case eomc_jsetconstraint_cerhand:
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
    
        if (o->Smj)
        {
            // inverse Jacobian
            float **Ji = o->Smj; 
            
            CTRL_UNITS motor_vel_ref = ZERO;
        
            for (int js=0; js<N; ++js)
            {
                int j = o->joints_of_set[js];
                
                // inverse Jacobian OK
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
        case eomc_jsetconstraint_trifid:
            JointSet_manage_trifid_constraint(o);
            break;
        
        case eomc_jsetconstraint_cerhand:
            JointSet_manage_cable_constraint(o);
            break;
        
        case eomc_jsetconstraint_cerhand2:
            JointSet_manage_R1_finger_tension_constraint(o);
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
        case eomc_controlmode_velocity: //
        case eomc_controlmode_vel_direct:
        case eomc_ctrlmval_velocity_pos:
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
            //first of all I need to restore rotor limits if i was doing mais calib
            if(eomc_calibration_typeMixed == o->calibration_in_progress)
            {
                for (int k=0;  k<*(o->pN); ++k)
                {
                    int m = o->motors_of_set[k];
                    int j = o->joints_of_set[k];
                    Joint* j_ptr = o->joint+j;
                    if(eomc_calibration_type6_mais == j_ptr->running_calibration.type)
                    {                
                        //restore rotor limits
                        o->motor[m].pos_min = j_ptr->running_calibration.data.type6.rotorposmin;
                        o->motor[m].pos_max = j_ptr->running_calibration.data.type6.rotorposmax;
                    }
                }
            }    
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
        
        case eomc_calibration_type12_absolute_sensor:
            o->is_calibrated = JointSet_do_wait_calibration_12(o);
            break;
            
        case eomc_calibration_type13_cer_hands_2:
            o->is_calibrated = JointSet_do_wait_calibration_13(o);
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
            Motor_set_run(o->motor+e, o->postrj_ctrl_out_type);
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            break;
        }
        case eomc_calibration_type5_hard_stops:
        {
//            //Debug code
//            char message[150];
//            snprintf(message, sizeof(message), "calib cmd rec: pwm%d cz%d", calibrator->params.type5.pwmlimit, calibrator->params.type5.calibrationZero);
//            JointSet_send_debug_message(message, e);
            o->calibration_timeout = 0;
            BOOL ret = Motor_calibrate_moving2Hardstop(o->motor+e, calibrator->params.type5.pwmlimit, (calibrator->params.type5.final_pos - calibrator->params.type5.calibrationZero));
            
            if(!ret)
            {
                o->joint[e].control_mode = joint_controlMode_old;
                o->control_mode = jointSet_controlMode_old;
                return;
            }
            
            Joint_set_hardware_limit(o->joint+e);
            
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
                JointSet_send_debug_message(info, e, 0, 0);
                ////debug code ended
                return;
            }
            
                
            //if I'm here I can perform calib type 6.
            
            // 2) set state
            o->calibration_timeout = 0;
            o->calibration_in_progress = eomc_calibration_typeMixed;
            o->joint[e].running_calibration.type = (eOmc_calibration_type_t)calibrator->type;
            o->joint[e].running_calibration.data.type6.is_active = TRUE;
            o->joint[e].running_calibration.data.type6.state = calibtype6_st_inited;
            
            
//            o->joint[e].calib_type6_data.is_active = TRUE;
//            o->joint[e].calib_type6_data.state = calibtype6_st_inited;
//            o->joint[e].calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            
            
            // 3) calculate new joint encoder factor and param_zero
            eOmc_joint_config_t *jconfig = &o->joint[e].eo_joint_ptr->config;
           
            float computedJntEncoderResolution = (float)(calibrator->params.type6.vmax - calibrator->params.type6.vmin) / (float) (jconfig->userlimits.max  - jconfig->userlimits.min);
            
            eOresult_t res = eo_appEncReader_UpdatedMaisConversionFactors(eo_appEncReader_GetHandle(), e, computedJntEncoderResolution);
            if(eores_OK != res)
            {    
                ////debug code
                char info[70];
                snprintf(info, 70, "calib6: error updating Mais conversion factor j%d", e);
                JointSet_send_debug_message(info, e, 0, 0);
                ////debug code ended
                return;
            }

            AbsEncoder_config_resolution(o->absEncoder+e, computedJntEncoderResolution);
            
            //Now I need to re-init absEncoder because I chenged maisConversionFactor, therefore the values returned by EOappEncoreReder are changed.
            o->absEncoder[e].state.bits.not_initialized = TRUE;

            float computedJntEncoderZero =  - (float)(jconfig->userlimits.min) + ((float)(calibrator->params.type6.vmin) / computedJntEncoderResolution);
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
            o->calibration_timeout = 0;
            o->calibration_in_progress = eomc_calibration_typeMixed;
            o->joint[e].running_calibration.type = (eOmc_calibration_type_t)calibrator->type;
            o->joint[e].running_calibration.data.type7.state = calibtype7_st_inited;
            o->joint[e].running_calibration.data.type7.is_active = TRUE;
            
//            o->joint[e].calib_type7_data.is_active = TRUE;
//            o->joint[e].calib_type7_data.state = calibtype7_st_inited;
//            o->joint[e].calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            
            // 2) calculate new joint encoder factor and param_zero
            eOmc_joint_config_t *jconfig = &o->joint[e].eo_joint_ptr->config;
            float computedJntEncoderResolution = (float)(calibrator->params.type7.vmax - calibrator->params.type7.vmin) / (float) (jconfig->userlimits.max  - jconfig->userlimits.min);
            
            int32_t offset = (((float)calibrator->params.type7.vmin)/computedJntEncoderResolution) - jconfig->userlimits.min;
            
            eOresult_t res = eo_appEncReader_UpdatedHallAdcOffset(eo_appEncReader_GetHandle(), e, offset);
            if(eores_OK != res)
            {    
                ////debug code 
                char info[70];
                snprintf(info, 70, "calib7: error updating HallADC offset j%d", e);
                JointSet_send_debug_message(info, e, 0, 0);
                ////debug code ended
                return;
            }
            
            
            res = eo_appEncReader_UpdatedHallAdcConversionFactors(eo_appEncReader_GetHandle(), e, computedJntEncoderResolution);
            if(eores_OK != res)
            {    
                ////debug code 
                char info[70];
                snprintf(info, 70, "calib7: error updating HallADC conversion factor j%d", e);
                JointSet_send_debug_message(info, e, 0, 0);
                ////debug code ended
                return;
            }

            AbsEncoder_config_resolution(o->absEncoder+e, computedJntEncoderResolution);
            
            //Now I need to re init absEncoder because I chenged hallADCConversionFactor, therefore the values returned by EOappEncoreReder are changed.
            o->absEncoder[e].state.bits.not_initialized = TRUE;
            
            float computedJntEncoderZero =  (((float)calibrator->params.type7.vmin) / computedJntEncoderResolution) - ((float)(jconfig->userlimits.min)) - offset;

            
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
            
            Motor_set_run(o->motor+o->motors_of_set[0], eomc_ctrl_out_type_pwm);
            Motor_set_run(o->motor+o->motors_of_set[1], eomc_ctrl_out_type_pwm);
            Motor_set_run(o->motor+o->motors_of_set[2], eomc_ctrl_out_type_pwm);
            
            o->tripod_calib.start_pos[0] = o->motor[o->motors_of_set[0]].pos_fbk;
            o->tripod_calib.start_pos[1] = o->motor[o->motors_of_set[1]].pos_fbk;
            o->tripod_calib.start_pos[2] = o->motor[o->motors_of_set[2]].pos_fbk;
            
            break;
        }
        
        case eomc_calibration_type10_abs_hard_stop:
        {
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            o->calibration_timeout = 0;
            
            Joint_set_hardware_limit(o->joint+e);
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            Motor_set_run(o->motor+e, eomc_ctrl_out_type_pwm);
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
            Motor_set_run(o->motor+e, eomc_ctrl_out_type_pwm);
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
        case eomc_calibration_type12_absolute_sensor:
        {
            int32_t offset;
            int32_t zero;
            eOmc_joint_config_t *jointcfg = eo_entities_GetJointConfig(eo_entities_GetHandle(), e);
            //1) Take absolute value of calibation parametr
            int32_t abs_raw = (calibrator->params.type12.rawValueAtZeroPos > 0) ? calibrator->params.type12.rawValueAtZeroPos : -calibrator->params.type12.rawValueAtZeroPos;
            // 1.1) update abs_raw with gearbox_E2J
            abs_raw = abs_raw * jointcfg->gearbox_E2J;
            // 2) calculate offset
            if(abs_raw >= TICKS_PER_HALF_REVOLUTION)
                offset = abs_raw - TICKS_PER_HALF_REVOLUTION;
            else
                offset = abs_raw + TICKS_PER_HALF_REVOLUTION;
            
            // 3) find out sign of zero
            
            if(jointcfg->jntEncoderResolution > 0)
                zero = TICKS_PER_HALF_REVOLUTION / jointcfg->gearbox_E2J;
            else
                zero = -TICKS_PER_HALF_REVOLUTION / jointcfg->gearbox_E2J;
            
            zero+=calibrator->params.type12.calibrationDelta;  //this parameter should contain only the delta
            // 4) call calibration function
            
            ////debug code
            char info[80];
            snprintf(info, sizeof(info), "CALIB 12 j %d: offset=%d zero=%d ", e, offset, zero);
            JointSet_send_debug_message(info, e, 0, 0);
            ////debug code ended
            AbsEncoder_calibrate_absolute(o->absEncoder+e, offset, zero);
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            Motor_set_run(o->motor+e, o->postrj_ctrl_out_type);
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;

            break;
        }
        
        case eomc_calibration_type13_cer_hands_2:
        {
            AbsEncoder* enc = o->absEncoder + 4*e;
            
            //int32_t rawValueAtZeroPos[4];
            
            //rawValueAtZeroPos[0] = calibrator->params.type13.rawValueAtZeroPos0;
            //rawValueAtZeroPos[1] = calibrator->params.type13.rawValueAtZeroPos1;
            //rawValueAtZeroPos[2] = calibrator->params.type13.rawValueAtZeroPos2;
            //rawValueAtZeroPos[3] = calibrator->params.type13.rawValueAtZeroPos3;
            
            AbsEncoder_calibrate_absolute(enc  , calibrator->params.type13.rawValueAtZeroPos0, enc[0].mul*32767);
            AbsEncoder_calibrate_absolute(enc+1, calibrator->params.type13.rawValueAtZeroPos1, enc[1].mul*32767);
            AbsEncoder_calibrate_absolute(enc+2, calibrator->params.type13.rawValueAtZeroPos2, enc[2].mul*32767);
            AbsEncoder_calibrate_absolute(enc+3, calibrator->params.type13.rawValueAtZeroPos3, enc[3].mul*32767);
            
            /*
            for (int k=0; k<4; ++k)
            {
                int32_t offset;
                int32_t zero;
                
                //eOmc_joint_config_t *jointcfg = eo_entities_GetJointConfig(eo_entities_GetHandle(), e);
                //1) Take absolute value of calibation parametr
                int32_t abs_raw = (rawValueAtZeroPos[k] > 0) ? rawValueAtZeroPos[k] : -rawValueAtZeroPos[k];
                // 1.1) update abs_raw with gearbox_E2J
                //abs_raw = abs_raw * jointcfg->gearbox_E2J;
                // 2) calculate offset
                if(abs_raw >= TICKS_PER_HALF_REVOLUTION)
                    offset = abs_raw - TICKS_PER_HALF_REVOLUTION;
                else
                    offset = abs_raw + TICKS_PER_HALF_REVOLUTION;
            
                // 3) find out sign of zero
            
                //if(jointcfg->jntEncoderResolution > 0)
                    zero = TICKS_PER_HALF_REVOLUTION;// / jointcfg->gearbox_E2J;
                //else
                //    zero = -TICKS_PER_HALF_REVOLUTION;// / jointcfg->gearbox_E2J;
            
                //zero+=calibrator->params.type12.calibrationDelta;  //this parameter should contain only the delta
                // 4) call calibration function
            
                ////debug code
                char info[80];
                snprintf(info, sizeof(info), "CALIB 13 j(%d)[%d]: offset=%d zero=%d ", e, k, offset, zero);
                JointSet_send_debug_message(info, e);
                
                ////debug code ended
                AbsEncoder_calibrate_absolute(o->absEncoder+4*e+k, offset, zero);
            }
            */
            
            JointSet_do_odometry(o);
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
        }
        
        default:
            break;
    }
}

void JointSet_send_debug_message(char *message, uint8_t jid, uint16_t par16, uint64_t par64)
{

    eOerrmanDescriptor_t errdes = {0};

    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = jid;
    errdes.par16            = par16;
    errdes.par64            = par64;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, message, NULL, &errdes);

}

#ifdef WRIST_MK2
BOOL JointSet_set_pos_ref(JointSet* o, int j, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref)
{
    if (o->is_parking) return FALSE;
    
    if ((o->control_mode != eomc_controlmode_position) && (o->control_mode != eomc_controlmode_mixed) && (o->control_mode != eomc_ctrlmval_velocity_pos))
    {
        return FALSE;
    }
    
    if (vel_ref == 0.0f) return TRUE;
    
    Trajectory_set_pos_end(&(o->ypr_trajectory[j]), pos_ref, vel_ref);
    
    return TRUE;
}

void JointSet_stop(JointSet* o, int j)
{
    Trajectory_stop(&(o->ypr_trajectory[j]), o->ypr_pos_fbk[j]);
}

extern void JointSet_get_state(JointSet* o, int j, eOmc_joint_status_t* joint_state)
{
    joint_state->core.modes.interactionmodestatus    = o->interaction_mode;
    joint_state->core.modes.controlmodestatus        = o->control_mode;
    joint_state->core.modes.ismotiondone             = Trajectory_is_done(&(o->ypr_trajectory[j]));
    joint_state->core.measures.meas_position         = o->ypr_pos_fbk[j];           
    joint_state->core.measures.meas_velocity         = ZERO;        
    joint_state->core.measures.meas_acceleration     = ZERO;      
    joint_state->core.measures.meas_torque           = ZERO;
}
#endif
