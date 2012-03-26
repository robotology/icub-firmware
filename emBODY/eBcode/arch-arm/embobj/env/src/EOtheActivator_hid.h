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
#ifndef _EOTHEACTIVATOR_HID_H_
#define _EOTHEACTIVATOR_HID_H_


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoAction_hid.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheActivator.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

struct EOtheActivator_hid 
{
    EOvectorMutex           *vector;
    eOreltime_t             tout;
    EOaction                act;
    eOsizecntnr_t           onnumitems;
    eObool_t                activated;   
}; 


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


