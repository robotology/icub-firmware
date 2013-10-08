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

/** @defgroup eo_protocolendpoint protocol of ... 
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocolCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum eOprot_endpoint_category_t;
    @brief      It contains all the categories of endpoints. this type is used to assign the values to the endpoints.
 **/
typedef enum
{
    eoprot_ep_category_management       = 0,        /**< endpoints used to manage the application or the communication */
    eoprot_ep_category_motioncontrol    = 1,        /**< endpoints used to manage motion control */
    eoprot_ep_category_analogsensor     = 2,        /**< endpoints used to manage analog sensors */
    eoprot_ep_category_skin             = 3,        /**< endpoints used to manage the skin */
    eoprot_ep_category_none             = 255       /**< invalid category */
} eOprot_endpoint_category_t;

enum { eoprot_endpoint_categories_numberof = 4 }; // it does not count the eoprot_ep_category_none.

enum { eoprot_endpoint_maxnum_in_category = 16 }; 

/** @typedef    typedef enum eOprot_endpoint_base_t;
    @brief      It contains the base address for all categories of endpoint. this type is used to assign the values to the endpoints.
 **/
typedef enum
{
    eoprot_ep_base_mn                   = eoprot_ep_category_management*eoprot_endpoint_maxnum_in_category,     /**< 0x0000 start of management endpoints */
    eoprot_ep_base_mc                   = eoprot_ep_category_motioncontrol*eoprot_endpoint_maxnum_in_category,  /**< 0x0010 start of motion control endpoints */
    eoprot_ep_base_as                   = eoprot_ep_category_analogsensor*eoprot_endpoint_maxnum_in_category,   /**< 0x0020 start of analog sensor endpoints */
    eoprot_ep_base_sk                   = eoprot_ep_category_skin*eoprot_endpoint_maxnum_in_category            /**< 0x0030 start of skin endpoints */
} eOprot_endpoint_base_t;


/** @typedef    typedef enum eOprot_endpoint_offset_t;
    @brief      It contains the offsets used to compute the endpoint values. 
 **/
typedef enum
{
    eoprot_ep_offset_emsboard           = 1,
    eoprot_ep_offset_comm               = 2,
    eoprot_ep_offset_appl               = 3,
    eoprot_ep_offset_errors             = 4,
    
    eoprot_ep_offset_leftupperarm       = 1,
    eoprot_ep_offset_leftlowerarm       = 2,
    eoprot_ep_offset_rightupperarm      = 3,
    eoprot_ep_offset_rightlowerarm      = 4,
    
    eoprot_ep_offset_torso              = 5,
    
    eoprot_ep_offset_leftupperleg       = 6,
    eoprot_ep_offset_leftlowerleg       = 7,
    eoprot_ep_offset_rightupperleg      = 8,
    eoprot_ep_offset_rightlowerleg      = 9   
} eOprot_endpoint_offset_t;

enum { eoprot_endpoint_offset_highestvalue = 9 }; 


/** @typedef    typedef enum eOprot_endpoint_t;
    @brief      It contains all the endpoints used in iCub. The regularity in the values helps to speed-up 
                the encoding/decoding process (e.g., by using rom-based tables or cpu-efficient bitwise arithmetic).
 **/
typedef enum
{
    eoprot_endpoint_mn_emsboard         = eoprot_ep_base_mn+eoprot_ep_offset_emsboard,      /**< = 0x0001, management of the ems board: comm+appl */   
    eoprot_endpoint_mn_comm             = eoprot_ep_base_mn+eoprot_ep_offset_comm,          /**< = 0x0002, management of communication */   
    eoprot_endpoint_mn_appl             = eoprot_ep_base_mn+eoprot_ep_offset_appl,          /**< = 0x0003, management of application */ 
    eoprot_endpoint_mn_errors           = eoprot_ep_base_mn+eoprot_ep_offset_errors,        /**< = 0x0004, management of errors */       
    
    eoprot_endpoint_mc_leftupperarm     = eoprot_ep_base_mc+eoprot_ep_offset_leftupperarm,  /**< = 0x0011, management of motion control in left upper arm */    
    eoprot_endpoint_mc_leftlowerarm     = eoprot_ep_base_mc+eoprot_ep_offset_leftlowerarm,  /**< = 0x0012, management of motion control in left lower arm */ 
    
    eoprot_endpoint_mc_rightupperarm    = eoprot_ep_base_mc+eoprot_ep_offset_rightupperarm, /**< = 0x0013, management of motion control in right upper arm */    
    eoprot_endpoint_mc_rightlowerarm    = eoprot_ep_base_mc+eoprot_ep_offset_rightlowerarm, /**< = 0x0014, management of motion control in right lower arm */  

    eoprot_endpoint_mc_torso            = eoprot_ep_base_mc+eoprot_ep_offset_torso,         /**< = 0x0015, management of motion control in torso */

    eoprot_endpoint_mc_leftupperleg     = eoprot_ep_base_mc+eoprot_ep_offset_leftupperleg,  /**< = 0x0016, management of motion control in left upper leg */   
    eoprot_endpoint_mc_leftlowerleg     = eoprot_ep_base_mc+eoprot_ep_offset_leftlowerleg,  /**< = 0x0017, management of motion control in left lower leg */
    
    eoprot_endpoint_mc_rightupperleg    = eoprot_ep_base_mc+eoprot_ep_offset_rightupperleg, /**< = 0x0018, management of motion control in right upper leg */  
    eoprot_endpoint_mc_rightlowerleg    = eoprot_ep_base_mc+eoprot_ep_offset_rightlowerleg, /**< = 0x0019, management of motion control in right lower leg */
    
    eoprot_endpoint_as_leftupperarm     = eoprot_ep_base_as+eoprot_ep_offset_leftupperarm,  /**< = 0x0021, management of analog sensors in left upper arm */ 
    eoprot_endpoint_as_leftlowerarm     = eoprot_ep_base_as+eoprot_ep_offset_leftlowerarm,  /**< = 0x0022, management of analog sensors in left lower arm */  
    
    eoprot_endpoint_as_rightupperarm    = eoprot_ep_base_as+eoprot_ep_offset_rightupperarm, /**< = 0x0023, management of analog sensors in right upper arm */  
    eoprot_endpoint_as_rightlowerarm    = eoprot_ep_base_as+eoprot_ep_offset_rightlowerarm, /**< = 0x0024, management of analog sensors in right lower arm */   

    eoprot_endpoint_as_leftupperleg     = eoprot_ep_base_as+eoprot_ep_offset_leftupperleg,  /**< = 0x0026, management of analog sensors in left upper leg */     
    eoprot_endpoint_as_rightupperleg    = eoprot_ep_base_as+eoprot_ep_offset_rightupperleg, /**< = 0x0028, management of analog sensors in right upper leg */

    eoprot_endpoint_sk_leftlowerarm     = eoprot_ep_base_sk+eoprot_ep_offset_leftlowerarm,  /**< = 0x0032, management of skin in left lower arm */     
    eoprot_endpoint_sk_rightlowerarm    = eoprot_ep_base_sk+eoprot_ep_offset_rightlowerarm, /**< = 0x0034, management of skin in right lower arm */          

    eoprot_endpoint_none                = EOK_uint16dummy
} eOprot_endpoint_t;

enum { eoprot_endpoints_numberof = 21 }; // it does not count the eoprot_endpoint_none.

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern eOprot_endpoint_category_t eoprot_ep_category_get(eOprotEP_t ep)
    @brief      it tells which category the endpoint belongs to.
    @param      ep              The endpoint.
    @return     the category.
 **/
extern eOprot_endpoint_category_t eoprot_ep_category_get(eOprotEP_t ep);


/** @fn         extern uint16_t eoprot_ep_variables_numberof_get(eOprotEP_t ep)
    @brief      it tells how many variables are inside a given endpoint.
    @param      ep              The endpoint.
    @return     the total number or EOK_uint16dummy if the endpoint in invalid.
 **/
extern uint16_t eoprot_ep_variables_numberof_get(eOprotEP_t ep); 


/** @fn         extern eOprotID_t eoprot_ep_variable_ID_get(eOprotEP_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOprotTag_t tag)
    @brief      it retrieves the endpoint of a variable in the endpoint given a triple (entity, index, tag).
    @param      ep              the endpoint.
    @param      entity          the entity. Use relevant definition in the specific endpoint.
    @param      index           the index of the entity.  
    @param      tag             the tag of the variable of the index-th entity. 
    @return     the variable ID or EOK_uint16dummy if there is no variable defined by the triple (entity, index, tag) in that endpoint.
 **/
extern eOprotID_t eoprot_ep_variable_ID_get(eOprotEP_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOprotTag_t tag); 


/** @fn         extern eOprotProgNumber_t eoprot_ep_variable_ID2prognumber(eOprotEP_t ep, eOprotID_t id)
    @brief      it extract the progressive number of the variable of a given ID inside the endpoint.
    @param      ep              the endpoint.
    @param      id              the identifier of the variable.
    @return     the progressive number of the variable or EOK_uint16dummy in case of invalid parameters.
 **/
extern eOprotProgNumber_t eoprot_ep_variable_ID2prognumber(eOprotEP_t ep, eOprotID_t id); 


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
    @param      ep              the endpoint.
    @return     the size of the ram or EOK_uint16dummy in case of invalid parameters.
 **/
extern uint16_t eoprot_ep_ram_sizeof_get(eOprotEP_t ep); 


/** @fn         extern uint16_t eoprot_ep_variable_ram_sizeof_get(eOprotEP_t ep, eOprotID_t id)
    @brief      it tells the number of bytes required by a given variable of a given endpoint.
    @param      ep              the endpoint.
    @param      id              the identifier of the variable.
    @return     the size of the ram or EOK_uint16dummy in case of invalid parameters.
 **/
extern uint16_t eoprot_ep_variable_ram_sizeof_get(eOprotEP_t ep, eOprotID_t id); 


/** @fn         extern void* eoprot_ep_variable_ram_extract(void* epram, eOprotEP_t ep, eOprotID_t id)
    @brief      it returns the ram used by a given variables of a given endpoint staring from the total ram of the endpoint.
    @param      ep              the endpoint.
    @param      id              the identifier of the variable.
    @return     pointer to the ram of the variable or NULL upon failure.
 **/
extern void* eoprot_ep_variable_ram_extract(void* epram, eOprotEP_t ep, eOprotID_t id); 


/** @fn         extern const void* eoprot_ep_variable_rom_get(eOprotEP_t ep, eOprotID_t id)
    @brief      it returns the rom pointer used as descriptor by a given variables of a given endpoint 
    @param      ep              the endpoint.
    @param      id              the identifier of the variable.
    @return     pointer to the rom descriptor of the variable or NULL upon failure.
 **/
extern const void* eoprot_ep_variable_rom_get(eOprotEP_t ep, eOprotID_t id);


/** @}            
    end of group eo_protocolendpoint  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




