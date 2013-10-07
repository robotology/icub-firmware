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

/* @file       EoProtocolMC.c
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

#include "EoProtocol.h"
#include "EoProtocolMC_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolMC.h"


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

// - guard on max number of entities
EO_VERIFYproposition(eoprot_mc_dew4, eoprot_entities_mc_numberof <= eoprot_entities_maxnumberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_mc_tagsnum_jo, eoprot_tags_mc_joint_numberof == eoprot_rwms_mc_joint_numberof);
EO_VERIFYproposition(eoprot_mc_tagsmax_jo, eoprot_tags_mc_joint_numberof <= eoprot_tags_maxnumberof);

EO_VERIFYproposition(eoprot_mc_tagsnum_mo, eoprot_tags_mc_motor_numberof == eoprot_rwms_mc_motor_numberof);
EO_VERIFYproposition(eoprot_mc_tagsmax_mo, eoprot_tags_mc_motor_numberof <= eoprot_tags_maxnumberof);

EO_VERIFYproposition(eoprot_mc_tagsnum_co, eoprot_tags_mc_controller_numberof == eoprot_rwms_mc_controller_numberof);
EO_VERIFYproposition(eoprot_mc_tagsmax_co, eoprot_tags_mc_controller_numberof <= eoprot_tags_maxnumberof);
 

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// functions used to configure the eOnvset_EPcfg_t

static uint16_t eoprot_mc_numberofvariables_get(eOprotBRD_t brd);
static eObool_t eoprot_mc_id_isvalid(eOprotBRD_t brd, eOnvID32_t id);
static eOprotID32_t eoprot_mc_prognum2id(eOprotBRD_t brd, eOprotProgNumber_t prog);
static eOprotProgNumber_t eoprot_mc_id2prognum(eOprotBRD_t brd, eOprotID32_t id);
static void* eoprot_mc_eonvrom_get(eOprotBRD_t brd, eOprotID32_t id);


static uint16_t s_eoprot_mc_brdentityindex2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity, eOprotIndex_t index);
static uint16_t s_eoprot_mc_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id);


static eObool_t s_eoprot_mc_entity_tag_is_valid(eOprotEntity_t entity, eOprotTag_t tag);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const uint8_t s_eoprot_mc_tags_numberof[] = 
{
    eoprot_tags_mc_joint_numberof, 
    eoprot_tags_mc_motor_numberof, 
    eoprot_tags_mc_controller_numberof
};  EO_VERIFYsizeof(s_eoprot_mc_tags_numberof, eoprot_entities_mc_numberof*sizeof(uint8_t)); 


static const uint16_t s_eoprot_mc_entities_sizeof[] = 
{
    sizeof(eOmc_joint_t), 
    sizeof(eOmc_motor_t), 
    sizeof(eOmc_controller_t)
};  EO_VERIFYsizeof(s_eoprot_mc_entities_sizeof, eoprot_entities_mc_numberof*sizeof(uint16_t)); 


#if 0   // dynamic mode
static const uint8_t** s_eoprot_mc_board_numberofeachentity = NULL; // s_eoprot_mc_board_numberofeachentity[brd][entype] tells how many entities of kind entype are in brd
static uint16_t s_eoprot_mc_board_numberofthem = 0;
#endif

static const uint16_t s_eoprot_mc_board_numberofthem = eoprot_boards_maxnumberof;
static const uint8_t* s_eoprot_mc_board_numberofeachentity[eoprot_boards_maxnumberof] = { NULL };
static void* s_eoprot_mc_board_ramofeachendpoint[eoprot_boards_maxnumberof] = { NULL };


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const eOprot_nvset_interface_t eoprot_eonvset_interface_mc =
{
    EO_INIT(.loadram)               eoprot_mc_config_endpoint_ram,
    EO_INIT(.getvarsnumberof)       eoprot_mc_numberofvariables_get,       
    EO_INIT(.isidsupported)         eoprot_mc_id_isvalid,     
    EO_INIT(.getid)                 eoprot_mc_prognum2id,            
    EO_INIT(.getprognumber)         eoprot_mc_id2prognum,     
    EO_INIT(.getrom)                eoprot_mc_eonvrom_get,             
    EO_INIT(.getram)                eoprot_mc_variable_ramof_get                      
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


#if 0   // dynamic mode
extern eOresult_t eoprot_mc_number_of_boards_Load(uint16_t numofboards)
{
    if((0 != s_eoprot_mc_board_numberofthem) || (NULL != s_eoprot_mc_board_numberofeachentity))
    {
        return(eores_NOK_generic);
    }
    
    s_eoprot_mc_board_numberofthem = numofboards;
    s_eoprot_mc_board_numberofeachentity = (const uint8_t**) calloc(numofboards, sizeof(uint8_t*));
    
    return(eores_OK);   
}
#endif



extern eOresult_t eoprot_mc_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity)
{
    if(NULL == s_eoprot_mc_board_numberofeachentity)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_mc_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_mc_board_numberofeachentity[brd] = numberofeachentity;
    
    return(eores_OK);       
}


extern uint16_t eoprot_mc_endpoint_sizeof_get(eOprotBRD_t brd)
{
    uint16_t size = 0;
    
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    uint8_t i;
    for(i=0; i<eoprot_entities_mc_numberof; i++)
    {
        size += s_eoprot_mc_entities_sizeof[i] * s_eoprot_mc_board_numberofeachentity[brd][i];
    }
    
    return(size);
}

extern eOresult_t eoprot_mc_config_endpoint_ram(eOprotBRD_t brd, void *ram, uint16_t sizeofram)
{
    if(NULL == s_eoprot_mc_board_ramofeachendpoint)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_mc_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_mc_board_ramofeachendpoint[brd] = ram;
    
    return(eores_OK);       
}

extern void* eoprot_mc_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id)
{
    if((NULL == s_eoprot_mc_board_ramofeachendpoint) || (NULL == s_eoprot_mc_board_ramofeachendpoint[brd]))
    {
        return(NULL);
    }
    
    uint8_t* startofdata = (void*)s_eoprot_mc_board_ramofeachendpoint[brd];
    uint16_t offset = s_eoprot_mc_brdid2ramoffset(brd, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
}

extern uint16_t eoprot_mc_variable_sizeof_get(eOprotID32_t id)
{
    return(eoprot_mc_rom_get_sizeofvar(id));
}

extern void* eoprot_mc_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index)
{
    if((NULL == s_eoprot_mc_board_ramofeachendpoint) || (NULL == s_eoprot_mc_board_ramofeachendpoint[brd]))
    {
        return(NULL);
    }
    
    uint8_t* startofdata = (void*)s_eoprot_mc_board_ramofeachendpoint[brd];
    uint16_t offset = s_eoprot_mc_brdentityindex2ramoffset(brd, ent, index);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
} 


extern uint16_t eoprot_mc_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent)
{
    uint16_t sizeofentity = EOK_uint16dummy;
    
    if(ent < eoprot_entities_mc_numberof)
    {
        sizeofentity = s_eoprot_mc_entities_sizeof[ent];
    }
    
    return(sizeofentity);    
}


extern uint8_t eoprot_mc_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent)
{
    uint8_t numberof = 0;
    
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    if(ent < eoprot_entities_mc_numberof)
    {
        numberof = s_eoprot_mc_board_numberofeachentity[brd][ent];
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


static uint16_t eoprot_mc_numberofvariables_get(eOprotBRD_t brd)
{
    uint16_t num = 0;
    
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    uint8_t i;
    for(i=0; i<eoprot_entities_mc_numberof; i++)
    {   
        // simply the sum for each entity of the number of tags multiplied the number of each entity. 
        // it also works if an entity is not present in the board
        num += (s_eoprot_mc_tags_numberof[i] * s_eoprot_mc_board_numberofeachentity[brd][i]);
    }

    return(num);
}


static eObool_t eoprot_mc_id_isvalid(eOprotBRD_t brd, eOnvID32_t id)
{
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(eobool_false);
    }    
    
    // just verifies that the entity, index, and tag have numbers which are consistent with their maximum number
    
    eOprotEntity_t ent = eoprot_ID2entity(id);
    eOprotIndex_t  ind = eoprot_ID2index(id);
    eOprotTag_t    tag = eoprot_ID2tag(id);
    
    if(ent >= eoprot_entities_mc_numberof)
    {
        return(eobool_false);
    }
    
    if(ind >= s_eoprot_mc_board_numberofeachentity[brd][ent])
    {
        return(eobool_false);
    }   
    
    return(s_eoprot_mc_entity_tag_is_valid(ent, tag));

}

static eOprotID32_t eoprot_mc_prognum2id(eOprotBRD_t brd, eOprotProgNumber_t prog)
{
    eOprotTag_t tag = 0xff;
    eOprotIndex_t index = 0xff;
    eOprotEntity_t entity = 0xff;
    
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }   
    
    uint8_t i;
    for(i=0; i<eoprot_entities_mc_numberof; i++)
    {
        // starting from the first entity (if it present in the board) we progressively check if the signedprog is in its relevant range.
        uint8_t tags_number_ith = s_eoprot_mc_tags_numberof[i];
        eOprotProgNumber_t progs_ith = tags_number_ith * s_eoprot_mc_board_numberofeachentity[brd][i]; // num of progs in all the entities i-th
        if((0 != progs_ith) && (prog < (progs_ith)))
        {   // entity is the i-th 
            entity  = i;
            index   = prog / tags_number_ith;       // tags_number_ith cannot be zero if progs_ith is non-zero
            tag     = prog % tags_number_ith;  

            return(eoprot_ID_get(eoprot_endpoint_analogsensors, entity, index, tag));
        }
        prog -= progs_ith; // entity may be the next one. 
    }

    return(EOK_uint32dummy);   
}


static eOprotProgNumber_t eoprot_mc_id2prognum(eOprotBRD_t brd, eOprotID32_t id)
{
    eOprotProgNumber_t prog = 0;
    
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }
    
    eOprotEntity_t entity = eoprot_ID2entity(id);
    eOprotIndex_t  index  = eoprot_ID2index(id);
    
    if(entity >= eoprot_entities_mc_numberof)
    {
        return(EOK_uint32dummy);
    }
    
    uint8_t i;
    for(i=0; i<=entity; i++)
    {   // we add all the tags in the entities below
        prog += (s_eoprot_mc_tags_numberof[i] * s_eoprot_mc_board_numberofeachentity[brd][i]);
    }
    // then we add only the tags of the entities equal to the current one + the progessive number of the tag
    prog += (index*s_eoprot_mc_tags_numberof[entity] + eoprot_mc_rom_get_prognum(id));

    return(prog);
}    
    


static void* eoprot_mc_eonvrom_get(eOprotBRD_t brd, eOprotID32_t id)
{
    brd =  brd;
    return(eoprot_mc_rom_get_nvrom(id));
}

static uint16_t s_eoprot_mc_brdentityindex2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity, eOprotIndex_t index)
{
    uint16_t offset = 0;
        
    if(entity >= eoprot_entities_mc_numberof)
    {
        return(EOK_uint16dummy);
    }
    
    uint8_t i;
    for(i=0; i<=entity; i++)
    {   // we sum the size of all the entities before the current one
        offset += (s_eoprot_mc_board_numberofeachentity[brd][i] * s_eoprot_mc_entities_sizeof[i]);
    }
    // then we add the offset of the current entity
    offset += (index*s_eoprot_mc_entities_sizeof[entity]);

    return(offset);
}    



static uint16_t s_eoprot_mc_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id)
{
    eOprotEntity_t entity = eoprot_ID2entity(id);
    eOprotIndex_t index = eoprot_ID2index(id);
    eOprotTag_t tag = eoprot_ID2tag(id);
    
    // we compute the offset of the entity
    uint16_t offset = s_eoprot_mc_brdentityindex2ramoffset(brd, entity, index);
       
    if(EOK_uint16dummy == offset)
    {
        return(EOK_uint16dummy);
    }
    
    // then we add the offset of the tag
    offset += (eoprot_mc_rom_get_offset(entity, tag)); 

    return(offset);
} 



static eObool_t s_eoprot_mc_entity_tag_is_valid(eOprotEntity_t entity, eOprotTag_t tag)
{
    return((tag < s_eoprot_mc_tags_numberof[entity])?(eobool_true):(eobool_false));
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



