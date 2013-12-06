/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOarray.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOarray_hid.h" 


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOarray";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOarray* eo_array_New(uint8_t capacity, uint8_t itemsize, void *memory)
{
    EOarray *retptr = NULL;   
    uint8_t capacity08 = (uint8_t)capacity;     
    uint8_t itemsize08 = (uint8_t)itemsize;
    
//    eo_errman_Assert(eo_errman_GetHandle(), (capacity08 == capacity), s_eobj_ownname, ":eo_array_New() has capacity > uint8_t");
//    eo_errman_Assert(eo_errman_GetHandle(), (itemsize08 == itemsize), s_eobj_ownname, ":eo_array_New() has itemsize > uint8_t");
    
    if(NULL != memory)
    {   // i use external memory
        retptr = memory;
    }
    else
    {   // i get the memory for the object
        retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOarray_head_t) + capacity08*itemsize08, 1);
    }

    retptr->head.capacity       = capacity08;
    retptr->head.itemsize       = itemsize08;
    retptr->head.size           = 0;
    
    eo_array_Reset(retptr);

    return(retptr);
}

extern eOresult_t eo_array_Reset(EOarray *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    p->head.size = 0;
    memset(p->data, 0, (uint16_t)p->head.capacity*p->head.itemsize);

    return(eores_OK);
}

extern void eo_array_Resize(EOarray *p, uint8_t size)
{
    uint8_t *start = NULL;
    uint16_t first;
    uint16_t last;
    
    if(NULL == p)
    {
        return;     // invalid array
    }
    
    if(size > p->head.size)
    {
        return;     // nothing to do
    }
    
    if(size > p->head.capacity)
    {
        return;     // nothing to do
    }

    if(size < p->head.size)
    {
        first   = size;
        last    = p->head.size;
    }
    else
    {
        first   = p->head.size;
        last    = size;
    }
    
    // new size
    p->head.size = size;
    
    
    // clear the removed elements or the added ones     
    start = (uint8_t*) (p->data);
    memset(&start[(uint32_t)(first*p->head.itemsize)], 0, (last-first)*p->head.itemsize);

    return;
}

extern uint8_t eo_array_Capacity(EOarray *p)
{
    if(NULL == p)
    {
        return(0);
    }

    return((uint8_t)p->head.capacity);
}

extern uint8_t eo_array_ItemSize(EOarray *p)
{
    if(NULL == p)
    {
        return(0);
    }

    return(p->head.itemsize);
}

extern uint8_t eo_array_Size(EOarray *p)
{
    if(NULL == p)
    {
        return(0);
    }


    return(p->head.size);
}

extern uint16_t eo_array_UsedBytes(EOarray *p)
{
    if(NULL == p)
    {
        return(0);
    }

    if(p->head.capacity < p->head.size)
    {
        // there must be an error
        return(0);
    }

    return(sizeof(eOarray_head_t) + (uint16_t)p->head.size*p->head.itemsize);
}

extern eOresult_t eo_array_PushBack(EOarray *p, const void *item)
{
    if((NULL == p) || (NULL == item))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(p->head.size == p->head.capacity)
    {
        return(eores_NOK_generic);
    }
    
    memcpy(&(p->data[(uint16_t)p->head.size*p->head.itemsize]), item, p->head.itemsize);
    p->head.size++;

    return(eores_OK);
}



extern void * eo_array_At(EOarray *p, uint8_t pos)
{

    if((NULL == p) || (pos >= p->head.capacity))
    {
        return(NULL);
    }

    return(&(p->data[(uint16_t)pos*p->head.itemsize]));
}

extern eOresult_t eo_array_PopBack(EOarray *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(0 == p->head.size)
    {
        return(eores_NOK_generic);
    }
    p->head.size--;

    return(eores_OK);
}


extern void eo_array_AssignOne(EOarray *p, uint8_t pos, const void *item) 
{
    uint8_t *start = NULL;
        
    if((NULL == p) || (NULL == item)) 
    {    
        // invalid data
        return;    
    }
    
    if(pos >= p->head.capacity) 
    { 
        // beyond the capacity of the array
        return;
    }
 
    
    if(pos >= p->head.size)
    {
        eo_array_Resize(p, pos+1); 
    }
    
    // now fill the pos-th position w/ object item
        
    start = (uint8_t*) (p->data);
    // cast to uint16_t to tell the reader that index of array start[] can be bigger
    start = &start[(uint16_t)pos * p->head.size]; 
    
    memcpy(start, item, p->head.itemsize);
    
    return; 
}

extern void eo_array_Assign(EOarray *p, uint8_t pos, const void *items, uint8_t nitems)
{
    uint8_t *start = NULL;
        
    if((NULL == p) || (NULL == items) || (0 == nitems)) 
    {    
        // invalid data
        return;    
    }
    
    if((pos+nitems-1) >= p->head.capacity) 
    { 
        // beyond the capacity of the array
        return;
    }
 
    
    if((pos+nitems-1) >= p->head.size)
    {
        eo_array_Resize(p, pos+nitems); 
    }
    
    // now fill from the pos-th position until the (pos+nitems-1)-th position w/ object items
        
    start = (uint8_t*) (p->data);
    // cast to uint16_t to tell the reader that index of array start[] can be bigger
    start = &start[(uint16_t)pos * p->head.size]; 
    
    memcpy(start, items, (uint16_t)nitems*p->head.itemsize);
    
    return;     
    
    
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




