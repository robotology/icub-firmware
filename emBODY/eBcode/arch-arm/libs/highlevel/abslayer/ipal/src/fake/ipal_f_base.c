/*
 * Copyright (C) 2014 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       ipal_f_base.c
	@brief      This file implements internal implementation of the fake ipal base module
	@author     marco.accame@iit.it
    @date       11/03/2014
**/


#ifdef IPAL_USE_BASE


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_f_base_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

ipal_cfg_t ipal_base_hid_cfg;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern uint32_t ipal_base_memory_getsize(const ipal_cfg_t *cfg, uint32_t *size04aligned)
{   
    return(0);
}
                                    

extern ipal_result_t ipal_base_initialise(const ipal_cfg_t *cfg, uint32_t *data04aligned)
{ 

    if(NULL == cfg)
    {
        ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise(): missing cfg");
        return(ipal_res_NOK_generic);
    }

    // set the usr_on_fatal_error asap ... i do it even if cfg->extfn.usr_on_fatal_error is NULL.

    ipal_base_hid_cfg.extfn.usr_on_fatal_error = cfg->extfn.usr_on_fatal_error;
        

//    if(1 == ipal_sys_hid_started)
//    {
//        ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_base_initialise(): already started");
//        return(ipal_res_NOK_generic);
//    }
    
    
    // ... sets used config: ipal_base_hid_cfg
    memcpy(&ipal_base_hid_cfg, cfg, sizeof(ipal_base_hid_cfg));
    
    
    return(ipal_res_OK);
}


extern uint32_t ipal_memory_getsize(const ipal_cfg_t *cfg, uint32_t *size04aligned)
{
    return(ipal_base_memory_getsize(cfg, size04aligned));
}


extern ipal_result_t ipal_initialise(const ipal_cfg_t *cfg, uint32_t *data04aligned)
{
    return(ipal_base_initialise(cfg, data04aligned));
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



extern void ipal_base_hid_on_fatalerror(ipal_fatalerror_t errorcode, const char * errormsg)
{
 
    if(NULL != ipal_base_hid_cfg.extfn.usr_on_fatal_error)
    {
        ipal_base_hid_cfg.extfn.usr_on_fatal_error(errorcode, errormsg);
    }
    else
    {
        for(;;)
        {
            errorcode = errorcode;
        }
    }
}


extern ipal_ipv4addr_t ipal_base_hid_addr_array_to_ipv4(ipal_arrayaddr_t iparray)
{
    return(*((uint32_t*)iparray));
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



#endif//IPAL_USE_BASE

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




