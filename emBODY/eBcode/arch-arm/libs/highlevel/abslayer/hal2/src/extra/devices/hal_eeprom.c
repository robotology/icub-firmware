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

/* @file       hal_eeprom.h
	@brief      This file implements internals for management of hal device eeprom
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
#include "hal_middleware_interface.h"

#ifdef HAL_USE_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"

#include "hal_flash.h"
#include "hal_i2c.h"
#include "hal_brdcfg.h"
#include "hal_base_hid.h" 



#include "hl_bits.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_eeprom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_eeprom_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_eeprom_id2index(t)               ((uint8_t)(t))

#undef HAL_USE_EEPROM_EMULATEDFLASH


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_eeprom_cfg_t hal_eeprom_cfg_default =
{
    .flashpagebuffer    = NULL, 
    .flashpagesize      = 0
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint8_t                                 dummy;
} hal_eeprom_internal_item_t;

typedef struct
{
    uint32_t                                inittedmask;
    hal_eeprom_cfg_t                        config_of_emulated_flash;
} hal_eeprom_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_eeprom_supported_is(hal_eeprom_t id);
static void s_hal_eeprom_initted_set(hal_eeprom_t id);
static hal_boolval_t s_hal_eeprom_initted_is(hal_eeprom_t id);

#if     defined(HAL_USE_EEPROM_EMULATEDFLASH)
static hal_result_t s_hal_eeprom_eraseflash(uint32_t addr, uint32_t size);
static hal_result_t s_hal_eeprom_writeflash(uint32_t addr, uint32_t size, void * data);
#endif//defined(HAL_USE_EEPROM_EMULATEDFLASH)

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_eeprom_theinternals_t s_hal_eeprom_theinternals =
{
    .inittedmask                = 0,
    .config_of_emulated_flash   = { .flashpagebuffer = NULL, .flashpagesize = 0 }
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_eeprom_init(hal_eeprom_t id, const hal_eeprom_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_unsupported;
    if(hal_true != s_hal_eeprom_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_eeprom_cfg_default;
    }
           
    if(hal_true == s_hal_eeprom_initted_is(id))
    {
        if(hal_eeprom_emulatedflash != id)
        {
            return(hal_res_OK);
        }
        else if(0 == memcmp(cfg, &s_hal_eeprom_theinternals.config_of_emulated_flash, sizeof(hal_eeprom_cfg_t)))
        {   // ok only if the previously used config is the same as the current one
            return(hal_res_OK);
        }
        else
        {
            return(hal_res_NOK_generic);
        }
        
    }
        

    const hal_eeprom_driver_t* driver = &hal_eeprom__theboardconfig.driver[HAL_eeprom_id2index(id)];

    switch(id)
    {
#if     defined(HAL_USE_EEPROM_EMULATEDFLASH)        
        case hal_eeprom_emulatedflash:
        {
            if((0 == cfg->flashpagesize) || (NULL == cfg->flashpagebuffer))
            {
                return(hal_res_NOK_generic);
            }
            
            memcpy(&s_hal_eeprom_theinternals.config_of_emulated_flash, cfg, sizeof(hal_eeprom_cfg_t)); 

            hal_flash_unlock();
            res = hal_res_OK;
        } break;
#endif//defined(HAL_USE_EEPROM_EMULATEDFLASH)
        case hal_eeprom_i2c_01:
        case hal_eeprom_i2c_02:
        {
            res = hal_i2c_init(driver->cfg.i2c.i2cid, NULL);   // i use default configuration for i2c
            if(hal_res_OK != res)
            {
                return(res);
            }
            
            if((NULL != driver->fun.init) &&
               (NULL != driver->fun.read) &&
               (NULL != driver->fun.write)  )
            {
                res = driver->fun.init(id, driver->cfg.i2c.initpar);
            }
            else
            {
                res = hal_res_NOK_generic;
            }

        } break;

        default:
        {
            res = hal_res_NOK_unsupported;
        } break;

    }

    
    if(hal_res_OK == res)
    {
        s_hal_eeprom_initted_set(id);
    }

    return(hal_res_OK);
}


extern hal_result_t hal_eeprom_erase(hal_eeprom_t id, uint32_t addr, uint32_t size)
{   
    hal_result_t res = hal_res_NOK_generic;
    // must be static const to be used within any shared library
    static const uint64_t value8[4] = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
    #define factor 32
    uint32_t num8 = 0;
    uint32_t rem8 = 0;
    uint32_t i = 0;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_true != s_hal_eeprom_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)     
    if(size == 0)
    {
         return(hal_res_NOK_generic);
    }
    
    if(hal_false == hal_eeprom_address_is_valid(id, addr))
    {   // cannot write at the address the first bytes
        return(hal_res_NOK_generic);
    }

    if(hal_false == hal_eeprom_address_is_valid(id, addr+size-1))
    {   // cannot write at the address the last bytes
        return(hal_res_NOK_generic);
    }    
#endif
    
    // marker: evaluate the kind of eeprom
    
    const hal_eeprom_driver_t* driver = &hal_eeprom__theboardconfig.driver[HAL_eeprom_id2index(id)];
    
    switch(id)
    {
#if     defined(HAL_USE_EEPROM_EMULATEDFLASH)        
        case hal_eeprom_emulatedflash:
        {
            res = s_hal_eeprom_eraseflash(addr, size);
        } break;
#endif//defined(HAL_USE_EEPROM_EMULATEDFLASH)
        case hal_eeprom_i2c_01:
        case hal_eeprom_i2c_02:
        {
            num8 = size / factor;
            rem8 = size % factor;

            for(i=0; i<num8; i++)
            {
                res = driver->fun.write(id, addr, factor, (uint8_t*)&value8, NULL);
                if(hal_res_OK != res) return(hal_res_NOK_generic);
                addr += factor;
            }

            if(0 != rem8)
            {
                res = driver->fun.write(id, addr, rem8, (uint8_t*)&value8, NULL);
                if(hal_res_OK != res) return(hal_res_NOK_generic);
            }


        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;

    }


    return(res);
}



extern hal_result_t hal_eeprom_write(hal_eeprom_t id, uint32_t addr, uint32_t size, void *data)
{
    hal_result_t res = hal_res_NOK_generic;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_true != s_hal_eeprom_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if((NULL == data)  || (size == 0))
    {
         return(hal_res_NOK_generic);
    }

    if(hal_false == hal_eeprom_address_is_valid(id, addr))
    {   // cannot write at the address the first bytes
        return(hal_res_NOK_generic);
    }

    if(hal_false == hal_eeprom_address_is_valid(id, addr+size-1))
    {   // cannot write at the address the last bytes
        return(hal_res_NOK_generic);
    }    
#endif
    
    const hal_eeprom_driver_t* driver = &hal_eeprom__theboardconfig.driver[HAL_eeprom_id2index(id)];
    
    switch(id)
    {
#if     defined(HAL_USE_EEPROM_EMULATEDFLASH)        
        case hal_eeprom_emulatedflash:
        {
            res = s_hal_eeprom_writeflash(addr, size, data);
        } break;
#endif//defined(HAL_USE_EEPROM_EMULATEDFLASH)
        case hal_eeprom_i2c_01:
        case hal_eeprom_i2c_02:    
        {
            res = driver->fun.write(id, addr, size, (uint8_t*)data, NULL);
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;

    }

    return(res);
}


extern hal_result_t hal_eeprom_read(hal_eeprom_t id, uint32_t addr, uint32_t size, void *data)
{
    hal_result_t res = hal_res_NOK_generic;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_true != s_hal_eeprom_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif


#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)       
	if((NULL == data)  || (size == 0))
	{
	     return(hal_res_NOK_generic);
	}

    if(hal_false == hal_eeprom_address_is_valid(id, addr))
    {   // cannot write at the address the first bytes
        return(hal_res_NOK_generic);
    }

    if(hal_false == hal_eeprom_address_is_valid(id, addr+size-1))
    {   // cannot write at the address the last bytes
        return(hal_res_NOK_generic);
    }  
#endif

    const hal_eeprom_driver_t* driver = &hal_eeprom__theboardconfig.driver[HAL_eeprom_id2index(id)];
    
    switch(id)
    {
#if     defined(HAL_USE_EEPROM_EMULATEDFLASH)        
        case hal_eeprom_emulatedflash:
        {
            res = hal_flash_read(addr, size, data);
        } break;
#endif//defined(HAL_USE_EEPROM_EMULATEDFLASH)
        case hal_eeprom_i2c_01:
        case hal_eeprom_i2c_02:
        {
            res = driver->fun.read(id, addr, size, (uint8_t*)data, NULL);
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;

    }

		   
	return(res);
}



extern uint32_t hal_eeprom_get_baseaddress(hal_eeprom_t id)
{
    uint32_t val = hal_NA32;
    
    const hal_eeprom_driver_t* driver = &hal_eeprom__theboardconfig.driver[HAL_eeprom_id2index(id)];

    switch(id)
    {
#if     defined(HAL_USE_EEPROM_EMULATEDFLASH)       
        case hal_eeprom_emulatedflash:
        {
            val = driver->cfg.fls.baseaddress;;
        } break;
#endif//defined(HAL_USE_EEPROM_EMULATEDFLASH) 
        case hal_eeprom_i2c_01:
        case hal_eeprom_i2c_02:    
        {
            val = driver->cfg.i2c.baseaddress;
        } break;

        default:
        {
            val = hal_NA32;
        } break;
    }

    return(val);
}


extern uint32_t hal_eeprom_get_totalsize(hal_eeprom_t id)
{
    uint32_t val = hal_NA32;
    
    const hal_eeprom_driver_t* driver = &hal_eeprom__theboardconfig.driver[HAL_eeprom_id2index(id)];

    switch(id)
    {
#if     defined(HAL_USE_EEPROM_EMULATEDFLASH)        
        case hal_eeprom_emulatedflash:
        {
            val = driver->cfg.fls.totalsize;
        } break;
#endif//defined(HAL_USE_EEPROM_EMULATEDFLASH)
        case hal_eeprom_i2c_01:
        case hal_eeprom_i2c_02:    
        {
            val = driver->cfg.i2c.totalsize;
        } break;

        default:
        {
            val = hal_NA32;
        } break;
    }

    return(val);
}


extern hal_bool_t hal_eeprom_address_is_valid(hal_eeprom_t id, uint32_t addr)
{
    uint32_t base;
    uint32_t size;
    
    const hal_eeprom_driver_t* driver = &hal_eeprom__theboardconfig.driver[HAL_eeprom_id2index(id)];

    switch(id)
    {
#if     defined(HAL_USE_EEPROM_EMULATEDFLASH)        
        case hal_eeprom_emulatedflash:
        {
            base = driver->cfg.fls.baseaddress;
            size = driver->cfg.fls.totalsize;
        } break;
#endif//defined(HAL_USE_EEPROM_EMULATEDFLASH)
        case hal_eeprom_i2c_01:
        case hal_eeprom_i2c_02:    
        {
            base = driver->cfg.i2c.baseaddress;
            size = driver->cfg.i2c.totalsize;
        } break;

        default:
        {
            base = hal_NA32;
            size = hal_NA32;
        } break;
    }


    if((hal_NA32 == base) || (hal_NA32 == size))
    {
        return(hal_false);
    }
    
    if((addr >= base) && (addr < (base+size)))
    {
        return(hal_true);
    }
    else
    {
        return(hal_false);
    }
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

static hal_boolval_t s_hal_eeprom_supported_is(hal_eeprom_t id)
{
#if     !defined(HAL_USE_EEPROM_EMULATEDFLASH)    
    if(hal_eeprom_emulatedflash == id)
    {
        return(hal_false);
    }
#endif//!defined(HAL_USE_EEPROM_EMULATEDFLASH)    
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_eeprom__theboardconfig.supportedmask, HAL_eeprom_id2index(id)));
}

static void s_hal_eeprom_initted_set(hal_eeprom_t id)
{
     hl_bits_word_bitset(&s_hal_eeprom_theinternals.inittedmask, HAL_eeprom_id2index(id));
}

static hal_boolval_t s_hal_eeprom_initted_is(hal_eeprom_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_eeprom_theinternals.inittedmask, HAL_eeprom_id2index(id)));
}



#if     defined(HAL_USE_EEPROM_EMULATEDFLASH)

static hal_result_t s_hal_eeprom_eraseflash(uint32_t addr, uint32_t size)
{
    uint32_t pageaddr   = hal_flash_get_pageaddr(addr);
    uint32_t pagesize   = hal_flash_get_pagesize(addr);
    uint32_t offset     = addr - pageaddr;
    uint32_t numbytes   = 0;

    if(hal_false == hal_flash_address_isvalid(addr))
    {   // cannot erase at the address of the first bytes
        return(hal_res_NOK_generic);
    }

    if(hal_false == hal_flash_address_isvalid(addr+size-1))
    {   // cannot erase at the address of the last bytes
        return(hal_res_NOK_generic);
    }

    if(NULL == s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagebuffer)
    {   // dont have a pagebuffer
        return(hal_res_NOK_generic);
    }


    for(;;)
    {
        pageaddr   = hal_flash_get_pageaddr(addr);
        pagesize   = hal_flash_get_pagesize(addr);
        offset     = addr - pageaddr;
        numbytes   = (size<(pagesize-offset)) ? (size) : (pagesize-offset);

        if(s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagesize < pagesize)
        {   // cannot erase a page with size bigger than the buffer capacity
            return(hal_res_NOK_generic);
        }

        if(numbytes == pagesize)
        {   // just erase
            hal_flash_erase(pageaddr, pagesize);
        }
        else
        {   // need to save some data in the page...
            // read a page and place content in buffer
            hal_flash_read(pageaddr, pagesize, s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagebuffer);
            // set to 0xff a part of the buffer
            memset(&((uint8_t*)s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagebuffer)[offset], 0xFF, numbytes);
            // erase the full page
            hal_flash_erase(pageaddr, pagesize);
            // write the buffer
            hal_flash_write(pageaddr, pagesize, s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagebuffer);
        }

        size -= numbytes;
        addr = pageaddr+pagesize;
        

        if(0 == size)
        {
            return(hal_res_OK);
        }

    }
  
}


static hal_result_t s_hal_eeprom_writeflash(uint32_t addr, uint32_t size, void * data)
{
    uint32_t pageaddr   = hal_flash_get_pageaddr(addr);
    uint32_t pagesize   = hal_flash_get_pagesize(addr);
    uint32_t offset     = addr - pageaddr;
    uint32_t count      = 0;
    uint32_t numbytes   = 0;
    uint8_t* data2write = data;

    if(hal_false == hal_flash_address_isvalid(addr))
    {   // cannot write at the address the first bytes
        return(hal_res_NOK_generic);
    }

    if(hal_false == hal_flash_address_isvalid(addr+size-1))
    {   // cannot write at the address the last bytes
        return(hal_res_NOK_generic);
    }

    if(NULL == s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagebuffer)
    {   // dont have a pagebuffer
        return(hal_res_NOK_generic);
    }


    for(;;)
    {
        pageaddr   = hal_flash_get_pageaddr(addr);
        pagesize   = hal_flash_get_pagesize(addr);
        offset     = addr - pageaddr;
        numbytes   = (size<(pagesize-offset)) ? (size) : (pagesize-offset);
        data2write = (uint8_t*)data + count;;

        if(s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagesize < pagesize)
        {   // cannot write in a page with size bigger than the buffer capacity
            return(hal_res_NOK_generic);
        }
        
        // read a page and place content in buffer
        hal_flash_read(pageaddr, pagesize, s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagebuffer);
        // change the buffer
        memcpy(&((uint8_t*)s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagebuffer)[offset], data2write, numbytes);
        // erase the page
        hal_flash_erase(pageaddr, pagesize);
        // write the buffer
        hal_flash_write(pageaddr, pagesize, s_hal_eeprom_theinternals.config_of_emulated_flash.flashpagebuffer);

        count += numbytes;
        size -= numbytes;
        addr = pageaddr+pagesize;
        

        if(0 == size)
        {
            return(hal_res_OK);
        }
    }   
}

#endif//defined(HAL_USE_EEPROM_EMULATEDFLASH)


#endif//HAL_USE_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



