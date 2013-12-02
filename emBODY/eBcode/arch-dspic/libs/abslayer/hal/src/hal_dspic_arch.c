/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
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

/* @file       hal_dspic_arch.c
    @brief      This file keeps internal implementation of parts of hal specifics of architecture.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/16/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "hal_dspic_base_hid.h"

//#include "extra/crc16.h"

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_arch_dspic.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_dspic_arch_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

//const hal_arch_dspic_crc16_polynomial_t hal_arch_dspic_crc16_polynomial_ccitt = 0x1021;

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
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions 
// --------------------------------------------------------------------------------------------------------------------

extern void hal_arch_dspic_delay(hal_time_t t)
{
    uint32_t ncycles = 40*t;    // actually ... ncycles = (cpu_freq/1000000)*usec
    __delay32(ncycles);
}

//extern hal_result_t hal_arch_dspic_crc16_init(hal_arch_dspic_crc16_polynomial_t poly)
//{
//    // so far we always use the ccitt-16
//    return(hal_res_OK);
//}

//extern uint16_t hal_arch_dspic_crc16_compute(const void *buf, uint32_t len)
//{
//    #warning HAL says: in hal_arch_dspic_crc16_compute() i use a sw implementation and not yet a hw one
//    return(crc16_ccitt(buf, len));
//}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern uint32_t hal_arch_dspic_hid_getsize(const hal_cfg_t *cfg)
{   
    // no memory needed
    return(0);
}

extern hal_result_t hal_arch_dspic_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }
    return(hal_res_OK); 
}


#ifdef HAL_DONT_INLINE
extern uint32_t hal_arch_dspic_convert_halflash_address_to_dspicflash_address(uint32_t addr)
{   // in libpic30.h: typedef unsigned long _prog_addressT; thus i use a uint32_t
    //return(addr/2);
    return(addr);
}

extern uint32_t hal_arch_dspic_convert_ptr_to_halflash_address(void *ptr)
{   // in libpic30.h: typedef unsigned long _prog_addressT; thus i use a uint32_t
    return(2L*(uint32_t)((uint16_t)ptr));
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



