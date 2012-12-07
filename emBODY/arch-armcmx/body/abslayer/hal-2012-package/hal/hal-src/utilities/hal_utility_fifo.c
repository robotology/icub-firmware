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


/* @file       hal_utility_fifo.c
    @brief      this file implements internals of a tool module used in hal
    @author     marco.accame@iit.it
    @date       10/29/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_UTILITY_FIFO

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

//#include "hal_stm32xx_include.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_utility_fifo.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_utility_fifo_hid.h" 


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

static void s_hal_utility_fifo_itemofsize01copy(uint8_t* dst, uint32_t offsetdst, uint8_t* src, uint32_t offsetsrc);
static uint8_t* s_hal_utility_fifo_itemofsize01addr(uint8_t* pool, uint32_t pos);

static void s_hal_utility_fifo_itemofsize02copy(uint16_t* dst, uint32_t offsetdst, uint16_t* src, uint32_t offsetsrc);
static uint8_t* s_hal_utility_fifo_itemofsize02addr(uint16_t* pool, uint32_t pos);

static void s_hal_utility_fifo_itemofsize04copy(uint32_t* dst, uint32_t offsetdst, uint32_t* src, uint32_t offsetsrc);
static uint8_t* s_hal_utility_fifo_itemofsize04addr(uint32_t* pool, uint32_t pos);

static void s_hal_utility_fifo_itemofsize08copy(uint64_t* dst, uint32_t offsetdst, uint64_t* src, uint32_t offsetsrc);
static uint8_t* s_hal_utility_fifo_itemofsize08addr(uint64_t* pool, uint32_t pos);

static void s_hal_utility_fifo_itemofsize16copy(uint64_t* dst, uint32_t offsetdst, uint64_t* src, uint32_t offsetsrc);
static uint8_t* s_hal_utility_fifo_itemofsize16addr(uint64_t* pool, uint32_t pos);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




extern void hal_utility_fifo_init(hal_utility_fifo_t *fifo, uint8_t capacity, uint8_t sizeofitem, uint8_t *data, uint8_t *iteminitval)
{
    // avoid checks .... be careful in calling
    
    fifo->capacity      = capacity;
    fifo->sizeofitem    = sizeofitem;
    fifo->data          = data;
    
    switch(sizeofitem)
    {
        case 16:
        {   
            fifo->itemaddr  = (hal_utility_fifo_fn_itemaddr_t)s_hal_utility_fifo_itemofsize16addr;
            fifo->itemcopy  = (hal_utility_fifo_fn_itemcopy_t)s_hal_utility_fifo_itemofsize16copy;
        } break;
        
        case 8:
        {   
            fifo->itemaddr  = (hal_utility_fifo_fn_itemaddr_t)s_hal_utility_fifo_itemofsize08addr;
            fifo->itemcopy  = (hal_utility_fifo_fn_itemcopy_t)s_hal_utility_fifo_itemofsize08copy;
        } break;
 
        case 4:
        {   
            fifo->itemaddr  = (hal_utility_fifo_fn_itemaddr_t)s_hal_utility_fifo_itemofsize04addr;
            fifo->itemcopy  = (hal_utility_fifo_fn_itemcopy_t)s_hal_utility_fifo_itemofsize04copy;
        } break;
 
        case 2:
        {   
            fifo->itemaddr  = (hal_utility_fifo_fn_itemaddr_t)s_hal_utility_fifo_itemofsize02addr;
            fifo->itemcopy  = (hal_utility_fifo_fn_itemcopy_t)s_hal_utility_fifo_itemofsize02copy;
        } break;

        case 1:
        {   
            fifo->itemaddr  = (hal_utility_fifo_fn_itemaddr_t)s_hal_utility_fifo_itemofsize01addr;
            fifo->itemcopy  = (hal_utility_fifo_fn_itemcopy_t)s_hal_utility_fifo_itemofsize01copy;
        } break;
        
        default:
        {
            fifo->itemaddr  = NULL;  
            fifo->itemcopy  = NULL;            
        } break;
    }
    
    if((NULL != iteminitval) && (NULL != fifo->data))
    {
        uint8_t i = 0;
        for(i=0; i<fifo->capacity; i++)
        {
            memcpy(&fifo->data[i*fifo->sizeofitem], iteminitval, fifo->sizeofitem);
        }
    }
    
    hal_utility_fifo_reset(fifo);
}


extern void hal_utility_fifo_reset(hal_utility_fifo_t *fifo)
{
    // avoid checks .... be careful in calling   
    fifo->size = 0;
    fifo->index = 0;
}

extern void hal_utility_fifo_clear(hal_utility_fifo_t *fifo)
{
    // avoid checks .... be careful in calling   
    memset(fifo->data, 0, fifo->capacity * fifo->sizeofitem);
    
    fifo->size = 0;
    fifo->index = 0;
}



extern hal_result_t hal_utility_fifo_put(hal_utility_fifo_t *fifo, uint8_t *data)
{
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(hal_res_NOK_generic);
    }
    
    if(NULL != fifo->itemcopy)
    {
        fifo->itemcopy(fifo->data, fifo->index, data, 0);
    }
    else
    {
        memcpy(&(fifo->data[fifo->index * fifo->sizeofitem]), data, fifo->sizeofitem);
    }
       
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hal_res_OK);
}


extern hal_result_t hal_utility_fifo_get(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hal_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
 
    if(NULL != fifo->itemcopy)
    {
        fifo->itemcopy(data, 0, fifo->data, start);
    }
    else
    {
        memcpy(data, &(fifo->data[start * fifo->sizeofitem]), fifo->sizeofitem);
    }
    
    fifo->size --;
	*size = fifo->size;
    
    return(hal_res_OK);
}



extern uint8_t * hal_utility_fifo_front(hal_utility_fifo_t *fifo)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(NULL);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    if(NULL != fifo->itemaddr)
    {
        return(fifo->itemaddr(fifo->data, start));  
    }
    else
    {
        return(&(fifo->data[start * fifo->sizeofitem]));
    }
}





extern void hal_utility_fifo_pop(hal_utility_fifo_t *fifo)
{
    // avoid checks .... be careful in calling

    if(0 != fifo->size)
    {
        fifo->size --;
    }
    
    return;
}


extern uint8_t hal_utility_fifo_size(hal_utility_fifo_t *fifo)
{
    return(fifo->size);
}


extern hal_bool_t hal_utility_fifo_full(hal_utility_fifo_t *fifo)
{
    return((fifo->size == fifo->capacity) ? (hal_true) : (hal_false));
}



// -- fast version: size 16

extern uint8_t * hal_utility_fifo_front16(hal_utility_fifo_t *fifo)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(NULL);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }

    return(s_hal_utility_fifo_itemofsize16addr((uint64_t*)fifo->data, start));   
}


extern hal_result_t hal_utility_fifo_get16(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hal_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    
    s_hal_utility_fifo_itemofsize16copy((uint64_t*)data, 0, (uint64_t*)fifo->data, start);

    
    fifo->size --;
    if(NULL != size)
    {
        *size = fifo->size;
    }
    
    return(hal_res_OK);
}


extern hal_result_t hal_utility_fifo_put16(hal_utility_fifo_t *fifo, uint8_t *data)
{
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(hal_res_NOK_generic);
    }
 
    s_hal_utility_fifo_itemofsize16copy((uint64_t*)fifo->data, fifo->index, (uint64_t*)data, 0);
   
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hal_res_OK);
}


// -- fast version: size 08

extern uint8_t * hal_utility_fifo_front08(hal_utility_fifo_t *fifo)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(NULL);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }

    return(s_hal_utility_fifo_itemofsize08addr((uint64_t*)fifo->data, start));   
}


extern hal_result_t hal_utility_fifo_get08(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hal_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    
    s_hal_utility_fifo_itemofsize08copy((uint64_t*)data, 0, (uint64_t*)fifo->data, start);

    
    fifo->size --;
	*size = fifo->size;
    
    return(hal_res_OK);
}


extern hal_result_t hal_utility_fifo_put08(hal_utility_fifo_t *fifo, uint8_t *data)
{
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(hal_res_NOK_generic);
    }
 
    s_hal_utility_fifo_itemofsize08copy((uint64_t*)fifo->data, fifo->index, (uint64_t*)data, 0);
   
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hal_res_OK);
}

// -- fast version: size 04

extern uint8_t * hal_utility_fifo_front04(hal_utility_fifo_t *fifo)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(NULL);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }

    return(s_hal_utility_fifo_itemofsize04addr((uint32_t*)fifo->data, start));   
}


extern hal_result_t hal_utility_fifo_get04(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hal_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    
    s_hal_utility_fifo_itemofsize04copy((uint32_t*)data, 0, (uint32_t*)fifo->data, start);

    
    fifo->size --;
	*size = fifo->size;
    
    return(hal_res_OK);
}


extern hal_result_t hal_utility_fifo_put04(hal_utility_fifo_t *fifo, uint8_t *data)
{
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(hal_res_NOK_generic);
    }
 
    s_hal_utility_fifo_itemofsize04copy((uint32_t*)fifo->data, fifo->index, (uint32_t*)data, 0);
   
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hal_res_OK);
}



// -- fast version: size 02

extern uint8_t * hal_utility_fifo_front02(hal_utility_fifo_t *fifo)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(NULL);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }

    return(s_hal_utility_fifo_itemofsize02addr((uint16_t*)fifo->data, start));   
}


extern hal_result_t hal_utility_fifo_get02(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hal_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    
    s_hal_utility_fifo_itemofsize02copy((uint16_t*)data, 0, (uint16_t*)fifo->data, start);

    
    fifo->size --;
	*size = fifo->size;
    
    return(hal_res_OK);
}


extern hal_result_t hal_utility_fifo_put02(hal_utility_fifo_t *fifo, uint8_t *data)
{
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(hal_res_NOK_generic);
    }
 
    s_hal_utility_fifo_itemofsize02copy((uint16_t*)fifo->data, fifo->index, (uint16_t*)data, 0);
   
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hal_res_OK);
}


// -- fast version: size 01

extern uint8_t * hal_utility_fifo_front01(hal_utility_fifo_t *fifo)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(NULL);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }

    return(s_hal_utility_fifo_itemofsize01addr((uint8_t*)fifo->data, start));   
}


extern hal_result_t hal_utility_fifo_get01(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size)
{
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hal_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    
    s_hal_utility_fifo_itemofsize01copy((uint8_t*)data, 0, (uint8_t*)fifo->data, start);

    
    fifo->size --;
	*size = fifo->size;
    
    return(hal_res_OK);
}


extern hal_result_t hal_utility_fifo_put01(hal_utility_fifo_t *fifo, uint8_t *data)
{
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(hal_res_NOK_generic);
    }
 
    s_hal_utility_fifo_itemofsize01copy((uint8_t*)fifo->data, fifo->index, (uint8_t*)data, 0);
   
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern uint32_t hal_utility_fifo_hid_getsize(const hal_cfg_t *cfg)
{   
    // no memory needed
    return(0);
}

extern hal_result_t hal_utility_fifo_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
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



static void s_hal_utility_fifo_itemofsize01copy(uint8_t* dst, uint32_t offsetdst, uint8_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst);
    src   += (offsetsrc);
    *dst   = *src; 
}

static uint8_t* s_hal_utility_fifo_itemofsize01addr(uint8_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos]));
}



static void s_hal_utility_fifo_itemofsize02copy(uint16_t* dst, uint32_t offsetdst, uint16_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst);
    src   += (offsetsrc);
    *dst   = *src; 
}

static uint8_t* s_hal_utility_fifo_itemofsize02addr(uint16_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos]));
}



static void s_hal_utility_fifo_itemofsize04copy(uint32_t* dst, uint32_t offsetdst, uint32_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst);
    src   += (offsetsrc);
    *dst   = *src; 
}

static uint8_t* s_hal_utility_fifo_itemofsize04addr(uint32_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos]));
}



static void s_hal_utility_fifo_itemofsize08copy(uint64_t* dst, uint32_t offsetdst, uint64_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst);
    src   += (offsetsrc);
    *dst   = *src; 
}

static uint8_t* s_hal_utility_fifo_itemofsize08addr(uint64_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos]));
}



static void s_hal_utility_fifo_itemofsize16copy(uint64_t* dst, uint32_t offsetdst, uint64_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst<<1);
    src   += (offsetsrc<<1);
    *dst++ = *src++;
    *dst   = *src; 
}

static uint8_t* s_hal_utility_fifo_itemofsize16addr(uint64_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos<<1]));
}



#endif//HAL_USE_UTILITY_FIFO

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



