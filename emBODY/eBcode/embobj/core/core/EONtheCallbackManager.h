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
#ifndef _EONTHECALLBACKMANAGER_H_
#define _EONTHECALLBACKMANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EONtheCallbackManager.h
    @brief      This header file implements public interface to the NEE callback manager singleton.
    @author     marco.accame@iit.it
    @date       11/21/2013
**/

/** @defgroup eon_thecallbackmanager Object EONtheCallbackManager
    The EONtheCallbackManager ... does nothing.

    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
 

/** @typedef    typedef struct EONtheCallbackManager_hid EONtheCallbackManager
    @brief      EONtheCallbackManager is an opaque struct. 
 **/  
typedef struct EONtheCallbackManager_hid EONtheCallbackManager;


   
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EONtheCallbackManager * eon_callbackman_Initialise(void)
    @brief      Initialises the singleton EONtheCallbackManager.
                It does ... nothing 
    @return     The handle to the callback manager.
 **/
extern EONtheCallbackManager * eon_callbackman_Initialise(void); 


/** @fn         extern EONtheCallbackManager * eon_callbackman_GetHandle(void)
    @brief      Returns an handle to the singleton EONtheCallbackManager. The singleton must have been initialised
                with eon_callbackman_Initialise(), otherwise this function will return NULL.
    @return     The handle to the callback manager or NULL.
 **/
extern EONtheCallbackManager * eon_callbackman_GetHandle(void);



/** @fn         extern EONtask * eon_callbackman_GetTask(EONtheCallbackManager *p)
    @brief      Retrieves the working task of the EONtheCallbackManager
    @param      p               Pointer to the object
    @return     The pointer to the EONtask.
 **/
extern EONtask * eon_callbackman_GetTask(EONtheCallbackManager *p);


/** @}            
    end of group eon_thecallbackmanager  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



