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

//#include "stdlib.h"
//#include <string.h>

#include "Motor_hid.h"

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
#include "EOtheEntities.h"

#include <math.h>

#if defined(EOTHESERVICES_customize_handV3_7joints)

// -- here is new code for the pmc board

static void Motor_stop_PMC(Motor *o);
static void Motor_config_PMC(Motor* o, eOmc_motor_config_t* config);
static void Motor_set_control_mode_PMC(Motor *o, icubCanProto_controlmode_t control_mode);
void Motor_config_encoder_PMC(Motor* o, int32_t resolution);
static void pmc_motor_disable(eObrd_canlocation_t canloc);

#endif


/////////////////////////////////////////////////////////
// Motor

static void send_debug_message(char *message, uint8_t jid, uint16_t par16, uint64_t par64)
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
    
    ((int16_t*)KpKiKdKs)[0] = Kp;
    ((int16_t*)KpKiKdKs)[1] = Ki;
    ((int16_t*)KpKiKdKs)[2] = Kd; //(unused in 2FOC)
               KpKiKdKs [6] = Ks; // shift
    
    send_debug_message("CURRENT PID", o->ID, Ks, (((uint64_t)Kp)<<32) | Ki);
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, o->ID, 0);
    
    eOcanprot_command_t cmdPid;
    cmdPid.clas = eocanprot_msgclass_pollingMotorControl;
    cmdPid.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    cmdPid.value = KpKiKdKs;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdPid, id32);
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
    
    ((int16_t*)KpKiKdKs)[0] = Kp;
    ((int16_t*)KpKiKdKs)[1] = Ki;
    ((int16_t*)KpKiKdKs)[2] = Kd; //(unused in 2FOC)
               KpKiKdKs [6] = Ks; // shift
        
    send_debug_message("VELOCITY PID", o->ID, Ks, (((uint64_t)Kp)<<32) | Ki);
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, o->ID, 0);
    
    eOcanprot_command_t cmdPid;
    cmdPid.clas = eocanprot_msgclass_pollingMotorControl;
    cmdPid.type = ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID;
    cmdPid.value = KpKiKdKs;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdPid, id32);
}

static void Motor_config_max_currents_2FOC(Motor* o, eOmc_current_limits_params_t* current_params)
{    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, o->ID, 0);
    
    uint32_t max_current = current_params->peakCurrent;
    
    eOcanprot_command_t cmdMaxCurrent;
    cmdMaxCurrent.clas = eocanprot_msgclass_pollingMotorControl;
    cmdMaxCurrent.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    cmdMaxCurrent.value = &max_current;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdMaxCurrent, id32);
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

    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, o->ID, 0);
    
    Motor_config_current_PID_2FOC(o, &(config->pidcurrent));
    Motor_config_velocity_PID_2FOC(o, &(config->pidspeed));
        
    eOcanprot_command_t cmdMaxCurrent;
    cmdMaxCurrent.clas = eocanprot_msgclass_pollingMotorControl;
    cmdMaxCurrent.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    cmdMaxCurrent.value = &(config->currentLimits);
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdMaxCurrent, id32);

    eOcanprot_command_t cmdMotorConfig;
    cmdMotorConfig.clas = eocanprot_msgclass_pollingMotorControl;
    cmdMotorConfig.type = ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG;
    cmdMotorConfig.value = o->can_motor_config;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdMotorConfig, id32);
}

static void Motor_set_control_mode_2FOC(uint8_t motor, icubCanProto_controlmode_t control_mode)
{
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.clas = eocanprot_msgclass_pollingMotorControl;
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

static void torque_calib_init(Motor* o);

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
    o->diagnostics_refresh = 0;
    o->diagnostics_refresh_warning = 0;
    o->qencoder_err_counter = 0;
    
    WatchDog_init(&o->control_mode_req_wdog);
    WatchDog_init(&o->can_2FOC_alive_wdog);
    
    Motor_hardStopCalbData_reset(o);
    
    //o->outOfLimitsSignaled = FALSE;
    
#ifdef ERGOJOINT
    o->torque_model.initialize();

    o->joint_position_raw = 0;
    //o->invalid_torque_measure = 0;
    //o->invalid_torque_measure_cnt = 0;
    //o->spikes = 0;
    
    o->offmax = -32768;
    o->offmin =  32767;
    o->torsion_raw = 0;
    o->torsion = 0;
    
    torque_calib_init(o);
#endif
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
        hal_motor_disable((hal_motor_t)o->actuatorPort);
    }    
#if defined(EOTHESERVICES_customize_handV3_7joints)   
    else if (o->HARDWARE_TYPE == HARDWARE_PMC)
    {      
        // marco.accame on 08 jan 2021.
        // this motor is on the pmc board. so far i treat it as if it were on the 2foc
        Motor_config_PMC(o, config);
        
        // marco.accame on 08 jan 2021. so far i dont use teh watchdog, hence ...
        // i just do what Motor_new_state_req(o, icubCanProto_controlmode_idle); does but w/out watchdog
        o->control_mode = icubCanProto_controlmode_idle;
        
        // WatchDog_set_base_time_msec(&o->can_2FOC_alive_wdog, CAN_ALIVE_TIMEOUT);
        // WatchDog_rearm(&o->can_2FOC_alive_wdog);    
        // WatchDog_set_base_time_msec(&o->control_mode_req_wdog, CTRL_REQ_TIMEOUT);        
        //Motor_new_state_req(o, icubCanProto_controlmode_idle);
    }
#endif         
}

void Motor_config_encoder(Motor* o, int32_t resolution)
{
    o->enc_sign = resolution >= 0 ? 1 : -1;
    
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        *(int16_t*)(o->can_motor_config+1) = resolution;
        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, o->ID, 0);
        
        eOcanprot_command_t cmdMotorConfig;
        cmdMotorConfig.clas = eocanprot_msgclass_pollingMotorControl;
        cmdMotorConfig.type = ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG;
        cmdMotorConfig.value = o->can_motor_config;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdMotorConfig, id32);  
    }
#if defined(EOTHESERVICES_customize_handV3_7joints)    
    else if(o->HARDWARE_TYPE == HARDWARE_PMC)
    {
        Motor_config_encoder_PMC(o, resolution);      
    }
#endif      
}

void Motor_config_max_currents(Motor* o, eOmc_current_limits_params_t* current_params)
{
    o->Iqq_ovl = current_params->overloadCurrent;
    
    if (o->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        Motor_config_max_currents_2FOC(o, current_params);
    }
#if defined(EOTHESERVICES_customize_handV3_7joints)       
    else if(o->HARDWARE_TYPE == HARDWARE_PMC)
    {
        //Motor_config_currents_PMC(o, current_params);
        // nothing so far
    }
#endif    
}

//extern void Motor_config_current_PID(Motor* o, eOmc_PID_t* pid);
//extern void Motor_config_torque_PID(Motor* o, eOmc_PID_t* pid);
//extern void Motor_config_speed_PID(Motor* o, eOmc_PID_t* pid);

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
#if defined(EOTHESERVICES_customize_handV3_7joints)       
    else if(o->HARDWARE_TYPE == HARDWARE_PMC)
    {   
        o->not_calibrated = FALSE;
    }
#endif    
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
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, o->ID, 0);
        eOcanprot_command_t command = {0};
        command.clas = eocanprot_msgclass_pollingMotorControl;
        command.type  = ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    }
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
        
//        //debug code
//        char message[150];
//        snprintf(message, sizeof(message), "Hw lim reached: cp%d cz%d co%d", o->pos_fbk, o->hardstop_calibdata.zero, o->pos_calib_offset);
//        send_debug_message(message, o->ID, 0, 0);
        
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
        Motor_set_control_mode_2FOC(o->ID, control_mode);
        Motor_new_state_req(o, control_mode);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_reenable_break_interrupts();
        
        hal_motor_enable((hal_motor_t)o->actuatorPort);

        o->control_mode = control_mode;
    }
#if defined(EOTHESERVICES_customize_handV3_7joints)      
    else if (o->HARDWARE_TYPE == HARDWARE_PMC)
    {      
        Motor_set_control_mode_PMC(o, control_mode);      
        o->control_mode = control_mode;
        // so far we dont manage any watchdog
        // Motor_new_state_req(o, control_mode);
    }
#endif      
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
        Motor_set_control_mode_2FOC(o->ID, icubCanProto_controlmode_idle);
        Motor_new_state_req(o, icubCanProto_controlmode_idle);
    }
    else if (o->HARDWARE_TYPE == HARDWARE_MC4p)
    {
        hal_motor_disable((hal_motor_t)o->actuatorPort);
        
        if (o->control_mode != icubCanProto_controlmode_hwFault)
        {
            o->control_mode = icubCanProto_controlmode_idle;
        }
    }
#if defined(EOTHESERVICES_customize_handV3_7joints)     
    else if (o->HARDWARE_TYPE == HARDWARE_PMC)
    {
       
        Motor_set_control_mode_PMC(o, icubCanProto_controlmode_idle);
      
        if (o->control_mode != icubCanProto_controlmode_hwFault)
        {
            o->control_mode = icubCanProto_controlmode_idle;
        }
        //Motor_new_state_req(o, icubCanProto_controlmode_idle);
    }
#endif  
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
        hal_motor_disable((hal_motor_t)o->actuatorPort);
        
        o->control_mode = icubCanProto_controlmode_idle;
    }
#if defined(EOTHESERVICES_customize_handV3_7joints)      
    else if (o->HARDWARE_TYPE == HARDWARE_PMC)
    {      
        Motor_set_control_mode_PMC(o, icubCanProto_controlmode_forceIdle);    
        o->control_mode = icubCanProto_controlmode_idle;
    }
#endif   
    
    o->can_dead = FALSE;
    o->wrong_ctrl_mode = FALSE;
    
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
    o->diagnostics_refresh = 0;
    o->diagnostics_refresh_warning = 0;
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
#if defined(EOTHESERVICES_customize_handV3_7joints)       
    else if(o->HARDWARE_TYPE == HARDWARE_PMC)
    {   
        // we retrieve the external fault from the mc4plus
        o->fault_state.bits.ExternalFaultAsserted = hal_motor_externalfaulted();
    }
#endif
    
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
        if (++o->diagnostics_refresh_warning > 5*CTRL_LOOP_FREQUENCY_INT ) {
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
        hal_motor_disable((hal_motor_t)o->actuatorPort);
    }
#if defined(EOTHESERVICES_customize_handV3_7joints)      
    else if (o->HARDWARE_TYPE == HARDWARE_PMC)
    {      
        pmc_motor_disable(o->canloc);       
    }
#endif 
    
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
        MotorFaultState fault_state;
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
        
        #define CAN_GENERIC_ERROR 0x00003D00
        
        if ((o->fault_state.bitmask & CAN_GENERIC_ERROR) && ((o->fault_state.bitmask & CAN_GENERIC_ERROR) != (o->fault_state_prec.bitmask & CAN_GENERIC_ERROR)))
        {
            Motor_send_error(o->ID, eoerror_value_MC_motor_can_generic, (o->fault_state.bitmask & CAN_GENERIC_ERROR));
            fault_state.bitmask &= ~CAN_GENERIC_ERROR;
        }
                
        if (o->fault_state.bits.EncoderFault && !o->fault_state_prec.bits.EncoderFault)
        {
            if (o->qe_state.bits.index_broken)
            {
                Motor_send_error(o->ID, eoerror_value_MC_motor_qencoder_index, 0);
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
    hal_motor_disable((hal_motor_t)o->actuatorPort);
    
    o->fault_state.bits.OverCurrentFailure = TRUE;
    
    o->control_mode = icubCanProto_controlmode_hwFault;
}

void Motor_raise_fault_i2t(Motor* o)
{
    hal_motor_disable((hal_motor_t)o->actuatorPort);
    
    o->fault_state.bits.I2TFailure = TRUE;
    
    o->control_mode = icubCanProto_controlmode_hwFault;
}
/*
void Motor_raise_fault_external(Motor* o)
{
    hal_motor_disable((hal_motor_t)o->actuatorPort);
    
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

#if !defined(ERGOJOINT)

CTRL_UNITS Motor_do_trq_control(Motor* o, CTRL_UNITS trq_ref, CTRL_UNITS trq_fbk) //
{
    o->trq_ref = trq_ref;
    o->trq_fbk = trq_fbk;
    
    o->trq_err = trq_ref - trq_fbk;
    
    return PID_do_out(&o->trqPID, o->trq_err) + PID_do_friction_comp(&o->trqPID, o->vel_raw_fbk, o->trq_ref);
}

#else

#define FABS(x) ((x)>ZERO?(x):(-x))
#define FSGN(x) ((x)>ZERO ? 1.f : ((x)<ZERO? -1.f : ZERO))

CTRL_UNITS Motor_do_trq_control(Motor* o, CTRL_UNITS trq_ref, CTRL_UNITS trq_fbk) //
{
    trq_fbk = o->torque_model.rtY.Torque; // overwrites with torsion model output
    
    o->trq_fbk = trq_fbk;
    
    o->trq_ref = trq_ref;
    
    o->trq_err = trq_ref - trq_fbk;
          
    trq_ref *= 1.0e-6f; // [uNm] -> [Nm]
    trq_fbk *= 1.0e-6f; // [uNm] -> [Nm]
    
    constexpr CTRL_UNITS ENC2RAD = 3.1415926f/32768.0f;
    
    CTRL_UNITS omega = ENC2RAD*o->vel_fbk; // -> [rad/s]
    
    CTRL_UNITS omega_sgn = FSGN(omega);
    
    CTRL_UNITS Ifriction = o->trq_ctrl.Ko*omega_sgn + o->trq_ctrl.Kw*omega;
    
    LIMIT(Ifriction, o->trq_ctrl.Ifriction_max);
        
    CTRL_UNITS Iquadratic = o->trq_ctrl.Kw2*omega*omega*omega_sgn;
    
    LIMIT(Iquadratic, o->trq_ctrl.Iquadratic_max);
    
    CTRL_UNITS Iff = o->trq_ctrl.Kff*trq_ref;
    
    LIMIT(Iff, o->trq_ctrl.Iff_max);
    
    CTRL_UNITS Iproportional = o->trq_ctrl.Kp*(trq_ref-trq_fbk);
    
    LIMIT(Iproportional, o->trq_ctrl.Iproportional_max);
    LIMIT(Iproportional, Iff);
    
    CTRL_UNITS Iout = Ifriction + Iquadratic + Iff + Iproportional;
    
    LIMIT(Iout, o->trq_ctrl.Iout_max);
    
    Iout *= 1000.0f; // [mA]
    
    return Iout;
}

#endif

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

#if !defined(EOTHESERVICES_customize_handV3_7joints)
void Motor_actuate(Motor* motor, uint8_t N) //
{
    if (motor->HARDWARE_TYPE == HARDWARE_2FOC)
    {
        
        int16_t output[MAX_JOINTS_PER_BOARD] = {0};
    
        for (int m=0; m<N; ++m)
        {
            output[motor[m].actuatorPort] = motor[m].output;
        }
                
//        static int turn = 0;
//        
//        if (++turn > 4000) turn = 0;
//        
//        if (turn < 2000)
//        {
//            ((int16_t*)output)[1] = motor[0].angmin;
//            ((int16_t*)output)[2] = motor[0].offmin;
//            ((int16_t*)output)[3] = motor[0].angle;
//        }
//        else
//        {
//            ((int16_t*)output)[1] = motor[0].angmax;
//            ((int16_t*)output)[2] = motor[0].offmax;
//            ((int16_t*)output)[3] = motor[0].angle;
//        }
        
//        ((int16_t*)output)[1] = motor[0].pos_raw_fbk;
//        ((int16_t*)output)[2] = motor[0].joint_position_raw>>3;
//        ((int16_t*)output)[3] = motor[0].torsion;
    
//        ((int16_t*)output)[1] = motor[0].invalid_torque_measure;
//        ((int16_t*)output)[2] = motor[0].spikes;
        
        eOcanprot_command_t command = {0};
        command.clas = eocanprot_msgclass_periodicMotorControl;    
        command.type  = ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT;
        command.value = output;
    
        eObrd_canlocation_t location = {0};
        location.port = eOcanport1;
        location.addr = 0;
        location.insideindex = eobrd_caninsideindex_first; // because all 2foc have motor on index-0. 

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
#else
void Motor_actuate(MC_ACTUATION_t act, Motor* motor, uint8_t N) //
{
    if(act == ACT_TYPE_MC4pPMC)
    {
        // first 4 motors are local and are managed by mc4plus board
        for (int m=0; m<4; ++m)
        {
            motor[m].pwm_fbk = motor[m].output;
            hal_motor_pwmset((hal_motor_t)motor[m].actuatorPort, motor[m].output);
        }
        // last 3 motors are on pmc and are managed by can messages
        // so far i use the ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT
        int16_t output[3];
    
        for (int m=0; m<3; ++m)
        {
            output[m] = motor[m+4].output;
        }
    
        eOcanprot_command_t command = {0};
        command.clas = eocanprot_msgclass_periodicMotorControl;    
        command.type  = ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT;
        command.value = output;
    
        eObrd_canlocation_t location = {0};
        location.port = eOcanport1;
        location.addr = 0xf;
        location.insideindex = eobrd_caninsideindex_first; // because all 2foc have motor on index-0. 

        // and i send the command
        eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, location);         
    }
    else
    {
        if (motor->HARDWARE_TYPE == HARDWARE_2FOC)
        {
            int16_t output[MAX_JOINTS_PER_BOARD];
        
            for (int m=0; m<N; ++m)
            {
                output[motor[m].actuatorPort] = motor[m].output;
                //output[m] = motor[m].output;
            }
        
            eOcanprot_command_t command = {0};
            command.clas = eocanprot_msgclass_periodicMotorControl;    
            command.type  = ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT;
            command.value = output;
        
            eObrd_canlocation_t location = {0};
            location.port = eOcanport1;
            location.addr = 0;
            location.insideindex = eobrd_caninsideindex_first; // because all 2foc have motor on index-0. 

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
}  
#endif

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
#if defined(EOTHESERVICES_customize_handV3_7joints)       
    else if (o->HARDWARE_TYPE == HARDWARE_PMC)
    {
        if (pwm_limit > 3360) pwm_limit = 3360; 
    }  
#endif    
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
#if defined(EOTHESERVICES_customize_handV3_7joints)       
    else if (o->HARDWARE_TYPE == HARDWARE_PMC)
    {
        return FALSE;
    } 
#endif    
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

#ifdef ERGOJOINT

void Motor_calc_torque_invalid(Motor* o, int32_t joint_encoder)
{
//    o->invalid_torque_measure++;
//    
//    if (++o->invalid_torque_measure_cnt > 1000)
//    {
//        o->invalid_torque_measure = 0;
//        o->invalid_torque_measure_cnt = 0;
//        o->spikes = 0;
//    }
}

CTRL_UNITS Motor_calc_torque(Motor* o, int32_t joint_encoder)
{   
    const int32_t FULL_REV = o->joint_full_res > 0 ? o->joint_full_res : -o->joint_full_res;
    const int32_t HALF_REV = FULL_REV>>1;
    const int16_t DIVISOR = FULL_REV>>16;
    
    joint_encoder = joint_encoder & 0x7FFFF;
    
    if (o->joint_full_res < 0)
    {
        joint_encoder = FULL_REV-1 - joint_encoder; // old setup
    }
    
    //static int32_t motor_offset = 0;
    
    static int cnt = 0;
    
    if (cnt < 4)
    {
        if (o->joint_encoder_last != joint_encoder) cnt = 0;

        o->joint_encoder_last = joint_encoder;
        o->joint_encoder_sure = joint_encoder;
        o->joint_position_raw = joint_encoder;
        
        if (++cnt < 4) return ZERO;
    }
    
    int32_t joint_check = joint_encoder - o->joint_encoder_last;
    o->joint_encoder_last  = joint_encoder;

    while (joint_check >  HALF_REV) joint_check -= FULL_REV;
    while (joint_check < -HALF_REV) joint_check += FULL_REV;
    
    joint_check /=DIVISOR;
    
    BOOL good = FALSE;
    
    if (-16 < joint_check && joint_check < 16) 
    {
        int32_t joint_speed = joint_check*1000;
        
        if (o->vel_fbk-250 <= joint_speed && joint_speed <= o->vel_fbk+250)
        {
            int32_t joint_delta = joint_encoder - o->joint_encoder_sure;

            o->joint_encoder_sure = joint_encoder;
            
            while (joint_delta >  HALF_REV) joint_delta -= FULL_REV;
            while (joint_delta < -HALF_REV) joint_delta += FULL_REV;
        
            o->joint_position_raw += joint_delta;
            
            good = TRUE;
        }
    }
    
    static constexpr float IDEG2RAD = 3.1415926f/32768.0f;
    
    //#define CALIBRATION
    
    if (good)
    {
        uint32_t anglemod = o->joint_position_raw;
        uint8_t angle256 = (anglemod+DIVISOR*128)/(DIVISOR*256);
        
        o->torsion_raw = ((int16_t)o->GEARBOX/DIVISOR)*o->joint_position_raw - o->pos_raw_fbk;
   
        #ifdef CALIBRATION
        o->torsion = o->torsion_raw; // - o->torsion_offset;
        #else
        o->torsion = o->torsion_raw - o->torsion_offset;
        o->torsion -= (int16_t)o->torsion_fine_tuning[angle256];
        #endif
        
        //////////////////////////////////////////////////////////
        o->torque_model.rtU.Torsion = IDEG2RAD*(float)o->torsion/o->GEARBOX;
    
        o->torque_model.step();
    
        o->trq_fbk = 1000000.0f*o->torque_model.rtY.Torque; // uNm
        //////////////////////////////////////////////////////////
        #ifdef CALIBRATION
        if (o->joint_position_raw > 2*FULL_REV || o->joint_position_raw < -2*FULL_REV)
        {
            o->angle = o->joint_position_raw/DIVISOR;
                
            if (o->torsion_raw > o->offmax)
            {
                o->offmax = o->torsion_raw;
                o->angmax = o->angle;
            }
        
            if (o->torsion_raw < o->offmin)
            {
                o->offmin = o->torsion_raw;
                o->angmin = o->angle;
            }
            
            o->torsion_offset = (o->offmin+o->offmax)/2;
            
            ////////////////////////////////////////////

            static uint8_t angle256_old = angle256;
            static int32_t torsion_avg = 0;
            static int16_t cnt = 0;
         
            if (angle256 != angle256_old)
            {
                int32_t correction = 0;
            
                if (cnt!=0) correction = torsion_avg/cnt;
            
                // send message angle256_old,correction
            
                eOerrmanDescriptor_t errdes = {0};

                errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
                errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress    = 0;
                errdes.par16            = angle256_old;
                errdes.par64            = correction - o->torsion_offset;
                if (angle256_old == 0)
                {
                    errdes.par16 = o->torsion_offset;
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, "<<OFFSET", NULL, &errdes);
                }
                else
                {
                    errdes.par16 = angle256_old;
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, "<<TORSION", NULL, &errdes);
                }
                
                
                angle256_old = angle256;
            
                torsion_avg = o->torsion;
            
                cnt = 1;
            }
            else
            {
                torsion_avg += o->torsion;
            
                ++cnt;
            }
        }
        #endif
    }
    
    return o->trq_fbk; 
}

#define NEW_JOINT

// temporary solution until the data aren't transmitted
// by protocol from yarprobotinterface at startup
static void torque_calib_init(Motor* o)
{
    if (o->ID==0)
    { 
        #ifdef NEW_JOINT
        
        CTRL_UNITS omega_max = 2.0f; // [rad/s]
        
        o->trq_ctrl.Ko = 0.8f; // Static friction coefficient  [A]
        o->trq_ctrl.Kw = 1.8f; // Viscous friction coefficient [A/(rad/s)]
        o->trq_ctrl.Ifriction_max = o->trq_ctrl.Ko + o->trq_ctrl.Kw*omega_max; // Limits the two contributions above [A] 
    
        o->trq_ctrl.Kw2 = 0.0f; // Quadratic term for non linear characteristic [A/(rad/s)^2]
                            // It can also be negative to prevent runaway.
        o->trq_ctrl.Iquadratic_max = o->trq_ctrl.Kw2*omega_max*omega_max; // Limits the quadratic contribution above [A]
    
        o->trq_ctrl.Kff = 1/FABS((0.058f*o->GEARBOX*0.85f)); // Feed forward term, equal to 1/(torque constant * gearbox ratio * efficiency) [A/Nm]
        o->trq_ctrl.Iff_max = o->trq_ctrl.Kff * 50.0f; // Limits the feed forward contribution above [A]
    
        o->trq_ctrl.Kp = 5.0f * o->trq_ctrl.Kff; // Proportional term [A/Nm]
        o->trq_ctrl.Iproportional_max = o->trq_ctrl.Iff_max/10.0f; // Limits the proportional contribution above [A]
    
        o->trq_ctrl.Iout_max = 10.0f; // Limits the final output [A] 
        
                
        o->torsion_offset = 0x3d16;
        
        o->joint_full_res = 524288;
        
        o->torsion_fine_tuning[0x00] = 0x0670;
        o->torsion_fine_tuning[0x01] = 0x0624;
        o->torsion_fine_tuning[0x02] = 0x05dc;
        o->torsion_fine_tuning[0x03] = 0x05c1;
        o->torsion_fine_tuning[0x04] = 0x059b;
        o->torsion_fine_tuning[0x05] = 0x0519;
        o->torsion_fine_tuning[0x06] = 0x04c4;
        o->torsion_fine_tuning[0x07] = 0x04b1;
        o->torsion_fine_tuning[0x08] = 0x0439;
        o->torsion_fine_tuning[0x09] = 0x03ae;
        o->torsion_fine_tuning[0x0a] = 0x0395;
        o->torsion_fine_tuning[0x0b] = 0x03db;
        o->torsion_fine_tuning[0x0c] = 0x03d0;
        o->torsion_fine_tuning[0x0d] = 0x03bc;
        o->torsion_fine_tuning[0x0e] = 0x0345;
        o->torsion_fine_tuning[0x0f] = 0x02ed;
        o->torsion_fine_tuning[0x10] = 0x0221;
        o->torsion_fine_tuning[0x11] = 0x01ad;
        o->torsion_fine_tuning[0x12] = 0x0147;
        o->torsion_fine_tuning[0x13] = 0x00cf;
        o->torsion_fine_tuning[0x14] = 0x008a;
        o->torsion_fine_tuning[0x15] = 0x008e;
        o->torsion_fine_tuning[0x16] = 0x0047;
        o->torsion_fine_tuning[0x17] = 0x001f;
        o->torsion_fine_tuning[0x18] = 0xffb0;
        o->torsion_fine_tuning[0x19] = 0xffa5;
        o->torsion_fine_tuning[0x1a] = 0xff66;
        o->torsion_fine_tuning[0x1b] = 0xfed9;
        o->torsion_fine_tuning[0x1c] = 0xfe8a;
        o->torsion_fine_tuning[0x1d] = 0xff09;
        o->torsion_fine_tuning[0x1e] = 0xfe5c;
        o->torsion_fine_tuning[0x1f] = 0xfe50;
        o->torsion_fine_tuning[0x20] = 0xfde0;
        o->torsion_fine_tuning[0x21] = 0xfd1b;
        o->torsion_fine_tuning[0x22] = 0xfd63;
        o->torsion_fine_tuning[0x23] = 0xfd1e;
        o->torsion_fine_tuning[0x24] = 0xfd38;
        o->torsion_fine_tuning[0x25] = 0xfd41;
        o->torsion_fine_tuning[0x26] = 0xfd47;
        o->torsion_fine_tuning[0x27] = 0xfd64;
        o->torsion_fine_tuning[0x28] = 0xfcd9;
        o->torsion_fine_tuning[0x29] = 0xfcf8;
        o->torsion_fine_tuning[0x2a] = 0xfca5;
        o->torsion_fine_tuning[0x2b] = 0xfc21;
        o->torsion_fine_tuning[0x2c] = 0xfb9a;
        o->torsion_fine_tuning[0x2d] = 0xfb98;
        o->torsion_fine_tuning[0x2e] = 0xfba7;
        o->torsion_fine_tuning[0x2f] = 0xfb65;
        o->torsion_fine_tuning[0x30] = 0xfb63;
        o->torsion_fine_tuning[0x31] = 0xfb05;
        o->torsion_fine_tuning[0x32] = 0xfb4f;
        o->torsion_fine_tuning[0x33] = 0xfa8a;
        o->torsion_fine_tuning[0x34] = 0xfa16;
        o->torsion_fine_tuning[0x35] = 0xfa25;
        o->torsion_fine_tuning[0x36] = 0xfa48;
        o->torsion_fine_tuning[0x37] = 0xfa18;
        o->torsion_fine_tuning[0x38] = 0xf9c1;
        o->torsion_fine_tuning[0x39] = 0xf9eb;
        o->torsion_fine_tuning[0x3a] = 0xf9eb;
        o->torsion_fine_tuning[0x3b] = 0xf9a9;
        o->torsion_fine_tuning[0x3c] = 0xf9b3;
        o->torsion_fine_tuning[0x3d] = 0xf97a;
        o->torsion_fine_tuning[0x3e] = 0xf991;
        o->torsion_fine_tuning[0x3f] = 0xf98b;
        o->torsion_fine_tuning[0x40] = 0xf9fd;
        o->torsion_fine_tuning[0x41] = 0xf9c8;
        o->torsion_fine_tuning[0x42] = 0xf9a8;
        o->torsion_fine_tuning[0x43] = 0xf990;
        o->torsion_fine_tuning[0x44] = 0xf936;
        o->torsion_fine_tuning[0x45] = 0xf8e1;
        o->torsion_fine_tuning[0x46] = 0xf8c7;
        o->torsion_fine_tuning[0x47] = 0xf8ba;
        o->torsion_fine_tuning[0x48] = 0xf875;
        o->torsion_fine_tuning[0x49] = 0xf847;
        o->torsion_fine_tuning[0x4a] = 0xf81f;
        o->torsion_fine_tuning[0x4b] = 0xf7ac;
        o->torsion_fine_tuning[0x4c] = 0xf74a;
        o->torsion_fine_tuning[0x4d] = 0xf6b8;
        o->torsion_fine_tuning[0x4e] = 0xf68e;
        o->torsion_fine_tuning[0x4f] = 0xf607;
        o->torsion_fine_tuning[0x50] = 0xf57a;
        o->torsion_fine_tuning[0x51] = 0xf53a;
        o->torsion_fine_tuning[0x52] = 0xf58f;
        o->torsion_fine_tuning[0x53] = 0xf59c;
        o->torsion_fine_tuning[0x54] = 0xf5ff;
        o->torsion_fine_tuning[0x55] = 0xf663;
        o->torsion_fine_tuning[0x56] = 0xf6d6;
        o->torsion_fine_tuning[0x57] = 0xf7bd;
        o->torsion_fine_tuning[0x58] = 0xf812;
        o->torsion_fine_tuning[0x59] = 0xf892;
        o->torsion_fine_tuning[0x5a] = 0xf8a8;
        o->torsion_fine_tuning[0x5b] = 0xf89f;
        o->torsion_fine_tuning[0x5c] = 0xf8a3;
        o->torsion_fine_tuning[0x5d] = 0xf8a4;
        o->torsion_fine_tuning[0x5e] = 0xf845;
        o->torsion_fine_tuning[0x5f] = 0xf881;
        o->torsion_fine_tuning[0x60] = 0xf8fa;
        o->torsion_fine_tuning[0x61] = 0xf8f1;
        o->torsion_fine_tuning[0x62] = 0xf8a2;
        o->torsion_fine_tuning[0x63] = 0xf97e;
        o->torsion_fine_tuning[0x64] = 0xf9f7;
        o->torsion_fine_tuning[0x65] = 0xf9a8;
        o->torsion_fine_tuning[0x66] = 0xfa07;
        o->torsion_fine_tuning[0x67] = 0xfa06;
        o->torsion_fine_tuning[0x68] = 0xf9dc;
        o->torsion_fine_tuning[0x69] = 0xfa0b;
        o->torsion_fine_tuning[0x6a] = 0xfa45;
        o->torsion_fine_tuning[0x6b] = 0xfb12;
        o->torsion_fine_tuning[0x6c] = 0xface;
        o->torsion_fine_tuning[0x6d] = 0xfb50;
        o->torsion_fine_tuning[0x6e] = 0xfbd5;
        o->torsion_fine_tuning[0x6f] = 0xfbde;
        o->torsion_fine_tuning[0x70] = 0xfc5c;
        o->torsion_fine_tuning[0x71] = 0xfc43;
        o->torsion_fine_tuning[0x72] = 0xfc73;
        o->torsion_fine_tuning[0x73] = 0xfcbf;
        o->torsion_fine_tuning[0x74] = 0xfcc3;
        o->torsion_fine_tuning[0x75] = 0xfd90;
        o->torsion_fine_tuning[0x76] = 0xfd3a;
        o->torsion_fine_tuning[0x77] = 0xfdcb;
        o->torsion_fine_tuning[0x78] = 0xfdb5;
        o->torsion_fine_tuning[0x79] = 0xfd8f;
        o->torsion_fine_tuning[0x7a] = 0xfdda;
        o->torsion_fine_tuning[0x7b] = 0xfddf;
        o->torsion_fine_tuning[0x7c] = 0xfe18;
        o->torsion_fine_tuning[0x7d] = 0xfe55;
        o->torsion_fine_tuning[0x7e] = 0xfea7;
        o->torsion_fine_tuning[0x7f] = 0xff08;
        o->torsion_fine_tuning[0x80] = 0xff57;
        o->torsion_fine_tuning[0x81] = 0xff9f;
        o->torsion_fine_tuning[0x82] = 0xffb8;
        o->torsion_fine_tuning[0x83] = 0xffe5;
        o->torsion_fine_tuning[0x84] = 0x0066;
        o->torsion_fine_tuning[0x85] = 0x0153;
        o->torsion_fine_tuning[0x86] = 0x01b5;
        o->torsion_fine_tuning[0x87] = 0x0220;
        o->torsion_fine_tuning[0x88] = 0x028e;
        o->torsion_fine_tuning[0x89] = 0x02b0;
        o->torsion_fine_tuning[0x8a] = 0x02e7;
        o->torsion_fine_tuning[0x8b] = 0x034b;
        o->torsion_fine_tuning[0x8c] = 0x02ce;
        o->torsion_fine_tuning[0x8d] = 0x0253;
        o->torsion_fine_tuning[0x8e] = 0x024d;
        o->torsion_fine_tuning[0x8f] = 0x02dd;
        o->torsion_fine_tuning[0x90] = 0x02c6;
        o->torsion_fine_tuning[0x91] = 0x0287;
        o->torsion_fine_tuning[0x92] = 0x02df;
        o->torsion_fine_tuning[0x93] = 0x03b6;
        o->torsion_fine_tuning[0x94] = 0x03dd;
        o->torsion_fine_tuning[0x95] = 0x03da;
        o->torsion_fine_tuning[0x96] = 0x04d3;
        o->torsion_fine_tuning[0x97] = 0x0605;
        o->torsion_fine_tuning[0x98] = 0x0675;
        o->torsion_fine_tuning[0x99] = 0x0691;
        o->torsion_fine_tuning[0x9a] = 0x0720;
        o->torsion_fine_tuning[0x9b] = 0x0771;
        o->torsion_fine_tuning[0x9c] = 0x07a4;
        o->torsion_fine_tuning[0x9d] = 0x070f;
        o->torsion_fine_tuning[0x9e] = 0x06fe;
        o->torsion_fine_tuning[0x9f] = 0x06c3;
        o->torsion_fine_tuning[0xa0] = 0x0732;
        o->torsion_fine_tuning[0xa1] = 0x06ba;
        o->torsion_fine_tuning[0xa2] = 0x06f2;
        o->torsion_fine_tuning[0xa3] = 0x079e;
        o->torsion_fine_tuning[0xa4] = 0x07ef;
        o->torsion_fine_tuning[0xa5] = 0x07bc;
        o->torsion_fine_tuning[0xa6] = 0x07fc;
        o->torsion_fine_tuning[0xa7] = 0x08ae;
        o->torsion_fine_tuning[0xa8] = 0x08b2;
        o->torsion_fine_tuning[0xa9] = 0x099d;
        o->torsion_fine_tuning[0xaa] = 0x09fa;
        o->torsion_fine_tuning[0xab] = 0x0a4c;
        o->torsion_fine_tuning[0xac] = 0x0a58;
        o->torsion_fine_tuning[0xad] = 0x0ab4;
        o->torsion_fine_tuning[0xae] = 0x0ad7;
        o->torsion_fine_tuning[0xaf] = 0x0aaa;
        o->torsion_fine_tuning[0xb0] = 0x0b13;
        o->torsion_fine_tuning[0xb1] = 0x0b7a;
        o->torsion_fine_tuning[0xb2] = 0x0ba8;
        o->torsion_fine_tuning[0xb3] = 0x0bec;
        o->torsion_fine_tuning[0xb4] = 0x0bb4;
        o->torsion_fine_tuning[0xb5] = 0x0bf0;
        o->torsion_fine_tuning[0xb6] = 0x0bb6;
        o->torsion_fine_tuning[0xb7] = 0x0ba8;
        o->torsion_fine_tuning[0xb8] = 0x0be5;
        o->torsion_fine_tuning[0xb9] = 0x0b9c;
        o->torsion_fine_tuning[0xba] = 0x0c23;
        o->torsion_fine_tuning[0xbb] = 0x0cc9;
        o->torsion_fine_tuning[0xbc] = 0x0d06;
        o->torsion_fine_tuning[0xbd] = 0x0cd3;
        o->torsion_fine_tuning[0xbe] = 0x0c5d;
        o->torsion_fine_tuning[0xbf] = 0x0c38;
        o->torsion_fine_tuning[0xc0] = 0x0c32;
        o->torsion_fine_tuning[0xc1] = 0x0bd0;
        o->torsion_fine_tuning[0xc2] = 0x0bf1;
        o->torsion_fine_tuning[0xc3] = 0x0c5c;
        o->torsion_fine_tuning[0xc4] = 0x0c87;
        o->torsion_fine_tuning[0xc5] = 0x0ca5;
        o->torsion_fine_tuning[0xc6] = 0x0c8f;
        o->torsion_fine_tuning[0xc7] = 0x0ce3;
        o->torsion_fine_tuning[0xc8] = 0x0d04;
        o->torsion_fine_tuning[0xc9] = 0x0cde;
        o->torsion_fine_tuning[0xca] = 0x0cc4;
        o->torsion_fine_tuning[0xcb] = 0x0ce6;
        o->torsion_fine_tuning[0xcc] = 0x0d6b;
        o->torsion_fine_tuning[0xcd] = 0x0d27;
        o->torsion_fine_tuning[0xce] = 0x0cf5;
        o->torsion_fine_tuning[0xcf] = 0x0cce;
        o->torsion_fine_tuning[0xd0] = 0x0c2b;
        o->torsion_fine_tuning[0xd1] = 0x0be1;
        o->torsion_fine_tuning[0xd2] = 0x0b7a;
        o->torsion_fine_tuning[0xd3] = 0x0b9f;
        o->torsion_fine_tuning[0xd4] = 0x0b24;
        o->torsion_fine_tuning[0xd5] = 0x0b26;
        o->torsion_fine_tuning[0xd6] = 0x0b0f;
        o->torsion_fine_tuning[0xd7] = 0x0b22;
        o->torsion_fine_tuning[0xd8] = 0x0b30;
        o->torsion_fine_tuning[0xd9] = 0x0afd;
        o->torsion_fine_tuning[0xda] = 0x0b14;
        o->torsion_fine_tuning[0xdb] = 0x0abb;
        o->torsion_fine_tuning[0xdc] = 0x0ad9;
        o->torsion_fine_tuning[0xdd] = 0x0ad4;
        o->torsion_fine_tuning[0xde] = 0x0a85;
        o->torsion_fine_tuning[0xdf] = 0x0a82;
        o->torsion_fine_tuning[0xe0] = 0x0a08;
        o->torsion_fine_tuning[0xe1] = 0x099b;
        o->torsion_fine_tuning[0xe2] = 0x08af;
        o->torsion_fine_tuning[0xe3] = 0x0889;
        o->torsion_fine_tuning[0xe4] = 0x088b;
        o->torsion_fine_tuning[0xe5] = 0x0808;
        o->torsion_fine_tuning[0xe6] = 0x0790;
        o->torsion_fine_tuning[0xe7] = 0x0784;
        o->torsion_fine_tuning[0xe8] = 0x0723;
        o->torsion_fine_tuning[0xe9] = 0x071a;
        o->torsion_fine_tuning[0xea] = 0x06ad;
        o->torsion_fine_tuning[0xeb] = 0x0717;
        o->torsion_fine_tuning[0xec] = 0x06cf;
        o->torsion_fine_tuning[0xed] = 0x06f8;
        o->torsion_fine_tuning[0xee] = 0x0751;
        o->torsion_fine_tuning[0xef] = 0x079f;
        o->torsion_fine_tuning[0xf0] = 0x07d1;
        o->torsion_fine_tuning[0xf1] = 0x078a;
        o->torsion_fine_tuning[0xf2] = 0x0726;
        o->torsion_fine_tuning[0xf3] = 0x0694;
        o->torsion_fine_tuning[0xf4] = 0x066c;
        o->torsion_fine_tuning[0xf5] = 0x0613;
        o->torsion_fine_tuning[0xf6] = 0x058e;
        o->torsion_fine_tuning[0xf7] = 0x05c7;
        o->torsion_fine_tuning[0xf8] = 0x0591;
        o->torsion_fine_tuning[0xf9] = 0x0592;
        o->torsion_fine_tuning[0xfa] = 0x056f;
        o->torsion_fine_tuning[0xfb] = 0x053d;
        o->torsion_fine_tuning[0xfc] = 0x0521;
        o->torsion_fine_tuning[0xfd] = 0x04d2;
        o->torsion_fine_tuning[0xfe] = 0x0489;
        o->torsion_fine_tuning[0xff] = 0x0469;
        #endif
        
        #ifdef OLD_JOINT

        CTRL_UNITS omega_max = 2.0f; // [rad/s]
        
        o->trq_ctrl.Ko = 0.350f; // Static friction coefficient  [A]
        o->trq_ctrl.Kw = 0.9f; // Viscous friction coefficient [A/(rad/s)]
        o->trq_ctrl.Ifriction_max = o->trq_ctrl.Ko + o->trq_ctrl.Kw*omega_max; // Limits the two contributions above [A] 
    
        o->trq_ctrl.Kw2 = 0.0f; // Quadratic term for non linear characteristic [A/(rad/s)^2]
                            // It can also be negative to prevent runaway.
        o->trq_ctrl.Iquadratic_max = o->trq_ctrl.Kw2*omega_max*omega_max; // Limits the quadratic contribution above [A]
    
        o->trq_ctrl.Kff = 1/FABS((0.058f*o->GEARBOX*0.85f)); // Feed forward term, equal to 1/(torque constant * gearbox ratio * efficiency) [A/Nm]
        o->trq_ctrl.Iff_max = o->trq_ctrl.Kff * 50.0f; // Limits the feed forward contribution above [A]
    
        o->trq_ctrl.Kp = 5.0f * o->trq_ctrl.Kff; // Proportional term [A/Nm]
        o->trq_ctrl.Iproportional_max = o->trq_ctrl.Iff_max/10.0f; // Limits the proportional contribution above [A]
    
        o->trq_ctrl.Iout_max = 10.0f; // Limits the final output [A] 
        
        o->torsion_offset = 0x4179;
        
        o->joint_full_res = 524288;
                
        o->torsion_fine_tuning[0x00] = 0x0058;
        o->torsion_fine_tuning[0x01] = 0x0094;
        o->torsion_fine_tuning[0x02] = 0x0094;
        o->torsion_fine_tuning[0x03] = 0x00f0;
        o->torsion_fine_tuning[0x04] = 0x00f6;
        o->torsion_fine_tuning[0x05] = 0x0146;
        o->torsion_fine_tuning[0x06] = 0x018d;
        o->torsion_fine_tuning[0x07] = 0x0158;
        o->torsion_fine_tuning[0x08] = 0x01db;
        o->torsion_fine_tuning[0x09] = 0x026c;
        o->torsion_fine_tuning[0x0a] = 0x0283;
        o->torsion_fine_tuning[0x0b] = 0x0288;
        o->torsion_fine_tuning[0x0c] = 0x0290;
        o->torsion_fine_tuning[0x0d] = 0x02c6;
        o->torsion_fine_tuning[0x0e] = 0x02da;
        o->torsion_fine_tuning[0x0f] = 0x02c7;
        o->torsion_fine_tuning[0x10] = 0x02e7;
        o->torsion_fine_tuning[0x11] = 0x0344;
        o->torsion_fine_tuning[0x12] = 0x036e;
        o->torsion_fine_tuning[0x13] = 0x035d;
        o->torsion_fine_tuning[0x14] = 0x03a6;
        o->torsion_fine_tuning[0x15] = 0x03f9;
        o->torsion_fine_tuning[0x16] = 0x040d;
        o->torsion_fine_tuning[0x17] = 0x03f2;
        o->torsion_fine_tuning[0x18] = 0x041b;
        o->torsion_fine_tuning[0x19] = 0x047d;
        o->torsion_fine_tuning[0x1a] = 0x048e;
        o->torsion_fine_tuning[0x1b] = 0x04b0;
        o->torsion_fine_tuning[0x1c] = 0x04e6;
        o->torsion_fine_tuning[0x1d] = 0x0532;
        o->torsion_fine_tuning[0x1e] = 0x0581;
        o->torsion_fine_tuning[0x1f] = 0x0524;
        o->torsion_fine_tuning[0x20] = 0x053e;
        o->torsion_fine_tuning[0x21] = 0x05ac;
        o->torsion_fine_tuning[0x22] = 0x05a5;
        o->torsion_fine_tuning[0x23] = 0x0577;
        o->torsion_fine_tuning[0x24] = 0x0538;
        o->torsion_fine_tuning[0x25] = 0x054a;
        o->torsion_fine_tuning[0x26] = 0x0553;
        o->torsion_fine_tuning[0x27] = 0x0516;
        o->torsion_fine_tuning[0x28] = 0x054e;
        o->torsion_fine_tuning[0x29] = 0x054f;
        o->torsion_fine_tuning[0x2a] = 0x0586;
        o->torsion_fine_tuning[0x2b] = 0x056f;
        o->torsion_fine_tuning[0x2c] = 0x055c;
        o->torsion_fine_tuning[0x2d] = 0x0563;
        o->torsion_fine_tuning[0x2e] = 0x0552;
        o->torsion_fine_tuning[0x2f] = 0x04ee;
        o->torsion_fine_tuning[0x30] = 0x04f4;
        o->torsion_fine_tuning[0x31] = 0x04fd;
        o->torsion_fine_tuning[0x32] = 0x04fb;
        o->torsion_fine_tuning[0x33] = 0x04ea;
        o->torsion_fine_tuning[0x34] = 0x04a0;
        o->torsion_fine_tuning[0x35] = 0x04e4;
        o->torsion_fine_tuning[0x36] = 0x04ee;
        o->torsion_fine_tuning[0x37] = 0x048c;
        o->torsion_fine_tuning[0x38] = 0x04a8;
        o->torsion_fine_tuning[0x39] = 0x049f;
        o->torsion_fine_tuning[0x3a] = 0x0486;
        o->torsion_fine_tuning[0x3b] = 0x0401;
        o->torsion_fine_tuning[0x3c] = 0x03ab;
        o->torsion_fine_tuning[0x3d] = 0x03c0;
        o->torsion_fine_tuning[0x3e] = 0x03a9;
        o->torsion_fine_tuning[0x3f] = 0x037f;
        o->torsion_fine_tuning[0x40] = 0x0365;
        o->torsion_fine_tuning[0x41] = 0x037b;
        o->torsion_fine_tuning[0x42] = 0x03ca;
        o->torsion_fine_tuning[0x43] = 0x0373;
        o->torsion_fine_tuning[0x44] = 0x0332;
        o->torsion_fine_tuning[0x45] = 0x035e;
        o->torsion_fine_tuning[0x46] = 0x0322;
        o->torsion_fine_tuning[0x47] = 0x02ef;
        o->torsion_fine_tuning[0x48] = 0x02fc;
        o->torsion_fine_tuning[0x49] = 0x031c;
        o->torsion_fine_tuning[0x4a] = 0x0345;
        o->torsion_fine_tuning[0x4b] = 0x032a;
        o->torsion_fine_tuning[0x4c] = 0x0309;
        o->torsion_fine_tuning[0x4d] = 0x02f4;
        o->torsion_fine_tuning[0x4e] = 0x0288;
        o->torsion_fine_tuning[0x4f] = 0x026a;
        o->torsion_fine_tuning[0x50] = 0x0287;
        o->torsion_fine_tuning[0x51] = 0x02a7;
        o->torsion_fine_tuning[0x52] = 0x025e;
        o->torsion_fine_tuning[0x53] = 0x0285;
        o->torsion_fine_tuning[0x54] = 0x02e2;
        o->torsion_fine_tuning[0x55] = 0x0312;
        o->torsion_fine_tuning[0x56] = 0x0356;
        o->torsion_fine_tuning[0x57] = 0x0321;
        o->torsion_fine_tuning[0x58] = 0x0326;
        o->torsion_fine_tuning[0x59] = 0x0357;
        o->torsion_fine_tuning[0x5a] = 0x0340;
        o->torsion_fine_tuning[0x5b] = 0x02e5;
        o->torsion_fine_tuning[0x5c] = 0x029c;
        o->torsion_fine_tuning[0x5d] = 0x02af;
        o->torsion_fine_tuning[0x5e] = 0x027f;
        o->torsion_fine_tuning[0x5f] = 0x0252;
        o->torsion_fine_tuning[0x60] = 0x023a;
        o->torsion_fine_tuning[0x61] = 0x0263;
        o->torsion_fine_tuning[0x62] = 0x0284;
        o->torsion_fine_tuning[0x63] = 0x02b0;
        o->torsion_fine_tuning[0x64] = 0x02bf;
        o->torsion_fine_tuning[0x65] = 0x02e7;
        o->torsion_fine_tuning[0x66] = 0x032c;
        o->torsion_fine_tuning[0x67] = 0x0322;
        o->torsion_fine_tuning[0x68] = 0x0302;
        o->torsion_fine_tuning[0x69] = 0x0307;
        o->torsion_fine_tuning[0x6a] = 0x02f1;
        o->torsion_fine_tuning[0x6b] = 0x02e3;
        o->torsion_fine_tuning[0x6c] = 0x027c;
        o->torsion_fine_tuning[0x6d] = 0x025b;
        o->torsion_fine_tuning[0x6e] = 0x027a;
        o->torsion_fine_tuning[0x6f] = 0x0255;
        o->torsion_fine_tuning[0x70] = 0x01f0;
        o->torsion_fine_tuning[0x71] = 0x017c;
        o->torsion_fine_tuning[0x72] = 0x0162;
        o->torsion_fine_tuning[0x73] = 0x0121;
        o->torsion_fine_tuning[0x74] = 0x00aa;
        o->torsion_fine_tuning[0x75] = 0x0083;
        o->torsion_fine_tuning[0x76] = 0x008e;
        o->torsion_fine_tuning[0x77] = 0x0085;
        o->torsion_fine_tuning[0x78] = 0x006e;
        o->torsion_fine_tuning[0x79] = 0x0066;
        o->torsion_fine_tuning[0x7a] = 0x0011;
        o->torsion_fine_tuning[0x7b] = 0xffc5;
        o->torsion_fine_tuning[0x7c] = 0xffbd;
        o->torsion_fine_tuning[0x7d] = 0xffc7;
        o->torsion_fine_tuning[0x7e] = 0xff69;
        o->torsion_fine_tuning[0x7f] = 0xfef0;
        o->torsion_fine_tuning[0x80] = 0xff0b;
        o->torsion_fine_tuning[0x81] = 0xfeda;
        o->torsion_fine_tuning[0x82] = 0xfebd;
        o->torsion_fine_tuning[0x83] = 0xfe9e;
        o->torsion_fine_tuning[0x84] = 0xfe33;
        o->torsion_fine_tuning[0x85] = 0xfe29;
        o->torsion_fine_tuning[0x86] = 0xfdd0;
        o->torsion_fine_tuning[0x87] = 0xfd50;
        o->torsion_fine_tuning[0x88] = 0xfd18;
        o->torsion_fine_tuning[0x89] = 0xfce0;
        o->torsion_fine_tuning[0x8a] = 0xfcae;
        o->torsion_fine_tuning[0x8b] = 0xfc5f;
        o->torsion_fine_tuning[0x8c] = 0xfc9b;
        o->torsion_fine_tuning[0x8d] = 0xfcab;
        o->torsion_fine_tuning[0x8e] = 0xfc2c;
        o->torsion_fine_tuning[0x8f] = 0xfc1a;
        o->torsion_fine_tuning[0x90] = 0xfbea;
        o->torsion_fine_tuning[0x91] = 0xfbb3;
        o->torsion_fine_tuning[0x92] = 0xfb7f;
        o->torsion_fine_tuning[0x93] = 0xfb52;
        o->torsion_fine_tuning[0x94] = 0xfb7e;
        o->torsion_fine_tuning[0x95] = 0xfb7a;
        o->torsion_fine_tuning[0x96] = 0xfbb9;
        o->torsion_fine_tuning[0x97] = 0xfbf1;
        o->torsion_fine_tuning[0x98] = 0xfbf9;
        o->torsion_fine_tuning[0x99] = 0xfc09;
        o->torsion_fine_tuning[0x9a] = 0xfbda;
        o->torsion_fine_tuning[0x9b] = 0xfb8d;
        o->torsion_fine_tuning[0x9c] = 0xfb39;
        o->torsion_fine_tuning[0x9d] = 0xfb34;
        o->torsion_fine_tuning[0x9e] = 0xfb3b;
        o->torsion_fine_tuning[0x9f] = 0xfac7;
        o->torsion_fine_tuning[0xa0] = 0xfaf2;
        o->torsion_fine_tuning[0xa1] = 0xfaf1;
        o->torsion_fine_tuning[0xa2] = 0xfab7;
        o->torsion_fine_tuning[0xa3] = 0xfa1e;
        o->torsion_fine_tuning[0xa4] = 0xf9b9;
        o->torsion_fine_tuning[0xa5] = 0xf97a;
        o->torsion_fine_tuning[0xa6] = 0xf94f;
        o->torsion_fine_tuning[0xa7] = 0xf952;
        o->torsion_fine_tuning[0xa8] = 0xf9c7;
        o->torsion_fine_tuning[0xa9] = 0xfa7f;
        o->torsion_fine_tuning[0xaa] = 0xfb84;
        o->torsion_fine_tuning[0xab] = 0xfc0d;
        o->torsion_fine_tuning[0xac] = 0xfca4;
        o->torsion_fine_tuning[0xad] = 0xfd09;
        o->torsion_fine_tuning[0xae] = 0xfcfc;
        o->torsion_fine_tuning[0xaf] = 0xfca6;
        o->torsion_fine_tuning[0xb0] = 0xfc4a;
        o->torsion_fine_tuning[0xb1] = 0xfbba;
        o->torsion_fine_tuning[0xb2] = 0xfb22;
        o->torsion_fine_tuning[0xb3] = 0xfac0;
        o->torsion_fine_tuning[0xb4] = 0xfa7b;
        o->torsion_fine_tuning[0xb5] = 0xfa4f;
        o->torsion_fine_tuning[0xb6] = 0xfa21;
        o->torsion_fine_tuning[0xb7] = 0xf9e0;
        o->torsion_fine_tuning[0xb8] = 0xfa2c;
        o->torsion_fine_tuning[0xb9] = 0xfa01;
        o->torsion_fine_tuning[0xba] = 0xf9fd;
        o->torsion_fine_tuning[0xbb] = 0xfa53;
        o->torsion_fine_tuning[0xbc] = 0xfa59;
        o->torsion_fine_tuning[0xbd] = 0xfae9;
        o->torsion_fine_tuning[0xbe] = 0xfb53;
        o->torsion_fine_tuning[0xbf] = 0xfb1d;
        o->torsion_fine_tuning[0xc0] = 0xfb77;
        o->torsion_fine_tuning[0xc1] = 0xfb9d;
        o->torsion_fine_tuning[0xc2] = 0xfb34;
        o->torsion_fine_tuning[0xc3] = 0xfb24;
        o->torsion_fine_tuning[0xc4] = 0xfb15;
        o->torsion_fine_tuning[0xc5] = 0xfb16;
        o->torsion_fine_tuning[0xc6] = 0xfb04;
        o->torsion_fine_tuning[0xc7] = 0xfaee;
        o->torsion_fine_tuning[0xc8] = 0xfb2e;
        o->torsion_fine_tuning[0xc9] = 0xfb0a;
        o->torsion_fine_tuning[0xca] = 0xfb01;
        o->torsion_fine_tuning[0xcb] = 0xfb60;
        o->torsion_fine_tuning[0xcc] = 0xfb64;
        o->torsion_fine_tuning[0xcd] = 0xfbad;
        o->torsion_fine_tuning[0xce] = 0xfb93;
        o->torsion_fine_tuning[0xcf] = 0xfb76;
        o->torsion_fine_tuning[0xd0] = 0xfbfc;
        o->torsion_fine_tuning[0xd1] = 0xfc2b;
        o->torsion_fine_tuning[0xd2] = 0xfbfa;
        o->torsion_fine_tuning[0xd3] = 0xfc1b;
        o->torsion_fine_tuning[0xd4] = 0xfc38;
        o->torsion_fine_tuning[0xd5] = 0xfc49;
        o->torsion_fine_tuning[0xd6] = 0xfc4d;
        o->torsion_fine_tuning[0xd7] = 0xfc0e;
        o->torsion_fine_tuning[0xd8] = 0xfc61;
        o->torsion_fine_tuning[0xd9] = 0xfc80;
        o->torsion_fine_tuning[0xda] = 0xfc3a;
        o->torsion_fine_tuning[0xdb] = 0xfca6;
        o->torsion_fine_tuning[0xdc] = 0xfcb2;
        o->torsion_fine_tuning[0xdd] = 0xfd27;
        o->torsion_fine_tuning[0xde] = 0xfd81;
        o->torsion_fine_tuning[0xdf] = 0xfd45;
        o->torsion_fine_tuning[0xe0] = 0xfd75;
        o->torsion_fine_tuning[0xe1] = 0xfd69;
        o->torsion_fine_tuning[0xe2] = 0xfcff;
        o->torsion_fine_tuning[0xe3] = 0xfcc0;
        o->torsion_fine_tuning[0xe4] = 0xfcac;
        o->torsion_fine_tuning[0xe5] = 0xfc9c;
        o->torsion_fine_tuning[0xe6] = 0xfc8a;
        o->torsion_fine_tuning[0xe7] = 0xfc57;
        o->torsion_fine_tuning[0xe8] = 0xfcb0;
        o->torsion_fine_tuning[0xe9] = 0xfcfa;
        o->torsion_fine_tuning[0xea] = 0xfcf6;
        o->torsion_fine_tuning[0xeb] = 0xfd46;
        o->torsion_fine_tuning[0xec] = 0xfdc2;
        o->torsion_fine_tuning[0xed] = 0xfe1c;
        o->torsion_fine_tuning[0xee] = 0xfe63;
        o->torsion_fine_tuning[0xef] = 0xfe55;
        o->torsion_fine_tuning[0xf0] = 0xfe7c;
        o->torsion_fine_tuning[0xf1] = 0xfe9c;
        o->torsion_fine_tuning[0xf2] = 0xfe80;
        o->torsion_fine_tuning[0xf3] = 0xfe96;
        o->torsion_fine_tuning[0xf4] = 0xfeac;
        o->torsion_fine_tuning[0xf5] = 0xfeec;
        o->torsion_fine_tuning[0xf6] = 0xff1d;
        o->torsion_fine_tuning[0xf7] = 0xff4e;
        o->torsion_fine_tuning[0xf8] = 0xff67;
        o->torsion_fine_tuning[0xf9] = 0xff82;
        o->torsion_fine_tuning[0xfa] = 0xffc1;
        o->torsion_fine_tuning[0xfb] = 0xffc4;
        o->torsion_fine_tuning[0xfc] = 0xffa8;
        o->torsion_fine_tuning[0xfd] = 0x0039;
        o->torsion_fine_tuning[0xfe] = 0x0060;
        o->torsion_fine_tuning[0xff] = 0x000e;
        #endif
    }
}
#endif


#if defined(EOTHESERVICES_customize_handV3_7joints)

// - here is the new code for the pmc board

static void Motor_stop_PMC(Motor *o)
{
    pmc_motor_disable(o->canloc);
}

static void Motor_config_PMC(Motor* o, eOmc_motor_config_t* config)
{  
#if 1
    // marco.accame on 08 jan 2021: so far, we dont configure the motors on the pmc. we just stop them moving
    Motor_stop_PMC(o);
#else   
    #error marco.accame: add code for motor config
    static const uint16_t pmc_HAS_QE        = 0x0001;
    static const uint16_t pmc_HAS_HALL      = 0x0002;
    static const uint16_t pmc_HAS_TSENS     = 0x0004;
    static const uint16_t pmc_USE_INDEX     = 0x0008;
    static const uint16_t pmc_HAS_SPEED_QE  = 0x0010;
    static const uint16_t pmc_ENA_VERBOSE   = 0x0020;
    
    o->can_motor_config[0] = 0;
    
    if (config->hasRotorEncoder)        o->can_motor_config[0] |= pmc_HAS_QE;
    if (config->hasHallSensor)          o->can_motor_config[0] |= pmc_HAS_HALL;
    if (config->hasRotorEncoderIndex)   o->can_motor_config[0] |= pmc_USE_INDEX;
    if (config->hasTempSensor)          o->can_motor_config[0] |= pmc_HAS_TSENS;
    if (config->hasSpeedEncoder)        o->can_motor_config[0] |= pmc_HAS_SPEED_QE;
    if (config->verbose)                o->can_motor_config[0] |= pmc_ENA_VERBOSE;
    
    *(int16_t*)(o->can_motor_config+1) = config->rotorEncoderResolution;
    *(int16_t*)(o->can_motor_config+3) = config->rotorIndexOffset;
    
    o->can_motor_config[5] = config->motorPoles;
    
    o->can_motor_config[6] = (int)(o->enc_tolerance*10.0f);

    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, o->ID, 0);
    
    Motor_config_current_PID_2FOC(o, &(config->pidcurrent));
    Motor_config_velocity_PID_2FOC(o, &(config->pidspeed));
        
    eOcanprot_command_t cmdMaxCurrent;
    cmdMaxCurrent.clas = eocanprot_msgclass_pollingMotorControl;
    cmdMaxCurrent.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    cmdMaxCurrent.value = &(config->currentLimits);
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdMaxCurrent, id32);

    eOcanprot_command_t cmdMotorConfig;
    cmdMotorConfig.clas = eocanprot_msgclass_pollingMotorControl;
    cmdMotorConfig.type = ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG;
    cmdMotorConfig.value = o->can_motor_config;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &cmdMotorConfig, id32);
#endif    
}

static void Motor_set_control_mode_PMC(Motor *o, icubCanProto_controlmode_t control_mode)
{
#if 1
    if((control_mode == icubCanProto_controlmode_idle) || 
       (control_mode == icubCanProto_controlmode_forceIdle) ||
       (control_mode == icubCanProto_controlmode_hwFault)
      )
    {
        Motor_stop_PMC(o);
        //#warning we dont need to send a proper can message to the pmc. we just stop its piezo motors
    }
    else
    {
        //#warning add a proper message, but maybe not
    }
    // which contains [mskofmotors][ctrl0][ctrl1][ctrl2][ctrl3]
#else
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.clas = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &control_mode;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
#endif
}

static void pmc_motor_disable(eObrd_canlocation_t canloc)
{
    #warning add a proper message which sends the velocity = 0
    #warning maybe in the pmc we should put a timeout for the velocity control
}

void Motor_config_encoder_PMC(Motor* o, int32_t resolution)
{
    // so far nothing ??
}

#endif