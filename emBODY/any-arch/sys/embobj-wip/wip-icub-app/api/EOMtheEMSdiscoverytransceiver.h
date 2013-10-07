/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _EOMTHEEMSDISCOVERYTRANSCEIVER_H_
#define _EOMTHEEMSDISCOVERYTRANSCEIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSdiscoverytransceiver.h
	@brief      This header file implements public interface to the EMS discoverytransceiver singleton.
 	@author     marco.accame@iit.it
	@date       05/21/2012
 **/

/** @defgroup eom_theemsdiscoverytransceiver Singleton EOMtheEMSdiscoverytransceiver 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOpacket.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSdiscoverytransceiver_hid EOMtheEMSdiscoverytransceiver
    @brief      EOMtheEMSdiscoverytransceiver is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMSdiscoverytransceiver_hid EOMtheEMSdiscoverytransceiver;


typedef enum
{
    eodiscovery_protocol_ethloader_reduced          = 0  
} eOemsdiscovery_protocol_t;


/**	@typedef    typedef struct eOemsdiscoverytransceiver_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMSdiscoverytransceiver. 
 **/
typedef struct
{
    eOipv4addr_t                hostipv4addr;           /**< if EO_COMMON_IPV4ADDR_LOCALHOST, then we process packets from every ip address and reply to it */
    eOipv4port_t                hostipv4port; 
    uint16_t                    txpktcapacity;
    eOemsdiscovery_protocol_t   discoveryprotocol;
} eOemsdiscoverytransceiver_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// in here we shall place configuration of rop dimensione etc. now in defines such as EOK_BOARDDISCOVERYTRANSCEIVER_capacityofpacket etc.
extern const eOemsdiscoverytransceiver_cfg_t eom_emsdiscoverytransceiver_DefaultCfg; // = { .hostipv4addr  = EO_COMMON_IPV4ADDR_LOCALHOST,
                                                                                     //     .hostipv4port  = 3333,
                                                                                     //     .txpktcapacity = 64
                                                                                     //   };


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSdiscoverytransceiver * eom_emsdiscoverytransceiver_Initialise(const eOemsdiscoverytransceiver_cfg_t *emsdiscoverytransceivercfg)
    @brief      Initialise the EOMtheEMSdiscoverytransceiver singleton 
    @arg        emsdiscoverytransceivercfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMSdiscoverytransceiver
 **/
extern EOMtheEMSdiscoverytransceiver * eom_emsdiscoverytransceiver_Initialise(const eOemsdiscoverytransceiver_cfg_t *emsdiscoverytransceivercfg);


/** @fn         extern EOMtheEMSdiscoverytransceiver * eom_emsdiscoverytransceiver_GetHandle(void)
    @brief      Retrieve the EOMtheEMSdiscoverytransceiver singleton 
    @return     The handle to the EOMtheEMSdiscoverytransceiver
 **/
extern EOMtheEMSdiscoverytransceiver * eom_emsdiscoverytransceiver_GetHandle(void);



extern eOresult_t eom_emsdiscoverytransceiver_Parse(EOMtheEMSdiscoverytransceiver* p, EOpacket* rxpkt);

extern eOresult_t eom_emsdiscoverytransceiver_GetReply(EOMtheEMSdiscoverytransceiver* p, EOpacket** txpkt);


/** @}            
    end of group eom_theemsdiscoverytransceiver  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

