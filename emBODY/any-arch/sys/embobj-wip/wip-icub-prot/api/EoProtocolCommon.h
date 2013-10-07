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
#ifndef _EOPROTOCOLCOMMON_H_
#define _EOPROTOCOLCOMMON_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolCommon.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_protocolcommon protocol of ... 
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct eOprotVAR_t
    @brief      the eOprotVAR_t contains data structures representing a variable in a device. It can be univoquely
                by a triple (ip, ep, id) and can be remotely manipulated by means of a remote operation, or ROP,
                which can set or reset its value, ask its value. the variable can say its value to response to an ask,
                or can spontaneously signal it.
 **/
typedef struct
{
    void*   implementation;
} eOprotVAR_t;


// si potrebbe cambiare il protocollo per avere un eOprotID32_t a 32 bit che abbia come sottocampi ognuno a 8 bit: ep, entity, index, tag


/** @typedef    typedef uint32_t eOprotIP_t
    @brief      used to identify a variable in a network, as the eOprotIP_t resembles the IP address of a device..
 **/
typedef uint32_t eOprotIP_t;


/** @typedef    typedef uint8_t eOprotBRD_t
    @brief      used as a short identifier of an IP address with values starting from 0 upto the max number of boards in the network
 **/
typedef uint8_t eOprotBRD_t;


/** @typedef    typedef uint32_t eOprotID32_t
    @brief      used to identify a variable inside a device. It can have holes in its representation
 **/
typedef uint32_t eOprotID32_t;

/** @typedef    typedef uint32_t eOprotPROGnum_t
    @brief      used to identify a variable inside a device. It does not have holes in its representation
 **/
typedef uint32_t eOprotPROGnum_t;


/** @typedef    typedef uint8_t eOprotEP_t
    @brief      It is part of eOprotID32_t. The endopint is used to group some variables of homogeneous type inside a given device. 
                for example it can be motion control in left lower arm. 
 **/
typedef uint8_t eOprotEP_t;

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


/** @typedef    uint32_t eOprotProgNumber_t
    @brief      it identifies a variable inside a device but it does not have holes in its representation.
 **/
typedef uint32_t eOprotProgNumber_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group eo_protocolcommon  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




