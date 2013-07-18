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

/* @file       EoProtocolSK.c
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

#include "EoProtocolSK_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolSK.h"


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
EO_VERIFYproposition(eoprot_sk_ded34, eoprot_entities_sk_numberof <= eoprot_entities_maxnumberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_sk_tagsnum_sk, eoprot_tags_sk_skin_numberof == eoprot_rwms_sk_skin_numberof);
EO_VERIFYproposition(eoprot_sk_tagsmax_sk, eoprot_tags_sk_skin_numberof <= eoprot_tags_maxnumberof);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_sk_brdentityindex2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity, eOprotIndex_t index);



static uint16_t s_eoprot_sk_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id);

static eObool_t s_eoprot_sk_tag_skin_is_valid(eOprotTag_t tag);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if 0   // dynamic mode
static const uint8_t** s_eoprot_sk_board_numberofeachentity = NULL; // s_eoprot_sk_board_numberofeachentity[brd][entype] tells how many entities of kind entype are in brd
static uint16_t s_eoprot_sk_board_numberofthem = 0;
#endif

static const uint16_t s_eoprot_sk_board_numberofthem = eoprot_boards_maxnumberof;
static const uint8_t* s_eoprot_sk_board_numberofeachentity[eoprot_boards_maxnumberof] = { NULL };
static void* s_eoprot_sk_board_ramofeachendpoint[eoprot_boards_maxnumberof] = { NULL };

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#if 0   // dynamic mode
extern eOresult_t eoprot_sk_number_of_boards_Load(uint16_t numofboards)
{
    if((0 != s_eoprot_sk_board_numberofthem) || (NULL != s_eoprot_sk_board_numberofeachentity))
    {
        return(eores_NOK_generic);
    }
    
    s_eoprot_sk_board_numberofthem = numofboards;
    s_eoprot_sk_board_numberofeachentity = (const uint8_t**) calloc(numofboards, sizeof(uint8_t*));
    
    return(eores_OK);   
}
#endif

extern eOresult_t eoprot_sk_config_endpoint_entities(eOprotBRD_t brd, const uint8_t* numberofeachentity)
{
    if(NULL == s_eoprot_sk_board_numberofeachentity)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_sk_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_sk_board_numberofeachentity[brd] = numberofeachentity;
    
    return(eores_OK);       
}

extern uint16_t eoprot_sk_endpoint_sizeof_get(eOprotBRD_t brd)
{
    uint16_t size = 0;
    
    if((NULL == s_eoprot_sk_board_numberofeachentity) || (NULL == s_eoprot_sk_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    size = sizeof(eOsk_skin_t) * s_eoprot_sk_board_numberofeachentity[brd][eosk_entity_skin];
    
    return(size);
}


extern eOresult_t eoprot_sk_config_endpoint_ram(eOprotBRD_t brd, void *ram)
{
    if(NULL == s_eoprot_sk_board_ramofeachendpoint)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_sk_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_sk_board_ramofeachendpoint[brd] = ram;
    
    return(eores_OK);       
}


extern uint16_t eoprot_sk_variable_sizeof_get(eOprotID32_t id)
{
    return(eoprot_sk_rom_get_sizeofvar(id));
}

extern void* eoprot_sk_entity_ramof_get(eOprotBRD_t brd, eOprotEntity_t ent, eOprotIndex_t index)
{
    if((NULL == s_eoprot_sk_board_ramofeachendpoint) || (NULL == s_eoprot_sk_board_ramofeachendpoint[brd]))
    {
        return(NULL);
    }
    
    uint8_t* startofdata = (void*)s_eoprot_sk_board_ramofeachendpoint[brd];
    uint16_t offset = s_eoprot_sk_brdentityindex2ramoffset(brd, ent, index);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
} 

extern uint16_t eoprot_sk_entity_sizeof_get(eOprotBRD_t brd, eOprotEntity_t ent)
{
    uint16_t sizeofentity = 0;
    switch(ent)
    {
        case eosk_entity_skin:
        {  
            sizeofentity = sizeof(eOsk_skin_t); 
        } break;
         
        default:
        {   // error
            sizeofentity = 0;
        } break;    
    }
    
    return(sizeofentity);    
}

extern uint8_t eoprot_sk_entity_numberof_get(eOprotBRD_t brd, eOprotEntity_t ent)
{
    uint8_t numberof = 0;
    
    if((NULL == s_eoprot_sk_board_numberofeachentity) || (NULL == s_eoprot_sk_board_numberofeachentity[brd]))
    {
        return(0);
    }
 
    switch(ent)
    {    
        case eosk_entity_skin: 
        {
            numberof = s_eoprot_sk_board_numberofeachentity[brd][ent];
        } break;
                     
        default:
        {   
            numberof = 0;
        } break; 
    }        
    
    return(numberof);
}


// ---

extern uint16_t eoprot_sk_numberofvariables_get(eOprotBRD_t brd)
{
    uint16_t num = 0;
    
    if((NULL == s_eoprot_sk_board_numberofeachentity) || (NULL == s_eoprot_sk_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    num = eoprot_tags_sk_skin_numberof * s_eoprot_sk_board_numberofeachentity[brd][eosk_entity_skin];
          
    return(num);
}

extern eObool_t eoprot_sk_id_isvalid(eOprotBRD_t brd, eOnvID32_t id)
{
    eObool_t ret = eobool_false;    
    
    if((NULL == s_eoprot_sk_board_numberofeachentity) || (NULL == s_eoprot_sk_board_numberofeachentity[brd]))
    {
        return(eobool_false);
    }    
    
    eOprotEntity_t ent = eoprot_ID2entity(id);
    eOprotIndex_t  ind = eoprot_ID2index(id);
    eOprotTag_t    tag = eoprot_ID2tag(id);
    
    switch(ent)
    {
        case eosk_entity_skin:
        {   
            if(ind < s_eoprot_sk_board_numberofeachentity[brd][eosk_entity_skin])
            {
                ret = s_eoprot_sk_tag_skin_is_valid(tag);
            }            
        } break;
        
        default:
        {           
        } break;        
    }
    
    return(ret);     
}


extern eOprotID32_t eoprot_sk_prognum2id(eOprotBRD_t brd, eOprotProgNumber_t prog)
{
    eOprotTag_t tag = 0xff;
    eOprotIndex_t index = 0xff;
    eOprotEntity_t entity = 0xff;
    
    if((NULL == s_eoprot_sk_board_numberofeachentity) || (NULL == s_eoprot_sk_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }
    
    eOprotProgNumber_t progsinskins = eoprot_tags_sk_skin_numberof * s_eoprot_sk_board_numberofeachentity[brd][eosk_entity_skin];
    
    if((0 != progsinskins) && (prog < (progsinskins)))
    {   // entity is eosk_entity_skin 
        entity  = eosk_entity_skin;
        index   = prog / eoprot_tags_sk_skin_numberof;       // eoprot_tags_sk_skin_numberof cannot be zero if progsinskins is non-zero
        tag     = prog % eoprot_tags_sk_skin_numberof;        
    }
    else
    {
        return(EOK_uint32dummy);
    }
    
    return(eoprot_ID_get(eoprot_endpoint_skin, entity, index, tag));   
}

extern eOprotProgNumber_t eoprot_sk_id2prognum(eOprotBRD_t brd, eOprotID32_t id)
{
    eOprotProgNumber_t prog = EOK_uint32dummy;
    
    if((NULL == s_eoprot_sk_board_numberofeachentity) || (NULL == s_eoprot_sk_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }
    
    eOprotEntity_t entity = eoprot_ID2entity(id);
    eOprotIndex_t index = eoprot_ID2index(id);
    switch(entity)
    {
        case eosk_entity_skin: 
        {
            prog = index*eoprot_tags_sk_skin_numberof + eoprot_sk_rom_get_prognum(id);
        } break;
        
                
        default:
        {   
            prog = EOK_uint32dummy;
        } break;    
    }
    
    return(prog);
}

extern void* eoprot_sk_eonvrom_get(/*eOprotBRD_t brd,*/ eOprotID32_t id)
{
    return(eoprot_sk_rom_get_nvrom(id));
}


extern void* eoprot_sk_ramofvariable_get(eOprotBRD_t brd, eOprotID32_t id)
{
    if((NULL == s_eoprot_sk_board_ramofeachendpoint) || (NULL == s_eoprot_sk_board_ramofeachendpoint[brd]))
    {
        return(NULL);
    }
    
    uint8_t* startofdata = (void*)s_eoprot_sk_board_ramofeachendpoint[brd];
    uint16_t offset = s_eoprot_sk_brdid2ramoffset(brd, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   


    return(&startofdata[offset]);
} 


extern void* eoprot_sk_ramofvariable_extract(eOprotBRD_t brd, eOprotID32_t id, void* epram)
{
    uint8_t* startofdata = epram;
    uint16_t offset = s_eoprot_sk_brdid2ramoffset(brd, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
} 






// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_sk_brdentityindex2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity, eOprotIndex_t index)
{
    uint16_t offset = 0;
    
    switch(entity)
    {
        case eosk_entity_skin:
        {   // the skin, if present, are all displaced at the beginning of the data in every as endpoint
            offset = 0;
            offset += sizeof(eOsk_skin_t)*index; 
        } break;
        
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;    
    }
    
    return(offset);  
}




static uint16_t s_eoprot_sk_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id)
{
    uint16_t offset = 0;
    eOprotTag_t tag = eoprot_ID2tag(id);
    
    if(EOK_uint08dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    
    eOprotEntity_t entity = eoprot_ID2entity(id);
    uint16_t index = eoprot_ID2index(id);
    
    switch(entity)
    {
        case eosk_entity_skin:
        {   // the skin are all displaced at the beginning of the data in every sk endpoint
            offset = index*sizeof(eOsk_skin_t) + eoprot_sk_rom_skin_get_offset(tag); 
        } break;   
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;    
    }
    
    return(offset);  
}

static eObool_t s_eoprot_sk_tag_skin_is_valid(eOprotTag_t tag)
{   // in case of holes in tags ... change the code
    return((tag < eoprot_tags_sk_skin_numberof)?(eobool_true):(eobool_false));
}


//static uint16_t s_eoprot_sk_skins_numberof_Get(eOprotBRD_t brd)
//{
////    if((NULL == s_eoprot_sk_board_numberofeachentity) || (NULL == s_eoprot_sk_board_numberofeachentity[brd]))
////    {
////        return(0);
////    }
//    
//    return(s_eoprot_sk_board_numberofeachentity[brd][eosk_entity_skin]);
//}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




