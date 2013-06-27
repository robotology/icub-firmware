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

/* @file       EoProtocolEPas.c
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

#include "EoProtocolEPas_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPas.h"


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
EO_VERIFYproposition(eoprot_ep_as_ded34, eoprot_ep_as_entities_numberof <= eoprot_entities_numberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_ep_as_tagsnum_st, eoprot_ep_as_strain_tags_numberof == eoprot_ep_as_strain_rwmodes_numberof);
EO_VERIFYproposition(eoprot_ep_as_tagsmax_st, eoprot_ep_as_strain_tags_numberof <= eoprot_tags_numberof);

EO_VERIFYproposition(eoprot_ep_as_tagsnum_ma, eoprot_ep_as_mais_tags_numberof == eoprot_ep_as_mais_rwmodes_numberof);
EO_VERIFYproposition(eoprot_ep_as_tagsmax_ma, eoprot_ep_as_mais_tags_numberof <= eoprot_tags_numberof);


 

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_as_brdid2ramoffset(eOprotBRD_t brd, eOprotID_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if 0   // dynamic mode
static const uint8_t** s_eoprot_ep_as_board_numberofeachentity = NULL; // s_eoprot_ep_as_board_numberofeachentity[brd][entype] tells how many entities of kind entype are in brd
static uint16_t s_eoprot_ep_as_board_numberofthem = 0;
#endif
static const uint8_t* s_eoprot_ep_as_board_numberofeachentity[EOCFG_EPS_BOARDS_MAXNUMBER] = { NULL };
static const uint16_t s_eoprot_ep_as_board_numberofthem = EOCFG_EPS_BOARDS_MAXNUMBER;

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


#if 0   // dynamic mode
extern eOresult_t eoprot_ep_as_number_of_boards_Load(uint16_t numofboards)
{
    if((0 != s_eoprot_ep_as_board_numberofthem) || (NULL != s_eoprot_ep_as_board_numberofeachentity))
    {
        return(eores_NOK_generic);
    }
    
    s_eoprot_ep_as_board_numberofthem = numofboards;
    s_eoprot_ep_as_board_numberofeachentity = (const uint8_t**) calloc(numofboards, sizeof(uint8_t*));
    
    return(eores_OK);   
}
#endif

extern eOresult_t eoprot_ep_as_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity)
{
    if(NULL == s_eoprot_ep_as_board_numberofeachentity)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_ep_as_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_ep_as_board_numberofeachentity[brd] = numberofeachentity;
    
    return(eores_OK);       
}


extern uint16_t eoprot_ep_as_variables_numberof_Get(eOprotBRD_t brd)
{
    uint16_t num = 0;
    
    if((NULL == s_eoprot_ep_as_board_numberofeachentity) || (NULL == s_eoprot_ep_as_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    num = eoprot_ep_as_strain_tags_numberof * s_eoprot_ep_as_board_numberofeachentity[brd][eoas_entity_strain] +
          eoprot_ep_as_mais_tags_numberof * s_eoprot_ep_as_board_numberofeachentity[brd][eoas_entity_mais];
          
    return(num);
}

extern eOprotID_t eoprot_ep_as_variable_idfromprognumber_Get(eOprotBRD_t brd, uint16_t prog)
{   
    eOprotTag_t tag = 0xffff;
    eOprotIndex_t index = 0xff;
    eOprotEntity_t entity = 0xff;
    
    if((NULL == s_eoprot_ep_as_board_numberofeachentity) || (NULL == s_eoprot_ep_as_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    uint16_t progsinstrains = eoprot_ep_as_strain_tags_numberof * s_eoprot_ep_as_board_numberofeachentity[brd][eoas_entity_strain];
    uint16_t progsinmaises = eoprot_ep_as_mais_tags_numberof * s_eoprot_ep_as_board_numberofeachentity[brd][eoas_entity_mais];
    
    if((0 != progsinstrains) && (prog < (progsinstrains)))
    {   // entity is eoas_entity_strain 
        entity  = eoas_entity_strain;
        index   = prog / eoprot_ep_as_strain_tags_numberof;       // eoprot_ep_as_strain_tags_numberof cannot be zero if progsinstrains is non-zero
        tag     = prog % eoprot_ep_as_strain_tags_numberof;        
    }
    else if((0 != progsinmaises) && (prog < (progsinstrains + progsinmaises)))
    {   // entity is eoas_entity_mais 
        prog -= progsinstrains;
        entity  = eoas_entity_mais;
        index   = prog / eoprot_ep_as_mais_tags_numberof;       // eoprot_ep_as_mais_tags_numberof cannot be zero if progsinmaises is non-zero
        tag     = prog % eoprot_ep_as_mais_tags_numberof;                      
    }
    else
    {
        return(EOK_uint16dummy);
    }
    
    return(eoprot_ep_variable_ID_get(eoprot_endpoint_analogsensors, entity, index, tag));
    
}

extern uint16_t eoprot_ep_as_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID_t id)
{
    uint16_t prog = 0;
    
    if((NULL == s_eoprot_ep_as_board_numberofeachentity) || (NULL == s_eoprot_ep_as_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_analogsensors, id);
    eOprotIndex_t index = eoprot_ep_variable_ID2index(eoprot_endpoint_analogsensors, id);
    switch(entity)
    {
        case eoas_entity_strain: 
        {
            prog = index*eoprot_ep_as_strain_tags_numberof + eoprot_ep_as_rom_get_prognum(id);
        } break;
        
        case eoas_entity_mais: 
        {
            prog = s_eoprot_ep_as_board_numberofeachentity[brd][eoas_entity_strain]*eoprot_ep_as_strain_tags_numberof +
                   index*eoprot_ep_as_mais_tags_numberof + eoprot_ep_as_rom_get_prognum(id);
        } break; 
                
        default:
        {   
            prog = 0;
        } break;    
    }
    
    return(prog);
}

extern uint16_t eoprot_ep_as_strains_numberof_Get(eOprotBRD_t brd)
{
    if((NULL == s_eoprot_ep_as_board_numberofeachentity) || (NULL == s_eoprot_ep_as_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    return(s_eoprot_ep_as_board_numberofeachentity[brd][eoas_entity_strain]);
}

extern uint16_t eoprot_ep_as_maises_numberof_Get(eOprotBRD_t brd)
{
    if((NULL == s_eoprot_ep_as_board_numberofeachentity) || (NULL == s_eoprot_ep_as_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    return(s_eoprot_ep_as_board_numberofeachentity[brd][eoas_entity_mais]);
}


extern uint16_t eoprot_ep_as_ram_sizeof_Get(eOprotBRD_t brd)
{
    uint16_t size = 0;
    
    if((NULL == s_eoprot_ep_as_board_numberofeachentity) || (NULL == s_eoprot_ep_as_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    //#warning --> it is valid only if organisation of memory is strain mais
    size = sizeof(eOas_strain_t) * s_eoprot_ep_as_board_numberofeachentity[brd][eoas_entity_strain] +
           sizeof(eOas_mais_t) * s_eoprot_ep_as_board_numberofeachentity[brd][eoas_entity_mais];
    
    return(size);
}

extern uint16_t eoprot_ep_as_variable_ram_sizeof_Get(eOprotID_t id)
{
    return(eoprot_ep_as_rom_get_sizeofvar(id));
}

extern void* eoprot_ep_as_variable_ram_Extract(void* epram, eOprotBRD_t brd, eOprotID_t id)
{
    uint8_t* startofdata = epram;
    uint16_t offset = s_eoprot_ep_as_brdid2ramoffset(brd, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
} 

extern void* eoprot_ep_as_variable_rom_Get(eOprotID_t id)
{
    return(eoprot_ep_as_rom_get_nvrom(id));
}

extern const eOas_strain_t* eoprot_ep_as_strain_default_Get(void)
{
    return(&eoprot_ep_as_rom_strain_defaultvalue);
}

extern const eOas_mais_t* eoprot_ep_as_mais_default_Get(void)
{
    return(&eoprot_ep_as_rom_mais_defaultvalue);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_as_brdid2ramoffset(eOprotBRD_t brd, eOprotID_t id)
{
    uint16_t offset = 0;
    uint16_t tag = eoprot_ep_variable_ID2tag(eoprot_endpoint_analogsensors, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    //#warning --> it is valid only if organisation of memory is jo[n], mo[m], c[l]
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_analogsensors, id);
    uint16_t index = eoprot_ep_variable_ID2index(eoprot_endpoint_analogsensors, id);
    
    switch(entity)
    {
        case eoas_entity_strain:
        {   // the strain are all displaced at the beginning of the data in every mc endpoint
            offset = index*sizeof(eOas_strain_t) + eoprot_ep_as_rom_strain_get_offset(tag); 
        } break;
        
        case eoas_entity_mais:
        {   // the maises are placed after all the strains (if any strains ...). 
            uint16_t numberofstrains = eoprot_ep_as_strains_numberof_Get(brd);
            offset = numberofstrains*sizeof(eOas_strain_t) + index*sizeof(eOas_mais_t) + eoprot_ep_as_rom_mais_get_offset(tag);  
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



