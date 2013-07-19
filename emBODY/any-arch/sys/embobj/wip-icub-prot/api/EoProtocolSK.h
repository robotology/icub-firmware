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
#ifndef _EOPROTOCOLSK_H_
#define _EOPROTOCOLSK_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolSK.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolSK Protocol for skin endpoint 
    This file contains tags, rw mode, and callback functions specific of the eoprot_endpoint_skin endpoint.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoSkin.h"
#include "EOnv.h"
#include "EOrop.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


enum { eoprot_entities_sk_numberof = eosk_entities_numberof };

// - entity skin


/** @typedef    typedef enum eOprot_tag_sk_skin_t
    @brief      It contains the tags for all variables of the skin entity.
                See definition of eOsk_skin_t (and its fields) in file EoSkin.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_sk_skin_wholeitem                                     = 0,
    eoprot_tag_sk_skin_config_sigmode                                = 1,
    eoprot_tag_sk_skin_status_arrayof10canframes                     = 2
} eOprot_tag_sk_skin_t;

enum { eoprot_tags_sk_skin_numberof = 3 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_rwm_sk_skin_t
    @brief      It contains the rw modes for all variables of the skin entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_sk_skin_t.
 **/
typedef enum
{
    eoprot_rwm_sk_skin_wholeitem                                  = eo_nv_rwmode_RO,
    eoprot_rwm_sk_skin_config_sigmode                             = eo_nv_rwmode_RW,
    eoprot_rwm_sk_skin_status_arrayof10canframes                  = eo_nv_rwmode_RO
} eOprot_rwm_sk_skin_t; 

enum { eoprot_rwms_sk_skin_numberof = 3 };  // it MUST be equal to the number of rw modes. 


// - structures implementing the endpoint
  

/** @typedef    typedef struct eOprot_template_sk_t;
    @brief      It is a template for the organisation of skin entities in the skin endpoint.
                The effective number may depend on the board.
 **/
typedef struct                  // 176+0 = 176              
{
    eOsk_skin_t                 skin;
} eOprot_template_sk_t;         //EO_VERIFYsizeof(eOprot_template_sk_t, 176);



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOprot_nvset_interface_t eoprot_eonvset_interface_sk;  // used by EOnvset to access the endpoint


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

// functions used inside EoProtocol

/** @fn         extern eOresult_t eoprot_sk_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity)
    @brief      This function loads the maximum number of entities managed by the endpoint
    @param      brd                     the board
    @param      numberofeachentity      array of the values.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
  */
extern eOresult_t eoprot_sk_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity);


/** @fn         extern uint16_t eoprot_sk_endpoint_sizeof_get(eOprotBRD_t brd)
    @brief      it tells the size of the ram used for a given board.
    @param      brd                 the number of board 
    @return     the size or 0 upon failure.
 **/
extern uint16_t eoprot_sk_endpoint_sizeof_get(eOprotBRD_t brd);


/** @fn         extern eOresult_t eoprot_sk_config_endpoint_ram(eOprotBRD_t brd, void *ram)
    @brief      This function loads the ram used by the endpoint
    @param      brd                     the board
    @param      ram                     the externally allocated ram.
    @param      sizeofram               its size
    @return     eores_OK if successful or eores_NOK_generic upon failure.
  */
extern eOresult_t eoprot_sk_config_endpoint_ram(eOprotBRD_t brd, void *ram, uint16_t sizeofram);


/** @fn         extern void* eoprot_sk_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id)
    @brief      This function gets the ram used by the specified variable
    @param      brd                     the board number
    @param      id                      the id
    @return     the ram or NULL upon failure.
  */
extern void* eoprot_sk_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id);


/** @fn         extern uint16_t eoprot_sk_variable_sizeof_get(eOprotID32_t id)
    @brief      This function gets the size of the variable identified by its ID
    @param      id                      the ID
    @return     the size or 0 upon failure.
  */
extern uint16_t eoprot_sk_variable_sizeof_get(eOprotID32_t id);


/** @fn         extern void* eoprot_sk_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index)
    @brief      This function gets the ram used by the specified entity
    @param      brd                     the board number
    @param      ent                     the entity
    @param      index                   the entity index
    @return     the ram or NULL upon failure.
  */
extern void* eoprot_sk_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index);


/** @fn         extern uint16_t eoprot_sk_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent)
    @brief      This function gets the size of the entity
    @param      brd                     the board number
    @param      ent                     the entity
    @return     the size or 0 upon failure.
  */
extern uint16_t eoprot_sk_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent);


/** @fn         extern uint8_t eoprot_sk_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent)
    @brief      This function gets the number of entities in the board
    @param      brd                     the board number
    @param      ent                     the entity
    @return     the number or 0 upon failure or if no entities of that kind.
  */
extern uint8_t eoprot_sk_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent);




// - declaration of extern overridable functions ----------------------------------------------------------------------

extern void eoprot_fun_INITIALISE_sk(eOprotIP_t ip, void *ram);

extern void eoprot_fun_INIT_sk_skin_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_sk_skin_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_sk_skin_config_sigmode(const EOnv* nv);
extern void eoprot_fun_UPDT_sk_skin_config_sigmode(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_sk_skin_status_arrayof10canframes(const EOnv* nv);
extern void eoprot_fun_UPDT_sk_skin_status_arrayof10canframes(const EOnv* nv, const eOropdescriptor_t* rd);



/** @}            
    end of group eo_EoProtocolSK  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


