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
#ifndef _EOTHEAGENT_H_
#define _EOTHEAGENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtheAgent.h
    @brief      This header file implements public interface to the parser singleton used for communication protocol
    @author     marco.accame@iit.it
    @date       09/06/2011
**/

/** @defgroup eo_theagent Object EOtheAgent
    The EOtheAgent is a singleton which is only responsible to evaluate the content of a received EOrop
    and to execute it and to prepare a EOrop for transmission.
      
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOrop.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct EOtheAgent_hid EOtheAgent
    @brief      EOtheAgent is an opaque struct. It is used to implement data abstraction for the Parser  
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOtheAgent_hid EOtheAgent;



typedef struct
{
    void (*on_rop_conf_received)(eOipv4addr_t ipaddr, eOropdescriptor_t* ropdes);
    void (*on_rop_conf_requested)(eOipv4addr_t ipaddr, eOropdescriptor_t* ropdes);
} eOagent_cfg_t;
 

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOagent_cfg_t eo_agent_cfg_default; // = {NULL, NULL};


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
 
/** @fn         extern EOtheAgent* eo_agent_Initialise(void)
    @brief      Initialise the singleton EOtheAgent. 
    @param      cfg         Contains actions to be done on reception or transmission which are specific to the application.
                            If NULL, then  is is issued a info by the EOtheErrorManager.
    @return     A valid and not-NULL pointer to the EOtheAgent singleton.
 **/
extern EOtheAgent* eo_agent_Initialise(const eOagent_cfg_t *cfg);



/** @fn         extern EOtheAgent* eo_agent_GetHandle(void)
    @brief      Gets the handle of the EOtheAgent singleton 
    @return     Constant pointer to the singleton.
 **/
extern EOtheAgent* eo_agent_GetHandle(void);


// processes a received rop, it may call on_rop_confirmation_received(), it may execute some actions related to the rop,
// it may produce a rop in output. the rop in output must not be fed to eo_agent_OutROPinit() or eo_agent_OutROPfill()
extern eOresult_t eo_agent_InpROPprocess(EOtheAgent *p, EOrop *ropin, EOnvSet* nvset, eOipv4addr_t fromipaddr, EOrop *replyrop);

// OK: called by eo_transmitter_occasional_rops_Load() and eo_transmitter_regular_rops_Load(). 
// if data is required this function uses ropdescr->data/size if not NULL/0, otherwise if NULL it used data from EOnv.
extern eOresult_t eo_agent_OutROPprepare(EOtheAgent* p, EOnv* nv, eOropdescriptor_t* ropdescr, EOrop* rop, uint16_t* requiredbytes);






/** @}            
    end of group eo_theagent  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



