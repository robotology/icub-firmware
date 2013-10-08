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

EoProtocol.h:       include tutti ...

EoProtocolROP.h
EoProtocolROPframe.h
EoProtocolEndpoint.h
EoProtocolEndpointMC.h
EoProtocolEndpointAS.h
EoProtocolEndpointSK.h


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

/** @defgroup eo_protocol protocol of ... 
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

// -- definition of ip
typedef uint32_t eOprotIP_t;

// -- structure of a rop and of a ropframe

typedef uint16_t eOprotEP_t;
typedef uint16_t eOprotID_t;

typedef enum
{
    eo_ropconf_none     = 0,    /**< the ROP does not contain any confirmation info */ 
    eo_ropconf_nak      = 2,    /**< the ROP is a NAK */ 
    eo_ropconf_ack      = 3     /**< the ROP is a ACK */ 
} eOprot_ROP_confinfo_t;

typedef struct
{
    uint8_t             ffu         : 1;    /**< not used */ 
    uint8_t             confinfo    : 2;    /**< use eOprot_ROP_confinfo_t values */     
    uint8_t             plustime    : 1;
    uint8_t             plussign    : 1;
    uint8_t             rqsttime    : 1;
    uint8_t             rqstconf    : 1;
    uint8_t             userdefn    : 1;    /**< not used */ 
} eOprot_ROP_ctrl_t;    EO_VERIFYsizeof(eOprot_ROP_ctrl_t, 1);

typedef enum
{
    eoprot_ropcode_none = 0,
    eoprot_ropcode_ask  = 2,
    eoprot_ropcode_say  = 3,
    eoprot_ropcode_set  = 4,
    eoprot_ropcode_sig  = 5,
    eoprot_ropcode_rst  = 6
} eOprot_ROP_code_value_t;


typedef struct
{
    eOprot_ROP_ctrl_t   control;
    eOprot_ROP_code_t   ropcode;        /**< use eOprot_ROP_code_value_t values */
    eOprotEP_t          ep;
    eOprotID_t          id;
    uint16_t            datasize;       /**< the true size of the data field */
} eOprot_ROP_head_t;    EO_VERIFYsizeof(eOprot_ROP_head_t, 8);

typedef struct
{
    eOprot_ROP_head_t   head;           /**< always present */
    uint32_t*           data;           /**< if present, its effective size is multiple of 4 bytes */
    uint32_t*           signature;      /**< if present, its size is 4 bytes */
    uint64_t*           time;           /**< if present, its size is 8 bytes */
} eOprot_ROP_descriptor_t;


typedef struct  // 24 bytes
{
    uint32_t            startofframe;
    uint16_t            ropssizeof;
    uint16_t            ropsnumberof;
    uint64_t            ageofframe;
    uint64_t            sequencenumber;
} eOprot_ROPframe_head_t;   EO_VERIFYsizeof(eOprot_ROPframe_head_t, 24);

typedef struct
{
    eOprot_ROPframe_head_t  head;           /**< always present */
    uint32_t*               rops;           /**< if present, its effective size is multiple of 4 bytes */
    uint32_t*               footer;         /**< always present */
} eOprot_ROPframe_descriptor_t;


// -- the available endpoints 

typedef enum
{
    eoprot_ep_base_mn                     = 0x0000,           /**< start of management endpoints */
    eoprot_ep_base_mc                     = 0x0010,           /**< start of motion control endpoints */
    eoprot_ep_base_as                     = 0x0020,           /**< start of analog sensor endpoints */
    eoprot_ep_base_sk                     = 0x0030            /**< start of skin endpoints */
} eOprot_endpoint_base_t;

typedef enum
{
    eoprot_ep_offset_comm                          = 0x0001,
    eoprot_ep_offset_appl                          = 0x0002,
    eoprot_ep_offset_errors                        = 0x0003,
    
    eoprot_ep_offset_leftupperarm                  = 0x0001,
    eoprot_ep_offset_leftlowerarm                  = 0x0002,
    eoprot_ep_offset_rightupperarm                 = 0x0003,
    eoprot_ep_offset_rightlowerarm                 = 0x0004,
    
    eoprot_ep_offset_torso                         = 0x0005,
    
    eoprot_ep_offset_leftupperleg                  = 0x0006,
    eoprot_ep_offset_leftlowerleg                  = 0x0007,
    eoprot_ep_offset_rightupperleg                 = 0x0008,
    eoprot_ep_offset_rightlowerleg                 = 0x0009
    
} eOprot_endpoint_offset_t;



/** @typedef    typedef enum eOprot_endpoint_t;
    @brief      It contains all the endpoints used in iCub. The regularity in the values helps to speed-up 
                the encoding/decoding process (e.g., by using rom-based tables or cpu-efficient bitwise arithmetic).
 **/
typedef enum
{
    eoprot_endpoint_mn_comm                     = eoprot_ep_base_mn+eoprot_ep_offset_comm,   
    eoprot_endpoint_mn_appl                     = eoprot_ep_base_mn+eoprot_ep_offset_appl,   
    
    eoprot_endpoint_mc_leftupperarm             = eoprot_ep_base_mc+eoprot_ep_offset_leftupperarm,  
    eoprot_endpoint_mc_leftlowerarm             = eoprot_ep_base_mc+eoprot_ep_offset_leftlowerarm,  
    
    eoprot_endpoint_mc_rightupperarm            = eoprot_ep_base_mc+eoprot_ep_offset_rightupperarm,   
    eoprot_endpoint_mc_rightlowerarm            = eoprot_ep_base_mc+eoprot_ep_offset_rightlowerarm,  

    eoprot_endpoint_mc_torso                    = eoprot_ep_base_mc+eoprot_ep_offset_torso,   

    eoprot_endpoint_mc_leftupperleg             = eoprot_ep_base_mc+eoprot_ep_offset_leftupperleg,   
    eoprot_endpoint_mc_leftlowerleg             = eoprot_ep_base_mc+eoprot_ep_offset_leftlowerleg,  
    
    eoprot_endpoint_mc_rightupperleg            = eoprot_ep_base_mc+eoprot_ep_offset_rightupperleg,  
    eoprot_endpoint_mc_rightlowerleg            = eoprot_ep_base_mc+eoprot_ep_offset_rightlowerleg,
    
    eoprot_endpoint_as_leftupperarm             = eoprot_ep_base_as+eoprot_ep_offset_leftupperarm,  
    eoprot_endpoint_as_leftlowerarm             = eoprot_ep_base_as+eoprot_ep_offset_leftlowerarm,   
    
    eoprot_endpoint_as_rightupperarm            = eoprot_ep_base_as+eoprot_ep_offset_rightupperarm,   
    eoprot_endpoint_as_rightlowerarm            = eoprot_ep_base_ac+eoprot_ep_offset_rightlowerarm,   

    eoprot_endpoint_as_leftupperleg             = eoprot_ep_base_as+eoprot_ep_offset_leftupperleg,       
    eoprot_endpoint_as_rightupperleg            = eoprot_ep_base_as+eoprot_ep_offset_rightupperleg,

    eoprot_endpoint_sk_leftlowerarm             = eoprot_ep_base_sk+eoprot_ep_offset_leftlowerarm,     
    eoprot_endpoint_sk_rightlowerarm            = eoprot_ep_base_sk+eoprot_ep_offset_rightlowerarm          

} eOprot_endpoint_t;

enum { eoprot_endpoints_numberof = 19 };


// -- basic types used for ID preparation

typedef uint8_t eOprotEntity_t;
typedef uint8_t eOprotNumber_t;
typedef uint16_t eOprotIndex_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------

// it tells how many variables are inside an endpoint
extern uint16_t eoprot_ep_variable_numberofthem_get(eOprotEP_t ep); 

// it retrieves the variable ID inside a given endpoint. it requires some more parameters whose meaning depends
// on the endpoint. for example in motion control: entity can be a joint (or a motor), the number is
// the number of the joint, and the index is the descriptor of the variable used inside the joint.
// each endpoint can have different rules on how to assign the ID given the triple (entity, number, index). 
// however we could use a common strategy: the ID has 3 bits for entity, 5 bits for the number, and 8 bits for the index.
// we leave u8, u8, u16 in order to be able to increase to 9 bits the size of index.
extern eOprotID_t eoprot_ep_variable_ID_get(eOprotEP_t ep, eOprotEntity_t entity, eOprotNumber_t number, eOprotIndex_t index); 

extern eOprotEntity_t eoprot_ep_variable_ID2entity(eOprotEP_t ep, eOprotID_t id); 
extern eOprotNumber_t eoprot_ep_variable_ID2number(eOprotEP_t ep, eOprotID_t id); 
extern eOprotIndex_t eoprot_ep_variable_ID2index(eOprotEP_t ep, eOprotID_t id); 

// size of ram used by the entire endpoint
extern uint16_t eoprot_ep_ram_sizeof_get(eOprotEP_t ep); 

// size of ram used by the variable with (ep, id)
extern uint16_t eoprot_ep_variable_ram_sizeof_get(eOprotEP_t ep, eOprotID_t id); 

// extracts from the epram used by the entire endpoint the ram of the variable with (ep, id)  
extern void* eoprot_ep_variable_ram_extract(void* epram, eOprotEP_t ep, eOprotID_t id); 

/** @}            
    end of group eo_protocol  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




