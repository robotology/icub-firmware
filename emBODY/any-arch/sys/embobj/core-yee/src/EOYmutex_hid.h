/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Alberto Cardellino
 * email:   marco.accame@iit.it, alberto.cardellino@iit.it
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
#ifndef _EOYMUTEX_HID_H_
#define _EOYMUTEX_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOYmutex_hid.h
    @brief      This header file gives hidden interface to the yarp mutex object.
    @author     marco.accame@iit.it, alberto.cardellino@iit.it
    @date       11/20/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOVmutex.h"




// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOYmutex.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


/** @struct     EOYmutex_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOYmutex_hid 
{ 
    // - base object
    EOVmutex                *mutex;

    // - other stuff
    void                    *acemutex;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




