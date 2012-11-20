/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Alberto Cardellino
 * email:   marco.accame@iit.it, alberto.cardellino@iit.it
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
#ifndef _EOYMUTEX_H_
#define _EOYMUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOYmutex.h
    @brief      This header file implements public interface to a yarp mutex.
    @author     marco.accame@iit.it, alberto.cardellino@iit.it
    @date       11/20/2012
**/

/** @defgroup eoy_mutex Object EOYmutex
    The EOYmutex is an object for the YARP execution environment derived from the abstract object EOVmutex.
    It allows mutual exclusion in the YEE with priority inversion. The underlying mechanism
    is based on ... ADD_YARP_REF.  

    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
 

/** @typedef    typedef struct EOYmutex_hid EOYmutex
    @brief      EOYmutex is an opaque struct. It is used to implement data abstraction for the YARP 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOYmutex_hid EOYmutex;


   
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOYmutex * eoy_mutex_New(void)
    @brief      Creates a new EOYmutex object by derivation from an abstract object EOVmutex. This mutex is to be used
                in the YARP environment. The underlying mechanism is based on ... ADD_YARP_REF. 
    @return     The pointer to the required EOYmutex. Never NULL.
 **/
extern EOYmutex * eoy_mutex_New(void);


/** @fn         extern eOresult_t eoy_mutex_Take(EOYmutex *const m, eOreltime_t tout)
    @brief      It takes a YARP mutex with a given timeout. If the mutex is already taken by another task,
                the function waits its release according to the value of @e tout. The mutex has priority inversion.
    @param      m               The mutex
    @param      tout            The required timeout in micro-seconds. If eok_reltimeZERO the mutex does not wait
                                and if another task has already taken it, it returns immediately with a failure.
                                If eok_reltimeINFINITE the mutex waits indefinitely until the mutex is released.     
    @return     eores_OK in case of success. eores_NOK_timeout upon failure to take the mutex, or 
                or eores_NOK_nullpointer if mutex is NULL.
 **/
extern eOresult_t eoy_mutex_Take(EOYmutex *const m, eOreltime_t tout);


/** @fn         extern eOresult_t eoy_mutex_Release(EOYmutex *const m)
    @brief      It releases a YARP mutex previously taken by the same thread.
    @param      m               The mutex
    @return     eores_OK in case of success. osal_res_NOK_generic upon failure to release the mutex, or 
                or eores_NOK_nullpointer if mutex is NULL.
 **/
extern eOresult_t eoy_mutex_Release(EOYmutex *const m); 





/** @}            
    end of group eoy_mutex  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




