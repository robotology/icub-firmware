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


/* @file       eOcfg_icubCanProto_motorBoardMsgfunction.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       04/04/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOconstLookupTbl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOconstLookupTbl_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOconstLookupTbl";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOconstLookupTbl * eo_constLookupTbl_New(eOsizecntnr_t capacity, uint16_t offset , eOres_fp_voidp_t exceptionMngFn, const void* itemsList)
{
    EOconstLookupTbl *retptr = NULL;
 
    if(NULL == itemsList)
    {
        return(retptr);
    }

    // i get the memory for the object. no need to check versus NULL because the memory pool already does it
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOconstLookupTbl), 1);


    // now the obj has valid memory. i need to initialise it with user-defined data,

    eo_errman_Assert(eo_errman_GetHandle(), (0 != capacity), s_eobj_ownname, "capacity is zero");

    retptr->capacity           = capacity;
    retptr->offset             = offset;
    retptr->exceptionMngFn     = exceptionMngFn;
    retptr->itemsList          = itemsList;     
    
    return(retptr);   
}


extern eOsizecntnr_t eo_constLookupTbl_GetCapacity(EOconstLookupTbl *p) 
{
    if(NULL == p) 
    {
        // invalid p
        return(0);    
    }
    
    return(p->capacity);        
}

extern eOsizecntnr_t eo_constLookupTbl_GetOffset(EOconstLookupTbl *p) 
{
    if(NULL == p) 
    {
        // invalid p
        return(0);    
    }
    
    return(p->offset);        
}


extern const void * eo_constLookupTbl_GetItemsList(EOconstLookupTbl *p) 
{
   
    if(NULL == p) 
    {
        return(NULL);    
    }
        
    return(p->itemsList);         
}


extern eOres_fp_voidp_t  eo_constLookupTbl_GetExceptionMngFn(EOconstLookupTbl *p)
{
    if(NULL == p) 
    {
        return(NULL);    
    }
        
    return(p->exceptionMngFn);         

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



