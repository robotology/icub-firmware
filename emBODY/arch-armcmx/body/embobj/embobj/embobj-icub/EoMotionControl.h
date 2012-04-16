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



/** @typedef    typedef struct eOmc_PIDq15_t
    @brief      eOmc_PIDq15_t contains the values required to configure a PID control when using q15 arithmetic
    @warning    This struct must be of fixed size and multiple of 4.
    @todo       describe how the 16bit values are defined and how to map to/from floating point
 **/
typedef struct              // size is 2+2+2+2+2+2+1+3+0 bytes
{
    int16_t                 kp;                 /**< proportional gain */
    int16_t                 ki;                 /**< integral gain */
    int16_t                 kd;                 /**< derivative gain */
    int16_t                 limitonintegral;    /**< limit of integral term */ 
    int16_t                 limitonoutput;      /**< limit of the output of the pid */
    int16_t                 offset;             /**< the k0 in the pid formula */
    int8_t                  scale;              /**< scale factor for the gains (usato per shift a destra)*/
    uint8_t                 filler03[3];        /**< filler to make struct a multiple of 4 */
} eOmc_PIDq15_t;            EO_VERIFYsizeof(eOmc_PIDq15_t, 16);

// the PID to be used is a q15
typedef eOmc_PIDq15_t eOmc_PID_t;


/** @typedef    typedef enum eOmc_controlmode_t
    @brief      eOmc_controlmode_t contains all the possible modes for motor control.
    @warning    On an EMS only modes eomc_controlmode_idle, eomc_controlmode_position, eomc_controlmode_velocity, eomc_controlmode_torque,
                eomc_controlmode_impedance_pos, eomc_controlmode_impedance_vel, and eomc_controlmode_openloop are allowed.
                On a 2FOC the only possible mode is eomc_controlmode_current.                
 **/
typedef enum
{
    eomc_controlmode_idle                       = 0x00,
    eomc_controlmode_position                   = 0x01,
    eomc_controlmode_velocity                   = 0x02,
    eomc_controlmode_torque                     = 0x03,
    eomc_controlmode_impedance_pos              = 0x04,
    eomc_controlmode_impedance_vel              = 0x05,
    eomc_controlmode_current                    = 0x06,
    eomc_controlmode_calib_abs_pos_sens         = 0x10,
    eomc_controlmode_calib_hard_stops           = 0x20,
    eomc_controlmode_handle_hard_stops          = 0x30,
    eomc_controlmode_margin_reached             = 0x40,
    eomc_controlmode_calib_abs_and_inc          = 0x41,
    eomc_controlmode_openloop                   = 0x50    
} eOmc_controlmode_t;



/** @typedef    typedef enum eOmc_controlstatus_t
    @brief      eOmc_controlstatus_t contains the possible status for a motor control.
 **/
typedef enum
{
    eomc_controlstatus_idle                     = 0x00,
    eomc_controlstatus_running                  = 0x01
} eOmc_controlstatus_t;


/** @typedef    typedef enum eOmc_setpoint_type_t
    @brief      eOmc_setpoint_type_t contains the possible types of setpoints.
 **/
typedef enum
{
    eomc_setpoint_position                      = 0,
    eomc_setpoint_velocity                      = 1,
    eomc_setpoint_torque                        = 2,
    eomc_setpoint_current                       = 3
} eOmc_setpoint_type_t;



/** @typedef    typedef struct eOmc_setpoint_t
    @brief      eOmc_setpoint_t contains the setpoint to be sent to a pid
    @warning    This struct must be of fixed size and multiple of 4.
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


/** @typedef    typedef struct eOmc_joint_config_t
    @brief      eOmc_joint_config_t contains the values required to configure a joint
    @warning    This struct must be of fixed size and multiple of 4.
 **/
typedef struct                  // size is: 16+16+16+4+4+2+2+4+0 = 64
{
    eOmc_PID_t                  pidposition;                /**< the pid for position control */
    eOmc_PID_t                  pidvelocity;                /**< the pid for velocity control */
    eOmc_PID_t                  pidtorque;                  /**< the pid for torque control */
    eOmeas_position_t           minpositionofjoint;         /**< the minimum position of the joint */
    eOmeas_position_t           maxpositionofjoint;         /**< the maximum position of the joint */
    eOmeas_time_t               velocitysetpointtimeout;    /**< max time between two setpoints in eomc_controlmode_velocity before going back to eomc_controlmode_position */
    // removed ... uint8_t                     velocityshiftfactor;        /**< rigth shift to be applied to the velocity values of mc4 boards.. it could be removed and ... generato dalla ems e solo per la mc4 */
    //uint8_t                     filler05[5];              // instead of filler05 :
    eOutil_chameleon_descriptor_t upto02descrforchameleon02[2];     /**< accomodates up to 2 descriptors for a chameleon of 2 bytes. */
    uint8_t                     filler04[4];
} eOmc_joint_config_t;          EO_VERIFYsizeof(eOmc_joint_config_t, 64);


#warning --> si potrebbe trasformare il eOmeas_time_t in micro-sec a 32 bit. 

/** @typedef    typedef struct eOmc_motor_config_t
    @brief      eOmc_motor_config_t contains the values required to configure a motor
    @warning    This struct must be of fixed size and multiple of 4.
 **/
typedef struct                  // size is: 16+4+2+2+0 = 24
{
    eOmc_PID_t                  pidcurrent;                 /**< the pid for current control */
    eOmeas_velocity_t           maxvelocityofmotor;         /**< the maximum velocity in the motor */
    eOmeas_current_t            maxcurrentofmotor;          /**< the maximum current in the motor */
    //uint8_t                     filler02[2];
    eOutil_chameleon_descriptor_t upto02descrforchameleon06[2]; /**< accomodates up to 2 descriptors for a chameleon of 6 bytes. */
} eOmc_motor_config_t;          EO_VERIFYsizeof(eOmc_motor_config_t, 24);
#warning --> I2T params misssed 


/** @typedef    typedef struct eOmc_joint_status_t
    @brief      eOmc_joint_status_t contains the status of the joint
    @warning    This struct must be of fixed size and multiple of 4.
 **/
typedef struct                  // size is: 4+4+4+2+2+0 = 16
{
    eOmeas_position_t           position;                   /**< the position of the joint */           
    eOmeas_velocity_t           velocity;                   /**< the velocity of the joint */          
    eOmeas_acceleration_t       acceleration;               /**< the acceleration of the joint */       
    eOmeas_torque_t             torque;                     /**< the torque of the joint */
    uint8_t                     chameleon02[2];             /**< these two bytes can be configured with eOmc_joint_config_t::upto02descrforchameleon02 to contain 0, 1, or 2 variables */  
} eOmc_joint_status_t;          EO_VERIFYsizeof(eOmc_joint_status_t, 16);


/** @typedef    typedef struct eOmc_pid_status_t
    @brief      eOmc_pid_status_t contains the status of a generic pid
    @warning    This struct must be of fixed size and multiple of 4.
 **/
typedef struct                  // size is: 4+2+2+0 = 8
{
    int32_t                     reference;              /**< the reference of the pid. as it can be position, velocity, torque, current ... we use the biggest size */
    int16_t                     output;                 /**< the output of the pid */ 
    int16_t                     error;                  /**< the error of the pid */        
} eOmc_PID15_status_t;          EO_VERIFYsizeof(eOmc_PID15_status_t, 8);

typedef eOmc_PID15_status_t eOmc_PID_status; 


/** @typedef    typedef struct eOmc_motor_status_t
    @brief      eOmc_motor_status_t contains the status of a motor
    @warning    This struct must be of fixed size and multiple of 4.
 **/
typedef struct                  // size is: 4+4+2+6+0 = 16
{
    eOmeas_position_t           position;               /**< the position of the motor */         
    eOmeas_velocity_t           velocity;               /**< the velocity of the motor */ 
    eOmeas_current_t            current;                /**< the current of the motor */  
    //uint8_t                     filler06[6];          // we could transform it in a chameleon48_t configurable with eOmc_motor_config_t::filler02 
    uint8_t                     chameleon06[6];         /**< these size bytes can be configured with eOmc_motor_config_t::upto02descrforchameleon06 to contain 0, 1, or 2 variables */
} eOmc_motor_status_t;          EO_VERIFYsizeof(eOmc_motor_status_t, 16);


///** @typedef    typedef struct eOmc_joint_status_t
//    @brief      eOmc_joint_status_t contains the status of a joint with a position control loop
//    @warning    This struct must be of fixed size and multiple of 4.
// **/
//typedef struct                  // size is: 16+12+4+8+0 = 40
//{
//    eOmc_joint_status_t         joint;
//    eOmc_motor_status_t         motor;
//    uint8_t                     filler04[4];
//    eOmc_pid_status_t           pidpos;
//} eOmc_1motorjoint_status_t;    EO_VERIFYsizeof(eOmc_1motorjoint_status_t, 40);


///** @typedef    typedef struct eOmc_joint_status_t
//    @brief      eOmc_joint_status_t contains the status of a joint with a position control loop with the addition of a debug 
//    @warning    This struct must be of fixed and size and multiple of 4.
// **/
//typedef struct                  // size is: 40+8+0 = 48
//{
//    eOmc_1motorjoint_status_t   onemotorjointstatus;
//    eOutil_debug_values_t       debugvalues;
//} eOmc_1motorjoint_status_debug_t;    EO_VERIFYsizeof(eOmc_1motorjoint_status_debug_t, 48);

//#warning --> calibration types and modes are still to be defined


/** @typedef    typedef enum eOmc_calibration_types_t
    @brief      eOmc_calibration_types_t contains the possible types of calibration.
 **/
typedef enum
{
    eomc_calibration_type0_hard_stops               = 0,
    eomc_calibration_type1_abs_sens_analog          = 1,
    eomc_calibration_type2_hard_stops_diff          = 2,
    eomc_calibration_type3_abs_sens_digital         = 3,
    eomc_calibration_type4_abs_and_incremental      = 4
} eOmc_calibration_type_t;


/** @typedef    typedef struct eOmc_calibrator_params_type0_hard_stops_t
    @brief      eOmc_calibrator_params_type0_hard_stops_t contains the params in case of eomc_calibration_type0_hard_stops
 **/
typedef struct  
{
    uint16_t                    pwmlimit;
    uint16_t                    velocity;
} eOmc_calibrator_params_type0_hard_stops_t;


/** @typedef    typedef struct eOmc_calibrator_params_type1_abs_sens_analog_t
    @brief      eOmc_calibrator_params_type1_abs_sens_analog_t contains the params in case of eomc_calibration_type1_abs_sens_analog_t
 **/
typedef struct  
{
    uint16_t                    position;
    uint16_t                    velocity;
} eOmc_calibrator_params_type1_abs_sens_analog_t;


/** @typedef    typedef struct eOmc_calibrator_params_type2_hard_stops_diff_t
    @brief      eOmc_calibrator_params_type2_hard_stops_diff_t contains the params in case of eomc_calibration_type2_hard_stops_diff_t
 **/
typedef struct  
{
    uint16_t                    pwmlimit;
    uint16_t                    velocity;
} eOmc_calibrator_params_type2_hard_stops_diff_t;


/** @typedef    typedef struct eOmc_calibrator_params_type3_abs_sens_digital_t
    @brief      eOmc_calibrator_params_type3_abs_sens_digital_t contains the params in case of eomc_calibration_type3_abs_sens_digital_t
 **/
typedef struct  
{
    uint16_t                    position;
    uint16_t                    velocity;
    uint16_t                    offset;
} eOmc_calibrator_params_type3_abs_sens_digital_t;


/** @typedef    typedef struct eOmc_calibrator_params_type4_abs_and_incremental_t
    @brief      eOmc_calibrator_params_type4_abs_and_incremental_t contains the params in case of eomc_calibration_type4_abs_and_incremental_t
 **/
typedef struct  
{
    uint16_t                    position;
    uint16_t                    velocity;
    uint16_t                    maxencoder;
} eOmc_calibrator_params_type4_abs_and_incremental_t;



/** @typedef    typedef struct eOmc_calibrator_t
    @brief      eOmc_calibrator_t specifies a calibrator with type and parameters
 **/
typedef struct                  // size is 1+1+2+2+2+0 = 8
{
    uint8_t                     filler1[1];                         /**< in front to match the memory mapping of the can message */
    eOenum08_t                  type;                               /**< use eOmc_calibration_type_t */
    union                           
    {
        uint16_t                                            any[3];
        eOmc_calibrator_params_type0_hard_stops_t           type0;
        eOmc_calibrator_params_type1_abs_sens_analog_t      type1;
        eOmc_calibrator_params_type2_hard_stops_diff_t      type2;
        eOmc_calibrator_params_type3_abs_sens_digital_t     type3;
        eOmc_calibrator_params_type4_abs_and_incremental_t  type4;
    } params;                                                       /**< the params of the calibrator */   
} eOmc_calibrator_t;           EO_VERIFYsizeof(eOmc_calibrator_t, 8);


#warning --> ricordarsi di chiedere dove metterlo e come usarlo
/** @typedef    typedef struct eOmc_impedance_t
    @brief      eOmc_impedance_t specifies a the parameters used for impedance ...
 **/
typedef struct                  // size is 4+4+2+6+0 = 16
{
    eOmeas_stiffness_t          stiffness;                          /**< the Ks parameter */
    eOmeas_damping_t            dumping;                            /**< the Kd parameter */
    eOmeas_torque_t             offset;                             /**< the Ko parameter */
    uint8_t                     filler06[6];                        
} eOmc_impedance_t;             EO_VERIFYsizeof(eOmc_impedance_t, 16);



// manca la calibrazione ed altro ...



//typedef struct                  // size is 68+8+36+12+1+1+1+1 = 128
//{
//    eOmc_joint_config_t         config;                     /**< the configuration of the joint */
//    eOmc_calibration_t          calibration;                // ok, da definire meglio ....
//    eOmc_joint_status_debug_t   jointstatusdebug;           /**< the configuration of the joint. If a normal joint status is enough, use jointstatusdebug.jointstatus */
//    eOmc_setpoint_t             setpoint;                   /**< the setpoint of the joint */
//    eOenum08_t                  controlmode;                /**< use values from eOmc_controlmode_t */
//    eOenum08_t                  controlstatus;              /**< use values from eOmc_controlstatus_t */
//    eObool_t                    motorenabled;               /**< if eobool_true the motor is enabled via PWM or else */
//    
//    uint8_t                     filler01[1];
//} eOmc_old_joint_t;                 EO_VERIFYsizeof(eOmc_joint_t, 128);



//typedef struct                  // size is 68+8+36+12+1+1+1+1 = 128
//{
//    eOmc_joint_config_t         jconfig;                     /**< the configuration of the joint */
//    eOmc_motor_config_t         mconfig;                     /**< the configuration of the joint */
//    eOmc_calibration_t          calibration;                // ok, da definire meglio ....
//    eOenum08_t                  controlmode;            // use a eOmc_controlmode_t value.
//    eOmc_1motorjoint_status_debug_t   onemotorjointstatusdebug;           /**< the configuration of the joint. If a normal joint status is enough, use jointstatusdebug.jointstatus */
//    eOmc_setpoint_t             setpoint;                   /**< the setpoint of the joint */
//    eOenum08_t                  controlmode;                /**< use values from eOmc_controlmode_t */
//    eOenum08_t                  controlstatus;              /**< use values from eOmc_controlstatus_t */
//    eObool_t                    motorenabled;               /**< if eobool_true the motor is enabled via PWM or else */   
//    uint8_t                     filler01[1];
//} eOmc_united_joint_t;          EO_VERIFYsizeof(eOmc_joint_t, 128);



typedef struct                  // size is 64+16+8+12+1+3+0 = 104
{
    eOmc_joint_config_t         jconfig;                    /**< the configuration of the joint */
    eOmc_joint_status_t         jstatus;                    /**< the status of the joint */
    eOmc_calibrator_t           calibrator;                 /**< the calibrator to use */
    eOmc_setpoint_t             setpoint;                   /**< the setpoint of the joint */
    eOenum08_t                  controlmode;                /**< use values from eOmc_controlmode_t, but maybe its enum type will change*/
    uint8_t                     filler03[3];                // space for some other variables ...
} eOmc_joint_t;                 EO_VERIFYsizeof(eOmc_joint_t, 104);
#warning -> usare due variabili booleane per CAN_MOTION_DONE
//eObool_t    signalwhendesiredpositionisreached;     // beh, oppure output. magari si mette anche il numero di milli di validita'. 
//eObool_t    desiredpositionisreached;               // input. magari si segnala anche la position.


typedef struct                  // size is 24+16+0 = 40
{
    eOmc_motor_config_t         mconfig;                    /**< the configuration of the motor */
    eOmc_motor_status_t         mstatus;                    /**< the status of the motor */   
} eOmc_motor_t;                 EO_VERIFYsizeof(eOmc_motor_t, 40); 
 

 
// some considerations by acemor.
// each joint-motor needs 144 bytes.  
// the left leg uses ... 144*6 joint-motor = 864 bytes
// the left arm uses ... 7 joint-motor + 9 = 16 --> 2304 bytes. .... however: if we split we had: 4*144=576 on the first and 12*144=1728 on the second
// we need to signal eOmc_joint_status_t and eOmc_motor_status_t for each jm. thery are 32 bytes. thus:
// the ems1 on leg sends 32*4 = 128 bytes
// the ems2 on leg sends 32*2 = 64 bytes.
// the ems1 on arm sends 32*4 = 128 bytes
// the ems2 on arm sends 32*12 = 384 bytes 


typedef struct                  // size is 104*6+40*6+0 = 864
{
    eOmc_joint_t                j01_hip_flexext;
    eOmc_joint_t                j02_hip_abdadd;
    eOmc_joint_t                j03_tight_rot;
    eOmc_joint_t                j04_knee;
    eOmc_joint_t                j05_ankle_flexext;
    eOmc_joint_t                j06_ankle_abdadd;  
 
    eOmc_motor_t                m01_hip_flexext;
    eOmc_motor_t                m02_hip_abdadd;
    eOmc_motor_t                m03_tight_rot;
    eOmc_motor_t                m04_knee;
    eOmc_motor_t                m05_ankle_flexext;
    eOmc_motor_t                m06_ankle_abdadd;  
} eOmc_EP_leftleg_names_t;      EO_VERIFYsizeof(eOmc_EP_leftleg_names_t, 864); 

#define EOMC_EP_index_leftleg_j01_hip_flexext           1
#define EOMC_EP_index_leftleg_j02_hip_abdadd            2
#define EOMC_EP_index_leftleg_j03_tight_rot             3
//etc..

#define EOMC_EP_index_leftleg_m01_hip_flexext           1
#define EOMC_EP_index_leftleg_m02_hip_abdadd            2
#define EOMC_EP_index_leftleg_m03_tight_rot             3
// etc.

typedef struct                  // size is 104*6+40*6+0 = 864
{
    eOmc_joint_t                joints[6]; 
    eOmc_motor_t                motors[6];
} eOmc_EP_leftleg_indices_t;    EO_VERIFYsizeof(eOmc_EP_leftleg_indices_t, 864);



/** @typedef    typedef uint8_t  eOmc_jointUniqueId_t
    @brief      eOmc_jointUniqueId_t contains the values required to identify a joint 
                of robot univocally on a body part
 **/
typedef uint8_t  eOmc_jointUniqueId_t;


/** @typedef    typedef uint8_t  eOmc_jointBoardId_t
    @brief      eOmc_jointBoardId_t contains the values required to identify a joint on a board.
 **/
typedef uint8_t  eOmc_jointBoardId_t;


/** @typedef    typedef uint8_t  eOmc_motorUniqueId_t
    @brief      eOmc_motorUniqueId_t contains the values required to identify a motor 
                of robot univocally on a body part
 **/
typedef uint8_t  eOmc_motorUniqueId_t;


/** @typedef    typedef uint8_t  eOmc_motorBoardId_t
    @brief      eOmc_motorBoardId_t contains the values required to identify a motor on a board.
 **/
typedef uint8_t  eOmc_motorBoardId_t;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_devervrffdvf  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




