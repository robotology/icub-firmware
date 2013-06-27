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
#ifndef _EOPROTOCOLEP_H_
#define _EOPROTOCOLEP_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolEP.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolEP protocol of ... 
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocolCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 



/** @typedef    typedef enum eOprot_endpoint_t;
    @brief      It contains all the endpoints used in iCub. The regularity in the values helps to speed-up 
                the encoding/decoding process (e.g., by using rom-based tables or cpu-efficient bitwise arithmetic).
 **/
typedef enum
{
    eoprot_endpoint_management          = 0x0001,   /**< management of the ems board: comm+appl */   
    eoprot_endpoint_motioncontrol       = 0x0011,   /**< management of motion control  */        
    eoprot_endpoint_analogsensors       = 0x0021,   /**< management of analog sensors  */ 
    eoprot_endpoint_skin                = 0x0031,   /**< management of skin  */     
    eoprot_endpoint_none                = EOK_uint16dummy
} eOprot_endpoint_t;

enum { eoprot_endpoints_numberof = 4 }; // it does not count the eoprot_endpoint_none.

#if 0
/** @typedef    typedef struct eOprot_ep_mc_variables1_t;
    @brief      it contains all variables in the motion control endpoint.
 **/
typedef struct                  // 4+4 = 8              
{
    void**                      entities;           // pointer to an array containing pointers to the arrays of the entities
    uint8_t                     pool[4];            // the memory pool for the entities. 
} eOprot_ep_thevariables_t;     EO_VERIFYsizeof(eOprot_ep_thevariables_t, 8);


/** @typedef    typedef struct eOprot_ep_mc_variableS_t;
    @brief      it contains all variables in the motion control endpoint.
 **/
typedef struct                  // 4+4 = 8              
{
    uint8_t                     numofentitytypes;           // in mc its value is 3: joint, motor, controller
    uint8_t                     filler[3];
    uint16_t*                   sizeofentity;               // in mc it is: [sizeof(eOmc_joint_t), sizeof(motor), sizeof(controller)]
    uint8_t*                    numofentities;              // in mc board 1 it is: [4, 4, 1].
    void**                      entities;                   // to address joint 1, config: ((eOmc_joint_t**) entities)[1]->config
                                                            // it also works with the ... ?
    uint8_t                     pool[8];            // the memory pool for the entities. 
} eOprot_ep_thevariableS_t;     EO_VERIFYsizeof(eOprot_ep_thevariableS_t, 24);

#endif

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern uint16_t eoprot_ep_variables_numberof_get(eOprotBRD_t brd, eOprotEP_t ep)
    @brief      it tells how many variables are inside a given endpoint in a given board
    @param      brd             The board
    @param      ep              The endpoint.
    @return     the total number or EOK_uint16dummy if the endpoint in invalid.
 **/
extern uint16_t eoprot_ep_variables_numberof_get(eOprotBRD_t brd, eOprotEP_t ep); 


/** @fn         extern eOprotID_t eoprot_ep_variable_ID_get(eOprotEP_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOprotTag_t tag)
    @brief      it retrieves the endpoint of a variable in the endpoint given a triple (entity, index, tag).
    @param      ep              the endpoint.
    @param      entity          the entity. Use relevant values for the enum of the specific endpoint: eOprotEntityEPas_t, 
                                eOprotEntityEPmc_t, eOprotEntityEPmn_t, or eOprotEntityEPsk_t
    @param      index           the index of the entity (from 0 to eoprot_indices_numberof-1).  
    @param      tag             the tag of the variable of the index-th entity (from 0 to eoprot_tags_numberof-1).
    @return     the variable ID or EOK_uint16dummy if there is no variable defined by the triple (entity, index, tag) in that endpoint.
 **/
extern eOprotID_t eoprot_ep_variable_ID_get(eOprotEP_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOprotTag_t tag); 


/** @fn         extern eOprotProgNumber_t eoprot_ep_variable_ID2prognumber(eOprotBRD_t brd, eOprotEP_t ep, eOprotID_t id)
    @brief      it extract the progressive number of the variable of a given ID inside the endpoint.
    @param      brd             The board
    @param      ep              the endpoint.
    @param      id              the identifier of the variable.
    @return     the progressive number of the variable or EOK_uint16dummy in case of invalid parameters.
 **/
extern eOprotProgNumber_t eoprot_ep_variable_ID2prognumber(eOprotBRD_t brd, eOprotEP_t ep, eOprotID_t id); 


/** @fn         extern eOprotEntity_t eoprot_ep_variable_ID2entity(eOprotEP_t ep, eOprotID_t id)
    @brief      it extract the entity of the ID of a given endpoint.
    @param      ep              the endpoint.
    @param      id              the identified of the variable.
    @return     the entity or EOK_uint16dummy in case of invalid parameters.
 **/
extern eOprotEntity_t eoprot_ep_variable_ID2entity(eOprotEP_t ep, eOprotID_t id); 


/** @fn         extern eOprotIndex_t eoprot_ep_variable_ID2number(eOprotEP_t ep, eOprotID_t id)
    @brief      it extract the ordinal number of entity of the ID of a given endpoint.
    @param      ep              the endpoint.
    @param      id              the identified of the variable.
    @return     the index of the entity or EOK_uint16dummy in case of invalid parameters.
 **/
extern eOprotIndex_t eoprot_ep_variable_ID2index(eOprotEP_t ep, eOprotID_t id); 


/** @fn         extern eOprotTag_t eoprot_ep_variable_ID2tag(eOprotEP_t ep, eOprotID_t id)
    @brief      it extract the tag of the variable defined by the ID of a given endpoint.
    @param      ep              the endpoint.
    @param      id              the identified of the variable.
    @return     the tag of the entity or EOK_uint16dummy in case of invalid parameters.
 **/
extern eOprotTag_t eoprot_ep_variable_ID2tag(eOprotEP_t ep, eOprotID_t id); 


/** @fn         extern uint16_t eoprot_ep_ram_sizeof_get(eOprotEP_t ep)
    @brief      it tells the number of bytes required to contain all the variables of a given endpoint.
    @param      brd             The board
    @param      ep              the endpoint.
    @return     the size of the ram or EOK_uint16dummy in case of invalid parameters.
 **/
extern uint16_t eoprot_ep_ram_sizeof_get(eOprotBRD_t brd, eOprotEP_t ep); 


/** @fn         extern uint16_t eoprot_ep_variable_ram_sizeof_get(eOprotBRD_t brd, eOprotEP_t ep, eOprotID_t id)
    @brief      it tells the number of bytes required by a given variable of a given endpoint.
    @param      brd             The board
    @param      ep              the endpoint.
    @param      id              the identifier of the variable.
    @return     the size of the ram or EOK_uint16dummy in case of invalid parameters.
 **/
extern uint16_t eoprot_ep_variable_ram_sizeof_get(eOprotBRD_t brd, eOprotEP_t ep, eOprotID_t id); 


/** @fn         extern void* eoprot_ep_variable_ram_extract(void* epram, eOprotBRD_t brd, eOprotEP_t ep, eOprotID_t id)
    @brief      it returns the ram used by a given variables of a given endpoint staring from the total ram of the endpoint.
    @param      epram           The ram of the complete endpoint
    @param      brd             The board
    @param      ep              the endpoint.
    @param      id              the identifier of the variable.
    @return     pointer to the ram of the variable or NULL upon failure.
 **/
extern void* eoprot_ep_variable_ram_extract(void* epram, eOprotBRD_t brd, eOprotEP_t ep, eOprotID_t id); 


/** @fn         extern const void* eoprot_ep_variable_rom_get(eOprotEP_t ep, eOprotID_t id)
    @brief      it returns the rom pointer used as descriptor by a given variables of a given endpoint 
    @param      ep              the endpoint.
    @param      id              the identifier of the variable.
    @return     pointer to the rom descriptor (EOnv_rom_t*) of the variable or NULL upon failure.
 **/
extern const void* eoprot_ep_variable_rom_get(eOprotEP_t ep, eOprotID_t id);


/** @}            
    end of group eo_EoProtocolEP  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




