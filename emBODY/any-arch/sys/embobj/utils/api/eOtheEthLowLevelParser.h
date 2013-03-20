/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _EOTHEETHLOWLEVELPARSER_H_
#define _EOTHEETHLOWLEVELPARSER_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOtheEthLowLevelParser.h
    @brief      
    @author     valentina.gaggero@iit.it
    @date       03/18/2013
**/

/** @defgroup eo_array Object EOarray
    The EOarray object is a container which is work-in-progress. It could be used as a slim vector
    in which the content of memory is exposed outsize via a number of data strcuctures such as eOarrayofbytes_t
    or eOarrayofhalves_t or eOarrayofwords_t
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------

  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef enum
{
    protoType_udp = 0,
    protoType_tcp = 1,
    protoType_icmp = 2
} eOethLowLevParser_protocolType_t;

typedef struct
{
    uint8_t                             *payload_ptr;
    uint32_t                            size;
    uint32_t                            src_addr;      //host order
    uint32_t                            dst_addr;       //host order
    uint32_t                            src_port;      //host order
    uint32_t                            dst_port;       //host order
    eOethLowLevParser_protocolType_t    prototype;
} eOethLowLevParser_packetInfo_t;

typedef struct
{
    uint32_t                            src_addr;      /**<  if it values 0 ==> no filter by src addr is applied */
    uint32_t                            dst_addr;      /**<  if it values 0 ==> no filter by dst addr is applied */
    uint32_t                            src_port;      /**<  if it values 0 ==> no filter by src port is applied */
    uint32_t                            dst_port;      /**<  if it values 0 ==> no filter by dst port is applied */
    eOethLowLevParser_protocolType_t    prototype;     /* ATTENTION: currently only UDP packet are parsed, so this filed is not used to filter packets*/
} eOethLowLevParser_cfg_connectionfilters_t;

typedef struct
{
    eOethLowLevParser_cfg_connectionfilters_t   filters;
    uint8_t                                     filtersEnable;
} eOethLowLevParser_cfg_connectionfiltersData_t;



/* this callback is invoked to parse the payload of packet (without Ethernet, IP and UDP/TCP protocols eheders)*/
typedef     eOresult_t        (*eOtheEthLowLevParser_applParser_t)    (void* arg, eOethLowLevParser_packetInfo_t *pktInfo_ptr);


typedef struct
{
    eOtheEthLowLevParser_applParser_t   func;
    void                                *arg;
} eOethLowLevParser_applicationParserData_t;

/*contains the configuration data of eOethLowLevParser*/
typedef struct
{
    eOethLowLevParser_cfg_connectionfiltersData_t       conFiltersData; //cosa filtrare. In alcune piattaforme e' permesso proprio non catturare i pacchetti non interessanti,in altre vengono catturati tutti e poi filtrati via sw
    eOethLowLevParser_applicationParserData_t           appParserData;    
} eOethLowLevParser_cfg_t;



typedef struct eOTheEthLowLevParser_hid eOTheEthLowLevParser;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
extern eOTheEthLowLevParser * eo_ethLowLevParser_Initialise(const eOethLowLevParser_cfg_t *cfg);
extern eOTheEthLowLevParser * eo_ethLowLevParser_GetHandle(void);
extern eOresult_t eOTheEthLowLevParser_GetUDPdatagramPayload(eOTheEthLowLevParser *p, uint8_t *packet, eOethLowLevParser_packetInfo_t *pktInfo_ptr);
extern eOresult_t eOTheEthLowLevParser_DissectPacket(eOTheEthLowLevParser *p, uint8_t *packet);
 


/** @}            
    end of group eo_array  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

