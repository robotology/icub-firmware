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
#include "EOsm.h"
#include "eOcfg_sm_EMSappl.h"
#include "EOrop.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  
  
#if defined(USE_EMBOT_appl)   
#error USE_EMBOT_appl is defined, dont include EOMtheEMSappl.h
#endif

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
    const eEmoduleInfo_t*   emsappinfo;
    eOipv4addr_t            hostipv4addr; 
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


//extern EOsm* eom_emsappl_GetStateMachine(EOMtheEMSappl *p) ;


extern eOresult_t eom_emsappl_SM_ProcessEvent(EOMtheEMSappl *p, eOsmEventsEMSappl_t ev);

/** @fn         eOresult_t eom_emsappl_ProcessGo2stateRequest(EOMtheEMSappl *p, eOemsappl_applstate_t newstate)
    @brief      say the application to go to new state as soon as possible. In some case the tarnsition can not be done immediatly,
                so it is necessary waiting current activity finished.
                if new state is equal to current state any trnasition is performed, and return ok.
                if current state is error, this function returns eores_NOK_unsupported, becoause is not possible live error state.
                if new state is error, this function returns eores_NOK_unsupported, becoause is not possible go to aerror state with command, but only because an interal error happened.
    @arg        p           EOMtheEMSappl handle
                newstate    the new state of application
    @return     eores_OK on success
                eores_NOK_unsupported if transition is not possible
                eores_NOK_nullpointer if p is null.
    @note       the user can overriden eom_emsappl_hid_userdef_ProcessGo2stateRequest, in order to notify to its obj transition toward newstate.
 **/
extern eOresult_t eom_emsappl_ProcessGo2stateRequest(EOMtheEMSappl *p, eOsmStatesEMSappl_t newstate);

 
extern eOresult_t eom_emsappl_GetCurrentState(EOMtheEMSappl *p, eOsmStatesEMSappl_t *currentstate);


extern eOresult_t eom_emsappl_SendTXRequest(EOMtheEMSappl *p);

extern eOresult_t eom_emsappl_Transmit_OccasionalROP(EOMtheEMSappl *p, eOropdescriptor_t *ropdes);



/** @}            
    end of group eom_theemsappl  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

