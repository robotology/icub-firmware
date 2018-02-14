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

#ifndef _ICUBCANPROTO_MOTORCONTROLMESSAGES_H_
#define _ICUBCANPROTO_MOTORCONTROLMESSAGES_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       iCubCanProto_motorControlMessages.h
    @brief      This header file gives the definitions of the iCub CAN protocols commands relating to motion control
    @author     valentina.gaggero@iit.it
    @date       12/03/2012
    @ingroup    iCubCanProtocol
**/

/** @ingroup    iCubCanProtocol
    @{
 **/




// - external dependencies --------------------------------------------------------------------------------------------
// empty-section


// - public #define  --------------------------------------------------------------------------------------------------

/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL
/***************************************************************************/
#define ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE                     0
#define ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN                 1
#define ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE                2
#define ICUBCANPROTO_POL_MC_CMD__TOGGLE_VERBOSE                 3
#define ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER              4
#define ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD                 5
#define ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD                6
#define ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE               7
#define ICUBCANPROTO_POL_MC_CMD__MOTION_DONE                    8
#define ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE               9
#define ICUBCANPROTO_POL_MC_CMD__WRITE_FLASH_MEM                10
#define ICUBCANPROTO_POL_MC_CMD__READ_FLASH_MEM                 11
#define ICUBCANPROTO_POL_MC_CMD__GET_ADDITIONAL_INFO            12
#define ICUBCANPROTO_POL_MC_CMD__SET_ADDITIONAL_INFO            13
#define ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT          14
#define ICUBCANPROTO_POL_MC_CMD__SET_MAX_MOTOR_POS              15
#define ICUBCANPROTO_POL_MC_CMD__SET_MIN_MOTOR_POS              16

#define ICUBCANPROTO_POL_MC_CMD__SET_DEBUG_PARAM                18
#define ICUBCANPROTO_POL_MC_CMD__GET_DEBUG_PARAM                19
#define ICUBCANPROTO_POL_MC_CMD__GET_ENCODER_POSITION           20
#define ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_POSITION           21
#define ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_POSITION           22
#define ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_VELOCITY           23
#define ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_VELOCITY           24
#define ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_ACCELER            25
#define ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_ACCELER            26
#define ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE                  27
#define ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE                  28
#define ICUBCANPROTO_POL_MC_CMD__SET_ENCODER_POSITION           29
#define ICUBCANPROTO_POL_MC_CMD__SET_P_GAIN                     30
#define ICUBCANPROTO_POL_MC_CMD__GET_P_GAIN                     31
#define ICUBCANPROTO_POL_MC_CMD__SET_D_GAIN                     32
#define ICUBCANPROTO_POL_MC_CMD__GET_D_GAIN                     33
#define ICUBCANPROTO_POL_MC_CMD__SET_I_GAIN                     34
#define ICUBCANPROTO_POL_MC_CMD__GET_I_GAIN                     35
#define ICUBCANPROTO_POL_MC_CMD__SET_ILIM_GAIN                  36
#define ICUBCANPROTO_POL_MC_CMD__GET_ILIM_GAIN                  37
#define ICUBCANPROTO_POL_MC_CMD__SET_OFFSET                     38
#define ICUBCANPROTO_POL_MC_CMD__GET_OFFSET                     39
#define ICUBCANPROTO_POL_MC_CMD__SET_SCALE                      40
#define ICUBCANPROTO_POL_MC_CMD__GET_SCALE                      41
#define ICUBCANPROTO_POL_MC_CMD__SET_TLIM                       42
#define ICUBCANPROTO_POL_MC_CMD__GET_TLIM                       43
#define ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE             44
#define ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE             45
#define ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY                46
#define ICUBCANPROTO_POL_MC_CMD__SET_PWM_LIMIT                  47
#define ICUBCANPROTO_POL_MC_CMD__GET_PWM_LIMIT                  48

#define ICUBCANPROTO_POL_MC_CMD__SET_BOARD_ID                   50
#define ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID                   51
#define ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_SOURCE              52

#define ICUBCANPROTO_POL_MC_CMD__GET_PID_ERROR                  55
#define ICUBCANPROTO_POL_MC_CMD__GET_CALIBRATION_STATUS         56
#define ICUBCANPROTO_POL_MC_CMD__GET_POSITION_MOVE_REF          57
#define ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_MOVE_REF          58

#define ICUBCANPROTO_POL_MC_CMD__GET_ERROR_STATUS               60
#define ICUBCANPROTO_POL_MC_CMD__GET_ENCODER_VELOCITY           61
#define ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION           62
#define ICUBCANPROTO_POL_MC_CMD__GET_PID_OUTPUT                 63
#define ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION               64 //0x40
#define ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION               65
#define ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION               66 //0x42
#define ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION               67
#define ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY               68
#define ICUBCANPROTO_POL_MC_CMD__GET_MAX_VELOCITY               69
#define ICUBCANPROTO_POL_MC_CMD__GET_ACTIVE_ENCODER_POSITION    70
#define ICUBCANPROTO_POL_MC_CMD__SET_ACTIVE_ENCODER_POSITION    71
#define ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT              72
#define ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY               73
#define ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT                  74 //0x4A
#define ICUBCANPROTO_POL_MC_CMD__SET_OFFSET_ABS_ENCODER         75
#define ICUBCANPROTO_POL_MC_CMD__GET_OFFSET_ABS_ENCODER         76
#define ICUBCANPROTO_POL_MC_CMD__SET_SMOOTH_PID                 77
#define ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID                 78 //0x4E
#define ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID                 79
#define ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS           80 //0x50
#define ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS           81
#define ICUBCANPROTO_POL_MC_CMD__SET_POS_PID                    82 //0x52
#define ICUBCANPROTO_POL_MC_CMD__GET_POS_PID                    83
#define ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS              84 //0x54
#define ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS              85
#define ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT                86 //0x56
#define ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS           87 //0x57
#define ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS           88
#define ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET           89
#define ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET           90
#define ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION           91
#define ICUBCANPROTO_POL_MC_CMD__SET_OPTICAL_ENC_RATIO          92
#define ICUBCANPROTO_POL_MC_CMD__SET_POS_STICTION_PARAMS        93
#define ICUBCANPROTO_POL_MC_CMD__GET_POS_STICTION_PARAMS        94
#define ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_STICTION_PARAMS     95
#define ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_STICTION_PARAMS     96
#define ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_PARAMS               97
#define ICUBCANPROTO_POL_MC_CMD__GET_MOTOR_PARAMS               98
#define ICUBCANPROTO_POL_MC_CMD__SET_MODEL_PARAMS               99
#define ICUBCANPROTO_POL_MC_CMD__GET_MODEL_PARAMS               100
#define ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID                101
#define ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PID                102
#define ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS          103
#define ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PIDLIMITS          104
#define ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID               105
#define ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PID               106
#define ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PIDLIMITS         107
#define ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PIDLIMITS         108
#define ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT            109
#define ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_CURRENT            110
#define ICUBCANPROTO_POL_MC_CMD__SET_PERIODIC_MSG_CONTENTS      111
#define ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS                 112
#define ICUBCANPROTO_POL_MC_CMD__GET_I2T_PARAMS                 113
#define ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS            114
#define ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS            115
#define ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE           116
#define ICUBCANPROTO_POL_MC_CMD__GET_INTERACTION_MODE           117
#define ICUBCANPROTO_POL_MC_CMD__SET_TCFILTER_TYPE              118
#define ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG               119
#define ICUBCANPROTO_POL_MC_CMD__GET_TCFILTER_TYPE              120
#define ICUBCANPROTO_POL_MC_CMD_MAXNUM                          (ICUBCANPROTO_POL_MC_CMD__GET_TCFILTER_TYPE + 1) 



/***************************************************************************/
// messages of class ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL
/***************************************************************************/
#define ICUBCANPROTO_PER_MC_MSG__2FOC                        0
#define ICUBCANPROTO_PER_MC_MSG__POSITION                    1
#define ICUBCANPROTO_PER_MC_MSG__PID_VAL                     2
#define ICUBCANPROTO_PER_MC_MSG__STATUS                      3
#define ICUBCANPROTO_PER_MC_MSG__CURRENT                     4
#define ICUBCANPROTO_PER_MC_MSG__OVERFLOW                    5
#define ICUBCANPROTO_PER_MC_MSG__PRINT                       6
#define ICUBCANPROTO_PER_MC_MSG__VELOCITY                    7
#define ICUBCANPROTO_PER_MC_MSG__PID_ERROR                   8
#define ICUBCANPROTO_PER_MC_MSG__DEBUG                       9
#define ICUBCANPROTO_PER_MC_MSG__MOTOR_POSITION              10
#define ICUBCANPROTO_PER_MC_MSG__MOTOR_SPEED                 11
#define ICUBCANPROTO_PER_MC_MSG__ADDITIONAL_STATUS           12
#define ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT   15

#define ICUBCANPROTO_PER_MC_MSG_MAXNUM                       (ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT + 1)



// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section


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







