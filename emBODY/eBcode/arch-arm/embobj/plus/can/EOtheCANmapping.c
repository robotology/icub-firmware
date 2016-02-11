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

static uint8_t s_eo_canmap_max_entities(eOprotEndpoint_t ep, eOprotEntity_t entity);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


//static const char s_eobj_ownname[] = "EOtheCANmapping";

// note: by using [2][16], i can be sure that any value of .port and .addr inside eOcanmap_location_t will
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
};  EO_VERIFYsizeof(s_eo_canmap_canmapcfg_boards, 2*sizeof(eOcanmap_board_extended_t **));   


static eOcanmap_board_extended_t * s_eo_canmap_boards_mc_jomo[eocanmap_joints_maxnumberof] = {NULL};
static eOcanmap_board_extended_t * s_eo_canmap_boards_as_strain[eocanmap_strains_maxnumberof] = {NULL};
static eOcanmap_board_extended_t * s_eo_canmap_boards_as_mais[eocanmap_maises_maxnumberof] = {NULL};
static eOcanmap_board_extended_t * s_eo_canmap_boards_as_extorque[1] = {NULL};
static eOcanmap_board_extended_t * s_eo_canmap_boards_as_inertial[eocanmap_inertials_maxnumberof] = {NULL};
//static eOcanmap_board_extended_t * s_eo_canmap_boards_sk_skin[eocanmap_skins_maxnumberof] = {NULL};
static eOcanmap_board_extended_t * s_eo_canmap_boards_sk_skin_00[eocanmap_skin_index_boards_maxnumberof] = {NULL};
static eOcanmap_board_extended_t * s_eo_canmap_boards_sk_skin_01[eocanmap_skin_index_boards_maxnumberof] = {NULL};

static eOcanmap_board_extended_t ** s_eo_canmap_boards_mc[] =
{   
    s_eo_canmap_boards_mc_jomo,         // joints
    s_eo_canmap_boards_mc_jomo          // motors
};  

static eOcanmap_board_extended_t ** s_eo_canmap_boards_as[] =
{   
    s_eo_canmap_boards_as_strain,       // strain
    s_eo_canmap_boards_as_mais,         // mais
    s_eo_canmap_boards_as_extorque,     // extorque are not managed ...
    s_eo_canmap_boards_as_inertial      // inertial
};

//static eOcanmap_board_extended_t ** s_eo_canmap_boards_sk[] =
//{   
//    s_eo_canmap_boards_sk_skin_00,      // skin00
//    s_eo_canmap_boards_sk_skin_01       // skin01    
//};

static eOcanmap_board_extended_t *** s_eo_canmap_canmapcfg_entities[eoprot_endpoints_numberof] =
{   
    NULL,       // nothing for management
    (eOcanmap_board_extended_t ***)&s_eo_canmap_boards_mc,   
    (eOcanmap_board_extended_t ***)&s_eo_canmap_boards_as,
    NULL        // skin is managed in another way
}; 


static eOcanmap_board_extended_t ** s_eo_canmap_canmapcfg_skin[eocanmap_skins_maxnumberof] =
{   
    (eOcanmap_board_extended_t **)&s_eo_canmap_boards_sk_skin_00,   
    (eOcanmap_board_extended_t **)&s_eo_canmap_boards_sk_skin_01
}; 

static EOtheCANmapping s_eo_canmap_singleton = 
{
	.config             =
    {   
        0
    },
    .canmapping             = NULL,
    .entitylocation         = NULL,
    .skinlocation           = NULL,
    .numofskinboardsindex   = {0, 0},
//    .arrayofboardlocations  = {0},
    .tobedefined            = 0
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
    s_eo_canmap_singleton.entitylocation = s_eo_canmap_canmapcfg_entities;
    s_eo_canmap_singleton.skinlocation = s_eo_canmap_canmapcfg_skin;
    
//    eo_array_New(eocanmap_maxlocations, sizeof(eOcanmap_location_t), &s_eo_canmap_singleton.arrayofboardlocations);
    
    uint8_t i=0;
    uint8_t j=0;
    for(i=0; i<2; i++)
    {
        for(j=0; j<16; j++)
        {
            // i use board.props.type to understand if to load a board.
            s_eo_canmap_boards_ram[i][j].board.props.type = eobrd_cantype_none;
            // i use board.indexofentity[0,1] to understand if to unload a board 
            s_eo_canmap_boards_ram[i][j].board.indexofentity[0] = s_eo_canmap_boards_ram[i][j].board.indexofentity[1] = entindexNONE;
        }
    }
    
    
    // so far i dont clear canmapping[][] but i should do it.
    

    return(&s_eo_canmap_singleton);
}



extern EOtheCANmapping* eo_canmap_GetHandle(void) 
{
    return(&s_eo_canmap_singleton);    
}



extern eOresult_t eo_canmap_LoadBoards(EOtheCANmapping *p,  EOconstvector *vectorof_boardprops)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    uint8_t num = 0;
    if((NULL == vectorof_boardprops) || (0 == (num = eo_constvector_Size(vectorof_boardprops))))
    {
//        // must unload everything from the CAN
//        memset(s_eo_canmap_boards_can1, 0, sizeof(s_eo_canmap_boards_can1));
//        memset(s_eo_canmap_boards_can2, 0, sizeof(s_eo_canmap_boards_can2));
        return(eores_NOK_generic);
    }
    
    uint8_t i = 0;
    for(i=0; i<num; i++)
    {   // note: i can use location.port and location.addr safely in addressing memory because their max values are 1 and 15 respectively.
        eOcanmap_board_properties_t *prop = (eOcanmap_board_properties_t*)eo_constvector_At(vectorof_boardprops, i);
        eOcanmap_board_extended_t *boardext = &s_eo_canmap_boards_ram[prop->location.port][prop->location.addr];
        // now that we have a boardext, we must NOT memset it to 0!!! we must keep the existing indexofentity[] values.
        // the problem does not apply if this board is used by one service only. BUT if it is used by more than one, THEN ...
        // Example: i call eo_canmap_LoadBoards() for skin service and i load some mtb boards. then i call it again for inertial and i have the same mtb boards.
        // in such a case, i must not load the board again.

        if(NULL == s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr])
        {   // it means that noboby uses this board yet. i perform a full initialisation
            memcpy(&boardext->board.props, prop, sizeof(eOcanmap_board_properties_t));
            boardext->board.indexofentity[0] = boardext->board.indexofentity[1] = entindexNONE;
            s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr] = boardext;
        }
        else
        {   // it means that someone else already is using the board OR that i have loader them twice. what i can do in here is just to verify if there are problems.
            eOcanmap_board_extended_t *existing = s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr];
            if(existing->board.props.type != prop->type)
            {
                // send diagnostics because someone is attempting to load at the same address a different board type
            }            
        }

//        if(NULL == s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr])
//        {   // first time we add the location
//            eo_array_PushBack((EOarray*)&s_eo_canmap_singleton.arrayofboardlocations, &prop->location);
//        }
        // i dont care if we load again in the same position
//        s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr] = boardext;
    }
     
    return(eores_OK);    
}


extern eOresult_t eo_canmap_UnloadBoards(EOtheCANmapping *p,  EOconstvector *vectorof_boardprops)
{
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
        
        // lo posso togliere. devo pero' capire cosa fare nel caso di board che offrono piu' servizi. 
        // ad esempio: la mtb che offre skin ed inertial.
        // remove board only if it is not used by any entity, thus if both indexofentity[0] and indexofentity[1] are equal to entindexNONE
        
        
        eOcanmap_board_properties_t *prop = (eOcanmap_board_properties_t*)eo_constvector_At(vectorof_boardprops, i);
        eOcanmap_board_extended_t *boardext = &s_eo_canmap_boards_ram[prop->location.port][prop->location.addr];
        if((entindexNONE == boardext->board.indexofentity[0]) && (entindexNONE == boardext->board.indexofentity[1])) 
        {
            memset(boardext, 0, sizeof(eOcanmap_board_extended_t));
            memset(&boardext->board.props, 0, sizeof(eOcanmap_board_properties_t));
            boardext->board.indexofentity[0] = boardext->board.indexofentity[1] = entindexNONE;
            boardext->board.props.type = eobrd_cantype_none;
            s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr] = NULL;
        }
        
        // vedi come fare per rimuovere questa location dall'array. dovrei cercare questo item, segnarmi l'indice e rimuovere l'item.
//        if(NULL == s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr])
//        {   // first time we add the location
//            eo_array_PushBack((EOarray*)&s_eo_canmap_singleton.arrayofboardlocations, &prop->location);
//        }
        
        
        // lo posso togliere. devo pero' capire cosa fare nel caso di board che offrono piu' servizi. 
        // ad esempio: la mtb che offre skin ed inertial
        //s_eo_canmap_singleton.canmapping[prop->location.port][prop->location.addr] = NULL;
    }
     
    return(eores_OK);    
}


extern eOresult_t eo_canmap_ConfigEntity(EOtheCANmapping *p,  eOprotEndpoint_t ep, eOprotEntity_t entity, EOconstvector *vectorof_entitydescriptors)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
         
    if(eobool_false == s_eocanmap_is_entity_supported(ep, entity))
    {
        return(eores_NOK_generic);
    }
           
    if((NULL == vectorof_entitydescriptors) || (0 == eo_constvector_Size(vectorof_entitydescriptors)))
    {
        return(eores_NOK_generic);
        // must unload the entity from CAN
        //memset(&s_eo_canmap_singleton.entitylocation[ep][entity], 0, sizeof(s_eo_canmap_singleton.entitylocation[ep][entity]));
    }   

    // numofcanlocations is the number of canlocations used to define the entity. for instance: 
    // - for mc4can-based motion control of eb2 with 12 joints we have 12 can locations (3 mc4 boards, each with two 
    //   can addresses, each can address with two indices).
    // - for skin of board eb2 which as only one entity but uses 7 can boards we have 7 can locations. but we have only ONE index.
    uint8_t numofcanlocations = eo_constvector_Size(vectorof_entitydescriptors);
    uint8_t maxnumofentities = s_eo_canmap_max_entities(ep, entity);
    
    
    // ok, i allocate the pointers     
    uint8_t i = 0;
    for(i=0; i<numofcanlocations; i++)
    {
        eOcanmap_entitydescriptor_t *des = (eOcanmap_entitydescriptor_t*) eo_constvector_At(vectorof_entitydescriptors, i); 
        if(des->index >= maxnumofentities)
        {   // something is going wrong
            continue;
        }
        eOcanmap_location_t *loc = &des->location; 
        // now .... i retrieve the board.
        eOcanmap_board_extended_t *boardext = s_eo_canmap_singleton.canmapping[loc->port][loc->addr];
        if(NULL == boardext)
        {
            continue;
        }
        // it must not be NULL .... but for now i dont check it
        // now i must do:
        // 1. set the boardext with the index of the entity.
        // 2. fill the entity array        
        if(eoprot_endpoint_motioncontrol == ep)
        {   // for motion control the board can be 1foc or mc4. 1foc manages 1 joint only and is placed in position 0 of indexofentity[]. mc4 manages two joints which are placed in position 0 and position 1
            boardext->board.indexofentity[loc->insideindex] = des->index;
            s_eo_canmap_singleton.entitylocation[ep][eoprot_entity_mc_joint][des->index] = boardext;
            s_eo_canmap_singleton.entitylocation[ep][eoprot_entity_mc_motor][des->index] = boardext;
        }   
        else if(eoprot_endpoint_skin == ep)
        {   // for skin endpoint the board is a mtb. for entity of type skin we use position 0 of indexofentity[]
            boardext->board.indexofentity[0] = des->index;  // as described in NOTE-123454321 we use position 0    
            uint8_t pos = s_eo_canmap_singleton.numofskinboardsindex[des->index];
            s_eo_canmap_singleton.skinlocation[des->index][pos] = boardext; 
            s_eo_canmap_singleton.numofskinboardsindex[des->index] ++;            
        }
        else if((eoprot_endpoint_analogsensors == ep) && (eoprot_entity_as_inertial == entity))
        {   // NOTE-123454321: for analog sensor endpoint and inertial entity the board is a mtb. we use position 1 of indexofentity[] because position 0 is reserved to skin entity managed by the same board
            boardext->board.indexofentity[1] = des->index; // as described in NOTE-123454321 we use position 1
            s_eo_canmap_singleton.entitylocation[ep][entity][des->index] = boardext;            
        }
        else 
        {   // all other cases are managed by a board only which manages only one entity. we use position 0 of indexofentity[]
            boardext->board.indexofentity[0] = des->index;
            s_eo_canmap_singleton.entitylocation[ep][entity][des->index] = boardext;
        }
    }
    
    return(eores_OK);     
}

// remove info about the entity, but does not remove the board
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
           
    if((NULL == vectorof_entitydescriptors) || (0 == eo_constvector_Size(vectorof_entitydescriptors)))
    {
        return(eores_NOK_generic);
    }   

    // numofcanlocations is the number of canlocations used to define the entity. for instance: 
    // - for mc4can-based motion control of eb2 with 12 joints we have 12 can locations (3 mc4 boards, each with two 
    //   can addresses, each can address with two indices).
    // - for skin of board eb2 which as only one entity but uses 7 can boards we have 7 can locations. but we have only ONE index.
    uint8_t numofcanlocations = eo_constvector_Size(vectorof_entitydescriptors);
    uint8_t maxnumofentities = s_eo_canmap_max_entities(ep, entity);
    
    
    // ok, i allocate the pointers     
    uint8_t i = 0;
    for(i=0; i<numofcanlocations; i++)
    {
        eOcanmap_entitydescriptor_t *des = (eOcanmap_entitydescriptor_t*) eo_constvector_At(vectorof_entitydescriptors, i); 
        if(des->index >= maxnumofentities)
        {   // something is going wrong
            continue;
        }
        eOcanmap_location_t *loc = &des->location; 
        // now .... i retrieve the board.
        eOcanmap_board_extended_t *boardext = s_eo_canmap_singleton.canmapping[loc->port][loc->addr];
        if(NULL == boardext)
        {
            continue;
        }
        // it must not be NULL .... but for now i dont check it
        // now i must do:
        // 1. set the boardext with the index of the entity.
        // 2. fill the entity array        
        if(eoprot_endpoint_motioncontrol == ep)
        {   // for motion control the board can be 1foc or mc4. 1foc manages 1 joint only and is placed in position 0 of indexofentity[]. mc4 manages two joints which are placed in position 0 and position 1
            boardext->board.indexofentity[loc->insideindex] = entindexNONE;
            s_eo_canmap_singleton.entitylocation[ep][eoprot_entity_mc_joint][des->index] = NULL;
            s_eo_canmap_singleton.entitylocation[ep][eoprot_entity_mc_motor][des->index] = NULL;
        }   
        else if(eoprot_endpoint_skin == ep)
        {   // for skin endpoint the board is a mtb. for entity of type skin we use position 0 of indexofentity[]
            boardext->board.indexofentity[0] = entindexNONE;  // as described in NOTE-123454321 we use position 0    
            uint8_t pos = s_eo_canmap_singleton.numofskinboardsindex[des->index];
            s_eo_canmap_singleton.skinlocation[des->index][pos] = NULL; 
            s_eo_canmap_singleton.numofskinboardsindex[des->index] --;            
        }
        else if((eoprot_endpoint_analogsensors == ep) && (eoprot_entity_as_inertial == entity))
        {   // NOTE-123454321: for analog sensor endpoint and inertial entity the board is a mtb. we use position 1 of indexofentity[] because position 0 is reserved to skin entity managed by the same board
            boardext->board.indexofentity[1] = entindexNONE; // as described in NOTE-123454321 we use position 1
            s_eo_canmap_singleton.entitylocation[ep][entity][des->index] = NULL;            
        }
        else 
        {   // all other cases are managed by a board only which manages only one entity. we use position 0 of indexofentity[]
            boardext->board.indexofentity[0] = entindexNONE;
            s_eo_canmap_singleton.entitylocation[ep][entity][des->index] = NULL;
        }
    }
    
    return(eores_OK);     
}


extern const eOcanmap_board_extended_t * eo_canmap_GetBoard(EOtheCANmapping *p, eOcanmap_location_t bloc)
{
//    eOcanmap_board_extended_t *board = NULL;
    
// useless if port is only one bit    
//    if(loc.port > 1)
//    {
//        return(NULL);
//    } 
// useless if addr is only four bits    
//    if(loc.addr > 15)
//    {
//        return(NULL);
//    }
    
    return((const eOcanmap_board_extended_t*)s_eo_canmap_singleton.canmapping[bloc.port][bloc.addr]);
}


extern eObrd_cantype_t eo_canmap_GetBoardType(EOtheCANmapping *p, eOcanmap_location_t bloc)
{   
    const eOcanmap_board_extended_t *brd = eo_canmap_GetBoard(p, bloc);
    if(NULL == brd)
    {
        return(eobrd_cantype_unknown);
    }
    return((eObrd_cantype_t)brd->board.props.type);
}


extern eOresult_t eo_canmap_BoardSetDetected(EOtheCANmapping *p, eOcanmap_location_t bloc, eObrd_info_t *detected)
{
    
//      eOresult_t res = eores_NOK_generic;
    
// useless if port is only one bit    
//    if(loc.port > 1)
//    {
//        return(eores_NOK_generic);
//    } 
// useless if addr is only four bits        
//    if(loc.addr > 15)
//    {
//        return(eores_NOK_generic);
//    }
    
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

// we dont need the ep and entity to retrieve the index. 
extern eOprotIndex_t eo_canmap_GetEntityIndex(EOtheCANmapping *p, eOcanmap_location_t loc)
{
    eOprotIndex_t index = EOK_uint08dummy; // init with this value. it changed only if we find a valid index
    
// useless if port is only one bit    
//    if(loc.port > 1)
//    {
//        return(EOK_uint08dummy);
//    }
// useless if addr is only four bits       
//    if(loc.addr > 15)
//    {
//        return(EOK_uint08dummy);
//    }
    
    const eOcanmap_board_extended_t * theboard = s_eo_canmap_singleton.canmapping[loc.port][loc.addr];
    
    if(NULL == theboard)
    {   // it means that we dont have such a board at the specified location
        return(EOK_uint08dummy);
    }
    
    
    // ok, we have a board at that location. we just get the index given by loc.insideindex
    // we dont verify vs type of board and type of requested entity
    
    if(eocanmap_insideindex_first == loc.insideindex)
    {
        index = theboard->board.indexofentity[0];
    }
    else if(eocanmap_insideindex_second == loc.insideindex)
    {
        index = theboard->board.indexofentity[1];
    }
    else if(eocanmap_insideindex_none == loc.insideindex)
    {
        index = theboard->board.indexofentity[0];
    }
        
    if(entindexNONE == index)
    {   // in case some board.indexofentity[0] or board.indexofentity[1] has a entindexNONE value we must be sure to return EOK_uint08dummy
        index = EOK_uint08dummy;
    }
    
    return(index);            
}

// but if we use ep and entity we surely have cross-check  
extern eOprotIndex_t eo_canmap_GetEntityIndexExtraCheck(EOtheCANmapping *p, eOcanmap_location_t loc, eOprotEndpoint_t ep, eOprotEntity_t entity)
{
    eOprotIndex_t index = EOK_uint08dummy; // init with this value. it changed only if we find a valid index
    
 
// useless if port is only one bit    
//    if(loc.port > 1)
//    {
//        return(EOK_uint08dummy);
//    }
// useless if addr is only four bits        
//    if(loc.addr > 15)
//    {
//        return(EOK_uint08dummy);
//    }
    
    const eOcanmap_board_extended_t * theboard = s_eo_canmap_singleton.canmapping[loc.port][loc.addr];
    
    if(NULL == theboard)
    {   // it means that we dont have such a board at the specified location
        return(EOK_uint08dummy);
    }
    
    // ok, we have a board at that location. is it compatible with endpoint and entity?

    switch(ep)
    {
        case eoprot_endpoint_motioncontrol:
        {   // on can boards there are either joints or motors
            if((eoprot_entity_mc_joint == entity) || (eoprot_entity_mc_motor == entity))
            {   // get the jomoindex. it depends upon the board being mc4 or 1foc
                if(theboard->board.props.type == eobrd_cantype_1foc) 
                {   // if 1foc it is always on board.indexofentity[0], irrespectively of the value of loc.insideindex 
                    index = theboard->board.indexofentity[0];
                }
                else if(theboard->board.props.type == eobrd_cantype_mc4)
                {   // if mc4, index depends on value of loc.insideindex. it cn be only first or second. if loc.insideindex is none or even else: nothing is done
                    if(eocanmap_insideindex_first == loc.insideindex)
                    {
                        index = theboard->board.indexofentity[0];
                    }
                    else if(eocanmap_insideindex_second == loc.insideindex)
                    {
                        index = theboard->board.indexofentity[1];
                    }
                }                
            }
            
        } break;
        
        
        case eoprot_endpoint_analogsensors:
        {   // strain or mais or inertial
            if(eoprot_entity_as_strain == entity)
            {   // the board can be only a strain ... the index is always on board.indexofentity[0]
                if(theboard->board.props.type == eobrd_cantype_strain) 
                {
                    index = theboard->board.indexofentity[0];
                }
            }
            else if(eoprot_entity_as_mais == entity)
            {   // the board can be only a mais ... the index is always on board.indexofentity[0]
                if(theboard->board.props.type == eobrd_cantype_mais) 
                {
                    index = theboard->board.indexofentity[0];
                }
            } 
            else if(eoprot_entity_as_inertial == entity)
            {   // the board can be only a skin ... the index is always on board.indexofentity[1] !!!!!! SEE NOTE-123454321 in FUNCTION eo_canmap_ConfigEntity()
                if(theboard->board.props.type == eobrd_cantype_skin) 
                {
                    index = theboard->board.indexofentity[1]; // as described in NOTE-123454321 we use position 1
                }
            } 
        } break;
 

        case eoprot_endpoint_skin:
        {   // only a skin
            if(eoprot_entity_sk_skin == entity)
            {   // the board can be only a skin board ... the index is on board.indexofentity[0]
                if(theboard->board.props.type == eobrd_cantype_skin) 
                {
                    index = theboard->board.indexofentity[0]; // as described in NOTE-123454321 we use position 0
                }
            }
        } break;        
 
        
        case eoprot_endpoint_management:
        default:
        {   
            index = EOK_uint08dummy;
        } break;
        
    }
    
    if(entindexNONE == index)
    {   // in case some board.indexofentity[0] or board.indexofentity[1] has a entindexNONE value we must be sure to return EOK_uint08dummy
        index = EOK_uint08dummy;
    }
    
    return(index);
}

extern eOresult_t eo_canmap_GetEntityLocation(EOtheCANmapping *p, eOprotID32_t id32, eOcanmap_location_t *loc, uint8_t *numoflocs, eObrd_cantype_t *boardtype)
{
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id32);
    eOprotEntity_t entity = eoprot_ID2entity(id32);
    eOprotIndex_t index = eoprot_ID2index(id32);
    
    eOresult_t res = eores_NOK_generic;
    const eOcanmap_board_extended_t *theboard = NULL;
        
    if(NULL == loc)
    {   
        return(res);
    }  
    
        
    // now we retrieve the pointer to board specified inside the tables.
    // in here we can also verify if the board type is coherent with the asked endpoint-entity
    // and we can also retrieve address.
    switch(ep)
    {
        case eoprot_endpoint_motioncontrol:
        {
            if(eoprot_entity_mc_joint == entity)
            {
                if(index < eocanmap_joints_maxnumberof)
                {
                    theboard = s_eo_canmap_singleton.entitylocation[ep][entity][index];
                }                
            }
            else if(eoprot_entity_mc_motor == entity)
            {
                if(index < eocanmap_motors_maxnumberof)
                {
                    theboard = s_eo_canmap_singleton.entitylocation[ep][entity][index];
                }                
            }

            // verification for motion control is equal for joint or motor
            if(NULL != theboard)
            {   // ok, we have a board. now see if it is a 2foc or 4dc
                if((eobrd_cantype_1foc == theboard->board.props.type) || (eobrd_cantype_mc4 == theboard->board.props.type))
                {   // ok, correct board type: we retrieve the info
                    if(NULL != numoflocs)
                    {
                        *numoflocs = 1;
                    }
                    loc->port = theboard->board.props.location.port;
                    loc->addr = theboard->board.props.location.addr; 
                    if(index == theboard->board.indexofentity[0])
                    {
                        loc->insideindex = eocanmap_insideindex_first; // for 1foc we could use eocanmap_insideindex_none, but it is ok even in this way
                        res = eores_OK;
                    }
                    else if(index == theboard->board.indexofentity[1])
                    {
                        loc->insideindex = eocanmap_insideindex_second;
                        res = eores_OK;
                    } 
                    //else
                    //{   // the board is correct but none of the inside indices matches the target index. cannot give ok result                        
                    //}
                    if(NULL != boardtype)
                    {
                        *boardtype = (eObrd_cantype_t)theboard->board.props.type;
                    }
                }               
            }
        } break;
        

        case eoprot_endpoint_analogsensors:
        {
            if(eoprot_entity_as_strain == entity)
            {
                if(index < eocanmap_strains_maxnumberof)
                {
                    theboard = s_eo_canmap_singleton.entitylocation[ep][entity][index];
                    if(NULL != theboard)
                    {
                        if(eobrd_cantype_strain == theboard->board.props.type)
                        {   // ok, correct board. we retrieve the info
                            if(NULL != numoflocs)
                            {
                                *numoflocs = 1;
                            }
                            loc->port = theboard->board.props.location.port;
                            loc->addr = theboard->board.props.location.addr;             
                            if(index == theboard->board.indexofentity[0])
                            {
                                loc->insideindex = eocanmap_insideindex_none; // if it is a strain we dont care about the insideindex
                                res = eores_OK;
                            }
                            //else
                            //{   // the board is correct but the insideindex0 does not match the target index. cannot give ok result                        
                            //} 
                            if(NULL != boardtype)
                            {
                                *boardtype = (eObrd_cantype_t)theboard->board.props.type;
                            }
                        }
                    }
                }                
            }
            else if(eoprot_entity_as_mais == entity)
            {
                if(index < eocanmap_maises_maxnumberof)
                {
                    theboard = s_eo_canmap_singleton.entitylocation[ep][entity][index];
                    if(NULL != theboard)
                    {
                        if(eobrd_cantype_mais == theboard->board.props.type)
                        {   // ok, correct board. we retrieve the info
                            if(NULL != numoflocs)
                            {
                                *numoflocs = 1;
                            }
                            loc->port = theboard->board.props.location.port;
                            loc->addr = theboard->board.props.location.addr;             
                            if(index == theboard->board.indexofentity[0])
                            {
                                loc->insideindex = eocanmap_insideindex_none; // if it is a mais we dont care about the inside index
                                res = eores_OK;
                            }
                            //else
                            //{   // the board is correct but the insideindex0 does not match the target index. cannot give ok result                        
                            //}  
                            if(NULL != boardtype)
                            {
                                *boardtype = (eObrd_cantype_t)theboard->board.props.type;
                            }                            
                        }
                    }                    
                }                
            }
            else if(eoprot_entity_as_inertial == entity)
            {
                if(index < eocanmap_inertials_maxnumberof)
                {
                    theboard = s_eo_canmap_singleton.entitylocation[ep][entity][index];
                    if(NULL != theboard)
                    {
                        if(eobrd_cantype_skin == theboard->board.props.type)
                        {   // ok, correct board. we retrieve the info
                            if(NULL != numoflocs)
                            {
                                *numoflocs = 1;
                            }
                            loc->port = theboard->board.props.location.port;
                            loc->addr = theboard->board.props.location.addr;             
                            if(index == theboard->board.indexofentity[1]) // as described in NOTE-123454321 we use position 1
                            {
                                loc->insideindex = eocanmap_insideindex_none; // if it is a skin board we dont care about the inside index
                                res = eores_OK;
                            }
                            //else
                            //{   // the board is correct but the insideindex0 does not match the target index. cannot give ok result                        
                            //}  
                            if(NULL != boardtype)
                            {
                                *boardtype = (eObrd_cantype_t)theboard->board.props.type;
                            }                            
                        }
                    }                    
                }                
            }             
        } break;  
        
        case eoprot_endpoint_skin:
        {
            if(eoprot_entity_sk_skin == entity)
            {
                if(index < eocanmap_skins_maxnumberof)
                {
                    
                    eOcanmap_board_extended_t ** theboards = (eOcanmap_board_extended_t **)s_eo_canmap_singleton.skinlocation[index];
                    //const eOcanmap_board_extended_t * const * theboards = eo_canmapcfg_skins_boards[index];
                    //eOcanmap_board_extended_t ** theboards = (eOcanmap_board_extended_t **)s_eo_canmap_singleton.entitylocation[ep][entity][index];
                    if(NULL != theboards)
                    {   // ok, we have a valid array of boards. let us see the first one ...
                        theboard = theboards[0];
                        if(NULL != theboard)
                        {   // must verify it is non NULL ... and that it is a skin board
                            if(eobrd_cantype_skin == theboard->board.props.type)
                            {   // ok, correct board. we retrieve the info
                                if(NULL != numoflocs)
                                {
                                    *numoflocs = s_eo_canmap_singleton.numofskinboardsindex[index];
                                }
                                loc->port = theboard->board.props.location.port;
                                loc->addr = theboard->board.props.location.addr; 
                                loc->insideindex = eocanmap_insideindex_none; // if it is a skin we dont care about insideindex
                                res = eores_OK;
                                if(NULL != boardtype)
                                {
                                    *boardtype = (eObrd_cantype_t)theboard->board.props.type;
                                }
                            }
                        }
                    }
                }                
            }
           
        } break;  
        
        default:
        {
        } break;
        
    }
       
    
    // returns ok or nok depeding on what the function has found
    return(res);
}
   

//extern EOconstarray* eo_canmap_GetBoardLocations(EOtheCANmapping *p)
//{
//    if(NULL == p)
//    {
//        return(NULL);
//    }
//    
//    return((EOconstarray*)&s_eo_canmap_singleton.arrayofboardlocations);   
//}


///**	@typedef    typedef struct eOcanmap_compact_address_list_t 
// 	@brief      Contains a compact address list of up to 16 can boards, each using 4 bits. 
// **/
//typedef struct
//{   // or rather, the other way round in arm ... thus maybe better saying that the adresses are organised in nibbles
//    uint64_t    b00 : 4;
//    uint64_t    b01 : 4;
//    uint64_t    b02 : 4;
//    uint64_t    b03 : 4;
//    uint64_t    b04 : 4;
//    uint64_t    b05 : 4;
//    uint64_t    b06 : 4;
//    uint64_t    b07 : 4;
//    uint64_t    b08 : 4;
//    uint64_t    b09 : 4;
//    uint64_t    b10 : 4;
//    uint64_t    b11 : 4;
//    uint64_t    b12 : 4;
//    uint64_t    b13 : 4;
//    uint64_t    b14 : 4;
//    uint64_t    b15 : 4;
//} eOcanmap_compact_address_list_t;  EO_VERIFYsizeof(eOcanmap_compact_address_list_t, 8); 
//
//extern eOresult_t eo_canmap_GetCompactAddressList(EOtheCANmapping *p, eOcanport_t port, eOcanmap_compact_address_list_t *addresslist, uint8_t *numofboards)
//{
//
//    if(NULL == addresslist)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    eOcanmap_board_extended_t * const * theboards = s_eo_canmap_singleton.canmapping[port];
//    
//    uint8_t i = 0;
//    uint8_t num = 0;
//    uint64_t list = 0;
//    
//    for(i=0; i<15; i++)
//    {
//        eOcanmap_board_extended_t * board = theboards[i];
//        if(NULL == board)
//        {
//            continue;
//        }
//        uint64_t tmp = (board->board.props.location.addr) << (4*num);
//        num++;
//        list = list | tmp;      
//    }
//        
//    // ok, now i copy into the param
//    memcpy(addresslist, &list, 8);
//    
//    if(NULL != numofboards)
//    {
//        *numofboards = num;
//    }
//    
//    return(eores_OK);
//}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_eocanmap_is_entity_supported(eOprotEndpoint_t ep, eOprotEntity_t entity)
{
    eObool_t ret = eobool_false;
    
    switch(ep)
    {
        case eoprot_endpoint_motioncontrol:
        {
            if((eoprot_entity_mc_joint == entity) || (eoprot_entity_mc_motor == entity))
            {
                ret = eobool_true;
            }    
        } break; 
        
        case eoprot_endpoint_analogsensors:
        {
            if((eoprot_entity_as_strain == entity) || (eoprot_entity_as_mais == entity) || (eoprot_entity_as_inertial == entity))
            {
                ret = eobool_true;
            }                         
        } break;
        
        case eoprot_endpoint_skin:
        {
            if(eoprot_entity_sk_skin == entity)
            {
                ret = eobool_true;
            }                         
        } break;
        
        default:
        {
            ret = eobool_false;   
        } break;       
    }  

    return(ret);
}

static uint8_t s_eo_canmap_max_entities(eOprotEndpoint_t ep, eOprotEntity_t entity)
{
    uint8_t max = 0;
    switch(ep)
    {
        case eoprot_endpoint_motioncontrol:
        {
            if((eoprot_entity_mc_joint == entity) || (eoprot_entity_mc_motor == entity))
            {
                max = eocanmap_joints_maxnumberof;
            }    
        } break; 
        
        case eoprot_endpoint_analogsensors:
        {
            if(eoprot_entity_as_strain == entity)
            {
                max = eocanmap_strains_maxnumberof;
            } 
            else if(eoprot_entity_as_mais == entity)
            {
                max = eocanmap_maises_maxnumberof;
            }   
            else if(eoprot_entity_as_inertial == entity)
            {
                max = eocanmap_inertials_maxnumberof;
            }             
        } break;
        
        case eoprot_endpoint_skin:
        {
            if(eoprot_entity_sk_skin == entity)
            {
                max = eocanmap_skins_maxnumberof;
            }                         
        } break;
        
        default:
        {
            max = 0;   
        } break;       
    }  
    
    return(max);
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





