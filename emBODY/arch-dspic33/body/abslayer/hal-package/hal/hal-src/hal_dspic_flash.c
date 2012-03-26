
/* @file       hal_dspic_flash.c
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


#include "hal_dspic_base_hid.h"
#include "hal_arch_dspic.h"
#include "hal_brdcfg.h"


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_flash.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_dspic_flash_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
const uint32_t hal_flash_BASEADDR   = 0x00000000;
const uint32_t hal_flash_TOTALSIZE  = 172L*1024L; //dspic33-256K has 0x00015800 = 88064 locations -> 2*88064 = 176128 = 172*1024 
const uint32_t hal_flash_PAGESIZE   = 1024L;
const uint32_t hal_flash_UNITSIZE   = 128L;



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_result_t s_hal_flash_erasepage(uint32_t addr);
static hal_result_t s_hal_flash_writedata(uint32_t addr, uint32_t size, uint32_t *data);
static hal_result_t s_hal_flash_write128bytes(uint32_t addr, int16_t *data);

//static hal_result_t s_hal_flash_write_1_page(_prog_addressT EE_address, int16_t size, void *buf);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_flash_lock(void)
{
    return(hal_res_OK);
}


extern hal_result_t hal_flash_unlock(void)
{
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
    uint8_t addr128 = 0;
    uint8_t size128 = 0;
 
    if((hal_false == hal_flash_address_isvalid(addr)) || (0 == size) || (NULL == data))
    {
        return(hal_res_NOK_generic);
    }


    if(0 == (addr&0x0000007f))
    {
        addr128 = 1;
    }
    else
    {
        //hal_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "hal_flash_write() unsupports it");
        return(hal_res_NOK_generic);
    }


    if(0 == (size&0x0000007f))
    {
        size128 = 1;
    }
    else
    {
        //hal_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "hal_flash_write() unsupports it");
        return(hal_res_NOK_generic);
    }


    // we can only write multiple of 128-bytes on addresses which are 128-bytes aligned
    return(s_hal_flash_writedata(addr, size, data));
    //return(s_hal_flash_write_1_page(addr, size, data));
}


extern hal_result_t hal_flash_read(uint32_t addr, uint32_t size, void *data)
{
    volatile uint32_t next;
    if((hal_false == hal_flash_address_isvalid(addr)) || (0 == size) || (NULL == data))
    {
        return(hal_res_NOK_generic);
    }

    addr = hal_arch_dspic_convert_halflash_address_to_dspicflash_address(addr);
    
    next = _memcpy_p2d16((void*)data, addr, (uint16_t)size);
    next = next;

    return(hal_res_OK);
}



extern uint32_t hal_flash_get_baseaddress(void)
{
    return(hal_flash_BASEADDR);
}


extern uint32_t hal_flash_get_totalsize(void)
{
    return(hal_flash_TOTALSIZE);
}


extern hal_boolval_t hal_flash_address_isvalid(uint32_t addr)
{
    if((addr >= hal_flash_BASEADDR) && (addr < (hal_flash_BASEADDR+hal_flash_TOTALSIZE)))
    {
        return(hal_true);
    }
    else
    {
        return(hal_false);
    }
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

    // every page is 1024
    addr >>= 10;
    addr <<= 10;
    return(addr);
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

 

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static hal_result_t s_hal_flash_erasepage(uint32_t addr)
{
    if(hal_false == hal_flash_address_isvalid(addr))
    {
        return(hal_res_NOK_generic);
    }

    // find beginning of page
    addr = hal_flash_get_pageaddr(addr);


    _erase_flash(hal_arch_dspic_convert_halflash_address_to_dspicflash_address(addr));

    return(hal_res_OK);
}


static hal_result_t s_hal_flash_write128bytes(uint32_t addr, int16_t *data)
{   // the only possible way to write flash on dspic is by 128 bytes: by _FLASH_ROW = 64 uint16_t -> 128 bytes
    addr = hal_arch_dspic_convert_halflash_address_to_dspicflash_address(addr);
    _write_flash16(addr, data);
    return(hal_res_OK);
}



static hal_result_t s_hal_flash_writedata(uint32_t addr, uint32_t size, uint32_t *data)
{   // size and addr can be only a multiple of 128.
    volatile hal_result_t res = hal_res_OK;
    uint32_t EndAddr = 0;
    int16_t *data16 = (int16_t*)data;

    EndAddr = addr + size; 

    while((addr < EndAddr) && (res == hal_res_OK))
    {
        res = s_hal_flash_write128bytes(addr, data16);
        addr = addr + 128;
        data16 += (128/2);
    }
    
    return(res);
}


#endif//HAL_USE_FLASH


#if 0

#define __FLASH_PAGE   512
#define __FLASH_ROW     64

#define ROWS_PER_PAGE                                   (__FLASH_PAGE/__FLASH_ROW)
#define BYTES_PER_PAGE                                  (__FLASH_PAGE*2)
#define PCUNIT_PER_PAGE                                 (__FLASH_PAGE*2)

#define BYTES_PER_ROW                                   (__FLASH_ROW*2)
#define PCUNIT_PER_ROW                                  (__FLASH_ROW*2)

static hal_result_t s_hal_flash_write_1_page(_prog_addressT EE_address, int16_t size, void *buf)
{

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
     
    EE_address = hal_arch_dspic_convert_halflash_address_to_dspicflash_address(EE_address);

    //_erase_flash(EE_address); //a flash page is the smallest program mem erase unit

        // a row is the smollest program mem program unit (a raw = 64 instructions)
    addr_row = EE_address;
    buf_row = (int16_t *)buf;
    for(i=0; i<(ROWS_PER_PAGE); i++)
    {
        _write_flash16(addr_row, buf_row);
        addr_row += PCUNIT_PER_ROW;
        buf_row += BYTES_PER_ROW/2; //divided by two because buf_row is a pointer to int16_t=2 bytes
    }
    
    return(hal_res_OK);
}

#endif

#if 0

#define _FLASH_PAGE   512
#define _FLASH_ROW     64

#define ROWS_PER_PAGE                                   (_FLASH_PAGE/_FLASH_ROW)
#define BYTES_PER_PAGE                                  (_FLASH_PAGE*2)
#define PCUNIT_PER_PAGE                                 (_FLASH_PAGE*2)

#define BYTES_PER_ROW                                   (_FLASH_ROW*2)
#define PCUNIT_PER_ROW                                  (_FLASH_ROW*2)

    for(i=0; i<ROWS_PER_PAGE; i++)
    {
        _write_flash16(addr_row, buf_row);
        addr_row += PCUNIT_PER_ROW;
        buf_row += BYTES_PER_ROW/2; //divided by two because buf_row is a pointer to int16_t=2 bytes
    }


#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



