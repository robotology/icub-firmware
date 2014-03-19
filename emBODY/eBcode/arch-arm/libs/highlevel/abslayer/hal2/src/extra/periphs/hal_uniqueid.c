/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

/* @file       hal_uniqueid.c
	@brief      This file implements internal implementation of the hal uniqueid module.
	@author     marco.accame@iit.it
    @date       02/27/2013
**/


// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_PERIPH_UNIQUEID


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"
#include "stdlib.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"
#include "hl_bits.h" 

#include "hal_middleware_interface.h" 

 
#include "hl_sys.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_uniqueid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_uniqueid_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_uiniqueid2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_uniqueid_cfg_t hal_uniqueid_cfg_default =
{
    .dummy                  = 0
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint8_t         nothing;     
} hal_uiniqueid_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_uniqueid_supported_is(hal_uniqueid_hid_id_t uniqueid);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// static hal_uiniqueid_theinternals_t s_hal_uniqueid_theinternals =
// {
//     .nothing = 0
// };

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_uniqueid_init(const hal_uniqueid_cfg_t *cfg)
{
//    hal_result_t res = hal_res_NOK_generic;

//    if(hal_false == s_hal_uniqueid_supported_is(uniqueid))
//    {
//        return(hal_res_NOK_unsupported);
//    }

     
//    if(NULL == cfg)
//    {
//        cfg = &hal_uniqueid_cfg_default;
//    }

    
    // nothing to configure
    
//    res = hal_res_OK;
   
//    if(hal_res_OK == res)
//    {
//        s_hal_uniqueid_initted_set(uniqueid);
//    }

    return(hal_res_OK);
}


extern hal_uniqueid_id64bit_t hal_uniqueid_id64bit_get(void)
{
     if(hal_false == s_hal_uniqueid_supported_is(hal_uniqueid_id64bit))
    {
        return(hal_NA64);
    } 
    
    return(hl_sys_uniqueid64bit_get());
}

#if 0
extern hal_uniqueid_id64bit_t hal_uniqueid_macaddr_get(void)
{
}
#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_uniqueid_supported_is(hal_uniqueid_hid_id_t uniqueid)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(hal_brdcfg_uniqueid__theconfig.supported_mask, HAL_uiniqueid2index(uniqueid)));
}





#endif//HAL_USE_PERIPH_UNIQUEID

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



