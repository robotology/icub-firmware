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
#ifndef _EOPROTOCOL_H_
#define _EOPROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocol.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocol ETH protocol for iCub 
    This file contains public interface for the iCub ETH protocol.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


#include "EoAnalogSensors.h"
#include "EoManagement.h"
#include "EoMotionControl.h"
#include "EoSkin.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


// - types used in definition of functions and for generic use


/** @typedef    typedef uint32_t eOprotIP_t
    @brief      used to identify a variable in a network, as the eOprotIP_t resembles the IP address of a device..
 **/
typedef uint32_t eOprotIP_t;


/** @typedef    typedef uint8_t eOprotBRD_t
    @brief      used as a short identifier of an IP address with values starting from 0 upto the max number of boards in the network
 **/
typedef uint8_t eOprotBRD_t;

enum { eoprot_boards_maxnumberof = 10 };    // this number forces static allocation of some data structure, thus keep it low

enum { eoprot_board_localboard = 254 };     // use this value to retrieve info about the board configured as local

/** @typedef    typedef uint32_t eOprotID32_t
    @brief      used to identify a variable inside a device. It can have holes in its representation
 **/
typedef uint32_t eOprotID32_t;


/** @typedef    uint32_t eOprotProgNumber_t
    @brief      it identifies a variable inside a device but it does not have holes in its representation.
 **/
typedef uint32_t eOprotProgNumber_t;


/** @typedef    typedef uint8_t eOprotEndpoint_t
    @brief      It is part of eOprotID32_t. The endpoint is used to group some variables of homogeneous type inside a given device. 
                for example it can be motion control in left lower arm. 
 **/
typedef uint8_t eOprotEndpoint_t;

enum { eoprot_endpoints_maxnumberof = 256 };  


/** @typedef    uint8_t eOprotEntity_t
    @brief      It is part of eOprotID32_t. it can be a joint or a motor in a motion control endpoint.
                It is used as a generic type for function arguments. You should use the relevant type of the endpoint
                in use: eOprotEntityEPas_t, eOprotEntityEPmc_t, eOprotEntityEPmn_t, or eOprotEntityEPsk_t
 **/
typedef uint8_t eOprotEntity_t;

enum { eoprot_entities_maxnumberof = 256 };  


/** @typedef    uint8_t eOprotIndex_t
    @brief      It is part of eOprotID32_t. It can be the index of a joint or a motor in a motion control endpoint.
 **/
typedef uint8_t eOprotIndex_t;

enum { eoprot_indices_maxnumberof = 256 }; 


/** @typedef    uint8_t eOprotTag_t
    @brief      It is part of eOprotID32_t. it can be associated to the PID of a joint, or to its measured position.
 **/
typedef uint8_t eOprotTag_t;

enum { eoprot_tags_maxnumberof = 256 };    // the current implementation uses at most 256 possible tags per entity of a given type per endpoint.


// - specific values and types

/** @typedef    typedef enum eOprot_endpoint_t;
    @brief      It contains all the endpoints used in iCub. The values could be assigned in any way, however,
                its regularity helps to speed-up the encoding/decoding process (e.g., by using rom-based tables
                or cpu-efficient bitwise arithmetic).
 **/
typedef enum
{
    eoprot_endpoint_management              = 0,        /**< management of the ems board: comm+appl */   
    eoprot_endpoint_motioncontrol           = 1,        /**< management of motion control  */        
    eoprot_endpoint_analogsensors           = 2,        /**< management of analog sensors  */ 
    eoprot_endpoint_skin                    = 3,        /**< management of skin  */     
    eoprot_endpoint_none                    = EOK_uint08dummy
} eOprot_endpoint_t;

enum { eoprot_endpoints_numberof = 4 }; // it does not count the eoprot_endpoint_none.


/** @typedef    typedef enum eOprot_entity_t;
    @brief      It contains all the entities used in iCub. The used values are coming from the relevant files.
                For instance, the value for eoprot_entity_mc_joint is eomc_entity_joint which is defined in
                file EoMotionControl.h.               
 **/
typedef enum
{
    eoprot_entity_mn_comm                   = eomn_entity_comm,         /**<  */   
    eoprot_entity_mn_appl                   = eomn_entity_appl,         /**<  */
    eoprot_entity_mc_joint                  = eomc_entity_joint,        /**<  */ 
    eoprot_entity_mc_motor                  = eomc_entity_motor,        /**<  */     
    eoprot_entity_mc_controller             = eomc_entity_controller,   /**<  */    
    eoprot_entity_as_strain                 = eoas_entity_strain,       /**<  */   
    eoprot_entity_as_mais                   = eoas_entity_mais,         /**<  */            
    eoprot_entity_as_extorque               = eoas_entity_extorque,     /**<  */  
    eoprot_entity_sk_skin                   = eosk_entity_skin,         /**<  */
    eoprot_entity_none                      = EOK_uint08dummy
} eOprot_entity_t;

enum { eoprot_entities_numberof = 9 }; // it does not count the eoprot_entity_none.


/** @typedef    typedef enum eOprot_index_t
    @brief      It describes the index of the entity.  It is simply a number from 0 to EOK_uint08dummy-1.                
 **/
typedef enum
{
    eoprot_index_0                          = 0,                /**< the first index */
    eoprot_index_1                          = 1,                /**< the second index */
    eoprot_index_254                        = 254,              /**< the last index */
    eoprot_index_none                       = EOK_uint08dummy   /**< the dummy index */
} eOprot_index_t;


/** @typedef    typedef enum eOprot_tag_t;
    @brief      It describes the name structure of the tags. The tags are defined in the header files of the
                relevant endpoint. For instance, the tag of the endpoint motion control which identifies
                the whole configuration of the entity joint is eoprot_tag_mc_joint_config and is defined
                inside file EoProtocolMC.h.                
 **/
typedef enum
{
    eoprot_tag_xx_nameofentity_nameoftag    = 0     /**< xx is the two-letter short for the entity: as, mn, mc, sk, etc. */
} eOprot_tag_t;



/** @typedef    typedef struct eOprot_nvset_interface_t;
    @brief      It contains those functions which are required to offer services to the EOnvset object. The functions in here must
                match those defined inside eOnvset_protocol_Interface_t. Every endpoint must export a variables of this type.    
 **/ 
typedef struct
{
    eOres_fp_uint8_uint8_voidp_uint16_t loadram;            /*< a function which loads the ram of the endpoint given: (brd, ep, ram, sizeof) */
    eOuint16_fp_uint8_uint8_t           getvarsnumberof;    /*< a function which returns the total number of variables given: (brd, ep) */
    eObool_fp_uint8_uint32_t            isidsupported;      /*< a function which tells if the id is supported given: (brd, id) */
    eOuint32_fp_uint8_uint8_uint32_t    getid;              /*< a function which returns the full ID given: (brd, ep, prognumber)  */
    eOuint32_fp_uint8_uint32_t          getprognumber;      /*< a function which returns a progressive number given: (brd, id) */
    eOvoidp_fp_uint8_uint32_t           getrom;             /*< a function which returns the .rom part of the NV given: (brd, id) */
    eOvoidp_fp_uint8_uint32_t           getram;             /*< a function which returns the .ram part of the NV given: (brd, id) */  
} eOprot_nvset_Interface_t;

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOprot_nvset_Interface_t eoprot_eonvset_Interface;


// - declaration of extern public functions ---------------------------------------------------------------------------

// functions which just makes conversione

/** @fn         extern eOprotID32_t eoprot_ID_get(eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOprotTag_t tag)
    @brief      it retrieves the ID of a variable given (ep, entity, index, tag). The ID is equal for any board.
    @param      ep              the endpoint. Use values from eOprot_endpoint_t.
    @param      entity          the entity. Use relevant values for the enum of the specific endpoint: eOprotEntityEPas_t, 
                                eOprotEntityEPmc_t, eOprotEntityEPmn_t, or eOprotEntityEPsk_t
    @param      index           the index of the entity (from 0).  
    @param      tag             the tag of the variable of the index-th entity (from 0 to eoprot_tags_numberof-1).
    @return     the variable ID or EOK_uint32dummy if there is no variable defined by the quadruple (ep, entity, index, tag).
 **/
extern eOprotID32_t eoprot_ID_get(eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOprotTag_t tag); 


/** @fn         extern eOprotEndpoint_t eoprot_ID2endpoint(eOprotID32_t id)
    @brief      it extracts the endpoint from the ID.
    @param      id              the identifier of the variable.
    @return     the entity or EOK_uint8dummy in case of invalid parameters.
 **/
extern eOprotEndpoint_t eoprot_ID2endpoint(eOprotID32_t id); 


/** @fn         extern eOprotEntity_t eoprot_ep_variable_ID2entity(eOprotID32_t id)
    @brief      it extracts the entity from the ID.
    @param      id              the identifier of the variable.
    @return     the entity or EOK_uint8dummy in case of invalid parameters.
 **/
extern eOprotEntity_t eoprot_ID2entity(eOprotID32_t id); 


/** @fn         extern eOprotIndex_t eoprot_ep_variable_ID2number(eOprotID32_t id)
    @brief      it extracts the ordinal number of entity from the ID.
    @param      id              the identifier of the variable.
    @return     the index of the entity or EOK_uint8dummy in case of invalid parameters.
 **/
extern eOprotIndex_t eoprot_ID2index(eOprotID32_t id); 


/** @fn         extern eOprotTag_t eoprot_ep_variable_ID2tag(eOprotID32_t id)
    @brief      it extract the tag of the variable from the ID.
    @param      id              the identifier of the variable.
    @return     the tag of the entity or EOK_uint8dummy in case of invalid parameters.
 **/
extern eOprotTag_t eoprot_ID2tag(eOprotID32_t id); 


// functions which allows to use the variables etc.


/** @fn         extern eOresult_t eoprot_config_board_numberof(uint8_t numofboards)
    @brief      it configure the library to use a given number of boards.
    @param      numofboards         the number of boards.
    @return     eores_OK or eores_NOK_generic upon failure.
 **/
extern eOresult_t eoprot_config_board_numberof(uint8_t numofboards);


/** @fn         extern eOresult_t eoprot_config_board_local(eOprotBRD_t brd)
    @brief      it configure the library to use a given number of boards.
    @param      brd             the number of board to be set as the local one.
    @return     eores_OK or eores_NOK_generic upon failure.
 **/
extern eOresult_t eoprot_config_board_local(eOprotBRD_t brd);


/** @fn         extern eOresult_t eoprot_config_endpoint_entities(eOprotBRD_t brd, eOprotEndpoint_t ep, const uint8_t* numberofentities)
    @brief      it configures the library to use a given number of entities for that endpoint on that board.
    @param      brd                 the number of board 
    @param      ep                  the endpoint
    @param      numberofentities    the number of entities expressed as a const array whose address will be copied and used in the
                                    entire lifetime of the library.    
    @return     eores_OK or eores_NOK_generic upon failure.
 **/
extern eOresult_t eoprot_config_endpoint_entities(eOprotBRD_t brd, eOprotEndpoint_t ep, const uint8_t* numberofentities);


/** @fn         extern uint16_t eoprot_endpoint_sizeof_get(eOprotBRD_t brd, eOprotEndpoint_t ep)
    @brief      it tells the size of the ram used for a given board and endpoint.
    @param      brd                 the number of board 
    @param      ep                  the endpoint
    @return     the size or 0 upon failure.
 **/
extern uint16_t eoprot_endpoint_sizeof_get(eOprotBRD_t brd, eOprotEndpoint_t ep);


/** @fn         extern eOresult_t eoprot_config_endpoint_ram(eOprotBRD_t brd, eOprotEndpoint_t ep, void* ram, uint16_t sizeofram)
    @brief      it configures the library to use some ram for that board and endpoint. The ram must be externally allocated and be
                of the correct dimension. One can use a sizeof(eOprot_bxx_endpointname_t) or use the function 
                eoprot_endpoint_sizeof_get().
    @param      brd                 the number of board 
    @param      ep                  the endpoint
    @param      ram                 the ram
    @param      sizeofram           the size of the ram.    
    @return     eores_OK or eores_NOK_generic upon failure (also if sizeofram is not coherent with brd and ep).
 **/
extern eOresult_t eoprot_config_endpoint_ram(eOprotBRD_t brd, eOprotEndpoint_t ep, void* ram, uint16_t sizeofram);


/** @fn         extern void* eoprot_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id)
    @brief      it gets the ram of the variable on a given (board, ID). The dependency from the board is necessary because
                for the same endpoint the number of entities may be different.
    @param      brd             the number of the board.
    @param      id              the identifier of the variable.
    @return     the ram of the variable or NULL in case of invalid parameters.
 **/
extern void* eoprot_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id);


/** @fn         extern uint16_t eoprot_variable_sizeof_get(eOprotBRD_t brd, eOprotID32_t id)
    @brief      it gets the size of the variable on a given (board, ID). The dependency from the board is necessary because
                for the same endpoint the number of entities may be different.
    @param      brd             the number of the board.
    @param      id              the identifier of the variable.
    @return     the size of the variable or 0 in case of invalid parameters.
 **/
extern uint16_t eoprot_variable_sizeof_get(eOprotBRD_t brd, eOprotID32_t id);


/** @fn         extern void* eoprot_entity_ramof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index)
    @brief      it gets the ram of the entity on a given (board, endpoint, entity). The dependency from the board is necessary because
                for the same endpoint the number of entities may be different.
    @param      brd             the number of the board.
    @param      ep              the endpoint.
    @param      entity          the entity.
    @param      index           the index.
    @return     the ram of the entity or NULL in case of invalid parameters.
 **/
extern void* eoprot_entity_ramof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index);


/** @fn         extern uint16_t eoprot_entity_sizeof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity)
    @brief      it gets the size of the entity on a given (board, endpoint, entity). The dependency from the board is necessary because
                for the same endpoint the number of entities may be different.
    @param      brd             the number of the board.
    @param      ep              the endpoint.
    @param      entity          the entity.
    @return     the size of the entity or 0 in case of invalid parameters.
 **/
extern uint16_t eoprot_entity_sizeof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity);


/** @fn         extern uint8_t eoprot_entity_numberof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity)
    @brief      it gets the number of the entities on a given (board, endpoint, entity). The dependency from the board is necessary because
                for the same endpoint the number of entities may be different..
    @param      brd             the number of the board.
    @param      ep              the endpoint.
    @param      entity          the entity.
    @return     the number of the entities or 0 in case of invalid parameters.
 **/
extern uint8_t eoprot_entity_numberof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity);


/** @fn         extern eObool_t eoprot_id_isvalid(eOprotBRD_t brd, eOnvID32_t id)
    @brief      it tells if a given ID is valid on that board.
    @param      brd             the number of the board.
    @param      id              the ID.
    @return     eobool_true or eobool_false.
 **/
extern eObool_t eoprot_id_isvalid(eOprotBRD_t brd, eOnvID32_t id);


/** @fn         extern eOprotID32_t eoprot_prognum2id(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotProgNumber_t prog)
    @brief      it converts the progressive number into the ID for a given endpoint on a given board 
    @param      brd             the number of the board.
    @param      ep              the endpoint.
    @param      prog            the progressive number.
    @return     the ID or EOK_uint32dummy in case of invalid parameters.
 **/
extern eOprotID32_t eoprot_prognum2id(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotProgNumber_t prog);

/** @fn         extern eOprotProgNumber_t eoprot_id2prognum(eOprotBRD_t brd, eOprotID32_t id)
    @brief      it converts the ID into the progressive number for a given board 
    @param      brd             the number of the board.
    @param      id              the ID.
    @return     the progressive number or EOK_uint32dummy in case of invalid parameters.
 **/
extern eOprotProgNumber_t eoprot_id2prognum(eOprotBRD_t brd, eOprotID32_t id);



/** @}            
    end of group eo_EoProtocol  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




