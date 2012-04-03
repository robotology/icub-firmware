
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
    @warning    This struct must be of fixed and size and multiple of 4.
    @todo       describe how the 16bit values are defined and how to map to/from floating point
 **/
typedef struct              // size is 12 bytes
{
    int16_t                 kp;             /**< proportional gain */
    int16_t                 ki;             /**< integral gain */
    int16_t                 kd;             /**< derivative gain */
    int16_t                 limitonintegral;  /**< limit of integral term */ 
    int16_t                 limitonoutput;  /**< limit of the output of the pid */
    int8_t                  scale;          /**< scale factor for the gains (and what about the rest?)*/
    int8_t                  offset;         /**< the k0 in the pid formula */
//    uint8_t                 filler02[2];    /**< filler to make struct a multiple of 4 */
} eOmc_PIDq15_t;            EO_VERIFYsizeof(eOmc_PIDq15_t, 12);

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


/** @typedef    typedef struct eOmc_setpoint_t
    @brief      eOmc_setpoint_t contains the setpoint to be sent to a pid
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct
{
    uint8_t                 controlmode;
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
    }to;
} eOmc_setpoint_t;              EO_VERIFYsizeof(eOmc_setpoint_t, 12);


/** @typedef    typedef struct eOmc_joint_config_t
    @brief      eOmc_joint_config_t contains the values required to configure a joint with one controlling motor
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 12+12+12+12+4+4+2+2+2+2+1+3 = 68
{
    eOmc_PID_t                  pidpos;
    eOmc_PID_t                  pidvel;
    eOmc_PID_t                  pidtor;
    eOmc_PID_t                  pidcur;
    eOmeas_position_t           minpositionofjoint;     // of joint
    eOmeas_position_t           maxpositionofjoint;     // of joint
    eOmeas_current_t            maxcurrentofmotor;      // of motor
    eOmeas_velocity_t           maxvelocityofmotor;     // of motor
    uint16_t                    velocityshiftfactor;    // of joint !!!!!!!!
    uint16_t                    velocitysetpointtimeout;        // max time between two velocity setpoints before control switches to position mode
    eOenum08_t                  controlmode;            // use a eOmc_controlmode_t value. 
    uint8_t                     filler03[3]; // oppure filler00 se tolgo controlmode
} eOmc_joint_config_t;          EO_VERIFYsizeof(eOmc_joint_config_t, 68);


/** @typedef    typedef struct eOmc_axis_status_t
    @brief      eOmc_axis_status_t contains the status of the axis of a joint
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 4+2+2+2+2+0 = 12
{
    eOmeas_position_t           position;           
    eOmeas_velocity_t           velocity;           
    eOmeas_acceleration_t       acceleration;       
    eOmeas_torque_t             torque;  
    uint8_t                     filler02[2];    
} eOmc_axis_status_t;           EO_VERIFYsizeof(eOmc_axis_status_t, 12);


/** @typedef    typedef struct eOmc_pid_status_t
    @brief      eOmc_pid_status_t contains the status of a generic pid
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 4+2+2+0 = 8
{
    int32_t                     reference;       // in che unita' di misura? eOmeas_position_t ???
    int16_t                     output;          // perche' reference and output non sono nella stessa unita' di misura?
    int16_t                     error;           // QUALE PID? quello di position piu' esterno?        
} eOmc_pid_status_t;            EO_VERIFYsizeof(eOmc_pid_status_t, 8);


/** @typedef    typedef struct eOmc_motor_status_t
    @brief      eOmc_motor_status_t contains the status of a motor
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 4+2+2+0 = 8
{
    eOmeas_position_t           position;          
    eOmeas_velocity_t           velocity; 
    eOmeas_current_t            current; 
} eOmc_motor_status_t;          EO_VERIFYsizeof(eOmc_motor_status_t, 8);


/** @typedef    typedef struct eOmc_joint_status_t
    @brief      eOmc_joint_status_t contains the status of a joint with a position control loop
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 12+8+8+0 = 28
{
    eOmc_axis_status_t          axis;
    eOmc_motor_status_t         motor;
    eOmc_pid_status_t           pidpos;
} eOmc_joint_status_t;          EO_VERIFYsizeof(eOmc_joint_status_t, 28);


/** @typedef    typedef struct eOmc_joint_status_t
    @brief      eOmc_joint_status_t contains the status of a joint with a position control loop with the addition of a debug 
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 28+8+0 = 36
{
    eOmc_joint_status_t         jointstatus;
    eOutil_debug_values_t       debugvalues;
} eOmc_joint_status_debug_t;    EO_VERIFYsizeof(eOmc_joint_status_debug_t, 36);

//#warning --> calibration types and modes are still to be defined

typedef enum
{
    eomc_calibration_type0_hard_stops               = 0,
    eomc_calibration_type1_abs_sens_analog          = 1,
    eomc_calibration_type2_hard_stops_diff          = 2,
    eomc_calibration_type3_abs_sens_digital         = 3,
    eomc_calibration_type4_abs_and_incremental      = 4
} eOmc_calibration_types_t;


typedef struct  
{
    uint16_t                    pwmlimit;
    uint16_t                    velocity;
} eOmc_calibration_params_type0_hard_stops_t;

typedef struct  
{
    uint16_t                    position;
    uint16_t                    velocity;
} eOmc_calibration_params_type1_abs_sens_analog_t;

typedef struct  
{
    uint16_t                    pwmlimit;
    uint16_t                    velocity;
} eOmc_calibration_params_type2_hard_stops_diff_t;

typedef struct  
{
    uint16_t                    position;
    uint16_t                    velocity;
    uint16_t                    offset;
} eOmc_calibration_params_type3_abs_sens_digital_t;

typedef struct  
{
    uint16_t                    position;
    uint16_t                    velocity;
    uint16_t                    maxencoder;
} eOmc_calibration_params_type4_abs_and_incremental_t;


typedef struct
{
    uint8_t                     filler;
    eOenum08_t                  type;
    union
    {
        uint16_t                                            any[3];
        eOmc_calibration_params_type0_hard_stops_t          type0;
        eOmc_calibration_params_type1_abs_sens_analog_t     type1;
        eOmc_calibration_params_type2_hard_stops_diff_t     type2;
        eOmc_calibration_params_type3_abs_sens_digital_t    type3;
        eOmc_calibration_params_type4_abs_and_incremental_t type4;
    } params;   
} eOmc_calibration_t;           EO_VERIFYsizeof(eOmc_calibration_t, 8);



typedef struct  
{
    int16_t                     stiffness;      /**< the Ks parameter */
    int16_t                     dumping;        /**< the Kd parameter */
    int16_t                     offset;         /**< the Ko parameter */
} eOmc_impedance_t;             EO_VERIFYsizeof(eOmc_impedance_t, 6);



// manca la calibrazione ed altro ...

// usare due variabili booleane per CAN_MOTION_DONE
//eObool_t    signalwhendesiredpositionisreached;     // beh, oppure output. magari si mette anche il numero di milli di validita'. 
//eObool_t    desiredpositionisreached;               // input. magari si segnala anche la position.

typedef struct                  // size is 68+8+36+12+1+1+1+1 = 128
{
    eOmc_joint_config_t         config;                     /**< the configuration of the joint */
    eOmc_calibration_t          calibration;                // ok, da definire meglio ....
    eOmc_joint_status_debug_t   jointstatusdebug;           /**< the configuration of the joint. If a normal joint status is enough, use jointstatusdebug.jointstatus */
    eOmc_setpoint_t             setpoint;                   /**< the setpoint of the joint */
    eOenum08_t                  controlmode;                /**< use values from eOmc_controlmode_t */
    eOenum08_t                  controlstatus;              /**< use values from eOmc_controlstatus_t */
    eObool_t                    motorenabled;               /**< if eobool_true the motor is enabled via PWM or else */
    
    uint8_t                     filler01[1];
} eOmc_joint_t;                 EO_VERIFYsizeof(eOmc_joint_t, 128);
 

// #error separare il joint dal motor. il joint possiede la config



/** @typedef    typedef uint8_t  eOmc_jointUniqueId_t
    @brief      eOmc_jointUniqueId_t contains the values required to identify a joint of robot univocally
 **/
typedef uint8_t  eOmc_jointUniqueId_t;

//VALE
/** @typedef    typedef uint8_t  eOmc_jointBoardId_t
    @brief      eOmc_jointBoardId_t contains the values required to identify a joint on a board.
 **/
typedef uint8_t  eOmc_jointBoardId_t;

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_devervrffdvf  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




