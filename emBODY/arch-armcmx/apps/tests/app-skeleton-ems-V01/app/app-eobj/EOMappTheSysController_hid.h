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
#ifndef _EOMAPPSYSCONTROLLER_HID_H_
#define _EOMAPPSYSCONTROLLER_HID_H_


// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOMappSysController.h
    @brief      his header file implements hidden interface to EOMappSysController obj
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EOMtask.h"
//application
#include "EOethBaseModule.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOMappTheSysController.h"

// - definition of the hidden struct implementing the object ----------------------------------------------------------
typedef enum
{
    eOm_appTheSysController_st__inited      = 0,
    eOm_appTheSysController_st__configured  = 1,
    eOm_appTheSysController_st__started     = 2,
    eOm_appTheSysController_st__stopped     = 3
} eOm_appTheSysController_states_hid_t;

struct EOMappTheSysController_hid
{
    EOMappTheSysController_cfg_t            cfg;              /**< configuration data */
    EOMtask                                 *mytask;          /**< pointer to my task */
    EOethBaseModule                         *eth_mod;         /**< module to manage send and rec eth datagram */
//    eObool_t            allObjRegistered_st;       /**< true if all obj of my application are registered*/
//    eObool_t            activateSysReqReceived_st; /**< true if i have received activate system request from pc104 */
//    eObool_t            systemIsActivate_st; NON MI SERVE PIU?      /**< true when i activate the system */
    eOm_appTheSysController_states_hid_t   st;                /**< status of the singleton */
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



