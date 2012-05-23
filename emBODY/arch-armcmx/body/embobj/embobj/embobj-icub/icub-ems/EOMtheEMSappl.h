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
#ifndef _EOMTHEEMSAPPL_H_
#define _EOMTHEEMSAPPL_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSappl.h
	@brief      This header file implements public interface to the EMS application singleton.
 	@author     marco.accame@iit.it
	@date       05/21/2012
 **/

/** @defgroup eom_theemsappl Singleton EOMtheEMSappl 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "eEcommon.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSappl_hid EOMtheEMSappl
    @brief      EOMtheEMSappl is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMSappl_hid EOMtheEMSappl;



/**	@typedef    typedef struct eOemsappl_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMSappl. 
 **/
typedef struct
{
    eEmoduleInfo_t* emsappinfo;
    eOipv4addr_t    hostipv4addr;
    eOipv4port_t    hostipv4port;    
} eOemsappl_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOemsappl_cfg_t eom_emsappl_DefaultCfg; // = {.emsappinfo = NULL, .hostipv4addr = EO_COMMON_IPV4ADDR(10, 0, 1, 200), .hostipv4port = 12345};


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSappl * eom_emsappl_Initialise(const eOemsappl_cfg_t *emsapplcfg)
    @brief      Initialise the EOMtheEMSappl singleton 
    @arg        emsapplcfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMSappl
 **/

extern EOMtheEMSappl * eom_emsappl_Initialise(const eOemsappl_cfg_t *emsapplcfg);


/** @fn         extern EOMtheEMSappl * eom_emsappl_GetHandle(void)
    @brief      Retrieve the EOMtheEMSappl singleton 
    @return     The handle to the EOMtheEMSappl
 **/
extern EOMtheEMSappl * eom_emsappl_GetHandle(void);
 
 





/** @}            
    end of group eom_theemsappl  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

