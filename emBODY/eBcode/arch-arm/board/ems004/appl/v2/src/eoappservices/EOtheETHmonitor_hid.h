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

#ifndef _EOTHEETHMONITOR_HID_H_
#define _EOTHEETHMONITOR_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"
#include "osal_semaphore.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheETHmonitor.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


struct EOtheETHmonitor_hid
{
    eObool_t            initted;
    eObool_t            started;
    eObool_t            newresultsavailable;
    EOMtask*            taskworker;
    osal_semaphore_t*   semaphoreworker;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

void eo_ethmonitor(void *p);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



