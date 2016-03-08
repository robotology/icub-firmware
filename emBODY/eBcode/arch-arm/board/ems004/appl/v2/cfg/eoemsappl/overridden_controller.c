/*
 * Copyright (C) 2015 ICub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "EOtheCANservice.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "Controller.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

#if 0
extern void eo_emsController_hid_userdef_config_motor(EOemsController* ctrl, uint8_t motor)
{   
    int8_t KpKiKdKs[7];
    ((int16_t*)KpKiKdKs)[0] =  8; //Kp
    ((int16_t*)KpKiKdKs)[1] =  2; //Ki
    ((int16_t*)KpKiKdKs)[2] =  0; //Kd (unused in 2FOC)
               KpKiKdKs [6] = 10; // shift
    
    uint32_t max_current = ctrl->motor_config_maxcurrentofmotor[motor]; 
    /* max_current = 5000; // 5A*/
    
    #define HAS_QE      0x0001
    #define HAS_HALL    0x0002
    #define HAS_TSENS   0x0004
    #define USE_INDEX   0x0008

    uint8_t motor_config[6];
    motor_config[0] = 0; // HAS_QE|HAS_HALL;
    if (ctrl->motor_config_hasRotorEncoder[motor])        motor_config[0] |= HAS_QE;
    if (ctrl->motor_config_hasHallSensor[motor])          motor_config[0] |= HAS_HALL;
    if (ctrl->motor_config_hasRotorEncoderIndex[motor])   motor_config[0] |= USE_INDEX;
    if (ctrl->motor_config_hasTempSensor[motor])          motor_config[0] |= HAS_TSENS;
    /*motor_config[0] = HAS_HALL; // |HAS_QE;*/
    
    *(int16_t*)(motor_config+1) = ctrl->motor_config_rotorencoder[motor];//-14400;//-8192;
    *(int16_t*)(motor_config+3) = 0; // offset (degrees)
    
    motor_config[5] = ctrl->motor_config_motorPoles[motor];
    /* motor_config[5] = 8;*/
    
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
    
    eo_motors_new_state_req(ctrl->motors, motor, icubCanProto_controlmode_idle);
}

extern void eo_emsController_hid_userdef_set_motor_idle(EOemsController* ctrl, uint8_t motor)
{
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_idle;

    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    eo_motors_new_state_req(ctrl->motors, motor, icubCanProto_controlmode_idle);
}

extern void eo_emsController_hid_userdef_force_motor_idle(EOemsController* ctrl, uint8_t motor)
{
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_forceIdle;
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    eo_motors_new_state_req(ctrl->motors, motor, icubCanProto_controlmode_idle);
}

extern void eo_emsController_hid_userdef_set_motor_running(EOemsController* ctrl, uint8_t motor)
{
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_openloop;
    
    #ifdef EXPERIMENTAL_SPEED_CONTROL
    controlmode_2foc = icubCanProto_controlmode_speed_voltage;
    #endif
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);

    eo_motors_new_state_req(ctrl->motors, motor, controlmode_2foc);
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




