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
#ifndef _EOMTHEEMSDISCOVERYLISTENER_H_
#define _EOMTHEEMSDISCOVERYLISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSdiscoverylistener.h
	@brief      This header file implements public interface to the EMS discovery listener singleton.
 	@author     marco.accame@iit.it
	@date       05/21/2012
 **/

/** @defgroup eom_EOMtheEMSdiscoverylistener Singleton EOMtheEMSdiscoverylistener 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSdiscoverylistener_hid EOMtheEMSdiscoverylistener
    @brief      EOMtheEMSdiscoverylistener is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMSdiscoverylistener_hid EOMtheEMSdiscoverylistener;



/**	@typedef    typedef struct eOemsdiscoverylistener_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMSdiscoverylistener. 
 **/
typedef struct
{
    uint8_t                     taskpriority;
    uint16_t                    taskstacksize;   
    eOsizecntnr_t               inpdatagramnumber;
    eOsizecntnr_t               outdatagramnumber;
    eOsizeitem_t                inpdatagramsizeof;
    eOsizeitem_t                outdatagramsizeof;
    eOipv4port_t                localport;
    eOipv4port_t                remoteport;
    eOipv4addr_t                remoteipaddr;   // if EO_COMMON_IPV4ADDR_LOCALHOST, then the reply is sent to the ip of the caller
    eObool_t                    usemutex;     
} eOemsdiscoverylistener_cfg_t;




    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOemsdiscoverylistener_cfg_t eom_emsdiscoverylistener_DefaultCfg; // = {.taskpriority = 51, .taskstacksize = 512};


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSdiscoverylistener * eom_emsdiscoverylistener_Initialise(const eOemsdiscoverylistener_cfg_t *emsdiscoverylistenercfg)
    @brief      Initialise the EOMtheEMSdiscoverylistener singleton 
    @arg        emsdiscoverylistenercfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMSdiscoverylistener
 **/
extern EOMtheEMSdiscoverylistener * eom_emsdiscoverylistener_Initialise(const eOemsdiscoverylistener_cfg_t *emsdiscoverylistenercfg);


/** @fn         extern EOMtheEMSdiscoverylistener * eom_emsdiscoverylistener_GetHandle(void)
    @brief      Retrieve the EOMtheEMSdiscoverylistener singleton 
    @return     The handle to the EOMtheEMSdiscoverylistener
 **/
extern EOMtheEMSdiscoverylistener * eom_emsdiscoverylistener_GetHandle(void);




extern EOMtask * eom_emsdiscoverylistener_GetTask(EOMtheEMSdiscoverylistener *p);





/** @}            
    end of group eom_EOMtheEMSdiscoverylistener  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

