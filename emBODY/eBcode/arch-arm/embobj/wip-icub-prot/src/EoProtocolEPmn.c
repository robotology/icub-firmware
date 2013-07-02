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

/* @file       EoProtocolEPmn.c
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

#include "EoProtocolEPmn_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPmn.h"


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
EO_VERIFYproposition(eoprot_ep_mn_ded34, eoprot_ep_mn_entities_numberof <= eoprot_entities_numberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_ep_mn_tagsnum_co, eoprot_ep_mn_comm_tags_numberof == eoprot_ep_mn_comm_rwmodes_numberof);
EO_VERIFYproposition(eoprot_ep_mn_tagsmax_co, eoprot_ep_mn_comm_tags_numberof <= eoprot_tags_numberof);

EO_VERIFYproposition(eoprot_ep_mn_tagsnum_ap, eoprot_ep_mn_appl_tags_numberof == eoprot_ep_mn_appl_rwmodes_numberof);
EO_VERIFYproposition(eoprot_ep_mn_tagsmax_ap, eoprot_ep_mn_appl_tags_numberof <= eoprot_tags_numberof);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_mn_brdid2ramoffset(eOprotBRD_t brd, eOprotID_t id);

static eObool_t s_eoprot_ep_mn_comm_tag_is_valid(eOprotTag_t tag);
static eObool_t s_eoprot_ep_mn_appl_tag_is_valid(eOprotTag_t tag);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if 0   // dynamic mode
static const uint8_t** s_eoprot_ep_mn_board_numberofeachentity = NULL; // s_eoprot_ep_mn_board_numberofeachentity[brd][entype] tells how many entities of kind entype are in brd
static uint16_t s_eoprot_ep_mn_board_numberofthem = 0;
#endif
static const uint8_t* s_eoprot_ep_mn_board_numberofeachentity[EOCFG_EPS_BOARDS_MAXNUMBER] = { NULL };
static const uint16_t s_eoprot_ep_mn_board_numberofthem = EOCFG_EPS_BOARDS_MAXNUMBER;

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


#if 0   // dynamic mode
extern eOresult_t eoprot_ep_mn_number_of_boards_Load(uint16_t numofboards)
{
    if((0 != s_eoprot_ep_mn_board_numberofthem) || (NULL != s_eoprot_ep_mn_board_numberofeachentity))
    {
        return(eores_NOK_generic);
    }
    
    s_eoprot_ep_mn_board_numberofthem = numofboards;
    s_eoprot_ep_mn_board_numberofeachentity = (const uint8_t**) calloc(numofboards, sizeof(uint8_t*));
    
    return(eores_OK);   
}
#endif

extern eOresult_t eoprot_ep_mn_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity)
{
    if(NULL == s_eoprot_ep_mn_board_numberofeachentity)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_ep_mn_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_ep_mn_board_numberofeachentity[brd] = numberofeachentity;
    
    return(eores_OK);       
}


extern eObool_t eoprot_ep_mn_variables_id_isvalid(eOprotBRD_t brd, eOnvID_t id)
{
    eObool_t ret = eobool_false;    
    
    if((NULL == s_eoprot_ep_mn_board_numberofeachentity) || (NULL == s_eoprot_ep_mn_board_numberofeachentity[brd]))
    {
        return(eobool_false);
    }    
    
    eOprotEntity_t ent = eoprot_ep_variable_ID2entity(eoprot_endpoint_management, id);
    eOprotIndex_t  ind = eoprot_ep_variable_ID2index(eoprot_endpoint_management, id);
    eOprotTag_t    tag = eoprot_ep_variable_ID2tag(eoprot_endpoint_management, id);
    
    switch(ent)
    {
        case eomn_entity_comm:
        {   
            if(ind < s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_comm])
            {
                ret = s_eoprot_ep_mn_comm_tag_is_valid(tag);
            }            
        } break;
        
        case eomn_entity_appl:
        {
            if(ind < s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_appl]) 
            {
                ret = s_eoprot_ep_mn_appl_tag_is_valid(tag);
            }            
        } break;        
        
        default:
        {           
        } break;        
    }
    
    return(ret);     
}


extern uint16_t eoprot_ep_mn_variables_numberof_Get(eOprotBRD_t brd)
{
    uint16_t num = 0;
    
    if((NULL == s_eoprot_ep_mn_board_numberofeachentity) || (NULL == s_eoprot_ep_mn_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    num = eoprot_ep_mn_comm_tags_numberof * s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_comm] +
          eoprot_ep_mn_appl_tags_numberof * s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_appl];
          
    return(num);
}

extern eOprotID_t eoprot_ep_mn_variable_idfromprognumber_Get(eOprotBRD_t brd, uint16_t prog)
{
    eOprotTag_t tag = 0xffff;
    eOprotIndex_t index = 0xff;
    eOprotEntity_t entity = 0xff;
    
    if((NULL == s_eoprot_ep_mn_board_numberofeachentity) || (NULL == s_eoprot_ep_mn_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    uint16_t progsincomms = eoprot_ep_mn_comm_tags_numberof * s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_comm];
    uint16_t progsinappls = eoprot_ep_mn_appl_tags_numberof * s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_appl];
    
    if((0 != progsincomms) && (prog < (progsincomms)))
    {   // entity is eomn_entity_comm 
        entity  = eomn_entity_comm;
        index   = prog / eoprot_ep_mn_comm_tags_numberof;       // eoprot_ep_mn_comm_tags_numberof cannot be zero if progsincomms is non-zero
        tag     = prog % eoprot_ep_mn_comm_tags_numberof;        
    }
    else if((0 != progsinappls) && (prog < (progsincomms + progsinappls)))
    {   // entity is eomn_entity_appl 
        prog -= progsincomms;
        entity  = eomn_entity_appl;
        index   = prog / eoprot_ep_mn_appl_tags_numberof;       // eoprot_ep_mn_appl_tags_numberof cannot be zero if progsinappls is non-zero
        tag     = prog % eoprot_ep_mn_appl_tags_numberof;                      
    }
    else
    {
        return(EOK_uint16dummy);
    }
    
    return(eoprot_ep_variable_ID_get(eoprot_endpoint_management, entity, index, tag));
    
}

extern uint16_t eoprot_ep_mn_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID_t id)
{
    uint16_t prog = 0;
    
    if((NULL == s_eoprot_ep_mn_board_numberofeachentity) || (NULL == s_eoprot_ep_mn_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_management, id);
    eOprotIndex_t index = eoprot_ep_variable_ID2index(eoprot_endpoint_management, id);
    switch(entity)
    {
        case eomn_entity_comm: 
        {
            prog = index*eoprot_ep_mn_comm_tags_numberof + eoprot_ep_mn_rom_get_prognum(id);
        } break;
        
        case eomn_entity_appl: 
        {
            prog = s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_comm]*eoprot_ep_mn_comm_tags_numberof +
                   index*eoprot_ep_mn_appl_tags_numberof + eoprot_ep_mn_rom_get_prognum(id);
        } break; 
                
        default:
        {   
            prog = 0;
        } break;    
    }
    
    return(prog);
}

extern uint16_t eoprot_ep_mn_comms_numberof_Get(eOprotBRD_t brd)
{
    if((NULL == s_eoprot_ep_mn_board_numberofeachentity) || (NULL == s_eoprot_ep_mn_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    return(s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_comm]);
}

extern uint16_t eoprot_ep_mn_appls_numberof_Get(eOprotBRD_t brd)
{
    if((NULL == s_eoprot_ep_mn_board_numberofeachentity) || (NULL == s_eoprot_ep_mn_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    return(s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_appl]);
}


extern uint16_t eoprot_ep_mn_ram_sizeof_Get(eOprotBRD_t brd)
{
    uint16_t size = 0;
    
    if((NULL == s_eoprot_ep_mn_board_numberofeachentity) || (NULL == s_eoprot_ep_mn_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    //#warning --> it is valid only if organisation of memory is comm appl
    size = sizeof(eOmn_comm_t) * s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_comm] +
           sizeof(eOmn_appl_t) * s_eoprot_ep_mn_board_numberofeachentity[brd][eomn_entity_appl];
    
    return(size);
}

extern uint16_t eoprot_ep_mn_variable_ram_sizeof_Get(eOprotID_t id)
{
    return(eoprot_ep_mn_rom_get_sizeofvar(id));
}

extern void* eoprot_ep_mn_variable_ram_Extract(void* epram, eOprotBRD_t brd, eOprotID_t id)
{
    uint8_t* startofdata = epram;
    uint16_t offset = s_eoprot_ep_mn_brdid2ramoffset(brd, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
} 

extern void* eoprot_ep_mn_variable_rom_Get(eOprotID_t id)
{
    return(eoprot_ep_mn_rom_get_nvrom(id));
}

extern const eOmn_comm_t* eoprot_ep_mn_comm_default_Get(void)
{
    return(&eoprot_ep_mn_rom_comm_defaultvalue);
}

extern const eOmn_appl_t* eoprot_ep_mn_appl_default_Get(void)
{
    return(&eoprot_ep_mn_rom_appl_defaultvalue);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_mn_brdid2ramoffset(eOprotBRD_t brd, eOprotID_t id)
{
    uint16_t offset = 0;
    uint16_t tag = eoprot_ep_variable_ID2tag(eoprot_endpoint_management, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    //#warning --> it is valid only if organisation of memory is co[n], ap[m]
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_management, id);
    uint16_t index = eoprot_ep_variable_ID2index(eoprot_endpoint_management, id);
    
    switch(entity)
    {
        case eomn_entity_comm:
        {   // the comm are all displaced at the beginning of the data in every mn endpoint
            offset = index*sizeof(eOmn_comm_t) + eoprot_ep_mn_rom_comm_get_offset(tag); 
        } break;
        
        case eomn_entity_appl:
        {   // the appls are placed after all the comms. 
            uint16_t numberofcomms = eoprot_ep_mn_comms_numberof_Get(brd);
            offset = numberofcomms*sizeof(eOmn_comm_t) + index*sizeof(eOmn_appl_t) + eoprot_ep_mn_rom_appl_get_offset(tag);  
        } break;      
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;    
    }
    
    return(offset);  
}

static eObool_t s_eoprot_ep_mn_comm_tag_is_valid(eOprotTag_t tag)
{   // in case of holes in tags ... change the code
    return((tag < eoprot_ep_mn_comm_tags_numberof)?(eobool_true):(eobool_false));
}

static eObool_t s_eoprot_ep_mn_appl_tag_is_valid(eOprotTag_t tag)
{   // in case of holes in tags ... change the code  
    return((tag < eoprot_ep_mn_appl_tags_numberof)?(eobool_true):(eobool_false));
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



