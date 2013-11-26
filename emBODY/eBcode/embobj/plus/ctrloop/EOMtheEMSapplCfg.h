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

#if !defined(EMSAPPL_USE_CORE)
#include "EOtheEMSapplBody.h"
#endif

#warning --> pensare a come rendere questo file indipendente dalla board usata (ems001 oppure ems004 oppure mc4).
// bisogna togliere: EOtheEMSapplBody.h, rendere eom_emsapplcfg_boardid_t un semplice eOenum08_t, eom_emsapplcfg_deviceid_t, etc.

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
    eom_emsappl_boardid_eb1         = 1,
    eom_emsappl_boardid_eb2         = 2,
    eom_emsappl_boardid_eb3         = 3,
    eom_emsappl_boardid_eb4         = 4,
    eom_emsappl_boardid_eb5         = 5,
    eom_emsappl_boardid_eb6         = 6,
    eom_emsappl_boardid_eb7         = 7,
    eom_emsappl_boardid_eb8         = 8,
    eom_emsappl_boardid_eb9         = 9,
    eom_emsappl_boardid_ebnone      = 255
} eom_emsapplcfg_boardid_t;

typedef enum
{
    eom_emsappl_deviceid_skin       = 0,
    eom_emsappl_deviceid_mc4        = 1,
    eom_emsappl_deviceid_2foc       = 2
} eom_emsapplcfg_deviceid_t;

enum {eom_emsappl_deviceid_numberof = 3};


typedef enum
{
    eom_emsappl_eptype_mc           = 0,
    eom_emsappl_eptype_as           = 1,
    eom_emsappl_eptype_sk           = 2
} eom_emsapplcfg_eptype_t;

// ok, but better move it elsewhere .... in the same place where we define teh endpoint categories

enum {eom_emsappl_eptype_numberof = 3};


/** @typedef    typedef const struct EOMtheEMSapplCfg
    @brief      EOMtheEMSapplCfg is used ...
 **/  
typedef const struct
{
    eOmsystem_whole_cfg_t           wsyscfg;        /**< the cfg used by the principal object EOMtheSystem */
    eOemsappl_cfg_t                 applcfg;        /**< the cfg used by the object EOMtheEMSappl initialised in eom_sys_Start() */
    eOmipnet_whole_cfg_t            wipnetcfg;      /**< the cfg used by the object EOMtheIPnet launched by the EOMtheEMSappl */
    eObool_t                        getipaddrFROMenvironment;   
    eObool_t                        errmng_haltrace_enabled;  /**< if true enable hal trace in error manager to write to debug window */
    eom_emsapplcfg_boardid_t        boardid;
    eObool_t                        hasdevice[eom_emsappl_deviceid_numberof];
    eOnvEP_t                        eps[eom_emsappl_eptype_numberof];
    eOemsdiscoverylistener_cfg_t    disclistcfg;
    eOemsbackdoor_cfg_t             backdoorcfg;
    eOemssocket_cfg_t               socketcfg;      /**< the cfg used by the object EOMtheEMSsocket launched by the EOMtheEMSappl */
    eOemstransceiver_cfg_t          transcfg;       /**< the cfg used by the object EOMtheEMStransceiver launched by the EOMtheEMSappl */
    eOemserror_cfg_t                errobjcfg;      /**<   */
    eOemsconfigurator_cfg_t         cfgobjcfg;
    eOemsrunner_cfg_t               runobjcfg;
#if !defined(EMSAPPL_USE_CORE)    
    eOtheEMSappBody_cfg_t           applbodycfg;         /**<  the cfg used by services provider */
#endif    
} EOMtheEMSapplCfg;        





    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

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


/** @fn         extern eObool_t eom_emsapplcfg_HasDevice(EOMtheEMSapplCfg *p, eom_emsapplcfg_deviceid_t dev)
    @brief      tells if the application manages a given device.
    @return     eobool_true or eobool_false
    @warning    inside the _hid.h part it is possible to use the macros EOMTHEEMSAPPLCFG_EBX_hasSKIN, EOMTHEEMSAPPLCFG_EBX_hasMC4
                and EOMTHEEMSAPPLCFG_EBX_has2FOC which keep a eObool_t value.
 **/
extern eObool_t eom_emsapplcfg_HasDevice(EOMtheEMSapplCfg *p, eom_emsapplcfg_deviceid_t dev); 


/** @fn         extern eOnvEP_t eom_emsapplcfg_Get_nvEPfor(EOMtheEMSapplCfg *p, eom_emsapplcfg_eptype_t eptype)
    @brief      tells the ep for a given category of endpoint.
    @return     teh ep value or 0xffff if the ep type is not present
 **/
extern eOnvEP_t eom_emsapplcfg_Get_nvEPfor(EOMtheEMSapplCfg *p, eom_emsapplcfg_eptype_t eptype); 


/** @}            
    end of group eom_theemsapplcfg  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

