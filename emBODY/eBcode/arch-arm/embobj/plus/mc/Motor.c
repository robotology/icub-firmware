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

#if !defined(__cplusplus)
    #error this Motor.c file must be compiled in C++
#endif

// - API
#include "Motor.h"
    
// - dependencies   
#include "EoCommon.h"
#include "iCubCanProto_types.h"
#include "EoProtocol.h"
#include "EOtheCANprotocol.h"
#include "hal_motor.h"

#include "hal_led.h"

#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOtheEntities.h"
    

#include "Motor_hid.h"

#include "embot_app_eth_mc_messaging.h"



/////////////////////////////////////////////////////////
// Motor

static void send_debug_message(const char *message, uint8_t jid, uint16_t par16, uint64_t par64)
{

    eOerrmanDescriptor_t errdes = {0};

    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = jid;
    errdes.par16            = par16;
    errdes.par64            = par64;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, message, NULL, &errdes);

}

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

BOOL Motor_is_still(Motor* o, int32_t space_window, int32_t time_window)
{
    o->partial_space += o->pos_raw_fbk - o->pos_raw_fbk_old;
    o->pos_raw_fbk_old = o->pos_raw_fbk;
    
    BOOL still = FALSE;
    
    if (++o->partial_timer > time_window)
    {        
        still = abs(o->partial_space) < space_window;
        
        o->partial_timer = 0;
        o->partial_space = 0;
    }
    
    return still;
}

void Motor_still_check_reset(Motor* o)
{
    o->partial_timer = 0;
    o->partial_space = 0;
}

static void Motor_hardStopCalbData_reset(Motor* o)
{
    memset(&o->hardstop_calibdata, 0, sizeof(HardStopCalibData));
    //explicit reset calib
    o->hardstop_calibdata.u.bits.iscalibrating = 0;
}

static void Motor_config_current_PID_2FOC(Motor* o, eOmc_PID_t* pidcurrent)
{
    int8_t KpKiKdKs[7];
    
    float32_t ks = 1.0f/(float32_t)(1<<pidcurrent->scale);
    float32_t kp = ks*pidcurrent->kp;
    float32_t ki = ks*pidcurrent->ki;
    float32_t kd = ks*pidcurrent->kd;
    
    if (kp<0.0f || ki<0.0f || kd<0.0f) return;
    
    float32_t   max = kp;
    if (ki>max) max = ki;
    if (kd>max) max = kd;
    
    int16_t Kp = 0;
    int16_t Ki = 0;
    int16_t Kd = 0;
    uint8_t Ks = 0;
    
    for (int exponent = 0; exponent < 16; ++exponent)
    {
        float32_t power = (float32_t)(1<<exponent);
        
        if (max < power)
        {
            Kp = (int16_t)(kp*32768.0f/power);
            Ki = (int16_t)(ki*32768.0f/power);
            Kd = (int16_t)(kd*32768.0f/power);
            Ks = 15-exponent;
            
            break;
        }    
    }
    
    // ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID
    embot::app::eth::mc::messaging::sender::Set_Current_PID msg {{&o->motorlocation}, {Kp, Ki, Kd, Ks}};
    msg.transmit(); 
}

static void Motor_config_velocity_PID_2FOC(Motor* o, eOmc_PID_t* pidvelocity)
{
    int8_t KpKiKdKs[7];
    
    float32_t ks = 1.0f/(float32_t)(1<<pidvelocity->scale);
    float32_t kp = ks*pidvelocity->kp;
    float32_t ki = ks*pidvelocity->ki;
    float32_t kd = ks*pidvelocity->kd;
    
    if (kp<0.0f || ki<0.0f || kd<0.0f) return;
    
    float32_t   max = kp;
    if (ki>max) max = ki;
    if (kd>max) max = kd;
    
    int16_t Kp = 0;
    int16_t Ki = 0;
    int16_t Kd = 0;
    uint8_t Ks = 0;
    
    for (int exponent = 0; exponent < 16; ++exponent)
    {
        float32_t power = (float32_t)(1<<exponent);
        
        if (max < power)
        {
            Kp = (int16_t)(kp*32768.0f/power);
            Ki = (int16_t)(ki*32768.0f/power);
            Kd = (int16_t)(kd*32768.0f/power);
            Ks = 15-exponent;
            
            break;
        }    
    }

    // ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID    
    embot::app::eth::mc::messaging::sender::Set_Velocity_PID msg {{&o->motorlocation}, {Kp, Ki, Kd, Ks}};
    msg.transmit();    
}

static void Motor_config_max_currents_2FOC(Motor* o, eOmc_current_limits_params_t* current_params)
{ 
    // ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT    
    embot::app::eth::mc::messaging::sender::Set_Current_Limit msg 
    {
        {&o->motorlocation},
        {current_params->nominalCurrent, current_params->peakCurrent, current_params->overloadCurrent}
    };
    msg.transmit();   
}

static void Motor_config_motor_max_temperature_2FOC(Motor* o, eOmeas_temperature_t* motor_temperature_limit)
{
    // ICUBCANPROTO_POL_MC_CMD__SET_TEMPERATURE_LIMIT
    embot::app::eth::mc::messaging::sender::Set_Temperature_Limit msg
    {
        {&o->motorlocation},
        {*(int16_t*)motor_temperature_limit}
    };
    msg.transmit();
      
}

static void Motor_config_2FOC(Motor* o, eOmc_motor_config_t* config)
{   
    #define HAS_QE         0x0001
    #define HAS_HALL       0x0002
    #define HAS_TSENS      0x0004
    #define USE_INDEX      0x0008
    #define HAS_SPEED_QE   0x0010
    #define ENA_VERBOSE    0x0020
    
    o->can_motor_config[0] = 0;
    
    if (config->hasRotorEncoder)        o->can_motor_config[0] |= HAS_QE;
    if (config->hasHallSensor)          o->can_motor_config[0] |= HAS_HALL;
    if (config->hasRotorEncoderIndex)   o->can_motor_config[0] |= USE_INDEX;
    if (config->hasTempSensor)          o->can_motor_config[0] |= HAS_TSENS;
    if (config->hasSpeedEncoder)        o->can_motor_config[0] |= HAS_SPEED_QE;
    if (config->verbose)                o->can_motor_config[0] |= ENA_VERBOSE;
    
    *(int16_t*)(o->can_motor_config+1) = config->rotorEncoderResolution;
    *(int16_t*)(o->can_motor_config+3) = config->rotorIndexOffset;
    
    o->can_motor_config[5] = config->motorPoles;
    
    o->can_motor_config[6] = (int)(o->enc_tolerance*10.0f);

    Motor_config_current_PID_2FOC(o, &(config->pidcurrent));
    Motor_config_velocity_PID_2FOC(o, &(config->pidspeed));
    
    // ICUBCANPROTO_POL_MC_CMD__SET_TEMPERATURE_LIMIT
    embot::app::eth::mc::messaging::sender::Set_Temperature_Limit msgtmp
    {
        {&o->motorlocation},
        {(int16_t)config->temperatureLimit}
    };
    msgtmp.transmit();

    // ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT 
    embot::app::eth::mc::messaging::sender::Set_Current_Limit msg 
    {
        {&o->motorlocation},
        {config->currentLimits.nominalCurrent, config->currentLimits.peakCurrent, config->currentLimits.overloadCurrent}
    };
    msg.transmit(); 
        
    // ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG    
    embot::app::eth::mc::messaging::sender::Set_Motor_Config msgmc {{&o->motorlocation}, {&o->can_motor_config[0]}};
    msgmc.transmit();          
}

static void Motor_set_control_mode_2FOC(Motor* o, icubCanProto_controlmode_t control_mode)
{   
    // ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE
    embot::app::eth::mc::messaging::sender::Set_Control_Mode msg {{&o->motorlocation}, {control_mode}};
    msg.transmit(); 
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

    o->GEARBOX = 1;
    
    o->not_init = TRUE;
    o->not_calibrated = TRUE;
    
    o->control_mode           = icubCanProto_controlmode_notConfigured;
    o->control_mode_req       = icubCanProto_controlmode_notConfigured;

    o->can_dead = FALSE;
    o->wrong_ctrl_mode = FALSE;
    
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
    //o->diagnostics_refresh = 0;
    //o->diagnostics_refresh_warning = 0;
//    o->qencoder_err_counter = 0;
    o->diagnostic.total_counter=0;
    o->diagnostic.qe_dirty_err_counter=0;
    o->diagnostic.qe_index_broken_err_counter=0;
    o->diagnostic.qe_phase_broken_err_counter=0;
    
    WatchDog_init(&o->control_mode_req_wdog);
    WatchDog_init(&o->can_2FOC_alive_wdog);
    
    Motor_hardStopCalbData_reset(o);
    
    //o->outOfLimitsSignaled = FALSE;
}

void Motor_config(Motor* o, uint8_t ID, eOmc_motor_config_t* config) //
{
    // const init
    o->ID                 = ID;
    o->GEARBOX            = config->gearbox_M2J;
    o->HAS_TEMP_SENSOR    = config->hasTempSensor;
    
    o->enc_sign = config->rotorEncoderResolution >= 0 ? 1 : -1; 
    
    
    
    //in this case tolarance is the numeber of encoder units lost each complete rotation
    //send can message to 2foc
    o->enc_tolerance = config->rotEncTolerance; //maybe this field is not necessary //ALE
    
    o->temperature_max = config->temperatureLimit;

    o->pos_min = config->limitsofrotor.min;
    o->pos_max = config->limitsofrotor.max;

    o->vel_max = config->maxvelocityofmotor;
    
    o->Iqq_max = config->pidcurrent.limitonoutput;
    
    o->Iqq_ovl = config->currentLimits.overloadCurrent;
    
    config->pwmLimit = Motor_config_pwm_limit(o, config->pwmLimit);
 
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_config_2FOC(o, config);
        
        WatchDog_set_base_time_msec(&o->can_2FOC_alive_wdog, CAN_ALIVE_TIMEOUT);
        WatchDog_rearm(&o->can_2FOC_alive_wdog);
    
        WatchDog_set_base_time_msec(&o->control_mode_req_wdog, CTRL_REQ_TIMEOUT);
        
        Motor_new_state_req(o, icubCanProto_controlmode_idle);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        //Motor_config_MC4p(o->ID, config);

        o->control_mode = icubCanProto_controlmode_idle;
        hal_motor_disable(static_cast<hal_motor_t>(o->motorlocation.adr));
    }    
 
#if defined(SENSORLESS_TORQUE)
    o->sensorless_torque = TRUE;
#else
    o->sensorless_torque = FALSE;
#endif

    if (o->sensorless_torque)
    {
        o->torque_estimator.initialize();
        o->torque_estimator.rtU.Km  = 4.3;
        o->torque_estimator.rtU.Kw  = 0.045;
        o->torque_estimator.rtU.S0  = 2.5;
        o->torque_estimator.rtU.S1  = 0.0;
        o->torque_estimator.rtU.Vth = 5.0;
        o->torque_estimator.rtU.Fc_pos = 3.3;
        o->torque_estimator.rtU.Fc_neg = 3.3;
        o->torque_estimator.rtU.Fs_pos = 7.0;
        o->torque_estimator.rtU.Fs_neg = 7.0;
    }
}

void Motor_config_encoder(Motor* o, int32_t resolution)
{
    o->enc_sign = resolution >= 0 ? 1 : -1;
    
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        *(int16_t*)(o->can_motor_config+1) = resolution;

        // ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG   
        embot::app::eth::mc::messaging::sender::Set_Motor_Config msgmc {{&o->motorlocation}, {&o->can_motor_config[0]}};
        msgmc.transmit();           
    }     
}

void Motor_config_max_currents(Motor* o, eOmc_current_limits_params_t* current_params)
{
    o->Iqq_ovl = current_params->overloadCurrent;
    
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_config_max_currents_2FOC(o, current_params);
    }   
}

void Motor_config_max_temperature(Motor* o, eOmeas_temperature_t* motor_temperature_limit)
{
    o->temperature_max = *motor_temperature_limit;
    
    if(o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_config_motor_max_temperature_2FOC(o, motor_temperature_limit);
    }
}

void Motor_config_current_PID(Motor* o, eOmc_PID_t* pidcurrent)
{
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_config_current_PID_2FOC(o, pidcurrent);
    }
}

void Motor_config_torque_PID(Motor* o, eOmc_PID_t* pid) //
{
    PID_config(&o->trqPID, pid);
}

void Motor_config_speed_PID(Motor* o, eOmc_PID_t* pidvelocity)
{
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_config_velocity_PID_2FOC(o, pidvelocity);
    }
}

void Motor_destroy(Motor* o) //
{
    DELETE(o);
}

void Motor_config_filter(Motor* o, uint8_t filter) //
{
    PID_config_filter(&o->trqPID, filter);
}

void Motor_config_friction(Motor* o, float Bemf, float Ktau, eOmc_FrictionParams_t friction) //
{
    PID_config_friction(&o->trqPID, Bemf, Ktau, friction);
}

void Motor_config_LuGre(Motor* o, float Km, float Kw, float S0, float S1, float Vth, float Fc_pos, float Fc_neg, float Fs_pos, float Fs_neg)
{
    o->torque_estimator.rtU.Km  = Km;
    o->torque_estimator.rtU.Kw  = Kw;
    o->torque_estimator.rtU.S0  = S0;
    o->torque_estimator.rtU.S1  = S1;
    o->torque_estimator.rtU.Vth = Vth;
    o->torque_estimator.rtU.Fc_pos  = Fc_pos;
    o->torque_estimator.rtU.Fc_neg  = Fc_neg;
    o->torque_estimator.rtU.Fs_pos  = Fs_pos;
    o->torque_estimator.rtU.Fs_neg  = Fs_neg;
}

void Motor_calibrate_withOffset(Motor* o, int32_t offset) //
{
    o->pos_calib_offset = offset;
    o->pos_fbk_old = 0;
    o->not_init = TRUE;
    //Motor_set_run(o, PWM_INPUT_MOTOR);
    
    if(o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        o->not_calibrated = FALSE;
    }
  
}

BOOL Motor_calibrate_moving2Hardstop(Motor* o, int32_t pwm, int32_t zero) //
{
    Motor_set_run(o, eomc_ctrl_out_type_pwm);
    Motor_check_faults(o);
    
    if(Motor_is_in_fault(o))
        return FALSE;
    
    Motor_reset(o);
    
    Motor_hardStopCalbData_reset(o);
    //int16_t gearbox = 1;
    o->pos_calib_offset = 0;
    o->hardstop_calibdata.limited_pwm = pwm;
    o->hardstop_calibdata.u.bits.hwlimitreached = 0;
    o->hardstop_calibdata.u.bits.iscalibrating = 1;

    o->hardstop_calibdata.last_pos = o->pos_fbk;
    
    o->hardstop_calibdata.zero = zero;
    //Motor_set_run(o);
    return TRUE;
}

extern void Motor_uncalibrate(Motor* o)
{
    //o->not_init = TRUE;
    o->not_calibrated = TRUE;
        
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {        
        // ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER
        // i can use use icubCanProto_calibration_type0_hard_stops because the 2foc does not process the payload. 
        // BUT much better to use a new icubCanProto_calibration_type_undefined 
        embot::app::eth::mc::messaging::sender::Calibrate_Encoder msg {{&o->motorlocation}, {{icubCanProto_calibration_type0_hard_stops, 0}}};
        msg.transmit();         
    }
}


#define MOTOR_HARDSTOP_WAITCALIBCOUNTER_MAX     1200
#define MOTOR_POSSTABLE_COUNTER_MAX             100

static void Motor_check_hardstopReached(Motor *o)
{
    //if for 20 consecutive times (~20ms) I'm in the same position (but let the calibration start before...), it means that I reached the hardware limit
    //TODO: how are we checking position?? Using motor qenc?? are we using some sort of threshold

    o->hardstop_calibdata.waitcalib_counter += 1;
    if (o->hardstop_calibdata.waitcalib_counter > MOTOR_HARDSTOP_WAITCALIBCOUNTER_MAX)
    {
        // TODO: I would put also a delta around last_pos -> so that I do not need to find perfect last pos
        // but wwe can recover from small errors on the qenc which measures rotor pos
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

extern void Motor_do_calibration_hard_stop(Motor* o)
{
    if((0 == o->hardstop_calibdata.u.bits.iscalibrating) || Motor_is_external_fault(o))
        return;
    
    Motor_check_hardstopReached(o);
    if(o->hardstop_calibdata.u.bits.hwlimitreached)
    {
        o->output = 0;
        o->pos_calib_offset = (o->pos_fbk - o->hardstop_calibdata.zero);
        //reset value of position
        o->pos_fbk = o->pos_fbk - o->pos_calib_offset;
        o->pos_fbk_old = 0;
        o->not_init = TRUE;
        
        //debug code
        char message[150];
        snprintf(message, sizeof(message), "Hw lim reached: cp%d cz%d co%d", o->pos_fbk, o->hardstop_calibdata.zero, o->pos_calib_offset);
        send_debug_message(message, o->ID, 0, 0);
        //ends here
        
        o->not_calibrated = FALSE;
        Motor_hardStopCalbData_reset(o);
        
        
    }
    else
    {
         o->output = o->hardstop_calibdata.limited_pwm;
    }
}

BOOL Motor_set_run(Motor* o, int16_t low_lev_ctrl_type)
{
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        if (o->fault_state.bits.ExternalFaultAsserted)
        {
            return FALSE;
        }
    }
    
    icubCanProto_controlmode_t control_mode;
    
    switch (low_lev_ctrl_type)
    {
        case eomc_ctrl_out_type_pwm:
            control_mode = icubCanProto_controlmode_openloop;
            break;
        
        case eomc_ctrl_out_type_vel:
            control_mode = icubCanProto_controlmode_speed_voltage;
            break;

        case eomc_ctrl_out_type_cur:
            control_mode = icubCanProto_controlmode_current;
            break;
        
        default:
            return FALSE;
    }
    
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_set_control_mode_2FOC(o, control_mode);
        Motor_new_state_req(o, control_mode);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_reenable_break_interrupts();
        
        hal_motor_enable(static_cast<hal_motor_t>(o->motorlocation.adr));

        o->control_mode = control_mode;
    }    
    else
    {
        return FALSE;
    }
    
    return TRUE;
}
/*
BOOL Motor_clear_ext_fault(Motor *o)
{
    if(o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_reenable_break_interrupts();
        
        return TRUE;
    }
    
    return FALSE;
}
*/
void Motor_set_idle(Motor* o) //
{
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_set_control_mode_2FOC(o, icubCanProto_controlmode_idle);
        Motor_new_state_req(o, icubCanProto_controlmode_idle);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_disable(static_cast<hal_motor_t>(o->motorlocation.adr));
        
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
        Motor_set_control_mode_2FOC(o, icubCanProto_controlmode_forceIdle);
        Motor_new_state_req(o, icubCanProto_controlmode_idle);
        WatchDog_rearm(&o->can_2FOC_alive_wdog);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_disable(static_cast<hal_motor_t>(o->motorlocation.adr));
        
        o->control_mode = icubCanProto_controlmode_idle;
    }

    o->can_dead = FALSE;
    o->wrong_ctrl_mode = FALSE;
    
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
    o->diagnostics_refresh = 0;
    //o->diagnostics_refresh_warning = 0;
    o->qencoder_err_counter = 0;
    
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
    
    eOmc_motor_status_t *mstatus = NULL;
    mstatus = eo_entities_GetMotorStatus(eo_entities_GetHandle(), id);
    if (NULL != mstatus)
    {
        mstatus->mc_fault_state = descriptor.code;
    }
}


static void Motor_send_diagnostic(eOerrmanErrorType_t errortype, uint8_t id, eOerror_value_MC_t err_id, uint64_t mask)
{
    static eOerrmanDescriptor_t descriptor = {0}; 
    // but also not static can be fine as well. as long as we have enough stack size. 
    // eOerrmanDescriptor_t is 16 bytes big    
    descriptor.par16 = id;
    descriptor.par64 = mask;
    descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
    descriptor.sourceaddress = 0;
    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, err_id);
    eo_errman_Error(eo_errman_GetHandle(), errortype, NULL, NULL, &descriptor);    
}

#define ERROR_COUNTER_MAX                 10

BOOL Motor_check_faults(Motor* o) //
{
    if (o->control_mode == icubCanProto_controlmode_hwFault)
    {
        o->hardware_fault = TRUE;
    }
    
    if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        o->fault_state.bits.ExternalFaultAsserted = hal_motor_externalfaulted();
    }

    
    if (o->fault_state.bits.ExternalFaultAsserted)
    {
        o->control_mode_req = icubCanProto_controlmode_idle;
    }
    
    BOOL can_dead        = FALSE;
    BOOL wrong_ctrl_mode = FALSE;
    
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        if (o->control_mode == o->control_mode_req || o->hardware_fault || o->fault_state.bits.ExternalFaultAsserted)
        {
            o->wrong_ctrl_mode = FALSE;
            
            WatchDog_rearm(&o->control_mode_req_wdog);
        }
        else
        {
            if (WatchDog_check_expired(&o->control_mode_req_wdog))
            {
                wrong_ctrl_mode = TRUE;
                        
                o->hardware_fault = TRUE;    
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

        // No HW fault triggered by this warnings
       /* if (++o->diagnostics_refresh_warning > 5*CTRL_LOOP_FREQUENCY_INT ) {
            if (o->qencoder_err_counter < ERROR_COUNTER_MAX) // we are still below the maximum number of error messages
            {
                if (o->qe_state.bits.dirty) {
                    Motor_send_error(o->ID, eoerror_value_MC_motor_qencoder_dirty, 0);
                    ++o->qencoder_err_counter;
                }
                if (o->qe_state.bits.phase_broken) {
                    Motor_send_error(o->ID, eoerror_value_MC_motor_qencoder_phase, 0);
                    ++o->qencoder_err_counter;
                }
            }
            else // if the maximum number of error messages has been received
            {
                if (!o->qe_state.bits.dirty && !o->qe_state.bits.phase_broken) // if not more errors detected, reset counter
                {
                    o->qencoder_err_counter = 0;
                    Motor_send_diagnostic(eo_errortype_warning, o->ID, eoerror_value_MC_motor_qencoder_phase_disappeared, 0);
                }
            }
            o->diagnostics_refresh_warning = 0; //restart the counter for the warnings
        }
        */
        if (o->qe_state.bits.dirty)
        {
            o->diagnostic.qe_dirty_err_counter ++;
        }
        
        if (o->qe_state.bits.index_broken)
        {
            o->diagnostic.qe_index_broken_err_counter ++;
        }
        
         if (o->qe_state.bits.phase_broken)
        {
            o->diagnostic.qe_phase_broken_err_counter ++;
        }
        
        
        if (++o->diagnostic.total_counter > QEDiagnosticMaxCounter)
        {
            uint64_t mask = o->diagnostic.qe_dirty_err_counter | ((uint64_t)(o->diagnostic.qe_index_broken_err_counter) << 16) | ((uint64_t)(o->diagnostic.qe_phase_broken_err_counter) << 32);
            if(mask != 0)
            {
                Motor_send_error(o->ID, eoerror_value_MC_motor_qencoder_dirty, mask);
            }
            
            //reset diagnostic counter
            o->diagnostic.total_counter = 0;
            o->diagnostic.qe_dirty_err_counter=0;
            o->diagnostic.qe_index_broken_err_counter=0;
            o->diagnostic.qe_phase_broken_err_counter=0;
        }
        
    }

    if (!o->fault_state.bits.ExternalFaultAsserted && !o->hardware_fault)
    {
        o->fault_state_prec.bitmask = 0;
        o->wrong_ctrl_mode = FALSE;
        o->can_dead = FALSE;
           
        return FALSE;
    }
    
    
    // fault or external fault
    
    if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_disable(static_cast<hal_motor_t>(o->motorlocation.adr));
    }
    
    // DIAGNOSTICS MESSAGES
    
    if (++o->diagnostics_refresh > 5*CTRL_LOOP_FREQUENCY_INT)
    {
        o->diagnostics_refresh = 0;
        o->fault_state_prec.bitmask = 0;
        
        o->can_dead = FALSE;
        o->wrong_ctrl_mode = FALSE;
    }
    
    if (o->fault_state_prec.bitmask != o->fault_state.bitmask)
    {
        eOmc_motorFaultState_t fault_state;
        fault_state.bitmask = o->fault_state.bitmask;
        
        if (o->fault_state.bits.ExternalFaultAsserted && !o->fault_state_prec.bits.ExternalFaultAsserted)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_external_fault, 0);
            fault_state.bits.ExternalFaultAsserted = FALSE;
        }
        
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
        
        if (o->fault_state.bits.OverHeatingFailure && !o->fault_state_prec.bits.OverHeatingFailure)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_overheating, (uint64_t)o->temperature_fbk);
            fault_state.bits.OverHeatingFailure = FALSE;
        }
        
//        #define CAN_GENERIC_ERROR 0x00003D00
//        
//        if ((o->fault_state.bitmask & CAN_GENERIC_ERROR) && ((o->fault_state.bitmask & CAN_GENERIC_ERROR) != (o->fault_state_prec.bitmask & CAN_GENERIC_ERROR)))
//        {
//            Motor_send_error(o->ID, eoerror_value_MC_motor_can_generic, (o->fault_state.bitmask & CAN_GENERIC_ERROR));
//            fault_state.bitmask &= ~CAN_GENERIC_ERROR;
//        }
                
        /* 12th July 2024: Valegagge: I commented this check because the bit "EncoderFault" is no longer used by 2foc to signal an encoder error.
        The 2FOC board sets the bit in qe_state when it detects an error in the the qe encoder.
        
        if (o->fault_state.bits.EncoderFault && !o->fault_state_prec.bits.EncoderFault)
        {
            if (o->qe_state.bits.index_broken)
            {
                Motor_send_error(o->ID, eoerror_value_MC_motor_qencoder_index, 0);
            }

            fault_state.bits.EncoderFault = FALSE;
        }
*/
				

        if (can_dead && !o->can_dead)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_can_no_answer, 0);
        }
        
        if (wrong_ctrl_mode && !o->wrong_ctrl_mode)
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_wrong_state, 0);
        }
        
        if (fault_state.bitmask)
        {
            Motor_send_error(o->ID, eoerror_value_MC_generic_error, fault_state.bitmask);
        }
        
        o->fault_state_prec.bitmask = o->fault_state.bitmask;
    }
    
    o->wrong_ctrl_mode = wrong_ctrl_mode;
    o->can_dead = can_dead;
    
    return o->hardware_fault;
}

static void Motor_raise_fault_overcurrent(Motor* o)
{
    hal_motor_disable(static_cast<hal_motor_t>(o->motorlocation.adr));
    
    o->fault_state.bits.OverCurrentFailure = TRUE;
    
    o->control_mode = icubCanProto_controlmode_hwFault;
}

static void Motor_raise_fault_overheating(Motor* o)
{
    hal_motor_disable(static_cast<hal_motor_t>(o->motorlocation.adr));
    
    o->fault_state.bits.OverHeatingFailure = TRUE;
    
    o->control_mode = icubCanProto_controlmode_hwFault;
}

void Motor_raise_fault_i2t(Motor* o)
{
    hal_motor_disable(static_cast<hal_motor_t>(o->motorlocation.adr));
    
    o->fault_state.bits.I2TFailure = TRUE;
    
    o->control_mode = icubCanProto_controlmode_hwFault;
}
/*
void Motor_raise_fault_external(Motor* o)
{
    hal_motor_disable(static_cast<hal_motor_t>(o->motorlocation.adr));
    
    o->fault_state.bits.ExternalFaultAsserted = TRUE;
    
    if (o->control_mode != icubCanProto_controlmode_hwFault)
    {
        o->control_mode = icubCanProto_controlmode_idle;
    }
}
*/
void Motor_motion_reset(Motor *o) //
{
    PID_reset(&o->trqPID);
}

BOOL Motor_is_calibrated(Motor* o) //
{
    return !(o->not_calibrated);
}


CTRL_UNITS Motor_do_trq_control_EXPERIMENTAL(Motor* o, CTRL_UNITS trq_ref, CTRL_UNITS trq_fbk, CTRL_UNITS motor_vel_icubdeg_sec) //
{
    o->trq_ref = trq_ref;
    o->trq_fbk = trq_fbk;
    
    o->trq_err = trq_ref - trq_fbk;
 
//  CTRL_UNITS current = (o->motor[m].GEARBOX > 0)? o->motor[m].Iqq_fbk : -o->motor[m].Iqq_fbk;
//  motor_current_ref = 0.975f*current + (o->joint[m].trq_ref - o->joint[m].trq_fbk)/4500.0f - 0.04f*o->joint[m].vel_fbk;
//  LIMIT(motor_current_ref, 3000.0f);
//  if (o->motor[m].GEARBOX < 0) motor_current_ref = -motor_current_ref;
//  //motor_current_ref = Motor_do_trq_control(o->motor+m, o->joint[m].trq_ref, o->joint[m].trq_fbk, motor_vel_kf_icubdeg_sec);
        
    
    CTRL_UNITS current = (o->GEARBOX > 0)? o->Iqq_fbk : -o->Iqq_fbk;
      
    CTRL_UNITS motor_current_ref = 0.975f*current + o->trqPID.Ktau*(trq_ref - trq_fbk) - 0.0004f*motor_vel_icubdeg_sec;
                
    LIMIT(motor_current_ref, o->trqPID.out_max);
                
    if (o->GEARBOX < 0) motor_current_ref = -motor_current_ref;
    
    return motor_current_ref;
}


CTRL_UNITS Motor_do_trq_control(Motor* o, CTRL_UNITS trq_ref, CTRL_UNITS trq_fbk, CTRL_UNITS motor_vel_icubdeg_sec) //
{
    o->trq_ref = trq_ref;
    o->trq_fbk = trq_fbk;
    
    o->trq_err = trq_ref - trq_fbk;
 
    return PID_do_out(&o->trqPID, o->trq_err) + PID_do_friction_comp(&o->trqPID, motor_vel_icubdeg_sec, o->trq_ref);
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
    
    /*
    if (o->control_mode==icubCanProto_controlmode_hwFault || o->fault_state.bitmask)
    {
        static int cnt[5] = {0,1000,2000,3000,4000};
        
        if (++cnt[o->ID] >= 4000)
        {
            cnt[o->ID] = 0;
            
            send_debug_message("2FOC error state", o->ID, o->control_mode, o->fault_state.bitmask);
        }
    }
    */
}

void Motor_actuate(Motor* motor, uint8_t N, MC_ACTUATION_t act) //
{
    // marco.accame on 07aug2023:
    // in here we decide the type of actuation by comparing motor->HARDWARE_TYPE vs HARDWARE_2FOC etc.
    // we also act so that in some cases we can use ACT_TYPE_2FOC / ACT_TYPE_MC4p / etc.
    
    if(ACT_TYPE_2FOC == act)
    {
        // ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT
        embot::app::eth::mc::messaging::sender::Actuate_Motors msg {};
        for(uint8_t m=0; m<std::min(N, embot::app::eth::mc::messaging::sender::Actuate_Motors::maxnumberofmotors); m++)
        {
            msg.push_back({{&motor[m].motorlocation}, {motor[m].output}});
        }
        msg.transmit();                
    }
    else if (ACT_TYPE_MC4p == act)
    {
        for (uint8_t m=0; m<N; m++)
        {
            motor[m].pwm_fbk = motor[m].output;
            hal_motor_pwmset(static_cast<hal_motor_t>(motor[m].motorlocation.adr), motor[m].output);
        }
    }
}  

// Motor
/////////////////////////////////////////////////////////

void Motor_set_pwm_ref(Motor* o, int32_t pwm_ref)
{
    if (o->pos_min != o->pos_max)
    {        
        if ((o->pos_raw_cal_fbk < o->pos_min) && (pwm_ref < 0))
        {
            o->output = o->pwm_ref = 0;
//            if(!o->outOfLimitsSignaled)
//            {
//                char message [180]; 
//                snprintf(message, sizeof(message),"Motor reached min limit.Can't move with neg pwm" );
//                send_debug_message(message, o->ID, pwm_ref, o->pos_raw_cal_fbk);
//                o->outOfLimitsSignaled = TRUE;
//            }
        }
        else if ((o->pos_raw_cal_fbk > o->pos_max) && (pwm_ref > 0))
        {
            o->output = o->pwm_ref = 0;
//            if(!o->outOfLimitsSignaled)
//            {
//                char message [180]; 
//                snprintf(message, sizeof(message),"Motor reached max limit.Can't move with pos pwm" );
//                send_debug_message(message, o->ID, pwm_ref, o->pos_raw_cal_fbk);
//                o->outOfLimitsSignaled = TRUE;
//            }
        }
        else
        {
            o->output = o->pwm_ref = CUT(pwm_ref, o->pwm_max);
            //o->outOfLimitsSignaled = FALSE;
        }
    }
    else
    {
        o->output = o->pwm_ref = CUT(pwm_ref, o->pwm_max);
        //o->outOfLimitsSignaled = FALSE;
    }
}

void Motor_set_Iqq_ref(Motor* o, int32_t Iqq_ref)
{   
    if (o->pos_min != o->pos_max)
    {        
        if ((o->pos_raw_cal_fbk < o->pos_min) && (Iqq_ref < 0))
        {
            o->output = o->Iqq_ref = 0;
        }
        else if ((o->pos_raw_cal_fbk > o->pos_max) && (Iqq_ref > 0))
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
        if ((o->pos_raw_cal_fbk < o->pos_min) && (vel_ref < 0))
        {
            o->output = o->vel_ref = 0;
        }
        else if ((o->pos_raw_cal_fbk > o->pos_max) && (vel_ref > 0))
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
    motor_status->basic.mot_position = o->pos_raw_cal_fbk;
    motor_status->basic.mot_velocity = o->vel_raw_fbk;

    //motor_status->basic.mot_position = o->pos_fbk;
    //motor_status->basic.mot_velocity = o->vel_fbk;

    motor_status->basic.mot_acceleration = 0; // not implemented  
    
    motor_status->basic.mot_current  = o->Iqq_fbk; //o->Iqq_peak_fbk;    
    motor_status->basic.mot_pwm      = o->pwm_fbk;
    
    if (Motor_is_motor_joint_fault_over(o))
    {
        motor_status->mc_fault_state = eoerror_code_dummy;
    }
}

void Motor_update_odometry_fbk_can(Motor* o, CanOdometry2FocMsg* can_msg) //
{
    WatchDog_rearm(&o->can_2FOC_alive_wdog);
    
    o->Iqq_fbk = can_msg->current;
    
    o->vel_raw_fbk = can_msg->velocity*1000;
    o->vel_fbk = o->vel_raw_fbk/o->GEARBOX;
    
    o->pos_raw_fbk = can_msg->position;
    o->pos_fbk = o->pos_raw_fbk/o->GEARBOX - o->pos_calib_offset;
    o->pos_raw_cal_fbk = o->pos_raw_fbk - o->pos_calib_offset*o->GEARBOX;
    
    if (o->sensorless_torque)
    {
        static const float IDEG2RAD = 6.28f/65536.0f;
    
        o->torque_estimator.rtU.Current  = 0.001f* o->Iqq_fbk;
        o->torque_estimator.rtU.Velocity = IDEG2RAD*o->vel_raw_fbk;

        if (o->GEARBOX < 0)
        {
            o->torque_estimator.rtU.Current      = -o->torque_estimator.rtU.Current;
            o->torque_estimator.rtU.Velocity     = -o->torque_estimator.rtU.Velocity;
        }
    
        o->torque_estimator.step();
    
        o->torque = o->torque_estimator.rtY.Torque;
    }
}

void Motor_update_pos_fbk(Motor* o, int32_t position_raw)
{
    //if (o->HARDWARE_TYPE == HARDWARE_2FOC) return; removed workaround
    //Note: this function should be call only to update motor position read by a connected encoder, ie 
    //the encoder is read by this board and not by 2foc.
    //in case of joint controlled by ems+2foc, the motor encoder is connected to 2foc and the 2foc sends motor position to ems by can message. 
    //see Motor_update_odometry_fbk_can function.

    int32_t delta = position_raw - o->pos_raw_fbk;
    
    while (delta < -TICKS_PER_HALF_REVOLUTION) delta += TICKS_PER_REVOLUTION;
    while (delta >  TICKS_PER_HALF_REVOLUTION) delta -= TICKS_PER_REVOLUTION;
    
    o->pos_raw_fbk += delta;
    
    int32_t pos_fbk = o->pos_raw_fbk/o->GEARBOX - o->pos_calib_offset;
      
    //if (o->not_init)
    if (o->pos_fbk_old == 0)
    {
        o->not_init = FALSE;
        o->pos_fbk_old = pos_fbk;
    }
    
    //direction of movement changes depending on the sign
    delta = o->enc_sign * (pos_fbk - o->pos_fbk_old);
    
    o->pos_fbk_old = pos_fbk;
    
    //normalize delta to avoid discontinuities
    while (delta < -TICKS_PER_HALF_REVOLUTION) delta += TICKS_PER_REVOLUTION;
    while (delta >  TICKS_PER_HALF_REVOLUTION) delta -= TICKS_PER_REVOLUTION;
        
    o->pos_fbk += delta;
    
    //update velocity
    o->vel_fbk = delta*CTRL_LOOP_FREQUENCY_INT;
    o->vel_raw_fbk = o->vel_fbk*o->GEARBOX;
    o->pos_raw_cal_fbk = o->pos_fbk*o->GEARBOX;
}

void Motor_update_current_fbk(Motor* o, int16_t current)
{
    if (abs(o->Iqq_fbk + current)/2 > o->Iqq_ovl)
    {
        Motor_raise_fault_overcurrent(o);
    }
    
    o->Iqq_fbk = current;
}

void Motor_update_temperature_fbk(Motor* o, int16_t temperature)
{   
    o->temperature_fbk = temperature;
}

void Motor_config_gearbox_M2J(Motor* o, float32_t gearbox_M2J)
{
    o->GEARBOX = gearbox_M2J;
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
    
    return o->pwm_max;
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

BOOL Motor_is_in_fault(Motor *o)
{
    return(o->hardware_fault || Motor_is_external_fault(o));
}

BOOL Motor_is_running(Motor* o)
{
    switch (o->control_mode)
    {
        case icubCanProto_controlmode_openloop:
        case icubCanProto_controlmode_speed_voltage:
        case icubCanProto_controlmode_current:
            return TRUE;
        
        default: 
            return FALSE;
    }
}

void Motor_reset(Motor *o)
{
    o->pwm_fbk=ZERO;
    o->pwm_ref=ZERO;

    o->pos_raw_cal_fbk=ZERO;
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
    o->not_init = TRUE;

    //o->control_mode = ???
    //o->control_mode_req;
}


BOOL Motor_is_motor_joint_fault_over(Motor* o)
{
    BOOL ret = TRUE;
    
    ret &= !Motor_is_in_fault(o);
    
    eOmc_joint_status_t* jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), o->ID);    
    
    if (NULL != jstatus)
    {
        ret &= (jstatus->core.modes.controlmodestatus != eomc_ctrlmval_hwFault);
    }
        
    return ret;
}


// eof

