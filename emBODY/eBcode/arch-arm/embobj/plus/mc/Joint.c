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

#include "Joint.h"

#include "hal_adc.h"

#include "EOtheCANprotocol.h"

#include "EOtheErrorManager.h"
#include "EoError.h"

static void Joint_set_inner_control_flags(Joint* o);

Joint* Joint_new(uint8_t n)
{
    Joint* o = NEW(Joint, n);
    
    for (int i=0; i<n; ++i)
    {
        Joint_init(o+i);
    }
    
    return o;
}

void Joint_init(Joint* o)
{
    o->dead_zone = ZERO;
    
    o->pos_min = ZERO;
    o->pos_max = ZERO;
    o->vel_max = ZERO;
    o->acc_max = ZERO;

    o->pos_min_soft = ZERO;
    o->pos_max_soft = ZERO;
    o->pos_min_hard = ZERO;
    o->pos_max_hard = ZERO;
    
    o->use_hard_limit = FALSE;
    
    o->output_lim = ZERO;
    
    //o->trq_max = ZERO;
    //o->out_max = ZERO;
    
    o->pos_fbk = ZERO;
    o->vel_fbk = ZERO;
    o->acc_fbk = ZERO;
    o->trq_fbk = ZERO;
    
    o->vel_fbk_old = ZERO;
    
    o->pos_fbk_from_motors = ZERO;
    o->vel_fbk_from_motors = ZERO;
    
    o->out_ref = ZERO;
    o->pos_ref = ZERO;
    o->vel_ref = ZERO;
    o->acc_ref = ZERO;
    o->trq_ref = ZERO;
    
    o->pos_err = ZERO;
    o->vel_err = ZERO;
    o->trq_err = ZERO;
    
    o->output = ZERO;
    
    o->tcKoffset = ZERO;
    o->tcKstiff  = ZERO;
    o->tcKdamp   = ZERO;
    
    //o->scKpos   = ZERO;
    //o->scKvel   = ZERO;
    
    WatchDog_init(&o->trq_fbk_wdog);
    WatchDog_init(&o->vel_ref_wdog);
    
    WatchDog_set_base_time_msec(&o->trq_fbk_wdog, 2*DEFAULT_WATCHDOG_TIME_MSEC);
    WatchDog_set_base_time_msec(&o->vel_ref_wdog,   DEFAULT_WATCHDOG_TIME_MSEC);
    
    WatchDog_rearm(&o->trq_fbk_wdog);
    WatchDog_rearm(&o->vel_ref_wdog);
    
    Trajectory_init(&o->trajectory, 0, 0, 0);
    
    PID_init(&o->minjerkPID);
    PID_init(&o->directPID);
    
    o->control_mode = eomc_controlmode_notConfigured;
    o->interaction_mode = eOmc_interactionmode_stiff;
    
    o->pos_control_active = FALSE;
    o->trq_control_active = FALSE;
    
    o->pushing_limit = FALSE;
    
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
    o->diagnostics_refresh = 0;
    
    o->eo_joint_ptr = NULL;
    
    o->not_reversible = FALSE;
    
    Joint_reset_calibration_data(o);
}

void Joint_reset_calibration_data(Joint* o)
{
    memset(&o->running_calibration, 0, sizeof(jointCalibrationData));
    
    o->running_calibration.type = eomc_calibration_typeUndefined;
}

void Joint_config(Joint* o, uint8_t ID, eOmc_joint_config_t* config)
{
    o->ID = ID;
        
    o->pos_min_soft = config->userlimits.min;
    o->pos_max_soft = config->userlimits.max;    
    o->pos_min_hard = config->hardwarelimits.min;
    o->pos_max_hard = config->hardwarelimits.max;
    
    o->pos_min = config->userlimits.min;
    o->pos_max = config->userlimits.max;

    o->vel_max = config->maxvelocityofjoint;
    o->acc_max = 10000000.0f;
    
    o->tcKstiff  = (CTRL_UNITS)(config->impedance.stiffness);
    o->tcKdamp   = (CTRL_UNITS)(config->impedance.damping);
    o->tcKoffset = config->impedance.offset;
    
    o->Kadmitt = (o->tcKstiff == ZERO) ? ZERO : (1.0f/o->tcKstiff); 
    
    Trajectory_config_limits(&o->trajectory, o->pos_min, o->pos_max, o->vel_max, o->acc_max);
    
    WatchDog_set_base_time_msec(&o->vel_ref_wdog, config->velocitysetpointtimeout);
    WatchDog_rearm(&o->vel_ref_wdog);
    
    // TODOALE joint admittance missing
    o->Kadmitt = ZERO;
    
    o->dead_zone = config->deadzone;
    
//    eOerrmanDescriptor_t errdes = {0};
//    char message[150];
//    
//    snprintf(message, sizeof(message), "CFG HW LIMITS: max=%.2f, min=%.2f",o->pos_min_hard, o->pos_max_hard);

//    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
//    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//    errdes.sourceaddress    = o->ID;
//    errdes.par16            = 0;
//    errdes.par64            = 0;
//    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, message, NULL, &errdes);
}

void Joint_destroy(Joint* o)
{
    DELETE(o);
}

void Joint_motion_reset(Joint *o)
{
    PID_reset(&o->minjerkPID);
    PID_reset(&o->directPID);
    
    Trajectory_stop(&o->trajectory, o->pos_fbk);
        
    //Watchdog_disarm(&o->vel_ref_wdog);
    
    o->pos_ref = o->pos_fbk;
    o->vel_ref = ZERO;
    o->acc_ref = ZERO;
    o->out_ref = ZERO;
    o->trq_ref = o->trq_fbk;
       
    o->pos_err = ZERO;
    o->trq_err = ZERO;
    o->vel_err = ZERO;
    
    o->output = ZERO;
}
void Joint_update_status_reference(Joint* o)
{    
    switch (o->control_mode)
    {
        case eomc_controlmode_idle:
        case eomc_controlmode_notConfigured:
        case eomc_controlmode_configured:
            break;
        
        case eomc_controlmode_mixed:
        case eomc_ctrlmval_velocity_pos:
            o->eo_joint_ptr->status.target.trgt_velocity = Trajectory_get_target_velocity(&(o->trajectory));
            o->eo_joint_ptr->status.target.trgt_position = Trajectory_get_target_position(&(o->trajectory));
            break;
        case eomc_controlmode_velocity: //
        case eomc_controlmode_vel_direct:
        case eomc_controlmode_impedance_vel:
            o->eo_joint_ptr->status.target.trgt_velocity = Trajectory_get_target_velocity(&(o->trajectory));
            break;
        case eomc_controlmode_position:
        case eomc_controlmode_impedance_pos:
            o->eo_joint_ptr->status.target.trgt_position = Trajectory_get_target_position(&(o->trajectory));
            break;
        case eomc_controlmode_direct:
            o->eo_joint_ptr->status.target.trgt_positionraw = Trajectory_get_target_position(&(o->trajectory));
            break;
                
        case eomc_controlmode_openloop:
            o->eo_joint_ptr->status.target.trgt_pwm = o->out_ref;
            break;

        case eomc_controlmode_current:
            o->eo_joint_ptr->status.target.trgt_current = o->out_ref;
            break;
        
        case eomc_controlmode_torque:
            o->eo_joint_ptr->status.target.trgt_torque = o->trq_ref;
            break;
            
        default:
            ;
    }

}

static void Joint_send_debug_message(char *message, uint8_t jid, uint16_t par16, uint64_t par64)
{

    eOerrmanDescriptor_t errdes = {0};

    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = jid;
    errdes.par16            = par16;
    errdes.par64            = par64;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, message, NULL, &errdes);

}

BOOL Joint_set_control_mode(Joint* o, eOmc_controlmode_command_t control_mode)
{
    if (o->control_mode == ((eOmc_controlmode_t)control_mode)) return TRUE;
    
    if (o->control_mode == eomc_controlmode_notConfigured) return FALSE;
        
    if (o->control_mode == eomc_controlmode_calib) return FALSE;    
    
    if (o->control_mode == eomc_controlmode_hwFault)
    {
        if (control_mode == eomc_controlmode_cmd_force_idle)
        {
            Joint_clear_faults(o);
        }
        else
        {
            return FALSE;
        }
    }
    
    switch (control_mode)
    {
        case eomc_controlmode_cmd_force_idle:
            control_mode = eomc_controlmode_cmd_idle;
            break;
        case eomc_controlmode_cmd_velocity_pos:
            control_mode = eomc_controlmode_cmd_mixed;
            break;
        case eomc_controlmode_cmd_velocity:
        //    control_mode = eomc_controlmode_cmd_vel_direct;
        //    break;
        case eomc_controlmode_cmd_vel_direct:
        case eomc_controlmode_cmd_idle:
        case eomc_controlmode_cmd_mixed:
        case eomc_controlmode_cmd_position:
        case eomc_controlmode_cmd_direct:
        case eomc_controlmode_cmd_openloop:
        case eomc_controlmode_cmd_current:
        case eomc_controlmode_cmd_torque:
            break;
            
        default:
            return FALSE;
    }
    
    o->control_mode = (eOmc_controlmode_t)control_mode;
    
    Joint_send_debug_message("SET CONTROLMODE", o->ID, control_mode, o->control_mode);
    
    Joint_set_inner_control_flags(o);
    
    Joint_motion_reset(o);
    
    Joint_update_status_reference(o);
    
    return TRUE;
}

BOOL Joint_set_interaction_mode(Joint* o, eOmc_interactionmode_t interaction_mode)
{
    if (o->interaction_mode == interaction_mode) return TRUE;
    
    o->interaction_mode = interaction_mode;
    
    Joint_set_inner_control_flags(o);
    
    if (interaction_mode == eOmc_interactionmode_stiff)
    {
        Joint_motion_reset(o);
    }
    
    return TRUE;
}

void Joint_update_odometry_fbk(Joint* o, CTRL_UNITS pos_fbk, CTRL_UNITS vel_fbk)
{
    o->pos_fbk = pos_fbk;
    o->vel_fbk = vel_fbk;
}

void Joint_update_torque_fbk(Joint* o, CTRL_UNITS trq_fbk)
{
    o->trq_fbk = trq_fbk;
    
    WatchDog_rearm(&o->trq_fbk_wdog);
}

BOOL Joint_check_faults(Joint* o)
{
    if (WatchDog_check_expired(&o->trq_fbk_wdog))
    {
        o->trq_fbk = o->trq_ref;
        o->trq_err = ZERO;
        
        if (o->trq_control_active)
        {
            o->fault_state.bits.torque_sensor_timeout = TRUE;
        }
    }
    
    if ((o->control_mode != eomc_controlmode_notConfigured) && (o->control_mode != eomc_controlmode_calib))
    {
        if ((o->pos_min != o->pos_max) && ((o->pos_fbk < o->pos_min_hard - POS_LIMIT_MARGIN) || (o->pos_fbk > o->pos_max_hard + POS_LIMIT_MARGIN))) 
        {
            o->fault_state.bits.hard_limit_reached = TRUE;
            
            o->control_mode = eomc_controlmode_hwFault;
        }
    }
    
    if (o->control_mode != eomc_controlmode_hwFault) return FALSE;

    if (++o->diagnostics_refresh > 5*CTRL_LOOP_FREQUENCY_INT)
    {
        o->diagnostics_refresh = 0;
        o->fault_state.bitmask = 0;
    }
        
    if (o->fault_state.bitmask != o->fault_state_prec.bitmask)
    {        
        if (o->fault_state.bits.torque_sensor_timeout && !o->fault_state_prec.bits.torque_sensor_timeout)
        {   
            static eOerrmanDescriptor_t descriptor = {0};
            descriptor.par16 = o->ID;
            descriptor.par64 = 0;
            descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
            descriptor.sourceaddress = 0;
            descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_axis_torque_sens);
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        }
        
        if (o->fault_state.bits.hard_limit_reached && !o->fault_state_prec.bits.hard_limit_reached)
        {   
            static eOerrmanDescriptor_t descriptor = {0};
            descriptor.par16 = o->ID;
            descriptor.par64 = 0;
            descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
            descriptor.sourceaddress = 0;
            descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_joint_hard_limit);
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        }
        
        o->fault_state_prec.bitmask = o->fault_state.bitmask;
    }
    
    return TRUE;
}

extern void Joint_clear_faults(Joint* o)
{
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
}

int8_t Joint_check_limits(Joint* o)
{
    if (o->pos_min != o->pos_max)
    {
        if (o->pos_fbk <= o->pos_min) return -1;
        if (o->pos_fbk >= o->pos_max) return  1;
    }
    
    return 0;
}

int8_t Joint_pushing_limit(Joint* o)
{
    return o->pushing_limit;
}

void Joint_set_limits(Joint* o, CTRL_UNITS pos_min, CTRL_UNITS pos_max)
{
    o->pos_min = pos_min;
    o->pos_max = pos_max;
    
    Trajectory_config_limits(&o->trajectory, pos_min, pos_max, 0.0f, 0.0f);
}

void Joint_set_hardware_limit(Joint* o)
{
    o->use_hard_limit = TRUE;
    
    Joint_set_limits(o, o->pos_min_hard, o->pos_max_hard);
}

BOOL Joint_manage_cable_constraint(Joint* o)
{    
    //BOOL opening_intention = (o->pos_err < ZERO);
    
    if (o->pos_err < ZERO)
    {
        if (o->pos_fbk_from_motors < o->cable_constr.motor_pos_min) return TRUE;
    }
    else
    {
        if (o->pos_fbk_from_motors > o->cable_constr.motor_pos_max) return TRUE;
        
        if (o->trq_fbk > o->cable_constr.max_tension) return TRUE;
    }
    
    return FALSE;
}

BOOL Joint_manage_R1_finger_tension_constraint(Joint* o)
{
    static BOOL loose_cable[4]={FALSE,FALSE,FALSE,FALSE};

    //if switch_val< 1000 than hard stop reached, if switch_val> 4000 that hard stop is not reached, 
    //...in between we could use the last value....(hysteresis)
    
    uint32_t switch_val = hal_adc_get_hall_sensor_analog_input_mV(1 - o->ID);
            
    if (switch_val < 1500) 
    {
        loose_cable[o->ID] = TRUE;
    }
    else if (switch_val > 3500)
    {        
        loose_cable[o->ID] = FALSE;
    } 
    
    // open intention    
    
//    static int noflood[] = {0, 250};
//    
//    if (++noflood[o->ID]> 500)
//    {
//        noflood[o->ID] = 0;
//        
//        eOerrmanDescriptor_t errdes = {0};

//        errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = o->ID;
//        errdes.par16            = switch_val;
//        errdes.par64            = 0;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, "FINECORSA", NULL, &errdes);
//    }
    
    //return FALSE;
    return ((o->pos_err < ZERO) && loose_cable[o->ID]);
}

CTRL_UNITS Joint_do_pwm_or_current_control(Joint* o)
{    
    PID *pid = (o->control_mode == eomc_controlmode_direct || o->control_mode == eomc_controlmode_vel_direct) ?  &o->directPID : &o->minjerkPID; 
    
    o->pushing_limit = FALSE;
    
    switch (o->control_mode)
    {
        case eomc_controlmode_openloop:
        case eomc_controlmode_current:
        {
            o->pos_err = o->pos_ref = ZERO;
            o->vel_err = o->vel_ref = ZERO;
            o->trq_err = o->trq_ref = ZERO;
        
            o->output = o->out_ref;
            break;
        }
        case eomc_controlmode_torque:
        {
            o->pos_err = o->pos_ref = ZERO;
            o->vel_err = o->vel_ref = ZERO;
            o->trq_err = o->trq_ref - o->trq_fbk;
        
            o->output = o->trq_ref;
        
            if (o->pos_min != o->pos_max)
            {
                if (o->pos_fbk <= o->pos_min) 
                {
                    o->output_lim = PID_do_out(pid, o->pos_min-o->pos_fbk);
                    o->pushing_limit = -1;
                }
                else if (o->pos_fbk >= o->pos_max)
                {
                    o->output_lim = PID_do_out(pid, o->pos_max-o->pos_fbk);
                    o->pushing_limit =  1;
                }
                else
                {
                    o->output_lim = ZERO;
                }
            }
            else
            {
                o->output_lim = ZERO;
            }
        
            break;
        }
        case eomc_controlmode_vel_direct:
        case eomc_controlmode_velocity:
        case eomc_controlmode_mixed:
        {
            if (WatchDog_check_expired(&o->vel_ref_wdog))
            {
                Trajectory_velocity_stop(&o->trajectory);
            }
        }
        case eomc_controlmode_direct:
        case eomc_controlmode_position:
        case eomc_controlmode_calib:
        {    
            if ((o->running_calibration.type == eomc_calibration_type7_hall_sensor) || 
               ((o->running_calibration.type == eomc_calibration_type6_mais) && 
                (o->running_calibration.data.type6.is_active == FALSE)))
            {
                o->output = ZERO;
                break;
            }
            
            Trajectory_do_step(&o->trajectory, &o->pos_ref, &o->vel_ref, &o->acc_ref);
            
            //static int noflood = 0;
            //if (++noflood > 500)
            //{
            //    noflood = 0;
            //
            //    Joint_send_debug_message("PWM or CURRENT", o->ID, o->control_mode, (((uint64_t) o->vel_ref) << 32) | ((uint64_t) o->pos_ref));
            //}
        
            //CTRL_UNITS pos_err_old = o->pos_err;
        
            o->pos_err = o->pos_ref - o->pos_fbk;
            o->vel_err = o->vel_ref - o->vel_fbk;
        
            if (o->interaction_mode == eOmc_interactionmode_stiff)
            {
                o->trq_err = o->trq_ref = ZERO;
                
                if (o->use_hard_limit)
                {
                    if (o->control_mode != eomc_controlmode_calib)
                    {
                        if (o->pos_fbk > o->pos_min_soft && o->pos_fbk < o->pos_max_soft)
                        {
                            o->use_hard_limit = FALSE;
                            
                            Joint_set_limits(o, o->pos_min_soft, o->pos_max_soft);
                        }
                    }
                }
                
                if ((o->pos_min != o->pos_max) && ((o->pos_fbk < o->pos_min_hard - POS_LIMIT_MARGIN) || (o->pos_fbk > o->pos_max_hard + POS_LIMIT_MARGIN))) 
                {
                    o->output = ZERO;
                }
                else
                {
                    if (o->pos_err > o->dead_zone)
                    {
                        o->pos_err -= o->dead_zone;
                        
                        o->output = PID_do_out(pid, o->pos_err);
                    }
                    else if (o->pos_err < -o->dead_zone)
                    {
                        o->pos_err += o->dead_zone;
                        
                        o->output = PID_do_out(pid, o->pos_err);
                    }
                    else
                    {
                        o->pos_err = ZERO;
                        
                        if (o->not_reversible)
                        {
                            PID_reset(pid);
                        
                            o->output = 0;
                        }
                        else
                        {
                            o->output = PID_do_out(pid, o->pos_err);
                        }
                    }
                }
            }
            else
            {
                o->trq_ref = o->tcKoffset + o->tcKstiff*o->pos_err - o->tcKdamp*o->vel_fbk;
                o->trq_err = o->trq_ref - o->trq_fbk;
                
                o->output = o->trq_ref;
                
                if (o->pos_min != o->pos_max)
                {
                    if (o->pos_fbk <= o->pos_min) 
                    {
                        o->output_lim = PID_do_out(pid, o->pos_min-o->pos_fbk);
                        o->pushing_limit = -1;
                    }
                    else if (o->pos_fbk >= o->pos_max)
                    {
                        o->output_lim = PID_do_out(pid, o->pos_max-o->pos_fbk);
                        o->pushing_limit =  1;
                    }
                    else
                    {
                        o->output_lim = ZERO;
                    }
                }
                else
                {
                    o->output_lim = ZERO;
                }
            }
            
            break;
        }    
        default:
        {
            o->pos_err = o->pos_ref = ZERO;
            o->vel_err = o->vel_ref = ZERO;
            o->trq_err = o->trq_ref = ZERO;
            
            o->output = ZERO;
            break;
        }
    }
    
    return o->output;
}

CTRL_UNITS Joint_do_vel_control(Joint* o)
{            
    PID *pid = (o->control_mode == eomc_controlmode_direct || o->control_mode == eomc_controlmode_vel_direct) ?  &o->directPID : &o->minjerkPID; 
    
    o->pushing_limit = FALSE;
    
    if (o->control_mode == eomc_controlmode_torque)
    {
        o->pos_err = o->pos_ref = ZERO;
        o->vel_err = o->vel_ref = ZERO;
        o->trq_err = o->trq_ref - o->trq_fbk;
        
        if (o->pos_min != o->pos_max)
        {
            if (o->pos_fbk <= o->pos_min) 
            {
                o->pushing_limit = -1;
                
                o->vel_ref = 0.1f*pid->Kp*(o->pos_min - o->pos_fbk);                
            }
            else if (o->pos_fbk >= o->pos_max)
            {
                o->pushing_limit =  1;
                
                o->vel_ref = 0.1f*pid->Kp*(o->pos_max - o->pos_fbk);
            }
            else
            {
                o->vel_ref = -o->Kadmitt * o->trq_err;
            }
        }
        else
        {
            o->vel_ref = -o->Kadmitt * o->trq_err;
        }
            
        LIMIT(o->vel_ref, o->vel_max);
            
        return o->output = o->vel_ref;
    }
    
    //////////////////////////////////
    
    switch (o->control_mode)
    {
        case eomc_controlmode_mixed:
        case eomc_ctrlmval_velocity_pos:
        case eomc_controlmode_velocity: //
        case eomc_controlmode_vel_direct:
            if (WatchDog_check_expired(&o->vel_ref_wdog))
            {
                Trajectory_velocity_stop(&o->trajectory);
            }
        case eomc_controlmode_direct:
        case eomc_controlmode_position:
        {
            Trajectory_do_step(&o->trajectory, &o->pos_ref, &o->vel_ref, &o->acc_ref);
        
            o->pos_err = o->pos_ref - o->pos_fbk;
            o->vel_err = o->vel_ref - o->vel_fbk;        

            break;
        }
        
        default:
            o->pos_err = o->pos_ref = ZERO;
            o->vel_err = o->vel_ref = ZERO;
            o->trq_err = o->trq_ref = ZERO;
            
            o->output = ZERO;
            
            return ZERO;
    }
    
    if (o->interaction_mode == eOmc_interactionmode_stiff)
    {
        o->trq_err = o->trq_ref = ZERO;
     
        switch (o->control_mode)
        {
            case eomc_controlmode_direct:
                o->vel_ref = pid->Kp * o->pos_err;
                break;
            
            case eomc_controlmode_vel_direct:
                o->pos_err = ZERO;
                o->vel_ref = pid->Kff * o->vel_ref;
                break;
            
            case eomc_controlmode_mixed:
            case eomc_controlmode_position:
            case eomc_controlmode_velocity:
                if (o->vel_ref == ZERO)
                {
                    if (o->pos_err > o->dead_zone)
                    {
                        o->pos_err -= o->dead_zone;
                    }
                    else if (o->pos_err < -o->dead_zone)
                    {
                        o->pos_err += o->dead_zone;
                    }
                    else
                    {
                        o->pos_err = ZERO;
                    }
                    
                    o->vel_ref = pid->Kp*o->pos_err;
                }
                else
                {
                    o->vel_ref = pid->Kff*o->vel_ref + pid->Kp*o->pos_err;
                }
                break;
            
            default:
                break;
        }

        LIMIT(o->vel_ref, o->vel_max);
                
        return o->output = o->vel_ref;
    }
    else // COMPLIANT
    {
        if (o->pos_min != o->pos_max)
        {
            if (o->pos_fbk <= o->pos_min) 
            {
                o->pushing_limit = -1;
                o->vel_ref = 0.1f*pid->Kp * (o->pos_min - o->pos_fbk);
            }
            else if (o->pos_fbk >= o->pos_max)
            {
                o->pushing_limit =  1;
                o->vel_ref = 0.1f*pid->Kp * (o->pos_max - o->pos_fbk);
            }
            else
            {
                o->vel_ref += pid->Kp * (o->pos_err + o->Kadmitt * o->trq_fbk);
            }
        }
        else
        {
            o->vel_ref += pid->Kp * (o->pos_err + o->Kadmitt * o->trq_fbk);
        }
                
        LIMIT(o->vel_ref, o->vel_max);
                
        return o->output = o->vel_ref;
    }
}

void Joint_set_impedance(Joint* o, eOmc_impedance_t* impedance)
{
    o->tcKstiff  = (CTRL_UNITS)(impedance->stiffness);
    o->tcKdamp   = (CTRL_UNITS)(impedance->damping);
    o->tcKoffset = impedance->offset;
    
    if (o->tcKstiff != 0.0f)
    {
        o->Kadmitt = 1.0f/o->tcKstiff; 
    }
    else
    {
        o->Kadmitt = 0.0f;
    }
}

void Joint_get_impedance(Joint* o, eOmc_impedance_t* impedance)
{
    impedance->stiffness = (eOmeas_stiffness_t)(o->tcKstiff);
    impedance->damping   = (eOmeas_damping_t)(o->tcKdamp);
    impedance->offset    = (eOmeas_torque_t)(o->tcKoffset);
}

void Joint_get_state(Joint* o, eOmc_joint_status_t* joint_state)
{
    joint_state->core.modes.interactionmodestatus    = o->interaction_mode;
    joint_state->core.modes.controlmodestatus        = o->control_mode;
    joint_state->core.modes.ismotiondone             = Trajectory_is_done(&o->trajectory);
    joint_state->core.measures.meas_position         = o->pos_fbk;           
    joint_state->core.measures.meas_velocity         = o->vel_fbk;        
    joint_state->core.measures.meas_acceleration     = o->acc_fbk;      
    joint_state->core.measures.meas_torque           = o->trq_fbk;
}

BOOL Joint_get_pid_state(Joint* o, eOmc_joint_status_ofpid_t* pid_state)
{
    BOOL trq_active = FALSE;
    
    if (o->pos_control_active)
    {
        pid_state->complpos.refpos = o->pos_ref;
        pid_state->complpos.errpos = o->pos_err;
    }
    else
    {
        pid_state->complpos.refpos = 0;
        pid_state->complpos.errpos = 0;
    }
    
    if (o->trq_control_active)
    {
        trq_active = TRUE;
        
        pid_state->complpos.reftrq = o->trq_ref;
        pid_state->complpos.errtrq = o->trq_err;
    }
    else
    {
        pid_state->complpos.reftrq = 0;
        pid_state->complpos.errtrq = 0;
        
        pid_state->complpos.output = o->output;
    }
    
    return trq_active;
}

static BOOL Joint_set_pos_ref_core(Joint* o, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref)
{
    LIMIT(vel_ref, o->vel_max);
    
    o->pos_ref = pos_ref;
    o->vel_ref = vel_ref;

    if (vel_ref == 0.0f) return TRUE;
    
    Trajectory_set_pos_end(&o->trajectory, pos_ref, vel_ref);
    
    return TRUE;  
}

BOOL Joint_set_pos_ref(Joint* o, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref)
{
    if ((o->control_mode != eomc_controlmode_position) && (o->control_mode != eomc_controlmode_mixed) && (o->control_mode != eomc_ctrlmval_velocity_pos))
    {
        return FALSE;
    }
    
    if (o->pos_min != o->pos_max) LIMIT2(o->pos_min, pos_ref, o->pos_max);
    
    return(Joint_set_pos_ref_core(o, pos_ref, vel_ref));
}

BOOL Joint_set_pos_raw(Joint* o, CTRL_UNITS pos_ref)
{    
    if (o->control_mode != eomc_controlmode_direct)
    {
        return FALSE;
    }
    
    if (o->pos_min != o->pos_max) LIMIT2(o->pos_min, pos_ref, o->pos_max);
    
    o->pos_ref = pos_ref;
    o->vel_ref = ZERO;
    
    Trajectory_set_pos_raw(&o->trajectory, pos_ref);
    
    return TRUE;  
}

BOOL Joint_set_pos_ref_in_calibType6(Joint* o, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref)
{
    CTRL_UNITS pos_ref_limited = pos_ref;
    
    if( (o->control_mode != eomc_controlmode_calib) || (o->running_calibration.type != eomc_calibration_type6_mais) || (o->running_calibration.data.type6.is_active == FALSE) )
    {
        return FALSE;
    }
    
    if(pos_ref > o->pos_max)
    {
        pos_ref_limited = o->pos_max;
    }
    if(pos_ref < o->pos_min)
    {
        pos_ref_limited = o->pos_min;
    }
    
    return(Joint_set_pos_ref_core(o, pos_ref_limited, vel_ref));
}

BOOL Joint_set_vel_ref(Joint* o, CTRL_UNITS vel_ref, CTRL_UNITS acc_ref)
{
    WatchDog_rearm(&o->vel_ref_wdog);
    
    if ((o->control_mode != eomc_controlmode_vel_direct) &&
        (o->control_mode != eomc_controlmode_velocity) && //
        (o->control_mode != eomc_controlmode_velocity_pos) &&    
        (o->control_mode != eomc_controlmode_mixed))
    {
        return FALSE;
    }
    
    LIMIT(vel_ref, o->vel_max);
    LIMIT(acc_ref, o->acc_max);
 
    o->vel_ref = vel_ref;
    o->acc_ref = acc_ref;
    
    if (acc_ref == 0.0f)
    {
        Trajectory_velocity_stop(&o->trajectory);

        return TRUE;
    }
     
    Trajectory_set_vel_end(&o->trajectory, vel_ref, acc_ref);
    
    return TRUE;
}

BOOL Joint_set_vel_raw(Joint* o, CTRL_UNITS vel_ref)
{
    WatchDog_rearm(&o->vel_ref_wdog);
    
    if (o->control_mode != eomc_controlmode_vel_direct)
    {
        return FALSE;
    }
    
    LIMIT(vel_ref, o->vel_max);
 
    o->vel_ref = vel_ref;
     
    Trajectory_set_vel_raw(&o->trajectory, vel_ref);
    
    return TRUE;
}

BOOL Joint_set_trq_ref(Joint* o, CTRL_UNITS trq_ref)
{
    if (o->control_mode != eomc_controlmode_torque)
    {
        return FALSE;
    }
    
    o->trq_ref = trq_ref;
    
    return TRUE;
}

BOOL Joint_set_pwm_ref(Joint* o, CTRL_UNITS pwm_ref)
{
    if (o->control_mode != eomc_controlmode_openloop)
    {
        return FALSE;
    }
    
    o->out_ref = pwm_ref;

    return TRUE;
}

BOOL Joint_set_cur_ref(Joint* o, CTRL_UNITS cur_ref)
{
    if (o->control_mode != eomc_controlmode_current)
    {
        return FALSE;
    }
    
    o->out_ref = cur_ref;

    //Joint_send_debug_message("current reference",o->ID,0,(uint64_t)cur_ref);
    
    return TRUE;
}

void Joint_stop(Joint* o)
{
    Trajectory_stop(&o->trajectory, o->pos_fbk);
}

static void Joint_set_inner_control_flags(Joint* o)
{
    switch (o->control_mode)
    {
        case eomc_controlmode_position:
        case eomc_controlmode_velocity: //
        case eomc_controlmode_vel_direct:
        case eomc_controlmode_mixed:
        case eomc_controlmode_velocity_pos:
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

//VALE: debug function. I'll remove it ASAP
//void Joint_update_debug_current_info(Joint *o, int32_t avgCurrent, int32_t accum_Ep)
//{
//    if you use this function, comments updates meas_acceleration and meas_velocity in 
//      Joint_get_state function
//    o->eo_joint_ptr->status.core.measures.meas_acceleration = avgCurrent;
//    o->eo_joint_ptr->status.core.measures.meas_velocity = accum_Ep;
//}
