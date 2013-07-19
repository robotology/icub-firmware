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

/* @file       EoProtocolAS.c
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

#include "EoProtocolAS_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolAS.h"


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
EO_VERIFYproposition(eoprot_as_ded34, eoprot_entities_as_numberof <= eoprot_entities_maxnumberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_as_tagsnum_st, eoprot_tags_as_strain_numberof == eoprot_rwms_as_strain_numberof);
EO_VERIFYproposition(eoprot_as_tagsmax_st, eoprot_tags_as_strain_numberof <= eoprot_tags_maxnumberof);

EO_VERIFYproposition(eoprot_as_tagsnum_ma, eoprot_tags_as_mais_numberof == eoprot_rwms_as_mais_numberof);
EO_VERIFYproposition(eoprot_as_tagsmax_ma, eoprot_tags_as_mais_numberof <= eoprot_tags_maxnumberof);


 

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// functions used to configure the eOnvset_EPcfg_t
static uint16_t eoprot_as_numberofvariables_get(eOprotBRD_t brd);
static eObool_t eoprot_as_id_isvalid(eOprotBRD_t brd, eOnvID32_t id);
static eOprotID32_t eoprot_as_prognum2id(eOprotBRD_t brd, eOprotProgNumber_t prog);
static eOprotProgNumber_t eoprot_as_id2prognum(eOprotBRD_t brd, eOprotID32_t id);
static void* eoprot_as_eonvrom_get(eOprotBRD_t brd, eOprotID32_t id);
//static void* eoprot_as_ramofvariable_extract(eOprotBRD_t brd, eOprotID32_t id, void* epram);



static uint16_t s_eoprot_as_brdentityindex2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity, eOprotIndex_t index);
static uint16_t s_eoprot_as_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id);

static eObool_t s_eoprot_as_tag_strain_is_valid(eOprotTag_t tag);
static eObool_t s_eoprot_as_tag_mais_is_valid(eOprotTag_t tag);

static uint16_t s_eoprot_as_strains_numberof_Get(eOprotBRD_t brd);

//static uint16_t s_eoprot_as_maises_numberof_Get(eOprotBRD_t brd);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if 0   // dynamic mode
static const uint8_t** s_eoprot_as_board_numberofeachentity = NULL; // s_eoprot_as_board_numberofeachentity[brd][entype] tells how many entities of kind entype are in brd
static uint16_t s_eoprot_as_board_numberofthem = 0;
#endif

static const uint16_t s_eoprot_as_board_numberofthem = eoprot_boards_maxnumberof;
static const uint8_t* s_eoprot_as_board_numberofeachentity[eoprot_boards_maxnumberof] = { NULL };
static void* s_eoprot_as_board_ramofeachendpoint[eoprot_boards_maxnumberof] = { NULL };


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const eOprot_nvset_interface_t eoprot_eonvset_interface_as =
{
    EO_INIT(.loadram)               eoprot_as_config_endpoint_ram,
    EO_INIT(.getvarsnumberof)       eoprot_as_numberofvariables_get,       
    EO_INIT(.isidsupported)         eoprot_as_id_isvalid,     
    EO_INIT(.getid)                 eoprot_as_prognum2id,            
    EO_INIT(.getprognumber)         eoprot_as_id2prognum,     
    EO_INIT(.getrom)                eoprot_as_eonvrom_get,             
    EO_INIT(.getram)                eoprot_as_variable_ramof_get                      
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


#if 0   // dynamic mode
extern eOresult_t eoprot_as_number_of_boards_Load(uint16_t numofboards)
{
    if((0 != s_eoprot_as_board_numberofthem) || (NULL != s_eoprot_as_board_numberofeachentity))
    {
        return(eores_NOK_generic);
    }
    
    s_eoprot_as_board_numberofthem = numofboards;
    s_eoprot_as_board_numberofeachentity = (const uint8_t**) calloc(numofboards, sizeof(uint8_t*));
    
    return(eores_OK);   
}
#endif



extern eOresult_t eoprot_as_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity)
{
    if(NULL == s_eoprot_as_board_numberofeachentity)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_as_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_as_board_numberofeachentity[brd] = numberofeachentity;
    
    return(eores_OK);       
}


extern uint16_t eoprot_as_endpoint_sizeof_get(eOprotBRD_t brd)
{
    uint16_t size = 0;
    
    if((NULL == s_eoprot_as_board_numberofeachentity) || (NULL == s_eoprot_as_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    //#warning --> it is valid only if organisation of memory is st[n], ma[m]
    size = sizeof(eOas_strain_t) *      s_eoprot_as_board_numberofeachentity[brd][eoas_entity_strain] +
           sizeof(eOas_mais_t) *      s_eoprot_as_board_numberofeachentity[brd][eoas_entity_mais];
    
    return(size);
}

extern eOresult_t eoprot_as_config_endpoint_ram(eOprotBRD_t brd, void *ram, uint16_t sizeofram)
{
    if(NULL == s_eoprot_as_board_ramofeachendpoint)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_as_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_as_board_ramofeachendpoint[brd] = ram;
    
    return(eores_OK);       
}

extern void* eoprot_as_variable_ramof_get(eOprotBRD_t brd, eOprotID32_t id)
{
    if((NULL == s_eoprot_as_board_ramofeachendpoint) || (NULL == s_eoprot_as_board_ramofeachendpoint[brd]))
    {
        return(NULL);
    }
    
    uint8_t* startofdata = (void*)s_eoprot_as_board_ramofeachendpoint[brd];
    uint16_t offset = s_eoprot_as_brdid2ramoffset(brd, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
}

extern uint16_t eoprot_as_variable_sizeof_get(eOprotID32_t id)
{
    return(eoprot_as_rom_get_sizeofvar(id));
}

extern void* eoprot_as_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index)
{
    if((NULL == s_eoprot_as_board_ramofeachendpoint) || (NULL == s_eoprot_as_board_ramofeachendpoint[brd]))
    {
        return(NULL);
    }
    
    uint8_t* startofdata = (void*)s_eoprot_as_board_ramofeachendpoint[brd];
    uint16_t offset = s_eoprot_as_brdentityindex2ramoffset(brd, ent, index);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
} 


extern uint16_t eoprot_as_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent)
{
    uint16_t sizeofentity = EOK_uint16dummy;
    switch(ent)
    {
        case eoas_entity_strain:
        {  
            sizeofentity = sizeof(eOas_strain_t); 
        } break;
        
        case eoas_entity_mais:
        {  
            sizeofentity = sizeof(eOas_mais_t);  
        } break;      
        
        default:
        {   // error
            sizeofentity = EOK_uint16dummy;
        } break;    
    }
    
    return(sizeofentity);    
}


extern uint8_t eoprot_as_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent)
{
    uint8_t numberof = 0;
    
    if((NULL == s_eoprot_as_board_numberofeachentity) || (NULL == s_eoprot_as_board_numberofeachentity[brd]))
    {
        return(0);
    }
 
    switch(ent)
    {    
        case eoas_entity_strain: 
        case eoas_entity_mais:
        {
            numberof = s_eoprot_as_board_numberofeachentity[brd][ent];
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

// ---


static uint16_t eoprot_as_numberofvariables_get(eOprotBRD_t brd)
{
    uint16_t num = 0;
    
    if((NULL == s_eoprot_as_board_numberofeachentity) || (NULL == s_eoprot_as_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    num = eoprot_tags_as_strain_numberof * s_eoprot_as_board_numberofeachentity[brd][eoas_entity_strain] +
          eoprot_tags_as_mais_numberof * s_eoprot_as_board_numberofeachentity[brd][eoas_entity_mais];
          
    return(num);
}


static eObool_t eoprot_as_id_isvalid(eOprotBRD_t brd, eOnvID32_t id)
{
    eObool_t ret = eobool_false;    
    
    if((NULL == s_eoprot_as_board_numberofeachentity) || (NULL == s_eoprot_as_board_numberofeachentity[brd]))
    {
        return(eobool_false);
    }    
    
    eOprotEntity_t ent = eoprot_ID2entity(id);
    eOprotIndex_t  ind = eoprot_ID2index(id);
    eOprotTag_t    tag = eoprot_ID2tag(id);
    
    switch(ent)
    {
        case eoas_entity_strain:
        {   
            if(ind < s_eoprot_as_board_numberofeachentity[brd][eoas_entity_strain])
            {
                ret = s_eoprot_as_tag_strain_is_valid(tag);
            }            
        } break;
        
        case eoas_entity_mais:
        {
            if(ind < s_eoprot_as_board_numberofeachentity[brd][eoas_entity_mais]) 
            {
                ret = s_eoprot_as_tag_mais_is_valid(tag);
            }            
        } break;        
        
        default:
        {           
        } break;        
    }
    
    return(ret);     
}


static eOprotID32_t eoprot_as_prognum2id(eOprotBRD_t brd, eOprotProgNumber_t prog)
{
    eOprotTag_t tag = 0xff;
    eOprotIndex_t index = 0xff;
    eOprotEntity_t entity = 0xff;
    
    if((NULL == s_eoprot_as_board_numberofeachentity) || (NULL == s_eoprot_as_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }
    
    eOprotProgNumber_t progsinstrains = eoprot_tags_as_strain_numberof * s_eoprot_as_board_numberofeachentity[brd][eoas_entity_strain];
    eOprotProgNumber_t progsinmaises = eoprot_tags_as_mais_numberof * s_eoprot_as_board_numberofeachentity[brd][eoas_entity_mais];
    
    if((0 != progsinstrains) && (prog < (progsinstrains)))
    {   // entity is eoas_entity_strain 
        entity  = eoas_entity_strain;
        index   = prog / eoprot_tags_as_strain_numberof;       // eoprot_tags_as_strain_numberof cannot be zero if progsinstrains is non-zero
        tag     = prog % eoprot_tags_as_strain_numberof;        
    }
    else if((0 != progsinmaises) && (prog < (progsinstrains + progsinmaises)))
    {   // entity is eoas_entity_mais 
        prog -= progsinstrains;
        entity  = eoas_entity_mais;
        index   = prog / eoprot_tags_as_mais_numberof;       // eoprot_tags_as_mais_numberof cannot be zero if progsinmaises is non-zero
        tag     = prog % eoprot_tags_as_mais_numberof;                      
    }
    else
    {
        return(EOK_uint32dummy);
    }
    
    return(eoprot_ID_get(eoprot_endpoint_analogsensors, entity, index, tag));
}

static eOprotProgNumber_t eoprot_as_id2prognum(eOprotBRD_t brd, eOprotID32_t id)
{
    eOprotProgNumber_t prog = EOK_uint32dummy;
    
    if((NULL == s_eoprot_as_board_numberofeachentity) || (NULL == s_eoprot_as_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }
    
    eOprotEntity_t entity = eoprot_ID2entity(id);
    eOprotIndex_t index = eoprot_ID2index(id);
    switch(entity)
    {
        case eoas_entity_strain: 
        {
            prog = index*eoprot_tags_as_strain_numberof + eoprot_as_rom_get_prognum(id);
        } break;
        
        case eoas_entity_mais: 
        {
            prog = s_eoprot_as_board_numberofeachentity[brd][eoas_entity_strain]*eoprot_tags_as_strain_numberof +
                   index*eoprot_tags_as_mais_numberof + eoprot_as_rom_get_prognum(id);
        } break; 
                
        default:
        {   
            prog = EOK_uint32dummy;
        } break;    
    }
    
    return(prog);
}


static void* eoprot_as_eonvrom_get(eOprotBRD_t brd, eOprotID32_t id)
{
    brd =  brd;
    return(eoprot_as_rom_get_nvrom(id));
}


// static void* eoprot_as_ramofvariable_extract(eOprotBRD_t brd, eOprotID32_t id, void* epram)
// {
//     uint8_t* startofdata = epram;
//     uint16_t offset = s_eoprot_as_brdid2ramoffset(brd, id);
//     
//     if(EOK_uint16dummy == offset)
//     {
//         return(NULL);
//     }   

//     return(&startofdata[offset]);
// }



static uint16_t s_eoprot_as_brdentityindex2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity, eOprotIndex_t index)
{
    uint16_t offset = 0;
    
    switch(entity)
    {
        case eoas_entity_strain:
        {   // the strains, if present, are all displaced at the beginning of the data in every as endpoint
            offset = 0;
            offset += sizeof(eOas_strain_t)*index; 
        } break;
        
        case eoas_entity_mais:
        {   // the maises, if present, are placed after all the strains (if present). 
            uint16_t numberofstrains = s_eoprot_as_strains_numberof_Get(brd);
            offset = numberofstrains*sizeof(eOas_strain_t);  
            offset += sizeof(eOas_mais_t)*index;
        } break;      
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;    
    }
    
    return(offset);  
}



static uint16_t s_eoprot_as_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id)
{
    uint16_t offset = 0;
    eOprotTag_t tag = eoprot_ID2tag(id);
    
    if(EOK_uint08dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    //#warning --> it is valid only if organisation of memory is jo[n], mo[m], c[l]
    
    eOprotEntity_t entity = eoprot_ID2entity(id);
    uint16_t index = eoprot_ID2index(id);
    
    switch(entity)
    {
        case eoas_entity_strain:
        {   // the strain are all displaced at the beginning of the data in every mc endpoint
            offset = index*sizeof(eOas_strain_t) + eoprot_as_rom_strain_get_offset(tag); 
        } break;
        
        case eoas_entity_mais:
        {   // the maises are placed after all the strains (if any strains ...). 
            uint16_t numberofstrains = s_eoprot_as_strains_numberof_Get(brd);
            offset = numberofstrains*sizeof(eOas_strain_t) + index*sizeof(eOas_mais_t) + eoprot_as_rom_mais_get_offset(tag);  
        } break;      
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;    
    }
    
    return(offset);  
}

static eObool_t s_eoprot_as_tag_strain_is_valid(eOprotTag_t tag)
{   // in case of holes in tags ... change the code
    return((tag < eoprot_tags_as_strain_numberof)?(eobool_true):(eobool_false));
}

static eObool_t s_eoprot_as_tag_mais_is_valid(eOprotTag_t tag)
{   // in case of holes in tags ... change the code  
    return((tag < eoprot_tags_as_mais_numberof)?(eobool_true):(eobool_false));
}

static uint16_t s_eoprot_as_strains_numberof_Get(eOprotBRD_t brd)
{
//     if((NULL == s_eoprot_as_board_numberofeachentity) || (NULL == s_eoprot_as_board_numberofeachentity[brd]))
//     {
//         return(0);
//     }
    
    return(s_eoprot_as_board_numberofeachentity[brd][eoas_entity_strain]);
}

// static uint16_t s_eoprot_as_maises_numberof_Get(eOprotBRD_t brd)
// {
// //     if((NULL == s_eoprot_as_board_numberofeachentity) || (NULL == s_eoprot_as_board_numberofeachentity[brd]))
// //     {
// //         return(0);
// //     }
//     
//     return(s_eoprot_as_board_numberofeachentity[brd][eoas_entity_mais]);
// }


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



