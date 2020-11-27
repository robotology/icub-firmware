/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "EoCommon.h"


#include "EOtheMemoryPool.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANmapping.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANmapping_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


const eOcanmap_cfg_t eo_canmap_DefaultCfg = 
{
    0
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_eocanmap_is_entity_supported(eOprotEndpoint_t ep, eOprotEntity_t entity);

static const eOcanmap_board_extended_t * s_eo_canmap_GetBoardEXT(EOtheCANmapping *p, eObrd_canlocation_t loc);

static void s_eo_canmap_entities_clear(eOcanmap_board_extended_t * theboard);

static eObool_t s_eo_canmap_entities_noneispresent(const eOcanmap_board_extended_t * theboard);

static void s_eo_canmap_entities_index_add(eOcanmap_board_extended_t * theboard, uint8_t ep, uint8_t en, const eOcanmap_entitydescriptor_t *des);

static void s_eo_canmap_entities_rem(eOcanmap_board_extended_t * theboard, uint8_t ep, uint8_t en, const eOcanmap_entitydescriptor_t *des);

static eOprotIndex_t s_eo_canmap_entities_index_get(const eOcanmap_board_extended_t * theboard, eObrd_canlocation_t loc, uint8_t ep, uint8_t entity);

static eOprotIndex_t s_eo_canmap_mc_index_get(const eOcanmap_board_extended_t * theboard, eObrd_canlocation_t loc);

static eOprotIndex_t s_eo_canmap_sk_index_get(const eOcanmap_board_extended_t * theboard);

static eObrd_caninsideindex_t s_eo_canmap_entities_caninsideindex_get(const eOcanmap_board_t *board, const eOprotIndex_t index);

static eObrd_canlocation_t s_eo_canmap_compute_location(const eOcanmap_board_extended_t * theboard, uint8_t ep, uint8_t entity, uint8_t index);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


//static const char s_eobj_ownname[] = "EOtheCANmapping";

// note: by using [2][16], i can be sure that any value of .port and .addr inside eObrd_canlocation_t will
//       always be inside the array, so easier checks are done. 
//       i know, i know ... we dont use addr 0 because it is reserved for ems and dont use addr 15 because it is broadcast.
//       but who cares wasting two positions ...

static eOcanmap_board_extended_t s_eo_canmap_boards_ram[2][16] = {0};


static eOcanmap_board_extended_t * s_eo_canmap_boards_can1[16] =
{   // they must be 16
    NULL,   // the ems has address 0
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL, 
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL    // unused
}; 

static eOcanmap_board_extended_t * s_eo_canmap_boards_can2[16] =
{   // they must be 15
    NULL,   // the ems has address 0
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL, 
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL    // unused
}; 


static eOcanmap_board_extended_t ** s_eo_canmap_canmapcfg_boards[2] =
{   
    (eOcanmap_board_extended_t **)&s_eo_canmap_boards_can1,   
    (eOcanmap_board_extended_t **)&s_eo_canmap_boards_can2
};  EO_VERIFYsizeof(s_eo_canmap_canmapcfg_boards, 2*sizeof(eOcanmap_board_extended_t **))   



static EOarray* s_eo_canmap_array_joints[eocanmap_joints_maxnumberof] = { NULL };
static EOarray* s_eo_canmap_array_motors[eocanmap_motors_maxnumberof] = { NULL };

static EOarray* s_eo_canmap_array_strains[eocanmap_strains_maxnumberof] = { NULL };
static EOarray* s_eo_canmap_array_maises[eocanmap_maises_maxnumberof] = { NULL };
static EOarray* s_eo_canmap_array_temperatures[eocanmap_temperatures_maxnumberof] = { NULL };
static EOarray* s_eo_canmap_array_inertials[eocanmap_inertials_maxnumberof] = { NULL };
static EOarray* s_eo_canmap_array_inertials3[eocanmap_inertials3_maxnumberof] = { NULL };
static EOarray* s_eo_canmap_array_pscs[eocanmap_pscs_maxnumberof] = { NULL };
static EOarray* s_eo_canmap_array_poses[eocanmap_poses_maxnumberof] = { NULL };

static EOarray* s_eo_canmap_array_skins[eocanmap_skins_maxnumberof] = { NULL };


static EOtheCANmapping s_eo_canmap_singleton = 
{
	EO_INIT(.config)
    {   
        0
    },
    EO_INIT(.canmapping) NULL,
    EO_INIT(.arrayOfBRDEXTptr) 
    { 
        s_eo_canmap_array_joints, s_eo_canmap_array_motors,
        
        s_eo_canmap_array_strains, s_eo_canmap_array_maises, s_eo_canmap_array_temperatures, 
        s_eo_canmap_array_inertials, s_eo_canmap_array_inertials3, s_eo_canmap_array_pscs, s_eo_canmap_array_poses,
        
        s_eo_canmap_array_skins
    }
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheCANmapping * eo_canmap_Initialise(const eOcanmap_cfg_t *canmapcfg)
{
    if(NULL == canmapcfg)
    {
        canmapcfg = &eo_canmap_DefaultCfg;    
    }
    
    if(NULL != s_eo_canmap_singleton.canmapping)
    {
        return(&s_eo_canmap_singleton);
    }

    memcpy(&s_eo_canmap_singleton.config, canmapcfg, sizeof(eOcanmap_cfg_t));
    
    s_eo_canmap_singleton.canmapping = s_eo_canmap_canmapcfg_boards;
  
    
    uint8_t m = 0;
    
    m = eocanmap_posOfEPEN(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    for(uint8_t n=0; n<eocanmap_joints_maxnumberof; n++)
    {    
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_joint_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL); 
    }

    m = eocanmap_posOfEPEN(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
    for(uint8_t n=0; n<eocanmap_motors_maxnumberof; n++)
    {    
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_motor_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL); 
    }
    
    m = eocanmap_posOfEPEN(eoprot_endpoint_analogsensors, eoprot_entity_as_strain);
    for(uint8_t n=0; n<eocanmap_strains_maxnumberof; n++)
    {
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_strain_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL); 
    }
    
    m = eocanmap_posOfEPEN(eoprot_endpoint_analogsensors, eoprot_entity_as_mais);
    for(uint8_t n=0; n<eocanmap_maises_maxnumberof; n++)
    {
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_mais_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL);
    }
    
    m = eocanmap_posOfEPEN(eoprot_endpoint_analogsensors, eoprot_entity_as_temperature);
    for(uint8_t n=0; n<eocanmap_temperatures_maxnumberof; n++)
    {
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_temperature_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL); 
    }
        
    m = eocanmap_posOfEPEN(eoprot_endpoint_analogsensors, eoprot_entity_as_inertial);
    for(uint8_t n=0; n<eocanmap_inertials_maxnumberof; n++)
    {
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_inertial_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL); 
    }

    m = eocanmap_posOfEPEN(eoprot_endpoint_analogsensors, eoprot_entity_as_inertial3);
    for(uint8_t n=0; n<eocanmap_inertials3_maxnumberof; n++)
    {
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_inertial3_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL); 
    }

    m = eocanmap_posOfEPEN(eoprot_endpoint_analogsensors, eoprot_entity_as_psc);
    for(uint8_t n=0; n<eocanmap_pscs_maxnumberof; n++)
    {
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_psc_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL); 
    }

    m = eocanmap_posOfEPEN(eoprot_endpoint_analogsensors, eoprot_entity_as_pos);
    for(uint8_t n=0; n<eocanmap_poses_maxnumberof; n++)
    {
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_pos_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL); 
    }
    
    m = eocanmap_posOfEPEN(eoprot_endpoint_skin, eoprot_entity_sk_skin);
    for(uint8_t n=0; n<eocanmap_skins_maxnumberof; n++)
    {
        s_eo_canmap_singleton.arrayOfBRDEXTptr[m][n] = eo_array_New(eocanmap_skin_boards_maxnumberof, sizeof(eOcanmap_board_extended_t*), NULL); 
    }

    
    uint8_t i=0;
    uint8_t j=0;
    for(i=0; i<2; i++)
    {
        for(j=0; j<16; j++)
        {
            // i use board.props.type to understand if to load a board.
            s_eo_canmap_boards_ram[i][j].board.props.type = eobrd_cantype_none;
            s_eo_canmap_entities_clear(&s_eo_canmap_boards_ram[i][j]);
        }
    }

    return(&s_eo_canmap_singleton);
}



extern EOtheCANmapping* eo_canmap_GetHandle(void) 
{
    return(&s_eo_canmap_singleton);    
}



extern eOresult_t eo_canmap_LoadBoards(EOtheCANmapping *p,  EOconstvector *vectorof_boardprops)
{//ok
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    uint8_t num = 0;
    if((NULL == vectorof_boardprops) || (0 == (num = eo_constvector_Size(vectorof_boardprops))))
    {
        return(eores_NOK_generic);
    }
    
    uint8_t i = 0;
    for(i=0; i<num; i++)
    {   // note: i can use location.port and location.addr safely in addressing memory because their max values are 1 and 15 respectively.
        eObrd_canproperties_t *prop = (eObrd_canproperties_t*)eo_constvector_At(vectorof_boardprops, i);
        eOcanmap_board_extended_t *boardext = &s_eo_canmap_boards_ram[prop->location.port][prop->location.addr];
        // now that we have a boardext, we must NOT memset it to 0!!! we must keep the existing entities.indexof[] values.
        // the problem does not apply if this board is used by one service only. BUT if it is used by more than one, THEN ...
        // Example: i call eo_canmap_LoadBoards() for skin service and i load some mtb boards. then i call it again for inertial and i have the same mtb boards.
        // in such a case, i must not load the board again.

        if(NULL == s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr])
        {   // it means that noboby uses this board yet. i perform a full initialisation
            memcpy(&boardext->board.props, prop, sizeof(eObrd_canproperties_t));
            s_eo_canmap_entities_clear(boardext);
            s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr] = boardext;
        }
        else
        {   // it means that someone else already is using the board OR that i have loaded them twice. what i can do in here is just to verify if there are problems.
            eOcanmap_board_extended_t *existing = s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr];
            if(existing->board.props.type != prop->type)
            {
                // send diagnostics because someone is attempting to load at the same address a different board type
            }            
        }
    }
     
    return(eores_OK);    
}


extern eOresult_t eo_canmap_UnloadBoards(EOtheCANmapping *p,  EOconstvector *vectorof_boardprops)
{//ok
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    uint8_t num = 0;
    if((NULL == vectorof_boardprops) || (0 == (num = eo_constvector_Size(vectorof_boardprops))))
    {
        return(eores_NOK_generic);
    }
    
    uint8_t i = 0;
    for(i=0; i<num; i++)
    {   // note: i can use location.port and location.addr safely in addressing memory because their max values are 1 and 15 respectively.
        
        // remove board only if it is not used by any entity
                
        eObrd_canproperties_t *prop = (eObrd_canproperties_t*)eo_constvector_At(vectorof_boardprops, i);
        eOcanmap_board_extended_t *boardext = &s_eo_canmap_boards_ram[prop->location.port][prop->location.addr];
        if(eobool_true == s_eo_canmap_entities_noneispresent(boardext)) 
        {
            memset(boardext, 0, sizeof(eOcanmap_board_extended_t));
            memset(&boardext->board.props, 0, sizeof(eObrd_canproperties_t));
            s_eo_canmap_entities_clear(boardext);
            boardext->board.props.type = eobrd_cantype_none;
            s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr] = NULL;
        }
        
    }
     
    return(eores_OK);    
}



extern eOresult_t eo_canmap_ConfigEntity(EOtheCANmapping *p,  eOprotEndpoint_t ep, eOprotEntity_t entity, EOconstvector *vectorof_entitydescriptors)
{//ok
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
         
    if(eobool_false == s_eocanmap_is_entity_supported(ep, entity))
    {
        return(eores_NOK_generic);
    }
    
    // if it is supported, then also eocanmap_posOfEPEN() is valid
           
    if((NULL == vectorof_entitydescriptors) || (0 == eo_constvector_Size(vectorof_entitydescriptors)))
    {
        return(eores_NOK_generic);
    }   

    // numofcanlocations is the number of canlocations used to define the entity. 
    // maxnumofentities is the maximum possible index of a given entity.
    // for instance: 
    // - for mc4can-based motion control of eb2 with 12 joints we have 12 can locations (3 mc4 boards, each with two 
    //   can addresses, each can address with two indices). 
    //   hence: numofcanlocations = 12, maxnumofentities = 12 (as jomo entity)
    // - for skin of board eb2 which has only one entity and uses 7 can boards we have 7 can locations. 
    //   hence: hence: numofcanlocations = 7, maxnumofentities = 2 (as skin entity)
    
    uint8_t numofcanlocations = eo_constvector_Size(vectorof_entitydescriptors);
    uint8_t maxnumofentities = eocanmap_maxINDEX(ep, entity);
    
    
    // ok, i allocate the pointers     
    uint8_t i = 0;
    for(i=0; i<numofcanlocations; i++)
    {
        eOcanmap_entitydescriptor_t *des = (eOcanmap_entitydescriptor_t*) eo_constvector_At(vectorof_entitydescriptors, i); 
        if(des->index >= maxnumofentities)
        {   // something is going wrong
            continue;
        }
        eObrd_canlocation_t *loc = &des->location; 
        // now .... i retrieve the board.
        eOcanmap_board_extended_t *boardext = s_eo_canmap_singleton.canmapping[loc->port][loc->addr];
        if(NULL == boardext)
        {
            continue;
        }
        
        // i check that the board is compatible with the given {ep, entity}       
        if(eobool_false == eocanmap_BRDisCompatible((eObrd_cantype_t)(boardext->board.props.type), ep, entity))
        {
            continue;
        }            
        
        // everything is ok.
        
        // 1. i add an index
        s_eo_canmap_entities_index_add(boardext, ep, entity, des);
        
        // 2. assign the board
        
        uint8_t epen = eocanmap_posOfEPEN(ep, entity);        
        EOarray *boardsEPENINDEX = s_eo_canmap_singleton.arrayOfBRDEXTptr[epen][des->index];        
        eo_array_PushBack(boardsEPENINDEX, &boardext);            
    }
    
    return(eores_OK);     
}

// it removes info about the entity, but does not remove the board
extern eOresult_t eo_canmap_DeconfigEntity(EOtheCANmapping *p,  eOprotEndpoint_t ep, eOprotEntity_t entity, EOconstvector *vectorof_entitydescriptors)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
         
    if(eobool_false == s_eocanmap_is_entity_supported(ep, entity))
    {
        return(eores_NOK_generic);
    }
    
    // if it is supported, then also eocanmap_posOfEPEN() is valid
           
    if((NULL == vectorof_entitydescriptors) || (0 == eo_constvector_Size(vectorof_entitydescriptors)))
    {
        return(eores_NOK_generic);
    }   

    uint8_t numofcanlocations = eo_constvector_Size(vectorof_entitydescriptors);
    uint8_t maxnumofentities = eocanmap_maxINDEX(ep, entity);
    
    
    // ok, i allocate the pointers     
    uint8_t i = 0;
    for(i=0; i<numofcanlocations; i++)
    {
        eOcanmap_entitydescriptor_t *des = (eOcanmap_entitydescriptor_t*) eo_constvector_At(vectorof_entitydescriptors, i); 
        if(des->index >= maxnumofentities)
        {   // something is going wrong
            continue;
        }
        eObrd_canlocation_t *loc = &des->location; 
        // now .... i retrieve the board.
        eOcanmap_board_extended_t *boardext = s_eo_canmap_singleton.canmapping[loc->port][loc->addr];
        if(NULL == boardext)
        {
            continue;
        }
        
        // i dont check vs correct board anymore.
        
        // 1. rem an index
        s_eo_canmap_entities_rem(boardext, ep, entity, des);
        
        // 2. remove the board. we assume the same order as when we have added ... in this mode i just do a popback rather than a remove ... which teh objetc EOarray does not have
        
        uint8_t epen = eocanmap_posOfEPEN(ep, entity);        
        EOarray *boardsEPENINDEX = s_eo_canmap_singleton.arrayOfBRDEXTptr[epen][des->index];        
        eo_array_PopBack(boardsEPENINDEX);                          
    }
    
    return(eores_OK);     
}



extern eObrd_cantype_t eo_canmap_GetBoardType(EOtheCANmapping *p, eObrd_canlocation_t loc)
{//ok   
    const eOcanmap_board_extended_t *brd = s_eo_canmap_GetBoardEXT(p, loc);
    if(NULL == brd)
    {
        return(eobrd_cantype_unknown);
    }
    return((eObrd_cantype_t)brd->board.props.type);
}


extern eOresult_t eo_canmap_BoardSetDetected(EOtheCANmapping *p, eObrd_canlocation_t bloc, eObrd_info_t *detected)
{//ok
    if(NULL == detected)
    {
        return(eores_NOK_nullpointer);
    }
    
    eOcanmap_board_extended_t * board = s_eo_canmap_singleton.canmapping[bloc.port][bloc.addr];  
    
    if(NULL == board)
    {
        return(eores_NOK_generic);
    }
    
    memcpy(&board->detected, detected, sizeof(eObrd_info_t));
    
    return(eores_OK);
}


// but if we use ep and entity we surely have cross-check  
extern eOprotIndex_t eo_canmap_GetEntityIndex(EOtheCANmapping *p, eObrd_canlocation_t loc, eOprotEndpoint_t ep, eOprotEntity_t entity)
{//ok   
    // safe: port and addr dont go beyond memory
    const eOcanmap_board_extended_t * theboard = s_eo_canmap_singleton.canmapping[loc.port][loc.addr];
    
    if(NULL == theboard)
    {   // it means that we dont have such a board at the specified location
        return(EOK_uint08dummy);
    }

    return(s_eo_canmap_entities_index_get(theboard, loc, ep, entity));
}


extern eOresult_t eo_canmap_GetEntityLocation(EOtheCANmapping *p, eOprotID32_t id32, eObrd_canlocation_t *loc, uint8_t *numoflocs, eObrd_cantype_t *boardtype)
{//ok
    if(NULL == loc)
    {   
        return(eores_NOK_generic);
    }  
    
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id32);
    eOprotEntity_t entity = eoprot_ID2entity(id32);
    eOprotIndex_t index = eoprot_ID2index(id32);    
    uint8_t epen = eocanmap_posOfEPEN(ep, entity);
    
    // now i verify ep and entity
    if(epen >= eocanmap_entities_maxnumberof)
    {
        return eores_NOK_generic;
    } 
    
    // now i verify the index     
    if(index > eocanmap_maxINDEX(ep, entity))
    {
        return eores_NOK_generic;
    }
    
    // ok: i get the board array which contains pointers to eOcanmap_board_extended_t in such a number 
    // as needed by the entity (all need 1 apart skin (<=8) and psc (=8) and pos)
    EOarray *boardsEPENINDEX = s_eo_canmap_singleton.arrayOfBRDEXTptr[epen][index];   
    
    if(NULL == boardsEPENINDEX)
    {
        return eores_NOK_generic;
    }
    
    uint8_t numofboards = eo_array_Size(boardsEPENINDEX);
    
    if(0 == numofboards)
    {
        return eores_NOK_generic;
    }
    
    // as numofboards is non-zero: i surely can call the eo_array_At(). then i assume that i added meaningful data inside
    // (because i put it inside it) and i expect to find the pointer to the eOcanmap_board_extended_t inside *thefirstboardpp
    // hence i may skip: check of thefirstboardpp vs NULL, check of *thefirstboardpp vs NULL  
    eOcanmap_board_extended_t **thefirstboardpp = (eOcanmap_board_extended_t **) eo_array_At(boardsEPENINDEX, 0);
 
#undef EXTRA_CHECK_ANYWAY   
#if defined(EXTRA_CHECK_ANYWAY)

    // it must not be NULL. 
    if(NULL == thefirstboardpp)
    {
        return eores_NOK_generic;
    }
    
    // and also the pointer board
    if(NULL == *thefirstboardpp)
    {
        return eores_NOK_generic;
    }
    
#endif   

    // ok: in *thefirstboardpp we have the pointer to a valid board. we check that the board type is coherent for the entity.
    if(eobool_false == eocanmap_BRDisCompatible((eObrd_cantype_t)((*thefirstboardpp)->board.props.type), ep, entity))
    {
        return eores_NOK_generic;
    }
    

    // we get the values of loc, boardtype, numoflocs
    
    *loc = s_eo_canmap_compute_location(*thefirstboardpp, ep, entity, index);
    if(NULL != boardtype)
    {
        *boardtype = (eObrd_cantype_t)(*thefirstboardpp)->board.props.type;
    }    
    if(NULL != numoflocs)
    {
        *numoflocs = numofboards;
    }    
        
    return(eores_OK);
}

// marco.accame on 2 apr 2019: first implementation is ... the same as the previuos one.
extern eOresult_t eo_canmap_GetEntityArrayOfLocations(EOtheCANmapping *p, eOprotID32_t id32, EOarray *arrayoflocs, eObrd_cantype_t *boardtype)
{
    if(NULL == arrayoflocs)
    {   
        return(eores_NOK_generic);
    }  
    
    eo_array_Reset(arrayoflocs);
    
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id32);
    eOprotEntity_t entity = eoprot_ID2entity(id32);
    eOprotIndex_t index = eoprot_ID2index(id32);    
    uint8_t epen = eocanmap_posOfEPEN(ep, entity);
    
    // now i verify ep and entity
    if(epen >= eocanmap_entities_maxnumberof)
    {
        return eores_NOK_generic;
    } 
    
    // now i verify the index     
    if(index > eocanmap_maxINDEX(ep, entity))
    {
        return eores_NOK_generic;
    }
    
    // ok: i get the board array which contains pointers to eOcanmap_board_extended_t 
    // in such a number as needed by the entity (all need 1 apart skin (<=8) and psc (=8) and pos )
    EOarray *boardsEPENINDEX = s_eo_canmap_singleton.arrayOfBRDEXTptr[epen][index];   
    
    if(NULL == boardsEPENINDEX)
    {
        return eores_NOK_generic;
    }
    
    uint8_t numofboards = eo_array_Size(boardsEPENINDEX);
    
    if(0 == numofboards)
    {
        return eores_NOK_generic;
    }
    
    // as numofboards is non-zero, i assume that i added meaningful data inside, hence i may not check anymore
    
    
    eObrd_cantype_t brd;
    for(uint8_t i=0; i<numofboards; i++)
    {
         // as numofboards is non-zero: i surely can call the eo_array_At(). then i assume that i added meaningful data inside
        // (because i put it inside it) and i expect to find the pointer to the eOcanmap_board_extended_t inside *boardpp
        // hence i may skip: check of boardpp vs NULL, check of *boardpp vs NULL  
        eOcanmap_board_extended_t **boardpp = (eOcanmap_board_extended_t **) eo_array_At(boardsEPENINDEX, i);
        
        if((NULL != boardpp) && (NULL != *boardpp))
        {
            brd = (eObrd_cantype_t)(*boardpp)->board.props.type;
            // i could do check that the board type is coherent for the entity.
            if(eobool_true == eocanmap_BRDisCompatible((eObrd_cantype_t)((*boardpp)->board.props.type), ep, entity))
            {
                eObrd_canlocation_t loc = s_eo_canmap_compute_location((*boardpp), ep, entity, index);
                eo_array_PushBack(arrayoflocs, &loc);   
            }
        }                            
    }

    if(NULL != boardtype)
    {
        *boardtype = brd;
    }      
        
    return(eores_OK);      
}

//#warning -> move it into board.c ??? if so, we must move also eocanmap_posOfEPEN() maybe in protocol.c
extern eObool_t eocanmap_BRDisCompatible(eObrd_cantype_t brd, eOprotEndpoint_t ep, eOprotEntity_t en)
{
    uint8_t epen = eocanmap_posOfEPEN(ep, en);
    if(epen >= eocanmap_entities_maxnumberof)
    {
        return eobool_false;
    } 
    if(eobool_false == eoboards_is_can(eoboards_cantype2type(brd)))
    {
        return eobool_false;
    }
    
    // epen: the order is joint-motor-strain-mais-temperature-inertial-inertial3-psc-pos-skin
    // brd: 
    static const uint32_t tableB[] = // [epen]
    {
        (1 << eobrd_cantype_mc4) | (1 << eobrd_cantype_foc) | (1 << eobrd_cantype_pmc),         // joint
        (1 << eobrd_cantype_mc4) | (1 << eobrd_cantype_foc) | (1 << eobrd_cantype_pmc),         // motor
        (1 << eobrd_cantype_strain) | (1 << eobrd_cantype_strain2),                             // strain
        (1 << eobrd_cantype_mais),                                                              // mais
        (1 << eobrd_cantype_mtb4) | (1 << eobrd_cantype_strain2),                               // temperature
        (1 << eobrd_cantype_mtb),                                                               // inertial
        (1 << eobrd_cantype_mtb4) | (1 << eobrd_cantype_strain2) | (1 << eobrd_cantype_rfe),    // inertial3
        (1 << eobrd_cantype_psc),                                                               // psc
        (1 << eobrd_cantype_pmc) | (1 << eobrd_cantype_psc),                                    // pos
        (1 << eobrd_cantype_mtb) | (1 << eobrd_cantype_mtb4) | (1 << eobrd_cantype_psc)         // skin
    }; EO_VERIFYsizeof(tableB, sizeof(const uint32_t)*(eocanmap_entities_maxnumberof))
    
    // it is safe to use brd because it is can hence it is < 32
    return eo_common_word_bitcheck(tableB[epen], brd);    
}    
   

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

EO_VERIFYproposition(eocanmap_posOfEPEN__pos, (eoprot_entities_numberof == 15))
// if error: change the magic number and put the new entity in the pos[][] map

extern uint8_t eocanmap_posOfEPEN(eOprotEndpoint_t ep, eOprotEntity_t en)
{
    enum { // keep in magic numbers. it is a check if any of the eoprot_xxx value changes
        s0 = 4, // eoprot_endpoints_numberof
        s1 = 7  // eoprot_entities_maxnumberofsupported
    };
    
    static const uint8_t pos[s0][s1] = // pos[eoprot_endpoints_numberof][eoprot_entities_maxnumberofsupported] = 
    {
        {0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf},  // ep->management [none, ...]
        {0,     1, 0xf, 0xf, 0xf, 0xf, 0xf},  // ep->mc [joi, mot, none, ...]
        {2,     3,   4,   5,   6,   7,   8},  // ep->as [str, mai, tem, ine, ine3, psc, pos]
        {9,   0xf, 0xf, 0xf, 0xf, 0xf, 0xf}   // ep->sk [sk, none]
    }; EO_VERIFYsizeof(pos, sizeof(const uint8_t)*(eoprot_endpoints_numberof)*(eoprot_entities_maxnumberofsupported))

    // the order is joint-motor-strain-mais-temperature-inertial-inertial3-psc-skin
    if((ep >= s0) || (en >= (s1)))
    {
        return 0x0f;
    }
    return pos[ep][en];    
}


extern uint8_t eocanmap_maxINDEX(eOprotEndpoint_t ep, eOprotEntity_t en)
{
    uint8_t epen = eocanmap_posOfEPEN(ep, en);
    if(epen >= eocanmap_entities_maxnumberof)
    {
        return 0;
    } 
    
    static const uint8_t numI[] = // [epen] the order is joint-motor-strain-mais-temperature-inertial-inertial3-psc-pmc-skin
    {
        eocanmap_joints_maxnumberof, eocanmap_motors_maxnumberof, 
        
        eocanmap_strains_maxnumberof, eocanmap_maises_maxnumberof, eocanmap_temperatures_maxnumberof, 
        eocanmap_inertials_maxnumberof, eocanmap_inertials3_maxnumberof, eocanmap_pscs_maxnumberof, 
        eocanmap_poses_maxnumberof,
        
        eocanmap_skins_maxnumberof
    }; EO_VERIFYsizeof(numI, sizeof(const uint8_t)*(eocanmap_entities_maxnumberof))
    
    return numI[epen];    
}


extern uint8_t eocanmap_maxBOARDnumber(eOprotEndpoint_t ep, eOprotEntity_t en)
{
    uint8_t epen = eocanmap_posOfEPEN(ep, en);
    if(epen >= eocanmap_entities_maxnumberof)
    {
        return 0;
    } 
    
    static const uint8_t numB[] = // [epen] the order is joint-motor-strain-mais-temperature-inertial-inertial3-psc-pos-skin
    {
        eocanmap_joint_boards_maxnumberof, eocanmap_motor_boards_maxnumberof, 
        
        eocanmap_strain_boards_maxnumberof, eocanmap_mais_boards_maxnumberof, eocanmap_temperature_boards_maxnumberof, 
        eocanmap_inertial_boards_maxnumberof, eocanmap_inertial3_boards_maxnumberof, eocanmap_psc_boards_maxnumberof, 
        eocanmap_pos_boards_maxnumberof,
        
        eocanmap_skin_boards_maxnumberof
    }; EO_VERIFYsizeof(numB, sizeof(const uint8_t)*(eocanmap_entities_maxnumberof))
    
    return numB[epen];    
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_canmap_entities_clear(eOcanmap_board_extended_t * theboard)
{//ok
    theboard->board.entities2.bitmapOfPresence = 0;
    theboard->board.entities2.compactIndicesOf = 0xff;
}

static eObool_t s_eo_canmap_entities_noneispresent(const eOcanmap_board_extended_t * theboard)
{//ok
    return (0 == theboard->board.entities2.bitmapOfPresence) ? (eobool_true) : (eobool_false);
}

static void s_eo_canmap_entities_index_add(eOcanmap_board_extended_t * theboard, uint8_t ep, uint8_t en, const eOcanmap_entitydescriptor_t *des)
{//ok
    uint8_t pos = eocanmap_posOfEPEN(ep, en);
    
    if(pos >= eocanmap_entities_maxnumberof)
    {
        return;
    }
    
    // 1. set presence of this entity    
    eo_common_hlfword_bitset(&theboard->board.entities2.bitmapOfPresence, pos);
    
    // 2. store the entity index, but only for mc and sk
    if((eoprot_endpoint_motioncontrol != ep) && (eoprot_endpoint_skin != ep))
    {
        return;
    }
    
    #warning TODO-pmc-motion-control: look at in here
    if((eobrd_mc4 == theboard->board.props.type) && (eoprot_endpoint_motioncontrol == ep) && (eobrd_caninsideindex_second == des->location.insideindex))
    {
        // we store in second position nibble-1
        uint8_t val = (des->index << 4) & 0xF0;
        theboard->board.entities2.compactIndicesOf &= 0x0F; // clear nib-1
        theboard->board.entities2.compactIndicesOf |= val;  // set nib-1              
    }
    else
    {
        // all other cases of motioncontrol and for skin: we store in first position nibble-0
        uint8_t val = des->index & 0x0F;
        theboard->board.entities2.compactIndicesOf &= 0xF0; // clear nib-0
        theboard->board.entities2.compactIndicesOf |= val;  // set nib-0      
    }    
}

static void s_eo_canmap_entities_rem(eOcanmap_board_extended_t * theboard, uint8_t ep, uint8_t en, const eOcanmap_entitydescriptor_t *des)
{//ok
    uint8_t pos = eocanmap_posOfEPEN(ep, en);

    if(pos >= eocanmap_entities_maxnumberof)
    {
        return;
    }
    
    // 1. clr presence of this entity    
    eo_common_hlfword_bitclear(&theboard->board.entities2.bitmapOfPresence, pos);
    
    // 2. clear the entity index, but only for mc and sk
    if((eoprot_endpoint_motioncontrol != ep) && (eoprot_endpoint_skin != ep))
    {
        return;
    }
    
    #warning TODO-pmc-motion-control: look at in here
    if((eobrd_mc4 == theboard->board.props.type) && (eoprot_endpoint_motioncontrol == ep) && (eobrd_caninsideindex_second == des->location.insideindex))
    {
        // we store in second position nibble-1
        uint8_t val = (entindexNONE << 4) & 0xF0;
        theboard->board.entities2.compactIndicesOf &= 0x0F; // clear nib-1
        theboard->board.entities2.compactIndicesOf |= val;  // set nib-1              
    }
    else
    {
        // all other cases of motioncontrol and for skin: we store in first position nibble-0
        uint8_t val = entindexNONE & 0x0F;
        theboard->board.entities2.compactIndicesOf &= 0xF0; // clear nib-0
        theboard->board.entities2.compactIndicesOf |= val;  // set nib-0      
    }
}

static eOprotIndex_t s_eo_canmap_mc_index_get(const eOcanmap_board_extended_t * theboard, eObrd_canlocation_t loc)
{//ok
    eOprotIndex_t index = EOK_uint08dummy;

    #warning TODO-pmc-motion-control: look at in here
    if(theboard->board.props.type == eobrd_cantype_foc) 
    {   // if 1foc it is always in nibble-0 
        index = theboard->board.entities2.compactIndicesOf & 0x0F;
    }
    else if(theboard->board.props.type == eobrd_cantype_mc4)
    {   // if mc4, index depends on value of loc.insideindex. it can be only first or second. if loc.insideindex is none or even else: nothing is done
                    
        if(eobrd_caninsideindex_first == loc.insideindex)
        {
            index = theboard->board.entities2.compactIndicesOf & 0x0F;
        }
        else if(eobrd_caninsideindex_second == loc.insideindex)
        {
            index = (theboard->board.entities2.compactIndicesOf >> 4) & 0x0F;
        }
    }      
    
    if(entindexNONE == index)
    {  
        index = EOK_uint08dummy;
    }
    return index; 
}

static eOprotIndex_t s_eo_canmap_sk_index_get(const eOcanmap_board_extended_t * theboard)
{//ok
    eOprotIndex_t index = EOK_uint08dummy;
    
    index = theboard->board.entities2.compactIndicesOf & 0x0F;    

    if(entindexNONE == index)
    {   // in case some board.entities.indexof[0] or board.entities.indexof[1] has a entindexNONE value we must be sure to return EOK_uint08dummy
        index = EOK_uint08dummy;
    }
    return index; 
}

static eOprotIndex_t s_eo_canmap_entities_index_get(const eOcanmap_board_extended_t * theboard, eObrd_canlocation_t loc, uint8_t ep, uint8_t entity)
{//ok
    eOprotIndex_t index = EOK_uint08dummy;

    // 1. check if we have this entity.
    
    uint8_t pos = eocanmap_posOfEPEN(ep, entity);
    
    if(pos >= eocanmap_entities_maxnumberof)
    {
        return index;
    }
     
    if(eobool_false == eo_common_hlfword_bitcheck(theboard->board.entities2.bitmapOfPresence, pos))
    {
        return index;
    }
    
    // 2. yes, we have it. now we retrieve it. all entities have zero index apart those on mc and sk
    if(eoprot_endpoint_motioncontrol == ep)
    {
        index = s_eo_canmap_mc_index_get(theboard, loc);
    }
    else if(eoprot_endpoint_skin == ep)
    {
        index = s_eo_canmap_sk_index_get(theboard);
    }
    else
    {   
        index = 0;
    }
    
    if(entindexNONE == index)
    {   // in case some board.entities.indexof[0] or board.entities.indexof[1] has a entindexNONE value we must be sure to return EOK_uint08dummy
        index = EOK_uint08dummy;
    }
    
    return index;    
}

static eObrd_canlocation_t s_eo_canmap_compute_location(const eOcanmap_board_extended_t * theboard, uint8_t ep, uint8_t entity, uint8_t index)
{    
    eObrd_canlocation_t loc;
    loc = theboard->board.props.location;
    loc.insideindex = eobrd_caninsideindex_none;
    
    if(eoprot_endpoint_motioncontrol == ep)
    {
        loc.insideindex = s_eo_canmap_entities_caninsideindex_get(&theboard->board, index);
    }
    
    return loc;
}



static eObrd_caninsideindex_t s_eo_canmap_entities_caninsideindex_get(const eOcanmap_board_t *board, const eOprotIndex_t index)
{//ok
    eObrd_caninsideindex_t inside = eobrd_caninsideindex_none;
    
    uint8_t tmp = board->entities2.compactIndicesOf;
    
    if(index == (tmp & 0x0F))
    {
        inside = eobrd_caninsideindex_first; // for 1foc we could use eobrd_caninsideindex_none, but it is ok even in this way
    }
    else if(index == ((tmp >> 4) & 0x0F))
    {
        inside = eobrd_caninsideindex_second;
    } 
    
    return inside;
}


static eObool_t s_eocanmap_is_entity_supported(eOprotEndpoint_t ep, eOprotEntity_t entity)
{
    if(eocanmap_posOfEPEN(ep, entity) >= eocanmap_entities_maxnumberof)
    {
        return eobool_false;
    } 
    return(eobool_true);
}


static const eOcanmap_board_extended_t * s_eo_canmap_GetBoardEXT(EOtheCANmapping *p, eObrd_canlocation_t bloc)
{//ok
    // safe because port is 1 bit-long and addr is 4-bits long, hence they dont map out of memory
    return((const eOcanmap_board_extended_t*)s_eo_canmap_singleton.canmapping[bloc.port][bloc.addr]);
}

    

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





