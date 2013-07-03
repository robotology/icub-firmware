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
#ifndef _EOPROTOCOLEPAS_H_
#define _EOPROTOCOLEPAS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolEPas.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolEPas Protocol for analog sensors endpoint 
    Ivreververv e
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocolCommon.h"
#include "EoProtocolEP.h"
#include "EoAnalogSensors.h"
#include "EOnv.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef eOas_entity_t eOprotEntityEPas_t
    @brief      It contains the entities in endpoint analosensor: strain and mais
 **/
typedef eOas_entity_t eOprotEntityEPas_t;

enum { eoprot_ep_as_entities_numberof = eoas_entities_numberof };


// - definition of the strain


/** @typedef    typedef enum eOprot_ep_as_tag_strain_t
    @brief      It contains the tags for all variables in the analog sensors endpoints.
                See definition of eOas_strain_t (and its fields) in file EoAnalogSensors.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_as_tag_strain_wholeitem                                   = 0,
    eoprot_ep_as_tag_strain_config                                      = 1,
    eoprot_ep_as_tag_strain_config_mode                                 = 2,
    eoprot_ep_as_tag_strain_config_datarate                             = 3,
    eoprot_ep_as_tag_strain_config_signaloncefullscale                  = 4,
    eoprot_ep_as_tag_strain_status                                      = 5,
    eoprot_ep_as_tag_strain_status_fullscale                            = 6,
    eoprot_ep_as_tag_strain_status_calibratedvalues                     = 7,
    eoprot_ep_as_tag_strain_status_uncalibratedvalues                   = 8
} eOprot_ep_as_tag_strain_t;

enum { eoprot_ep_as_tags_strain_numberof = 9 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_ep_as_strain_rwmode_t
    @brief      It contains the function and type for all the network variables. There must be a one-to-one
                correspondence to the values in eOprot_ep_as_tag_strain_t.
 **/
typedef enum
{
    eoprot_ep_as_strain_rwmode_wholeitem                                = eo_nv_rwmode_RO,
    eoprot_ep_as_strain_rwmode_config                                   = eo_nv_rwmode_RW,
    eoprot_ep_as_strain_rwmode_config_mode                              = eo_nv_rwmode_RW,
    eoprot_ep_as_strain_rwmode_config_datarate                          = eo_nv_rwmode_RW,
    eoprot_ep_as_strain_rwmode_config_signaloncefullscale               = eo_nv_rwmode_RW,
    eoprot_ep_as_strain_rwmode_status                                   = eo_nv_rwmode_RO,
    eoprot_ep_as_strain_rwmode_status_fullscale                         = eo_nv_rwmode_RO,
    eoprot_ep_as_strain_rwmode_status_calibratedvalues                  = eo_nv_rwmode_RO,
    eoprot_ep_as_strain_rwmode_status_uncalibratedvalues                = eo_nv_rwmode_RO    
} eOprot_ep_as_strain_rwmode_t; 

enum { eoprot_ep_as_rwmodes_strain_numberof = 9 };  // it MUST be equal to the number of tags. 



// - definition of the mais
 

/** @typedef    typedef enum eOprot_ep_as_tag_mais_t
    @brief      It contains the tags for all variables in the analosensor .
                See definition of eOas_mais_t (and its fields) in file EoAnalogSensors.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_ep_as_tag_mais_wholeitem                                     = 0,
    eoprot_ep_as_tag_mais_config                                        = 1,
    eoprot_ep_as_tag_mais_config_mode                                   = 2,
    eoprot_ep_as_tag_mais_config_datarate                               = 3,
    eoprot_ep_as_tag_mais_config_resolution                             = 4,
    eoprot_ep_as_tag_mais_status                                        = 5,
    eoprot_ep_as_tag_mais_status_the15values                            = 6
} eOprot_ep_as_tag_mais_t;

enum { eoprot_ep_as_tags_mais_numberof = 7 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_ep_as_rwmode_mais_t
    @brief      It contains the function and type for all the network variables. There must be a one-to-one
                correspondence to the values in eOprot_ep_as_tag_mais_t.
 **/
typedef enum
{
    eoprot_ep_as_rwmode_mais_wholeitem                                  = eo_nv_rwmode_RO,
    eoprot_ep_as_rwmode_mais_config                                     = eo_nv_rwmode_RW,
    eoprot_ep_as_rwmode_mais_config_mode                                = eo_nv_rwmode_RW,
    eoprot_ep_as_rwmode_mais_config_datarate                            = eo_nv_rwmode_RW,
    eoprot_ep_as_rwmode_mais_config_resolution                          = eo_nv_rwmode_RW,
    eoprot_ep_as_rwmode_mais_status                                     = eo_nv_rwmode_RO,
    eoprot_ep_as_rwmode_mais_status_the15values                         = eo_nv_rwmode_RO
} eOprot_ep_as_rwmode_mais_t; 

enum { eoprot_ep_as_rwmodes_mais_numberof = 7 };  // it MUST be equal to the number of tags. 


// - structures implementing the endpoints

/** @typedef    typedef struct eOprot_ep_as_template_t;
    @brief      is a template for the organisation of strains and maises in the analog sensors endpoint.
 **/
typedef struct                  // 56*1+48*1 = 104              
{
    eOas_strain_t               strains[1]; 
    eOas_mais_t                 maises[1];
} eOprot_ep_as_template_t;      EO_VERIFYsizeof(eOprot_ep_as_template_t, 104);
  
  

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - these three are used to configure the endpoint into a EOnvsCfg object
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


#if 0   // dynamic mode
extern eOresult_t eoprot_ep_as_number_of_boards_Load(uint16_t numofboards);
#endif

/** @fn         extern eOresult_t eoprot_ep_as_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity)
    @brief      This function loads the maximum number of entities managed by the endpoint ...
    @param      brd                     the board
    @param      numberofeachentity      array of the values.
    @return     the number of variables.
  */
extern eOresult_t eoprot_ep_as_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity);

/** @fn         extern eObool_t eoprot_ep_as_variables_id_isvalid(eOprotBRD_t brd, eOnvID_t id)
    @brief      This function tells if an id belongs to the board
    @param      brd                     the board
    @param      id                      the id
    @return     eobool_true if the ID belongs to the board, eobool_false if not.
  */
extern eObool_t eoprot_ep_as_variables_id_isvalid(eOprotBRD_t brd, eOnvID_t id);


/** @fn         extern uint16_t eoprot_ep_as_variable_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of variables given the board
    @param      brd                     the board
    @return     the number of variables.
  */
extern uint16_t eoprot_ep_as_variables_numberof_Get(eOprotBRD_t brd);


/** @fn         extern eOprotID_t eoprot_ep_as_variable_idfromprognumber_Get(eOprotBRD_t brd, uint16_t prog)
    @brief      This function retrieves the id of a variable from the progressive number given the board
    @param      brd                     the board
    @param      prog                    the progressive number
    @return     the ID or EOK_uint16dummy if invalid .
  */
extern eOprotID_t eoprot_ep_as_variable_idfromprognumber_Get(eOprotBRD_t brd, uint16_t prog);


/** @fn         extern uint16_t eoprot_ep_as_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID_t id)
    @brief      This function retrieves the progressive number of a variable given the board and the @e id
    @param      brd                     the board
    @param      id                      the ID
    @return     the progressive number or EOK_uint16dummy if invalid .
  */
extern uint16_t eoprot_ep_as_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID_t id);


/** @fn         extern uint16_t eoprot_ep_as_strains_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of strains entities given the board
    @param      brd                     the board
    @return     the number of strains.
  */
extern uint16_t eoprot_ep_as_strains_numberof_Get(eOprotBRD_t brd);


/** @fn         extern uint16_t eoprot_ep_as_maises_numberof_Get(eOprotBRD_t brd)
    @brief      This function retrieves the number of maises given the board
    @param      brd                     the board
    @return     the number of maises.
  */
extern uint16_t eoprot_ep_as_maises_numberof_Get(eOprotBRD_t brd);


extern uint16_t eoprot_ep_as_ram_sizeof_Get(eOprotBRD_t brd);

extern uint16_t eoprot_ep_as_variable_ram_sizeof_Get(eOprotID_t id);

extern void* eoprot_ep_as_variable_ram_Extract(void* epram, eOprotBRD_t brd, eOprotID_t id);

extern void* eoprot_ep_as_variable_rom_Get(eOprotID_t id);

extern const eOas_strain_t* eoprot_ep_as_strain_default_Get(void);

extern const eOas_mais_t* eoprot_ep_as_mais_default_Get(void);



/** @}            
    end of group eo_EoProtocolEPas  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


