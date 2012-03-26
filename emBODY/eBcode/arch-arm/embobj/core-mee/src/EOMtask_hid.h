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
#ifndef _EOMTASK_HID_H_
#define _EOMTASK_HID_H_


/* @file       EOMtask_hid.h
    @brief      This header file implements hidden interface to the rtos task object.
    @author     marco.accame@iit.it
    @date       08/03/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOVtask.h"
#include "osal.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtask.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


/** @struct     EOMtask_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOMtask_hid 
{ 
    // - base object
    EOVtask                 *tsk;

    // - other stuff
    osal_task_t             *osaltask;
    osal_messagequeue_t     *messagequeue;
    osal_messagequeue_t     *argumentqueue;     
    uint8_t                 maxmessages;
    uint8_t                 priority;
    eOmtaskType_t           type;
    eOvoid_fp_voidp_t       osalrun;
    eOreltime_t             ustime;
    eOevent_t               waitmask;
    void                    *extdata; 
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

// used to wait from inside the task
extern eOresult_t eom_task_hid_WaitMessage(EOMtask *const p, eOmessage_t *msg, eOreltime_t tout);
// used to wait from inside the task
extern eOresult_t eom_task_hid_WaitEvent(EOMtask *const p, eOevent_t msk, eOevent_t *rxmsk, eOreltime_t tout);

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




