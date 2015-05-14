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
#ifndef _EOCONSTLOOKUPTBL_H_
#define _EOCONSTLOOKUPTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EOconstLookupTbl.h
    @brief      This header file implements public interface to a constLookupTbl object
    @author     valentina.gaggero@iit.it
    @date       04/04/2012
**/

/** @defgroup eo_constLookupTbl Object EOconstLookupTbl
    This object implements a lookup table. Since this object has been created with the aim 
    of reduece retriving time of an item in list, here function for object manipulation are not provided.
    The only provided functions, let the user to access to object's fields.
    In many case we have a set of items to acess to, but usually they can not be enumerated starting from 0, but
    starting from a particular value, said "offset".

    Moreover, sometimes is difficult to enumerate items sequentially and this involes to allocate arry with holes,
    hence waste of memory. For this the object let the user to memorized a funcion to mange "excepion items", 
    called "exceptionMngFn".
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/**	@typedef    typedef struct EOconstLookupTbl_hid EOconstLookupTbl
 	@brief 		EOconstLookupTbl is an opaque struct. It is used to implement data abstraction for the constLookupTbl
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOconstLookupTbl_hid EOconstLookupTbl;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


 
/** @fn         extern EOconstLookupTbl * eo_constLookupTbl_New(eOsizecntnr_t capacity, uint16_t offset, 
                                                                eOres_fp_voidp_t exceptionMngFn, 
                                                                const void* itemsList);
    @brief      Creates a new EOconstLookupTbl object. The table's items must be already allocated like const vector.
    @param      capacity            Number of item objects stored by the itemList.
    @param      offset              The index to start to count the items.
    @param      exceptionMngFn      Function can be used to manage items that can not be stored in itemList.
    @param      itemsList           Item list alreday allocated in ROM.
    @return     Pointer to the required EOconstLookupTbl object. The pointer can be NULL if itemsList is NULL.
 **/
extern EOconstLookupTbl * eo_constLookupTbl_New(eOsizecntnr_t capacity, uint16_t offset, eOres_fp_voidp_t exceptionMngFn, const void* itemsList);


/** @fn         extern eOsizecntnr_t eo_constLookupTbl_GetCapacity(EOconstLookupTbl *p)
    @brief      Returns itemsList's capacity
    @param      p           Pointer to the EOconstLookupTbl object. 
    @return     Number of item in the lookup table.
 **/
extern eOsizecntnr_t eo_constLookupTbl_GetCapacity(EOconstLookupTbl *p);


/** @fn         extern uint16_t eo_constLookupTbl_GetOffset(EOconstLookupTbl *p)
    @brief      Returns the offset
    @param      p           Pointer to the EOconstLookupTbl object. 
    @return     the offset (see above).
 **/
extern uint16_t eo_constLookupTbl_GetOffset(EOconstLookupTbl *p);


/** @fn         extern const void * eo_constLookupTbl_GetItemsList(EOconstLookupTbl *p)
    @brief      Retrieves a reference to the item List
    @param      p           Pointer to the EOconstLookupTbl object. 
    @return     Pointer to the item List 
 **/
extern const void * eo_constLookupTbl_GetItemsList(EOconstLookupTbl *p);



/** @fn         extern eOres_fp_voidp_t eo_constLookupTbl_GetExceptionMngFn(EOconstLookupTbl *p)
    @brief      Returns the reference to function that manages exception items. 
    @param      p           Pointer to the EOconstLookupTbl object. 
    @return     the exceptionMngFn (see above).
 **/
extern eOres_fp_voidp_t eo_constLookupTbl_GetExceptionMngFn(EOconstLookupTbl *p);


/** @}            
    end of group eo_constvector  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


