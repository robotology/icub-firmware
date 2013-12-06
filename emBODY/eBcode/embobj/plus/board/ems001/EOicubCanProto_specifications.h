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
#ifndef _EO_ICUBCANPROTOCOL_SPECIFICATIONS_H_
#define _EO_ICUBCANPROTOCOL_SPECIFICATIONS_H_


#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       EOicubCanProto_specifications.h
    @brief      This file provides interface to ICUBCANPROTOcol
    @author    valentina.gaggero@iit.it
    @date       14/02/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
#include "iCubCanProtocol.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef     enum             eOicubCanProto_boardType_t
    @brief      contains board type can be mounted on robot.
                Important: its values are defined in according with icubInterface 
                (see downloader.h in canLoader module)
 **/
typedef enum
{
    eo_icubCanProto_boardType__dsp     = 0,
    eo_icubCanProto_boardType__pic     = 1,
    eo_icubCanProto_boardType__2dc     = 2,
    eo_icubCanProto_boardType__4dc     = 3,
    eo_icubCanProto_boardType__bll     = 4,
    eo_icubCanProto_boardType__skin    = 5,
    eo_icubCanProto_boardType__strain  = 6,
    eo_icubCanProto_boardType__mais    = 7,
    eo_icubCanProto_boardType__2foc    = 8,
    eo_icubCanProto_boardType__6sg     = 9,
    eo_icubCanProto_boardType__unknown = 255
} eOicubCanProto_boardType_t;


/** @typedef    typedef     enum             eOicubCanProto_motorAxis_t
    @brief      each CAN board can manage two axises.
                Note: a can board is different from a physical board: the seccond can combine
                two CAN board in a single board object. a Can board ca be identified by unique 
                CAN address on the same CAN bus.
                Each CAN board can manage two axis at most, 
                but a physical board can manage 4 axises at most.
 **/
typedef enum
{
    eo_icubCanProto_jm_index_first      = 0,
    eo_icubCanProto_jm_index_second     = 1
} eOicubCanProto_jm_indexinBoard_t;


/** @typedef    enum { eOicubCanProto_jm_indexInBoard_max = 2 };
    @brief      this is the max number of joint or motor for each canBoard
 **/
enum { eOicubCanProto_jm_indexInBoard_max = 2 };


/** @typedef    typedef     uint8_t             eOicubCanProto_canBoardAddress_t
    @brief      contains can address of board. 
                the value must be in the range [1, E] 
 **/
typedef     uint8_t             eOicubCanProto_canBoardAddress_t;



/** @typedef    typedef     struct             eOicubCanProto_msgDestination_t
    @brief      The destination of message belong to icub can protocol is a 
                particuar axis managed by a board.
 **/
typedef union
{
    struct 
    {
        uint16_t canAddr:8;                 /**< use  eOicubCanProto_canBoardAddress_t */
        uint16_t jm_indexInBoard:8;         /**< use eOicubCanProto_jm_indexinBoard_t.
                                                if the destination is not joint or motor, this field values 0 */
    } s;
    
    uint16_t dest;
} eOicubCanProto_msgDestination_t;

//#define ICUBCANPROTO_MSGDEST_CREATE(jm_indexInBoard, canAddr)   ((eOicubCanProto_msgDestination_t)(jm_indexInBoard<<8 | canAddr))
#define ICUBCANPROTO_MSGDEST_CREATE(jm_indexInBoard, canAddr)   ((uint16_t)(jm_indexInBoard<<8 | canAddr))
#define ICUBCANPROTO_MSGDEST_GET_INDEXINBOARD(msgdest)          (((eOicubCanProto_msgDestination_t)(msgdest)).jm_indexInBoard)


/** @typedef    typedef     uint8_t             eOicubCanProto_command_t
    @brief      contains command belong to icib can protocol.
                its values are defined in eOicubCanProtocol_messages.h TODO
 **/
typedef     uint8_t             eOicubCanProto_msgCommand_cmdId_t;



/** @typedef    typedef     uint8_t             eOicubCanProto_msgCommand_class_t
    @brief      contains command belong to icib can protocol.
                its values are defined in eOicubCanProtocol_messages.h 
 **/
typedef enum
{
    eo_icubCanProto_msgCmdClass_pollingMotorBoard      = 0,
    eo_icubCanProto_msgCmdClass_periodicMotorBoard     = 1,
    eo_icubCanProto_msgCmdClass_pollingSensorBoard     = 2,
    eo_icubCanProto_msgCmdClass_periodicSensorBoard    = 3,
    eo_icubCanProto_msgCmdClass_skinBoard              = 4, //ATTENTION: currently it is not used by other fw. Insert here to manage skin message like other calss messages.
    eo_icubCanProto_msgCmdClass_loader                 = 7    
} eOicubCanProto_msgCommand_class_t;


/** @typedef    typedef     uint8_t             eOicubCanProto_msgCommand_t
    @brief      contains command belong to icib can protocol.
                its values are defined in eOicubCanProtocol_messages.h 
 **/
// typedef union
// {
//     struct
//     {
//         uint16_t class:8;                   /**< use  eOicubCanProto_msgCommand_class_t */
//         uint16_t cmdId:8;                   /**< use  eOicubCanProto_msgCommand_cmdId_t */
//     }s;
//     
//     uint16_t command;
// } eOicubCanProto_msgCommand_t;
typedef struct
{
    uint16_t class:8;                   /**< use  eOicubCanProto_msgCommand_class_t */
    uint16_t cmdId:8;                   /**< use  eOicubCanProto_msgCommand_cmdId_t */
} eOicubCanProto_msgCommand_t;

#define ICUBCANPROTO_MSGCOMMAND_CREATE(class, cmdId)   ((eOicubCanProto_msgCommand_t)(class<<8 | cmdId))

enum { eOicubCanProto_bcastpolicy_maxsize = 4 };

typedef struct
{
    uint8_t val2bcastList[eOicubCanProto_bcastpolicy_maxsize];           //list of value to broadcast
} eOicubCanProto_bcastpolicy_t;



/** @typedef    typedef int32_t             eOicubCanProto_position_t
    @brief      eOicubCanProto_position_t contains the position values.
                it is expressed in encoder ticks.                 
 **/
typedef int32_t             eOicubCanProto_position_t;



/** @typedef    typedef int16_t              eOicubCanProto_velocity_t
    @brief      eOicubCanProto_velocity_t contains the velocity values.
 **/
typedef int16_t              eOicubCanProto_velocity_t;


/** @typedef    typedef int16_t              eOicubCanProto_acceleration_t
    @brief      eOicubCanProto_acceleration_t contains the acceleration values.
 **/
typedef int16_t              eOicubCanProto_acceleration_t;


/** @typedef    typedef int16_t             eOicubCanProto_force_t
    @brief      eOicubCanProto_force_t contains the force values.
 **/
typedef int16_t             eOicubCanProto_force_t;


/** @typedef    typedef int16_t             eOicubCanProto_torque_t
    @brief      eOicubCanProto_torque_t contains the torque values.
 **/
typedef int16_t             eOicubCanProto_torque_t;


/** @typedef    typedef uint32_t             eOicubCanProto_stiffness_t
    @brief      eOicubCanProto_stiffness_t contains the stiffness values.
 **/
typedef int16_t            eOicubCanProto_stiffness_t;


/** @typedef    typedef uint32_t             eOicubCanProto_damping_t
    @brief      eOicubCanProto_damping_t contains the damping values.
 **/
typedef int16_t            eOicubCanProto_damping_t;


/** @typedef    typedef int16_t             eOicubCanProto_current_t
    @brief      eOicubCanProto_current_t contains the current values.
                the measurement unit is equal to imA (icub milli Ampere)
                imA = 25000 mA / 32K = 0.762939453125 mA 
 **/
typedef int16_t             eOicubCanProto_current_t;


/** @typedef    typedef enum eOicubCanProto_setpoint_type_t
    @brief      contains the possible types of setpoints.
                this is compatible with eOicubCanProto_setpoint_type_t.
 **/
typedef enum
{
    eoicubCanProto_setpoint_position                      = 0,
    eoicubCanProto_setpoint_velocity                      = 1,
    eoicubCanProto_setpoint_torque                        = 2,
    eoicubCanProto_setpoint_current                       = 3
} eOicubCanProto_setpoint_type_t;

/** @typedef    typedef struct eOicubCanProto_setpoint_position_t
    @brief      eOicubCanProto_setpoint_position_t contains the position setpoint to be sent can board by can bus
 **/
typedef struct
{
    eOicubCanProto_position_t       value; 
    eOicubCanProto_velocity_t       withvelocity; 
} eOicubCanProto_setpoint_position_t; 


/** @typedef    typedef struct eOicubCanProto_setpoint_velocity_t
    @brief      eOicubCanProto_setpoint_velocity_t contains the velocity setpoint to be sent can board by can bus
 **/
typedef struct
{
    eOicubCanProto_velocity_t       value; 
    eOicubCanProto_acceleration_t   withacceleration; 
} eOicubCanProto_setpoint_velocity_t; 


/** @typedef    typedef struct eOicubCanProto_setpoint_torque_t
    @brief      eOicubCanProto_setpoint_torque_t contains the torque setpoint to be sent can board by can bus
 **/
typedef struct
{
    eOicubCanProto_torque_t        value; 
} eOicubCanProto_setpoint_torque_t; 


/** @typedef    typedef struct eOicubCanProto_setpoint_current_t
    @brief      eOicubCanProto_setpoint_current_t contains the current setpoint to be sent can board by can bus
 **/
typedef struct
{
    eOicubCanProto_current_t       value; 
} eOicubCanProto_setpoint_current_t; 


/** @typedef    typedef int16_t    eOicubCanProto_position4calib_t
    @brief      contains position on 16 bits. this type is use only to send position in calibration command.
 **/
typedef int16_t    eOicubCanProto_position4calib_t;

/* NOTE: velocity is expreess in 16 bits already. */



/** @typedef    typedef struct eOicubCanProto_calibrator_params_type0_hard_stops_t
    @brief      contains the params in case of eOicubCanProto_calibration_type0_hard_stops
 **/
typedef struct  
{
    int16_t                             pwmlimit;
    eOicubCanProto_velocity_t           velocity;
} eOicubCanProto_calibrator_params_type0_hard_stops_t;

/** @typedef    typedef struct eOicubCanProto_calibrator_params_type1_abs_sens_analog_t
    @brief      contains the params in case of eOicubCanProto_calibration_type1_abs_sens_analog_t
 **/
typedef struct  
{
    eOicubCanProto_position4calib_t     position;
    eOicubCanProto_velocity_t           velocity;
} eOicubCanProto_calibrator_params_type1_abs_sens_analog_t;


/** @typedef    typedef struct eOicubCanProto_calibrator_params_type2_hard_stops_diff_t
    @brief      contains the params in case of eOicubCanProto_calibration_type2_hard_stops_diff_t
 **/
typedef struct  
{
    int16_t                             pwmlimit;
    eOicubCanProto_velocity_t           velocity;
} eOicubCanProto_calibrator_params_type2_hard_stops_diff_t;


/** @typedef    typedef struct eOicubCanProto_calibrator_params_type3_abs_sens_digital_t
    @brief      contains the params in case of eOicubCanProto_calibration_type3_abs_sens_digital_t
 **/
typedef struct  
{
    eOicubCanProto_position4calib_t     position;
    eOicubCanProto_velocity_t           velocity;
    int16_t                             offset;
} eOicubCanProto_calibrator_params_type3_abs_sens_digital_t;


/** @typedef    typedef struct eOicubCanProto_calibrator_params_type4_abs_and_incremental_t
    @brief      contains the params in case of eOicubCanProto_calibration_type4_abs_and_incremental_t
 **/
typedef struct  
{
    eOicubCanProto_position4calib_t     position;
    eOicubCanProto_velocity_t           velocity;
    int16_t                            maxencoder;
} eOicubCanProto_calibrator_params_type4_abs_and_incremental_t;



/** @typedef    typedef enum eOicubCanProto_calibration_type_t
    @brief      contains the possible types of calibration.
    @warning   this type is equal to eOmc_calibration_type_t.
                Pay attention to mantain these types synchronized
 **/
typedef enum
{
    eo_icubCanProto_calibration_type0_hard_stops               = 0,
    eo_icubCanProto_calibration_type1_abs_sens_analog          = 1,
    eo_icubCanProto_calibration_type2_hard_stops_diff          = 2,
    eo_icubCanProto_calibration_type3_abs_sens_digital         = 3,
    eo_icubCanProto_calibration_type4_abs_and_incremental      = 4
} eOicubCanProto_calibration_type_t;



/** @typedef    typedef struct eOicubCanProto_calibrator_t
    @brief      eOicubCanProto_calibrator32_t specifies a calibrator with type and parameters for teh new definition of measures
    @warning    this type is similar to eOmc_calibrator32_t, witout padding. Pay attention to mantain these types synchronized
 **/
typedef struct 
{
    eOicubCanProto_calibration_type_t                  type; 
    union                           
    {
        eOicubCanProto_calibrator_params_type0_hard_stops_t               type0;
        eOicubCanProto_calibrator_params_type1_abs_sens_analog_t          type1;
        eOicubCanProto_calibrator_params_type2_hard_stops_diff_t          type2;
        eOicubCanProto_calibrator_params_type3_abs_sens_digital_t         type3;
        eOicubCanProto_calibrator_params_type4_abs_and_incremental_t      type4;
    } params;                                                       /**< the params of the calibrator */   
} eOicubCanProto_calibrator_t;



/** @typedef    typedef struct eOmc_impedance_t
    @brief      eOmc_impedance_t specifies a the parameters used in control of kind 
                eomc_controlmode_impedance_pos or eomc_controlmode_impedance_vel.
 **/
typedef struct
{
    eOicubCanProto_stiffness_t          stiffness;                          /**< the Ks parameter */
    eOicubCanProto_damping_t            damping;                            /**< the Kd parameter */
    eOicubCanProto_torque_t             offset;                             /**< the Ko parameter */                      
} eOicubCanProto_impedance_t;

typedef uint16_t                eOicubCanProto_velocityShift_t;

typedef struct
{
    uint8_t estimShiftJointVel;
    uint8_t estimShiftJointAcc;
    uint8_t estimShiftMotorVel;
    uint8_t estimShiftMotorAcc;
} eOicubCanProto_estimShift_t;

typedef struct 
{
    uint8_t major;
    uint8_t minor;
} eOicubCanProto_protocolVersion_t;


typedef enum
{
    eo_icubCanProto_controlmode_idle					    = 0x00,
    eo_icubCanProto_controlmode_position 					= 0x01,
    eo_icubCanProto_controlmode_velocity					= 0x02,
    eo_icubCanProto_controlmode_torque						= 0x03,
    eo_icubCanProto_controlmode_impedance_pos				= 0x04,
    eo_icubCanProto_controlmode_impedance_vel				= 0x05,
    eo_icubCanProto_controlmode_current				        = 0x06,
    eo_icubCanProto_controlmode_calib_abs_pos_sens			= 0x10,
    eo_icubCanProto_controlmode_calib_hard_stops			= 0x20,
    eo_icubCanProto_controlmode_handle_hard_stops			= 0x30,
    eo_icubCanProto_controlmode_margin_reached    			= 0x40,
    eo_icubCanProto_controlmode_calib_abs_and_incremental	= 0x41,
    eo_icubCanProto_controlmode_openloop               	    = 0x50
} eOicubCanProto_controlmode_t;

// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


