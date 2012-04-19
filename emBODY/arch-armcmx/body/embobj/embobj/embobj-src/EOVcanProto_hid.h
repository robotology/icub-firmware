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
#ifndef _EOVCANPROTO_HID_H_
#define _EOVCANPROTO_HID_H_


/** @file       EOVcanProto_hid.h
    @brief      This header file gives 
	@author     valentina.gagger@iit.it
	@date       04/06/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOVcanProto.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#define VF00_parse                   0
#define VF01_form                    1
#define VTABLESIZE_canProto     2


// - definition of the hidden struct implementing the object ----------------------------------------------------------


/** @typedef     eo_icubCanProto_former_fn_t
    @brief       ptr to parser-function.
 **/
typedef     eOresult_t  (*eOres_fp_canProto_parserFn_t)     (EOVcanProtoDerived*, eOcanframe_t *, eOcanport_t);


/** @typedef     eo_icubCanProto_former_fn_t
    @brief       ptr to former-function.
 **/
typedef    eOresult_t  (*eOres_fp_canProto_formerFn_t)  (EOVcanProtoDerived* p, eo_canProto_msgCommand_t, eo_canProto_msgDestination_t, void*, eOcanframe_t*);


/** @struct     EOVmutex_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOVcanProto_hid 
{
    // - vtable: must be on top of the struct
    void * vtable[VTABLESIZE_canProto];

    // - other stuff
    //empty section
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------

 
/** @fn         extern EOVmutex* eov_canProto_hid_New(void)
    @brief      Creates a new mutex object 
    @return     Pointer to the required mutex object.
    @warning    The EOVmutex cannot be used by itself, but inside a derived object.
 **/
extern EOVcanProto* eov_canProto_hid_New(void);


/** @fn         extern eOresult_t eov_mutex_hid_SetVTABLE(EOVmutex *p, eOres_fp_voidp_uint32_t v_take, eOres_fp_voidp_t v_release)
    @brief      Specialise the virtual functions of the abstract object
    @param      p               The object
    @param      v_take          the first virtual function
    @param      v_release       the second virtual function        
    @return     eores_OK.
 **/
extern eOresult_t eov_canProto_hid_SetVTABLE(EOVcanProto *p, eOres_fp_canProto_parserFn_t v_parse, eOres_fp_canProto_formerFn_t v_form);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



