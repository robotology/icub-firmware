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
#ifndef _EOCONFIRMATIONMANAGER_H_
#define _EOCONFIRMATIONMANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOconfirmationManager.h
    @brief      This header file implements public interface to ....
    @author     marco.accame@iit.it
    @date       02/05/2013
**/

/** @defgroup eo_confman Object EOconfirmationManager
    The EOconfirmationManager object is used as ...
         
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOrop.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct EOconfirmationManager_hid EOconfirmationManager
    @brief      EOconfirmationManager is an opaque struct. It is used to implement data abstraction for the datagram 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOconfirmationManager_hid EOconfirmationManager;


typedef struct
{
    void (*on_rop_conf_requested)(eOipv4addr_t toipaddr, eOropdescriptor_t* ropdes);
    void (*on_rop_conf_received)(eOipv4addr_t fromipaddr, eOropdescriptor_t* ropdes);
} eOconfman_cfg_t;
 

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOconfman_cfg_t eOconfman_cfg_default; 


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOconfirmationManager* eo_confman_New(const eOconfman_cfg_t *cfg)
    @brief      Creates a new EOconfirmationManager object
    @param      cfg     The cfg.
    @return     The pointer to the required object.
 **/

extern EOconfirmationManager* eo_confman_New(const eOconfman_cfg_t *cfg);


extern eOresult_t eo_confman_Confirmation_Requested(EOconfirmationManager *p, eOipv4addr_t toipaddr, eOropdescriptor_t* ropdes);
                                                   
                                                   
extern eOresult_t eo_confman_Confirmation_Received(EOconfirmationManager *p, eOipv4addr_t fromipaddr, eOropdescriptor_t* ropdes);

                                                   





/** @}            
    end of group eo_confman  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

