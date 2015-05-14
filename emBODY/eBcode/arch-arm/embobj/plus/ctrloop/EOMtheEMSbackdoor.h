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
#ifndef _EOMTHEEMSBACKDOOR_H_
#define _EOMTHEEMSBACKDOOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSbackdoor.h
	@brief      This header file implements public interface to the EMS backdoor singleton.
 	@author     marco.accame@iit.it
	@date       03/21/2013
 **/

/** @defgroup eom_EOMtheEMSbackdoor Singleton EOMtheEMSbackdoor 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSbackdoor_hid EOMtheEMSbackdoor
    @brief      EOMtheEMSbackdoor is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMSbackdoor_hid EOMtheEMSbackdoor;



/**	@typedef    typedef struct eOemsbackdoor_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMSbackdoor. 
 **/
typedef struct
{
    uint8_t                     enabled;
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
} eOemsbackdoor_cfg_t;




    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOemsbackdoor_cfg_t eom_emsbackdoor_DefaultCfg; // = {.enabled = 1, .taskpriority = 51, .taskstacksize = 512};


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSbackdoor * eom_emsbackdoor_Initialise(const eOemsbackdoor_cfg_t *emsbackdoorcfg)
    @brief      Initialise the EOMtheEMSbackdoor singleton 
    @arg        emsbackdoorcfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMSbackdoor
 **/
extern EOMtheEMSbackdoor * eom_emsbackdoor_Initialise(const eOemsbackdoor_cfg_t *emsbackdoorcfg);


/** @fn         extern EOMtheEMSbackdoor * eom_emsbackdoor_GetHandle(void)
    @brief      Retrieve the EOMtheEMSbackdoor singleton 
    @return     The handle to the EOMtheEMSbackdoor
 **/
extern EOMtheEMSbackdoor * eom_emsbackdoor_GetHandle(void);



extern EOMtask * eom_emsbackdoor_GetTask(EOMtheEMSbackdoor *p);


extern eOresult_t eom_emsbackdoor_Transmit(EOMtheEMSbackdoor *p, uint8_t* data, uint16_t size, eOreltime_t timeout);

extern eOresult_t eom_emsbackdoor_Signal(EOMtheEMSbackdoor *p, uint16_t var , eOreltime_t timeout);

/** @}            
    end of group eom_EOMtheEMSbackdoor  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

