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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "osal_semaphore.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSharedHW.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSharedHW_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const eOsharedhw_cfg_t eo_sharedhw_DefaultCfg = { 0 };

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

static EOtheSharedHW s_eo_thesharedhw = 
{
    EO_INIT(.semaphores)            {NULL},
    EO_INIT(.initted)               eobool_false
};

//static const char s_eobj_ownname[] = "EOtheSharedHW";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheSharedHW* eo_sharedhw_Initialise(const eOsharedhw_cfg_t *cfg)
{
    if(eobool_true == s_eo_thesharedhw.initted)
    {
        return(&s_eo_thesharedhw);
    }
    
    if(NULL == cfg)
    {
        cfg = &eo_sharedhw_DefaultCfg;       
    }
    
    // create all synch data (semaphore, results, etc)
    uint8_t i=0;
    for(i=0; i<eosharedhw_resources_numberof; i++)
    {
         s_eo_thesharedhw.semaphores[i] = osal_semaphore_new(1, 1); 
    }
    
    
    s_eo_thesharedhw.initted = eobool_true;
    
    return(&s_eo_thesharedhw);   
}


extern EOtheSharedHW* eo_sharedhw_GetHandle(void)
{
    if(eobool_true == s_eo_thesharedhw.initted)
    {
        return(&s_eo_thesharedhw);
    }
    return(NULL);
}

    
extern eOresult_t eo_sharedhw_Obtain(EOtheSharedHW *p, eOsharedhw_resource_t res, eOreltime_t timeout) 
{
    if(NULL == p)
    {
        return(eores_NOK_generic);
    }    
    
    if(res >= eosharedhw_resources_numberof)
    {
        return(eores_NOK_generic);
    }

    osal_result_t r = osal_semaphore_decrement(p->semaphores[res], timeout);
        
    return((eOresult_t)r);
}


extern eOresult_t eo_sharedhw_Release(EOtheSharedHW *p, eOsharedhw_resource_t res) 
{
    if(NULL == p)
    {
        return(eores_NOK_generic);
    }    
    
    if(res >= eosharedhw_resources_numberof)
    {
        return(eores_NOK_generic);
    }

    osal_result_t r = osal_semaphore_increment(p->semaphores[res], osal_callerAUTOdetect);
        
    return((eOresult_t)r);
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



