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
#warning --> later on u must add teh proper functions





extern uint16_t eoprot_ep_sk_variables_numberof_Get(eOprotEP_t ep)
{
    return(0);
}





extern uint16_t eoprot_ep_sk_variable_progressivenumber_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(0);
}



extern uint16_t eoprot_ep_sk_ram_sizeof_Get(eOprotEP_t ep)
{
    return(0);
}



extern uint16_t eoprot_ep_sk_variable_ram_sizeof_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(0);
}


extern void* eoprot_ep_sk_variable_ram_extract(void* epram, eOprotEP_t ep, eOprotID_t id)
{
    return(NULL);
}




extern void* eoprot_ep_sk_variable_romdescriptor_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(NULL);
}

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

EO_VERIFYproposition(eoprot_ep_aaa, eoprot_endpoint_maxnum_in_category > eoprot_endpoint_offset_highestvalue);


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

extern eOprot_endpoint_category_t eoprot_ep_category_get(eOprotEP_t ep)
{
    eOprot_endpoint_category_t cat = eoprot_ep_category_none;
    uint16_t base = ep & 0x00F0;
    switch(base)
    {
        case eoprot_ep_base_mn:         cat = eoprot_ep_category_management;        break;
        case eoprot_ep_base_mc:         cat = eoprot_ep_category_motioncontrol;     break;
        case eoprot_ep_base_as:         cat = eoprot_ep_category_analogsensor;      break;
        case eoprot_ep_base_sk:         cat = eoprot_ep_category_skin;              break;
        default:                        cat = eoprot_ep_category_none;              break;
    }
    return(cat);
}

extern uint16_t eoprot_ep_variables_numberof_get(eOprotEP_t ep)
{
    uint16_t total = 0;
    eOprot_endpoint_category_t cat = eoprot_ep_category_get(ep);
    
    switch(cat)
    {
        case eoprot_ep_category_management:
        {
            total = eoprot_ep_mn_variables_numberof_Get(ep);
        } break;
        
        case eoprot_ep_category_motioncontrol:
        {
            total = eoprot_ep_mc_variables_numberof_Get(ep);
        } break;  
        
        case eoprot_ep_category_analogsensor:
        {
            total = eoprot_ep_as_variables_numberof_Get(ep);
        } break;     
        
        case eoprot_ep_category_skin:
        {
            total = eoprot_ep_sk_variables_numberof_Get(ep);
        } break;  

        default:
        {
            total = 0;
        } break;    
    }
    
    return(total);
}


extern eOprotProgNumber_t eoprot_ep_variable_ID2prognumber(eOprotEP_t ep, eOprotID_t id)
{
    uint16_t prognum = EOK_uint16dummy;
    eOprot_endpoint_category_t cat = eoprot_ep_category_get(ep);
    
    switch(cat)
    {
        case eoprot_ep_category_management:
        {
            prognum = eoprot_ep_mn_variable_progressivenumber_Get(ep, id);
        } break;
        
        case eoprot_ep_category_motioncontrol:
        {
            prognum = eoprot_ep_mc_variable_progressivenumber_Get(ep, id);
        } break;  
        
        case eoprot_ep_category_analogsensor:
        {
            prognum = eoprot_ep_as_variable_progressivenumber_Get(ep, id);
        } break;     
        
        case eoprot_ep_category_skin:
        {
            prognum = eoprot_ep_sk_variable_progressivenumber_Get(ep, id);
        } break;  

        default:
        {
            prognum = EOK_uint16dummy;
        } break;    
    }
    
    return(prognum);
}


extern eOprotID_t eoprot_ep_variable_ID_get(eOprotEP_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOprotTag_t tag)
{
    uint32_t id = tag & 0xff;
    id |= ((index&0x1f)<<8);
    id |= ((entity&0x03)<<13);
    return((eOprotID_t)id);    
}

extern eOprotEntity_t eoprot_ep_variable_ID2entity(eOprotEP_t ep, eOprotID_t id)
{
    uint32_t ent = ((id>>13)&0x03);
    return((eOprotEntity_t)ent);    
}

extern eOprotIndex_t eoprot_ep_variable_ID2index(eOprotEP_t ep, eOprotID_t id)
{
    uint32_t index = ((id>>8)&0x1f);
    return((eOprotIndex_t)index);    
}


extern eOprotTag_t eoprot_ep_variable_ID2tag(eOprotEP_t ep, eOprotID_t id)
{
    uint32_t tag = (id&0xff);
    return((eOprotTag_t)tag);  
}


extern uint16_t eoprot_ep_ram_sizeof_get(eOprotEP_t ep)
{
    uint16_t size = 0;
    eOprot_endpoint_category_t cat = eoprot_ep_category_get(ep);
    
    switch(cat)
    {
        case eoprot_ep_category_management:
        {
            size = eoprot_ep_mn_ram_sizeof_Get(ep);
        } break;
        
        case eoprot_ep_category_motioncontrol:
        {
            size = eoprot_ep_mc_ram_sizeof_Get(ep);
        } break;  
        
        case eoprot_ep_category_analogsensor:
        {
            size = eoprot_ep_as_ram_sizeof_Get(ep);
        } break;     
        
        case eoprot_ep_category_skin:
        {
            size = eoprot_ep_sk_ram_sizeof_Get(ep);
        } break;  

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
    eOprot_endpoint_category_t cat = eoprot_ep_category_get(ep);
    
    switch(cat)
    {
        case eoprot_ep_category_management:
        {
            size = eoprot_ep_mn_variable_ram_sizeof_Get(ep, id);
        } break;
        
        case eoprot_ep_category_motioncontrol:
        {
            size = eoprot_ep_mc_variable_ram_sizeof_Get(ep, id);
        } break;  
        
        case eoprot_ep_category_analogsensor:
        {
            size = eoprot_ep_as_variable_ram_sizeof_Get(ep, id);
        } break;     
        
        case eoprot_ep_category_skin:
        {
            size = eoprot_ep_sk_variable_ram_sizeof_Get(ep, id);
        } break;  

        default:
        {
            size = 0;
        } break;    
    }
    
    return(size);
}

extern void* eoprot_ep_variable_ram_Extract(void* epram, eOprotEP_t ep, eOprotID_t id)
{
    void* ram = NULL;
     
    eOprot_endpoint_category_t cat = eoprot_ep_category_get(ep);
    
    switch(cat)
    {
        case eoprot_ep_category_management:
        {
            ram = eoprot_ep_mn_variable_ram_Extract(epram, ep, id);
        } break;
        
        case eoprot_ep_category_motioncontrol:
        {
            ram = eoprot_ep_mc_variable_ram_Extract(epram, ep, id);
        } break;  
        
        case eoprot_ep_category_analogsensor:
        {
            ram = eoprot_ep_as_variable_ram_Extract(epram, ep, id);
        } break;     
        
        case eoprot_ep_category_skin:
        {
            ram = eoprot_ep_sk_variable_ram_extract(epram, ep, id);
        } break;  

        default:
        {
            ram = NULL;
        } break;    
    }
    
    return(ram);      
}

extern const void* eoprot_ep_variable_rom_get(eOprotEP_t ep, eOprotID_t id)
{
    void* rom = NULL;
     
    eOprot_endpoint_category_t cat = eoprot_ep_category_get(ep);
    
    switch(cat)
    {
        case eoprot_ep_category_management:
        {
            rom = eoprot_ep_mn_variable_rom_Get(ep, id);
        } break;
        
        case eoprot_ep_category_motioncontrol:
        {
            rom = eoprot_ep_mc_variable_rom_Get(ep, id);
        } break;  
        
        case eoprot_ep_category_analogsensor:
        {
            rom = eoprot_ep_as_variable_rom_Get(ep, id);
        } break;     
        
        case eoprot_ep_category_skin:
        {
            rom = eoprot_ep_sk_variable_romdescriptor_Get(ep, id);
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



