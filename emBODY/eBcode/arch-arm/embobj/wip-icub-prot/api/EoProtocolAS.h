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
#ifndef _EOPROTOCOLAS_H_
#define _EOPROTOCOLAS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolAS.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolAS Protocol for analog sensors endpoint 
    This file contains tags, rw mode, and callback functions specific of the eoprot_endpoint_analogsensors endpoint.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoAnalogSensors.h"
#include "EOnv.h"
#include "EOrop.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { eoprot_entities_as_numberof = eoas_entities_numberof };


// - entity strain


/** @typedef    typedef enum eOprot_tag_as_strain_t
    @brief      It contains the tags for all variables of the strain entity.
                See definition of eOas_strain_t (and its fields) in file EoAnalogSensors.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_as_strain_wholeitem                                  = 0,
    eoprot_tag_as_strain_config                                     = 1,
    eoprot_tag_as_strain_config_mode                                = 2,
    eoprot_tag_as_strain_config_datarate                            = 3,
    eoprot_tag_as_strain_config_signaloncefullscale                 = 4,
    eoprot_tag_as_strain_status                                     = 5,
    eoprot_tag_as_strain_status_fullscale                           = 6,
    eoprot_tag_as_strain_status_calibratedvalues                    = 7,
    eoprot_tag_as_strain_status_uncalibratedvalues                  = 8
} eOprot_tag_as_strain_t;

enum { eoprot_tags_as_strain_numberof = 9 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_rwm_as_strain_t
    @brief      It contains the rw modes for all variables of the strain entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_as_strain_t.
 **/
typedef enum
{
    eoprot_rwm_as_strain_wholeitem                                  = eo_nv_rwmode_RO,
    eoprot_rwm_as_strain_config                                     = eo_nv_rwmode_RW,
    eoprot_rwm_as_strain_config_mode                                = eo_nv_rwmode_RW,
    eoprot_rwm_as_strain_config_datarate                            = eo_nv_rwmode_RW,
    eoprot_rwm_as_strain_config_signaloncefullscale                 = eo_nv_rwmode_RW,
    eoprot_rwm_as_strain_status                                     = eo_nv_rwmode_RO,
    eoprot_rwm_as_strain_status_fullscale                           = eo_nv_rwmode_RO,
    eoprot_rwm_as_strain_status_calibratedvalues                    = eo_nv_rwmode_RO,
    eoprot_rwm_as_strain_status_uncalibratedvalues                  = eo_nv_rwmode_RO    
} eOprot_rwm_as_strain_t; 

enum { eoprot_rwms_as_strain_numberof = 9 };  // it MUST be equal to the number of rw modes. 



// - entity mais
 

/** @typedef    typedef enum eOprot_tag_as_mais_t
    @brief      It contains the tags for all variables in the mais entity.
                See definition of eOas_mais_t (and its fields) in file EoAnalogSensors.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_as_mais_wholeitem                                    = 0,
    eoprot_tag_as_mais_config                                       = 1,
    eoprot_tag_as_mais_config_mode                                  = 2,
    eoprot_tag_as_mais_config_datarate                              = 3,
    eoprot_tag_as_mais_config_resolution                            = 4,
    eoprot_tag_as_mais_status                                       = 5,
    eoprot_tag_as_mais_status_the15values                           = 6
} eOprot_tag_as_mais_t;

enum { eoprot_tags_as_mais_numberof = 7 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_rwm_as_mais_t
    @brief      It contains the rw modes for all variables of the mais entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_as_mais_t.
 **/
typedef enum
{
    eoprot_rwm_as_mais_wholeitem                                    = eo_nv_rwmode_RO,
    eoprot_rwm_as_mais_config                                       = eo_nv_rwmode_RW,
    eoprot_rwm_as_mais_config_mode                                  = eo_nv_rwmode_RW,
    eoprot_rwm_as_mais_config_datarate                              = eo_nv_rwmode_RW,
    eoprot_rwm_as_mais_config_resolution                            = eo_nv_rwmode_RW,
    eoprot_rwm_as_mais_status                                       = eo_nv_rwmode_RO,
    eoprot_rwm_as_mais_status_the15values                           = eo_nv_rwmode_RO
} eOprot_rwm_as_mais_t; 

enum { eoprot_rwms_as_mais_numberof = 7 };  // it MUST be equal to the number of rw modes. 


// - structures implementing the endpoints

/** @typedef    typedef struct eOprot_template_as_t;
    @brief      It is a template for the organisation of strain and mais entities in the analog sensors endpoint.
                The effective number depends on the board.
 **/
typedef struct                  // 56*1+48*1 = 104              
{
    eOas_strain_t               strains[1]; 
    eOas_mais_t                 maises[1];
} eOprot_template_as_t;      //EO_VERIFYsizeof(eOprot_template_as_t, 104);
  
  

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - these three are used to configure the endpoint into a EOnvsCfg object
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

// - declaration of extern overridable functions ----------------------------------------------------------------------

// functions used inside EoProtocol

/** @fn         extern eOresult_t eoprot_as_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity)
    @brief      This function loads the maximum number of entities managed by the endpoint
    @param      brd                     the board
    @param      numberofeachentity      array of the values.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
  */
extern eOresult_t eoprot_as_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity);


/** @fn         extern uint16_t eoprot_as_endpoint_sizeof_get(eOprotBRD_t brd)
    @brief      it tells the size of the ram used for a given board.
    @param      brd                 the number of board 
    @return     the size or 0 upon failure.
 **/
extern uint16_t eoprot_as_endpoint_sizeof_get(eOprotBRD_t brd);


/** @fn         extern eOresult_t eoprot_as_config_endpoint_ram(eOprotBRD_t brd, void *ram)
    @brief      This function loads the ram used by the endpoint
    @param      brd                     the board
    @param      ram                     the externally allocated ram.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
  */
extern eOresult_t eoprot_as_config_endpoint_ram(eOprotBRD_t brd, void *ram);


/** @fn         extern uint16_t eoprot_as_variable_sizeof_get(eOprotID32_t id)
    @brief      This function gets the size of the variable identified by its ID
    @param      id                      the ID
    @return     the size or 0 upon failure.
  */
extern uint16_t eoprot_as_variable_sizeof_get(eOprotID32_t id);


/** @fn         extern void* eoprot_as_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index)
    @brief      This function gets the ram used by the specified entity
    @param      brd                     the board number
    @param      ent                     the entity
    @param      index                   the entity index
    @return     the ram or NULL upon failure.
  */
extern void* eoprot_as_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index);


/** @fn         extern uint16_t eoprot_as_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent)
    @brief      This function gets the size of the entity
    @param      brd                     the board number
    @param      ent                     the entity
    @return     the size or 0 upon failure.
  */
extern uint16_t eoprot_as_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent);


/** @fn         extern uint8_t eoprot_as_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent)
    @brief      This function gets the number of entities in the board
    @param      brd                     the board number
    @param      ent                     the entity
    @return     the number or 0 upon failure or if no entities of that kind.
  */
extern uint8_t eoprot_as_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent);


// functions used to configure the eOnvset_EPcfg_t

extern uint16_t eoprot_as_numberofvariables_get(eOprotBRD_t brd);

extern eObool_t eoprot_as_id_isvalid(eOprotBRD_t brd, eOnvID32_t id);

extern eOprotID32_t eoprot_as_prognum2id(eOprotBRD_t brd, eOprotProgNumber_t prog);

extern eOprotProgNumber_t eoprot_as_id2prognum(eOprotBRD_t brd, eOprotID32_t id);

extern void* eoprot_as_eonvrom_get(/*eOprotBRD_t brd,*/ eOprotID32_t id);

extern void* eoprot_as_ramofvariable_get(eOprotBRD_t brd, eOprotID32_t id);

extern void* eoprot_as_ramofvariable_extract(eOprotBRD_t brd, eOprotID32_t id, void* epram);



// - declaration of extern overridable functions ----------------------------------------------------------------------


extern void eoprot_fun_INITIALISE_as(eOprotIP_t ip, void *ram);


extern void eoprot_fun_INIT_as_strain_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_as_strain_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_strain_config(const EOnv* nv);
extern void eoprot_fun_UPDT_as_strain_config(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_strain_config_mode(const EOnv* nv);
extern void eoprot_fun_UPDT_as_strain_config_mode(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_strain_config_datarate(const EOnv* nv);
extern void eoprot_fun_UPDT_as_strain_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_strain_config_signaloncefullscale(const EOnv* nv);
extern void eoprot_fun_UPDT_as_strain_config_signaloncefullscale(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_strain_status(const EOnv* nv);
extern void eoprot_fun_UPDT_as_strain_status(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_strain_status_fullscale(const EOnv* nv);
extern void eoprot_fun_UPDT_as_strain_status_fullscale(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_strain_status_calibratedvalues(const EOnv* nv);
extern void eoprot_fun_UPDT_as_strain_status_calibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_strain_status_uncalibratedvalues(const EOnv* nv);
extern void eoprot_fun_UPDT_as_strain_status_uncalibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd);


extern void eoprot_fun_INIT_as_mais_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_as_mais_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_mais_config(const EOnv* nv);
extern void eoprot_fun_UPDT_as_mais_config(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_mais_config_mode(const EOnv* nv);
extern void eoprot_fun_UPDT_as_mais_config_mode(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_mais_config_datarate(const EOnv* nv);
extern void eoprot_fun_UPDT_as_mais_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_mais_config_resolution(const EOnv* nv);
extern void eoprot_fun_UPDT_as_mais_config_resolution(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_mais_status(const EOnv* nv);
extern void eoprot_fun_UPDT_as_mais_status(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_as_mais_status_the15values(const EOnv* nv);
extern void eoprot_fun_UPDT_as_mais_status_the15values(const EOnv* nv, const eOropdescriptor_t* rd);



/** @}            
    end of group eo_EoProtocolAS  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


