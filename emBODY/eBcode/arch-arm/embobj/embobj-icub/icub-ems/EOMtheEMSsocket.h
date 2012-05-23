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
#ifndef _EOMTHEEMSSOCKET_H_
#define _EOMTHEEMSSOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSsocket.h
	@brief      This header file implements public interface to the EMS application singleton.
 	@author     marco.accame@iit.it
	@date       05/21/2012
 **/

/** @defgroup eom_theemssocket Singleton EOMtheEMSsocket 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOaction.h"
#include "EOpacket.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSsocket_hid EOMtheEMSsocket
    @brief      EOMtheEMSsocket is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMSsocket_hid EOMtheEMSsocket;



/**	@typedef    typedef struct eOemssocket_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMSsocket. 
 **/
typedef struct
{
    eOsizecntnr_t               inpdatagramnumber;
    eOsizecntnr_t               outdatagramnumber;
    eOsizeitem_t                datagramsizeof;
    eOipv4port_t                localport;
    eObool_t                    usemutex;
} eOemssocket_cfg_t;

typedef enum
{
    emssocket_evt_packet_received   =   0x00000001
} eOemssocket_event_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOemssocket_cfg_t eom_emssocket_DefaultCfg; // = {.inpdatagramnumber = 2, .outdatagramnumber = 1, 
                                                         //    .datagramsizeof = 768, .localport = 12345, .usemutex = eobool_false};


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSsocket * eom_emssocket_Initialise(const eOemssocket_cfg_t *emssocketcfg)
    @brief      Initialise the EOMtheEMSsocket singleton 
    @arg        emssocketcfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMSsocket
 **/
extern EOMtheEMSsocket * eom_emssocket_Initialise(const eOemssocket_cfg_t *emssocketcfg);


/** @fn         extern EOMtheEMSsocket * eom_emssocket_GetHandle(void)
    @brief      Retrieve the EOMtheEMSsocket singleton 
    @return     The handle to the EOMtheEMSsocket
 **/
extern EOMtheEMSsocket * eom_emssocket_GetHandle(void);


/** @fn         extern eOresult_t eom_emssocket_Open(EOMtheEMSsocket *p, EOaction* withactiononrx)
    @brief      Opens the socket 
    @arg        p               The handle to the object
    @arg        withactiononrx  pointer to the action to be executed on reception. If NULL, then no action is executed.
    @return     The value eores_NOK_nullpointer if @e p is NULL, eores_OK otherwise.
 **/
extern eOresult_t eom_emssocket_Open(EOMtheEMSsocket *p, EOaction* withactiononrx);


/** @fn         extern eOresult_t eom_emssocket_Close(EOMtheEMSsocket *p)
    @brief      Deactivate the socket 
    @arg        p               The handle to the object
    @return     The value eores_NOK_nullpointer if @e p is NULL, eores_OK otherwise.
 **/
extern eOresult_t eom_emssocket_Close(EOMtheEMSsocket *p);

extern eOsizecntnr_t eom_emssocket_NumberOfReceivedPackets(EOMtheEMSsocket *p);

extern eOresult_t eom_emssocket_Receive(EOMtheEMSsocket *p, EOpacket** rxpkt, eOsizecntnr_t* remaining);  

extern eOresult_t eom_emssocket_Connect(EOMtheEMSsocket *p, eOipv4addr_t remaddr); 

extern eOresult_t eom_emssocket_Transmit(EOMtheEMSsocket *p, EOpacket* txpkt);   







/** @}            
    end of group eom_theemssocket  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

