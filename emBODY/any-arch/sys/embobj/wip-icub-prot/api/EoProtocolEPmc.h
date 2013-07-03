/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOPROTOCOLEPMC_H_
#define _EOPROTOCOLEPMC_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolEPmc.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolEPmc Protocol for motion control endpoint 
    Ivreververv e
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocolCommon.h"
#include "EoProtocolEP.h"
#include "EoMotionControl.h"
#include "EOnv.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef eOmc_entity_t eOprotEntityEPmc_t
    @brief      It contains the entities in endpoint motion control. They are eomc_entity_joint, eomc_entity_motor, 
                and eomc_entity_controller.
 **/
typedef eOmc_entity_t eOprotEntityEPmc_t;

enum { eoprot_ep_mc_entities_numberof = eomc_entities_numberof };


// - definition of the joint

/** @typedef    typedef enum eOprot_ep_mc_tag_joint_t
    @brief      It contains the tags for all variables in a joint.
                See definition of eOmc_joint_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_tag_joint_wholeitem                                    =  0,
    eoprot_ep_mc_tag_joint_config                                       =  1,
    eoprot_ep_mc_tag_joint_config_pidposition                           =  2,
    eoprot_ep_mc_tag_joint_config_pidvelocity                           =  3,
    eoprot_ep_mc_tag_joint_config_pidtorque                             =  4,
    eoprot_ep_mc_tag_joint_config_impedance                             =  5,
    eoprot_ep_mc_tag_joint_config_minpositionofjoint                    =  6,
    eoprot_ep_mc_tag_joint_config_maxpositionofjoint                    =  7,
    eoprot_ep_mc_tag_joint_config_velocitysetpointtimeout               =  8,
    eoprot_ep_mc_tag_joint_config_motionmonitormode                     =  9,
    eoprot_ep_mc_tag_joint_config_encoderconversionfactor               = 10,
    eoprot_ep_mc_tag_joint_config_encoderconversionoffset               = 11,    
    eoprot_ep_mc_tag_joint_status                                       = 12,
    eoprot_ep_mc_tag_joint_status_basic                                 = 13,
    eoprot_ep_mc_tag_joint_status_ofpid                                 = 14,
    eoprot_ep_mc_tag_joint_status_chamaleon04                           = 15,    
    eoprot_ep_mc_tag_joint_inputs                                       = 16,
    eoprot_ep_mc_tag_joint_inputs_externallymeasuredtorque              = 17,       
    eoprot_ep_mc_tag_joint_cmmnds_calibration                           = 18,
    eoprot_ep_mc_tag_joint_cmmnds_setpoint                              = 19,
    eoprot_ep_mc_tag_joint_cmmnds_stoptrajectory                        = 20,
    eoprot_ep_mc_tag_joint_cmmnds_controlmode                           = 21
} eOprot_ep_mc_tag_joint_t;

enum { eoprot_ep_mc_tags_joint_numberof = 22 };  // it MUST be equal to the number of tags in a joint.      


/** @typedef    typedef enum eOprot_ep_mc_rwmode_joint_t
    @brief      It contains the rw mode for all the network variables in a joint. There must be a one-to-one
                correspondence to the values in eOprot_ep_mc_tag_joint_t.
                See definition of eOmc_joint_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_rwmode_joint_wholeitem                                 = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_joint_config                                    = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_pidposition                        = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_pidvelocity                        = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_pidtorque                          = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_impedance                          = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_minpositionofjoint                 = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_maxpositionofjoint                 = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_velocitysetpointtimeout            = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_motionmonitormode                  = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_encoderconversionfactor            = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_config_encoderconversionoffset            = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_status                                    = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_joint_status_basic                              = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_joint_status_ofpid                              = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_joint_status_chamaleon04                        = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_joint_inputs                                    = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_inputs_externallymeasuredtorque           = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_cmmnds_calibration                        = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_cmmnds_setpoint                           = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_cmmnds_stoptrajectory                     = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_joint_cmmnds_controlmode                        = eo_nv_rwmode_RW
} eOprot_ep_mc_rwmode_joint_t;    

enum { eoprot_ep_mc_rwmodes_joint_numberof = 22 };  // it MUST be equal to the number of tags in a joint.  


// - definition of the motor


/** @typedef    typedef enum eOprot_ep_mc_tag_motor_t
    @brief      It contains the tags for all variables in a motor.
                See definition of eOmc_joint_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_tag_motor_wholeitem                                    = 0,
    eoprot_ep_mc_tag_motor_config                                       = 1,
    eoprot_ep_mc_tag_motor_config_pidcurrent                            = 2,
    eoprot_ep_mc_tag_motor_config_maxvelocityofmotor                    = 3,
    eoprot_ep_mc_tag_motor_config_maxcurrentofmotor                     = 4,    
    eoprot_ep_mc_tag_motor_status                                       = 5,
    eoprot_ep_mc_tag_motor_status_basic                                 = 6,
    eoprot_ep_mc_tag_motor_status_chamaleon04                           = 7
} eOprot_ep_mc_tag_motor_t;

enum { eoprot_ep_mc_tags_motor_numberof = 8 };   // it MUST be equal to the number of tags in a motor.   


/** @typedef    typedef enum eOprot_ep_mc_rwmode_motor_t
    @brief      It contains the rw mode for all the network variables in a motor. There must be a one-to-one
                correspondence to the values in eOprot_ep_mc_tag_motor_t.
                See definition of eOmc_motor_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_rwmode_motor_wholeitem                                 = eo_nv_rwmode_RO,    
    eoprot_ep_mc_rwmode_motor_config                                    = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_motor_config_pidcurrent                         = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_motor_config_maxvelocityofmotor                 = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_motor_config_maxcurrentofmotor                  = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_motor_status                                    = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_motor_status_basic                              = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_motor_status_chamaleon04                        = eo_nv_rwmode_RO 
} eOprot_ep_mc_rwmode_motor_t;  

enum { eoprot_ep_mc_rwmodes_motor_numberof = 8 };   // it MUST be equal to the number of tags in a motor. 



// - definition of the controller



/** @typedef    typedef enum eOprot_ep_mc_tag_controller_t
    @brief      It contains the tags for all variables in a motor.
                See definition of eOmc_controller_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
     eoprot_ep_mc_tag_controller_wholeitem                              = 0,
     eoprot_ep_mc_tag_controller_config                                 = 1,
     eoprot_ep_mc_tag_controller_config_durationofctrlloop              = 2,
     eoprot_ep_mc_tag_controller_status                                 = 3,
     eoprot_ep_mc_tag_controller_status_alljomoinitted                  = 4,
     eoprot_ep_mc_tag_controller_cmmnds_go2stateofcontroller            = 5
} eOprot_ep_mc_tag_controller_t;

enum { eoprot_ep_mc_tags_controller_numberof = 6 };  // it MUST be equal to the number of tags in a controller. 


/** @typedef    typedef enum eOprot_ep_mc_rwmode_controller_t
    @brief      It contains the function and type for all the network variables in a controller. There must be a one-to-one
                correspondence to the values in eOprot_ep_mc_tag_controller_t.
                See definition of eOmc_controller_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_rwmode_controller_wholeitem                            = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_controller_config                               = eo_nv_rwmode_RW,
    eoprot_ep_mc_rwmode_controller_config_durationofctrlloop            = eo_nv_rwmode_RW,

    eoprot_ep_mc_rwmode_controller_status                               = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_controller_status_alljomoinitted                = eo_nv_rwmode_RO,
    eoprot_ep_mc_rwmode_controller_cmmnds_go2stateofcontroller          = eo_nv_rwmode_RW
} eOprot_ep_mc_rwmode_controller_t; 

enum { eoprot_ep_mc_rwmodes_controller_numberof = 6 };  // it MUST be equal to the number of tags in a controller. 


// - structures implementing the endpoints


/** @typedef    typedef struct eOprot_ep_mc_template_t;
    @brief      is a template for the organisation of joints, motors, controllers in the motion control endpoint.
 **/
typedef struct                  // 152*1+40*1+24 = 216              
{
    eOmc_joint_t                joints[1]; 
    eOmc_motor_t                motors[1];
    eOmc_controller_t           thecontroller;
} eOprot_ep_mc_template_t;      EO_VERIFYsizeof(eOprot_ep_mc_template_t, 216);

#if 0
/** @typedef    typedef struct eOprot_ep_mc_variables_t;
    @brief      it contains all variables in the motion control endpoint.
 **/
typedef struct                  // 4+4+4+4 = 16              
{
    eOmc_joint_t*               joints;             // pointer to all joints organised in an array
    eOmc_motor_t*               motors;             // pointer to all motors organised in an array
    eOmc_controller_t*          controllers;        // pointer to all controllers (even if it is typically only one) organised in an array
    uint8_t                     pool[4];            // the memory pool for the entities. 
} eOprot_ep_mc_variables_t;     EO_VERIFYsizeof(eOprot_ep_mc_variables_t, 16);
#endif

#if 0
/** @typedef    typedef struct eOprot_ep_mc_variables1_t;
    @brief      it contains all variables in the motion control endpoint.
 **/
typedef struct                  // 3*4+4 = 16              
{
    void*                       entities[eoprot_ep_mc_entities_numberof];   // pointer to the arrays of the entities (joint, motor, controller)
    uint8_t                     pool[4];            // the memory pool for the entities. 
} eOprot_ep_mc_variables1_t;    EO_VERIFYsizeof(eOprot_ep_mc_variables1_t, 16);
#endif

  
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - these three are used to configure the endpoint into a EOnvsCfg object
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

#if 0   // dynamic mode
extern eOresult_t eoprot_ep_mc_number_of_boards_Load(uint16_t numofboards);
#endif


/** @fn         extern eOresult_t eoprot_ep_mc_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity)
    @brief      This function loads the maximum number of entities managed by the endpoint ...
    @param      brd                     the board
    @param      numberofeachentity      array of the values.
    @return     the number of variables.
  */
extern eOresult_t eoprot_ep_mc_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity);


/** @fn         extern eObool_t eoprot_ep_mc_variables_id_isvalid(eOprotBRD_t brd, eOnvID_t id)
    @brief      This function tells if an id belongs to the board
    @param      brd                     the board
    @param      id                      the id
    @return     eobool_true if the ID belongs to the board, eobool_false if not.
  */
extern eObool_t eoprot_ep_mc_variables_id_isvalid(eOprotBRD_t brd, eOnvID_t id);


/** @fn         extern uint16_t eoprot_ep_mc_variable_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of variables given the board
    @param      ep              the endpoint
    @return     the number of variables.
  */
extern uint16_t eoprot_ep_mc_variables_numberof_Get(eOprotBRD_t brd);


/** @fn         extern eOprotID_t eoprot_ep_mc_variable_idfromprognumber_Get(eOprotBRD_t brd, uint16_t prog)
    @brief      This function retrieves the id of a variable from the progressive number given the board
    @param      brd                     the board
    @param      prog                    the progressive number
    @return     the ID or EOK_uint16dummy if invalid .
  */
extern eOprotID_t eoprot_ep_mc_variable_idfromprognumber_Get(eOprotBRD_t brd, uint16_t prog);


/** @fn         extern uint16_t eoprot_ep_mc_variable_progressivenumber_Get(eOprotBRD_t brd)
    @brief      This function retrieves the progressive number of a variable given the board and the @e id
    @param      ep              the endpoint
    @return     the progressive number or EOK_uint16dummy if invalid .
  */
extern uint16_t eoprot_ep_mc_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID_t id);


/** @fn         extern uint16_t eoprot_ep_mc_joints_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of joints given the board
    @param      ep              the endpoint
    @return     the number of joints.
  */
extern uint16_t eoprot_ep_mc_joints_numberof_Get(eOprotBRD_t brd);


/** @fn         extern uint16_t eoprot_ep_mc_motors_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of motors given the board
    @param      ep              the endpoint
    @return     the number of motors.
  */
extern uint16_t eoprot_ep_mc_motors_numberof_Get(eOprotBRD_t brd);


/** @fn         extern uint16_t eoprot_ep_mc_controllers_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of controllers given the board
    @param      ep              the endpoint
    @return     the number of controllers.
  */
extern uint16_t eoprot_ep_mc_controllers_numberof_Get(eOprotBRD_t brd);


extern uint16_t eoprot_ep_mc_ram_sizeof_Get(eOprotBRD_t brd);

extern uint16_t eoprot_ep_mc_variable_ram_sizeof_Get(eOprotID_t id);

extern void* eoprot_ep_mc_variable_ram_Extract(void* epram, eOprotBRD_t brd, eOprotID_t id);

extern void* eoprot_ep_mc_variable_rom_Get(eOprotID_t id);

extern const eOmc_joint_t* eoprot_ep_mc_joint_default_Get(void);

extern const eOmc_motor_t* eoprot_ep_mc_motor_default_Get(void);

extern const eOmc_controller_t* eoprot_ep_mc_controller_default_Get(void);


/** @}            
    end of group eo_EoProtocolEPmc  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


