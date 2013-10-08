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

/* @file       EoProtocol.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocol.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_ep_variable_numberofthem_get(eOprotEP_t ep)
{
    uint16_t total = 0;
    switch(ep)
    {
        case eoprot_endpoint_mn_comm:
        {
            total = 0;
        } break;
        
        #warning --> fill the code in eoprot_ep_variables_total_get() by calling proper functions from the various endpoints
    
        default:
        {
            total = 0;
        } break;    
    }
    
    return(total);
}

extern eOprotID_t eoprot_ep_variable_ID_get(eOprotEP_t ep, eOprotEntity_t entity, eOprotNumber_t number, eOprotIndex_t index)
{
    uint32_t id = index & 0xff;
    id |= ((number&0x1f)<<8);
    id |= ((entity&0x03)<<13);
    return((eOprotID_t)id);    
}

extern eOprotEntity_t eoprot_ep_variable_ID2entity(eOprotEP_t ep, eOprotID_t id)
{
    uint32_t ent = ((id>>13)&0x03);
    return((eOprotEntity_t)ent);    
}

extern eOprotNumber_t eoprot_ep_variable_ID2number(eOprotEP_t ep, eOprotID_t id)
{
    uint32_t num = ((id>>8)&0x1f);
    return((eOprotNumber_t)num);    
}

 
extern eOprotIndex_t eoprot_ep_variable_ID2index(eOprotEP_t ep, eOprotID_t id)
{
    uint32_t ind = (id&0xff);
    return((eOprotIndex_t)ind);  
}

extern uint16_t eoprot_ep_ram_sizeof_get(eOprotEP_t ep)
{
    uint16_t size = 0;
    switch(ep)
    {
        case eoprot_endpoint_mn_comm:
        {
            size = 0;
        } break;
        
        #warning --> fill the code in eoprot_ep_variables_total_get() by calling proper functions from the various endpoints
    
        default:
        {
            size = 0;
        } break;    
    }
    
    return(size);
}

extern uint16_t eoprot_ep_variable_ram_sizeof_get(eOprotEP_t ep, eOprotID_t id)
{
    uint16_t size = 0;
    switch(ep)
    {
        case eoprot_endpoint_mn_comm:
        {
            size = 0;
        } break;
        
        #warning --> fill the code in eoprot_ep_variables_total_get() by calling proper functions from the various endpoints
    
        default:
        {
            size = 0;
        } break;    
    }
    
    return(size);
}

extern void* eoprot_ep_variable_ram_extract(void* epram, eOprotEP_t ep, eOprotID_t id)
{
    void* ram = NULL;
     
    switch(ep)
    {
        case eoprot_endpoint_mn_comm:
        {
            ram = NULL;
        } break;
        
        #warning --> fill the code in eoprot_ep_variables_total_get() by calling proper functions from the various endpoints
    
        default:
        {
            ram = NULL;
        } break;    
    }
    
    return(ram);      
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



