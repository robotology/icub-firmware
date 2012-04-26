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
#ifndef _OSAL_INFO_H_
#define _OSAL_INFO_H_


/** @file       osal_info.h
	@brief      This header file implements public interface to the os abstraction layer: info
	@author     marco.accame@iit.it
	@date       12/05/2011
**/

/* @defgroup osal_info OS abstraction layer: info
    cxecevcevce
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "osal_base.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


 
// - declaration of public user-defined types ------------------------------------------------------------------------- 



/** @typedef    typedef enum osal_info_status_t 
    @brief      osal_info_status_t contains all possible values of status for OSAL
 **/ 
typedef enum
{
    osal_info_status_zero                = 0,    /**< nothing is done yet */   
    osal_info_status_initialised         = 1,    /**< called osal_system_initialise() with valid parameters but not osal_system_start() yet */
    osal_info_status_running             = 2,    /**< called osal_system_start() and scheduler is running */
    osal_info_status_suspended           = 3     /**< called osal_system_start() but scheduler is suspended */
} osal_info_status_t;


/** @typedef    typedef enum osal_info_entitytype_t 
    @brief      osal_info_entitytype_t contains all possible entities made available by OSAL which requires some RAM
 **/  
typedef enum 
{
    osal_info_entity_globalstack    = 0,
    osal_info_entity_task           = 1,
    osal_info_entity_timer          = 2,
    osal_info_entity_mutex          = 3,
    osal_info_entity_semaphore      = 4,
    osal_info_entity_messagequeue   = 5,
    osal_info_entity_message        = 6,     
} osal_info_entity_t;

enum { osal_info_entity_numberof = osal_info_entity_message+1 }; // put the max in osal_info_entity_t + 1

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern osal_info_status_t osal_info_get_status(void)
    @brief      Gets the status of the OSAL.
    @return     The status of OSAL
 **/
extern osal_info_status_t osal_info_get_status(void);


/** @fn         extern void osal_info_entities_get_stats(const uint16_t **used, const uint16_t **free)
    @brief      Gives back statistics about entities of OSAL which are in use or available.  
    @param      used       Gets back a pointer with used entities. It is to be indexed with osal_info_entity_t type.
    @param      free       Gets back a pointer with free entities. It is to be indexed with osal_info_entity_t type.
 **/
extern void osal_info_entities_get_stats(const uint16_t **used, const uint16_t **free);


/** @fn         extern osal_abstime_t osal_info_idletime_get(void)
    @brief      Returns total time of idle life in micro seconds.
    @return     Time of idle life in usec.
 **/
extern osal_abstime_t osal_info_idletime_get(void);


/* @}            
    end of group osal_info  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

