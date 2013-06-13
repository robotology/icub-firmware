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

/** @defgroup eo_protocol_ep_mn Protocol for management endpoint 
    Ivreververv e
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocolCommon.h"
#include "EoProtocolEP.h"
#include "EoManagement.h"





// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum eOprot_endpoint_management_t;
    @brief      It contains all the endpoints used for management. 
 **/
typedef enum
{
    eoprot_ep_mn_emsboard           = eoprot_endpoint_mn_emsboard
} eOprot_endpoint_management_t;


enum { eoprot_endpoints_management_numberof = 1 };


/** @typedef    typedef eOmn_entity_t eOprotEntityEPmn_t
    @brief      It contains the entities in endpoint management: comm and appl
 **/
typedef eOmn_entity_t eOprotEntityEPmn_t;


// - definition of the comm: number of them in each endpoint, tag and funtyp values

typedef enum
{   
    eoprot_ep_mn_comms_numberof_emsboard        = 1 
} eOprot_ep_mn_comms_numberof_t;

  

/** @typedef    typedef enum eOprot_ep_mn_comm_tag_t
    @brief      It contains the tags for all variables in the management of the communication.
                See definition of eOmn_comm_t (and its fields) in file EoManagement.h for explanation of the variables.
 **/
typedef enum
{
     eoprot_ep_mn_comm_tag_cmmnds__ropsigcfg    = 0
} eOprot_ep_mn_comm_tag_t;

enum { eoprot_ep_mn_comm_tags_numberof = 1};  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_ep_mn_comm_funtyp_t
    @brief      It contains the function and type for all the network variables. There must be a one-to-one
                correspondence to the values in eOprot_ep_mn_comm_tag_t.
 **/
typedef enum
{
    eoprot_ep_mn_comm_funtyp_cmmnds__ropsigcfg  = EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_pkd)
} eOprot_ep_mn_comm_funtyp_t; 

enum { eoprot_ep_mn_comm_funtyps_numberof = 1};  // it MUST be equal to the number of tags. 


// - definition of the appl: number of them, tag and funtyp values

typedef enum
{   
    eoprot_ep_mn_appls_numberof_emsboard        = 1 
} eOprot_ep_mn_appls_numberof_t;

  

/** @typedef    typedef enum eOprot_ep_mn_appl_tag_t
    @brief      It contains the tags for all variables in the management of the application.
                See definition of eOmn_appl_t (and its fields) in file EoManagement.h for explanation of the variables.
 **/
typedef enum
{
     eoprot_ep_mn_appl_tag_config               = 0,
     eoprot_ep_mn_appl_tag_status               = 1,
     eoprot_ep_mn_appl_tag_cmmnds__go2state     = 2
} eOprot_ep_mn_appl_tag_t;

enum { eoprot_ep_mn_appl_tags_numberof = 3};  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_ep_mn_appl_funtyp_t
    @brief      It contains the function and type for all the network variables. There must be a one-to-one
                correspondence to the values in eOprot_ep_mn_appl_tag_t.
 **/
typedef enum
{
    eoprot_ep_mn_appl_funtyp_config             = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_mn_appl_funtyp_status             = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd),
    eoprot_ep_mn_appl_funtyp_cmmnds__go2state   = EO_nv_FUNTYP(eo_nv_FUN_beh, eo_nv_TYP_u08)
} eOprot_ep_mn_appl_funtyp_t; 

enum { eoprot_ep_mn_appl_funtyps_numberof = 3};  // it MUST be equal to the number of tags. 


// - structures implementing the endpoints
  
  
/** @typedef    typedef struct eOprot_ep_mn_emsboard_t;
    @brief      contains all the variables in the endpoint management for the ems board.
 **/
typedef struct                  // 136+24+0 = 160              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eOprot_ep_mn_emsboard_t;      EO_VERIFYsizeof(eOprot_ep_mn_emsboard_t, 160);  

  
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - these three are used to configure the endpoint into a EOnvsCfg object
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern uint16_t eoprot_ep_mn_variable_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of variables given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of variables.
  */
extern uint16_t eoprot_ep_mn_variables_numberof_Get(eOprotEP_t ep);


/** @fn         extern uint16_t eoprot_ep_mn_variable_progressivenumber_Get(eOprotEP_t ep)
    @brief      This function retrieves the progressive number of a variable given the endpoint @e ep and the @e id
    @param      ep              the endpoint
    @return     the progressive number or EOK_uint16dummy if invalid .
  */
extern uint16_t eoprot_ep_mn_variable_progressivenumber_Get(eOprotEP_t ep, eOprotID_t id);


/** @fn         extern uint16_t eoprot_ep_mn_comms_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of comms entities given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of comms.
  */
extern uint16_t eoprot_ep_mn_comms_numberof_Get(eOprotEP_t ep);


/** @fn         extern uint16_t eoprot_ep_mn_appls_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of motors given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of appls.
  */
extern uint16_t eoprot_ep_mn_appls_numberof_Get(eOprotEP_t ep);


extern uint16_t eoprot_ep_mn_ram_sizeof_Get(eOprotEP_t ep);

extern uint16_t eoprot_ep_mn_variable_ram_sizeof_Get(eOprotEP_t ep, eOprotID_t id);

extern void* eoprot_ep_mn_variable_ram_Extract(void* epram, eOprotEP_t ep, eOprotID_t id);

extern void* eoprot_ep_mn_variable_rom_Get(eOprotEP_t ep, eOprotID_t id);

extern const eOmn_comm_t* eoprot_ep_mn_comm_default_Get(void);

extern const eOmn_appl_t* eoprot_ep_mn_appl_default_Get(void);



/** @}            
    end of group eo_protocol_ep_mn  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


