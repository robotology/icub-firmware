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
#ifndef _EOPROTOCOLEPSK_H_
#define _EOPROTOCOLEPSK_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolEPsk.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolEPsk Protocol for skin endpoint 
    Ivreververv e
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocolCommon.h"
#include "EoProtocolEP.h"
#include "EoSkin.h"
#include "EOnv.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef eOsk_entity_t eOprotEntityEPsk_t
    @brief      It contains the entities in endpoint skin: skin and mais
 **/
typedef eOsk_entity_t eOprotEntityEPsk_t;

enum { eoprot_ep_sk_entities_numberof = eosk_entities_numberof };


// - definition of the skin


/** @typedef    typedef enum eOprot_ep_sk_skin_tag_t
    @brief      It contains the tags for all variables in the skin endpoints.
                See definition of eOsk_skin_t (and its fields) in file EoSkin.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_sk_skin_tag_config__sigmode                   = 0,
    eoprot_ep_sk_skin_tag_status__arrayof10canframes        = 1
} eOprot_ep_sk_skin_tag_t;

enum { eoprot_ep_sk_skin_tags_numberof = 2 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_ep_sk_skin_rwmode_t
    @brief      It contains the function and type for all the network variables. There must be a one-to-one
                correspondence to the values in eOprot_ep_sk_skin_tag_t.
 **/
typedef enum
{
    eoprot_ep_sk_skin_rwmode_config__sigmode                = eo_nv_rwmode_RW,
    eoprot_ep_sk_skin_rwmode_status__arrayof10canframes     = eo_nv_rwmode_RO
} eOprot_ep_sk_skin_rwmode_t; 

enum { eoprot_ep_sk_skin_rwmodes_numberof = 2 };  // it MUST be equal to the number of tags. 


// - structures implementing the endpoints
  

/** @typedef    typedef struct eOprot_ep_sk_template_t;
    @brief      it is a template for the organisation of the entities in the endpoint skin
 **/
typedef struct                  // 176+0 = 176              
{
    eOsk_skin_t                 skin;
} eOprot_ep_sk_template_t;      EO_VERIFYsizeof(eOprot_ep_sk_template_t, 176);



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - these three are used to configure the endpoint into a EOnvsCfg object
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


#if 0   // dynamic mode
extern eOresult_t eoprot_ep_sk_number_of_boards_Load(uint16_t numofboards);
#endif

/** @fn         extern eOresult_t eoprot_ep_sk_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity)
    @brief      This function loads the maximum number of entities managed by the endpoint ...
    @param      brd                     the board
    @param      numberofeachentity      array of the values.
    @return     the number of variables.
  */
extern eOresult_t eoprot_ep_sk_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity);


/** @fn         extern eObool_t eoprot_ep_sk_variables_id_isvalid(eOprotBRD_t brd, eOnvID_t id)
    @brief      This function tells if an id belongs to the board
    @param      brd                     the board
    @param      id                      the id
    @return     eobool_true if the ID belongs to the board, eobool_false if not.
  */
extern eObool_t eoprot_ep_sk_variables_id_isvalid(eOprotBRD_t brd, eOnvID_t id);


/** @fn         extern uint16_t eoprot_ep_sk_variable_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of variables given the board
    @param      brd                     the board
    @return     the number of variables.
  */
extern uint16_t eoprot_ep_sk_variables_numberof_Get(eOprotBRD_t brd);


/** @fn         extern eOprotID_t eoprot_ep_sk_variable_idfromprognumber_Get(eOprotBRD_t brd, uint16_t prog)
    @brief      This function retrieves the id of a variable from the progressive number given the board
    @param      brd                     the board
    @param      prog                    the progressive number
    @return     the ID or EOK_uint16dummy if invalid .
  */
extern eOprotID_t eoprot_ep_sk_variable_idfromprognumber_Get(eOprotBRD_t brd, uint16_t prog);


/** @fn         extern uint16_t eoprot_ep_sk_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID_t id)
    @brief      This function retrieves the progressive number of a variable given the board and the @e id
    @param      brd                     the board
    @param      id                      the ID
    @return     the progressive number or EOK_uint16dummy if invalid .
  */
extern uint16_t eoprot_ep_sk_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID_t id);


/** @fn         extern uint16_t eoprot_ep_sk_skins_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of skins entities given the board
    @param      brd                     the board
    @return     the number of skins.
  */
extern uint16_t eoprot_ep_sk_skins_numberof_Get(eOprotBRD_t brd);

extern uint16_t eoprot_ep_sk_ram_sizeof_Get(eOprotBRD_t brd);

extern uint16_t eoprot_ep_sk_variable_ram_sizeof_Get(eOprotID_t id);

extern void* eoprot_ep_sk_variable_ram_Extract(void* epram, eOprotBRD_t brd, eOprotID_t id);

extern void* eoprot_ep_sk_variable_rom_Get(eOprotID_t id);

extern const eOsk_skin_t* eoprot_ep_sk_skin_default_Get(void);



/** @}            
    end of group eo_EoProtocolEPsk  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


