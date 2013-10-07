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
#ifndef _EOMTHEENTITIESENV_HID_H_
#define _EOMTHEENTITIESENV_HID_H_

#ifdef __cplusplus
extern "C" {
#endif
// - doxy -------------------------------------------------------------------------------------------------------------

/* @file       EOMtheEntitiesEnv_hid.h
    @brief      This header file implements hidden interface to entity environment.
    @author     valentina.gaggero@iit.it
    @date       12/11/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EOVmutex.h"
#include "EOvectorMutex.h"
#include "EOaction.h"
#include "EOtimer.h"
#include "EOMtask.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOMtheEntitiesEnv.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     Object_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOMtheEntitiesEnv_hid
{
//    EOMtask                 *sysController_ptr;     /**< pointer to singleto task, the manager of this object.*/
    EOvectorMutex           *task_list;             /**< pointer to task list */
    EOvectorMutex           *eotimer_list;          /**< pointer to eotimer list */
    EOvectorMutex           *haltimer_list;         /**< pointer to hal-timer list */
    EOvectorMutex           *module_list;           /**< pointer to module list */
    EOMtheEntitiesEnv_systemControllercfg_t sysController_stuff;
//    void (*callback_allEntities_registered)(void);
//    void (*callback_etity_regitered)(EOMtheEntitiesEnv_type_t type, void *arg);
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



