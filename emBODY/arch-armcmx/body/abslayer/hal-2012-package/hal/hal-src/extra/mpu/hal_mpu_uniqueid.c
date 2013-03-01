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

#ifdef HAL_USE_UNIQUEID


#if     defined(HAL_USE_CPU_FAM_STM32F1)
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
#warning WIP --> verify that the uniqueid peripheral is the same in stm32f1 and stm32f4
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"
#include "stdlib.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"
#include "hal_utility_bits.h" 

#include "hal_middleware_interface.h" 

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_uniqueid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mpu_uniqueid_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_uiniqueid2index(t)              ((uint8_t)((t)))


#if     defined(HAL_USE_CPU_FAM_STM32F1)
    #define HAL_MPU_UNIQUEID_UniqueDeviceID96_baseaddress 0x1FFFF7E8
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
    #define HAL_MPU_UNIQUEID_UniqueDeviceID96_baseaddress 0x1FFF7A10
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 

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
// empty-section

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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_uniqueid_init(const hal_uniqueid_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic;

//    if(hal_false == s_hal_uniqueid_supported_is(uniqueid))
//    {
//        return(hal_res_NOK_unsupported);
//    }

     
//    if(NULL == cfg)
//    {
//        cfg = &hal_uniqueid_cfg_default;
//    }

    
    // nothing to configure
    
    res = hal_res_OK;
   
//    if(hal_res_OK == res)
//    {
//        s_hal_uniqueid_initted_set(uniqueid);
//    }

    return(res);
}


extern hal_uniqueid_id64bit_t hal_uniqueid_id64bit_get(void)
{
     if(hal_false == s_hal_uniqueid_supported_is(hal_uniqueid_id64bit))
    {
        return(hal_NA64);
    } 
    
    uint64_t val = *((const uint64_t *) (HAL_MPU_UNIQUEID_UniqueDeviceID96_baseaddress));
    val += *((const uint32_t *) (HAL_MPU_UNIQUEID_UniqueDeviceID96_baseaddress+8));
    if((0xFFFFFFFFFFFFFFFF+0xFFFFFFFF) == val)
    {   // 0xFFFFFFFE (=0xFFFFFFFFFFFFFFFF+0xFFFFFFFF) is the result when all the 96 bits have value 1 (some old versions of stm32f107 have such a hw bug)
        return(hal_NA64);
    }
    else
    {
        return(val);
    }    

}


extern hal_uniqueid_id64bit_t hal_uniqueid_macaddr_get(void)
{
     if(hal_false == s_hal_uniqueid_supported_is(hal_uniqueid_macaddr))
    {
        return(hal_NA64);
    } 
    

#if defined(HAL_BOARD_EMS004)
    #warning WIP --> must define the mac retrieval for ems004
    return(hal_NA64);
#else
    return(hal_NA64);
#endif    

}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern hal_result_t hal_uniqueid_hid_static_memory_init(void)
{
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_uniqueid_supported_is(hal_uniqueid_hid_id_t uniqueid)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_uniqueid__theconfig.supported_mask, HAL_uiniqueid2index(uniqueid)));
}





#endif//HAL_USE_UNIQUEID

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



