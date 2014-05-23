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
#ifndef _EOMTHEEMSAPPLCFG_H_
#define _EOMTHEEMSAPPLCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSapplCfg.h
	@brief      This header file implements public interface to the EMS application configuration singleton.
 	@author     marco.accame@iit.it
	@date       06/25/2012
 **/

/** @defgroup eom_theemsapplcfg Singleton EOMtheEMSapplCfg 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "eEcommon.h"

#include "EOMtheSystem.h"

#include "EOMtheEMSappl.h"

#include "EOMtheEMSdiscoverylistener.h"
#include "EOMtheEMSdiscoverytransceiver.h"
#include "EOMtheEMSbackdoor.h"
#include "EOMtheEMSsocket.h"

#include "EOMtheIPnet.h"
#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSerror.h"
#include "EOMtheEMSconfigurator.h"
#include "EOMtheEMSrunner.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

// ok, but better move it elsewhere .... in the same place where we define the endpoint categories
//enum {eom_emsappl_eptype_numberof = 3};


/** @typedef    typedef const struct EOMtheEMSapplCfg
    @brief      EOMtheEMSapplCfg is used ...
 **/  
typedef const struct
{
    eOmsystem_whole_cfg_t           wsyscfg;                    /**< the cfg used by the principal object EOMtheSystem */
    eOemsappl_cfg_t                 applcfg;                    /**< the cfg used by the object EOMtheEMSappl initialised in eom_sys_Start() */
    eOmipnet_whole_cfg_t            wipnetcfg;                  /**< the cfg used by the object EOMtheIPnet launched by the EOMtheEMSappl */
    eObool_t                        getipaddrFROMenvironment;   /**< if true gets IP address from shared-services */   
    eObool_t                        errmng_haltrace_enabled;    /**< if true enable hal trace in error manager to write to debug window */
    uint8_t                         boardid;                    /**< a number from 1 to max number. use 255 for none */
    eOemsdiscoverylistener_cfg_t    disclistcfg;                /**< configuration of the discovery listener */
    eOemsbackdoor_cfg_t             backdoorcfg;                /**< configuration of the backdoor */
    eOemssocket_cfg_t               socketcfg;                  /**< the cfg used by the object EOMtheEMSsocket launched by the EOMtheEMSappl */
    eOemstransceiver_cfg_t          transcfg;                   /**< the cfg used by the object EOMtheEMStransceiver launched by the EOMtheEMSappl */
    eOemserror_cfg_t                errobjcfg;                  /**<   */
    eOemsconfigurator_cfg_t         cfgobjcfg;                  /**<   */
    eOemsrunner_cfg_t               runobjcfg;                  /**<   */
} EOMtheEMSapplCfg;        





    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eEmoduleInfo_t eom_emsapplcfg_modinfo;

//extern const uint32_t eom_emsapplcfg_extra;

// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSapplCfg * eom_emsapplcfg_Initialise(void)
    @brief      Initialise the EOMtheEMSapplCfg singleton. It also performs a verification of the const values
                imposed by the users.
    @return     The handle to the EOMtheEMSapplCfg
 **/

extern EOMtheEMSapplCfg * eom_emsapplcfg_Initialise(void);


/** @fn         extern EOMtheEMSapplCfg * eom_emsapplcfg_GetHandle(void)
    @brief      Retrieve the EOMtheEMSapplCfg singleton. If not yet initialised, it calls eom_emsapplcfg_Initialise().
    @return     The handle to the EOMtheEMSapplCfg
 **/
extern EOMtheEMSapplCfg * eom_emsapplcfg_GetHandle(void);


/** @}            
    end of group eom_theemsapplcfg  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

