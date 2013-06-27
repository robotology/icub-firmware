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

/* @file       EoProtocolEPsk.c
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

#include "EoProtocolEPsk_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPsk.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if !defined(EOCFG_EPS_BOARDS_MAXNUMBER)
    #define EOCFG_EPS_BOARDS_MAXNUMBER 10
#endif



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// - guard on max number of entities
EO_VERIFYproposition(eoprot_ep_sk_ded34, eoprot_ep_sk_entities_numberof <= eoprot_entities_numberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_ep_sk_tagsnum_sk, eoprot_ep_sk_skin_tags_numberof == eoprot_ep_sk_skin_rwmodes_numberof);
EO_VERIFYproposition(eoprot_ep_sk_tagsmax_sk, eoprot_ep_sk_skin_tags_numberof <= eoprot_tags_numberof);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_sk_brdid2ramoffset(eOprotBRD_t brd, eOprotID_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if 0   // dynamic mode
static const uint8_t** s_eoprot_ep_sk_board_numberofeachentity = NULL; // s_eoprot_ep_sk_board_numberofeachentity[brd][entype] tells how many entities of kind entype are in brd
static uint16_t s_eoprot_ep_sk_board_numberofthem = 0;
#endif
static const uint8_t* s_eoprot_ep_sk_board_numberofeachentity[EOCFG_EPS_BOARDS_MAXNUMBER] = { NULL };
static const uint16_t s_eoprot_ep_sk_board_numberofthem = EOCFG_EPS_BOARDS_MAXNUMBER;

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#if 0   // dynamic mode
extern eOresult_t eoprot_ep_sk_number_of_boards_Load(uint16_t numofboards)
{
    if((0 != s_eoprot_ep_sk_board_numberofthem) || (NULL != s_eoprot_ep_sk_board_numberofeachentity))
    {
        return(eores_NOK_generic);
    }
    
    s_eoprot_ep_sk_board_numberofthem = numofboards;
    s_eoprot_ep_sk_board_numberofeachentity = (const uint8_t**) calloc(numofboards, sizeof(uint8_t*));
    
    return(eores_OK);   
}
#endif

extern eOresult_t eoprot_ep_sk_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity)
{
    if(NULL != s_eoprot_ep_sk_board_numberofeachentity)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_ep_sk_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_ep_sk_board_numberofeachentity[brd] = numberofeachentity;
    
    return(eores_OK);       
}


extern uint16_t eoprot_ep_sk_variables_numberof_Get(eOprotBRD_t brd)
{
    uint16_t num = 0;
    
    if((NULL == s_eoprot_ep_sk_board_numberofeachentity) || (NULL == s_eoprot_ep_sk_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    num = eoprot_ep_sk_skin_tags_numberof * s_eoprot_ep_sk_board_numberofeachentity[brd][eosk_entity_skin];
          
    return(num);
}

extern uint16_t eoprot_ep_sk_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID_t id)
{
    uint16_t prog = 0;
    
    if((NULL == s_eoprot_ep_sk_board_numberofeachentity) || (NULL == s_eoprot_ep_sk_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_skin, id);
    eOprotIndex_t index = eoprot_ep_variable_ID2index(eoprot_endpoint_skin, id);
    switch(entity)
    {
        case eosk_entity_skin: 
        {
            prog = index*eoprot_ep_sk_skin_tags_numberof + eoprot_ep_sk_rom_get_prognum(id);
        } break;
        
                
        default:
        {   
            prog = 0;
        } break;    
    }
    
    return(prog);
}

extern uint16_t eoprot_ep_sk_skins_numberof_Get(eOprotBRD_t brd)
{
    if((NULL == s_eoprot_ep_sk_board_numberofeachentity) || (NULL == s_eoprot_ep_sk_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    return(s_eoprot_ep_sk_board_numberofeachentity[brd][eosk_entity_skin]);
}

extern uint16_t eoprot_ep_sk_ram_sizeof_Get(eOprotBRD_t brd)
{
    uint16_t size = 0;
    
    if((NULL == s_eoprot_ep_sk_board_numberofeachentity) || (NULL == s_eoprot_ep_sk_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    //#warning --> it is valid only if organisation of memory is skin appl
    size = sizeof(eOsk_skin_t) * s_eoprot_ep_sk_board_numberofeachentity[brd][eosk_entity_skin];
    
    return(size);
}

extern uint16_t eoprot_ep_sk_variable_ram_sizeof_Get(eOprotID_t id)
{
    return(eoprot_ep_sk_rom_get_sizeofvar(id));
}

extern void* eoprot_ep_sk_variable_ram_Extract(void* epram, eOprotBRD_t brd, eOprotID_t id)
{
    uint8_t* startofdata = epram;
    uint16_t offset = s_eoprot_ep_sk_brdid2ramoffset(brd, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
} 

extern void* eoprot_ep_sk_variable_rom_Get(eOprotID_t id)
{
    return(eoprot_ep_sk_rom_get_nvrom(id));
}

extern const eOsk_skin_t* eoprot_ep_sk_skin_default_Get(void)
{
    return(&eoprot_ep_sk_rom_skin_defaultvalue);
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_sk_brdid2ramoffset(eOprotBRD_t brd, eOprotID_t id)
{
    uint16_t offset = 0;
    uint16_t tag = eoprot_ep_variable_ID2tag(eoprot_endpoint_skin, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_skin, id);
    uint16_t index = eoprot_ep_variable_ID2index(eoprot_endpoint_skin, id);
    
    switch(entity)
    {
        case eosk_entity_skin:
        {   // the skin are all displaced at the beginning of the data in every sk endpoint
            offset = index*sizeof(eOsk_skin_t) + eoprot_ep_sk_rom_skin_get_offset(tag); 
        } break;   
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;    
    }
    
    return(offset);  
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



