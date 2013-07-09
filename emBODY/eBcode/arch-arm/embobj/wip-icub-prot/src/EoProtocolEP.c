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

/* @file       EoProtocolEP.c
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

#include "EoProtocolEPmc.h"
#include "EoProtocolEPmn.h"
#include "EoProtocolEPas.h"
#include "EoProtocolEPsk.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEP.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



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



extern uint16_t eoprot_ep_variables_numberof_get(eOprotBRD_t brd, eOprotEP_t ep)
{
    uint16_t total = 0;

    switch(ep)
    { 
        case eoprot_endpoint_management:
        {
            total = eoprot_ep_mn_variables_numberof_Get(brd);
        } break;
     
        case eoprot_endpoint_motioncontrol:
        {
            total = eoprot_ep_mc_variables_numberof_Get(brd);
        } break;  
     
        case eoprot_endpoint_analogsensors:
        {
            total = eoprot_ep_as_variables_numberof_Get(brd);
        } break;     
       
        case eoprot_endpoint_skin:
        {
            total = eoprot_ep_sk_variables_numberof_Get(brd);
        } break;  

        default:
        {
            total = 0;
        } break;    
    }
    
    return(total);
}


// current implementation depends on the fact taht we have the 32 bits of the id assigned to ep-entity-index-tag
// with 8-8-8-8 bits. thus we put a guard which fails compilation in case those values change.
EO_VERIFYproposition(eoprot_ep_1233, 256 == eoprot_endpoints_maxnumberof);
EO_VERIFYproposition(eoprot_ep_1234, 256 == eoprot_entities_maxnumberof);
EO_VERIFYproposition(eoprot_ep_1235, 256 == eoprot_indices_maxnumberof);
EO_VERIFYproposition(eoprot_ep_1236, 256 == eoprot_tags_maxnumberof);

extern eOprotID32_t eoprot_ep_variable_ID_get(eOprotEP_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOprotTag_t tag)
{
    uint32_t id = (uint32_t)tag & 0xff;
    id |= (((uint32_t)index & 0xff)<<8);
    id |= (((uint32_t)entity & 0xff)<<16);
    id |= (((uint32_t)ep & 0xff)<<24);
    return((eOprotID32_t)id);    
}

extern eOprotEP_t eoprot_ep_variable_ID2endpoint(eOprotID32_t id)
{
    uint32_t end = (((uint32_t)id>>24) & 0xff);
    return((eOprotEP_t)end);    
}

extern eOprotEntity_t eoprot_ep_variable_ID2entity(eOprotID32_t id)
{
    uint32_t ent = (((uint32_t)id>>16) & 0xff);
    return((eOprotEntity_t)ent);    
}

extern eOprotIndex_t eoprot_ep_variable_ID2index(eOprotID32_t id)
{
    uint32_t index = (((uint32_t)id>>8) & 0xff);
    return((eOprotIndex_t)index);    
}


extern eOprotTag_t eoprot_ep_variable_ID2tag(eOprotID32_t id)
{
    uint32_t tag = ((uint32_t)id & 0xff);
    return((eOprotTag_t)tag);  
}


extern eOprotProgNumber_t eoprot_ep_variable_prognumber_get(eOprotBRD_t brd, eOprotID32_t id)
{
    eOprotProgNumber_t prognum = EOK_uint32dummy;
    
    eOprotEP_t ep = eoprot_ep_variable_ID2endpoint(id);
    
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            prognum = eoprot_ep_mn_variable_progressivenumber_Get(brd, id);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            prognum = eoprot_ep_mc_variable_progressivenumber_Get(brd, id);
        } break;  
        
        case eoprot_endpoint_analogsensors:
        {
            prognum = eoprot_ep_as_variable_progressivenumber_Get(brd, id);
        } break;     
        
        case eoprot_endpoint_skin:
        {
            prognum = eoprot_ep_sk_variable_progressivenumber_Get(brd, id);
        } break;  

        default:
        {
            prognum = EOK_uint32dummy;
        } break;    
    }

    return(prognum);
}



extern uint16_t eoprot_ep_ram_sizeof_get(eOprotBRD_t brd, eOprotEP_t ep)
{
    uint16_t size = 0;

    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            size = eoprot_ep_mn_ram_sizeof_Get(brd);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            size = eoprot_ep_mc_ram_sizeof_Get(brd);
        } break;  
        
        case eoprot_endpoint_analogsensors:
        {
            size = eoprot_ep_as_ram_sizeof_Get(brd);
        } break;     
        
        case eoprot_endpoint_skin:
        {
            size = eoprot_ep_sk_ram_sizeof_Get(brd);
        } break;  

        default:
        {
            size = 0;
        } break;    
    }
    
    return(size);
}


extern void* eoprot_ep_entity_ram_extract(eOprotBRD_t brd, eOprotEP_t ep, eOprotEntity_t ent, void* epram)
{
    void* ram = NULL;
         
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            ram = eoprot_ep_mn_entity_ram_Extract(brd, ent, epram);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            ram = eoprot_ep_mc_entity_ram_Extract(brd, ent, epram);
        } break;  
        
        case eoprot_endpoint_analogsensors:
        {
            ram = eoprot_ep_as_entity_ram_Extract(brd, ent, epram);
        } break;     
        
        case eoprot_endpoint_skin:
        {
            ram = eoprot_ep_sk_entity_ram_Extract(brd, ent, epram);
        } break;  

        default:
        {
            ram = NULL;
        } break;    
    }

    return(ram);      
}


extern uint16_t eoprot_ep_variable_ram_sizeof_get(eOprotID32_t id)
{
    uint16_t size = 0;
    
    eOprotEP_t ep = eoprot_ep_variable_ID2endpoint(id);

    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            size = eoprot_ep_mn_variable_ram_sizeof_Get(id);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            size = eoprot_ep_mc_variable_ram_sizeof_Get(id);
        } break;  
        
        case eoprot_endpoint_analogsensors:
        {
            size = eoprot_ep_as_variable_ram_sizeof_Get(id);
        } break;     
        
        case eoprot_endpoint_skin:
        {
            size = eoprot_ep_sk_variable_ram_sizeof_Get(id);
        } break;  

        default:
        {
            size = 0;
        } break;    
    }
    
    return(size);
}



extern void* eoprot_ep_variable_ram_extract(eOprotBRD_t brd, eOprotID32_t id, void* epram)
{
    void* ram = NULL;
    
    eOprotEP_t ep = eoprot_ep_variable_ID2endpoint(id);
     
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            ram = eoprot_ep_mn_variable_ram_Extract(brd, id, epram);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            ram = eoprot_ep_mc_variable_ram_Extract(brd, id, epram);
        } break;  
        
        case eoprot_endpoint_analogsensors:
        {
            ram = eoprot_ep_as_variable_ram_Extract(brd, id, epram);
        } break;     
        
        case eoprot_endpoint_skin:
        {
            ram = eoprot_ep_sk_variable_ram_Extract(brd, id, epram);
        } break;  

        default:
        {
            ram = NULL;
        } break;    
    }

    return(ram);      
}


extern const void* eoprot_ep_variable_rom_get(eOprotID32_t id)
{
    void* rom = NULL;
    
    eOprotEP_t ep = eoprot_ep_variable_ID2endpoint(id);
        
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            rom = eoprot_ep_mn_variable_rom_Get(id);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            rom = eoprot_ep_mc_variable_rom_Get(id);
        } break;  
        
        case eoprot_endpoint_analogsensors:
        {
            rom = eoprot_ep_as_variable_rom_Get(id);
        } break;     
        
        case eoprot_endpoint_skin:
        {
            rom = eoprot_ep_sk_variable_rom_Get(id);
        } break;  

        default:
        {
            rom = NULL;
        } break;    
    }    
    
    return(rom);       
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



