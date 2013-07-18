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
#ifndef _EOPROTOCOLMN_H_
#define _EOPROTOCOLMN_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolMN.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolMN Protocol for management endpoint 
    This file contains tags, rw mode, and callback functions specific of the eoprot_endpoint_management endpoint.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoManagement.h"
#include "EOnv.h"
#include "EOrop.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { eoprot_entities_mn_numberof = eomn_entities_numberof };

// - entity comm


/** @typedef    typedef enum eOprot_tag_mn_comm_t
    @brief      It contains the tags for all variables of the comm entity.
                See definition of eOmn_comm_t (and its fields) in file EoManagement.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_mn_comm_wholeitem                                    = 0,    
    eoprot_tag_mn_comm_cmmnds_ropsigcfg                             = 1
} eOprot_tag_mn_comm_t;

enum { eoprot_tags_mn_comm_numberof = 2 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_rwm_mn_comm_t
    @brief      It contains the rw modes for all variables of the comm entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_mn_comm_t.
 **/
typedef enum
{
    eoprot_rwm_mn_comm_wholeitem                                    = eo_nv_rwmode_RO,
    eoprot_rwm_mn_comm_cmmnds_ropsigcfg                             = eo_nv_rwmode_RW
} eOprot_rwm_mn_comm_t; 

enum { eoprot_rwms_mn_comm_numberof = 2 };  // it MUST be equal to the number of rw modes. 



// - entity appl
  

/** @typedef    typedef enum eOprot_tag_mn_appl_t
    @brief      It contains the tags for all variables of the appl entity.
                See definition of eOmn_appl_t (and its fields) in file EoManagement.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_mn_appl_wholeitem                                    = 0,
    eoprot_tag_mn_appl_config                                       = 1,
    eoprot_tag_mn_appl_status                                       = 2,
    eoprot_tag_mn_appl_cmmnds_go2state                              = 3
} eOprot_tag_mn_appl_t;

enum { eoprot_tags_mn_appl_numberof = 4 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_rwm_mn_appl_t
    @brief      It contains the rw modes for all variables of the appl entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_mn_appl_t.
 **/
typedef enum
{
    eoprot_rwm_mn_appl_wholeitem                                    = eo_nv_rwmode_RO,
    eoprot_rwm_mn_appl_config                                       = eo_nv_rwmode_RW,
    eoprot_rwm_mn_appl_status                                       = eo_nv_rwmode_RO,
    eoprot_rwm_mn_appl_cmmnds_go2state                              = eo_nv_rwmode_RW
} eOprot_rwm_mn_appl_t; 

enum { eoprot_rwms_mn_appl_numberof = 4 };  // it MUST be equal to the number of rw modes. 


// - memory organization in the endpoint
  
  
/** @typedef    typedef struct eOprot_template_mn_t;
    @brief      It is a template for the organisation of comm and appl entities in the management endpoint.
                The effective number may depend on the board.
 **/
typedef struct                  // 104+24+0 = 128              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eOprot_template_mn_t;         //EO_VERIFYsizeof(eOprot_template_mn_t, 128);  

  
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

// functions used inside EoProtocol

/** @fn         extern eOresult_t eoprot_mn_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity)
    @brief      This function loads the maximum number of entities managed by the endpoint
    @param      brd                     the board
    @param      numberofeachentity      array of the values.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
  */
extern eOresult_t eoprot_mn_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity);


/** @fn         extern uint16_t eoprot_mn_endpoint_sizeof_get(eOprotBRD_t brd)
    @brief      it tells the size of the ram used for a given board.
    @param      brd                 the number of board 
    @return     the size or 0 upon failure.
 **/
extern uint16_t eoprot_mn_endpoint_sizeof_get(eOprotBRD_t brd);


/** @fn         extern eOresult_t eoprot_mn_config_endpoint_ram(eOprotBRD_t brd, void *ram)
    @brief      This function loads the ram used by the endpoint
    @param      brd                     the board
    @param      ram                     the externally allocated ram.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
  */
extern eOresult_t eoprot_mn_config_endpoint_ram(eOprotBRD_t brd, void *ram);


/** @fn         extern uint16_t eoprot_mn_variable_sizeof_get(eOprotID32_t id)
    @brief      This function gets the size of the variable identified by its ID
    @param      id                      the ID
    @return     the size or 0 upon failure.
  */
extern uint16_t eoprot_mn_variable_sizeof_get(eOprotID32_t id);


/** @fn         extern void* eoprot_mn_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index)
    @brief      This function gets the ram used by the specified entity
    @param      brd                     the board number
    @param      ent                     the entity
    @param      index                   the entity index
    @return     the ram or NULL upon failure.
  */
extern void* eoprot_mn_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index);


/** @fn         extern uint16_t eoprot_mn_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent)
    @brief      This function gets the size of the entity
    @param      brd                     the board number
    @param      ent                     the entity
    @return     the size or 0 upon failure.
  */
extern uint16_t eoprot_mn_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent);


/** @fn         extern uint8_t eoprot_mn_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent)
    @brief      This function gets the number of entities in the board
    @param      brd                     the board number
    @param      ent                     the entity
    @return     the number or 0 upon failure or if no entities of that kind.
  */
extern uint8_t eoprot_mn_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent);


// functions used to configure the eOnvset_EPcfg_t

extern uint16_t eoprot_mn_numberofvariables_get(eOprotBRD_t brd);

extern eObool_t eoprot_mn_id_isvalid(eOprotBRD_t brd, eOnvID32_t id);

extern eOprotID32_t eoprot_mn_prognum2id(eOprotBRD_t brd, eOprotProgNumber_t prog);

extern eOprotProgNumber_t eoprot_mn_id2prognum(eOprotBRD_t brd, eOprotID32_t id);

extern void* eoprot_mn_eonvrom_get(/*eOprotBRD_t brd,*/ eOprotID32_t id);

extern void* eoprot_mn_ramofvariable_get(eOprotBRD_t brd, eOprotID32_t id);

extern void* eoprot_mn_ramofvariable_extract(eOprotBRD_t brd, eOprotID32_t id, void* epram);


// - declaration of extern overridable functions ----------------------------------------------------------------------

extern void eoprot_fun_INITIALISE_mn(eOprotIP_t ip, void *ram);


extern void eoprot_fun_INIT_mn_comm_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_comm_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mn_comm_cmmnds_ropsigcfg(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_comm_cmmnds_ropsigcfg(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mn_appl_config(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_appl_config(const EOnv* nv, const eOropdescriptor_t* rd);


extern void eoprot_fun_INIT_mn_appl_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_appl_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mn_appl_status(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_appl_status(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mn_appl_cmmnds_go2state(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_appl_cmmnds_go2state(const EOnv* nv, const eOropdescriptor_t* rd);


/** @}            
    end of group eo_EoProtocolMN  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


