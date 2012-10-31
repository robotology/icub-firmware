/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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
#ifndef _EOMOTIONCONTROL_H_
#define _EOMOTIONCONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EoMotionControl.h
	@brief      This header file gives 
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_devervrffdvf Configuation of the NVs for management of the ems board
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoUtilities.h"
#include "EoMeasures.h"



// - public #define  --------------------------------------------------------------------------------------------------





// - declaration of public user-defined types ------------------------------------------------------------------------- 

// -- all the possible enum


/** @typedef    typedef enum eOmc_controlmode_command_t
    @brief      eOmc_controlmode_command_t contains command to set the control mode.
    @warning    On an EMS only modes eomc_controlmode_cmd_idle, eomc_controlmode_cmd_position, eomc_controlmode_cmd_velocity, eomc_controlmode_cmd_torque,
                eomc_controlmode_cmd_impedance_pos, eomc_controlmode_cmd_impedance_vel, and eomc_controlmode_cmd_openloop are allowed.
                On a 2FOC the only possible mode is eomc_controlmode_cmd_current.                
 **/
typedef enum
{
    eomc_controlmode_cmd_position                   = 0x01,
    eomc_controlmode_cmd_torque                     = 0x03,
    eomc_controlmode_cmd_impedance_pos              = 0x04,
    eomc_controlmode_cmd_impedance_vel              = 0x05,
    eomc_controlmode_cmd_current                    = 0x06,
    eomc_controlmode_cmd_velocity                   = 0x07,      /**< velocity control loop */ 
    eomc_controlmode_cmd_openloop                   = 0x50,
    eomc_controlmode_cmd_switch_everything_off      = 0xf0      /**< it imposes a zero current on the motor and also turns the pwm off */    
} eOmc_controlmode_command_t;


/** @typedef    typedef enum eOmc_controlmode_t
    @brief      eOmc_controlmode_t contains all the possible modes for motion control.
    @warning    On an EMS only modes eomc_controlmode_idle, eomc_controlmode_position, eomc_controlmode_velocity, eomc_controlmode_torque,
                eomc_controlmode_impedance_pos, eomc_controlmode_impedance_vel, and eomc_controlmode_openloop are allowed.
                On a 2FOC the only possible mode is eomc_controlmode_current.
                when command eomc_controlmode_cmd_switch_everything_off is received the motor controller is in eomc_controlmode_idle.
                when command calibration (see eOmc_calibrator_t in joint commands) the motor control is in eomc_controlmode_calib.
 **/
typedef enum
{
    eomc_controlmode_idle                       = 0x00,     /**< pid and pwm off*/
    eomc_controlmode_position                   = 0x01,
    eomc_controlmode_velocity_pos               = 0x02,     /**< control mode is in position, but the control mode switch to eomc_controlmode_velocity_pos
                                                                 automatically when it receives a velocity set setpoint. 
                                                                 This mode is called "velocity" in icub can proto */ 
    eomc_controlmode_torque                     = 0x03,
    eomc_controlmode_impedance_pos              = 0x04,
    eomc_controlmode_impedance_vel              = 0x05,
    eomc_controlmode_current                    = 0x06, 
    eomc_controlmode_velocity                   = 0x07,      /**< velocity control loop */     
    eomc_controlmode_openloop                   = 0x50,
    eomc_controlmode_calib                      = 0xfe      /**< it means joint is in calibration, without specifing wich type of calibartion joint is using. this value doesn't belong to icub can proto. */ 
} eOmc_controlmode_t;

/** @typedef    typedef enum eOmc_motionmonitormode_t
    @brief      contains all the possible modes for motion monitoring.
                It is used to configure the reporting of the motion in relation to reaching a given setpoint,
                as the message motiondone over CAN.    
 **/
typedef enum
{
    eomc_motionmonitormode_dontmonitor      = 0,            /**< the motion is not monitored */
    eomc_motionmonitormode_untilreached     = 1,            /**< the motion is monitored only until the setpoint is reached (even if the setpoint is changed before being reached) */
    eomc_motionmonitormode_forever          = 2             /**< the motion is monitored vs any current of future setpoint until the mode is explicitly changed */
} eOmc_motionmonitormode_t;


/** @typedef    typedef enum eOmc_motionmonitorstatus_t
    @brief      contains the possible status for motion monitoring.
                It is used to reporting inside the status of teh joint about the reaching of a given setpoint,
                as the message motiondone over CAN.    
 **/
typedef enum
{
    eomc_motionmonitorstatus_notmonitored           = EOK_int08dummy,       /**< the motion is not monitored vs a desired setpoint */
    eomc_motionmonitorstatus_setpointnotreachedyet  = 0,                    /**< the motion is monitored but the desired setpoint is not reached yet */
    eomc_motionmonitorstatus_setpointisreached      = 1                     /**< the motion is monitored and the desired setpoint is reached */
} eOmc_motionmonitorstatus_t;



/** @typedef    typedef enum eOmc_controlstatus_t
    @brief      contains the possible status for a motor control.
 **/
typedef enum
{
    eomc_controlstatus_idle                     = 0x00,
    eomc_controlstatus_running                  = 0x01
} eOmc_controlstatus_t;


/** @typedef    typedef enum eOmc_setpoint_type_t
    @brief      contains the possible types of setpoints.
 **/
typedef enum
{
    eomc_setpoint_position                      = 0,
    eomc_setpoint_velocity                      = 1,
    eomc_setpoint_torque                        = 2,
    eomc_setpoint_current                       = 3
} eOmc_setpoint_type_t;


/** @typedef    typedef enum eOmc_calibration_types_t
    @brief      contains the possible types of calibration.
 **/
typedef enum
{
    eomc_calibration_type0_hard_stops               = 0,
    eomc_calibration_type1_abs_sens_analog          = 1,
    eomc_calibration_type2_hard_stops_diff          = 2,
    eomc_calibration_type3_abs_sens_digital         = 3,
    eomc_calibration_type4_abs_and_incremental      = 4
} eOmc_calibration_type_t;

EO_VERIFYproposition(isCalibrationTypeCompatiblewithicubcanproto, (eomc_calibration_type0_hard_stops == 0));
EO_VERIFYproposition(isCalibrationTypeCompatiblewithicubcanproto, (eomc_calibration_type1_abs_sens_analog == 1));
EO_VERIFYproposition(isCalibrationTypeCompatiblewithicubcanproto, (eomc_calibration_type2_hard_stops_diff == 2));
EO_VERIFYproposition(isCalibrationTypeCompatiblewithicubcanproto, (eomc_calibration_type3_abs_sens_digital == 3));
EO_VERIFYproposition(isCalibrationTypeCompatiblewithicubcanproto, (eomc_calibration_type4_abs_and_incremental == 4));


// -- all the possible data service structures


/** @typedef    typedef struct eOmc_calibrator16_params_type0_hard_stops_t
    @brief      contains the params in case of eomc_calibration_type0_hard_stops
 **/
typedef struct  
{
    int16_t                    pwmlimit;
    int16_t                    velocity;
} eOmc_calibrator16_params_type0_hard_stops_t;


/** @typedef    typedef struct eOmc_calibrator16_params_type1_abs_sens_analog_t
    @brief      contains the params in case of eomc_calibration_type1_abs_sens_analog_t
 **/
typedef struct  
{
    int16_t                    position;
    int16_t                    velocity;
} eOmc_calibrator16_params_type1_abs_sens_analog_t;


/** @typedef    typedef struct eOmc_calibrator16_params_type2_hard_stops_diff_t
    @brief      contains the params in case of eomc_calibration_type2_hard_stops_diff_t
 **/
typedef struct  
{
    int16_t                    pwmlimit;
    int16_t                    velocity;
} eOmc_calibrator16_params_type2_hard_stops_diff_t;


/** @typedef    typedef struct eOmc_calibrator16_params_type3_abs_sens_digital_t
    @brief      contains the params in case of eomc_calibration_type3_abs_sens_digital_t
 **/
typedef struct  
{
    int16_t                    position;
    int16_t                    velocity;
    int16_t                    offset;
} eOmc_calibrator16_params_type3_abs_sens_digital_t;


/** @typedef    typedef struct eOmc_calibrator16_params_type4_abs_and_incremental_t
    @brief      contains the params in case of eomc_calibration_type4_abs_and_incremental_t
 **/
typedef struct  
{
    int16_t                    position;
    int16_t                    velocity;
    int16_t                    maxencoder;
} eOmc_calibrator16_params_type4_abs_and_incremental_t;


/** @typedef    typedef struct eOmc_calibrator_params_type0_hard_stops_t
    @brief      contains the params in case of eomc_calibration_type0_hard_stops
 **/
typedef struct  
{
    int16_t                     pwmlimit;
    eOmeas_velocity_t           velocity;
} eOmc_calibrator_params_type0_hard_stops_t;


/** @typedef    typedef struct eOmc_calibrator_params_type1_abs_sens_analog_t
    @brief      contains the params in case of eomc_calibration_type1_abs_sens_analog_t
 **/
typedef struct  
{
    eOmeas_position_t           position;
    eOmeas_velocity_t           velocity;
} eOmc_calibrator_params_type1_abs_sens_analog_t;


/** @typedef    typedef struct eOmc_calibrator_params_type2_hard_stops_diff_t
    @brief      contains the params in case of eomc_calibration_type2_hard_stops_diff_t
 **/
typedef struct  
{
    int16_t                     pwmlimit;
    eOmeas_velocity_t           velocity;
} eOmc_calibrator_params_type2_hard_stops_diff_t;


/** @typedef    typedef struct eOmc_calibrator_params_type3_abs_sens_digital_t
    @brief      contains the params in case of eomc_calibration_type3_abs_sens_digital_t
 **/
typedef struct  
{
    eOmeas_position_t           position;
    eOmeas_velocity_t           velocity;
    int32_t                     offset;
} eOmc_calibrator_params_type3_abs_sens_digital_t;


/** @typedef    typedef struct eOmc_calibrator_params_type4_abs_and_incremental_t
    @brief      contains the params in case of eomc_calibration_type4_abs_and_incremental_t
 **/
typedef struct  
{
    eOmeas_position_t           position;
    eOmeas_velocity_t           velocity;
    uint32_t                    maxencoder;
} eOmc_calibrator_params_type4_abs_and_incremental_t;




// -- all the possible data holding structures used in a joint


/** @typedef    typedef struct eOmc_PID16_t
    @brief      eOmc_PID16_t contains the values required to configure a PID control compatible with CAN board.
                For the meaning of the values of a PID to be used on a CAN board see ...
                On a ARM-based board, the values kp, ki, kd, limitonlintegral, linitonoutput and offset represent
                the integer part of the floating point values which are effectively used.
 **/
typedef struct              // size is 2+2+2+2+2+2+1+3+0 bytes
{
    int16_t                 kp;                 /**< proportional gain */
    int16_t                 ki;                 /**< integral gain */
    int16_t                 kd;                 /**< derivative gain */
    int16_t                 limitonintegral;    /**< limit of integral term */ 
    int16_t                 limitonoutput;      /**< limit of the output of the pid */
    int16_t                 offset;             /**< the k0 in the pid formula */
    int8_t                  scale;              /**< scale factor for the gains (used for a rigth shift */
    uint8_t                 filler03[3];        /**< filler to make struct a multiple of 4 */
} eOmc_PID16_t;             EO_VERIFYsizeof(eOmc_PID16_t, 16);


typedef struct              // size is 4+4+4+4+4+4+0 bytes
{
    float32_t               kp;                 /**< proportional gain */
    float32_t               ki;                 /**< integral gain */
    float32_t               kd;                 /**< derivative gain */
    float32_t               limitonintegral;    /**< limit of integral term */ 
    float32_t               limitonoutput;      /**< limit of the output of the pid */
    float32_t               offset;             /**< the k0 in the pid formula */
} eOmc_PIDfl_t;             EO_VERIFYsizeof(eOmc_PIDfl_t, 24);

// the PID to be used is a PID16
typedef eOmc_PID16_t eOmc_PID_t;



/** @typedef    typedef struct eOmc_impedance_t
    @brief      eOmc_impedance_t specifies a the parameters used in control of kind 
                eomc_controlmode_impedance_pos or eomc_controlmode_impedance_vel.
 **/
typedef struct                  // size is 4+4+2+2+0 = 12
{
    eOmeas_stiffness_t          stiffness;                          /**< the Ks parameter */
    eOmeas_damping_t            damping;                            /**< the Kd parameter */
    eOmeas_torque_t             offset;                             /**< the Ko parameter */
    uint8_t                     filler02[2];                        
} eOmc_impedance_t;             EO_VERIFYsizeof(eOmc_impedance_t, 12);



/** @typedef    typedef struct eOmc_calibrator16_t
    @brief      eOmc_calibrator16_t specifies a calibrator with type and parameters
 **/
typedef struct                  // size is 1+1+2+2+2+0 = 8
{
    uint8_t                     filler1[1];                         /**< in front to match the memory mapping of the can message */
    eOenum08_t                  type;                               /**< use eOmc_calibration_type_t */
    union                           
    {
        uint16_t                                                any[3];
        eOmc_calibrator16_params_type0_hard_stops_t             type0;
        eOmc_calibrator16_params_type1_abs_sens_analog_t        type1;
        eOmc_calibrator16_params_type2_hard_stops_diff_t        type2;
        eOmc_calibrator16_params_type3_abs_sens_digital_t       type3;
        eOmc_calibrator16_params_type4_abs_and_incremental_t    type4;
    } params;                                                       /**< the params of the calibrator */   
} eOmc_calibrator16_t;           EO_VERIFYsizeof(eOmc_calibrator16_t, 8);



/** @typedef    typedef struct eOmc_calibrator32_t
    @brief      eOmc_calibrator32_t specifies a calibrator with type and parameters for teh new definition of measures
 **/
typedef struct                  // size is 1+3+4*3 = 16
{
    eOenum08_t                  type;                               /**< use eOmc_calibration_type_t */
    uint8_t                     filler03[3];
    union                           
    {
        uint32_t                                                any[3];
        eOmc_calibrator_params_type0_hard_stops_t               type0;
        eOmc_calibrator_params_type1_abs_sens_analog_t          type1;
        eOmc_calibrator_params_type2_hard_stops_diff_t          type2;
        eOmc_calibrator_params_type3_abs_sens_digital_t         type3;
        eOmc_calibrator_params_type4_abs_and_incremental_t      type4;
    } params;                                                       /**< the params of the calibrator */   
} eOmc_calibrator32_t;           EO_VERIFYsizeof(eOmc_calibrator32_t, 16);

typedef eOmc_calibrator32_t eOmc_calibrator_t;

/*  This proposition is used to check if used calibrator is eOmc_calibrator32_t.
    This check is important because calibrators'params are sent to can and so conrveted. 
    If used calibrator is a different one, please modify converter object. */
EO_VERIFYproposition(isusedcalibrator32, (sizeof(eOmc_calibrator_t) == sizeof(eOmc_calibrator32_t)));


/** @typedef    typedef struct eOmc_setpoint_t
    @brief      eOmc_setpoint_t contains the setpoint to be sent to a pid
 **/
typedef struct              // size is 1+3+8+0 = 12
{
    eOenum08_t              type;                   /**< use eOmc_setpoint_type_t */
    uint8_t                 filler03[3];
    union
    {
        uint8_t             sizeofunionis08[8];     
        struct 
        { 
            eOmeas_position_t       value; 
            eOmeas_velocity_t       withvelocity; 
        } position;
        struct 
        { 
            eOmeas_velocity_t       value; 
            eOmeas_acceleration_t   withacceleration; 
        } velocity;
        struct 
        { 
            eOmeas_torque_t        value; 
        } torque;
	    struct
        { 
            eOmeas_current_t       value; 
        } current;
    }to;                                        /**< the uinion containing the value field and optional param */
} eOmc_setpoint_t;              EO_VERIFYsizeof(eOmc_setpoint_t, 12);





// -- all the possible data holding structures used in a motor

// -- the definition of a joint


///** @typedef    typedef uint8_t  eOmc_jointUniqueId_t
//    @brief      eOmc_jointUniqueId_t contains the values required to identify a joint 
//                of robot univocally on a body part
// **/
//typedef uint8_t  eOmc_jointUniqueId_t;
//
//
///** @typedef    typedef uint8_t  eOmc_jointBoardId_t
//    @brief      eOmc_jointBoardId_t contains the values required to identify a joint on a board univocally.
// **/
//typedef uint8_t  eOmc_jointBoardId_t;


/** @typedef    typedef uint8_t  eOmc_jointId_t
    @brief      eOmc_jointId_t contains the values required to identify a joint;
                this type doesn't specify if it identifies a joint on a board or body part univocally,
                unlike eOmc_jointBoardId_t and eOmc_jointUniqueId_t. 
 **/
typedef uint16_t  eOmc_jointId_t;




/** @typedef    typedef struct eOmc_joint_config_t
    @brief      eOmc_joint_config_t contains the values required to configure a joint
 **/
typedef struct                  // size is: 16+16+16+12+4+4+2+1+1+4+4+2+1+1+2+2+0 = 88
{
    eOmc_PID_t                  pidposition;                /**< the pid for position control */
    eOmc_PID_t                  pidvelocity;                /**< the pid for velocity control */
    eOmc_PID_t                  pidtorque;                  /**< the pid for torque control */
    eOmc_impedance_t            impedance;                  /**< the impedance to use in control of the relevant kind */                 
    eOmeas_position_t           minpositionofjoint;         /**< the minimum position of the joint */
    eOmeas_position_t           maxpositionofjoint;         /**< the maximum position of the joint */
    eOmeas_time_t               velocitysetpointtimeout;    /**< max time between two setpoints in eomc_controlmode_velocity before going back to eomc_controlmode_position */
    eOenum08_t                  holder01FFU00;              /**< holder of a variable for future use */                              
    eOenum08_t                  motionmonitormode;          /**< use values from eOmc_motionmonitormode_t. it tells if and how to monitor the motion. the result is placed inside jstatus.jstatusbasic.motionmonitorstatus */
    eOutil_emulfloat32_t        encoderconversionfactor;
    eOutil_emulfloat32_t        encoderconversionoffset;
    eOutil_chameleon_descr_t    des02FORjstatuschamaleon04[2]; /**< accomodates up to 2 descriptors for a chameleon of 4 byte in the joint status. */
    uint8_t                     holder01FFU01;              /**< holder of a variable for future use */
    uint8_t                     holder01FFU02;              /**< holder of a variable for future use */
    uint16_t                    holder02FFU03;              /**< holder of a variable for future use */
    uint16_t                    holder02FFU04;              /**< holder of a variable for future use */
} eOmc_joint_config_t;          EO_VERIFYsizeof(eOmc_joint_config_t, 88);




/** @typedef    typedef struct eOmc_joint_status_basic_t
    @brief      eOmc_joint_status_basic_t contains the basic status of the joint
 **/
typedef struct                  // size is: 4+4+4+2+1+1+0 = 16
{
    eOmeas_position_t           position;                   /**< the position of the joint */           
    eOmeas_velocity_t           velocity;                   /**< the velocity of the joint */          
    eOmeas_acceleration_t       acceleration;               /**< the acceleration of the joint */       
    eOmeas_torque_t             torque;                     /**< the torque of the joint when locally measured */
    eOenum08_t                  motionmonitorstatus;        /**< use eOmc_motionmonitorstatus_t. it is eomc_motionmonitorstatus_notmonitored unless the monitor is activated in jconfig.motionmonitormode */  
    eOenum08_t                  controlmodestatus;          /**< use eOmc_controlmode_t. */
} eOmc_joint_status_basic_t;    EO_VERIFYsizeof(eOmc_joint_status_basic_t, 16);



/** @typedef    typedef struct eOmc_joint_status_ofpid_t
    @brief      eOmc_joint_status_ofpid_t contains the status of a PID. The variables reference, error, and output are defined as
                signed integers of 32 bits, but the must contain the relevant measurement units (eOmeas_position_t, eOmeas_velocity_t,
                eOmeas_torque_t, or eOmeas_current_t).
 **/
typedef struct                  // size is: 4+4+4+0 = 12
{
    int32_t                     reference;              /**< the reference of the pid. it can be position, velocity, torque, current  */
    int32_t                     error;                  /**< the error of the pid */ 
    int32_t                     output;                 /**< the output of the pid */ 
} eOmc_joint_status_ofpid_t;    EO_VERIFYsizeof(eOmc_joint_status_ofpid_t, 12);


/** @typedef    typedef struct eOmc_joint_status_t
    @brief      eOmc_joint_status_t contains the status of a joint. 
 **/
typedef struct                  // size is:  16+12+4 = 32
{
    eOmc_joint_status_basic_t   basic;              /**< the basic status */
    eOmc_joint_status_ofpid_t   ofpid;              /**< the pid status   */ 
    uint8_t                     chamaleon04[4];     /**< the content of these bytes can be configured with eOmc_joint_config_t::upto04descrFORjstatuschamaleon04 to contain 0, 1, 2, 3, 4 variables */
} eOmc_joint_status_t;          EO_VERIFYsizeof(eOmc_joint_status_t, 32);



typedef struct                  // size is 2+2+4 = 8
{
    eOmeas_torque_t             externallymeasuredtorque;   /**< the torque at the joint when externally measured or estimated */
    uint16_t                    holder02FFU01;              /**< holder of a variable for future use */
    uint32_t                    holder04FFU02;              /**< holder of a variable for future use */
} eOmc_joint_inputs_t;          EO_VERIFYsizeof(eOmc_joint_inputs_t, 8);



typedef struct                  // size is 16+12+1+1+1+1+0 = 32
{
    eOmc_calibrator_t           calibration;                /**< the calibrator to use */
    eOmc_setpoint_t             setpoint;                   /**< the setpoint of the joint */
    eObool_t                    stoptrajectory;             /**< it is an order to stop the current trajectory on the joint*/
    eOenum08_t                  controlmode;            /**< use values from eOmc_controlmode_command_t*/
    uint8_t                     holder01FFU02;              /**< holder of a variable for future use */
    uint8_t                     holder01FFU03;              /**< holder of a variable for future use */
} eOmc_joint_commands_t;        EO_VERIFYsizeof(eOmc_joint_commands_t, 32);


typedef struct                  // size is 88+32+8+32+0 = 160
{
    eOmc_joint_config_t         jconfig;                    /**< the configuration of the joint */
    eOmc_joint_status_t         jstatus;                    /**< the status of the joint */
    eOmc_joint_inputs_t         jinputs;                    /**< it contains all the values that a host can send to a joint as inputs */
    eOmc_joint_commands_t       jcmmnds;                    /**< it contains all the commands that a host can send to a joint */
} eOmc_joint_t;                 EO_VERIFYsizeof(eOmc_joint_t, 160);




// -- the definition of a motor


///** @typedef    typedef uint8_t  eOmc_motorUniqueId_t
//    @brief      eOmc_motorUniqueId_t contains the values required to identify a motor 
//                of robot univocally on a body part
// **/
//typedef uint8_t  eOmc_motorUniqueId_t;
//
//
///** @typedef    typedef uint8_t  eOmc_motorBoardId_t
//    @brief      eOmc_motorBoardId_t contains the values required to identify a motor on a board.
// **/
//typedef uint8_t  eOmc_motorBoardId_t;


/** @typedef    typedef uint8_t  eOmc_jointId_t
    @brief      eOmc_jointId_t contains the values required to identify a joint;
                this type doesn't specify if it identifies a joint on a board or body part univocally,
                unlike eOmc_motorBoardId_t and eOmc_motorUniqueId_t. 
 **/
typedef uint16_t  eOmc_motorId_t;


/** @typedef    typedef struct eOmc_motor_config_t
    @brief      eOmc_motor_config_t contains the values required to configure a motor
    @warning    This struct must be of fixed size and multiple of 4.
 **/
typedef struct                  // size is: 16+4+2+2+0 = 24
{
    eOmc_PID_t                  pidcurrent;                 /**< the pid for current control */
    eOmeas_velocity_t           maxvelocityofmotor;         /**< the maximum velocity in the motor */
    eOmeas_current_t            maxcurrentofmotor;          /**< the maximum current in the motor */
    eOutil_chameleon_descr_t    des02FORmstatuschamaleon04[2]; /**< accomodates up to 2 descriptors for a chameleon of 4 bytes. */
} eOmc_motor_config_t;          EO_VERIFYsizeof(eOmc_motor_config_t, 24);


/** @typedef    typedef struct eOmc_motor_status_t
    @brief      eOmc_motor_status_t contains the status of a motor
    @warning    This struct must be of fixed size and multiple of 4.
 **/
typedef struct                  // size is: 4+4+2+2+0 = 12
{
    eOmeas_position_t           position;                   /**< the position of the motor */         
    eOmeas_velocity_t           velocity;                   /**< the velocity of the motor */ 
    eOmeas_current_t            current;                    /**< the current of the motor */  
    uint8_t                     filler02[2];
} eOmc_motor_status_basic_t;    EO_VERIFYsizeof(eOmc_motor_status_basic_t, 12);


/** @typedef    typedef struct eOmc_motor_status_t
    @brief      eOmc_motor_status_t contains the status of a motor
    @warning    This struct must be of fixed size and multiple of 4.
 **/
typedef struct                  // size is: 12+4+0 = 16
{
    eOmc_motor_status_basic_t   basic;               /**< the basic status of a motor */
    uint8_t                     chamaleon04[4];         /**< its content is configured with eOmc_motor_config_t::des02FORmstatuschamal04 */
} eOmc_motor_status_t;          EO_VERIFYsizeof(eOmc_motor_status_t, 16);



typedef struct                  // size is 24+16+0 = 40
{
    eOmc_motor_config_t         mconfig;                    /**< the configuration of the motor */
    eOmc_motor_status_t         mstatus;                    /**< the status of the motor */   
} eOmc_motor_t;                 EO_VERIFYsizeof(eOmc_motor_t, 40); 
 

// -- the definition of a controller containing a given number of joints and motors  

/** @typedef    typedef enum eOmc_stateofcontroller_t
    @brief      contains the possible states of the controller.
 **/
typedef enum
{
    eomc_stateofcontroller_config                       = 0,
    eomc_stateofcontroller_ready                        = 1,
    eomc_stateofcontroller_active                       = 2
} eOmc_stateofcontroller_t;
 

typedef struct                  // size is 4+4+0 = 8
{
    eOreltime_t                 durationofctrlloop;         /**< the duration of the control-loop in micro-sec. its default is 1000. so far it cannot be changed. */
    uint8_t                     filler04[4];                /**< for future use */
} eOmc_controller_config_t;     EO_VERIFYsizeof(eOmc_controller_config_t, 8); 


typedef struct                  // size is 1+1+1+1+2+2+0 = 8
{
    eObool_t                    alljomoinitted;             /**< it is eobool_true only when every joint and motor is initted */
    uint8_t                     numofjoints;                /**< the number of joints */   
    uint8_t                     numofmotors;                /**< the number of motors */ 
    eOenum08_t                  stateofcontroller;          /**< it holds a value from enum eOmc_stateofcontroller_t */               
    eO16flags_t                 flagsinittedjoints;         /**< bit position 0 (1, 2, ..) keeps 1 if the joint 0 (1, 2, ..) is fully initted. */
    eO16flags_t                 flagsinittedmotors;         /**< bit position 0 (1, 2, ..) keeps 1 if the motor 0 (1, 2, ..) is fully initted. */ 
} eOmc_controller_status_t;     EO_VERIFYsizeof(eOmc_controller_status_t, 8); 


typedef struct                  // size is 1+7+0 = 8
{
    eOenum08_t                  go2stateofcontroller;       /**< it forces a transition towards the specified state. use a value from enum eOmc_stateofcontroller_t */
    uint8_t                     filler07[7];                /**< for future use */
} eOmc_controller_commands_t;   EO_VERIFYsizeof(eOmc_controller_commands_t, 8); 


typedef struct                  // size is 8+8+8+0 = 24
{
    eOmc_controller_config_t    cconfig;                    /**< controller configuration */
    eOmc_controller_status_t    cstatus;                    /**< controller status  */
    eOmc_controller_commands_t  ccmmnds;                    /**< controller commands  */
} eOmc_controller_t;            EO_VERIFYsizeof(eOmc_controller_t, 24); 




typedef struct
{
    uint16_t tresh; /**< Current threshold  */
    uint16_t time;  /**< Filter time parameter  */
} eOmc_i2tParams_t;

// typedef struct
// {
    // uint8_t estimShiftJointVel;
    // uint8_t estimShiftJointAcc;
    // uint8_t estimShiftMotorVel;
    // uint8_t estimShiftMotorAcc;
// } eOmc_estimShift_t;

 
// some considerations by acemor.
// each joint-motor needs 152+40 = 192 bytes.  
// the left leg uses ... 192*6 joint-motor = 1152 bytes
// the left arm uses ... 192*16 joint-motor --> 3072 bytes. .... however: if we split we had: 4*192=768 on the first and 12*192=2304 on the second

// if we signal the full eOmc_joint_status_t (32) and eOmc_motor_status_t (16) -> 48 bytes we shall signal:
// on ems with 2 jm:    -> 48*2     = 96
// on ems with 4 jm:    -> 48*4     = 192
// on ems with 12 jm:   -> 48*12    = 576
// however, ... we can minimally signal only basic status: eOmc_joint_status_basic_t (16) and eOmc_motor_status_basic_t (12) -> 28
// on ems with 2 jm:    -> 28*2     = 56
// on ems with 4 jm:    -> 28*4     = 112
// on ems with 12 jm:   -> 28*12    = 336





// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_devervrffdvf  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




