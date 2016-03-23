//#include "stdlib.h"
//#include <string.h>

#include "EoCommon.h"
#include "iCubCanProto_types.h"
#include "EoProtocol.h"
#include "EOtheCANprotocol.h"
#include "EOtheCANservice.h"
#include "hal_motor.h"
#include "Motor.h"
#include "hal_led.h"

#include "EOtheErrorManager.h"
#include "EoError.h"

/////////////////////////////////////////////////////////
// Motor

static void Motor_new_state_req(Motor *o, icubCanProto_controlmode_t control_mode)
{
    o->control_mode_req = control_mode;
    
    WatchDog_rearm(&o->control_mode_req_wdog);
}

/*
static void Motor_config_MC4p(uint8_t motor, eOmc_motor_config_t* config)
{
}
*/

static void Motor_hardStopCalbData_reset(Motor* o)
{
    memset(&o->hardstop_calibdata, 0, sizeof(HardStopCalibData));
    //explicit reset calib
    o->hardstop_calibdata.u.bits.iscalibrating = 0;

}
static void Motor_config_2FOC(uint8_t motor, eOmc_motor_config_t* config)
{   
    int8_t KpKiKdKs[7];
    
    ((int16_t*)KpKiKdKs)[0] = config->pidcurrent.kp;    //Kp
    ((int16_t*)KpKiKdKs)[1] = config->pidcurrent.ki;    //Ki
    ((int16_t*)KpKiKdKs)[2] = config->pidcurrent.kd;    //Kd (unused in 2FOC)
               KpKiKdKs [6] = config->pidcurrent.scale; // shift
    
    //((int16_t*)KpKiKdKs)[0] =  8; //Kp
    //((int16_t*)KpKiKdKs)[1] =  2; //Ki
    //((int16_t*)KpKiKdKs)[2] =  0; //Kd (unused in 2FOC)
    //           KpKiKdKs [6] = 10; // shift
    
    uint32_t max_current = config->currentLimits.overloadCurrent;
    
    #define HAS_QE      0x0001
    #define HAS_HALL    0x0002
    #define HAS_TSENS   0x0004
    #define USE_INDEX   0x0008

    uint8_t motor_config[6];
    
    motor_config[0] = 0; // HAS_QE|HAS_HALL;
    
    if (config->hasRotorEncoder)        motor_config[0] |= HAS_QE;
    if (config->hasHallSensor)          motor_config[0] |= HAS_HALL;
    if (config->hasRotorEncoderIndex)   motor_config[0] |= USE_INDEX;
    if (config->hasTempSensor)          motor_config[0] |= HAS_TSENS;
    
    *(int16_t*)(motor_config+1) = config->rotorEncoderResolution;
    *(int16_t*)(motor_config+3) = config->rotorIndexOffset;
    
    motor_config[5] = config->motorPoles;
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    
    eOcanprot_command_t cmdPid;
    cmdPid.class = eocanprot_msgclass_pollingMotorControl;
    cmdPid.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    cmdPid.value = KpKiKdKs;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdPid, id32);
    
    eOcanprot_command_t cmdMaxCurrent;
    cmdMaxCurrent.class = eocanprot_msgclass_pollingMotorControl;
    cmdMaxCurrent.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    cmdMaxCurrent.value = &max_current;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdMaxCurrent, id32);

    eOcanprot_command_t cmdMotorConfig;
    cmdMotorConfig.class = eocanprot_msgclass_pollingMotorControl;
    cmdMotorConfig.type = ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG;
    cmdMotorConfig.value = motor_config;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdMotorConfig, id32);      
}

static void Motor_set_control_mode_2FOC(uint8_t motor, icubCanProto_controlmode_t control_mode)
{
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &control_mode;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
}


// public
Motor* Motor_new(uint8_t n) //
{
    Motor *o = NEW(Motor, n);

    for (uint8_t i=0; i<n; ++i)
    {
        Motor_init(o+i);
    }
    
    return o;
}

void Motor_init(Motor* o) //
{
    memset(o, 0, sizeof(Motor));

    o->not_calibrated = TRUE;
    
    o->control_mode           = icubCanProto_controlmode_notConfigured;
    o->control_mode_req       = icubCanProto_controlmode_notConfigured;

    o->can_dead = FALSE;
    o->wrong_ctrl_mode = FALSE;
    o->external_fault = FALSE;
    
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
    o->diagnostics_refresh = 0;
    
    WatchDog_init(&o->control_mode_req_wdog);
    WatchDog_init(&o->can_2FOC_alive_wdog);
    
    Motor_hardStopCalbData_reset(o);
}

void Motor_config(Motor* o, uint8_t ID, eOmc_motor_config_t* config) //
{
    // const init
    o->ID                 = ID;
    //o->HARDWARE_TYPE      = hardware_type;
    //o->MOTOR_CONTROL_TYPE = motor_control_type;
    o->GEARBOX            = config->gearboxratio;
    o->HAS_TEMP_SENSOR    = config->hasTempSensor;
    
    o->enc_sign = config->rotorEncoderResolution >= 0 ? 1 : -1; 
    
    o->temperature_max = config->temperatureLimit;

    o->pos_min = config->limitsofrotor.min;
    o->pos_max = config->limitsofrotor.max;    
    o->vel_max = config->maxvelocityofmotor;
    o->pwm_max = config->pwmLimit;
    o->Iqq_max = config->pidcurrent.limitonoutput;
    //o->Trq_max = ???
 
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_config_2FOC(o->ID, config);
        
        WatchDog_set_base_time_msec(&o->can_2FOC_alive_wdog, CAN_ALIVE_TIMEOUT);
        WatchDog_rearm(&o->can_2FOC_alive_wdog);
    
        WatchDog_set_base_time_msec(&o->control_mode_req_wdog, CTRL_REQ_TIMEOUT);
        
        Motor_new_state_req(o, icubCanProto_controlmode_idle);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        //Motor_config_MC4p(o->ID, config);
        o->control_mode = icubCanProto_controlmode_idle;
        hal_motor_disable((hal_motor_t)o->ID);
    }    
}

void Motor_destroy(Motor* o) //
{
    DELETE(o);
}

void Motor_config_trqPID(Motor* o, eOmc_PID_t* pid) //
{
    PID_config(&o->trqPID, pid);
}

void Motor_config_filter(Motor* o, uint8_t filter) //
{
    PID_config_filter(&o->trqPID, filter);
}

void Motor_config_friction(Motor* o, float Bemf, float Ktau) //
{
    PID_config_friction(&o->trqPID, Bemf, Ktau);
}

void Motor_calibrate_withOffset(Motor* o, int32_t offset) //
{
    o->pos_calib_offset = offset;
    
    Motor_set_run(o);
    if(o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        o->not_calibrated = FALSE;
    }
}

void Motor_calibrate_moving2Hardstop(Motor* o, int32_t pwm, int32_t zero) //
{
    Motor_hardStopCalbData_reset(o);
    //int16_t gearbox = 1;
    o->pos_calib_offset = 0;
    o->hardstop_calibdata.limited_pwm = pwm;
    o->hardstop_calibdata.u.bits.hwlimitreached = 0;
    o->hardstop_calibdata.u.bits.iscalibrating = 1;
    o->hardstop_calibdata.last_pos = o->pos_fbk;
//    if(0 != o->GEARBOX)
//        gearbox  = o->GEARBOX; //to avoid to do a divition by zero ig gearbox is not configured correctly
//    
//    o->hardstop_calibdata.zero = zero*gearbox;
    o->hardstop_calibdata.zero = zero;
    Motor_set_run(o);
}
#define MOTOR_HARDSTOP_WAITCALIBCOUNTER_MAX     1200
#define MOTOR_POSSTABLE_COUNTER_MAX             100

static void Motor_check_hardstopReached(Motor *o)
{
//if for 20 consecutive times (~20ms) I'm in the same position (but let the calibration start before...), it means that I reached the hardware limit

    o->hardstop_calibdata.waitcalib_counter += 1;
    if (o->hardstop_calibdata.waitcalib_counter > MOTOR_HARDSTOP_WAITCALIBCOUNTER_MAX)
    {
        if (o->pos_fbk == o->hardstop_calibdata.last_pos)
        {
            o->hardstop_calibdata.posStable_counter += 1;
            if (o->hardstop_calibdata.posStable_counter == MOTOR_POSSTABLE_COUNTER_MAX)
            {
                o->hardstop_calibdata.posStable_counter = 0;
                o->hardstop_calibdata.waitcalib_counter = 0;
                o->hardstop_calibdata.u.bits.hwlimitreached = 1;
            }
        }
        else
        {
           o->hardstop_calibdata.posStable_counter = 0;
           o->hardstop_calibdata.last_pos = o->pos_fbk;
        }
    }
}


extern void Motor_do_calibration(Motor* o)
{
    if(0 == o->hardstop_calibdata.u.bits.iscalibrating)
        return;
    
    Motor_check_hardstopReached(o);
    if(o->hardstop_calibdata.u.bits.hwlimitreached)
    {
        o->output = 0;
        o->pos_calib_offset = (o->pos_fbk - o->hardstop_calibdata.zero);
        o->not_calibrated = FALSE;
        Motor_hardStopCalbData_reset(o);
    }
    else
    {
         o->output = o->hardstop_calibdata.limited_pwm;
    }

}

void Motor_set_run(Motor* o) //
{
    icubCanProto_controlmode_t control_mode;
    
    switch (o->MOTOR_CONTROL_TYPE)
    {
        case PWM_CONTROLLED_MOTOR:
            control_mode = icubCanProto_controlmode_openloop;
            break;
        
        case VEL_CONTROLLED_MOTOR:
            control_mode = icubCanProto_controlmode_speed_voltage;
            break;

        case IQQ_CONTROLLED_MOTOR:
            control_mode = icubCanProto_controlmode_current;
            break;
        
        default:
            return;
    }
    
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_set_control_mode_2FOC(o->ID, control_mode);
        Motor_new_state_req(o, control_mode);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_enable((hal_motor_t)o->ID);
        
        o->control_mode = control_mode;
    }
}

void Motor_set_idle(Motor* o) //
{
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_set_control_mode_2FOC(o->ID, icubCanProto_controlmode_idle);
        Motor_new_state_req(o, icubCanProto_controlmode_idle);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_disable((hal_motor_t)o->ID);
        
        if (o->control_mode != icubCanProto_controlmode_hwFault)
        {
            o->control_mode = icubCanProto_controlmode_idle;
        }
    }
}

void Motor_force_idle(Motor* o) //
{
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_set_control_mode_2FOC(o->ID, icubCanProto_controlmode_forceIdle);
        Motor_new_state_req(o, icubCanProto_controlmode_idle);
        WatchDog_rearm(&o->can_2FOC_alive_wdog);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_disable((hal_motor_t)o->ID);
        
        o->control_mode = icubCanProto_controlmode_idle;
    }
    
    o->can_dead = FALSE;
    o->wrong_ctrl_mode = FALSE;
    
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
    o->diagnostics_refresh = 0;
    
    o->hardware_fault = FALSE;
}

static void Motor_send_error(uint8_t id, eOerror_value_MC_t err_id, uint64_t mask)
{
    static eOerrmanDescriptor_t descriptor = {0};
    descriptor.par16 = id;
    descriptor.par64 = mask;
    descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
    descriptor.sourceaddress = 0;
    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, err_id);
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);    
}

BOOL Motor_check_faults(Motor* o) //
{
    if (o->control_mode == icubCanProto_controlmode_hwFault)
    {
        o->hardware_fault = TRUE;
    }
    
    BOOL can_dead        = FALSE;
    BOOL wrong_ctrl_mode = FALSE;
    BOOL external_fault  = FALSE;
    
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        if (o->fault_state.bits.ExternalFaultAsserted)
        {
            external_fault = TRUE;
        }
        else
        {
            o->external_fault = FALSE;
        }
        
        if (o->control_mode == o->control_mode_req)
        {
            o->wrong_ctrl_mode = FALSE;
            
            WatchDog_rearm(&o->control_mode_req_wdog);
        }
        else
        {
            if (!(external_fault || o->hardware_fault))
            {
                if (WatchDog_check_expired(&o->control_mode_req_wdog))
                {
                    wrong_ctrl_mode = TRUE;
                        
                    o->hardware_fault = TRUE;
                }    
            }
        }    
        
        if (WatchDog_check_expired(&o->can_2FOC_alive_wdog))
        {
            can_dead = TRUE;
            
            o->hardware_fault = TRUE;
        }
        else
        {
            o->can_dead = FALSE;
        }
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        external_fault = hal_motor_externalfaulted();
        
        if (o->hardware_fault || external_fault)
        {
            hal_motor_disable((hal_motor_t)o->ID);
        }
    }
    
    if (!(o->hardware_fault || external_fault))
    {
        o->fault_state_prec.bitmask = 0;

        return FALSE;
    }
    
    if (++o->diagnostics_refresh > 5*CTRL_LOOP_FREQUENCY_INT)
    {
        o->diagnostics_refresh = 0;
        o->fault_state_prec.bitmask = 0;
        
        o->can_dead = FALSE;
        o->wrong_ctrl_mode = FALSE;
        o->external_fault = FALSE;
    }
    
    if (o->fault_state_prec.bitmask != o->fault_state.bitmask)
    {
        MotorFaultState fault_state;
        fault_state.bitmask = o->fault_state.bitmask;
        
        if (o->fault_state.bits.OverCurrentFailure && !o->fault_state_prec.bits.OverCurrentFailure)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_overcurrent, 0);
            fault_state.bits.OverCurrentFailure = FALSE;
        }
        
        if (o->fault_state.bits.I2TFailure && !o->fault_state_prec.bits.I2TFailure)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_i2t_limit, 0);
            fault_state.bits.I2TFailure = FALSE;
        }
        
        if ((o->fault_state.bits.DHESInvalidSequence && !o->fault_state_prec.bits.DHESInvalidSequence)
         || (o->fault_state.bits.DHESInvalidValue && !o->fault_state_prec.bits.DHESInvalidValue))
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_hallsensors, 0);
            fault_state.bits.DHESInvalidSequence = FALSE;
            fault_state.bits.DHESInvalidValue = FALSE;
        }
        
        if (o->fault_state.bits.CANInvalidProtocol && !o->fault_state_prec.bits.CANInvalidProtocol)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_can_invalid_prot, 0);
            fault_state.bits.CANInvalidProtocol = FALSE;
        }
        
        #define CAN_GENERIC_ERROR 0x00003D00
        
        if ((o->fault_state.bitmask & CAN_GENERIC_ERROR) && ((o->fault_state.bitmask & CAN_GENERIC_ERROR) != (o->fault_state_prec.bitmask & CAN_GENERIC_ERROR)))
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_can_generic, (o->fault_state.bitmask & CAN_GENERIC_ERROR));
            fault_state.bitmask &= ~CAN_GENERIC_ERROR;
        }
                
        if (o->fault_state.bits.EncoderFault & !o->fault_state_prec.bits.EncoderFault)
        {
            if (o->qe_state.bits.dirty)
            {
                Motor_send_error(o->ID, eoerror_value_MC_motor_qencoder_dirty, 0);
            }
        
            if (o->qe_state.bits.index_broken)
            {
                Motor_send_error(o->ID, eoerror_value_MC_motor_qencoder_index, 0);
            }

            if (o->qe_state.bits.phase_broken)
            {
                Motor_send_error(o->ID, eoerror_value_MC_motor_qencoder_phase, 0);
            }
            
            fault_state.bits.EncoderFault = FALSE;
        }
    
        if (can_dead && !o->can_dead)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_can_no_answer, 0);
        }
        
        if (wrong_ctrl_mode && !o->wrong_ctrl_mode)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_wrong_state, 0);
        }
        
        if (external_fault && !o->external_fault)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_external_fault, 0);
            fault_state.bits.ExternalFaultAsserted = FALSE;
        }
        
        if (fault_state.bitmask)
        {
            Motor_send_error(o->ID, eoerror_value_MC_generic_error, fault_state.bitmask);
        }
        
        o->fault_state_prec.bitmask = o->fault_state.bitmask;
    }
    
    o->external_fault = external_fault;
    o->wrong_ctrl_mode = wrong_ctrl_mode;
    o->can_dead = can_dead;
    
    return o->hardware_fault;
}

void Motor_raise_fault_overcurrent(Motor* o)
{
    hal_motor_disable((hal_motor_t)o->ID);
    
    o->fault_state.bits.OverCurrentFailure = TRUE;
    
    o->control_mode = icubCanProto_controlmode_hwFault;
}

void Motor_raise_fault_i2t(Motor* o)
{
    hal_motor_disable((hal_motor_t)o->ID);
    
    o->fault_state.bits.I2TFailure = TRUE;
    
    o->control_mode = icubCanProto_controlmode_hwFault;
}

void Motor_raise_fault_external(Motor* o)
{
    hal_motor_disable((hal_motor_t)o->ID);
    
    o->fault_state.bits.ExternalFaultAsserted = TRUE;
    
    if (o->control_mode != icubCanProto_controlmode_hwFault)
    {
        o->control_mode = icubCanProto_controlmode_idle;
    }
}

void Motor_motion_reset(Motor *o) //
{
    PID_reset(&o->trqPID);
}

BOOL Motor_is_calibrated(Motor* o) //
{
    return !(o->not_calibrated);
}

CTRL_UNITS Motor_do_trq_control(Motor* o, CTRL_UNITS trq_ref, CTRL_UNITS trq_fbk) //
{
    o->trq_ref = trq_ref;
    o->trq_fbk = trq_fbk;
    
    o->trq_err = trq_ref - trq_fbk;
    
    return PID_do_out(&o->trqPID, o->trq_err) + PID_do_friction_comp(&o->trqPID, o->vel_fbk, o->trq_ref);
}

void Motor_update_state_fbk(Motor* o, void* state) //
{
    State2FocMsg* state_msg = (State2FocMsg*)state;
    
    WatchDog_rearm(&o->can_2FOC_alive_wdog);
   
    o->fault_state.bitmask = state_msg->fault_state;
    
    o->control_mode        = (icubCanProto_controlmode_t)state_msg->control_mode; 
    o->pwm_fbk             = state_msg->pwm_fbk;
    o->qe_state.bitmask    = state_msg->qe_state;
    o->not_calibrated      = o->qe_state.bits.not_calibrated;
}

void Motor_update_odometry_fbk_can(Motor* o, CanOdometry2FocMsg* can_msg) //
{
    WatchDog_rearm(&o->can_2FOC_alive_wdog);
    
    o->Iqq_fbk = can_msg->current;
    
    o->vel_raw_fbk = can_msg->current*1000;
    o->vel_fbk = o->vel_raw_fbk/o->GEARBOX;
    
    o->pos_raw_fbk = can_msg->position;
    o->pos_fbk = o->pos_calib_offset + o->pos_raw_fbk/o->GEARBOX;
}

void Motor_actuate(Motor* motor, uint8_t N) //
{
    if (motor->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        int16_t output[MAX_PER_BOARD];
    
        for (int m=0; m<N; ++m)
        {
            output[motor[m].actuatorPort] = motor[m].output;
            //output[m] = motor[m].output;
        }
    
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_periodicMotorControl;    
        command.type  = ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT;
        command.value = output;
    
        eOcanmap_location_t location = {0};
        location.port = eOcanport1;
        location.addr = 0; // marco.accame: we put 0 just because it is periodic and this is the source address (the EMS has can address 0).
        location.insideindex = eocanmap_insideindex_first; // because all 2foc have motor on index-0. 

        // and i send the command
        eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, location); 
    }
    else if (motor->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        for (int m=0; m<N; ++m)
        {
            motor[m].pwm_fbk = motor[m].output;
            hal_motor_pwmset((hal_motor_t)motor[m].actuatorPort, motor[m].output);
        }
    }
}  

// Motor
/////////////////////////////////////////////////////////

void Motor_set_pwm_ref(Motor* o, int32_t pwm_ref)
{
    if (o->pos_min != o->pos_max)
    {        
        if ((o->pos_fbk < o->pos_min) && (pwm_ref < 0))
        {
            o->output = o->pwm_ref = 0;
        }
        else if ((o->pos_fbk > o->pos_max) && (pwm_ref > 0))
        {
            o->output = o->pwm_ref = 0;
        }
        else
        {
            o->output = o->pwm_ref = CUT(pwm_ref, o->pwm_max);
        }
    }
    else
    {
        o->output = o->pwm_ref = CUT(pwm_ref, o->pwm_max);
    }
}

void Motor_set_Iqq_ref(Motor* o, int32_t Iqq_ref)
{
    if (o->pos_min != o->pos_max)
    {        
        if ((o->pos_fbk < o->pos_min) && (Iqq_ref < 0))
        {
            o->output = o->Iqq_ref = 0;
        }
        else if ((o->pos_fbk > o->pos_max) && (Iqq_ref > 0))
        {
            o->output = o->Iqq_ref = 0;
        }
        else
        {
            o->output = o->Iqq_ref = CUT(Iqq_ref, o->Iqq_max);
        }
    }
    else
    {
        o->output = o->Iqq_ref = CUT(Iqq_ref, o->Iqq_max);
    }
}

void Motor_set_vel_ref(Motor* o, int32_t vel_ref)
{
    if (o->pos_min != o->pos_max)
    {        
        if ((o->pos_fbk < o->pos_min) && (vel_ref < 0))
        {
            o->output = o->vel_ref = 0;
        }
        else if ((o->pos_fbk > o->pos_max) && (vel_ref > 0))
        {
            o->output = o->vel_ref = 0;
        }
        else
        {
            o->vel_ref = vel_ref; // CUT(vel_ref, o->vel_max);
            o->output = (o->vel_ref * o->GEARBOX)/1000;
        }
    }
    else
    {
        o->vel_ref = vel_ref; // CUT(vel_ref, o->vel_max);
        o->output = (o->vel_ref * o->GEARBOX)/1000;
    }
}

/*
void Motor_set_trq_ref(Motor* o, CTRL_UNITS trq_ref)
{ 
    o->trq_ref = trq_ref; 
}
*/
/*
uint32_t Motor_get_fault_mask(Motor* o)
{
    return o->fault_state.bitmask;
}
*/

void Motor_get_pid_state(Motor* o, eOmc_joint_status_ofpid_t* pid_state)
{
    pid_state->complpos.reftrq = o->trq_ref;
    pid_state->complpos.errtrq = o->trq_err;
    pid_state->complpos.output = o->output;
}

void Motor_get_state(Motor* o, eOmc_motor_status_t* motor_status)
{
    motor_status->basic.mot_position = o->pos_raw_fbk;
    motor_status->basic.mot_velocity = o->vel_raw_fbk;
    motor_status->basic.mot_acceleration = 0; // not implemented
    motor_status->basic.mot_current  = o->Iqq_fbk;    
    motor_status->basic.mot_pwm      = o->pwm_fbk;
}

void Motor_update_pos_fbk(Motor* o, int32_t position_raw)
{
    int32_t position;  
    int32_t pippo=0;
    
    o->pos_raw_fbk = position_raw;
    position = o->pos_raw_fbk/o->GEARBOX - o->pos_calib_offset;
    
    if(o->ID == 1)
    {
        pippo=position_raw;
    }    
    //valid for init
    if ((o->pos_fbk == 0) && (o->pos_fbk_old == 0))
    {
        o->pos_fbk     = position;
        o->pos_fbk_old = position;
        
        return;
    }
    
    //direction of movement changes depending on the sign
    int32_t delta = o->enc_sign * (position - o->pos_fbk_old);
    
    //normalize delta to avoid discontinuities
    while (delta < -TICKS_PER_HALF_REVOLUTION) delta += TICKS_PER_REVOLUTION;
    while (delta >  TICKS_PER_HALF_REVOLUTION) delta -= TICKS_PER_REVOLUTION;
        
    o->pos_fbk += delta;
    
    //update velocity
    o->vel_fbk = delta*CTRL_LOOP_FREQUENCY_INT;
    
    //update last position for next iteration
    o->pos_fbk_old = position;
}

void Motor_update_current_fbk(Motor* o, int16_t current)
{
    o->Iqq_fbk = current;
}

void Motor_config_gearbox_ratio(Motor* o, int32_t gearbox_ratio)
{
    o->GEARBOX = gearbox_ratio;
}

int16_t Motor_config_pwm_limit(Motor* o, int16_t pwm_limit)
{
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        if (pwm_limit > 32000) pwm_limit = 32000;
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        if (pwm_limit > 3360) pwm_limit = 3360; 
    }
    
    o->pwm_max = pwm_limit;
    
    return pwm_limit;
}

BOOL Motor_is_external_fault(Motor* o)
{
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        return o->fault_state.bits.ExternalFaultAsserted;
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        return hal_motor_externalfaulted();
    }
    
    return FALSE;
}

void Motor_reset(Motor *o)
{
    o->pwm_fbk=ZERO;
    o->pwm_ref=ZERO;

    o->pos_raw_fbk=ZERO;
    o->vel_raw_fbk=ZERO;

    o->pos_calib_offset=ZERO;

    o->pos_ref=ZERO;
    o->pos_fbk=ZERO;
    o->pos_err=ZERO;

    o->pos_fbk_old=ZERO;

    o->vel_ref=ZERO;
    o->vel_fbk=ZERO;
    o->vel_err=ZERO;

    Motor_motion_reset(o);

    o->output=ZERO;

    o->not_calibrated = TRUE;

    //o->control_mode = ???
    //o->control_mode_req;
}
/*
void Motor_update_temperature_fbk(Motor* o, int16_t temperature_fbk) { o->temperature_fbk = temperature_fbk; }
void Motor_update_pos_raw_fbk(Motor* o, int32_t pos_raw_fbk) { o->pos_raw_fbk = pos_raw_fbk; }
void Motor_update_vel_raw_fbk(Motor* o, int32_t vel_raw_fbk) { o->vel_raw_fbk = vel_raw_fbk; }
void Motor_update_pwm_fbk(Motor* o, int16_t pwm_fbk) { o->pwm_fbk = pwm_fbk; }
void Motor_update_Iqq_fbk(Motor* o, int16_t Iqq_fbk) { o->Iqq_fbk = Iqq_fbk; }
void Motor_update_pos_fbk(Motor* o, int32_t pos_fbk) { o->pos_fbk = pos_fbk; }
void Motor_update_vel_fbk(Motor* o, int32_t vel_fbk) { o->vel_fbk = vel_fbk; }
void Motor_update_trq_fbk(Motor* o, CTRL_UNITS trq_fbk) { o->trq_fbk = trq_fbk; }

int16_t Motor_get_temperature_fbk(Motor* o) { return o->temperature_fbk; }
int32_t Motor_get_pos_raw_fbk(Motor* o) { return o->pos_raw_fbk; }
int32_t Motor_get_vel_raw_fbk(Motor* o) { return o->vel_raw_fbk; }
int16_t Motor_get_pwm_fbk(Motor* o) { return o->pwm_fbk; }
int16_t Motor_get_Iqq_fbk(Motor* o) { return o->Iqq_fbk; }
int32_t Motor_get_pos_fbk(Motor* o) { return o->pos_fbk; }
int32_t Motor_get_vel_fbk(Motor* o) { return o->vel_fbk; }
CTRL_UNITS Motor_get_trq_fbk(Motor* o) { return o->trq_fbk; }
*/
