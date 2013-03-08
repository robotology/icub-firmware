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


/* @file       circular.c
    @brief      this file implements internals of a tool module used in hal
    @author     marco.accame@iit.it
    @date       10/29/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"


// http://stackoverflow.com/questions/827691/how-do-you-implement-a-circular-buffer-in-c

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "circular.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


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

static uint8_t s_circular_curr(circular_t *circ)
{
    return(circ->index);  
}

static uint8_t s_circular_next(circular_t *circ)
{
    uint8_t next = 0;
    
    next = circ->index + 1;
    if(next == circ->capacity)
    {
        next = 0;
    }
    
    return(next);  
}

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




extern void circular_init(circular_t *circ, uint8_t capacity, uint8_t sizeofitem)
{
    // avoid checks .... be careful in callingitem
    
    
    circ->capacity      = capacity;
    circ->sizeofitem    = sizeofitem;
    circ->data          = calloc(capacity*sizeofitem, 1);
    
    
    circular_reset(circ);
}


extern void circular_reset(circular_t *circ)
{
    // avoid checks .... be careful in calling   
    circ->earliest = 0;
    circ->index = 0;
    circ->size = 0;
}

extern void circular_clear(circular_t *circ)
{
    // avoid checks .... be careful in calling   
    memset(circ->data, 0, circ->capacity * circ->sizeofitem);
    
    circ->earliest = 0;
    circ->index = 0;
    circ->size = 0;
}



extern void circular_put(circular_t *circ, uint8_t *data)
{
    // avoid checks .... be careful in calling
        
    uint8_t* dest = &circ->data[circ->index * circ->sizeofitem];
          
    if(NULL!= data)
    {
        memcpy(dest, data, circ->sizeofitem);
    }
    
    circ->size ++;
    if(circ->size > circ->capacity)
    {
        circ->size = circ->capacity;
    }
    
    // then just advance the index      
    circ->index = s_circular_next(circ);
    
    if(circ->size == circ->capacity)
    {
        circ->earliest = circ->index;
    }
    
}



extern void circular_get_from_earliest(circular_t *circ, uint8_t pos, uint8_t *data)
{
    uint32_t position = 0;
    
    if(NULL == data)
    {
        return;
    }
    
    pos = pos % circ->capacity;
    
    position = circ->earliest + pos;
    if(position >= circ->capacity)
    {
        position -= circ->capacity;
    }
 
    memcpy(data, &(circ->data[position * circ->sizeofitem]), circ->sizeofitem);
}

extern void circular_get_from_latest_minus(circular_t *circ, uint8_t pos, uint8_t *data)
{
    int32_t position = 0;
    
    if(NULL == data)
    {
        return;
    }
    
    pos = pos % circ->capacity;
    
    position = circ->index - 1 - pos;
    if(position < 0)
    {
        position += circ->capacity;
    }
 
    memcpy(data, &(circ->data[position * circ->sizeofitem]), circ->sizeofitem);
}


extern uint8_t circular_size(circular_t *circ)
{
    return(circ->size);
}


extern uint8_t circular_full(circular_t *circ)
{
    return((circ->size == circ->capacity) ? (1) : (0));
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





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



