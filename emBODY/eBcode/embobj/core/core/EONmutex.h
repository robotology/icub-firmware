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
#ifndef _EONMUTEX_H_
#define _EONMUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EONmutex.h
    @brief      This header file implements public interface to a neutral execution environment mutex.
    @author     marco.accame@iit.it
    @date       11/21/2013
**/

/** @defgroup eon_mutex Object EONmutex
    The EONmutex is an object for the neutral execution environment derived from the abstract object EOVmutex.
    It does nothing. Its use is twofold: (1) for example of how to make a object for a specific execution environment,  
    and (2) for use in a context where the functionality is not needed (even if a NULL EOVmutexDerived pointer can be 
    used instead).

    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOVmutex.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
 

/** @typedef    typedef struct EONmutex_hid EONmutex
    @brief      EONmutex is an opaque struct. It is used to implement data abstraction for the nee 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EONmutex_hid EONmutex;


   
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EONmutex * eon_mutex_New(void)
    @brief      Creates a new EONmutex object by derivation from an abstract object EOVmutex. This mutex can be used
                in the neutral execution environment. 
    @return     The pointer to the required EONmutex. Never NULL.
 **/
extern EONmutex * eon_mutex_New(void);



/** @}            
    end of group eon_mutex  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




