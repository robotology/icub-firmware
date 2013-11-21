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
#ifndef _EONTASK_H_
#define _EONTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EONtask.h
    @brief      This header file implements public interface to a task for NEE
    @author     marco.accame@iit.it
    @date       11/21/2013
**/

/** @defgroup eon_task Object EONtask
    The EONtask object implements a task in the neutral-tasking execution environment (NEE). 
    It does nothing. Its use is twofold: (1) for example of how to make a object for a specific execution environment,  
    and (2) for use in a context where the functionality is not needed (even if a NULL EOVtaskDerived pointer can be 
    used instead).
    
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOVtask.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
 

/** @typedef    typedef struct EONtask_hid EONtask
    @brief      EONtask is an opaque struct. It is used to implement data abstraction for the nee 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EONtask_hid EONtask;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EONtask * eon_task_New(void)
    @brief      Creates a new neutral-tasking task object. 
    @return     The wanted object. Never NULL.
 **/ 
extern EONtask * eon_task_New(void);


/** @}            
    end of group eon_task  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


