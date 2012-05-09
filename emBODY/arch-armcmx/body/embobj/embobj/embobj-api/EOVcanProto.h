/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _EOVCANPROTO_H_
#define _EOVCANPROTO_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOVCanProto.h
	@brief      This header file gives 
	@author     valentina.gagger@iit.it
	@date       04/06/2012
**/

/** @defgroup eov_CanProto Object EoicubCanProto
    define icub can protocol........
   
    @{		
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
 

/** @typedef 	struct EOVcanProto_hid EOVcanProto
    @brief      EOVcanProto is an opaque struct. It is used to implement data abstraction for the CAN protocol obj 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOVcanProto_hid EOVcanProto;


/** @typedef     void 		EOVcanProtoDerived
    @brief      EOVmutexDerived is used to implement polymorphism in the objects derived from EoMutex
 **/
typedef void 		EOVcanProtoDerived;

typedef uint32_t 	eo_canProto_msgCommand_t;

typedef uint32_t        eo_canProto_msgDestination_t;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 

/** @fn         extern eOpurevirtual eOresult_t eov_mutex_Take(EOVmutexDerived *d, eOreltime_t tout)
    @brief      Waits until the mutex is either taken or the timeout has expired
    @param      d               Pointer to the mutex-derived object
    @param      tout           Timeout in micro-seconds. for no-wait or infinite wait use proper values.
    @return     eores_OK in case of success. eores_NOK_timeout upon failure to take the mutex, or 
                or eores_NOK_nullpointer if mutex is NULL.
    @warning    This function cannot be used with a EOVmutex object but only with one object derived
                from it.
 **/
extern eOpurevirtual eOresult_t eov_canProto_ParseCanFrame(EOVcanProtoDerived *d, eOcanframe_t *frame, eOcanport_t canPortRx);


/** @fn         extern eOpurevirtual eOresult_t eov_mutex_Release(EOVmutexDerived *mutex)
    @brief      Releases the mutex. 
    @param      d               Pointer to the mutex-derived object
    @return     eores_OK in case of success. osal_res_NOK_generic upon failure to release the mutex, or 
                or eores_NOK_nullpointer if mutex is NULL.
    @warning    This function cannot be used with a EOVmutex object but only with one object derived
                from it.
 **/
extern eOpurevirtual eOresult_t eov_canProto_FormCanFrame(EOVcanProtoDerived *d, eo_canProto_msgCommand_t command, 
											eo_canProto_msgDestination_t dest, void *value,
											eOcanframe_t *frame);




/** @}            
    end of group eov_canProto  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



