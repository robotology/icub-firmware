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
#ifndef _EOVMUTEX_HID_H_
#define _EOVMUTEX_HID_H_


/** @file       EOVmutex_hid.h
    @brief      This header file implements hidden interface to a mutex object.
    @author     marco.accame@iit.it
    @date       08/03/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOVmutex.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#define VF00_take                   0
#define VF01_release                1
#define VTABLESIZE_mutex            2


// - definition of the hidden struct implementing the object ----------------------------------------------------------


/** @struct     EOVmutex_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOVmutex_hid 
{
    // - vtable: must be on top of the struct
    void * vtable[VTABLESIZE_mutex];

    // - other stuff
    // empty-section
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------

 
/** @fn         extern EOVmutex* eov_mutex_hid_New(void)
    @brief      Creates a new mutex object 
    @return     Pointer to the required mutex object.
    @warning    The EOVmutex cannot be used by itself, but inside a derived object.
 **/
extern EOVmutex* eov_mutex_hid_New(void);


/** @fn         extern eOresult_t eov_mutex_hid_SetVTABLE(EOVmutex *p, eOres_fp_voidp_uint32_t v_take, eOres_fp_voidp_t v_release)
    @brief      Specialise the virtual functions of the abstract object
    @param      p               The object
    @param      v_take          the first virtual function
    @param      v_release       the second virtual function        
    @return     eores_OK.
 **/
extern eOresult_t eov_mutex_hid_SetVTABLE(EOVmutex *p, eOres_fp_voidp_uint32_t v_take, eOres_fp_voidp_t v_release);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



