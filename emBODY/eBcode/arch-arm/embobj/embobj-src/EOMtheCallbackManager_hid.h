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
#ifndef _EOMTHECALLBACKMANAGER_HID_H_
#define _EOMTHECALLBACKMANAGER_HID_H_


/* @file       EOMtheCallbackManager_hid.h
    @brief      This header file implements hidden interface to the singleton.
    @author     marco.accame@iit.it
    @date       08/03/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheCallbackManager.h"
#include "EOVtheCallbackManager_hid.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


/** @struct     EOMtheCallbackManager_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOMtheCallbackManager_hid 
{ 
    EOVtheCallbackManager   *vcm;
    EOMtask                 *tsk;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

void sys_cbackman(void *p);


#endif  // include guard




