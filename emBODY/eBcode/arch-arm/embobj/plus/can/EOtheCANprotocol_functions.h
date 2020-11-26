/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTHECANPROTOCOL_FUNCTIONS_H_
#define _EOTHECANPROTOCOL_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EOtheCANprotocol_functions.h
    @brief      This header file contains declaration of the functions used to parse / form can frames
    @author     marco.accame@iit.it
	@date       12 mar 2015
**/

/** @defgroup doxygroup_EOtheCANprotocolCfg CAN protocol parsing/forming functions
    This file contains function declarations for parsing / forming can frames
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheCANprotocol.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

  
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

// - analog sensors: polling

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__SET_TXMODE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__SET_CANDATARATE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__SET_RESOLUTION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotASpolling_parser_POL_AS_CMD__GET_FULL_SCALES(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__GET_FULL_SCALES(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_parser_POL_AS_CMD__GET_FIRMWARE_VERSION(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__GET_FIRMWARE_VERSION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__IMU_CONFIG_GET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotASpolling_parser_POL_AS_CMD__IMU_CONFIG_GET(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__IMU_CONFIG_SET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__IMU_TRANSMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__THERMOMETER_CONFIG_SET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__THERMOMETER_TRANSMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__TACT_SETUP(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__SET_BRD_CFG(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__TACT_SETUP2(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);


extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__ACC_GYRO_SETUP(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__SET_TRIANG_CFG(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__POS_CONFIG_GET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotASpolling_parser_POL_AS_CMD__POS_CONFIG_GET(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__POS_CONFIG_SET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__POS_TRANSMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);



// - analog sensors: periodic

// weakly defined
extern eObool_t eocanprotASperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_THERMOMETER_MSG(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__USERDEF(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__POS(eOcanframe_t *frame, eOcanport_t port);


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE(eOcanframe_t *frame, eOcanport_t port);


extern eOresult_t eocanprotASperiodic_former_PER_AS_MSG__FORCE_VECTOR(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__FORCE_VECTOR(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotASperiodic_former_PER_AS_MSG__TORQUE_VECTOR(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__TORQUE_VECTOR(eOcanframe_t *frame, eOcanport_t port);


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__HES0TO6(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__HES7TO14(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__THERMOMETER_MEASURE(eOcanframe_t *frame, eOcanport_t port);

// - inertial sensors: periodic

// weakly defined
extern eObool_t eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL_MSG(eOcanframe_t *frame, eOcanport_t port);

// weakly defined
extern eObool_t eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL3_MSG(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_GYROSCOPE(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_ACCELEROMETER(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__IMU_TRIPLE(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__IMU_QUATERNION(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__IMU_STATUS(eOcanframe_t *frame, eOcanport_t port);


// - motion control: polling

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_RUN(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_IDLE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__CALIBRATE_ENCODER(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__ENABLE_PWM_PAD(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__DISABLE_PWM_PAD(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_CONTROL_MODE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_CONTROL_MODE(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__MOTION_DONE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__MOTION_DONE(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CONTROL_MODE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__POSITION_MOVE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__VELOCITY_MOVE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_TORQUE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_DESIRED_TORQUE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__STOP_TRAJECTORY(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_COMMAND_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_MAX_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_MAX_POSITION(eOcanframe_t *frame, eOcanport_t port);



extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_PWM_LIMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_PWM_LIMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_PWM_LIMIT(eOcanframe_t *frame, eOcanport_t port);


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_MOTOR_POS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_MOTOR_POS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_VELOCITY(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_LIMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser
    
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_BCAST_POLICY(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_SHIFT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_PID(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_PIDLIMITS(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_POS_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_POS_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_PID(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_POS_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_POS_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_PIDLIMITS(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_TIMEOUT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_IMPEDANCE_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_IMPEDANCE_PARAMS(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_OFFSET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_IMPEDANCE_OFFSET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_IMPEDANCE_OFFSET(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_FIRMWARE_VERSION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_FIRMWARE_VERSION(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser
    
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VELOCITY_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser
    
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VELOCITY_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_CURRENT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_I2T_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_OPENLOOP_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_OPENLOOP_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser
    
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_INTERACTION_MODE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser
    
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_OPENLOOP_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_OPENLOOP_PARAMS(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MOTOR_CONFIG(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_POS_STICTION_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_POS_STICTION_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_STICTION_PARAMS(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_STICTION_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
// no parser

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_STICTION_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_STICTION_PARAMS(eOcanframe_t *frame, eOcanport_t port);

// - motion control: periodic

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__DEBUG(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__2FOC(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__POSITION(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__MOTOR_POSITION(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__PID_VAL(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__STATUS(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__CURRENT(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__PRINT(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__VELOCITY(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__PID_ERROR(eOcanframe_t *frame, eOcanport_t port);


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__ADDITIONAL_STATUS(eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eocanprotMCperiodic_former_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);




// - skin: polling
// nothing

// - skin: periodic

// weakly defined
//extern eObool_t eocanprotSKperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_SKIN_MSG(eOsk_skin_t *skin);

extern eOresult_t eocanprotSKperiodic_parser_ANY_PERIODIC_SKIN_MSG(eOcanframe_t *frame, eOcanport_t port);



/** @}            
    end of group doxygroup_EOtheCANprotocolCfg  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


