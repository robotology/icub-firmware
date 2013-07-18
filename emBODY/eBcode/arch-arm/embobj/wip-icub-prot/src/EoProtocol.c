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

#include "EoProtocolMC.h"
#include "EoProtocolMN.h"
#include "EoProtocolAS.h"
#include "EoProtocolSK.h"


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

static eOprotBRD_t s_eoprot_localboard = 255;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// current implementation depends on the fact taht we have the 32 bits of the id assigned to ep-entity-index-tag
// with 8-8-8-8 bits. thus we put a guard which fails compilation in case those values change.
EO_VERIFYproposition(eoprot_ep_1233, 256 == eoprot_endpoints_maxnumberof);
EO_VERIFYproposition(eoprot_ep_1234, 256 == eoprot_entities_maxnumberof);
EO_VERIFYproposition(eoprot_ep_1235, 256 == eoprot_indices_maxnumberof);
EO_VERIFYproposition(eoprot_ep_1236, 256 == eoprot_tags_maxnumberof);

extern eOprotID32_t eoprot_ID_get(eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOprotTag_t tag)
{
    uint32_t id = (uint32_t)tag & 0xff;
    id |= (((uint32_t)index & 0xff)<<8);
    id |= (((uint32_t)entity & 0xff)<<16);
    id |= (((uint32_t)ep & 0xff)<<24);
    return((eOprotID32_t)id);    
}

extern eOprotEndpoint_t eoprot_ID2endpoint(eOprotID32_t id)
{
    uint32_t end = (((uint32_t)id>>24) & 0xff);
    return((eOprotEndpoint_t)end);    
}

extern eOprotEntity_t eoprot_ID2entity(eOprotID32_t id)
{
    uint32_t ent = (((uint32_t)id>>16) & 0xff);
    return((eOprotEntity_t)ent);    
}

extern eOprotIndex_t eoprot_ID2index(eOprotID32_t id)
{
    uint32_t index = (((uint32_t)id>>8) & 0xff);
    return((eOprotIndex_t)index);    
}


extern eOprotTag_t eoprot_ID2tag(eOprotID32_t id)
{
    uint32_t tag = ((uint32_t)id & 0xff);
    return((eOprotTag_t)tag);  
}


extern eOresult_t eoprot_config_board_numberof(uint8_t numofboards)
{   
    // when and if we allow dynamic allocation of numboards ...
    if(numofboards > eoprot_boards_maxnumberof)
    {
        return(eores_NOK_generic);
    }
    return(eores_OK);
}

extern eOresult_t eoprot_config_board_local(eOprotBRD_t brd)
{
    eOresult_t res = eores_OK;
    
    if(brd >= eoprot_boards_maxnumberof)
    {
        return(eores_NOK_generic);
    }
    
    s_eoprot_localboard = brd;
    s_eoprot_localboard = s_eoprot_localboard;

    return(res);
}

extern eOresult_t eoprot_config_endpoint_entities(eOprotBRD_t brd, eOprotEndpoint_t ep, const uint8_t* numberofentities)
{
    eOresult_t res = eores_OK;
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    } 
    
    if((brd >= eoprot_boards_maxnumberof) || (NULL == numberofentities))
    {
        return(eores_NOK_generic);
    }
    
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            eoprot_mn_config_endpoint_entities(brd, numberofentities);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            eoprot_mc_config_endpoint_entities(brd, numberofentities);
        } break;  
         
        case eoprot_endpoint_analogsensors:
        {
            eoprot_as_config_endpoint_entities(brd, numberofentities);
        } break;     
         
        case eoprot_endpoint_skin:
        {
            eoprot_sk_config_endpoint_entities(brd, numberofentities);
        } break;  

        default:
        {
            res = eores_NOK_generic;
        } break;    
    }
    
    return(res);
}

extern uint16_t eoprot_endpoint_sizeof_get(eOprotBRD_t brd, eOprotEndpoint_t ep)
{
    uint16_t size = 0;
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }     
    
    if((brd >= eoprot_boards_maxnumberof))
    {
        return(0);
    }
    
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            size = eoprot_mn_endpoint_sizeof_get(brd);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            size = eoprot_mc_endpoint_sizeof_get(brd);
        } break;  
         
        case eoprot_endpoint_analogsensors:
        {
            size = eoprot_as_endpoint_sizeof_get(brd);
        } break;     
         
        case eoprot_endpoint_skin:
        {
            size = eoprot_sk_endpoint_sizeof_get(brd);
        } break;  

        default:
        {
            size = 0;
        } break;    
    }
    
    return(size);
}


extern eOresult_t eoprot_config_endpoint_ram(eOprotBRD_t brd, eOprotEndpoint_t ep, uint16_t sizeofram, void* ram)
{
    eOresult_t res = eores_OK;
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }     
    
    if((brd >= eoprot_boards_maxnumberof) || (0 == sizeofram) || (NULL == ram))
    {
        return(eores_NOK_generic);
    }
    
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            eoprot_mn_config_endpoint_ram(brd, ram);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            eoprot_mc_config_endpoint_ram(brd, ram);
        } break;  
         
        case eoprot_endpoint_analogsensors:
        {
            eoprot_as_config_endpoint_ram(brd, ram);
        } break;     
         
        case eoprot_endpoint_skin:
        {
            eoprot_sk_config_endpoint_ram(brd, ram);
        } break;  

        default:
        {
            res = eores_NOK_generic;
        } break;    
    }
    
    return(res);
}



extern void* eoprot_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id)
{
    void* ram = NULL;
    
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id);
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    } 
     
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            ram = eoprot_mn_ramofvariable_get(brd, id);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            ram = eoprot_mc_ramofvariable_get(brd, id);
        } break;  
         
        case eoprot_endpoint_analogsensors:
        {
            ram = eoprot_as_ramofvariable_get(brd, id);
        } break;     
         
        case eoprot_endpoint_skin:
        {
            ram = eoprot_sk_ramofvariable_get(brd, id);
        } break;  

        default:
        {
            ram = NULL;
        } break;    
    }

    return(ram);      
}

extern uint16_t eoprot_variable_sizeof_get(eOprotBRD_t brd, eOprotID32_t id)
{
    uint16_t size = 0;
    
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id);
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }     

    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            size = eoprot_mn_variable_sizeof_get(id);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            size = eoprot_mc_variable_sizeof_get(id);
        } break;  
         
        case eoprot_endpoint_analogsensors:
        {
            size = eoprot_as_variable_sizeof_get(id);
        } break;     
         
        case eoprot_endpoint_skin:
        {
            size = eoprot_sk_variable_sizeof_get(id);
        } break;  

        default:
        {
            size = 0;
        } break;    
    }
    
    return(size);
}


extern void* eoprot_entity_ramof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index)
{
    void* ram = NULL;
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }     
         
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            ram = eoprot_mn_entity_ramof_get(brd, entity, index);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            ram = eoprot_mc_entity_ramof_get(brd, entity, index);
        } break;  
         
        case eoprot_endpoint_analogsensors:
        {
            ram = eoprot_as_entity_ramof_get(brd, entity, index);
        } break;     
         
        case eoprot_endpoint_skin:
        {
            ram = eoprot_sk_entity_ramof_get(brd, entity, index);
        } break;  

        default:
        {
            ram = NULL;
        } break;    
    }


    return(ram);      
}


extern uint16_t eoprot_entity_sizeof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity)
{
    uint16_t size = 0;
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }     

    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            size = eoprot_mn_entity_sizeof_get(brd, entity);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            size = eoprot_mc_entity_sizeof_get(brd, entity);
        } break;  
         
        case eoprot_endpoint_analogsensors:
        {
            size = eoprot_as_entity_sizeof_get(brd, entity);
        } break;     
         
        case eoprot_endpoint_skin:
        {
            size = eoprot_sk_entity_sizeof_get(brd, entity);
        } break;  

        default:
        {
            size = 0;
        } break;    
    }
    
    return(size);
}


extern uint8_t eoprot_entity_numberof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity)
{
    uint8_t numberof = 0;
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }     

    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            numberof = eoprot_mn_entity_numberof_get(brd, entity);
        } break;
        
        case eoprot_endpoint_motioncontrol:
        {
            numberof = eoprot_mc_entity_numberof_get(brd, entity);
        } break;  
         
        case eoprot_endpoint_analogsensors:
        {
            numberof = eoprot_as_entity_numberof_get(brd, entity);
        } break;     
         
        case eoprot_endpoint_skin:
        {
            numberof = eoprot_sk_entity_numberof_get(brd, entity);
        } break;  

        default:
        {
            numberof = 0;
        } break;    
    }
    
    return(numberof);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



