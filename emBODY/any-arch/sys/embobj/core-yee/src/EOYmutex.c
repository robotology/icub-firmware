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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOVmutex_hid.h"


#include <FeatureInterface.h>   // to see the acemutex_* functions


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOYmutex.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOYmutex_hid.h" 


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

// virtual
static eOresult_t s_eoy_mutex_take(void *p, eOreltime_t tout);
// virtual
static eOresult_t s_eoy_mutex_release(void *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOYmutex";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOYmutex* eoy_mutex_New(void) 
{
    EOYmutex *retptr = NULL;    

    // i get the memory for the yarp mutex object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOYmutex), 1);
    
    // i get the base mutex
    retptr->mutex = eov_mutex_hid_New();

    // init its vtable
    eov_mutex_hid_SetVTABLE(retptr->mutex, s_eoy_mutex_take, s_eoy_mutex_release); 
    
    // i get a new yarp mutex
    retptr->acemutex = ace_mutex_new();

    // need to check because yarp may return NULL
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != retptr->acemutex), s_eobj_ownname, "ace cannot give a mutex");
    
    return(retptr);    
}


extern eOresult_t eoy_mutex_Take(EOYmutex *m, eOreltime_t tout)
{
    if(NULL == m)
    {
        return(eores_NOK_nullpointer);
    }
    
    return(s_eoy_mutex_take(m, tout));
}


extern eOresult_t eoy_mutex_Release(EOYmutex *m)
{
    if(NULL == m)
    {
        return(eores_NOK_nullpointer);
    }
    
    return(s_eoy_mutex_release(m));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eoy_mutex_take(void *p, eOreltime_t tout) 
{
    EOYmutex *m = (EOYmutex *)p;
    // p it is never NULL because the base function calls checks it before calling this function, then ace will
    // check m->acemutex vs NULL
    return((eOresult_t)ace_mutex_take(m->acemutex, tout));
}


static eOresult_t s_eoy_mutex_release(void *p) 
{
    EOYmutex *m = (EOYmutex *)p;
    // p it is never NULL because the base function calls checks it before calling this function, then ace will
    // check m->acemutex vs NULL
    return((eOresult_t)ace_mutex_release(m->acemutex));
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



