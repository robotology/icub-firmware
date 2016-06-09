#include "Joint.h"

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
    o->dead_zone = 0;
    
    o->pos_min = ZERO;
    o->pos_max = ZERO;
    o->vel_max = ZERO;
    o->acc_max = ZERO;
    
    o->output_lim = ZERO;
    
    //o->trq_max = ZERO;
    //o->out_max = ZERO;
    
    o->pos_fbk = ZERO;
    o->vel_fbk = ZERO;
    o->trq_fbk = ZERO;
    
    o->pos_fbk_from_motors = ZERO;
    o->vel_fbk_from_motors = ZERO;
    
    o->out_ref = ZERO;
    o->pos_ref = ZERO;
    o->vel_ref = ZERO;
    o->acc_ref = ZERO;
    o->trq_ref = ZERO;
    
    o->vel_dir_ref = ZERO;
    
    o->pos_err = ZERO;
    o->vel_err = ZERO;
    o->trq_err = ZERO;
    
    o->output = ZERO;
    
    o->tcKoffset = ZERO;
    o->tcKstiff  = ZERO;
    o->tcKdamp   = ZERO;
    
    o->scKpos   = ZERO;
    o->scKvel   = ZERO;
    o->scKstill = ZERO;
    
    WatchDog_init(&o->trq_fbk_wdog);
    WatchDog_init(&o->vel_ref_wdog);
    
    WatchDog_set_base_time_msec(&o->trq_fbk_wdog, 2*DEFAULT_WATCHDOG_TIME_MSEC);
    WatchDog_set_base_time_msec(&o->vel_ref_wdog,   DEFAULT_WATCHDOG_TIME_MSEC);
    
    WatchDog_rearm(&o->trq_fbk_wdog);
    WatchDog_rearm(&o->vel_ref_wdog);
    
    Trajectory_init(&o->trajectory, 0, 0, 0);
    
    PID_init(&o->posPID);
    PID_init(&o->velPID);
    
    o->control_mode = eomc_controlmode_notConfigured;
    o->interaction_mode = eOmc_interactionmode_stiff;
    
    o->pos_control_active = FALSE;
    o->trq_control_active = FALSE;
    
    o->MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
    
    o->CAN_DO_TRQ_CTRL = TRUE;
    
    o->pushing_limit = FALSE;
    
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
    o->diagnostics_refresh = 0;
    
    o->eo_joint_ptr = NULL;
    
    Joint_reset_calibration_data(o);
    
    //SpeedController_init(o->speedController);
}

void Joint_reset_calibration_data(Joint* o)
{
    memset(&o->running_calibration, 0, sizeof(jointCalibrationData));
    o->running_calibration.type = eomc_calibration_typeUndefined;

}

void Joint_config(Joint* o, uint8_t ID, eOmc_joint_config_t* config)
{
    o->ID = ID;
    
    PID_config(&o->posPID, &config->pidposition);
    PID_config(&o->velPID, &config->pidvelocity);
    
    //o->scKvel   = config->pidvelocity.kp;
    //o->scKpos   = config->pidvelocity.ki;
    //o->scKstill = config->pidposition.ki;
    
    o->scKpos   = config->pidposition.kp;
    o->scKvel   = config->pidposition.kd;
    o->scKstill = config->pidposition.ki;
    
    o->pos_min = config->limitsofjoint.min;
    o->pos_max = config->limitsofjoint.max;
    o->vel_max = config->maxvelocityofjoint;
    o->acc_max = 65535;
    
    o->tcKstiff  = 0.001f*(CTRL_UNITS)(config->impedance.stiffness);
    o->tcKdamp   = 0.001f*CTRL_LOOP_FREQUENCY*(CTRL_UNITS)(config->impedance.damping);
    o->tcKoffset = config->impedance.offset;
    
    o->Kadmitt = (o->tcKstiff == ZERO) ? ZERO: (1.0f/o->tcKstiff); 
    
    Trajectory_config_limits(&o->trajectory, o->pos_min, o->pos_max, o->vel_max, o->acc_max);
    
    WatchDog_set_base_time_msec(&o->vel_ref_wdog, config->velocitysetpointtimeout);
    WatchDog_rearm(&o->vel_ref_wdog);
    
    // TODOALE joint admittance missing
    o->Kadmitt = ZERO;
}

void Joint_destroy(Joint* o)
{
    DELETE(o);
}

void Joint_motion_reset(Joint *o)
{
    PID_reset(&o->posPID);
    PID_reset(&o->velPID);
    
    Trajectory_stop(&o->trajectory, o->pos_fbk);
        
    //Watchdog_disarm(&o->vel_ref_wdog);
    
    o->pos_ref = o->pos_fbk;
    o->vel_ref = ZERO;
    o->acc_ref = ZERO;
    o->out_ref = ZERO;
    o->trq_ref = o->trq_fbk;
       
    o->vel_dir_ref = ZERO;
    
    o->pos_err = ZERO;
    o->trq_err = ZERO;
    o->vel_err = ZERO;
    
    o->output = ZERO;
}
static void Joint_update_status_reference(Joint* o, eOmc_controlmode_command_t control_mode)
{
    switch (control_mode)
    {
        case eomc_controlmode_cmd_force_idle:
        case eomc_controlmode_cmd_idle:
            break;
        case eomc_controlmode_cmd_mixed:
            o->eo_joint_ptr->status.target.trgt_velocity = o->vel_ref;
            o->eo_joint_ptr->status.target.trgt_position = o->pos_ref;
            break;
        case eomc_controlmode_cmd_velocity:
            o->eo_joint_ptr->status.target.trgt_velocity = o->vel_ref;
            break;
        case eomc_controlmode_cmd_position:
            o->eo_joint_ptr->status.target.trgt_position = o->pos_ref;
            break;
        case eomc_controlmode_cmd_direct:
            o->eo_joint_ptr->status.target.trgt_positionraw = o->pos_ref;
            break;
                
        case eomc_controlmode_cmd_openloop:
            o->eo_joint_ptr->status.target.trgt_openloop = o->out_ref;
            break;

        case eomc_controlmode_cmd_torque:
            o->eo_joint_ptr->status.target.trgt_torque = o->trq_ref;
            break;
            
        default:
            ;
    }

}
BOOL Joint_set_control_mode(Joint* o, eOmc_controlmode_command_t control_mode)
{
    if (o->control_mode == control_mode) return TRUE;
    
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
        case eomc_controlmode_cmd_idle:
        case eomc_controlmode_cmd_mixed:
        case eomc_controlmode_cmd_velocity:
        case eomc_controlmode_cmd_position:
        case eomc_controlmode_cmd_direct:
            break;
                
        case eomc_controlmode_cmd_openloop:
            if (o->MOTOR_CONTROL_TYPE != PWM_CONTROLLED_MOTOR) return FALSE;
            break;

        case eomc_controlmode_cmd_torque:
            if (!o->CAN_DO_TRQ_CTRL) return FALSE;
            break;
            
        default:
            return FALSE;
    }
    
    o->control_mode = (eOmc_controlmode_t)control_mode;
    
    Joint_set_inner_control_flags(o);
    
    Joint_motion_reset(o);
    
    Joint_update_status_reference(o, control_mode);
    
    return TRUE;
}

BOOL Joint_set_interaction_mode(Joint* o, eOmc_interactionmode_t interaction_mode)
{
    if (o->interaction_mode == interaction_mode) return TRUE;
    
    if (!o->CAN_DO_TRQ_CTRL)
    {
        if (interaction_mode == eOmc_interactionmode_compliant)
        {
            return FALSE;
        }
    }
    
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
    // VALE: commented see issue 66 icub-firmware https://github.com/robotology/icub-firmware/issues/66
    //if ((trq_fbk == o->trq_fbk) && (trq_fbk == 0.0f)) return;
    
    o->trq_fbk = trq_fbk;
    
    WatchDog_rearm(&o->trq_fbk_wdog);
}

BOOL Joint_check_faults(Joint* o)
{
    if (WatchDog_check_expired(&o->trq_fbk_wdog))
    {
        o->trq_fbk = ZERO;
        o->trq_ref = ZERO;
        o->trq_err = ZERO;
        
        if (o->trq_control_active)
        {
            o->fault_state.bits.torque_sensor_timeout = TRUE;
            
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

BOOL Joint_manage_cable_constraint(Joint* o)
{    
    BOOL opening_intention = (o->pos_err < ZERO);
    
    BOOL opening_action = (o->pos_fbk < o->cable_constr.last_joint_pos);
    
    o->cable_constr.last_joint_pos = o->pos_fbk;
    
    //if (opening_intention && !opening_action) return TRUE;
    
    if (opening_intention)
    {
        if (o->pos_fbk_from_motors < o->cable_constr.motor_pos_min) return TRUE;
    }
    else
    {
        if (o->pos_fbk_from_motors > o->cable_constr.motor_pos_max) return TRUE;
    }
    
    return FALSE;
}

CTRL_UNITS Joint_do_pwm_control(Joint* o)
{
    o->pushing_limit = FALSE;
    
    switch (o->control_mode)
    {
        case eomc_controlmode_openloop:
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
                    o->output_lim = PID_do_out(&o->posPID, o->pos_min-o->pos_fbk);
                    o->pushing_limit = -1;
                }
                else if (o->pos_fbk >= o->pos_max)
                {
                    o->output_lim = PID_do_out(&o->posPID, o->pos_max-o->pos_fbk);
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
        case eomc_controlmode_velocity:
        case eomc_controlmode_mixed:
        {
            if (WatchDog_check_expired(&o->vel_ref_wdog))
            {
                Trajectory_velocity_stop(&o->trajectory);
            }
        }
        case eomc_controlmode_position:
        case eomc_controlmode_direct:
        case eomc_controlmode_calib:
        {    
//            if((eomc_controlmode_calib == o->control_mode) && (!o->calib_type6_data.is_active))
//            {
//                o->output = ZERO;
//                break;
//            }
            
            if( (o->running_calibration.type == eomc_calibration_type7_hall_sensor) || 
                ((o->running_calibration.type == eomc_calibration_type6_mais) && (o->running_calibration.data.type6.is_active == FALSE)) )
            {
                o->output = ZERO;
                break;
            }
            Trajectory_do_step(&o->trajectory, &o->pos_ref, &o->vel_ref, &o->acc_ref);
        
            CTRL_UNITS pos_err_old = o->pos_err;
        
            o->pos_err = o->pos_ref - o->pos_fbk;
            o->vel_err = o->vel_ref - o->vel_fbk;
        
            if (o->interaction_mode == eOmc_interactionmode_stiff)
            {
                o->trq_err = o->trq_ref = ZERO;
                
                if ((o->pos_min != o->pos_max) && ((o->pos_fbk < o->pos_min - POS_LIMIT_MARGIN) || (o->pos_fbk > o->pos_max + POS_LIMIT_MARGIN))) 
                {
                    o->output = ZERO;
                }
                else
                {
                    if (abs((int)o->pos_err)>o->dead_zone)
                    {
                        o->output = PID_do_out(&o->posPID, o->pos_err);
                    }
                    else
                    {
                        PID_reset(&o->posPID);
                        
                        o->output = 0;
                    }
                }
            }
            else
            {
                o->trq_ref = o->tcKoffset + o->tcKstiff*o->pos_err + o->tcKdamp*(o->pos_err - pos_err_old);
                o->trq_err = o->trq_ref - o->trq_fbk;
                
                o->output = o->trq_ref;
                
                if (o->pos_min != o->pos_max)
                {
                    if (o->pos_fbk <= o->pos_min) 
                    {
                        o->output_lim = PID_do_out(&o->posPID, o->pos_min-o->pos_fbk);
                        o->pushing_limit = -1;
                    }
                    else if (o->pos_fbk >= o->pos_max)
                    {
                        o->output_lim = PID_do_out(&o->posPID, o->pos_max-o->pos_fbk);
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
    o->pushing_limit = FALSE;
    
    switch (o->control_mode)
    { 
        case eomc_controlmode_torque:
        {
            o->pos_err = o->pos_ref = ZERO;
            o->vel_err = o->vel_ref = ZERO;
            o->trq_err = o->trq_ref - o->trq_fbk;
        
            if (o->pos_min != o->pos_max)
            {
                if (o->pos_fbk <= o->pos_min) 
                {
                    o->pushing_limit = -1;
                    o->vel_ref = o->scKstill * (o->pos_min - o->pos_fbk);        
                }
                else if (o->pos_fbk >= o->pos_max)
                {
                    o->pushing_limit =  1;
                    o->vel_ref = o->scKstill * (o->pos_max - o->pos_fbk);
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
            
            o->output = o->vel_ref;

            break;
        }
        case eomc_controlmode_velocity:
        case eomc_controlmode_mixed:
        {            
            if (WatchDog_check_expired(&o->vel_ref_wdog))
            {
                Trajectory_velocity_stop(&o->trajectory);
            }
        }
        case eomc_controlmode_position:
        case eomc_controlmode_direct:
        {    
            Trajectory_do_step(&o->trajectory, &o->pos_ref, &o->vel_ref, &o->acc_ref);
            
            //CTRL_UNITS pos_err_old = o->pos_err;
        
            o->pos_err = o->pos_ref - o->pos_fbk;
            o->vel_err = o->vel_ref - o->vel_fbk;
        
            if (o->interaction_mode == eOmc_interactionmode_stiff)
            {
                o->trq_err = o->trq_ref = ZERO;
                
                if (o->control_mode == eomc_controlmode_direct)
                {
                    o->vel_ref = o->scKpos*o->pos_err + o->vel_dir_ref;
                }
                else
                {
                    if (Trajectory_is_done(&o->trajectory))
                    {
                        o->vel_ref = o->scKstill*o->pos_err;
                    }
                    else
                    {
                        //o->vel_ref += o->scKvel*o->vel_err + o->scKpos*o->pos_err;
                        o->vel_ref  = o->scKvel*o->vel_ref + o->scKpos*o->pos_err;
                    }
                }
                
                LIMIT(o->vel_ref, o->vel_max);
                
                o->output = o->vel_ref;
            }
            else
            {
                if (o->pos_min != o->pos_max)
                {
                    if (o->pos_fbk <= o->pos_min) 
                    {
                        o->pushing_limit = -1;
                        o->vel_ref = o->scKstill * (o->pos_min - o->pos_fbk);
                    }
                    else if (o->pos_fbk >= o->pos_max)
                    {
                        o->pushing_limit =  1;
                        o->vel_ref = o->scKstill * (o->pos_max - o->pos_fbk);
                    }
                    else
                    {
                        o->vel_ref += o->scKpos * (o->pos_err + o->Kadmitt * o->trq_fbk);
                    }
                }
                else
                {
                    o->vel_ref += o->scKpos * (o->pos_err + o->Kadmitt * o->trq_fbk);
                }
                
                LIMIT(o->vel_ref, o->vel_max);
                
                o->output = o->vel_ref;
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

void Joint_set_impedance(Joint* o, eOmc_impedance_t* impedance)
{
    o->tcKstiff  = 0.001f*(CTRL_UNITS)(impedance->stiffness);
    o->tcKdamp   = 0.001f*CTRL_LOOP_FREQUENCY*(CTRL_UNITS)(impedance->damping);
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
    impedance->stiffness = (eOmeas_stiffness_t)(1000.0f*o->tcKstiff);
    impedance->damping   = (eOmeas_damping_t)(1000.0f*CTRL_LOOP_PERIOD*o->tcKdamp);
    impedance->offset    = (eOmeas_torque_t)(o->tcKoffset);
}

void Joint_get_state(Joint* o, eOmc_joint_status_t* joint_state)
{
    joint_state->core.modes.interactionmodestatus    = o->interaction_mode;
    joint_state->core.modes.controlmodestatus        = o->control_mode;
    joint_state->core.modes.ismotiondone             = Trajectory_is_done(&o->trajectory);
    joint_state->core.measures.meas_position         = o->pos_fbk;           
    joint_state->core.measures.meas_velocity         = o->vel_fbk;        
    joint_state->core.measures.meas_acceleration     = 0; // not implemented      
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
    if ((o->control_mode != eomc_controlmode_position) && (o->control_mode != eomc_controlmode_mixed))
    {
        return FALSE;
    }
    
    if (o->pos_min != o->pos_max) LIMIT2(o->pos_min, pos_ref, o->pos_max);
    
    return(Joint_set_pos_ref_core(o, pos_ref, vel_ref));
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
    if ((o->control_mode != eomc_controlmode_velocity) && (o->control_mode != eomc_controlmode_mixed))
    {
        return FALSE;
    }
    
    LIMIT(vel_ref, o->vel_max);
    LIMIT(acc_ref, o->acc_max);
    
    if (acc_ref == 0.0f)
    {
        Trajectory_velocity_stop(&o->trajectory);
        
        return TRUE;
    }
    
    Trajectory_set_vel_end(&o->trajectory, vel_ref, acc_ref);
    
    return TRUE;
}

BOOL Joint_set_pos_raw(Joint* o, CTRL_UNITS pos_ref)
{    
    if (o->control_mode != eomc_controlmode_direct)
    {
        return FALSE;
    }
    
    if (o->pos_min != o->pos_max) LIMIT2(o->pos_min, pos_ref, o->pos_max);
    
    o->pos_ref = pos_ref;
    o->vel_ref = 0.0f;
    
    Trajectory_set_pos_raw(&o->trajectory, pos_ref);
    
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

BOOL Joint_set_out_ref(Joint* o, CTRL_UNITS out_ref)
{
    if (o->control_mode != eomc_controlmode_openloop)
    {
        return FALSE;
    }
    
    o->out_ref = out_ref;

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

//VALE: debug function. I'll remove it ASAP
//void Joint_update_debug_current_info(Joint *o, int32_t avgCurrent, int32_t accum_Ep)
//{
//    if you use this function, comments updates meas_acceleration and meas_velocity in 
//      Joint_get_state function
//    o->eo_joint_ptr->status.core.measures.meas_acceleration = avgCurrent;
//    o->eo_joint_ptr->status.core.measures.meas_velocity = accum_Ep;
//}
