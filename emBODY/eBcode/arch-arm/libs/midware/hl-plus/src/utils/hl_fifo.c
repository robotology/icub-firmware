/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       hl_fifo.c
    @brief      this file implements internals of a tool module used in hl
    @author     marco.accame@iit.it
    @date       10/29/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"

#if     defined(HL_USE_UTIL_FIFO)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hl_sys.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_fifo.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_fifo_hid.h" 


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_fifo_itemofsize01copy(uint8_t* dst, uint32_t offsetdst, uint8_t* src, uint32_t offsetsrc);
static uint8_t* s_hl_fifo_itemofsize01addr(uint8_t* pool, uint32_t pos);

static void s_hl_fifo_itemofsize02copy(uint16_t* dst, uint32_t offsetdst, uint16_t* src, uint32_t offsetsrc);
static uint8_t* s_hl_fifo_itemofsize02addr(uint16_t* pool, uint32_t pos);

static void s_hl_fifo_itemofsize04copy(uint32_t* dst, uint32_t offsetdst, uint32_t* src, uint32_t offsetsrc);
static uint8_t* s_hl_fifo_itemofsize04addr(uint32_t* pool, uint32_t pos);

static void s_hl_fifo_itemofsize08copy(uint64_t* dst, uint32_t offsetdst, uint64_t* src, uint32_t offsetsrc);
static uint8_t* s_hl_fifo_itemofsize08addr(uint64_t* pool, uint32_t pos);

static void s_hl_fifo_itemofsize16copy(uint64_t* dst, uint32_t offsetdst, uint64_t* src, uint32_t offsetsrc);
static uint8_t* s_hl_fifo_itemofsize16addr(uint64_t* pool, uint32_t pos);

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


extern hl_fifo_t* hl_fifo_new(uint8_t capacity, uint8_t sizeofitem, uint8_t *iteminitval)
{
    if((0 == capacity) || (0 == sizeofitem))
    {
        return(NULL);
    }
    
    hl_fifo_t* fifo = hl_sys_heap_new(sizeof(struct hl_fifo_hid_t));
    
    if(NULL == fifo)
    {
        return(NULL);
    }
    
    uint8_t *dataforfifo = hl_sys_heap_new(sizeofitem*capacity);

    if(NULL == dataforfifo)
    {
        return(NULL);
    } 

    hl_fifo_init(fifo, capacity, sizeofitem, dataforfifo, iteminitval);
    
    return(fifo);    
}

extern void hl_fifo_delete(hl_fifo_t *fifo)
{
    if(NULL == fifo)
    {
        return;
    }
    
    if(NULL != fifo->data)
    {
        hl_sys_heap_delete(fifo->data);
    }
    
    hl_sys_heap_delete(fifo);    
}


extern hl_result_t hl_fifo_init(hl_fifo_t *fifo, uint8_t capacity, uint8_t sizeofitem, uint8_t *dataforfifo, uint8_t *iteminitval)
{
    if((NULL == fifo) || (NULL == dataforfifo) || (0 == capacity) || (0 == sizeofitem))
    {
        return(hl_res_NOK_generic);
    }
    
    fifo->capacity      = capacity;
    fifo->sizeofitem    = sizeofitem;
    fifo->data          = dataforfifo;
    
    switch(sizeofitem)
    {
        case 16:
        {   
            fifo->itemaddr  = (hl_fifo_fn_itemaddr_t)s_hl_fifo_itemofsize16addr;
            fifo->itemcopy  = (hl_fifo_fn_itemcopy_t)s_hl_fifo_itemofsize16copy;
        } break;
        
        case 8:
        {   
            fifo->itemaddr  = (hl_fifo_fn_itemaddr_t)s_hl_fifo_itemofsize08addr;
            fifo->itemcopy  = (hl_fifo_fn_itemcopy_t)s_hl_fifo_itemofsize08copy;
        } break;
 
        case 4:
        {   
            fifo->itemaddr  = (hl_fifo_fn_itemaddr_t)s_hl_fifo_itemofsize04addr;
            fifo->itemcopy  = (hl_fifo_fn_itemcopy_t)s_hl_fifo_itemofsize04copy;
        } break;
 
        case 2:
        {   
            fifo->itemaddr  = (hl_fifo_fn_itemaddr_t)s_hl_fifo_itemofsize02addr;
            fifo->itemcopy  = (hl_fifo_fn_itemcopy_t)s_hl_fifo_itemofsize02copy;
        } break;

        case 1:
        {   
            fifo->itemaddr  = (hl_fifo_fn_itemaddr_t)s_hl_fifo_itemofsize01addr;
            fifo->itemcopy  = (hl_fifo_fn_itemcopy_t)s_hl_fifo_itemofsize01copy;
        } break;
        
        default:
        {	// prefer using NULL rather than a standard function to avoid adding a 5-th argument which uses stack
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
    
    hl_fifo_reset(fifo);
    
    return(hl_res_OK);
}


extern void hl_fifo_reset(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return;
    }
#endif 
    
    // avoid checks .... be careful in calling   
    fifo->size = 0;
    fifo->index = 0;
}


extern void hl_fifo_clear(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return;
    }
#endif 
    
    // avoid checks .... be careful in calling   
    memset(fifo->data, 0, fifo->capacity * fifo->sizeofitem);
    
    fifo->size = 0;
    fifo->index = 0;
}


extern hl_result_t hl_fifo_put(hl_fifo_t *fifo, uint8_t *data)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo) // dat can be NULL
    {
        return(hl_res_NOK_generic);
    }
#endif  
    
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(hl_res_NOK_generic);
    }
    
    // cannot move up this control because it is legal to have data equal to NULL. in this case we dont copy. 
    if(NULL != data)
    {   // copy data inside ...
        if(NULL != fifo->itemcopy)
        {
            fifo->itemcopy(fifo->data, fifo->index, data, 0);
        }
        else
        {
            memcpy(&(fifo->data[fifo->index * fifo->sizeofitem]), data, fifo->sizeofitem);
        }
    }
    // else just advance the index and size
       
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hl_res_OK);
}


extern uint8_t * hl_fifo_end(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return(NULL);
    }
#endif  
    
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(NULL);
    }
        
    return(&(fifo->data[fifo->index * fifo->sizeofitem]));    
}


extern hl_result_t hl_fifo_get(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if((NULL == fifo) || (NULL == data))
    {
        return(hl_res_NOK_generic);
    }
#endif  
    
    // avoid checks .... be careful in calling
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        if(NULL != remaining)
        {
            *remaining = 0;
        }
        return(hl_res_NOK_nodata);
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
    if(NULL != remaining)
    {
        *remaining = fifo->size;
    }
    
    return(hl_res_OK);
}


extern uint8_t * hl_fifo_front(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return(NULL);
    }
#endif 
    
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


extern void hl_fifo_pop(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return;
    }
#endif 
    
    // avoid checks .... be careful in calling

    if(0 != fifo->size)
    {
        fifo->size --;
    }
    
    return;
}


extern uint8_t hl_fifo_size(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return(0);
    }
#endif   
    
    return(fifo->size);
}


extern hl_bool_t hl_fifo_full(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo) 
    {
        return(hl_true);
    }
#endif 
    
    return((fifo->size == fifo->capacity) ? (hl_true) : (hl_false));
}


// -- fast version: size 16

extern uint8_t * hl_fifo_front16(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return(NULL);
    }
#endif
    
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

    return(s_hl_fifo_itemofsize16addr((uint64_t*)fifo->data, start));   
}


extern hl_result_t hl_fifo_get16(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if((NULL == fifo) || (NULL == data))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hl_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    
    s_hl_fifo_itemofsize16copy((uint64_t*)data, 0, (uint64_t*)fifo->data, start);

    
    fifo->size --;
    if(NULL != remaining)
    {
        *remaining = fifo->size;
    }
    
    return(hl_res_OK);
}


extern hl_result_t hl_fifo_put16(hl_fifo_t *fifo, uint8_t *data)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo) // data can be NULL
    {
        return(hl_res_NOK_generic);
    }
#endif  
    
    if(fifo->size == fifo->capacity)
    {
        return(hl_res_NOK_generic);
    }
    
    // cannot move up this control because it is legal to have data equal to NULL. in this case we dont copy. 
    if(NULL != data)
    {   // copy data inside ... 
        s_hl_fifo_itemofsize16copy((uint64_t*)fifo->data, fifo->index, (uint64_t*)data, 0);
    }
    // else just advance the index and size
    
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hl_res_OK);
}


// -- fast version: size 08

extern uint8_t * hl_fifo_front08(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return(NULL);
    }
#endif
    
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

    return(s_hl_fifo_itemofsize08addr((uint64_t*)fifo->data, start));   
}


extern hl_result_t hl_fifo_get08(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if((NULL == fifo) || (NULL == data))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hl_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
       
    s_hl_fifo_itemofsize08copy((uint64_t*)data, 0, (uint64_t*)fifo->data, start);

    
    fifo->size --;
    if(NULL != remaining)
    {
        *remaining = fifo->size;
    }
    
    return(hl_res_OK);
}


extern hl_result_t hl_fifo_put08(hl_fifo_t *fifo, uint8_t *data)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo) // data can be NULL
    {
        return(hl_res_NOK_generic);
    }
#endif  
    
    if(fifo->size == fifo->capacity)
    {
        return(hl_res_NOK_generic);
    }

    // cannot move up this control because it is legal to have data equal to NULL. in this case we dont copy. 
    if(NULL != data)
    {   // copy data inside ...
        s_hl_fifo_itemofsize08copy((uint64_t*)fifo->data, fifo->index, (uint64_t*)data, 0);
    }
    // else just advance the index and size
   
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hl_res_OK);
}


// -- fast version: size 04

extern uint8_t * hl_fifo_front04(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return(NULL);
    }
#endif
    
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

    return(s_hl_fifo_itemofsize04addr((uint32_t*)fifo->data, start));   
}


extern hl_result_t hl_fifo_get04(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if((NULL == fifo) || (NULL == data))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hl_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    
    s_hl_fifo_itemofsize04copy((uint32_t*)data, 0, (uint32_t*)fifo->data, start);

    
    fifo->size --;
    if(NULL != remaining)
    {
        *remaining = fifo->size;
    }
    
    return(hl_res_OK);
}


extern hl_result_t hl_fifo_put04(hl_fifo_t *fifo, uint8_t *data)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo) // data can be NULL
    {
        return(hl_res_NOK_generic);
    }
#endif  
    
    if(fifo->size == fifo->capacity)
    {
        return(hl_res_NOK_generic);
    }
 
    // cannot move up this control because it is legal to have data equal to NULL. in this case we dont copy. 
    if(NULL != data)
    {   // copy data inside ...
        s_hl_fifo_itemofsize04copy((uint32_t*)fifo->data, fifo->index, (uint32_t*)data, 0);
    }
    // else just advance the index and size
    
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hl_res_OK);
}



// -- fast version: size 02

extern uint8_t * hl_fifo_front02(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return(NULL);
    }
#endif
    
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

    return(s_hl_fifo_itemofsize02addr((uint16_t*)fifo->data, start));   
}


extern hl_result_t hl_fifo_get02(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if((NULL == fifo) || (NULL == data))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hl_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    
    s_hl_fifo_itemofsize02copy((uint16_t*)data, 0, (uint16_t*)fifo->data, start);

    
    fifo->size --;
    if(NULL != remaining)
    {
        *remaining = fifo->size;
    }
    
    return(hl_res_OK);
}


extern hl_result_t hl_fifo_put02(hl_fifo_t *fifo, uint8_t *data)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo) // data can be NULL
    {
        return(hl_res_NOK_generic);
    }
#endif 
    
    if(fifo->size == fifo->capacity)
    {
        return(hl_res_NOK_generic);
    }

    // cannot move up this control because it is legal to have data equal to NULL. in this case we dont copy. 
    if(NULL != data)
    {   // copy data inside ...    
        s_hl_fifo_itemofsize02copy((uint16_t*)fifo->data, fifo->index, (uint16_t*)data, 0);
    }
    // else just advance the index and size
   
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hl_res_OK);
}


// -- fast version: size 01

extern uint8_t * hl_fifo_front01(hl_fifo_t *fifo)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo)
    {
        return(NULL);
    }
#endif
    
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

    return(s_hl_fifo_itemofsize01addr((uint8_t*)fifo->data, start));   
}


extern hl_result_t hl_fifo_get01(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if((NULL == fifo) || (NULL == data))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    uint32_t start = 0;
    
    if(0 == fifo->size)
    {
        return(hl_res_NOK_nodata);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    
    
    s_hl_fifo_itemofsize01copy((uint8_t*)data, 0, (uint8_t*)fifo->data, start);

    
    fifo->size --;
    if(NULL != remaining)
    {
        *remaining = fifo->size;
    }
    
    return(hl_res_OK);
}


extern hl_result_t hl_fifo_put01(hl_fifo_t *fifo, uint8_t *data)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == fifo) // data can be NULL
    {
        return(hl_res_NOK_generic);
    }
#endif 
    
    if(fifo->size == fifo->capacity)
    {
        return(hl_res_NOK_generic);
    }
 
    // cannot move up this control because it is legal to have data equal to NULL. in this case we dont copy. 
    if(NULL != data)
    {   // copy data inside ...
        s_hl_fifo_itemofsize01copy((uint8_t*)fifo->data, fifo->index, (uint8_t*)data, 0);
    }
    // else just advance the index and size
   
    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
    return(hl_res_OK);
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


static void s_hl_fifo_itemofsize01copy(uint8_t* dst, uint32_t offsetdst, uint8_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst);
    src   += (offsetsrc);
    *dst   = *src; 
}


static uint8_t* s_hl_fifo_itemofsize01addr(uint8_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos]));
}


static void s_hl_fifo_itemofsize02copy(uint16_t* dst, uint32_t offsetdst, uint16_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst);
    src   += (offsetsrc);
    *dst   = *src; 
}


static uint8_t* s_hl_fifo_itemofsize02addr(uint16_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos]));
}


static void s_hl_fifo_itemofsize04copy(uint32_t* dst, uint32_t offsetdst, uint32_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst);
    src   += (offsetsrc);
    *dst   = *src; 
}


static uint8_t* s_hl_fifo_itemofsize04addr(uint32_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos]));
}


static void s_hl_fifo_itemofsize08copy(uint64_t* dst, uint32_t offsetdst, uint64_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst);
    src   += (offsetsrc);
    *dst   = *src; 
}


static uint8_t* s_hl_fifo_itemofsize08addr(uint64_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos]));
}


static void s_hl_fifo_itemofsize16copy(uint64_t* dst, uint32_t offsetdst, uint64_t* src, uint32_t offsetsrc)
{
    dst   += (offsetdst<<1);
    src   += (offsetsrc<<1);
    *dst++ = *src++;
    *dst   = *src; 
}


static uint8_t* s_hl_fifo_itemofsize16addr(uint64_t* pool, uint32_t pos)
{
    return((uint8_t*)&(pool[pos<<1]));
}



#endif//defined(HL_USE_UTIL_FIFO)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



