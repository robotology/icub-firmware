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

/* @file       dspal_base.c
	@brief      This file implements internal implementation of the dspal module
	@author     marco.accame@iit.it
    @date       12/16/2010
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#warning    defined CM3, MATRIX_CHECH, ROUNDING

#include "dspal_base_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "dspal_base.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "dspal_base_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


extern dspal_cfg_t dspal_base_hid_cfg = {0};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern uint32_t dspal_base_memory_getsize(const dspal_cfg_t *cfg, uint32_t *size08aligned)
{
    if(NULL != size08aligned)
    {
        *size08aligned = 0;
    }

    return(0);
}


extern dspal_result_t dspal_base_initialise(const dspal_cfg_t *cfg, uint64_t *data08aligned)
{

    if(NULL == cfg)
    {
        dspal_base_hid_error(dspal_error_missingconfiguration, "dspal: NULL cfg in dspal_initialise()");
        return(dspal_res_NOK_generic);    
    }
    
    memcpy(&dspal_base_hid_cfg, cfg, sizeof(dspal_cfg_t));    
   

    if(dspal_dsplibtype_armcmsis != cfg->dsplibtype)
    {
        dspal_base_hid_error(dspal_error_unsupportedbehaviour, "dspal: unsupported library");
        return(dspal_res_NOK_generic);
    }

    return(dspal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t dspal_base_hid_getsize(const dspal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern dspal_result_t dspal_base_hid_setmem(const dspal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        dspal_base_hid_error(dspal_fatalerror_missingmemory, "dspal_xxx_hid_setmem(): memory missing");
//        return(dspal_res_NOK_generic);
//    }
    return(dspal_res_OK);  
}

extern void dspal_base_hid_error(dspal_errorcode_t err_code, const char *err_msg)
{
    if(NULL != dspal_base_hid_cfg.extfn.usr_on_fatal_error)
    {
        dspal_base_hid_cfg.extfn.usr_on_fatal_error(err_code, err_msg);
    }

    for(;;);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




