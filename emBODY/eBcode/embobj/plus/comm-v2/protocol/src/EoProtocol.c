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


#include "EoProtocolEPs.h"


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

static void* s_eoprot_eonvrom_get(eOprotBRD_t brd, eOprotID32_t id);

static uint16_t s_eoprot_endpoint_numberofvariables_get(eOprotBRD_t brd, eOprotEndpoint_t ep);
static uint16_t s_eoprot_brdentityindex2ramoffset(eOprotBRD_t brd, uint8_t epi, eOprotEntity_t entity, eOprotIndex_t index);
static uint16_t s_eoprot_brdid2ramoffset(eOprotBRD_t brd, uint8_t epi, eOprotID32_t id);
static eObool_t s_eoprot_entity_tag_is_valid(uint8_t epi, eOprotEntity_t entity, eOprotTag_t tag);

static uint16_t s_eoprot_rom_get_offset(uint8_t epi, eOprotEntity_t entity, eOprotTag_t tag);

static uint16_t s_eoprot_rom_epid2index_of_folded_descriptors(uint8_t epi, eOprotID32_t id);
static void* s_eoprot_rom_get_nvrom(uint8_t epi, eOprotID32_t id);
static uint16_t s_eoprot_rom_entity_offset_of_tag(uint8_t epi, uint8_t ent, eOprotTag_t tag);
static uint16_t s_eoprot_rom_get_sizeofvar(uint8_t epi, eOprotID32_t id);
static uint16_t s_eoprot_rom_get_prognum(eOprotID32_t id);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static eOprotBRD_t s_eoprot_localboard = 255;

extern const uint8_t* eoprot_board_numberofeachentity[eoprot_boards_maxnumberof][eoprot_endpoints_numberof] = { NULL };
extern void* eoprot_board_ramofeachendpoint[eoprot_boards_maxnumberof][eoprot_endpoints_numberof] = { NULL };
extern eObool_fp_uint32_t eoprot_board_isvarcached_fns[eoprot_boards_maxnumberof] = { NULL };

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const eOprot_nvset_Interface_t eoprot_eonvset_Interface =
{
    EO_INIT(.loadram)               eoprot_config_endpoint_ram,
    EO_INIT(.getvarsnumberof)       s_eoprot_endpoint_numberofvariables_get,       
    EO_INIT(.isidsupported)         eoprot_id_isvalid,     
    EO_INIT(.getid)                 eoprot_prognum2id,            
    EO_INIT(.getprognumber)         eoprot_id2prognum,     
    EO_INIT(.getrom)                s_eoprot_eonvrom_get,             
    EO_INIT(.getram)                eoprot_variable_ramof_get,
    EO_INIT(.isvarcached)           eoprot_variable_is_cached    
};



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
    
    if(ep > eoprot_endpoints_numberof)
    {
        return(eores_NOK_generic);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);
            
    eoprot_board_numberofeachentity[brd][epi] = numberofentities;    
        
    return(res);
}

extern eOresult_t eoprot_config_cached_variables(eOprotBRD_t brd, eObool_fp_uint32_t isvarcached_fn)
{
    eOresult_t res = eores_OK;
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    } 
    
    if((brd >= eoprot_boards_maxnumberof) || (NULL == isvarcached_fn))
    {
        return(eores_NOK_generic);
    }
       
            
    eoprot_board_isvarcached_fns[brd] = isvarcached_fn;    

    
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

    if(ep > eoprot_endpoints_numberof)
    {
        return(0);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);
    
    if(NULL == eoprot_board_numberofeachentity[brd][epi])
    {
        return(0);
    }
    
    uint8_t i;
    for(i=0; i<eoprot_ep_entities_numberof[epi]; i++)
    {
        size += eoprot_ep_entities_sizeof[epi][i] * eoprot_board_numberofeachentity[brd][epi][i];
    }
    
    return(size);
}


extern eOresult_t eoprot_config_endpoint_ram(eOprotBRD_t brd, eOprotEndpoint_t ep, void* ram, uint16_t sizeofram)
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

    if(ep > eoprot_endpoints_numberof)
    {
        return(eores_NOK_generic);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);    
        
    eoprot_board_ramofeachendpoint[brd][epi] = ram;    
        
    return(res);
}



extern void* eoprot_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id)
{
   
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id);
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    } 
     

    if(ep > eoprot_endpoints_numberof)
    {
        return(NULL);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep); 
    
    
    uint8_t* startofdata = (uint8_t*)eoprot_board_ramofeachendpoint[brd][epi];
    
    if(NULL == startofdata)
    {
        return(NULL);
    }
    
    uint16_t offset = s_eoprot_brdid2ramoffset(brd, epi, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);    
 
}

extern uint16_t eoprot_variable_sizeof_get(eOprotBRD_t brd, eOprotID32_t id)
{
    uint16_t size = 0;
    
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id);
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }     

    if(ep > eoprot_endpoints_numberof)
    {
        return(0);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);
    
    size = s_eoprot_rom_get_sizeofvar(epi, id);
    
    
    return(size);
}

extern eObool_t eoprot_variable_is_cached(eOprotBRD_t brd, eOprotID32_t id)
{
    eObool_t res = eobool_false;
    eObool_fp_uint32_t fptr = NULL;

    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }  
    
    if(brd >= eoprot_boards_maxnumberof)
    {
        return(res);
    }

    fptr = eoprot_board_isvarcached_fns[brd];
    
    if(NULL != fptr)
    {
        res = fptr(id);
    }
    
    return(res);
}


extern void* eoprot_entity_ramof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index)
{ 
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }    

    if(ep > eoprot_endpoints_numberof)
    {
        return(0);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);

    uint8_t* startofdata = (uint8_t*)eoprot_board_ramofeachendpoint[brd][epi];
    if(NULL == startofdata)
    {
        return(NULL);
    }
    uint16_t offset = s_eoprot_brdentityindex2ramoffset(brd, epi, entity, index);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]); 
}


extern uint16_t eoprot_entity_sizeof_get(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotEntity_t entity)
{
    uint16_t size = 0;
    
    if(eoprot_board_localboard == brd)
    {
        brd = s_eoprot_localboard;
    }  

    if(ep > eoprot_endpoints_numberof)
    {
        return(0);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);
    
    if(entity < eoprot_ep_entities_numberof[epi])
    {
        size = eoprot_ep_entities_sizeof[epi][entity];
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

    if(ep > eoprot_endpoints_numberof)
    {
        return(0);
    }  

    uint8_t epi = eoprot_ep_ep2index(ep);
    
    if(NULL == eoprot_board_numberofeachentity[brd][epi])
    {
        return(0);
    }

    if(entity < eoprot_ep_entities_numberof[epi])
    {
        numberof = eoprot_board_numberofeachentity[brd][ep][entity];
    }
    
    return(numberof);
}


extern eObool_t eoprot_id_isvalid(eOprotBRD_t brd, eOnvID32_t id)
{
    // just verifies that the entity, index, and tag have numbers which are consistent with their maximum number
    
    eOprotEntity_t ent = eoprot_ID2entity(id);
    eOprotIndex_t  ind = eoprot_ID2index(id);
    eOprotTag_t    tag = eoprot_ID2tag(id);
    
    
    uint8_t epi = eoprot_ep_ep2index(eoprot_ID2endpoint(id));
            
    
    if(ent >= eoprot_ep_entities_numberof[epi])
    {
        return(eobool_false);
    }
    
    if(NULL == eoprot_board_numberofeachentity[brd][epi])
    {
        return(eobool_false);
    }      
    
    if(ind >= eoprot_board_numberofeachentity[brd][epi][ent])
    {
        return(eobool_false);
    }   
    
    return(s_eoprot_entity_tag_is_valid(epi, ent, tag));
}


extern eOprotID32_t eoprot_prognum2id(eOprotBRD_t brd, eOprotEndpoint_t ep, eOprotProgNumber_t prog)
{
    eOprotTag_t tag = 0xff;
    eOprotIndex_t index = 0xff;
    eOprotEntity_t entity = 0xff;
    
    if(ep > eoprot_endpoints_numberof)
    {
        return(EOK_uint32dummy);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);
    
    if(NULL == eoprot_board_numberofeachentity[brd][epi])
    {
        return(EOK_uint32dummy);
    }
    
    
    uint8_t i;
    for(i=0; i<eoprot_ep_entities_numberof[epi]; i++)
    {
        // starting from the first entity (if it present in the board) we progressively check if the signedprog is in its relevant range.
        uint8_t tags_number_ith = eoprot_ep_tags_numberof[epi][i];
        eOprotProgNumber_t progs_ith = tags_number_ith * eoprot_board_numberofeachentity[brd][epi][i]; // num of progs in all the entities i-th
        if((0 != progs_ith) && (prog < (progs_ith)))
        {   // entity is the i-th 
            entity  = i;
            index   = prog / tags_number_ith;       // tags_number_ith cannot be zero if progs_ith is non-zero
            tag     = prog % tags_number_ith;  

            return(eoprot_ID_get(ep, entity, index, tag));
        }
        prog -= progs_ith; // entity may be the next one. 
    }

    return(EOK_uint32dummy);   
}


extern eOprotProgNumber_t eoprot_id2prognum(eOprotBRD_t brd, eOprotID32_t id)
{
    eOprotProgNumber_t prog = 0;
    
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id);
    
    if(ep > eoprot_endpoints_numberof)
    {
        return(EOK_uint32dummy);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);
    
    if(NULL == eoprot_board_numberofeachentity[brd][epi])
    {
        return(EOK_uint32dummy);
    }
    
    eOprotEntity_t entity = eoprot_ID2entity(id);
    eOprotIndex_t  index  = eoprot_ID2index(id);
    
    if(entity >= eoprot_ep_entities_numberof[epi])
    {
        return(EOK_uint32dummy);
    }
    
    uint8_t i;
    for(i=0; i<entity; i++)
    {   // we add all the tags in the entities below
        prog += (eoprot_ep_tags_numberof[epi][i] * eoprot_board_numberofeachentity[brd][epi][i]);
    }
    // then we add only the tags of the entities equal to the current one + the progessive number of the tag
    prog += (index*eoprot_ep_tags_numberof[epi][entity] + s_eoprot_rom_get_prognum(id));

    return(prog);
}    


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void* s_eoprot_eonvrom_get(eOprotBRD_t brd, eOprotID32_t id)
{
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id);
    
    if(ep > eoprot_endpoints_numberof)
    {
        return(NULL);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);
    
    brd =  brd;
    return(s_eoprot_rom_get_nvrom(epi, id));
}

static uint16_t s_eoprot_endpoint_numberofvariables_get(eOprotBRD_t brd, eOprotEndpoint_t ep)
{
    uint16_t num = 0;
    
    if(ep > eoprot_endpoints_numberof)
    {
        return(0);
    }
    
    uint8_t epi = eoprot_ep_ep2index(ep);
    
    if(NULL == eoprot_board_numberofeachentity[brd][epi])
    {
        return(0);
    }
    
    uint8_t i;
    for(i=0; i<eoprot_ep_entities_numberof[epi]; i++)
    {   
        // simply the sum for each entity of the number of tags multiplied the number of each entity. 
        // it also works if an entity is not present in the board
        num += (eoprot_ep_tags_numberof[epi][i] * eoprot_board_numberofeachentity[brd][epi][i]);
    }

    return(num);
}


static uint16_t s_eoprot_brdentityindex2ramoffset(eOprotBRD_t brd, uint8_t epi, eOprotEntity_t entity, eOprotIndex_t index)
{
    uint16_t offset = 0;
        
    if(entity >= eoprot_ep_entities_numberof[epi])
    {
        return(EOK_uint16dummy);
    }
    
    if(NULL == eoprot_board_numberofeachentity[brd][epi])
    {
        return(EOK_uint16dummy);
    }
    
    uint8_t i;
    for(i=0; i<entity; i++)
    {   // we sum the size of all the entities before the current one
        offset += (eoprot_board_numberofeachentity[brd][epi][i] * eoprot_ep_entities_sizeof[epi][i]);
    }
    // then we add the offset of the current entity
    offset += (index*eoprot_ep_entities_sizeof[epi][entity]);

    return(offset);
}    


static uint16_t s_eoprot_brdid2ramoffset(eOprotBRD_t brd, uint8_t epi, eOprotID32_t id)
{
    eOprotEntity_t entity = eoprot_ID2entity(id);
    eOprotIndex_t index = eoprot_ID2index(id);
    eOprotTag_t tag = eoprot_ID2tag(id);
    
    // we compute the offset of the entity
    uint16_t offset = s_eoprot_brdentityindex2ramoffset(brd, epi, entity, index);
       
    if(EOK_uint16dummy == offset)
    {
        return(EOK_uint16dummy);
    }
    
    // then we add the offset of the tag
    //offset += (eoprot_ep_romif[epi]->get_offset(entity, tag)); 
    offset += s_eoprot_rom_get_offset(epi, entity, tag); 

    return(offset);
} 


static eObool_t s_eoprot_entity_tag_is_valid(uint8_t epi, eOprotEntity_t entity, eOprotTag_t tag)
{
    return((tag < eoprot_ep_tags_numberof[epi][entity])?(eobool_true):(eobool_false));
}



// ciao

// returns the offset of the variable with a given tag from the start of the entity
static uint16_t s_eoprot_rom_entity_offset_of_tag(uint8_t epi, uint8_t ent, eOprotTag_t tag)
{
    const uint32_t *entityrom = eoprot_ep_entities_defval[epi][ent];
    uint32_t tmp = ((uint32_t) eoprot_ep_folded_descriptors[epi][tag]->resetval) - ((uint32_t) entityrom);
    return((uint16_t)tmp); 
}

static uint16_t s_eoprot_rom_get_offset(uint8_t epi, eOprotEntity_t entity, eOprotTag_t tag)
{
    if(entity >= eoprot_ep_entities_numberof[epi])
    {
        return(EOK_uint16dummy);
    }
    
    return(s_eoprot_rom_entity_offset_of_tag(epi, entity, tag));
}


static void* s_eoprot_rom_get_nvrom(uint8_t epi, eOprotID32_t id)
{
    uint16_t indexoffoldeddescriptors = s_eoprot_rom_epid2index_of_folded_descriptors(epi, id);
    
    if(EOK_uint16dummy == indexoffoldeddescriptors)
    {
        return(NULL);
    }
    
    return((void*)eoprot_ep_folded_descriptors[epi][indexoffoldeddescriptors]);   
}


static uint16_t s_eoprot_rom_get_sizeofvar(uint8_t epi, eOprotID32_t id)
{     
    EOnv_rom_t* rom = s_eoprot_rom_get_nvrom(epi, id);  
    if(NULL == rom)
    {
        return(0);
    }    
    return(rom->capacity); 
}


static uint16_t s_eoprot_rom_get_prognum(eOprotID32_t id)
{   // we assume that the variables are inserted in a progressive way without holes. and even if there are a few holes never mind.
    return(eoprot_ID2tag(id));
}



static uint16_t s_eoprot_rom_epid2index_of_folded_descriptors(uint8_t epi, eOprotID32_t id)
{      
    uint16_t ret = 0;
    
    // dont check validity of the tag. we could check inside the case xxxx: by verifying if ret is higher than 
    // the max number of tags for that entity.
       
    eOprotEntity_t entity = eoprot_ID2entity(id);
    
    if(entity >= eoprot_ep_entities_numberof[epi])
    {
        return(EOK_uint16dummy);
    }
    
    uint8_t i;
    for(i=0; i<entity; i++)
    {   // we add all the tags in the entities below
        ret += eoprot_ep_tags_numberof[epi][i];
    }
    // then we add only the tags of the entity equal to the current one
    ret += eoprot_ID2tag(id);
    
    return(ret);   
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



