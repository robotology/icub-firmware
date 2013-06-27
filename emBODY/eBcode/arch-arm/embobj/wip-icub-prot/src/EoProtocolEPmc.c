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

/* @file       EoProtocolEPmc.c
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

#include "EoProtocolEPmc_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPmc.h"


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
EO_VERIFYproposition(eoprot_ep_mc_dew4, eoprot_ep_mc_entities_numberof <= eoprot_entities_numberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_ep_mc_tagsnum_jo, eoprot_ep_mc_joint_tags_numberof == eoprot_ep_mc_joint_rwmodes_numberof);
EO_VERIFYproposition(eoprot_ep_mc_tagsmax_jo, eoprot_ep_mc_joint_tags_numberof <= eoprot_tags_numberof);

EO_VERIFYproposition(eoprot_ep_mc_tagsnum_mo, eoprot_ep_mc_motor_tags_numberof == eoprot_ep_mc_motor_rwmodes_numberof);
EO_VERIFYproposition(eoprot_ep_mc_tagsmax_mo, eoprot_ep_mc_motor_tags_numberof <= eoprot_tags_numberof);

EO_VERIFYproposition(eoprot_ep_mc_tagsnum_co, eoprot_ep_mc_controller_tags_numberof == eoprot_ep_mc_controller_rwmodes_numberof);
EO_VERIFYproposition(eoprot_ep_mc_tagsmax_co, eoprot_ep_mc_controller_tags_numberof <= eoprot_tags_numberof);
  

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_mc_brdid2ramoffset(eOprotBRD_t brd, eOprotID_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if 0   // dynamic mode
static const uint8_t** s_eoprot_ep_mc_board_numberofeachentity = NULL; // s_eoprot_ep_mc_board_numberofeachentity[brd][entype] tells how many entities of kind entype are in brd
static uint16_t s_eoprot_ep_mc_board_numberofthem = 0;
#endif
static const uint8_t* s_eoprot_ep_mc_board_numberofeachentity[EOCFG_EPS_BOARDS_MAXNUMBER] = { NULL };
static const uint16_t s_eoprot_ep_mc_board_numberofthem = EOCFG_EPS_BOARDS_MAXNUMBER;


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#if 0   // dynamic mode
extern eOresult_t eoprot_ep_mc_number_of_boards_Load(uint16_t numofboards)
{
    if((0 != s_eoprot_ep_mc_board_numberofthem) || (NULL != s_eoprot_ep_mc_board_numberofeachentity))
    {
        return(eores_NOK_generic);
    }
    
    s_eoprot_ep_mc_board_numberofthem = numofboards;
    s_eoprot_ep_mc_board_numberofeachentity = (const uint8_t**) calloc(numofboards, sizeof(uint8_t*));
    
    return(eores_OK);   
}
#endif

extern eOresult_t eoprot_ep_mc_number_of_entities_Load(eOprotBRD_t brd, const uint8_t* numberofeachentity)
{
    if(NULL == s_eoprot_ep_mc_board_numberofeachentity)
    {
        return(eores_NOK_generic);
    }
    
    if(brd > s_eoprot_ep_mc_board_numberofthem)
    {
        return(eores_NOK_generic);
    }
        
    s_eoprot_ep_mc_board_numberofeachentity[brd] = numberofeachentity;
    
    return(eores_OK);       
}


extern uint16_t eoprot_ep_mc_variables_numberof_Get(eOprotBRD_t brd)
{
    uint16_t num = 0;
    
    if((NULL == s_eoprot_ep_mc_board_numberofeachentity) || (NULL == s_eoprot_ep_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    num = eoprot_ep_mc_joint_tags_numberof * s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_joint] +
          eoprot_ep_mc_motor_tags_numberof * s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_motor] +
          eoprot_ep_mc_controller_tags_numberof * s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_controller];
          
    return(num);
}

extern eOprotID_t eoprot_ep_mc_variable_idfromprognumber_Get(eOprotBRD_t brd, uint16_t prog)
{
    eOprotTag_t tag = 0xffff;
    eOprotIndex_t index = 0xff;
    eOprotEntity_t entity = 0xff;
    
    if((NULL == s_eoprot_ep_mc_board_numberofeachentity) || (NULL == s_eoprot_ep_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    uint16_t progsinjoints = eoprot_ep_mc_joint_tags_numberof * s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_joint];
    uint16_t progsinmotors = eoprot_ep_mc_motor_tags_numberof * s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_motor];
    uint16_t progsincontrollers = eoprot_ep_mc_controller_tags_numberof * s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_controller];
    
    if((0 != progsinjoints) && (prog < (progsinjoints)))
    {   // entity is eomc_entity_joint 
        entity  = eomc_entity_joint;
        index   = prog / eoprot_ep_mc_joint_tags_numberof;       // eoprot_ep_mc_joint_tags_numberof cannot be zero if progsinjoints is non-zero
        tag     = prog % eoprot_ep_mc_joint_tags_numberof;        
    }
    else if((0 != progsinmotors) && (prog < (progsinjoints + progsinmotors)))
    {   // entity is eomc_entity_motor 
        prog -= (progsinjoints);
        entity  = eomc_entity_motor;
        index   = prog / eoprot_ep_mc_motor_tags_numberof;       // eoprot_ep_mc_motor_tags_numberof cannot be zero if progsinmotors is non-zero
        tag     = prog % eoprot_ep_mc_motor_tags_numberof;                      
    }
    else if((0 != progsincontrollers) && (prog < (progsinjoints + progsinmotors + progsincontrollers)))
    {   // entity is eomc_entity_controller 
        prog -= (progsinjoints+progsinmotors);
        entity  = eomc_entity_controller;
        index   = prog / eoprot_ep_mc_controller_tags_numberof;       // eoprot_ep_mc_controller_tags_numberof cannot be zero if progsincontrollers is non-zero
        tag     = prog % eoprot_ep_mc_controller_tags_numberof;                      
    }    
    else
    {
        return(EOK_uint16dummy);
    }
    
    return(eoprot_ep_variable_ID_get(eoprot_endpoint_motioncontrol, entity, index, tag));
    
}


extern uint16_t eoprot_ep_mc_variable_progressivenumber_Get(eOprotBRD_t brd, eOprotID_t id)
{
    uint16_t prog = 0;
    
    if((NULL == s_eoprot_ep_mc_board_numberofeachentity) || (NULL == s_eoprot_ep_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_motioncontrol, id);
    eOprotIndex_t index = eoprot_ep_variable_ID2index(eoprot_endpoint_motioncontrol, id);
    switch(entity)
    {
        case eomc_entity_joint: 
        {
            prog = index*eoprot_ep_mc_joint_tags_numberof + eoprot_ep_mc_rom_get_prognum(id);
        } break;
        
        case eomc_entity_motor: 
        {
            prog = s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_joint]*eoprot_ep_mc_joint_tags_numberof +
                   index*eoprot_ep_mc_motor_tags_numberof + eoprot_ep_mc_rom_get_prognum(id);
        } break; 
        
        case eomc_entity_controller: 
        {
            prog = s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_joint]*eoprot_ep_mc_joint_tags_numberof +
                   s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_motor]*eoprot_ep_mc_motor_tags_numberof +
                   index*eoprot_ep_mc_controller_tags_numberof + eoprot_ep_mc_rom_get_prognum(id);
        } break;   
        
        default:
        {   
            prog = 0;
        } break;    
    }
    
    return(prog);
}

extern uint16_t eoprot_ep_mc_joints_numberof_Get(eOprotBRD_t brd)
{
    if((NULL == s_eoprot_ep_mc_board_numberofeachentity) || (NULL == s_eoprot_ep_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    return(s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_joint]);
}

extern uint16_t eoprot_ep_mc_motors_numberof_Get(eOprotBRD_t brd)
{
    if((NULL == s_eoprot_ep_mc_board_numberofeachentity) || (NULL == s_eoprot_ep_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    return(s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_motor]);
}

extern uint16_t eoprot_ep_mc_controllers_numberof_Get(eOprotBRD_t brd)
{
    if((NULL == s_eoprot_ep_mc_board_numberofeachentity) || (NULL == s_eoprot_ep_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    return(s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_controller]);
}

extern uint16_t eoprot_ep_mc_ram_sizeof_Get(eOprotBRD_t brd)
{
    uint16_t size = 0;
    
    if((NULL == s_eoprot_ep_mc_board_numberofeachentity) || (NULL == s_eoprot_ep_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    //#warning --> it is valid only if organisation of memory is jo[n], mo[m], c[l]
    size = sizeof(eOmc_joint_t) *      s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_joint] +
           sizeof(eOmc_motor_t) *      s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_motor] +
           sizeof(eOmc_controller_t) * s_eoprot_ep_mc_board_numberofeachentity[brd][eomc_entity_controller];
    
    return(size);
}

extern uint16_t eoprot_ep_mc_variable_ram_sizeof_Get(eOprotID_t id)
{
    return(eoprot_ep_mc_rom_get_sizeofvar(id));
}


extern void* eoprot_ep_mc_variable_ram_Extract(void* epram, eOprotBRD_t brd, eOprotID_t id)
{
    uint8_t* startofdata = epram;
    uint16_t offset = s_eoprot_ep_mc_brdid2ramoffset(brd, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
}    
    

extern void* eoprot_ep_mc_variable_rom_Get(eOprotID_t id)
{
    return(eoprot_ep_mc_rom_get_nvrom(id));
}

extern const eOmc_joint_t* eoprot_ep_mc_joint_default_Get(void)
{
    return(&eoprot_ep_mc_rom_joint_defaultvalue);
}

extern const eOmc_motor_t* eoprot_ep_mc_motor_default_Get(void)
{
    return(&eoprot_ep_mc_rom_motor_defaultvalue);
}

extern const eOmc_controller_t* eoprot_ep_mc_controller_default_Get(void)
{
    return(&eoprot_ep_mc_rom_controller_defaultvalue);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_mc_brdid2ramoffset(eOprotBRD_t brd, eOprotID_t id)
{
    uint16_t offset = 0;
    uint16_t tag = eoprot_ep_variable_ID2tag(eoprot_endpoint_motioncontrol, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    //#warning --> it is valid only if organisation of memory is jo[n], mo[m], c[l]
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_motioncontrol, id);
    uint16_t index = eoprot_ep_variable_ID2index(eoprot_endpoint_motioncontrol, id);
    
    switch(entity)
    {
        case eomc_entity_joint:
        {   // the joints are all displaced at the beginning of the data in every mc endpoint
            offset = index*sizeof(eOmc_joint_t) + eoprot_ep_mc_rom_joint_get_offset(tag); 
        } break;
        
        case eomc_entity_motor:
        {   // the motors are placed after all the joints. the index of the joints depends on the board
            uint16_t numberofjoints = eoprot_ep_mc_joints_numberof_Get(brd);
            offset = numberofjoints*sizeof(eOmc_joint_t) + index*sizeof(eOmc_motor_t) + eoprot_ep_mc_rom_motor_get_offset(tag);  
        } break;      

        case eomc_entity_controller:
        {   // the controller is placed after all joints and motors. their index depends on the board
            uint16_t numberofjoints = eoprot_ep_mc_joints_numberof_Get(brd);
            uint16_t numberofmotors = eoprot_ep_mc_motors_numberof_Get(brd);
            offset = numberofjoints*sizeof(eOmc_joint_t) + numberofmotors*sizeof(eOmc_motor_t) + index*sizeof(eOmc_controller_t) + eoprot_ep_mc_rom_controller_get_offset(tag);
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



