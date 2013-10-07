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
    void (*on_rop_conf_requested)(EOrop *rop, eOipv4addr_t toipaddr);
    void (*on_rop_conf_received)(EOrop *rop, eOipv4addr_t fromipaddr);
} eOconfman_cfg_t;
 

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOconfman_cfg_t eOconfman_cfg_default; 


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOconfirmationManager* eo_transmitter_New(uint16_t capacity)
    @brief      Creates a new frame object and allocates memory able to store @e capacity bytes. If @e capacity is
                zero, then the object shall have external storage mode.
    @param      capacity   The max size of the packet.
    @return     The pointer to the required object.
 **/
 
 

extern EOconfirmationManager* eo_confman_New(const eOconfman_cfg_t *cfg);


extern eOresult_t eo_confman_Confirmation_Requested(EOconfirmationManager *p, EOrop *rop, eOipv4addr_t toipaddr);
                                                   
                                                   
extern eOresult_t eo_confman_Confirmation_Received(EOconfirmationManager *p, EOrop *rop, eOipv4addr_t fromipaddr);

                                                   





/** @}            
    end of group eo_confman  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

