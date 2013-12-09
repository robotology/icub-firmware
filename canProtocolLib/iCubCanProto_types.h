/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

#ifndef _ICUBCANPROTO_TYPES_H_
#define _ICUBCANPROTO_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       iCubCanProto_types.h
    @brief      This header file gives definitions of types used in iCub CAN protocol
    @author     valentina.gaggero@iit.it
    @date       12/03/2012
    @ingroup    iCubCanProtocol
**/

/** @ingroup    iCubCanProtocol
    @{
 **/



// - external dependencies --------------------------------------------------------------------------------------------
#include "iCubCanProto_classes.h"


// - public #define  --------------------------------------------------------------------------------------------------
// error flags in motor board status message (ICUBCANPROTO_PER_MC_MSG__STATUS)
#define ICUBCANPROTO_PER_MC_STATUS_FLAG_UNDERVOLTAGE    0x01
#define ICUBCANPROTO_PER_MC_STATUS_FLAG_OVERVOLTAGE     0x02
#define ICUBCANPROTO_PER_MC_STATUS_FLAG_EXTERNAL        0x04
#define ICUBCANPROTO_PER_MC_STATUS_FLAG_OVERCURRENT     0x08
#define ICUBCANPROTO_PER_MC_STATUS_FLAG_I2TFAILURE      0x20//in data[5]
#define ICUBCANPROTO_PER_MC_STATUS_FLAG_CANRECWARNING   0x20 //in data[4]   
#define ICUBCANPROTO_PER_MC_STATUS_FLAG_CANRECERROR     0x08 //in data[4]  
#define ICUBCANPROTO_PER_MC_STATUS_FLAG_CANRECHWOVERRUN 0x40 //in data[4]


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum icubCanProto_boardType_t
    @brief      contains board type can be mounted on robot.
                Important: its values are defined in according with icubInterface 
                (see downloader.h in canLoader module)
 **/
typedef enum
{
    icubCanProto_boardType__dsp     = 0,
    icubCanProto_boardType__pic     = 1,
    icubCanProto_boardType__2dc     = 2,
    icubCanProto_boardType__4dc     = 3,
    icubCanProto_boardType__bll     = 4,
    icubCanProto_boardType__skin    = 5,
    icubCanProto_boardType__strain  = 6,
    icubCanProto_boardType__mais    = 7,
    icubCanProto_boardType__2foc    = 8,
    icubCanProto_boardType__6sg     = 9,
    icubCanProto_boardType__unknown = 255
} icubCanProto_boardType_t;




/** @typedef    typedef     uint8_t             icubCanProto_canBoardAddress_t
    @brief      contains can address of board. 
                the value must be in the range [1, E] 
 **/
typedef     uint8_t             icubCanProto_canBoardAddress_t;



/** @typedef    typedef     uint8_t             icubCanProto_command_t
    @brief      contains command belong to icib can protocol.
                its values are defined in icubCanProtocol.h
 **/
typedef     uint8_t             icubCanProto_msgCommand_cmdId_t;



/** @typedef    typedef     uint8_t             icubCanProto_msgCommand_class_t
    @brief      contains command belong to icib can protocol.
                its values are defined in icubCanProtocol_classes.h 
 **/
typedef enum
{
    icubCanProto_msgCmdClass_pollingMotorControl      = ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL,
    icubCanProto_msgCmdClass_periodicMotorControl     = ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL,
    icubCanProto_msgCmdClass_pollingAnalogSensor      = ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR,
    icubCanProto_msgCmdClass_periodicAnalogSensor     = ICUBCANPROTO_CLASS_PERIODIC_ANALOGSENSOR,
    icubCanProto_msgCmdClass_skinBoard                = 4, //ATTENTION: currently it is not used by other fw. Insert here to manage skin message like other calss messages.
    icubCanProto_msgCmdClass_bootLoader               = ICUBCANPROTO_CLASS_BOOTLOADER    
}icubCanProto_msgCommand_class_t;



/** @typedef    typedef int32_t icubCanProto_position_t
    @brief      icubCanProto_position_t contains the position values.
                it is expressed in encoder ticks.
 **/
typedef int32_t             icubCanProto_position_t;



/** @typedef    typedef int16_t icubCanProto_velocity_t
    @brief      icubCanProto_velocity_t contains the velocity values.
 **/
typedef int16_t              icubCanProto_velocity_t;


/** @typedef    typedef int16_t icubCanProto_acceleration_t
    @brief      icubCanProto_acceleration_t contains the acceleration values.
 **/
typedef int16_t              icubCanProto_acceleration_t;


/** @typedef    typedef int16_t icubCanProto_force_t
    @brief      icubCanProto_force_t contains the force values.
 **/
typedef int16_t             icubCanProto_force_t;


/** @typedef    typedef int16_t icubCanProto_torque_t
    @brief      icubCanProto_torque_t contains the torque values.
 **/
typedef int16_t             icubCanProto_torque_t;


/** @typedef    typedef uint32_t icubCanProto_stiffness_t
    @brief      icubCanProto_stiffness_t contains the stiffness values.
 **/
typedef int16_t            icubCanProto_stiffness_t;


/** @typedef    typedef uint32_t icubCanProto_damping_t
    @brief      icubCanProto_damping_t contains the damping values.
 **/
typedef int16_t            icubCanProto_damping_t;


/** @typedef    typedef int16_t icubCanProto_current_t
    @brief      icubCanProto_current_t contains the current values.
                the measurement unit is equal to imA (icub milli Ampere)
                imA = 25000 mA / 32K = 0.762939453125 mA 
 **/
typedef int16_t             icubCanProto_current_t;


/** @typedef    typedef enum icubCanProto_setpoint_type_t
    @brief      contains the possible types of setpoints.
                this is compatible with icubCanProto_setpoint_type_t.
 **/
typedef enum
{
    icubCanProto_setpoint_position                      = 0,
    icubCanProto_setpoint_velocity                      = 1,
    icubCanProto_setpoint_torque                        = 2,
    icubCanProto_setpoint_current                       = 3
} icubCanProto_setpoint_type_t;

/** @typedef    typedef struct icubCanProto_setpoint_position_t
    @brief      icubCanProto_setpoint_position_t contains the position setpoint to be sent can board by can bus
 **/
typedef struct
{
    icubCanProto_position_t       value; 
    icubCanProto_velocity_t       withvelocity; 
} icubCanProto_setpoint_position_t; 


/** @typedef    typedef struct icubCanProto_setpoint_velocity_t
    @brief      icubCanProto_setpoint_velocity_t contains the velocity setpoint to be sent can board by can bus
 **/
typedef struct
{
    icubCanProto_velocity_t       value; 
    icubCanProto_acceleration_t   withacceleration; 
} icubCanProto_setpoint_velocity_t; 


/** @typedef    typedef struct icubCanProto_setpoint_torque_t
    @brief      icubCanProto_setpoint_torque_t contains the torque setpoint to be sent can board by can bus
 **/
typedef struct
{
    icubCanProto_torque_t        value; 
} icubCanProto_setpoint_torque_t; 


/** @typedef    typedef struct icubCanProto_setpoint_current_t
    @brief      icubCanProto_setpoint_current_t contains the current setpoint to be sent can board by can bus
 **/
typedef struct
{
    icubCanProto_current_t       value; 
} icubCanProto_setpoint_current_t; 


/** @typedef    typedef int16_t    icubCanProto_position4calib_t
    @brief      contains position on 16 bits. this type is use only to send position in calibration command.
 **/
typedef int16_t    icubCanProto_position4calib_t;

/* NOTE: velocity is expreess in 16 bits already. */



/** @typedef    typedef struct icubCanProto_calibrator_params_type0_hard_stops_t
    @brief      contains the params in case of icubCanProto_calibration_type0_hard_stops
 **/
typedef struct  
{
    int16_t                           pwmlimit;
    icubCanProto_velocity_t           velocity;
} icubCanProto_calibrator_params_type0_hard_stops_t;

/** @typedef    typedef struct icubCanProto_calibrator_params_type1_abs_sens_analog_t
    @brief      contains the params in case of icubCanProto_calibration_type1_abs_sens_analog_t
 **/
typedef struct  
{
    icubCanProto_position4calib_t     position;
    icubCanProto_velocity_t           velocity;
} icubCanProto_calibrator_params_type1_abs_sens_analog_t;


/** @typedef    typedef struct icubCanProto_calibrator_params_type2_hard_stops_diff_t
    @brief      contains the params in case of icubCanProto_calibration_type2_hard_stops_diff_t
 **/
typedef struct  
{
    int16_t                             pwmlimit;
    icubCanProto_velocity_t           velocity;
} icubCanProto_calibrator_params_type2_hard_stops_diff_t;


/** @typedef    typedef struct icubCanProto_calibrator_params_type3_abs_sens_digital_t
    @brief      contains the params in case of icubCanProto_calibration_type3_abs_sens_digital_t
 **/
typedef struct  
{
    icubCanProto_position4calib_t     position;
    icubCanProto_velocity_t           velocity;
    int16_t                           offset;
} icubCanProto_calibrator_params_type3_abs_sens_digital_t;


/** @typedef    typedef struct icubCanProto_calibrator_params_type4_abs_and_incremental_t
    @brief      contains the params in case of icubCanProto_calibration_type4_abs_and_incremental_t
 **/
typedef struct  
{
    icubCanProto_position4calib_t     position;
    icubCanProto_velocity_t           velocity;
    int16_t                           maxencoder;
} icubCanProto_calibrator_params_type4_abs_and_incremental_t;



/** @typedef    typedef enum icubCanProto_calibration_type_t
    @brief      contains the possible types of calibration.
    @warning    this type is equal to eOmc_calibration_type_t.
                Pay attention to mantain these types synchronized
 **/
typedef enum
{
    icubCanProto_calibration_type0_hard_stops               = 0,
    icubCanProto_calibration_type1_abs_sens_analog          = 1,
    icubCanProto_calibration_type2_hard_stops_diff          = 2,
    icubCanProto_calibration_type3_abs_sens_digital         = 3,
    icubCanProto_calibration_type4_abs_and_incremental      = 4
} icubCanProto_calibration_type_t;



/** @typedef    typedef struct icubCanProto_calibrator_t
    @brief      icubCanProto_calibrator32_t specifies a calibrator with type and parameters for teh new definition of measures
    @warning    this type is similar to eOmc_calibrator32_t, witout padding. Pay attention to mantain these types synchronized
 **/
typedef struct 
{
    icubCanProto_calibration_type_t                  type; 
    union                           
    {
        icubCanProto_calibrator_params_type0_hard_stops_t               type0;
        icubCanProto_calibrator_params_type1_abs_sens_analog_t          type1;
        icubCanProto_calibrator_params_type2_hard_stops_diff_t          type2;
        icubCanProto_calibrator_params_type3_abs_sens_digital_t         type3;
        icubCanProto_calibrator_params_type4_abs_and_incremental_t      type4;
    } params;                                                       /**< the params of the calibrator */   
} icubCanProto_calibrator_t;



/** @typedef    typedef struct icubCanProto_impedance_t
    @brief       specifies a the parameters used in control of kind 
                icubCanProto_controlmode_impedance_pos or icubCanProto_controlmode_impedance_vel.
 **/
typedef struct
{
    icubCanProto_stiffness_t          stiffness;                          /**< the Ks parameter */
    icubCanProto_damping_t            damping;                            /**< the Kd parameter */
    icubCanProto_torque_t             offset;                             /**< the Ko parameter */
} icubCanProto_impedance_t;

typedef uint16_t                icubCanProto_velocityShift_t;

typedef struct
{
    uint8_t estimShiftJointVel;
    uint8_t estimShiftJointAcc;
    uint8_t estimShiftMotorVel;
    uint8_t estimShiftMotorAcc;
} icubCanProto_estimShift_t;

typedef struct 
{
    uint8_t major;
    uint8_t minor;
} icubCanProto_protocolVersion_t;


typedef enum
{
    icubCanProto_controlmode_idle                       = 0x00,
    icubCanProto_controlmode_position                   = 0x01,
    icubCanProto_controlmode_velocity                   = 0x02,
    icubCanProto_controlmode_torque                     = 0x03,
    icubCanProto_controlmode_impedance_pos              = 0x04,
    icubCanProto_controlmode_impedance_vel              = 0x05,
    icubCanProto_controlmode_current                    = 0x06,
    icubCanProto_controlmode_calib_abs_pos_sens         = 0x10,
    icubCanProto_controlmode_calib_hard_stops           = 0x20,
    icubCanProto_controlmode_handle_hard_stops          = 0x30,
    icubCanProto_controlmode_margin_reached             = 0x40,
    icubCanProto_controlmode_calib_abs_and_incremental  = 0x41,
    icubCanProto_controlmode_openloop                   = 0x50
} icubCanProto_controlmode_t;

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @} **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







