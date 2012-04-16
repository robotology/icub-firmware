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
#ifndef _EO_APPSKELETONEMS_BODY_H_
#define _EO_APPSKELETONEMS_BODY_H_


/** @file       eo_appSkeletonEms_body.h
    @brief      This header file implements public interface to application body
    @author     valentina.gaggero@iit.it
    @date       02/20/2012
**/

/** @defgroup eo_appSkeletonEms_body 
    this file contains the body/core of the application    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "ipal.h"
#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EOsocketDatagram.h"
#include "EOMtheIPnet.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
extern const ipal_cfg_t* eom_appSkeletonEms_body_ipal_cfg;

extern const eOerrman_cfg_t  eom_appSkeletonEms_body_errcfg;

extern const eOmipnet_cfg_dtgskt_t eom_appSkeletonEms_body_dtgskt_cfg;



// - declaration of extern public functions ---------------------------------------------------------------------------


 
/** @fn         extern eOresult_t eom_appSkeletonEms_body_services_init(void)
    @brief      Initializes all needed services. They are provided by object depended on application,
                but also by object of embObj framework.
    @return     eores_OK or eores_NOK_generic
 **/
extern eOresult_t eom_appSkeletonEms_body_services_init(void);


/** @fn         extern eOresult_t eom_appSkeletonEms_body_services_init(void)
    @brief      This function initialized all application's actors.
                An actor is somethis that can do some action.
    @return     eores_OK or eores_NOK_generic
 **/
extern eOresult_t eom_appSkeletonEms_body_actors_init(void);


/** @fn         extern eOresult_t eom_appSkeletonEms_body_services_init(void)
    @brief      This function initialized all application's actors.
                An actor is somethis that can do some action.
    @return     eores_OK or eores_NOK_generic
 **/
extern eOresult_t eom_appSkeletonEms_body_actors_start(void);


/** @}            
    end of group eo_appSkeletonEms_body  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


