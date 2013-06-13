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

/** @defgroup eo_protocol_ep_sk Protocol for analog sensors endpoint 
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


/** @typedef    typedef enum eOprot_endpoint_skin_t;
    @brief      It contains all the endpoints used for skin. 
 **/
typedef enum
{
    eoprot_ep_sk_leftlowerarm       = eoprot_endpoint_sk_leftlowerarm,
    eoprot_ep_sk_rightlowerarm      = eoprot_endpoint_sk_rightlowerarm
} eOprot_endpoint_skin_t;


enum { eoprot_endpoints_skin_numberof = 2 };


/** @typedef    typedef eOsk_entity_t eOprotEntityEPsk_t
    @brief      It contains the entities in endpoint skin: strain and mais
 **/
typedef eOsk_entity_t eOprotEntityEPsk_t;


// - definition of the strain: number of them in each endpoint, tag and funtyp values

typedef enum
{   
    eoprot_ep_sk_strains_numberof_leftlowerarm          = 0,
    eoprot_ep_sk_strains_numberof_rightlowerarm         = 0
} eOprot_ep_sk_strains_numberof_t;

  

/** @typedef    typedef enum eOprot_ep_sk_strain_tag_t
    @brief      It contains the tags for all variables in the skin endpoints.
                See definition of eOsk_strain_t (and its fields) in file EoSkin.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_sk_strain_tag_config                            = 0,
    eoprot_ep_sk_strain_tag_config__mode                      = 1,
    eoprot_ep_sk_strain_tag_config__datarate                  = 2,
    eoprot_ep_sk_strain_tag_config__signaloncefullscale       = 3,
    eoprot_ep_sk_strain_tag_status                            = 4,
    eoprot_ep_sk_strain_tag_status__fullscale                 = 5,
    eoprot_ep_sk_strain_tag_status__calibratedvalues          = 6,
    eoprot_ep_sk_strain_tag_status__uncalibratedvalues        = 7
} eOprot_ep_sk_strain_tag_t;

enum { eoprot_ep_sk_strain_tags_numberof = 8 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_ep_sk_strain_funtyp_t
    @brief      It contains the function and type for all the network variables. There must be a one-to-one
                correspondence to the values in eOprot_ep_sk_strain_tag_t.
 **/
typedef enum
{
    eoprot_ep_sk_strain_funtyp_config                         = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_sk_strain_funtyp_config__mode                   = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08),
    eoprot_ep_sk_strain_funtyp_config__datarate               = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08),
    eoprot_ep_sk_strain_funtyp_config__signaloncefullscale    = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08),
    eoprot_ep_sk_strain_funtyp_status                         = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd),
    eoprot_ep_sk_strain_funtyp_status__fullscale              = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_arr),
    eoprot_ep_sk_strain_funtyp_status__calibratedvalues       = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_arr),
    eoprot_ep_sk_strain_funtyp_status__uncalibratedvalues     = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_arr)    
} eOprot_ep_sk_strain_funtyp_t; 

enum { eoprot_ep_sk_strain_funtyps_numberof = 8 };  // it MUST be equal to the number of tags. 


// - definition of the mais: number of them, tag and funtyp values

typedef enum
{   
    eoprot_ep_sk_maises_numberof_leftupperarm          = 0,
    eoprot_ep_sk_maises_numberof_leftlowerarm          = 1,
    eoprot_ep_sk_maises_numberof_rightupperarm         = 0,
    eoprot_ep_sk_maises_numberof_rightlowerarm         = 1,
    eoprot_ep_sk_maises_numberof_leftupperleg          = 0,
    eoprot_ep_sk_maises_numberof_rightupperleg         = 0
} eOprot_ep_sk_maises_numberof_t;

  

/** @typedef    typedef enum eOprot_ep_sk_mais_tag_t
    @brief      It contains the tags for all variables in the skin .
                See definition of eOsk_mais_t (and its fields) in file EoSkin.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_sk_mais_tag_config                            = 0,
    eoprot_ep_sk_mais_tag_config__mode                      = 1,
    eoprot_ep_sk_mais_tag_config__datarate                  = 2,
    eoprot_ep_sk_mais_tag_config__resolution                = 3,
    eoprot_ep_sk_mais_tag_status                            = 4,
    eoprot_ep_sk_mais_tag_status__the15values               = 5
} eOprot_ep_sk_mais_tag_t;

enum { eoprot_ep_sk_mais_tags_numberof = 6 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_ep_sk_mais_funtyp_t
    @brief      It contains the function and type for all the network variables. There must be a one-to-one
                correspondence to the values in eOprot_ep_sk_mais_tag_t.
 **/
typedef enum
{
    eoprot_ep_sk_mais_funtyp_config                         = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd),
    eoprot_ep_sk_mais_funtyp_config__mode                   = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08),
    eoprot_ep_sk_mais_funtyp_config__datarate               = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08),
    eoprot_ep_sk_mais_funtyp_config__resolution             = EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_u08),
    eoprot_ep_sk_mais_funtyp_status                         = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_pkd),
    eoprot_ep_sk_mais_funtyp_status__the15values            = EO_nv_FUNTYP(eo_nv_FUN_inp, eo_nv_TYP_arr)
} eOprot_ep_sk_mais_funtyp_t; 

enum { eoprot_ep_sk_mais_funtyps_numberof = 6 };  // it MUST be equal to the number of tags. 


// - structures implementing the endpoints
  

/** @typedef    typedef struct eOprot_ep_sk_lowerarm_t;
    @brief      contains all the variables in the endpoint skin for the ems board.
 **/
typedef struct                  // 48+0 = 160              
{
    eOsk_mais_t                 mais;
} eOprot_ep_sk_lowerarm_t;  EO_VERIFYsizeof(eOprot_ep_sk_lowerarm_t, 48);


typedef eOprot_ep_sk_lowerarm_t eOprot_ep_sk_leftlowerarm_t;
typedef eOprot_ep_sk_lowerarm_t eOprot_ep_sk_rightlowerarm_t;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - these three are used to configure the endpoint into a EOnvsCfg object
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern uint16_t eoprot_ep_sk_variable_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of variables given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of variables.
  */
extern uint16_t eoprot_ep_sk_variables_numberof_Get(eOprotEP_t ep);


/** @fn         extern uint16_t eoprot_ep_sk_variable_progressivenumber_Get(eOprotEP_t ep)
    @brief      This function retrieves the progressive number of a variable given the endpoint @e ep and the @e id
    @param      ep              the endpoint
    @return     the progressive number or EOK_uint16dummy if invalid .
  */
extern uint16_t eoprot_ep_sk_variable_progressivenumber_Get(eOprotEP_t ep, eOprotID_t id);


/** @fn         extern uint16_t eoprot_ep_sk_strains_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of comms entities given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of comms.
  */
extern uint16_t eoprot_ep_sk_strains_numberof_Get(eOprotEP_t ep);


/** @fn         extern uint16_t eoprot_ep_sk_maises_numberof_Get(eOprotEP_t ep)
    @brief      This function retrieves the number of motors given the endpoint @e ep
    @param      ep              the endpoint
    @return     the number of maiss.
  */
extern uint16_t eoprot_ep_sk_maises_numberof_Get(eOprotEP_t ep);


extern uint16_t eoprot_ep_sk_ram_sizeof_Get(eOprotEP_t ep);

extern uint16_t eoprot_ep_sk_variable_ram_sizeof_Get(eOprotEP_t ep, eOprotID_t id);

extern void* eoprot_ep_sk_variable_ram_Extract(void* epram, eOprotEP_t ep, eOprotID_t id);

extern void* eoprot_ep_sk_variable_rom_Get(eOprotEP_t ep, eOprotID_t id);

extern const eOsk_strain_t* eoprot_ep_sk_strain_default_Get(void);

extern const eOsk_mais_t* eoprot_ep_sk_mais_default_Get(void);



/** @}            
    end of group eo_protocol_ep_sk  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


