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
#ifndef _EOPROTOCOLMC_H_
#define _EOPROTOCOLMC_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolMC.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolMC Protocol for motion control endpoint 
    This file contains tags, rw mode, and callback functions specific of the eoprot_endpoint_motioncontrol endpoint.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EOnv.h"
#include "EOrop.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


enum { eoprot_entities_mc_numberof = eomc_entities_numberof };

// - entity joint

/** @typedef    typedef enum eOprot_tag_mc_joint_t
    @brief      It contains the tags for all variables of the joint entity.
                See definition of eOmc_joint_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_mc_joint_wholeitem                                   =  0,
    eoprot_tag_mc_joint_config                                      =  1,
    eoprot_tag_mc_joint_config_pidposition                          =  2,
    eoprot_tag_mc_joint_config_pidvelocity                          =  3,
    eoprot_tag_mc_joint_config_pidtorque                            =  4,
    eoprot_tag_mc_joint_config_impedance                            =  5,
    eoprot_tag_mc_joint_config_minpositionofjoint                   =  6,
    eoprot_tag_mc_joint_config_maxpositionofjoint                   =  7,
    eoprot_tag_mc_joint_config_velocitysetpointtimeout              =  8,
    eoprot_tag_mc_joint_config_motionmonitormode                    =  9,
    eoprot_tag_mc_joint_config_encoderconversionfactor              = 10,
    eoprot_tag_mc_joint_config_encoderconversionoffset              = 11,
    eoprot_tag_mc_joint_status                                      = 12,
    eoprot_tag_mc_joint_status_basic                                = 13,
    eoprot_tag_mc_joint_status_ofpid                                = 14,
    eoprot_tag_mc_joint_status_chamaleon04                          = 15,
    eoprot_tag_mc_joint_inputs                                      = 16,
    eoprot_tag_mc_joint_inputs_externallymeasuredtorque             = 17,
    eoprot_tag_mc_joint_cmmnds_calibration                          = 18,
    eoprot_tag_mc_joint_cmmnds_setpoint                             = 19,
    eoprot_tag_mc_joint_cmmnds_stoptrajectory                       = 20,
    eoprot_tag_mc_joint_cmmnds_controlmode                          = 21 
} eOprot_tag_mc_joint_t;

enum { eoprot_tags_mc_joint_numberof = 22 };  // it MUST be equal to the number of tags    


/** @typedef    typedef enum eOprot_rwm_mc_joint_t
    @brief      It contains the rw modes for all variables of the joint entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_mc_joint_t.
                See definition of eOmc_joint_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_rwm_mc_joint_wholeitem                                   = eo_nv_rwmode_RO,
    eoprot_rwm_mc_joint_config                                      = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_pidposition                          = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_pidvelocity                          = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_pidtorque                            = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_impedance                            = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_minpositionofjoint                   = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_maxpositionofjoint                   = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_velocitysetpointtimeout              = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_motionmonitormode                    = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_encoderconversionfactor              = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_config_encoderconversionoffset              = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_status                                      = eo_nv_rwmode_RO,
    eoprot_rwm_mc_joint_status_basic                                = eo_nv_rwmode_RO,
    eoprot_rwm_mc_joint_status_ofpid                                = eo_nv_rwmode_RO,
    eoprot_rwm_mc_joint_status_chamaleon04                          = eo_nv_rwmode_RO,
    eoprot_rwm_mc_joint_inputs                                      = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_inputs_externallymeasuredtorque             = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_cmmnds_calibration                          = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_cmmnds_setpoint                             = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_cmmnds_stoptrajectory                       = eo_nv_rwmode_RW,
    eoprot_rwm_mc_joint_cmmnds_controlmode                          = eo_nv_rwmode_RW
} eOprot_rwm_mc_joint_t;    

enum { eoprot_rwms_mc_joint_numberof = 22 };  // it MUST be equal to the number of rw modes  


// - definition of the motor


/** @typedef    typedef enum eOprot_tag_mc_motor_t
    @brief      It contains the tags for all variables of the motor entity.
                See definition of eOmc_joint_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_mc_motor_wholeitem                                   = 0,
    eoprot_tag_mc_motor_config                                      = 1,
    eoprot_tag_mc_motor_config_pidcurrent                           = 2,
    eoprot_tag_mc_motor_config_maxvelocityofmotor                   = 3,
    eoprot_tag_mc_motor_config_maxcurrentofmotor                    = 4,    
    eoprot_tag_mc_motor_status                                      = 5,
    eoprot_tag_mc_motor_status_basic                                = 6,
    eoprot_tag_mc_motor_status_chamaleon04                          = 7
} eOprot_tag_mc_motor_t;

enum { eoprot_tags_mc_motor_numberof = 8 };   // it MUST be equal to the number of tags 


/** @typedef    typedef enum eOprot_rwm_mc_motor_t
    @brief      IIt contains the rw modes for all variables of the motor entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_mc_motor_t.
                See definition of eOmc_motor_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_rwm_mc_motor_wholeitem                                   = eo_nv_rwmode_RO,    
    eoprot_rwm_mc_motor_config                                      = eo_nv_rwmode_RW,
    eoprot_rwm_mc_motor_config_pidcurrent                           = eo_nv_rwmode_RW,
    eoprot_rwm_mc_motor_config_maxvelocityofmotor                   = eo_nv_rwmode_RW,
    eoprot_rwm_mc_motor_config_maxcurrentofmotor                    = eo_nv_rwmode_RW,
    eoprot_rwm_mc_motor_status                                      = eo_nv_rwmode_RO,
    eoprot_rwm_mc_motor_status_basic                                = eo_nv_rwmode_RO,
    eoprot_rwm_mc_motor_status_chamaleon04                          = eo_nv_rwmode_RO 
} eOprot_rwm_mc_motor_t;  

enum { eoprot_rwms_mc_motor_numberof = 8 };   // it MUST be equal to the number of rw modes



// - definition of the controller



/** @typedef    typedef enum eOprot_tag_mc_controller_t
    @brief      It contains the tags for all variables of the controller entity.
                See definition of eOmc_controller_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
     eoprot_tag_mc_controller_wholeitem                              = 0,
     eoprot_tag_mc_controller_config                                 = 1,
     eoprot_tag_mc_controller_config_durationofctrlloop              = 2,
     eoprot_tag_mc_controller_status                                 = 3,
     eoprot_tag_mc_controller_status_alljomoinitted                  = 4,
     eoprot_tag_mc_controller_cmmnds_go2stateofcontroller            = 5
} eOprot_tag_mc_controller_t;

enum { eoprot_tags_mc_controller_numberof = 6 };  // it MUST be equal to the number of tags


/** @typedef    typedef enum eOprot_rwm_mc_controller_t
    @brief      It contains the rw modes for all variables of the motor entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_mc_controller_t.
                See definition of eOmc_controller_t (and its fields) in file EoMotionControl.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_rwm_mc_controller_wholeitem                              = eo_nv_rwmode_RO,
    eoprot_rwm_mc_controller_config                                 = eo_nv_rwmode_RW,
    eoprot_rwm_mc_controller_config_durationofctrlloop              = eo_nv_rwmode_RW,

    eoprot_rwm_mc_controller_status                                 = eo_nv_rwmode_RO,
    eoprot_rwm_mc_controller_status_alljomoinitted                  = eo_nv_rwmode_RO,
    eoprot_rwm_mc_controller_cmmnds_go2stateofcontroller            = eo_nv_rwmode_RW
} eOprot_rwm_mc_controller_t; 

enum { eoprot_rwms_mc_controller_numberof = 6 };  // it MUST be equal to the number of rw modes 


// - structures implementing the endpoints


/** @typedef    typedef struct eOprot_template_mc_t;
    @brief      It is a template for the organisation of joint, motor, controller entities in the motion control endpoint.
                The effective number depends on the board.
 **/
typedef struct                  // 152*1+40*1+24 = 216              
{
    eOmc_joint_t                joints[1]; 
    eOmc_motor_t                motors[1];
    eOmc_controller_t           thecontroller;
} eOprot_template_mc_t;         //EO_VERIFYsizeof(eOprot_template_mc_t, 216);


  
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOprot_nvset_interface_t eoprot_eonvset_interface_mc;  // used by EOnvset to access the endpoint


// - declaration of extern public functions ---------------------------------------------------------------------------


// functions used inside EoProtocol

/** @fn         extern eOresult_t eoprot_mc_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity)
    @brief      This function loads the maximum number of entities managed by the endpoint
    @param      brd                     the board
    @param      numberofeachentity      array of the values.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
  */
extern eOresult_t eoprot_mc_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity);


/** @fn         extern uint16_t eoprot_mc_endpoint_sizeof_get(eOprotBRD_t brd)
    @brief      it tells the size of the ram used for a given board.
    @param      brd                 the number of board 
    @return     the size or 0 upon failure.
 **/
extern uint16_t eoprot_mc_endpoint_sizeof_get(eOprotBRD_t brd);


/** @fn         extern eOresult_t eoprot_mc_config_endpoint_ram(eOprotBRD_t brd, void *ram)
    @brief      This function loads the ram used by the endpoint
    @param      brd                     the board
    @param      ram                     the externally allocated ram.
    @param      sizeofram               its size
    @return     eores_OK if successful or eores_NOK_generic upon failure.
  */
extern eOresult_t eoprot_mc_config_endpoint_ram(eOprotBRD_t brd, void *ram, uint16_t sizeofram);


/** @fn         extern void* eoprot_mc_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id)
    @brief      This function gets the ram used by the specified variable
    @param      brd                     the board number
    @param      id                      the id
    @return     the ram or NULL upon failure.
  */
extern void* eoprot_mc_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id);


/** @fn         extern uint16_t eoprot_mc_variable_sizeof_get(eOprotID32_t id)
    @brief      This function gets the size of the variable identified by its ID
    @param      id                      the ID
    @return     the size or 0 upon failure.
  */
extern uint16_t eoprot_mc_variable_sizeof_get(eOprotID32_t id);


/** @fn         extern void* eoprot_mc_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index)
    @brief      This function gets the ram used by the specified entity
    @param      brd                     the board number
    @param      ent                     the entity
    @param      index                   the entity index
    @return     the ram or NULL upon failure.
  */
extern void* eoprot_mc_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index);


/** @fn         extern uint16_t eoprot_mc_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent)
    @brief      This function gets the size of the entity
    @param      brd                     the board number
    @param      ent                     the entity
    @return     the size or 0 upon failure.
  */
extern uint16_t eoprot_mc_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent);


/** @fn         extern uint8_t eoprot_mc_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent)
    @brief      This function gets the number of entities in the board
    @param      brd                     the board number
    @param      ent                     the entity
    @return     the number or 0 upon failure or if no entities of that kind.
  */
extern uint8_t eoprot_mc_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent);



// - declaration of extern overridable functions ----------------------------------------------------------------------


extern void eoprot_fun_INITIALISE_mc(eOprotIP_t ip, void *ram);


extern void eoprot_fun_INIT_mc_joint_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_config(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_config_pidposition(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_config_pidvelocity(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd);
   
extern void eoprot_fun_INIT_mc_joint_config_pidtorque(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_config_impedance(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_config_minpositionofjoint(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_minpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd);
   
extern void eoprot_fun_INIT_mc_joint_config_maxpositionofjoint(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_maxpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_config_motionmonitormode(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_config_encoderconversionfactor(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_config_encoderconversionoffset(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_config_encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd);


extern void eoprot_fun_INIT_mc_joint_status(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_status(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_status_basic(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_status_basic(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_status_ofpid(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_status_ofpid(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_status_chamaleon04(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_status_chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd);


extern void eoprot_fun_INIT_mc_joint_inputs(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_inputs(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv);   
extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd);


extern void eoprot_fun_INIT_mc_joint_cmmnds_calibration(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_cmmnds_setpoint(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_joint_cmmnds_controlmode(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd);


extern void eoprot_fun_INIT_mc_motor_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_motor_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_motor_config(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_motor_config_pidcurrent(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_motor_config_maxvelocityofmotor(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_motor_config_maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_motor_config_maxcurrentofmotor(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_motor_status(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_motor_status(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_motor_status_basic(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_motor_status_basic(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_motor_status_chamaleon04(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_motor_status_chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd);
   
   
extern void eoprot_fun_INIT_mc_controller_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_controller_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_controller_config(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_controller_config(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_controller_config_durationofctrlloop(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_controller_config_durationofctrlloop(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_controller_status(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_controller_status(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_controller_status_alljomoinitted(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_controller_status_alljomoinitted(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mc_controller_cmmnds_go2stateofcontroller(const EOnv* nv);
extern void eoprot_fun_UPDT_mc_controller_cmmnds_go2stateofcontroller(const EOnv* nv, const eOropdescriptor_t* rd);

/** @}            
    end of group eo_EoProtocolMC  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


