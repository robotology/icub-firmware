/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       hal_flash.c
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

#include "hal_brdcfg.h"

#include "hal_middleware_interface.h"

#include "hal_base_hid.h" 

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_flash.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_flash_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_flash_id2index(t)              ((uint8_t)((t)))


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_flash_cfg_t hal_flash_cfg_default  = 
{ 
    .dummy = 0 
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


/** @typedef    typedef enum hal_flash_t 
    @brief      contains ids of every possible flash bank.
 **/ 
typedef enum  
{ 
    hal_flash1_internal = 0             /**< the only flash bank */
} hal_flash_t;

enum { hal_flashes_number = 1 };


typedef struct
{
    uint8_t                     nothing;
} hal_flash_internal_item_t;

typedef struct
{
    uint8_t                     initted;   
    hal_flash_internal_item_t   items[hal_flashes_number];   
} hal_flash_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_flash_initted_set(hal_flash_t id);
static hal_boolval_t s_hal_flash_initted_is(hal_flash_t id);


#if   defined(HAL_USE_MPU_TYPE_STM32F4)  
static uint32_t s_hal_flash_pageindex_get(uint32_t addr);
#endif

static uint32_t s_hal_flash_quickget_pagesize(uint32_t addr);

#if     defined(HAL_USE_MPU_TYPE_STM32F4)
static uint32_t s_hal_flash_stm32f4_sector_get(uint32_t addr);
#endif

static hal_result_t s_hal_flash_erasepage(uint32_t addr);
static hal_result_t s_hal_flash_writedata(uint32_t addr, uint32_t size, uint32_t *data);
static hal_result_t s_hal_flash_writehalfword(uint32_t addr, uint16_t hword);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

#define s_hal_flash_BASEADDR        (hal_brdcfg_flash__theconfig.baseaddress)
#define s_hal_flash_TOTALSIZE       (hal_brdcfg_flash__theconfig.totalsize)


#if     defined(HAL_USE_MPU_TYPE_STM32F1)

static const uint32_t s_hal_flash_PAGESIZE          = 2*1024;

#elif   defined(HAL_USE_MPU_TYPE_STM32F4)

static const uint32_t s_hal_flash_PAGESNUM          = 12;

static const uint32_t s_hal_flash_PAGESIZES[12]     = 
{
    16*1024,    
    16*1024,
    16*1024,
    16*1024,
    64*1024,
    128*1024,
    128*1024,
    128*1024,
    128*1024,
    128*1024,
    128*1024,
    128*1024
};

static const uint32_t s_hal_flash_PAGEADDRS[12]     = 
{
    0x08000000,         // s_hal_flash_BASEADDR
    0x08004000,         // s_hal_flash_BASEADDR +  16k
    0x08008000,         // s_hal_flash_BASEADDR +  32k
    0x0800C000,         // s_hal_flash_BASEADDR +  48k
    0x08010000,         // s_hal_flash_BASEADDR +  64k
    0x08020000,         // s_hal_flash_BASEADDR + 128k
    0x08040000,         // s_hal_flash_BASEADDR + 256k
    0x08060000,         // s_hal_flash_BASEADDR + 384k
    0x08080000,         // s_hal_flash_BASEADDR + 512k
    0x080A0000,         // s_hal_flash_BASEADDR + 640k
    0x080C0000,         // s_hal_flash_BASEADDR + 768k
    0x080E0000          // s_hal_flash_BASEADDR + 896k
};

#else //defined(HAL_USE_MPU_TYPE_*)
    #error ERR --> choose a HAL_USE_MPU_TYPE_*
#endif 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_flash_theinternals_t s_hal_flash_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_flash_init(const hal_flash_cfg_t *cfg)
{
    const hal_flash_t id = hal_flash1_internal;

    if(hal_true == s_hal_flash_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }    

    if(NULL == cfg)
    {
        cfg  = &hal_flash_cfg_default;
    }
    
    // i dont write code in here, but ... if intitem is NULL, then allocate it and do things
    
    // do thing if needed
    
    // finally ...
    s_hal_flash_initted_set(id);
    return(hal_res_OK); 
}


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

    uint32_t pagesize;

    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_res_NOK_generic);
    }

    if(0 == size)
    {
        return(hal_res_NOK_generic);    
    }

    pagesize = s_hal_flash_quickget_pagesize(addr);
    size += (addr % pagesize);
    
    
    for( ; size > 0; )
    {
        res = s_hal_flash_erasepage(addr);

        if(hal_res_NOK_generic == res)
        {
            return(hal_res_NOK_generic);
        }


        pagesize = s_hal_flash_quickget_pagesize(addr);
        addr += pagesize;

        if(size >= pagesize)
        {
            size -= pagesize;
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
    return(s_hal_flash_BASEADDR);
}



// extern uint32_t hal_flash_get_size(void)
// {
//     return(hal_flash_TOTALSIZE);
// }



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
    
    return(s_hal_flash_quickget_pagesize(addr));
}

extern uint32_t hal_flash_get_pageaddr(uint32_t addr)
{
    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_NA32);
    }

#if     defined(HAL_USE_MPU_TYPE_STM32F1)
    // every page is 2k
    addr >>= 11;
    addr <<= 11;
    return(addr);
#elif   defined(HAL_USE_MPU_TYPE_STM32F4)    
    uint32_t pageindex = s_hal_flash_pageindex_get(addr);   
    return(s_hal_flash_PAGEADDRS[pageindex]);
#else //defined(HAL_USE_MPU_TYPE_*)
    #error ERR --> choose a HAL_USE_MPU_TYPE_*
#endif    
}

extern uint32_t hal_flash_get_totalsize(void)
{
    return(s_hal_flash_TOTALSIZE);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------




  

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hal_flash_initted_set(hal_flash_t id)
{
    s_hal_flash_theinternals.initted = 1;
}

static hal_boolval_t s_hal_flash_initted_is(hal_flash_t id)
{
    return((0 == s_hal_flash_theinternals.initted) ? (hal_false) : (hal_true));
}


static hal_result_t s_hal_flash_erasepage(uint32_t addr)
{
    FLASH_Status status = FLASH_COMPLETE;

    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_res_NOK_generic);
    }
    
#if     defined(HAL_USE_MPU_TYPE_STM32F1)
    // find beginning of page
    addr = hal_flash_get_pageaddr(addr);
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    status = FLASH_ErasePage(addr);
#elif   defined(HAL_USE_MPU_TYPE_STM32F4)
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    // important: FLASH_EraseSector() DOES NOT use the pageindex, but its own values. 
    status = FLASH_EraseSector(s_hal_flash_stm32f4_sector_get(addr), VoltageRange_3);
#else //defined(HAL_USE_MPU_TYPE_*)
    #error ERR --> choose a HAL_USE_MPU_TYPE_*
#endif  

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


#if   defined(HAL_USE_MPU_TYPE_STM32F4)  
static uint32_t s_hal_flash_pageindex_get(uint32_t addr)
{
#if     defined(HAL_USE_MPU_TYPE_STM32F1)
    addr -= s_hal_flash_BASEADDR;
    //return(addr / s_hal_flash_PAGESIZE);
    // if s_hal_flash_PAGESIZE is 2k (= 2^11) -> division is equivalent to shift by 11 bits
    addr >>= 11;
    return(addr);
#elif   defined(HAL_USE_MPU_TYPE_STM32F4)
    uint8_t i;

    for(i=0; i<(s_hal_flash_PAGESNUM-1); i++)
    {
        if(addr < s_hal_flash_PAGEADDRS[i+1])
        {
            return(i);
        }
    }

    return(s_hal_flash_PAGESNUM-1);  
#else //defined(HAL_USE_MPU_TYPE_*)
    #error ERR --> choose a HAL_USE_MPU_TYPE_*
#endif    
}
#endif


#if     defined(HAL_USE_MPU_TYPE_STM32F4)
static uint32_t s_hal_flash_stm32f4_sector_get(uint32_t addr)
{
    static const uint16_t FLASH_Sectors[12] =
    {
        FLASH_Sector_0, FLASH_Sector_1, FLASH_Sector_2, FLASH_Sector_3, FLASH_Sector_4, FLASH_Sector_5, 
        FLASH_Sector_6, FLASH_Sector_7, FLASH_Sector_8, FLASH_Sector_9, FLASH_Sector_10, FLASH_Sector_11       
    };

//     if(hal_false == hal_flash_address_isvalid(addr))
//     {
//         return(hal_res_NOK_generic);
//     }

    // find pageindex;

    return(FLASH_Sectors[s_hal_flash_pageindex_get(addr)]);    
}
#endif


static uint32_t s_hal_flash_quickget_pagesize(uint32_t addr)
{
#if     defined(HAL_USE_MPU_TYPE_STM32F1)
    return(s_hal_flash_PAGESIZE);
#elif   defined(HAL_USE_MPU_TYPE_STM32F4)    
    uint32_t pageindex = s_hal_flash_pageindex_get(addr);   
    return(s_hal_flash_PAGESIZES[pageindex]);
#else //defined(HAL_USE_MPU_TYPE_*)
    #error ERR --> choose a HAL_USE_MPU_TYPE_*
#endif    
}


#endif//HAL_USE_FLASH

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



