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
#ifndef _EOPROTOCOLEPMN_H_
#define _EOPROTOCOLEPMN_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolEPmn.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolEPmn Protocol for management endpoint 
    Ivreververv e
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocolCommon.h"
#include "EoProtocolEP.h"
#include "EoManagement.h"
#include "EOnv.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef eOmn_entity_t eOprotEntityEPmn_t
    @brief      It contains the entities in endpoint management: comm and appl
 **/
typedef eOmn_entity_t eOprotEntityEPmn_t;

enum { eoprot_ep_mn_entities_numberof = eomn_entities_numberof };


// - definition of the comm


/** @typedef    typedef enum eOprot_ep_mn_tag_comm_t
    @brief      It contains the tags for all variables in the management of the communication.
                See definition of eOmn_comm_t (and its fields) in file EoManagement.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mn_tag_comm_wholeitem                                     = 0,    
    eoprot_ep_mn_tag_comm_cmmnds_ropsigcfg                              = 1
} eOprot_ep_mn_tag_comm_t;

enum { eoprot_ep_mn_tags_comm_numberof = 2 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_ep_mn_rwmode_comm_t
    @brief      It contains the rw mode for all the network variables. There must be a one-to-one
                correspondence to the values in eOprot_ep_mn_tag_comm_t.
 **/
typedef enum
{
    eoprot_ep_mn_rwmode_comm_wholeitem                                  = eo_nv_rwmode_RO,
    eoprot_ep_mn_rwmode_comm_cmmnds_ropsigcfg                           = eo_nv_rwmode_RW
} eOprot_ep_mn_rwmode_comm_t; 

enum { eoprot_ep_mn_rwmodes_comm_numberof = 2 };  // it MUST be equal to the number of tags. 



// - definition of the appl
  

/** @typedef    typedef enum eOprot_ep_mn_tag_appl_t
    @brief      It contains the tags for all variables in the management of the application.
                See definition of eOmn_appl_t (and its fields) in file EoManagement.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_mn_tag_appl_wholeitem                                     = 0,
    eoprot_ep_mn_tag_appl_config                                        = 1,
    eoprot_ep_mn_tag_appl_status                                        = 2,
    eoprot_ep_mn_tag_appl_cmmnds_go2state                               = 3
} eOprot_ep_mn_tag_appl_t;

enum { eoprot_ep_mn_tags_appl_numberof = 4 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_ep_mn_rwmode_appl_t
    @brief      It contains the rw mode for all the network variables. There must be a one-to-one
                correspondence to the values in eOprot_ep_mn_tag_appl_t.
 **/
typedef enum
{
    eoprot_ep_mn_rwmode_appl_wholeitem                                  = eo_nv_rwmode_RO,
    eoprot_ep_mn_rwmode_appl_config                                     = eo_nv_rwmode_RW,
    eoprot_ep_mn_rwmode_appl_status                                     = eo_nv_rwmode_RO,
    eoprot_ep_mn_rwmode_appl_cmmnds_go2state                            = eo_nv_rwmode_RW
} eOprot_ep_mn_rwmode_appl_t; 

enum { eoprot_ep_mn_rwmodes_appl_numberof = 4 };  // it MUST be equal to the number of tags. 


// - structures implementing the endpoints
  
  
/** @typedef    typedef struct eOprot_ep_mn_template_t;
    @brief      it is a template for the organisation of the entities in the endpoint management.
 **/
typedef struct                  // 104+24+0 = 128              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eOprot_ep_mn_template_t;      EO_VERIFYsizeof(eOprot_ep_mn_template_t, 128);  

  
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - these three are used to configure the endpoint into a EOnvsCfg object
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


#if 0   // dynamic mode
extern eOresult_t eoprot_ep_mn_number_of_boards_Load(uint16_t numofboards);
#endif

/** @fn         extern eOresult_t eoprot_ep_mn_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity)
    @brief      This function loads the maximum number of entities managed by the endpoint ...
    @param      brd                     the board
    @param      numberofeachentity      array of the values.
    @return     the number of variables.
  */
extern eOresult_t eoprot_ep_mn_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity);


/** @fn         extern eObool_t eoprot_ep_mn_variables_id_isvalid(eOprotBRD_t brd, eOnvID32_t id)
    @brief      This function tells if an id belongs to the board
    @param      brd                     the board
    @param      id                      the id
    @return     eobool_true if the ID belongs to the board, eobool_false if not.
  */
extern eObool_t eoprot_ep_mn_variables_id_isvalid(eOprotBRD_t brd, eOnvID32_t id);


/** @fn         extern uint16_t eoprot_ep_mn_variable_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of variables given the board
    @param      brd                     the board
    @return     the number of variables.
  */
extern uint16_t eoprot_ep_mn_variables_numberof_Get(eOprotBRD_t brd);

/** @fn         extern eOprotID32_t eoprot_ep_mn_variable_idfromprognumber_Get(eOprotBRD_t brd, eOprotPROGnum_t prog)
    @brief      This function retrieves the id of a variable from the progressive number given the board
    @param      brd                     the board
    @param      prog                    the progressive number
    @return     the ID or EOK_uint32dummy if invalid .
  */
extern eOprotID32_t eoprot_ep_mn_variable_idfromprognumber_Get(eOprotBRD_t brd, eOprotPROGnum_t prog);

/** @fn         extern uint16_t eoprot_ep_mn_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID32_t id)
    @brief      This function retrieves the progressive number of a variable given the board and the @e id
    @param      brd                     the board
    @param      id                      the ID
    @return     the progressive number or EOK_uint32dummy if invalid .
  */
extern eOprotPROGnum_t eoprot_ep_mn_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID32_t id);


/** @fn         extern uint16_t eoprot_ep_mn_comms_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of comms entities given the board
    @param      brd                     the board
    @return     the number of comms.
  */
extern uint16_t eoprot_ep_mn_comms_numberof_Get(eOprotBRD_t brd);


/** @fn         extern uint16_t eoprot_ep_mn_appls_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of appls given the board
    @param      brd                     the board
    @return     the number of appls.
  */
extern uint16_t eoprot_ep_mn_appls_numberof_Get(eOprotBRD_t brd);


extern uint16_t eoprot_ep_mn_ram_sizeof_Get(eOprotBRD_t brd);

extern void* eoprot_ep_mn_entity_ram_Extract(eOprotBRD_t brd, eOprotEntity_t ent, void* epram);

extern uint16_t eoprot_ep_mn_variable_ram_sizeof_Get(eOprotID32_t id);

extern void* eoprot_ep_mn_variable_ram_Extract(eOprotBRD_t brd, eOprotID32_t id, void* epram);

extern void* eoprot_ep_mn_variable_rom_Get(eOprotID32_t id);

extern const eOmn_comm_t* eoprot_ep_mn_comm_default_Get(void);

extern const eOmn_appl_t* eoprot_ep_mn_appl_default_Get(void);



/** @}            
    end of group eo_EoProtocolEPmn  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


