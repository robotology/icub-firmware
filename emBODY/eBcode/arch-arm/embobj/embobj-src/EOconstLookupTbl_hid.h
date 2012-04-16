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
#ifndef _EOCONSTLOOKUPTBL_HID_H_
#define _EOCONSTLOOKUPTBL_HID_H_


/** @file       EOconstLookupTbl_hid.h
    @brief      This header file implements hidden interface to a constLookupTbl object
    @author     valentina.gaggero@iit.it
    @date       04/04/2012
**/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOconstLookupTbl.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


/** @struct     EOconstLookupTbl_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOconstLookupTbl_hid 
{
    eOsizecntnr_t       capacity;           /**< max number of item contened in table */
    uint16_t            offset;             /**< Items in tbl are saved start form index 0, but if items are numereted from X( with X >0), 
                                            then is possible insert item from inde4x 0 with offset = X */
    eOres_fp_voidp_t    exceptionMngFn;     /**< if not NULL, this function ca be used to manage item which index is not in tbl */         
//    void *              exceptionMngFn_arg; /**< argument of exceptionMngFn. it can be used save a poiter to exception item list... TODO*/
    const void*         itemsList;        
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



