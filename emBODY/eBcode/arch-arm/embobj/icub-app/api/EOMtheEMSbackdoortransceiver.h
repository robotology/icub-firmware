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
#ifndef _EOMTHEEMSBACKDOORTRANSCEIVER_H_
#define _EOMTHEEMSBACKDOORTRANSCEIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSbackdoortransceiver.h
	@brief      This header file implements public interface to the EMS backdoortransceiver singleton.
 	@author     marco.accame@iit.it
	@date       03/19/2013
 **/

/** @defgroup eom_theemsbackdoortransceiver Singleton EOMtheEMSbackdoortransceiver 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOpacket.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSbackdoortransceiver_hid EOMtheEMSbackdoortransceiver
    @brief      EOMtheEMSbackdoortransceiver is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMSbackdoortransceiver_hid EOMtheEMSbackdoortransceiver;


typedef enum
{
    eobackdoor_protocol_userdef_opc         = 0
} eOemsbackdoor_protocol_t;


/**	@typedef    typedef struct eOemsbackdoortransceiver_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMSbackdoortransceiver. 
 **/
typedef struct
{
    eOipv4port_t                hostipv4port; 
    uint16_t                    replypktcapacity;
    eOemsbackdoor_protocol_t    backdoorprotocol;
} eOemsbackdoortransceiver_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// in here we shall place configuration of rop dimensione etc. now in defines such as EOK_BOARDDISCOVERYTRANSCEIVER_capacityofpacket etc.
extern const eOemsbackdoortransceiver_cfg_t eom_emsbackdoortransceiver_DefaultCfg;  // = {  .hostipv4port       = 3333,
                                                                                    //      .replypktcapacity   = 64,
                                                                                    //      .backdoorprotocol   = eobackdoor_protocol_userdef_opc
                                                                                    //   };


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSbackdoortransceiver * eom_emsbackdoortransceiver_Initialise(const eOemsbackdoortransceiver_cfg_t *emsbackdoortransceivercfg)
    @brief      Initialise the EOMtheEMSbackdoortransceiver singleton 
    @arg        emsbackdoortransceivercfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMSbackdoortransceiver
 **/
extern EOMtheEMSbackdoortransceiver * eom_emsbackdoortransceiver_Initialise(const eOemsbackdoortransceiver_cfg_t *emsbackdoortransceivercfg);


/** @fn         extern EOMtheEMSbackdoortransceiver * eom_emsbackdoortransceiver_GetHandle(void)
    @brief      Retrieve the EOMtheEMSbackdoortransceiver singleton 
    @return     The handle to the EOMtheEMSbackdoortransceiver
 **/
extern EOMtheEMSbackdoortransceiver * eom_emsbackdoortransceiver_GetHandle(void);



extern eOresult_t eom_emsbackdoortransceiver_Parse(EOMtheEMSbackdoortransceiver* p, EOpacket* rxpkt);

extern eOresult_t eom_emsbackdoortransceiver_GetReply(EOMtheEMSbackdoortransceiver* p, EOpacket** txpkt);


/** @}            
    end of group eom_theemsbackdoortransceiver  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

