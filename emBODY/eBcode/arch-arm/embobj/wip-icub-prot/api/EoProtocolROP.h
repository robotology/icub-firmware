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
#ifndef _EOPROTOCOLROP_H_
#define _EOPROTOCOLROP_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolROP.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_protocolrop rop protocol of ... 
    In here there are cnm97tj9mn
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocolCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef uint8_t eOprot_ROP_ctrl_t
    @brief      defines the control field of the rop header. 
 **/
typedef uint8_t eOprot_ROP_ctrl_t;


/** @typedef    typedef  eOprot_ROP_code_t
    @brief      defines the rop code field of the rop header. 
 **/
typedef uint8_t eOprot_ROP_code_t;


/** @typedef    struct eOprot_ROP_head_t
    @brief      the eOprot_ROP_head_t describes how the ROP manipulates a variable identified on a given device (the one which
                receives the ROP) by the pair (ep, id). The operation is specified by the field ropcode.
                Some operations (such as: eoprot_ropcode_set, eoprot_ropcode_say, and eoprot_ropcode_sig) also requires the data
                field in the ROP, whose size is always contained in the field datasize.
                The control field specifies presence of other fields in the ROP (such as the signature or the time), requests that
                the answer must have such fields, or requests confirmation (ACK/NAK), or even tells if the ROP is a ACK/NAK.                  
 **/
typedef struct
{
    eOprot_ROP_ctrl_t   control;        /**< use eOprot_ROP_control_t values */
    eOprot_ROP_code_t   ropcode;        /**< use eOprot_ROP_code_value_t values */
    uint16_t            datasize;       /**< the true size of the data field */
    eOprotEP_t          ep;             /**< the endpoint of the variable */
    eOprotID_t          id;             /**< the identifier of the variable */
} eOprot_ROP_head_t;    EO_VERIFYsizeof(eOprot_ROP_head_t, 8);


/** @typedef    typedef struct eOprot_ROP_descriptor_t
    @brief      the eOprot_ROP_descriptor_t contains data structures used to represent a ROP. The ROP is used to remotely manipulate
                a variable in a manner described by the eOprot_ROP_head_t and, if present by the data field. Optional fields can be 
                found such as the signature and the time.
 **/
typedef struct
{
    eOprot_ROP_head_t   head;           /**< always present */
    uint32_t*           data;           /**< if present, its effective size is multiple of 4 bytes */
    uint32_t*           signature;      /**< if present, its size is 4 bytes */
    uint64_t*           time;           /**< if present, its size is 8 bytes */
} eOprot_ROP_descriptor_t;


/** @typedef    typedef struct eOprot_ROP_control_t
    @brief      the eOprot_ROP_control_t contains fields which shape the format of the ROP.
 **/
typedef struct
{
    uint8_t             confinfo    : 2;    /**< use eOprot_ROP_confinfo_t values */     
    uint8_t             plustime    : 1;    /**< the rop contains the time field */        
    uint8_t             plussign    : 1;    /**< the rop contains the signature field */ 
    uint8_t             rqsttime    : 1;    /**< the reply rop shall contains the time field */ 
    uint8_t             rqstconf    : 1;    /**< the receiver shall send a confirmation for this rop */ 
    uint8_t             version     : 2;    /**< always zero */ 
} eOprot_ROP_control_t; EO_VERIFYsizeof(eOprot_ROP_control_t, 1);


/** @typedef    enum eOprot_ROP_confinfo_t
    @brief      contains information about a ROP being a ACK, a NAK or neither of them.                  
 **/
typedef enum
{
    eo_ropconf_none     = 0,    /**< the ROP is not a confirmation ROP */ 
    eo_ropconf_nak      = 1,    /**< the ROP is a NAK */ 
    eo_ropconf_ack      = 2     /**< the ROP is a ACK */ 
} eOprot_ROP_confinfo_t;


/** @typedef    enum eOprot_ROP_code_value_t
    @brief      contains the codes of all possible operations which can be done on a variable.                  
 **/
typedef enum
{
    eoprot_ropcode_none = 0,    /**< not a ropcode */ 
    eoprot_ropcode_ask  = 1,    /**< the sender ask the receiver the value of a variable. the receiver must reply with a eoprot_ropcode_say */ 
    eoprot_ropcode_say  = 2,    /**< used to communicate the value of a variable in reply to a eoprot_ropcode_ask */     
    eoprot_ropcode_set  = 3,    /**< the receiver shall modify the value of the variable with the data field of the rop */ 
    eoprot_ropcode_sig  = 4,    /**< the sender spontaneously signal the value of a variable */
    eoprot_ropcode_rst  = 5     /**< the receiver shall modify the value of the variable with its default value (not contained in the rop) */ 
} eOprot_ROP_code_value_t;

   
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group eo_protocolrop  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




