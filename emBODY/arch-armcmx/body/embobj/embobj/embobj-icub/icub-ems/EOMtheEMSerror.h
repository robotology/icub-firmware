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
#ifndef _EOMTHEEMSERROR_H_
#define _EOMTHEEMSERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSerror.h
	@brief      This header file implements public interface to the EMS error singleton.
 	@author     marco.accame@iit.it
	@date       05/21/2012
 **/

/** @defgroup eom_EOMtheEMSerror Singleton EOMtheEMSerror 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSerror_hid EOMtheEMSerror
    @brief      EOMtheEMSerror is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMSerror_hid EOMtheEMSerror;



/**	@typedef    typedef struct eOemserror_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMSerror. 
 **/
typedef struct
{
     uint8_t        taskpriority;
     uint16_t       taskstacksize;   
} eOemserror_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOemserror_cfg_t eom_emserror_DefaultCfg; // = {.taskpriority = 40, .taskstacksize = 512};


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSerror * eom_emserror_Initialise(const eOemserror_cfg_t *emserrorcfg)
    @brief      Initialise the EOMtheEMSerror singleton 
    @arg        emserrorcfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMSerror
 **/
extern EOMtheEMSerror * eom_emserror_Initialise(const eOemserror_cfg_t *emserrorcfg);


/** @fn         extern EOMtheEMSerror * eom_emserror_GetHandle(void)
    @brief      Retrieve the EOMtheEMSerror singleton 
    @return     The handle to the EOMtheEMSerror
 **/
extern EOMtheEMSerror * eom_emserror_GetHandle(void);






/** @}            
    end of group eom_EOMtheEMSerror  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

