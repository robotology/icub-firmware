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
#ifndef _EONTHETIMERMANAGER_H_
#define _EONTHETIMERMANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EONtheTimerManager.h
    @brief      This header file implements public interface to the NEE timer manager singleton.
    @author     marco.accame@iit.it
    @date       11/21/2013
**/

/** @defgroup eon_thetimermanager Object EONtheTimerManager
    The EONtheTimerManager is derived from EOVtheTimerManager and manages EOtimer objects in the nee.
    It does ... nothing
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
 

/** @typedef    typedef struct EONtheTimerManager_hid EONtheTimerManager
    @brief      EONtheTimerManager is an opaque struct. It is used to implement data abstraction for the nee 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EONtheTimerManager_hid EONtheTimerManager;


   
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EONtheTimerManager * eon_timerman_Initialise(void)
    @brief      Initialises the singleton EONtheTimerManager.
    @return     The handle to the timer manager.
 **/
extern EONtheTimerManager * eon_timerman_Initialise(void); 


/** @fn         extern EONtheTimerManager* eon_timerman_GetHandle(void)
    @brief      Returns an handle to the singleton EONtheTimerManager. The singleton must have been initialised
                with eon_timerman_Initialise(), otherwise this function call will return NULL.
    @return     The handle to the timer manager (or NULL upon in-initialised singleton)
    @warning    This function is of no utility until we don't have public functions which use the handle. However,
                we keep it for future use.
 **/
extern EONtheTimerManager* eon_timerman_GetHandle(void);





/** @}            
    end of group eon_thetimermanager  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

