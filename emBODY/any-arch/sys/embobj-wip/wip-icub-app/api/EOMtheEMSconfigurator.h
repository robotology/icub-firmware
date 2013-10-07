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
#ifndef _EOMTHEEMSCONFIGURATOR_H_
#define _EOMTHEEMSCONFIGURATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSconfigurator.h
	@brief      This header file implements public interface to the EMS configurator singleton.
 	@author     marco.accame@iit.it
	@date       05/21/2012
 **/

/** @defgroup eom_EOMtheEMSconfigurator Singleton EOMtheEMSconfigurator 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSconfigurator_hid EOMtheEMSconfigurator
    @brief      EOMtheEMSconfigurator is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMSconfigurator_hid EOMtheEMSconfigurator;



/**	@typedef    typedef struct eOemsconfigurator_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMSconfigurator. 
 **/
typedef struct
{
     uint8_t        taskpriority;
     uint16_t       taskstacksize;   
} eOemsconfigurator_cfg_t;




typedef enum
{
    emsconfigurator_evt_go2runner      =   0x00000002,
    emsconfigurator_evt_userdef        =   0x00000004,
    emsconfigurator_evt_ropframeTx     =   0x00000008           //the configurator sends a ropframe by transceiver
} eOemsconfigurator_event_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOemsconfigurator_cfg_t eom_emsconfigurator_DefaultCfg; // = {.taskpriority = 50, .taskstacksize = 1024};


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSconfigurator * eom_emsconfigurator_Initialise(const eOemsconfigurator_cfg_t *emsconfiguratorcfg)
    @brief      Initialise the EOMtheEMSconfigurator singleton 
    @arg        emsconfiguratorcfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMSconfigurator
 **/
extern EOMtheEMSconfigurator * eom_emsconfigurator_Initialise(const eOemsconfigurator_cfg_t *emsconfiguratorcfg);


/** @fn         extern EOMtheEMSconfigurator * eom_emsconfigurator_GetHandle(void)
    @brief      Retrieve the EOMtheEMSconfigurator singleton 
    @return     The handle to the EOMtheEMSconfigurator
 **/
extern EOMtheEMSconfigurator * eom_emsconfigurator_GetHandle(void);




extern EOMtask * eom_emsconfigurator_GetTask(EOMtheEMSconfigurator *p);





/** @}            
    end of group eom_EOMtheEMSconfigurator  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

