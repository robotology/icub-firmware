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
    
    The EOMtheEMSdiscoverytransceiver object works together with the EOMtheEMSdiscoverylistener to manage messages coming
    from the ethLoader. Its main task is to shutdown and order execution of the eUpdater when a valid message received.  
    The shutdown happens after 20 ms in order to allow a reply. TODO: make the 20ms a configuration parameter.
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOpacket.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSdiscoverytransceiver_hid EOMtheEMSdiscoverytransceiver
    @brief      EOMtheEMSdiscoverytransceiver is an opaque struct. 
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
    eOipv4addr_t                hostipv4addr;       /**< if eok_ipv4addr_localhost, then we process packets from every ip address and reply to it */
    eOipv4port_t                hostipv4port;       /**< the port is the same locally and in the host. the ethLoader uses port 3333 */ 
    uint16_t                    txpktcapacity;      /**< the capacity of teh reply packet */
    eOemsdiscovery_protocol_t   discoveryprotocol;  /**< the protocol to use */
    eOreltime_t                 dbgshutdowntime;    /**< if non-0, then after the specified time a shutdown is forced and teh eUpdater is executed */
} eOemsdiscoverytransceiver_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


extern const eOemsdiscoverytransceiver_cfg_t eom_emsdiscoverytransceiver_DefaultCfg; 


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

