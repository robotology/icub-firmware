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
EO_VERIFYproposition(eoprot_ep_sk_ded34, eoprot_ep_sk_entities_numberof <= eoprot_entities_maxnumberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_ep_sk_tagsnum_sk, eoprot_ep_sk_tags_skin_numberof == eoprot_ep_sk_rwmodes_skin_numberof);
EO_VERIFYproposition(eoprot_ep_sk_tagsmax_sk, eoprot_ep_sk_tags_skin_numberof <= eoprot_tags_maxnumberof);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_sk_brdentity2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity);

static uint16_t s_eoprot_ep_sk_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id);

static eObool_t s_eoprot_ep_sk_tag_skin_is_valid(eOprotTag_t tag);


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
    if(NULL == s_eoprot_ep_sk_board_numberofeachentity)
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

extern eObool_t eoprot_ep_sk_variables_id_isvalid(eOprotBRD_t brd, eOprotID32_t id)
{
    eObool_t ret = eobool_false;    
    
    if((NULL == s_eoprot_ep_sk_board_numberofeachentity) || (NULL == s_eoprot_ep_sk_board_numberofeachentity[brd]))
    {
        return(eobool_false);
    }    
    
    eOprotEntity_t ent = eoprot_ep_variable_ID2entity(id);
    eOprotIndex_t  ind = eoprot_ep_variable_ID2index(id);
    eOprotTag_t    tag = eoprot_ep_variable_ID2tag(id);
    
    switch(ent)
    {
        case eosk_entity_skin:
        {   
            if(ind < s_eoprot_ep_sk_board_numberofeachentity[brd][eosk_entity_skin])
            {
                ret = s_eoprot_ep_sk_tag_skin_is_valid(tag);
            }            
        } break;
        
        default:
        {           
        } break;        
    }
    
    return(ret);     
}


extern uint16_t eoprot_ep_sk_variables_numberof_Get(eOprotBRD_t brd)
{
    uint16_t num = 0;
    
    if((NULL == s_eoprot_ep_sk_board_numberofeachentity) || (NULL == s_eoprot_ep_sk_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    num = eoprot_ep_sk_tags_skin_numberof * s_eoprot_ep_sk_board_numberofeachentity[brd][eosk_entity_skin];
          
    return(num);
}

extern eOprotID32_t eoprot_ep_sk_variable_idfromprognumber_Get(eOprotBRD_t brd, eOprotPROGnum_t prog)
{
    eOprotTag_t tag = 0xff;
    eOprotIndex_t index = 0xff;
    eOprotEntity_t entity = 0xff;
    
    if((NULL == s_eoprot_ep_sk_board_numberofeachentity) || (NULL == s_eoprot_ep_sk_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }
    
    eOprotPROGnum_t progsinskins = eoprot_ep_sk_tags_skin_numberof * s_eoprot_ep_sk_board_numberofeachentity[brd][eosk_entity_skin];
    
    if((0 != progsinskins) && (prog < (progsinskins)))
    {   // entity is eosk_entity_skin 
        entity  = eosk_entity_skin;
        index   = prog / eoprot_ep_sk_tags_skin_numberof;       // eoprot_ep_sk_tags_skin_numberof cannot be zero if progsinskins is non-zero
        tag     = prog % eoprot_ep_sk_tags_skin_numberof;        
    }
    else
    {
        return(EOK_uint32dummy);
    }
    
    return(eoprot_ep_variable_ID_get(eoprot_endpoint_skin, entity, index, tag));
    
}

extern eOprotPROGnum_t eoprot_ep_sk_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID32_t id)
{
    eOprotPROGnum_t prog = EOK_uint32dummy;
    
    if((NULL == s_eoprot_ep_sk_board_numberofeachentity) || (NULL == s_eoprot_ep_sk_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(id);
    eOprotIndex_t index = eoprot_ep_variable_ID2index(id);
    switch(entity)
    {
        case eosk_entity_skin: 
        {
            prog = index*eoprot_ep_sk_tags_skin_numberof + eoprot_ep_sk_rom_get_prognum(id);
        } break;
        
                
        default:
        {   
            prog = EOK_uint32dummy;
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
    
    size = sizeof(eOsk_skin_t) * s_eoprot_ep_sk_board_numberofeachentity[brd][eosk_entity_skin];
    
    return(size);
}

extern uint16_t eoprot_ep_sk_variable_ram_sizeof_Get(eOprotID32_t id)
{
    return(eoprot_ep_sk_rom_get_sizeofvar(id));
}

extern void* eoprot_ep_sk_entity_ram_Extract(eOprotBRD_t brd, eOprotEntity_t ent, void* epram)
{
    uint8_t* startofdata = epram;
    uint16_t offset = s_eoprot_ep_sk_brdentity2ramoffset(brd, ent);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
}

extern void* eoprot_ep_sk_variable_ram_Extract(eOprotBRD_t brd, eOprotID32_t id, void* epram)
{
    uint8_t* startofdata = epram;
    uint16_t offset = s_eoprot_ep_sk_brdid2ramoffset(brd, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
} 

extern void* eoprot_ep_sk_variable_rom_Get(eOprotID32_t id)
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

static uint16_t s_eoprot_ep_sk_brdentity2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity)
{
    uint16_t offset = 0;
    
    switch(entity)
    {
        case eosk_entity_skin:
        {   // the skin are all displaced at the beginning of the data in every sk endpoint
            offset = 0; 
        } break;   
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;    
    }
    
    return(offset);  
}


static uint16_t s_eoprot_ep_sk_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id)
{
    uint16_t offset = 0;
    eOprotTag_t tag = eoprot_ep_variable_ID2tag(id);
    
    if(EOK_uint08dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(id);
    uint16_t index = eoprot_ep_variable_ID2index(id);
    
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

static eObool_t s_eoprot_ep_sk_tag_skin_is_valid(eOprotTag_t tag)
{   // in case of holes in tags ... change the code
    return((tag < eoprot_ep_sk_tags_skin_numberof)?(eobool_true):(eobool_false));
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



