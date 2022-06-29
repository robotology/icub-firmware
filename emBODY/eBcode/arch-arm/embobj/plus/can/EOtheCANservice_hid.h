/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOTHECANSERVICE_HID_H_
#define _EOTHECANSERVICE_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOtheCANservice_hid.h
    @brief      This header file implements hidden interface to the can protocol 
    @author     marco.accame@iit.it
    @date       12 mar 2015
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#if !defined(EMBOBJ_USE_EMBOT)
#include "osal.h"
#else
//#include "embot_os_rtos.h"
#endif // #if !defined(EMBOBJ_USE_EMBOT)


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheCANservice.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    eObool_t                        waitenable;
    uint8_t                         numoftxframe2send;
#if !defined(EMBOBJ_USE_EMBOT)    
    osal_semaphore_t*               semaphore;
#else
    void*                           semaphore;
#endif    
} eOcanserv_txwait_t;



typedef struct
{
    uint8_t                         totaltxframes;
    volatile uint8_t                numoftxframes;
#if !defined(EMBOBJ_USE_EMBOT)     
    osal_semaphore_t*               locksemaphore;
#else
    void*                           locksemaphore;
#endif     
} eOcanserv_lockdata_t;



/** @struct     EOtheCANservice_hid
    @brief      Hidden definition. 
 **/  
struct EOtheCANservice_hid 
{    
    eObool_t                initted;
    eObool_t                isactive[eOcanports_number];
	eOcanserv_cfg_t         config;
    eOcanserv_lockdata_t    locktilltxall[eOcanports_number];
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

