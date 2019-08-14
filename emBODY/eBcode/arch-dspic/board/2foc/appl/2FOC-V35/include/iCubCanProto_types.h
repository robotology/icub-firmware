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
#include "stdint.h"
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

// definition of board types.
#define ICUBCANPROTO_BOARDTYPE__DSP     0
#define ICUBCANPROTO_BOARDTYPE__PIC     1
#define ICUBCANPROTO_BOARDTYPE__2DC     2
#define ICUBCANPROTO_BOARDTYPE__4DC     3
#define ICUBCANPROTO_BOARDTYPE__BLL     4
#define ICUBCANPROTO_BOARDTYPE__SKIN    5 
#define ICUBCANPROTO_BOARDTYPE__STRAIN  6 
#define ICUBCANPROTO_BOARDTYPE__MAIS    7 
#define ICUBCANPROTO_BOARDTYPE__2FOC    8 
#define ICUBCANPROTO_BOARDTYPE__6SG     9
#define ICUBCANPROTO_BOARDTYPE__JOG     10
#define ICUBCANPROTO_BOARDTYPE__UNKNOWN 255

// skin types
#define ICUBCANPROTO_SKINTYPE__WITHTEMPCOMP    0  
#define ICUBCANPROTO_SKINTYPE__PALMFINGERTIP   1    
#define ICUBCANPROTO_SKINTYPE__WITHOUTTEMPCOMP 2    

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum icubCanProto_boardType_t
    @brief      contains board type can be mounted on robot.
                Important: its values are defined in according with icubInterface 
                (see downloader.h in canLoader module)
 **/
typedef enum
{
    icubCanProto_boardType__dsp     = ICUBCANPROTO_BOARDTYPE__DSP,
    icubCanProto_boardType__pic     = ICUBCANPROTO_BOARDTYPE__PIC,
    icubCanProto_boardType__2dc     = ICUBCANPROTO_BOARDTYPE__2DC,
    icubCanProto_boardType__4dc     = ICUBCANPROTO_BOARDTYPE__4DC,
    icubCanProto_boardType__bll     = ICUBCANPROTO_BOARDTYPE__BLL,
    icubCanProto_boardType__skin    = ICUBCANPROTO_BOARDTYPE__SKIN,
    icubCanProto_boardType__strain  = ICUBCANPROTO_BOARDTYPE__STRAIN,
    icubCanProto_boardType__mais    = ICUBCANPROTO_BOARDTYPE__MAIS,
    icubCanProto_boardType__2foc    = ICUBCANPROTO_BOARDTYPE__2FOC,
    icubCanProto_boardType__6sg     = ICUBCANPROTO_BOARDTYPE__6SG,
    icubCanProto_boardType__jog     = ICUBCANPROTO_BOARDTYPE__JOG,
    icubCanProto_boardType__unknown = ICUBCANPROTO_BOARDTYPE__UNKNOWN
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


/** @typedef    typedef int16_t icubCanProto_pwm_t
    @brief      icubCanProto_pwm_t contains the pwm values.
 **/
typedef int16_t             icubCanProto_pwm_t;



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




/** @typedef    typedef struct icubCanProto_setpoint_current_t
    @brief      icubCanProto_setpoint_current_t contains the current setpoint to be sent can board by can bus
 **/
typedef struct
{
    icubCanProto_pwm_t       value; 
} icubCanProto_setpoint_openloop_t; 

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

/** @typedef    typedef struct icubCanProto_calibrator_params_type6_mais_t
    @brief      contains the params in case of icubCanProto_calibration_type6_mais_t
 **/
typedef struct  
{
    icubCanProto_position4calib_t     position;
    icubCanProto_velocity_t           velocity;
    int16_t                           reserved;
} icubCanProto_calibrator_params_type6_mais_t;

/** @typedef    typedef struct icubCanProto_calibrator_params_type7_hall_sensor_t
    @brief      contains the params in case of icubCanProto_calibrator_params_type7_hall_sensor_t
 **/
typedef struct  
{
    icubCanProto_position4calib_t     position;
    icubCanProto_velocity_t           velocity;
    int16_t                           reserved;
} icubCanProto_calibrator_params_type7_hall_sensor_t;


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
    icubCanProto_calibration_type4_abs_and_incremental      = 4,
    icubCanProto_calibration_type6_mais                     = 6,
    icubCanProto_calibration_type7_hall_sensor              = 7
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
        icubCanProto_calibrator_params_type6_mais_t                     type6;
        icubCanProto_calibrator_params_type7_hall_sensor_t              type7;
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
    icubCanProto_controlmode_impedance_pos              = 0x04,  //TBR to remove
    icubCanProto_controlmode_impedance_vel              = 0x05,  //TBR to remove
    icubCanProto_controlmode_current                    = 0x06,
    icubCanProto_controlmode_mixed                      = 0x07,
    icubCanProto_controlmode_direct                     = 0x08,
    icubCanProto_controlmode_forceIdle                  = 0x09,
    icubCanProto_controlmode_speed_voltage              = 0x0A,
    icubCanProto_controlmode_speed_current              = 0x0B,
            
    icubCanProto_controlmode_openloop                   = 0x50,
    icubCanProto_controlmode_calibration                = 0x60,
    icubCanProto_controlmode_hwFault                    = 0xA0,
    icubCanProto_controlmode_notConfigured              = 0xB0,
    icubCanProto_controlmode_configured                 = 0xB1,
    icubCanProto_controlmode_no_request                 = 0xB2,
    icubCanProto_controlmode_unknownError               = 0xFF
} icubCanProto_controlmode_t;

typedef enum
{
    icubCanProto_interactionmode_stiff                  = 0x00,
    icubCanProto_interactionmode_compliant              = 0x01,
    icubCanProto_interactionmode_unknownError           = 0xFF
} icubCanProto_interactionmode_t;



/** @typedef    typedef enum icubCanProto_skinType_t
    @brief      contains skyn types
    @warning    if you change values of item of this enum, pay attention to 
 **/
typedef enum
{
    icubCanProto_skinType__withtempcomp       = ICUBCANPROTO_SKINTYPE__WITHTEMPCOMP,
    icubCanProto_skinType__palmfingertip      = ICUBCANPROTO_SKINTYPE__PALMFINGERTIP,
    icubCanProto_skinType__withouttempcomp    = ICUBCANPROTO_SKINTYPE__WITHOUTTEMPCOMP
} icubCanProto_skinType_t;


typedef struct
{
    icubCanProto_skinType_t     skintype;
    uint8_t                     period;
    uint8_t                     noload;
} icubCanProto_skinboard_config_t;          //EO_VERIFYsizeof(eOsk_config_board_t, 4);

typedef struct                      
{
    uint8_t                    idstart;
    uint8_t                    idend;
    uint8_t                    flags;
    uint8_t                    shift;
    uint16_t                   CDCoffset;
} icubCanProto_skintriangles_config_t;  


/** @typedef    typedef enum icubCanProto_as_sigmode_t
    @brief      contains signal mode for a general analog sensor
 **/
typedef enum
{
    icubCanProto_as_sigmode_signal             = 0,
    icubCanProto_as_sigmode_dontsignal         = 1
} icubCanProto_as_sigmode_t;


// -- inertial section. in here there are types used to manage the inertial sensors mounted on mtb board.


/** @typedef    typedef enum icubCanProto_inertial_sensorflags_t
    @brief      contains the flags to be used to specify the behaviour of acceleromters / gyroscope in the mtb board.
                note 1: icubCanProto_inertial_sensorflag_internaldigitalaccelerometer is used if the board is version .D
                note 2: icubCanProto_inertial_sensorflag_externaldigitalgyroscope and icubCanProto_inertial_sensorflag_externaldigitalaccelerometer are for the palm board only.
                note 3: the two accelerometers (internal and external) cannot be used at the same time. if for an error both are selected, the internal one is used.
 **/
typedef enum
{
    icubCanProto_inertial_sensorflag_none                             = 0x00,
    icubCanProto_inertial_sensorflag_analogaccelerometer              = 0x01,
    icubCanProto_inertial_sensorflag_internaldigitalaccelerometer     = 0x02,
    icubCanProto_inertial_sensorflag_externaldigitalgyroscope         = 0x04,
    icubCanProto_inertial_sensorflag_externaldigitalaccelerometer     = 0x08   
} icubCanProto_inertial_sensorflags_t;

typedef struct
{
    uint8_t                     enabledsensors; /**< the enabled sensor. fill with bitwise or of icubCanProto_inertial_sensorflags_t */ 
    uint8_t                     period;         /**< the transmission period in ms with range [1, 10] */
} icubCanProto_inertial_config_t;



/** @typedef    typedef enum icubCanProto_strain_saturationInfo_t
    @brief      specifies if a strain channel is saturated and the direction of saturation.
 **/
typedef enum
{
    saturationNONE  = 0,
    saturationLOW   = 1,
    saturationHIGH  = 2
} icubCanProto_strain_saturationInfo_t;

/** @typedef    typedef struct icubCanProto_strain_forceSaturationInfo_t
    @brief      contains info saturation of channels 1, 2 and 3.
 **/
typedef struct
{
    uint8_t thereIsSaturationInAtLeastOneChannel    : 2;    // is either 0 or 1
    uint8_t saturationInChannel_0                   : 2;    // use values from icubCanProto_strain_saturationInfo_t
    uint8_t saturationInChannel_1                   : 2;    // use values from icubCanProto_strain_saturationInfo_t
    uint8_t saturationInChannel_2                   : 2;    // use values from icubCanProto_strain_saturationInfo_t   
} icubCanProto_strain_forceSaturationInfo_t;


/** @typedef    typedef struct icubCanProto_strain_torqueSaturationInfo_t
    @brief      contains info saturation of channels 3, 4 and 5.
 **/
typedef struct
{
    uint8_t thereIsSaturationInAtLeastOneChannel    : 2;    // is either 0 or 1
    uint8_t saturationInChannel_3                   : 2;    // use values from icubCanProto_strain_saturationInfo_t
    uint8_t saturationInChannel_4                   : 2;    // use values from icubCanProto_strain_saturationInfo_t
    uint8_t saturationInChannel_5                   : 2;    // use values from icubCanProto_strain_saturationInfo_t   
} icubCanProto_strain_torqueSaturationInfo_t;



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







