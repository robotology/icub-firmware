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
#ifndef _EOTHENVS_HID_H_
#define _EOTHENVS_HID_H_


/* @file       EoTheNVs_hid.h
    @brief      This header file implements hidden interface to the NVs singleton.
    @author     marco.accame@iit.it
    @date       10/15/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheNVs.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section



// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOtheNVs_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOtheNVs_hid 
{
    EOVmutexDerived*    mutex;
    EOnvsCfg*           nvscfg;   
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

// used to load a cfg which is different from the one passed in _Initialise()
extern void eo_nvs_hid_Load(EOtheNVs *p, EOnvsCfg *c);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







