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

/* @file       hal_stm32_eeprom.h
	@brief      This file implements API for read/write on EEPROM by I2C wrapping stm32-library functions.
	@author     valentina.gaggero@iit.it
    @date       09/09/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"
#include "hal_stm32xx_include.h"

#include "hal_flash.h"
//#include "hal_i2c4hal.h"
#include "hal_i2c.h"
#include "hal_brdcfg.h"
#include "hal_stm32_base_hid.h" 
#include "hal_stm32_sys_hid.h"


#include "utils/stm32ee.h"  

#include "utils/hal_tools.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_eeprom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_eeprom_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_eeprom_t2index(t)               ((uint8_t)(t))



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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_eeprom_supported_is(hal_eeprom_t eep);
static void s_hal_eeprom_initted_set(hal_eeprom_t eep);
static hal_boolval_t s_hal_eeprom_initted_is(hal_eeprom_t eep);

static hal_result_t s_hal_eeprom_eraseflash(uint32_t addr, uint32_t size);
static hal_result_t s_hal_eeprom_writeflash(uint32_t addr, uint32_t size, void * data);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_eeprom_initted[hal_eeproms_num] = { hal_false };

static hal_eeprom_cfg_t s_hal_eeprom_emulated_flash_cfg = {0};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_eeprom_init(hal_eeprom_t eep, const hal_eeprom_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_unsupported;
    if(hal_true != s_hal_eeprom_supported_is(eep))
    {
        return(hal_res_NOK_unsupported);
    }

    if(NULL == cfg)
    {
        cfg = &hal_eeprom_cfg_default;
    }

    // marker: evaluate the kind of eeprom

    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            if((0 == cfg->flashpagesize) || (NULL == cfg->flashpagebuffer))
            {
                return(hal_res_NOK_generic);
            }
            
            memcpy(&s_hal_eeprom_emulated_flash_cfg, cfg, sizeof(hal_eeprom_cfg_t)); 

            hal_flash_unlock();
            res = hal_res_OK;
        } break;

        case hal_eeprom_i2c_01:
        {
            //res = hal_i2c4hal_init(hal_i2c4hal_port1, NULL);   // i use default configuration
            res = hal_i2c_init(hal_i2c_port1, NULL);   // i use default configuration
            if(hal_res_OK != res)
            {
                return(res);
            }
            
            stm32ee_cfg_t stm32eecfg            =
            {
                .devcfg             = 
               {
                    .device             = stm32ee_device_none,
                    .i2cport            = 1,        // is equivalent to hal_i2c_port1
                    .hwaddra2a1a0       = 0         // a0 = a1 = a2 = 0
//                     .wpval              = stm32gpio_valNONE,      // no write protection
//                     .wppin              =
//                     {
//                         .port               = stm32gpio_portNONE,
//                         .pin                = stm32gpio_pinNONE
//                     }                    
                },
                .i2cext             =
                {   // does not init/deinit. only use read/write/standby
                    .i2cinit            = NULL,         
                    .i2cdeinit          = NULL,
                    .i2cpar             = NULL,
                    .i2cread            = (stm32ee_int8_fp_uint8_uint8_regaddr_uint8p_uint16_t)hal_i2c_read,  
                    .i2cwrite           = (stm32ee_int8_fp_uint8_uint8_regaddr_uint8p_uint16_t)hal_i2c_write, 
                    .i2cstandby         = (stm32ee_int8_fp_uint8_uint8_t)hal_i2c_standby,            
                },
                .wpext              =
                {
                    .wpinit             = hal_brdcfg_eeprom__wp_init,
                    .wpdeinit           = NULL,
                    .wpenable           = hal_brdcfg_eeprom__wp_enable,
                    .wpdisable          = hal_brdcfg_eeprom__wp_disable                   
                }
            };
            
            stm32eecfg.devcfg.device        = (stm32ee_device_t)hal_brdcfg_eeprom__i2c_01_device.device;
            stm32eecfg.devcfg.i2cport       = hal_i2c_port1; //hal_i2c4hal_port1; // for i2c1 i2cport must be 0
            stm32eecfg.devcfg.hwaddra2a1a0  = hal_brdcfg_eeprom__i2c_01_device.hwaddra2a1a0;
//             stm32eecfg.devcfg.wpval         = (stm32gpio_val_t)hal_brdcfg_eeprom__i2c_01_device.wpval;
//             stm32eecfg.devcfg.wppin.port    = (stm32gpio_port_t)hal_brdcfg_eeprom__i2c_01_device.wppin.port;
//             stm32eecfg.devcfg.wppin.pin     = (stm32gpio_pin_t)hal_brdcfg_eeprom__i2c_01_device.wppin.pin;
            
            res = (hal_result_t) stm32ee_init(&stm32eecfg);

        } break;

        default:
        {
            res = hal_res_NOK_unsupported;
        } break;

    }

    if(hal_res_OK == res)
    {
        s_hal_eeprom_initted_set(eep);
    }

    return(hal_res_OK);
}


extern hal_result_t hal_eeprom_erase(hal_eeprom_t eep, uint32_t addr, uint32_t size)
{   
    hal_result_t res = hal_res_NOK_generic;
    // must be static const to be used within any shared library
    static const uint64_t value8[4] = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
    #define factor 32
    uint32_t num8 = 0;
    uint32_t rem8 = 0;
    uint32_t i = 0;
    //uint32_t rr = 0;
    
    if(hal_true != s_hal_eeprom_initted_is(eep))
    {
        return(hal_res_NOK_generic);
    }

    if(size == 0)
    {
         return(hal_res_NOK_generic);
    }
    
    if(hal_false == hal_eeprom_address_is_valid(eep, addr))
    {   // cannot write at the address the first bytes
        return(hal_res_NOK_generic);
    }

    if(hal_false == hal_eeprom_address_is_valid(eep, addr+size-1))
    {   // cannot write at the address the last bytes
        return(hal_res_NOK_generic);
    }    

    // marker: evaluate the kind of eeprom
    
    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            res = s_hal_eeprom_eraseflash(addr, size);
        } break;

        case hal_eeprom_i2c_01:
        {
            num8 = size / factor;
            rem8 = size % factor;

            for(i=0; i<num8; i++)
            {
                res = (hal_result_t)stm32ee_write(addr, factor, (uint8_t*)&value8, NULL);
                if(hal_res_OK != res) return(hal_res_NOK_generic);
                addr += factor;
            }

            if(0 != rem8)
            {
                res = (hal_result_t)stm32ee_write(addr, rem8, (uint8_t*)&value8, NULL);
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



extern hal_result_t hal_eeprom_write(hal_eeprom_t eep, uint32_t addr, uint32_t size, void *data)
{
    hal_result_t res = hal_res_NOK_generic;
    //uint32_t rr = 0;

    if(hal_true != s_hal_eeprom_initted_is(eep))
    {
        return(hal_res_NOK_generic);
    }

    if((NULL == data)  || (size == 0))
    {
         return(hal_res_NOK_generic);
    }

    if(hal_false == hal_eeprom_address_is_valid(eep, addr))
    {   // cannot write at the address the first bytes
        return(hal_res_NOK_generic);
    }

    if(hal_false == hal_eeprom_address_is_valid(eep, addr+size-1))
    {   // cannot write at the address the last bytes
        return(hal_res_NOK_generic);
    }    

    
    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            res = s_hal_eeprom_writeflash(addr, size, data);
        } break;

        case hal_eeprom_i2c_01:
        {
            res = (hal_result_t)stm32ee_write(addr, size, (uint8_t*)data, NULL);
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;

    }


    return(res);
}


extern hal_result_t hal_eeprom_read(hal_eeprom_t eep, uint32_t addr, uint32_t size, void *data)
{
    hal_result_t res = hal_res_NOK_generic;

    if(hal_true != s_hal_eeprom_initted_is(eep))
    {
        return(hal_res_NOK_generic);
    }

	if((NULL == data)  || (size == 0))
	{
	     return(hal_res_NOK_generic);
	}

    if(hal_false == hal_eeprom_address_is_valid(eep, addr))
    {   // cannot write at the address the first bytes
        return(hal_res_NOK_generic);
    }

    if(hal_false == hal_eeprom_address_is_valid(eep, addr+size-1))
    {   // cannot write at the address the last bytes
        return(hal_res_NOK_generic);
    }    
    
    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            res = hal_flash_read(addr, size, data);
        } break;

        case hal_eeprom_i2c_01:
        {
            res = (hal_result_t)stm32ee_read(addr, size, (uint8_t*)data, NULL);
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;

    }

		   
	return(res);
}



extern uint32_t hal_eeprom_get_baseaddress(hal_eeprom_t eep)
{
    uint32_t val = hal_NA32;

    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            val = hal_brdcfg_eeprom__emflash_baseaddress;
        } break;

        case hal_eeprom_i2c_01:
        {
            val = hal_brdcfg_eeprom__i2c_01_baseaddress;
        } break;

        default:
        {
            val = hal_NA32;
        } break;
    }

    return(val);
}


extern uint32_t hal_eeprom_get_totalsize(hal_eeprom_t eep)
{
    uint32_t val = hal_NA32;

    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            val = hal_brdcfg_eeprom__emflash_totalsize;
        } break;

        case hal_eeprom_i2c_01:
        {
            val = hal_brdcfg_eeprom__i2c_01_totalsize;
        } break;

        default:
        {
            val = hal_NA32;
        } break;
    }

    return(val);
}


extern hal_bool_t hal_eeprom_address_is_valid(hal_eeprom_t eep, uint32_t addr)
{
    uint32_t base;
    uint32_t size;

    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            base = hal_brdcfg_eeprom__emflash_baseaddress;
            size = hal_brdcfg_eeprom__emflash_totalsize;
        } break;

        case hal_eeprom_i2c_01:
        {
            base = hal_brdcfg_eeprom__i2c_01_baseaddress;
            size = hal_brdcfg_eeprom__i2c_01_totalsize;
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


extern uint32_t hal_eeprom_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_eeprom_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }

    // removed dependancy from NZI ram
    memset(s_hal_eeprom_initted, hal_false, hal_eeproms_num);
    memset(&s_hal_eeprom_emulated_flash_cfg, 0, sizeof(s_hal_eeprom_emulated_flash_cfg));

    return(hal_res_OK); 
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_eeprom_supported_is(hal_eeprom_t eep)
{
    return(hal_tools_bitoperator_byte_bitcheck(hal_brdcfg_eeprom__supported_mask, HAL_eeprom_t2index(eep)) );
}

static void s_hal_eeprom_initted_set(hal_eeprom_t eep)
{
    s_hal_eeprom_initted[HAL_eeprom_t2index(eep)] = hal_true;
}

static hal_boolval_t s_hal_eeprom_initted_is(hal_eeprom_t eep)
{
    return(s_hal_eeprom_initted[HAL_eeprom_t2index(eep)]);
}


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

    if(NULL == s_hal_eeprom_emulated_flash_cfg.flashpagebuffer)
    {   // dont have a pagebuffer
        return(hal_res_NOK_generic);
    }


    for(;;)
    {
        pageaddr   = hal_flash_get_pageaddr(addr);
        pagesize   = hal_flash_get_pagesize(addr);
        offset     = addr - pageaddr;
        numbytes   = (size<(pagesize-offset)) ? (size) : (pagesize-offset);

        if(s_hal_eeprom_emulated_flash_cfg.flashpagesize < pagesize)
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
            hal_flash_read(pageaddr, pagesize, s_hal_eeprom_emulated_flash_cfg.flashpagebuffer);
            // set to 0xff a part of the buffer
            memset(&((uint8_t*)s_hal_eeprom_emulated_flash_cfg.flashpagebuffer)[offset], 0xFF, numbytes);
            // erase the full page
            hal_flash_erase(pageaddr, pagesize);
            // write the buffer
            hal_flash_write(pageaddr, pagesize, s_hal_eeprom_emulated_flash_cfg.flashpagebuffer);
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

    if(NULL == s_hal_eeprom_emulated_flash_cfg.flashpagebuffer)
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

        if(s_hal_eeprom_emulated_flash_cfg.flashpagesize < pagesize)
        {   // cannot write in a page with size bigger than the buffer capacity
            return(hal_res_NOK_generic);
        }
        
        // read a page and place content in buffer
        hal_flash_read(pageaddr, pagesize, s_hal_eeprom_emulated_flash_cfg.flashpagebuffer);
        // change the buffer
        memcpy(&((uint8_t*)s_hal_eeprom_emulated_flash_cfg.flashpagebuffer)[offset], data2write, numbytes);
        // erase the page
        hal_flash_erase(pageaddr, pagesize);
        // write the buffer
        hal_flash_write(pageaddr, pagesize, s_hal_eeprom_emulated_flash_cfg.flashpagebuffer);

        count += numbytes;
        size -= numbytes;
        addr = pageaddr+pagesize;
        

        if(0 == size)
        {
            return(hal_res_OK);
        }

    }

   
}


#if 0
static hal_result_t s_hal_eeprom_writeflash(uint32_t addr, uint32_t size, void * data)
{
    //#warning --> acemor: todo: add a function which writes manages write data in flash: read page, change, erase, write.
    // need a buffer of 2k ... shall we pass it with the cfg?
    uint32_t pageaddr   = hal_flash_get_pageaddr(addr);
    uint32_t pagesize   = hal_flash_get_pagesize(addr);
    uint32_t offset     = addr - pageaddr;

    if(hal_false == hal_flash_address_isvalid(addr))
    {   // cannot write at the address the first bytes
        return(hal_res_NOK_generic);
    }

    if(hal_false == hal_flash_address_isvalid(addr+size-1))
    {   // cannot write at the address the last bytes
        return(hal_res_NOK_generic);
    }

    if(NULL == s_hal_eeprom_emulated_flash_cfg.flashpagebuffer)
    {   // dont have a pagebuffer
        return(hal_res_NOK_generic);
    }

    #warning: eeprom in flash so far i can write only .... fix it
    if((offset+size) > pagesize)
    {
        return(hal_res_NOK_generic);
    }

    memset(s_hal_eeprom_emulated_flash_cfg.flashpagebuffer, 0, s_hal_eeprom_emulated_flash_cfg.flashpagesize);


    hal_flash_read(pageaddr, pagesize, s_hal_eeprom_emulated_flash_cfg.flashpagebuffer);
    
    memcpy(&((uint8_t*)s_hal_eeprom_emulated_flash_cfg.flashpagebuffer)[offset], data, size);
        
    hal_flash_write(pageaddr, pagesize, s_hal_eeprom_emulated_flash_cfg.flashpagebuffer);

   

    return(hal_res_OK);
}
#endif


#endif//HAL_USE_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



