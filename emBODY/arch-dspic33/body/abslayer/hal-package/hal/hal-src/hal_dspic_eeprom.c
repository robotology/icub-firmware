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

/* @file       hal_dspic_eeprom.h
    @brief      This file implements API for read/write on EEPROM by I2C wrapping stm32-library functions.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
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
#include "hal_flash.h"
#include "hal_dspic_base_hid.h"
#include "hal_arch_dspic.h"
#include "hal_brdcfg.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_eeprom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_dspic_eeprom_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_eeprom_t2index(t)                           ((uint8_t)(t))

#define ROWS_PER_PAGE                                   (_FLASH_PAGE/_FLASH_ROW)
#define BYTES_PER_PAGE                                  (_FLASH_PAGE*2)
#define PCUNIT_PER_PAGE                                 (_FLASH_PAGE*2)

#define BYTES_PER_ROW                                   (_FLASH_ROW*2)
#define PCUNIT_PER_ROW                                  (_FLASH_ROW*2)



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

            res = hal_res_OK;
        } break;

        case hal_eeprom_i2c_01:
        {
            res = hal_res_NOK_generic;
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
            res = hal_res_NOK_generic;
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
            res = hal_res_NOK_generic;
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
            res = hal_res_NOK_generic;
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
            val = hal_NA32;
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
            val = hal_NA32;
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

    if(eep != hal_eeprom_emulatedflash)
    {
        return(hal_false);
    }
    
    if((addr >= hal_brdcfg_eeprom__emflash_baseaddress) && (addr < (hal_brdcfg_eeprom__emflash_baseaddress+hal_brdcfg_eeprom__emflash_totalsize)))
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
    return(hal_base_hid_byte_bitcheck(hal_brdcfg_eeprom__supported_mask, HAL_eeprom_t2index(eep)) );
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
        data2write = (uint8_t*)data + count;

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


#endif//HAL_USE_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


#if 0

static void s_hal_eeprom_dspic_emuflash_read(void *data, _prog_addressT addr, int16_t size);
static hal_result_t s_hal_eeprom_dspic_emuflash_write(_prog_addressT EE_address, int16_t size, void *buf);
extern hal_result_t s_hal_eeprom_dspic_emuflash_erase(_prog_addressT EE_address, int16_t size);
static void s_hal_eeprom_dspic_emuflash_write_1_page(_prog_addressT EE_address, int16_t size, void *buf);

extern hal_result_t hal_eeprom_startpage_erase(hal_eeprom_t eep, uint32_t addr, uint32_t size)
{
    hal_result_t res = hal_res_NOK_generic;
    
    if(hal_true != s_hal_eeprom_initted_is(eep))
    {
        return(hal_res_NOK_generic);
    }

    if(size == 0)
    {
         return(hal_res_NOK_generic);
    }    

    // marker: evaluate the kind of eeprom
    
    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            //res = s_hal_eeprom_eraseflash(addr, size);
            s_hal_eeprom_dspic_emuflash_erase((_prog_addressT)addr, (int16_t)size);
            res = hal_res_OK;
        } break;

        case hal_eeprom_i2c_01:
        {
            res = hal_res_NOK_generic;
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;

    }


    return(res);
}


extern hal_result_t hal_eeprom_startpage_write(hal_eeprom_t eep, uint32_t addr, uint32_t size, void *data)
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

    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            //res = s_hal_eeprom_writeflash(addr, size, data);
            s_hal_eeprom_dspic_emuflash_write((_prog_addressT)addr, (uint16_t)size, data);
            res = hal_res_OK;
        } break;

        case hal_eeprom_i2c_01:
        {
            res = hal_res_NOK_generic;
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;

    }


    return(res);
}

extern hal_result_t hal_eeprom_startpage_read(hal_eeprom_t eep, uint32_t addr, uint32_t size, void *data)
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


    switch(eep)
    {
        case hal_eeprom_emulatedflash:
        {
            //res = hal_flash_read(addr, size, data);
            s_hal_eeprom_dspic_emuflash_read(data, (_prog_addressT)addr, (int16_t)size);
            res = hal_res_OK;

        } break;

        case hal_eeprom_i2c_01:
        {
            res = hal_res_NOK_generic;
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;

    }

		   
	return(res);
}


static hal_result_t s_hal_eeprom_dspic_emuflash_write(_prog_addressT EE_address, int16_t size, void *buf)
{
    _prog_addressT addr;
    int16_t remaining_data;
    int16_t *data_ptr;

    #warning -> acemor: maybe a check upon validity of the address and size ??
    
    remaining_data = size;
    data_ptr = buf;
    addr = EE_address;
      
    do
    { 
        if(remaining_data < BYTES_PER_PAGE)
        {
            s_hal_eeprom_dspic_emuflash_write_1_page(addr, remaining_data, (void*)data_ptr);
            remaining_data = 0;
        }
        else
        {
            s_hal_eeprom_dspic_emuflash_write_1_page(addr, BYTES_PER_PAGE, (void*)data_ptr);
            remaining_data -= BYTES_PER_PAGE;
            addr += PCUNIT_PER_PAGE;
            data_ptr += (BYTES_PER_PAGE/2); // divided by two because data_ptr is pointer to int16_t=2Bytes
        }
    }
    while(remaining_data > 0);

    return(hal_res_OK);
}

static void s_hal_eeprom_dspic_emuflash_read(void *data, _prog_addressT addr, int16_t size)
{
    #warning -> acemor: maybe a check upon validity of the address and size ??
    _memcpy_p2d16(data, addr, size);
}


extern hal_result_t s_hal_eeprom_dspic_emuflash_erase(_prog_addressT EE_address, int16_t size)
{
    _prog_addressT addr = EE_address;
    int16_t remaining_data =  size;
     
    #warning -> acemor: maybe a check upon validity of the address and size ??
    
    do
    {
        
        if(remaining_data < BYTES_PER_PAGE)
        {
            _erase_flash(addr);
            remaining_data = 0;
        }
        else
        {
             _erase_flash(addr);
            remaining_data -= BYTES_PER_PAGE;
            addr += PCUNIT_PER_PAGE;
        }
    }
    while(remaining_data > 0);   

    return(hal_res_OK);
}

static void s_hal_eeprom_dspic_emuflash_write_1_page(_prog_addressT EE_address, int16_t size, void *buf)
{

    int16_t buf_flash_pag1[_FLASH_PAGE]; //Note: this buffer contains 1024 bytes = num of bytes in a flash page
    _prog_addressT addr_row;
    int16_t *buf_row;
    int16_t i;

    /*
        function for reading and writing from/to flash with 16 bits read and write 2 bytes for each instruction.
        One instruction = 2 PC units ==> 4 bytes : 1 phantom, 3 real.
        if you use function about 16 bits you use only 2 bytes of 3 real bytes
        else if use function about 24 bits you use 3 bytes of 3 real bytes.

        /--INSTRUCTION--/
        |---|---|---|---|
        | P | 1 | 2 | 3 |
        |___|___|___|___|
        /------//-------/
        1PCunit  1PCunit

        P = phantom byte
        1,2,3 = real bytes used in binary code and in ...d24 flash instruction
        2,3 = bytes used by ...d16 flash instructions (byte 3 is not used)
    
    */
 
    /* The third parameter in this case (function ...d16) express the number
     * of bytes to be written. Note that since we write 2 bytes per instruction 
     * (see above explanation) we are actually writing 2 bytes for each 2 PC units 
     * (because 1 instraction = 2 p.c. units) SO you can think about the 3rd parameter
     * also as P.C. units. In other words in this case (..d16 instruction) the number
     * of PC units for 3rd param is equal to the nuber of bytes.
     * !!!!!!! The important thing is that this has to be checked for
     * ...d24 functions!!!!!!!!
     */
    _memcpy_p2d16(buf_flash_pag1, EE_address, BYTES_PER_PAGE);


    _erase_flash(EE_address); //a flash page is the smallest program mem erase unit

    memcpy((int8_t*)buf_flash_pag1,(int8_t*)(buf), size);

    // a row is the smollest program mem program unit (a raw = 64 instructions)
    addr_row = EE_address;
    buf_row = buf_flash_pag1;
    for(i=0; i<ROWS_PER_PAGE; i++)
    {
        _write_flash16(addr_row, buf_row);
        addr_row += PCUNIT_PER_ROW;
        buf_row += BYTES_PER_ROW/2; //divided by two because buf_row is a pointer to int16_t=2 bytes
    }
}

#endif



