/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EOTHESHAREDHW_HID_H_
#define _EOTHESHAREDHW_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "osal_semaphore.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheSharedHW.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


struct EOtheSharedHW_hid
{
    osal_semaphore_t*           semaphores[eosharedhw_resources_numberof];
    eObool_t                    initted;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

void eo_sharedhw(void *p);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



