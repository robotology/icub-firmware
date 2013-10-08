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
#ifndef _EOPROTOCOLROPFRAME_H_
#define _EOPROTOCOLROPFRAME_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolROPframe.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_protocolropframe protocol of ... 
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocolCommon.h"
#include "EoProtocolROP.h"


// - public #define  --------------------------------------------------------------------------------------------------

#define EOPROT_ROPFRAME_STARTOFFRAME    0x12345678
#define EOPROT_ROPFRAME_FOOTER          0x87654321


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct eOprot_ROPframe_head_t
    @brief      describes the header of the ropframe. its format is the same as it is inside the packet.
 **/
typedef struct  // 24 bytes
{
    uint32_t                startofframe;       /**< used to identify the beginning of a frame. its value is EOPROT_ROPFRAME_STARTOFFRAME */
    uint16_t                ropssizeof;         /**< tells how big is the field of the ropframe which contains to rops */
    uint16_t                ropsnumberof;       /**< tells how many rops are inside the ropframe */
    uint64_t                ageofframe;         /**< contains the time of creation of teh ropframe */
    uint64_t                sequencenumber;     /**< contains a sequence number originated by the transmitter  */
} eOprot_ROPframe_head_t;   EO_VERIFYsizeof(eOprot_ROPframe_head_t, 24);


/** @typedef    typedef struct eOprot_ROPframe_descriptor_t
    @brief      describes the ropframe. 
 **/
typedef struct
{
    eOprot_ROPframe_head_t  head;           /**< always present */
    uint32_t*               rops;           /**< if present, its effective size is multiple of 4 bytes */
    uint32_t                footer;         /**< always present. its value is EOPROT_ROPFRAME_FOOTER */
} eOprot_ROPframe_descriptor_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group eo_protocolropframe  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




