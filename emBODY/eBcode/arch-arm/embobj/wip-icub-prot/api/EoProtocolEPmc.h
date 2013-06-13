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

/** @defgroup eo_protocol_ep_mc Protocol for motion control endpoint 
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


/** @typedef    typedef enum eOprot_endpoint_motioncontrol_t;
    @brief      It contains all the endpoints used for motion control. 
 **/
typedef enum
{
    eoprot_ep_mc_leftupperarm       = eoprot_endpoint_mc_leftupperarm,   
    eoprot_ep_mc_leftlowerarm       = eoprot_endpoint_mc_leftlowerarm,
    
    eoprot_ep_mc_rightupperarm      = eoprot_endpoint_mc_rightupperarm,   
    eoprot_ep_mc_rightlowerarm      = eoprot_endpoint_mc_rightlowerarm,   

    eoprot_ep_mc_torso              = eoprot_endpoint_mc_torso,  

    eoprot_ep_mc_leftupperleg       = eoprot_endpoint_mc_leftupperleg,   
    eoprot_ep_mc_leftlowerleg       = eoprot_endpoint_mc_leftlowerleg,   
    
    eoprot_ep_mc_rightupperleg      = eoprot_endpoint_mc_rightupperleg,   
    eoprot_ep_mc_rightlowerleg      = eoprot_endpoint_mc_rightlowerleg   
    
} eOprot_endpoint_motioncontrol_t;


enum { eoprot_endpoints_motioncontrol_numberof = 9 };


/** @typedef    typedef eOmc_entity_t eOprotEntityEPmc_t
    @brief      It contains the entities in endpoint motion control. They are eomc_entity_joint, eomc_entity_motor, 
                and eomc_entity_controller.
 **/
typedef eOmc_entity_t eOprotEntityEPmc_t;

enum { eoprot_ep_mc_entities_numberof = eomc_entities_numberof };

// - definition of the joint: number of them in each endpoint, tag and funtyp values

enum
{
    eoprot_ep_mc_joints_numberof_upperarm   = 4,
    eoprot_ep_mc_joints_numberof_lowerarm   = 12,
    eoprot_ep_mc_joints_numberof_thetorso   = 3,
    eoprot_ep_mc_joints_numberof_upperleg   = 4,
    eoprot_ep_mc_joints_numberof_lowerleg   = 2
}; 

typedef enum
{   
    eoprot_ep_mc_joints_numberof_leftupperarm   = eoprot_ep_mc_joints_numberof_upperarm,
    eoprot_ep_mc_joints_numberof_leftlowerarm   = eoprot_ep_mc_joints_numberof_lowerarm,
    
    eoprot_ep_mc_joints_numberof_rightupperarm  = eoprot_ep_mc_joints_numberof_upperarm,   
    eoprot_ep_mc_joints_numberof_rightlowerarm  = eoprot_ep_mc_joints_numberof_lowerarm,   

    eoprot_ep_mc_joints_numberof_torso          = eoprot_ep_mc_joints_numberof_thetorso,   

    eoprot_ep_mc_joints_numberof_leftupperleg   = eoprot_ep_mc_joints_numberof_upperleg,  
    eoprot_ep_mc_joints_numberof_leftlowerleg   = eoprot_ep_mc_joints_numberof_lowerleg,   
    
    eoprot_ep_mc_joints_numberof_rightupperleg  = eoprot_ep_mc_joints_numberof_upperleg,   
    eoprot_ep_mc_joints_numberof_rightlowerleg  = eoprot_ep_mc_joints_numberof_lowerleg   
} eOprot_ep_mc_joints_numberof_t;




/** @typedef    typedef enum eOprot_ep_mc_joint_tag_t
    @brief      It contains the tags for all variables in a joint.
                See definition of eOmc_joint_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_joint_tag_jconfig                                      =  0,
    eoprot_ep_mc_joint_tag_jconfig__pidposition                         =  1,
    eoprot_ep_mc_joint_tag_jconfig__pidvelocity                         =  2,
    eoprot_ep_mc_joint_tag_jconfig__pidtorque                           =  3,
    eoprot_ep_mc_joint_tag_jconfig__impedance                           =  4,
    eoprot_ep_mc_joint_tag_jconfig__minpositionofjoint                  =  5,
    eoprot_ep_mc_joint_tag_jconfig__maxpositionofjoint                  =  6,
    eoprot_ep_mc_joint_tag_jconfig__velocitysetpointtimeout             =  7,
    eoprot_ep_mc_joint_tag_jconfig__motionmonitormode                   =  8,
    eoprot_ep_mc_joint_tag_jconfig__encoderconversionfactor             =  9,
    eoprot_ep_mc_joint_tag_jconfig__encoderconversionoffset             = 10,
    
    eoprot_ep_mc_joint_tag_jstatus                                      = 11,
    eoprot_ep_mc_joint_tag_jstatus__basic                               = 12,
    eoprot_ep_mc_joint_tag_jstatus__ofpid                               = 13,
    eoprot_ep_mc_joint_tag_jstatus__chamaleon04                         = 14,
    
    eoprot_ep_mc_joint_tag_jinputs                                      = 15,
    eoprot_ep_mc_joint_tag_jinputs__externallymeasuredtorque            = 16,    
    
    eoprot_ep_mc_joint_tag_jcmmnds__calibration                         = 17,
    eoprot_ep_mc_joint_tag_jcmmnds__setpoint                            = 18,
    eoprot_ep_mc_joint_tag_jcmmnds__stoptrajectory                      = 19,
    eoprot_ep_mc_joint_tag_jcmmnds__controlmode                         = 20
} eOprot_ep_mc_joint_tag_t;

enum { eoprot_ep_mc_joint_tags_numberof = 21 };  // it MUST be equal to the number of tags in a joint.      


/** @typedef    typedef enum eOprot_ep_mc_joint_funtyp_t
    @brief      It contains the function and type for all the network variables in a joint. There must be a one-to-one
                correspondence to the values in eOprot_ep_mc_joint_tag_t.
                See definition of eOmc_joint_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_joint_funtyp_jconfig                                   = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jconfig__pidposition                      = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jconfig__pidvelocity                      = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jconfig__pidtorque                        = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jconfig__impedance                        = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jconfig__minpositionofjoint               = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32),
    eoprot_ep_mc_joint_funtyp_jconfig__maxpositionofjoint               = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32),
    eoprot_ep_mc_joint_funtyp_jconfig__velocitysetpointtimeout          = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16),
    eoprot_ep_mc_joint_funtyp_jconfig__motionmonitormode                = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08),
    eoprot_ep_mc_joint_funtyp_jconfig__encoderconversionfactor          = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32),
    eoprot_ep_mc_joint_funtyp_jconfig__encoderconversionoffset          = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32),

    eoprot_ep_mc_joint_funtyp_jstatus                                   = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jstatus__basic                            = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jstatus__ofpid                            = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jstatus__chamaleon04                      = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_u32),

    eoprot_ep_mc_joint_funtyp_jinputs                                   = EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jinputs__externallymeasuredtorque         = EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_u16),

    eoprot_ep_mc_joint_funtyp_jcmmnds__calibration                      = EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jcmmnds__setpoint                         = EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_pkd),
    eoprot_ep_mc_joint_funtyp_jcmmnds__stoptrajectory                   = EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_u08),
    eoprot_ep_mc_joint_funtyp_jcmmnds__controlmode                      = EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_u08)
} eOprot_ep_mc_joint_funtyp_t;    

enum { eoprot_ep_mc_joint_funtyps_numberof = 21 };  // it MUST be equal to the number of tags in a joint.  


// - definition of the motor: number of them in each endpoint, tag and funtyp values

enum
{
    eoprot_ep_mc_motors_numberof_upperarm   = 4,
    eoprot_ep_mc_motors_numberof_lowerarm   = 12,
    eoprot_ep_mc_motors_numberof_thetorso   = 3,
    eoprot_ep_mc_motors_numberof_upperleg   = 4,
    eoprot_ep_mc_motors_numberof_lowerleg   = 2
}; 

typedef enum
{   
    eoprot_ep_mc_motors_numberof_leftupperarm   = eoprot_ep_mc_motors_numberof_upperarm,
    eoprot_ep_mc_motors_numberof_leftlowerarm   = eoprot_ep_mc_motors_numberof_lowerarm,
    eoprot_ep_mc_motors_numberof_rightupperarm  = eoprot_ep_mc_motors_numberof_upperarm,   
    eoprot_ep_mc_motors_numberof_rightlowerarm  = eoprot_ep_mc_motors_numberof_lowerarm,   

    eoprot_ep_mc_motors_numberof_torso          = eoprot_ep_mc_motors_numberof_thetorso,   

    eoprot_ep_mc_motors_numberof_leftupperleg   = eoprot_ep_mc_motors_numberof_upperleg,  
    eoprot_ep_mc_motors_numberof_leftlowerleg   = eoprot_ep_mc_motors_numberof_lowerleg,   
    
    eoprot_ep_mc_motors_numberof_rightupperleg  = eoprot_ep_mc_motors_numberof_upperleg,   
    eoprot_ep_mc_motors_numberof_rightlowerleg  = eoprot_ep_mc_motors_numberof_lowerleg   
} eOprot_ep_mc_motors_numberof_t;




/** @typedef    typedef enum eOprot_ep_mc_motor_tag_t
    @brief      It contains the tags for all variables in a motor.
                See definition of eOmc_joint_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_motor_tag_mconfig                                    = 0,
    eoprot_ep_mc_motor_tag_mconfig__pidcurrent                        = 1,
    eoprot_ep_mc_motor_tag_mconfig__maxvelocityofmotor                = 2,
    eoprot_ep_mc_motor_tag_mconfig__maxcurrentofmotor                 = 3,
    
    eoprot_ep_mc_motor_tag_mstatus                                    = 4,
    eoprot_ep_mc_motor_tag_mstatus__basic                             = 5,
    eoprot_ep_mc_motor_tag_mstatus__chamaleon04                       = 6
} eOprot_ep_mc_motor_tag_t;

enum { eoprot_ep_mc_motor_tags_numberof = 7 };   // it MUST be equal to the number of tags in a motor.   


/** @typedef    typedef enum eOprot_ep_mc_motor_funtyp_t
    @brief      It contains the function and type for all the network variables in a motor. There must be a one-to-one
                correspondence to the values in eOprot_ep_mc_motor_tag_t.
                See definition of eOmc_motor_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_motor_funtyp_mconfig                                   = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_mc_motor_funtyp_mconfig__pidcurrent                       = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_mc_motor_funtyp_mconfig__maxvelocityofmotor               = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32),
    eoprot_ep_mc_motor_funtyp_mconfig__maxcurrentofmotor                = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u16),

    eoprot_ep_mc_motor_funtyp_mstatus                                   = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd),
    eoprot_ep_mc_motor_funtyp_mstatus__basic                            = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd),
    eoprot_ep_mc_motor_funtyp_mstatus__chamaleon04                      = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_u32) 
} eOprot_ep_mc_motor_funtyp_t;  

enum { eoprot_ep_mc_motor_funtyps_numberof = 7 };   // it MUST be equal to the number of tags in a motor. 

// - definition of the controller: number of them in each endpoint, tag and funtyp values

enum
{
    eoprot_ep_mc_controllers_numberof       = 1
}; 

typedef enum
{   
    eoprot_ep_mc_controllers_numberof_leftupperarm   = eoprot_ep_mc_controllers_numberof,
    eoprot_ep_mc_controllers_numberof_leftlowerarm   = eoprot_ep_mc_controllers_numberof,
    eoprot_ep_mc_controllers_numberof_rightupperarm  = eoprot_ep_mc_controllers_numberof,   
    eoprot_ep_mc_controllers_numberof_rightlowerarm  = eoprot_ep_mc_controllers_numberof,   

    eoprot_ep_mc_controllers_numberof_torso          = eoprot_ep_mc_controllers_numberof,   

    eoprot_ep_mc_controllers_numberof_leftupperleg   = eoprot_ep_mc_controllers_numberof,  
    eoprot_ep_mc_controllers_numberof_leftlowerleg   = eoprot_ep_mc_controllers_numberof,   
    
    eoprot_ep_mc_controllers_numberof_rightupperleg  = eoprot_ep_mc_controllers_numberof,   
    eoprot_ep_mc_controllers_numberof_rightlowerleg  = eoprot_ep_mc_controllers_numberof   
} eOprot_ep_mc_controllers_numberof_t;

  


/** @typedef    typedef enum eOprot_ep_mc_controller_tag_t
    @brief      It contains the tags for all variables in a motor.
                See definition of eOmc_controller_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
     eoprot_ep_mc_controller_tag_cconfig                               = 0,
     eoprot_ep_mc_controller_tag_cconfig__durationofctrlloop           = 1,
     eoprot_ep_mc_controller_tag_cstatus                               = 2,
     eoprot_ep_mc_controller_tag_cstatus__alljomoinitted               = 3,
     eoprot_ep_mc_controller_tag_ccmmnds__go2stateofcontroller         = 4
} eOprot_ep_mc_controller_tag_t;

enum { eoprot_ep_mc_controller_tags_numberof = 5 };  // it MUST be equal to the number of tags in a controller. 

/** @typedef    typedef enum eOprot_ep_mc_controller_funtyp_t
    @brief      It contains the function and type for all the network variables in a controller. There must be a one-to-one
                correspondence to the values in eOprot_ep_mc_controller_tag_t.
                See definition of eOmc_controller_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mc_controller_funtyp_cconfig                              = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_mc_controller_funtyp_cconfig__durationofctrlloop          = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u32),

    eoprot_ep_mc_controller_funtyp_cstatus                              = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd),
    eoprot_ep_mc_controller_funtyp_cstatus__alljomoinitted              = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_u08),
    eoprot_ep_mc_controller_funtyp_ccmmnds__go2stateofcontroller        = EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_u08)
} eOprot_ep_mc_controller_funtyp_t; 

enum { eoprot_ep_mc_controller_funtyps_numberof = 5 };  // it MUST be equal to the number of tags in a controller. 


// - structures implementing the endpoints


/** @typedef    typedef struct eOprot_ep_mc_upperarm_t;
    @brief      contains all the variables in the upperarm.
 **/
typedef struct                  // 152*4+40*4+24 = 792              
{
    eOmc_joint_t                joints[eoprot_ep_mc_joints_numberof_upperarm]; 
    eOmc_motor_t                motors[eoprot_ep_mc_motors_numberof_upperarm];
    eOmc_controller_t           thecontroller;
} eOprot_ep_mc_upperarm_t;      EO_VERIFYsizeof(eOprot_ep_mc_upperarm_t, 792);


/** @typedef    typedef struct eOprot_ep_mc_lowerarm_t;
    @brief      contains all the variables in the lowerarm.
 **/
typedef struct                  // 152*12+40*12+24 = 2328              
{
    eOmc_joint_t                joints[eoprot_ep_mc_joints_numberof_lowerarm]; 
    eOmc_motor_t                motors[eoprot_ep_mc_motors_numberof_lowerarm];
    eOmc_controller_t           thecontroller;
} eOprot_ep_mc_lowerarm_t;      EO_VERIFYsizeof(eOprot_ep_mc_lowerarm_t, 2328);


/** @typedef    typedef struct eOprot_ep_mc_torso_t;
    @brief      contains all the variables in the torso.
 **/
typedef struct                  // 152*3+40*3+24 = 600             
{
    eOmc_joint_t                joints[eoprot_ep_mc_joints_numberof_torso]; 
    eOmc_motor_t                motors[eoprot_ep_mc_motors_numberof_torso];
    eOmc_controller_t           thecontroller;
} eOprot_ep_mc_torso_t;         EO_VERIFYsizeof(eOprot_ep_mc_torso_t, 600);


/** @typedef    typedef struct eOprot_ep_mc_upperleg_t;
    @brief      contains all the variables in the upperleg.
 **/
typedef struct                  // 152*4+40*4+24 = 792              
{
    eOmc_joint_t                joints[eoprot_ep_mc_joints_numberof_upperleg]; 
    eOmc_motor_t                motors[eoprot_ep_mc_motors_numberof_upperleg];
    eOmc_controller_t           thecontroller;
} eOprot_ep_mc_upperleg_t;      EO_VERIFYsizeof(eOprot_ep_mc_upperleg_t, 792);


/** @typedef    typedef struct eOprot_ep_mc_lowerleg_t;
    @brief      contains all the variables in the lowerleg.
 **/
typedef struct                  // 152*2+40*2+24 = 408              
{
    eOmc_joint_t                joints[eoprot_ep_mc_joints_numberof_lowerleg]; 
    eOmc_motor_t                motors[eoprot_ep_mc_motors_numberof_lowerleg];
    eOmc_controller_t           thecontroller;
} eOprot_ep_mc_lowerleg_t;      EO_VERIFYsizeof(eOprot_ep_mc_lowerleg_t, 408);


typedef eOprot_ep_mc_upperarm_t eOprot_ep_mc_leftupperarm_t;
typedef eOprot_ep_mc_upperarm_t eOprot_ep_mc_rightupperarm_t;

typedef eOprot_ep_mc_lowerarm_t eOprot_ep_mc_leftlowerarm_t;
typedef eOprot_ep_mc_lowerarm_t eOprot_ep_mc_rightlowerarm_t;


typedef eOprot_ep_mc_upperleg_t eOprot_ep_mc_leftupperleg_t;
typedef eOprot_ep_mc_upperleg_t eOprot_ep_mc_rightupperleg_t;

typedef eOprot_ep_mc_lowerleg_t eOprot_ep_mc_leftlowerleg_t;
typedef eOprot_ep_mc_lowerleg_t eOprot_ep_mc_rightlowerleg_t;
  
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - these three are used to configure the endpoint into a EOnvsCfg object
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern uint16_t eoprot_ep_mc_variable_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of variables given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of variables.
  */
extern uint16_t eoprot_ep_mc_variables_numberof_Get(eOprotEP_t ep);


/** @fn         extern uint16_t eoprot_ep_mc_variable_progressivenumber_Get(eOprotEP_t ep)
    @brief      This function retrieves the progressive number of a variable given the endpoint @e ep and the @e id
    @param      ep              the endpoint
    @return     the progressive number or EOK_uint16dummy if invalid .
  */
extern uint16_t eoprot_ep_mc_variable_progressivenumber_Get(eOprotEP_t ep, eOprotID_t id);

/** @fn         extern uint16_t eoprot_ep_mc_joints_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of joints given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of joints.
  */
extern uint16_t eoprot_ep_mc_joints_numberof_Get(eOprotEP_t ep);


/** @fn         extern uint16_t eoprot_ep_mc_motors_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of motors given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of motors.
  */
extern uint16_t eoprot_ep_mc_motors_numberof_Get(eOprotEP_t ep);


/** @fn         extern uint16_t eoprot_ep_mc_controllers_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of controllers given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of controllers.
  */
extern uint16_t eoprot_ep_mc_controllers_numberof_Get(eOprotEP_t ep);


extern uint16_t eoprot_ep_mc_ram_sizeof_Get(eOprotEP_t ep);

extern uint16_t eoprot_ep_mc_variable_ram_sizeof_Get(eOprotEP_t ep, eOprotID_t id);

extern void* eoprot_ep_mc_variable_ram_Extract(void* epram, eOprotEP_t ep, eOprotID_t id);

extern void* eoprot_ep_mc_variable_rom_Get(eOprotEP_t ep, eOprotID_t id);

extern const eOmc_joint_t* eoprot_ep_mc_joint_default_Get(void);

extern const eOmc_motor_t* eoprot_ep_mc_motor_default_Get(void);

extern const eOmc_controller_t* eoprot_ep_mc_controller_default_Get(void);


/** @}            
    end of group eo_protocol_ep_mc  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


