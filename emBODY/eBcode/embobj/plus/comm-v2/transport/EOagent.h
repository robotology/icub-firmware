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
#ifndef _EOAGENT_H_
#define _EOAGENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOagent.h
    @brief      This header file implements public interface to the agent used for communication protocol
    @author     marco.accame@iit.it
    @date       12/12/2013
**/

/** @defgroup eo_agent Object EOagent
    The EOagent is responsible to evaluate the content of a received EOrop, to execute it and to prepare a EOrop for 
    reply.
      
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOrop.h"
#include "EOnvSet.h"
#include "EOconfirmationManager.h"
#include "EOproxy.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct EOagent_hid EOagent
    @brief      EOagent is an opaque struct. It is used to implement data abstraction for the agent  
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOagent_hid EOagent;


typedef struct
{
    EOnvSet*                nvset;
    EOconfirmationManager*  confman;
    EOproxy*                proxy;
} eOagent_cfg_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
 
/** @fn         extern EOagent* eo_agent_New(const eOagent_cfg_t* cfg)
    @brief      Initialise the EOagent. 
    @param      cfg         contains the configuration. it cannot be NULL.
    @return     A valid and not-NULL pointer to the EOagent
 **/
extern EOagent* eo_agent_New(const eOagent_cfg_t* cfg);

extern EOnvSet* eo_agent_GetNVset(EOagent *p);

extern EOconfirmationManager* eo_agent_GetConfirmationManager(EOagent *p);

extern EOproxy* eo_agent_GetProxy(EOagent *p);

// processes a received rop, it may call on_rop_confirmation_received(), it may execute some actions related to the rop,
// it may produce a rop in output. the rop in output must not be fed to eo_agent_OutROPinit() or eo_agent_OutROPfill()
extern eOresult_t eo_agent_InpROPprocess(EOagent *p, EOrop *ropin, eOipv4addr_t fromipaddr, EOrop *replyrop);

// OK: called by eo_transmitter_occasional_rops_Load() and eo_transmitter_regular_rops_Load(). 
// if data is required this function uses ropdescr->data/size if not NULL/0, otherwise if NULL it used data from EOnv.
extern eOresult_t eo_agent_OutROPprepare(EOagent* p, EOnv* nv, eOropdescriptor_t* ropdescr, EOrop* rop, uint16_t* requiredbytes);






/** @}            
    end of group eo_theagent  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



