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

//general
#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"
#include "EOarray.h"
#include "EOtheErrorManager.h"

//appl
#include "EOtheEMSapplBody.h"
#include "EOtheCANservice.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


#include "EOemsController.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

 #include "EOemsController_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eo_emsController_hid_userdef_config_motor(EOemsController* ctrl,uint8_t motor)
{
    return;
    
    //if (ems->act != emscontroller_actuation_2FOC) return;
    
    //eOmc_i2tParams_t i2t;
    //icubCanProto_current_t max_current;
    
    int8_t KpKiKdKs[7];
    ((int16_t*)KpKiKdKs)[0] =  8; //Kp
    ((int16_t*)KpKiKdKs)[1] =  2; //Ki
    ((int16_t*)KpKiKdKs)[2] =  0; //Kd (unused in 2FOC)
               KpKiKdKs [6] = 10; // shift
    
    uint32_t max_current = 5000; // 5A
    
    #define HAS_QE      0x0001
    #define HAS_HALL    0x0002
    #define HAS_TSENS   0x0004

    uint8_t motor_config[6];
    motor_config[0] = HAS_QE|HAS_HALL;
    *(int16_t*)(motor_config+1) = -8192;
    *(int16_t*)(motor_config+3) = 0; // offset (degrees)
    motor_config[5] = 8; // num motor poles
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;

    command.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    command.value = KpKiKdKs;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    command.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    command.value = &max_current;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);

    command.type = ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG;
    command.value = motor_config;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);      
    
    eo_motors_new_state_req(ctrl->motors, motor, icubCanProto_controlmode_idle);
}

extern void eo_emsController_hid_userdef_set_motor_idle(EOemsController* ctrl,uint8_t motor)
{
    return;
    
    //if (ems->act != emscontroller_actuation_2FOC) return;
    
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_idle;

    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    eo_motors_new_state_req(ctrl->motors, motor, icubCanProto_controlmode_idle);
}

extern void eo_emsController_hid_userdef_force_motor_idle(EOemsController* ctrl,uint8_t motor)
{
    return;
    
    //if (ems->act != emscontroller_actuation_2FOC) return;
    
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_forceIdle;
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    eo_motors_new_state_req(ctrl->motors, motor, icubCanProto_controlmode_idle);
}

extern void eo_emsController_hid_userdef_set_motor_running(EOemsController* ctrl,uint8_t motor)
{
    return;
    
    //if (ems->act != emscontroller_actuation_2FOC) return;
    
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_openloop;
    
    #ifdef EXPERIMENTAL_SPEED_CONTROL
    controlmode_2foc = icubCanProto_controlmode_velocity;
    #endif
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);

    eo_motors_new_state_req(ctrl->motors, motor, icubCanProto_controlmode_openloop);
}

#if 0
extern void eo_emsController_hid_userdef_config_motor(EOemsController* ctrl,uint8_t motor)
{
    // we want to send two can frames 
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    
    // first one: set current pid
    command.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    int8_t KpKiKdKs[7];
    ((int16_t*)KpKiKdKs)[0] =  8; //Kp
    ((int16_t*)KpKiKdKs)[1] =  2; //Ki
    ((int16_t*)KpKiKdKs)[2] =  0; //Kd (unused in 2FOC)
               KpKiKdKs [6] = 10; // shift
    command.value = KpKiKdKs;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    // second one: set current limit
    command.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    uint32_t max_current = 5000; // 5A
    command.value = &max_current;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);    

}


extern void eo_emsController_hid_userdef_set_motor_idle(EOemsController* ctrl,uint8_t motor)
{
    // we want to send one can frame: set control mode idle
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_idle; 
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    // and then
    eo_motor_set_motor_status(ctrl->motors, motor, 0);
}

extern void eo_emsController_hid_userdef_set_motor_running(EOemsController* ctrl,uint8_t motor)
{
   // we want to send one can frame: set control mode 
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_openloop; 
    
    #ifdef EXPERIMENTAL_SPEED_CONTROL
    controlmode_2foc = icubCanProto_controlmode_velocity;
    #else
    controlmode_2foc = icubCanProto_controlmode_openloop;
    //controlmode_2foc = icubCanProto_controlmode_current;
    #endif
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




