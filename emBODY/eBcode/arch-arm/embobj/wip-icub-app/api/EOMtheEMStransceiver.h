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
#ifndef _EOMTHEEMSTRANSCEIVER_H_
#define _EOMTHEEMSTRANSCEIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMStransceiver.h
	@brief      This header file implements public interface to the EMS transceiver singleton.
 	@author     marco.accame@iit.it
	@date       05/21/2012
 **/

/** @defgroup eom_theemstransceiver Singleton EOMtheEMStransceiver 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtransceiver.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMStransceiver_hid EOMtheEMStransceiver
    @brief      EOMtheEMStransceiver is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMStransceiver_hid EOMtheEMStransceiver;



/**	@typedef    typedef struct eOemstransceiver_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMStransceiver. 
 **/
typedef struct
{
    const eOnvset_DEVcfg_t*         nvsetdevcfg;
    eOipv4addr_t                    hostipv4addr;
    eOipv4port_t                    hostipv4port; 
    eo_transceiver_sizes_t          sizes; 
    eOtransceiver_protection_t      transprotection;
    eOnvset_protection_t            nvsetprotection; 
} eOemstransceiver_cfg_t;


typedef struct
{
    uint32_t    rxinvalidropframes;             /**< received invalid ropframe */
    uint32_t    rxseqnumwrong;                  /**< received ropframe with wrong sequence num*/  //errorsinsequencenumber;
    uint32_t    lostreplies;                    /**< error in appending a reply in reply-ropframe */
    uint32_t    failuresinloadofreplyropframe;  /**< error oin append reply-ropframe in the ropframe to tx*/
    uint32_t    txropframeistoobigforthepacket; /*the tx ropframe's size is bigger than pky payload's size*/
    uint32_t    cannotloadropinregulars;
    uint32_t    cannotloadropinoccasionals;
} eOemstransceiver_diagnosticsinfo_t;  

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// in here we shall place configuration of rop dimensione etc. now in defines such as EOK_BOARDTRANSCEIVER_capacityofpacket etc.
extern const eOemstransceiver_cfg_t eom_emstransceiver_DefaultCfg; // = {.hostipv4addr = EO_COMMON_IPV4ADDR(10, 0, 1, 200), .hostipv4port = 12345};


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMStransceiver * eom_emstransceiver_Initialise(const eOemstransceiver_cfg_t *emstransceivercfg)
    @brief      Initialise the EOMtheEMStransceiver singleton 
    @arg        emstransceivercfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMStransceiver
 **/
extern EOMtheEMStransceiver * eom_emstransceiver_Initialise(const eOemstransceiver_cfg_t *emstransceivercfg);


/** @fn         extern EOMtheEMStransceiver * eom_emstransceiver_GetHandle(void)
    @brief      Retrieve the EOMtheEMStransceiver singleton 
    @return     The handle to the EOMtheEMStransceiver
 **/
extern EOMtheEMStransceiver * eom_emstransceiver_GetHandle(void);


/** @fn         extern EOtransceiver* eom_emstransceiver_GetTransceiver(EOMtheEMStransceiver* p)
    @brief      Retrieves the handle to the transceiver instantiated in the EOMtheEMStransceiver 
    @arg        p                       The handle to the EOMtheEMStransceiver
    @return     The requ    ired EOtransceiver
 **/
extern EOtransceiver* eom_emstransceiver_GetTransceiver(EOMtheEMStransceiver* p);


extern EOnvSet* eom_emstransceiver_GetNVset(EOMtheEMStransceiver* p);


extern eOresult_t eom_emstransceiver_Parse(EOMtheEMStransceiver* p, EOpacket* rxpkt, uint16_t *numberofrops, eOabstime_t* txtime);

extern eOresult_t eom_emstransceiver_Form(EOMtheEMStransceiver* p, EOpacket** txpkt, uint16_t *numberofrops);

//this function copy internal data to dgn_ptr
extern eOresult_t eom_emstransceiver_GetDiagnosticsInfo(EOMtheEMStransceiver* p, eOemstransceiver_diagnosticsinfo_t *dgn_ptr);

extern eOemstransceiver_diagnosticsinfo_t* eom_emstransceiver_GetDiagnosticsInfoHandle(EOMtheEMStransceiver* p);

/** @}            
    end of group eom_theemstransceiver  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

