/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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


/* @file       hal_display.c
	@brief      This file implements internal implementation of the hal flash module.
	@author     marco.accame@iit.it
    @date       06/09/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_FLASH

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "stm32f1.h"


#include "hal_stm32_base_hid.h" 

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_flash.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_flash_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
extern const uint32_t hal_flash_BASEADDR   = 0x08000000;
extern const uint32_t hal_flash_TOTALSIZE  = 256*1024;
extern const uint32_t hal_flash_PAGESIZE   = 2048;
extern const uint32_t hal_flash_UNITSIZE   = 2;



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_result_t s_hal_flash_erasepage(uint32_t addr);
static hal_result_t s_hal_flash_writedata(uint32_t addr, uint32_t size, uint32_t *data);
static hal_result_t s_hal_flash_writehalfword(uint32_t addr, uint16_t hword);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


//const char msgerr[] = "no need";



extern hal_result_t hal_flash_lock(void)
{
    FLASH_Lock();
    return(hal_res_OK);
}


extern hal_result_t hal_flash_unlock(void)
{
    FLASH_Unlock();
    return(hal_res_OK);
}


extern hal_result_t hal_flash_erase(uint32_t addr, uint32_t size)
{
    hal_result_t res = hal_res_NOK_generic;

    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_res_NOK_generic);
    }

    if(0 == size)
    {
        return(hal_res_NOK_generic);    
    }

    size += (addr % hal_flash_PAGESIZE);
    
    for( ; size > 0; )
    {
        res = s_hal_flash_erasepage(addr);

        if(hal_res_NOK_generic == res)
        {
            return(hal_res_NOK_generic);
        }

        addr += hal_flash_PAGESIZE;

        if(size >= hal_flash_PAGESIZE)
        {
            size -= hal_flash_PAGESIZE;
        }
        else
        {
            size = 0;
        }
    }
    
    return(hal_res_OK);     

}

extern hal_result_t hal_flash_write(uint32_t addr, uint32_t size, void *data)
{
    hal_result_t res = hal_res_NOK_generic;
    uint8_t addr4 = 0;
    uint8_t size4 = 0;
    uint8_t *dd = data;
 
    if((hal_false == hal_flash_address_isvalid(addr)) || (0 == size) || (NULL == data))
    {
        return(hal_res_NOK_generic);
    }


    if(0 == (addr&0x00000003))
    {
        addr4 = 1;
    }
    else if(0 == (addr&0x00000001))
    {
        addr4 = 0;
    }
    else
    {
        //hal_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "hal_flash_write() unsupports it");
        return(hal_res_NOK_generic);
    }


    if(0 == (size&0x00000003))
    {
        size4 = 1;
    }
    else if(0 == (size&0x00000001))
    {
        size4 = 0;
    }
    else
    {
        //hal_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "hal_flash_write() unsupports it");
        return(hal_res_NOK_generic);
    }



    if((1 == addr4) && (1 == size4))
    {
        // 4-bytes aligned address and size is multiple of 4: write by word
        return(s_hal_flash_writedata(addr, size, data));
    }
    else if((1 == addr4) && (0 == size4))
    {
        uint32_t ss = size - 2;
        // 4-bytes aligned address and size is multiple of 2: write all 4-bytes first and then the half word
        if(ss > 0)
        {
            // 4-bytes first
            res = s_hal_flash_writedata(addr, ss, (uint32_t*)dd);

            if(hal_res_NOK_generic == res)
            {
                return(hal_res_NOK_generic);
            }

            addr -= ss;
            dd += ss;
            size = 2;
        }
       
        return(s_hal_flash_writehalfword(addr, *((uint16_t*)dd)));
    }
    else
    {                                       
        // 2-byte aligned address ... write by half-word

        FLASH_Status status = FLASH_COMPLETE;
    
        for(;size>0; size -=2)
        {
            if(0xFFFF != *((volatile uint16_t*)addr))
            {
                return(hal_res_NOK_generic);
            }

	        // no need to write a 0xffff
	        if(0xFFFF == *((uint16_t*)dd))
	        {
		        status = FLASH_COMPLETE;
	        }
            else
            {
                status = FLASH_ProgramHalfWord(addr, *((uint16_t*)dd));
            }

            addr += 2;
            dd += 2;

            if(FLASH_COMPLETE != status)
            {
                return(hal_res_NOK_generic);
            }
        }
    
        return(hal_res_OK);
    }



    // never in here
    // return(hal_res_NOK_generic);
}


extern hal_result_t hal_flash_read(uint32_t addr, uint32_t size, void *data)
{
    const void *flashaddr = (const void*)addr;
 
    if((hal_false == hal_flash_address_isvalid(addr)) || (0 == size) || (NULL == data))
    {
        return(hal_res_NOK_generic);
    }
 
    memcpy(data, flashaddr, size);

    return(hal_res_OK);
}



extern uint32_t hal_flash_get_baseaddress(void)
{
    return(hal_flash_BASEADDR);
}



extern uint32_t hal_flash_get_size(void)
{
    return(hal_flash_TOTALSIZE);
}



extern hal_boolval_t hal_flash_address_isvalid(uint32_t addr)
{
    return( (0 == IS_FLASH_ADDRESS(addr)) ? (hal_false) : (hal_true) );
}


extern uint32_t hal_flash_get_pagesize(uint32_t addr)
{
    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_NA32);
    }
    
    return(hal_flash_PAGESIZE);
}

extern uint32_t hal_flash_get_pageaddr(uint32_t addr)
{
    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_NA32);
    }

    // every page is 2k
    addr >>= 11;
    addr <<= 11;
    return(addr);
}

extern uint32_t hal_flash_get_totalsize(void)
{
    return(hal_flash_TOTALSIZE);
}

extern uint32_t hal_flash_get_unitsize(uint32_t addr)
{
    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_NA32);
    }
    
    return(hal_flash_UNITSIZE);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_flash_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_flash_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }
    return(hal_res_OK); 
}


extern hal_result_t hal_flash_hid_setlatency(hal_flash_cycles_t ncycles)
{
    FLASH_SetLatency(ncycles);
//    hal_on_fatalerror(0, msgerr);
    return(hal_res_OK);
}
  

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




static hal_result_t s_hal_flash_erasepage(uint32_t addr)
{
    FLASH_Status status = FLASH_COMPLETE;

    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_res_NOK_generic);
    }

    // find beginning of page
    addr = hal_flash_get_pageaddr(addr);


    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    status = FLASH_ErasePage(addr);

    return((FLASH_COMPLETE==status)?(hal_res_OK):(hal_res_NOK_generic));
}



static hal_result_t s_hal_flash_writehalfword(uint32_t addr, uint16_t hword)
{
    FLASH_Status status = FLASH_COMPLETE;

    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_res_NOK_generic);
    }

    if(0 != (addr&0x00000001))
    {
        return(hal_res_NOK_generic);
    }

    // cannot write on non-0xffff flash
    if(0xFFFF != *((volatile uint16_t*)addr))
    {
        return(hal_res_NOK_generic);
    }

    // no need to write a 0xffff
    if(0xFFFF == hword)
    {
        return(hal_res_OK);
    }

    status = FLASH_ProgramHalfWord(addr, hword);

    return((FLASH_COMPLETE==status)?(hal_res_OK):(hal_res_NOK_generic));
}

#if 0
static hal_result_t s_hal_flash_writeword(uint32_t addr, uint32_t word)
{
    FLASH_Status status = FLASH_COMPLETE;

    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_res_NOK_generic);
    }

    if(0 != (addr&0x00000003))
    {
        return(hal_res_NOK_generic);
    }

    if(0xFFFFFFFF != *((volatile uint32_t*)addr))
    {
        return(hal_res_NOK_generic);
    }

    // no need to write a 0xffffffff
    if(0xFFFFFFFF == word)
    {
        return(hal_res_OK);
    }

    status = FLASH_ProgramWord(addr, word);

    return((FLASH_COMPLETE==status)?(hal_res_OK):(hal_res_NOK_generic));
}
#endif

static hal_result_t s_hal_flash_writedata(uint32_t addr, uint32_t size, uint32_t *data)
{

    volatile FLASH_Status status = FLASH_COMPLETE;
    uint32_t EndAddr = 0;

// --- we already verify the following conditions in the calling function.
 
//    if(hal_false == hal_flash_address_isvalid(addr))
//    {
//        return(hal_res_NOK_generic);
//    }
//
//    if(0 != (addr&0x00000003))
//    {
//        return(hal_res_NOK_generic);
//    }
    

    EndAddr = addr + size; 

    while((addr < EndAddr) && (status == FLASH_COMPLETE))
    {
        // cannot write on non-0xffffffff flash
        if(0xFFFFFFFF != *((volatile uint32_t*)addr))
        {
            return(hal_res_NOK_generic);
        }

        // no need to write if already 0xffffffff
        if(0xFFFFFFFF == *data)
        {
            status = FLASH_COMPLETE;
        }
        else
        {
            status = FLASH_ProgramWord(addr, *data);
        }

        addr = addr + 4;
        data++;
    }
    
    return((FLASH_COMPLETE == status) ? (hal_res_OK) :(hal_res_NOK_generic));
}




#endif//HAL_USE_FLASH

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



