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
#ifndef _EOMAPPMOTORCONTROLLER_HID_H_
#define _EOMAPPMOTORCONTROLLER_HID_H_

// - doxy -------------------------------------------------------------------------------------------------------------

/* @file       EOMappMotorController_hid.h
    @brief      This header file implements hidden interface to an entity.
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EOMtask.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOMappMotorController.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     Object_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
typedef enum
{
    eOm_appMotorController_st__idle                     = 0,
    eOm_appMotorController_st__active                   = 1,
//    eOm_appMotorController_st__startCmdRec              = 2,
//    eOm_appMotorController_st__dataReady                = 3,
    eOm_appMotorController_st__calculating              = 2
} eOm_appMotorController_states_hid_t;


 
struct EOMappMotorController_hid
{
    EOMappMotorController_cfg_t         cfg;        /**< cfg of obj */
    EOMtask                             *mytask;    /**< pointer to my task */
    eOm_appMotorController_states_hid_t st;
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



