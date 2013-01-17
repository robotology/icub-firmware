/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       hal_utility_heap.c
    @brief      this file implements internals of a tool module used in hal
    @author     marco.accame@iit.it
    @date       01/17/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_UTILITY_HEAP

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_base_hid.h"

//#include "hal_stm32xx_include.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_utility_heap.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_utility_heap_hid.h" 


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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void* hal_utility_heap_new(uint32_t size)
{
    void* p = calloc(size, 1);
    if(NULL == p)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_utility_heap_new(): no heap anymore");
    }
    
    return(p);
}

extern void hal_utility_heap_delete(void** pp)
{
    uint32_t** p = (uint32_t**)pp;
    if(NULL != *p)
    {
        free(*p);
        *p = NULL;
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern uint32_t hal_utility_heap_hid_getsize(const hal_cfg_t *cfg)
{   
    // no memory needed
    return(0);
}

extern hal_result_t hal_utility_heap_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }
    return(hal_res_OK); 
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



#endif//HAL_USE_UTILITY_HEAP

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



