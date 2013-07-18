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

static uint16_t s_eoprot_mc_brdentityindex2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity, eOprotIndex_t index);
static uint16_t s_eoprot_mc_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id);

static uint16_t s_eoprot_mc_joints_numberof_Get(eOprotBRD_t brd);
static uint16_t s_eoprot_mc_motors_numberof_Get(eOprotBRD_t brd);
//static uint16_t s_eoprot_mc_controllers_numberof_Get(eOprotBRD_t brd);

static eObool_t s_eoprot_mc_tag_joint_is_valid(eOprotTag_t tag);
static eObool_t s_eoprot_mc_tag_motor_is_valid(eOprotTag_t tag);
static eObool_t s_eoprot_mc_tag_controller_is_valid(eOprotTag_t tag);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

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
    
    //#warning --> it is valid only if organisation of memory is jo[n], mo[m], c[l]
    size = sizeof(eOmc_joint_t) *      s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_joint] +
           sizeof(eOmc_motor_t) *      s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_motor] +
           sizeof(eOmc_controller_t) * s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_controller];
    
    return(size);
}

extern eOresult_t eoprot_mc_config_endpoint_ram(eOprotBRD_t brd, void *ram)
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
    switch(ent)
    {
        case eomc_entity_joint:
        {  
            sizeofentity = sizeof(eOmc_joint_t); 
        } break;
        
        case eomc_entity_motor:
        {  
            sizeofentity = sizeof(eOmc_motor_t);  
        } break;      
 
        case eomc_entity_controller:
        {  
            sizeofentity = sizeof(eOmc_controller_t);  
        } break;     
        
        default:
        {   // error
            sizeofentity = EOK_uint16dummy;
        } break;    
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
 
    switch(ent)
    {    
        case eomc_entity_joint: 
        case eomc_entity_motor:
        case eomc_entity_controller:
        {
            numberof = s_eoprot_mc_board_numberofeachentity[brd][ent];
        } break;
                     
        default:
        {   
            numberof = 0;
        } break; 
    }        
    
    return(numberof);
}




// --- 


extern uint16_t eoprot_mc_numberofvariables_get(eOprotBRD_t brd)
{
    uint16_t num = 0;
    
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(0);
    }
    
    num = eoprot_tags_mc_joint_numberof * s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_joint] +
          eoprot_tags_mc_motor_numberof * s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_motor] +
          eoprot_tags_mc_controller_numberof * s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_controller];
          
    return(num);
}


extern eObool_t eoprot_mc_id_isvalid(eOprotBRD_t brd, eOnvID32_t id)
{
    eObool_t ret = eobool_false;    
    
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(eobool_false);
    }    
    
    eOprotEntity_t ent = eoprot_ID2entity(id);
    eOprotIndex_t  ind = eoprot_ID2index(id);
    eOprotTag_t    tag = eoprot_ID2tag(id);
    
    switch(ent)
    {
        case eomc_entity_joint:
        {   
            if(ind < s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_joint])
            {
                ret = s_eoprot_mc_tag_joint_is_valid(tag);
            }            
        } break;
        
        case eomc_entity_motor:
        {
            if(ind < s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_motor]) 
            {
                ret = s_eoprot_mc_tag_motor_is_valid(tag);
            }            
        } break;        
 
        case eomc_entity_controller:
        {
            if(ind < s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_controller]) 
            {
                ret = s_eoprot_mc_tag_controller_is_valid(tag);
            }            
        } break; 
        
        default:
        {           
        } break;        
    }
    
    return(ret);     
}


extern eOprotID32_t eoprot_mc_prognum2id(eOprotBRD_t brd, eOprotProgNumber_t prog)
{
    eOprotTag_t tag = 0xff;
    eOprotIndex_t index = 0xff;
    eOprotEntity_t entity = 0xff;
    
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }
    
    eOprotProgNumber_t progsinjoints = eoprot_tags_mc_joint_numberof * s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_joint];
    eOprotProgNumber_t progsinmotors = eoprot_tags_mc_motor_numberof * s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_motor];
    eOprotProgNumber_t progsincontrollers = eoprot_tags_mc_controller_numberof * s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_controller];
    
    if((0 != progsinjoints) && (prog < (progsinjoints)))
    {   // entity is eomc_entity_joint 
        entity  = eomc_entity_joint;
        index   = prog / eoprot_tags_mc_joint_numberof;       // eoprot_tags_mc_joint_numberof cannot be zero if progsinjoints is non-zero
        tag     = prog % eoprot_tags_mc_joint_numberof;        
    }
    else if((0 != progsinmotors) && (prog < (progsinjoints + progsinmotors)))
    {   // entity is eomc_entity_motor 
        prog -= (progsinjoints);
        entity  = eomc_entity_motor;
        index   = prog / eoprot_tags_mc_motor_numberof;       // eoprot_tags_mc_motor_numberof cannot be zero if progsinmotors is non-zero
        tag     = prog % eoprot_tags_mc_motor_numberof;                      
    }
    else if((0 != progsincontrollers) && (prog < (progsinjoints + progsinmotors + progsincontrollers)))
    {   // entity is eomc_entity_controller 
        prog -= (progsinjoints+progsinmotors);
        entity  = eomc_entity_controller;
        index   = prog / eoprot_tags_mc_controller_numberof;       // eoprot_tags_mc_controller_numberof cannot be zero if progsincontrollers is non-zero
        tag     = prog % eoprot_tags_mc_controller_numberof;                      
    }    
    else
    {
        return(EOK_uint32dummy);
    }
    
    return(eoprot_ID_get(eoprot_endpoint_motioncontrol, entity, index, tag));
}

extern eOprotProgNumber_t eoprot_mc_id2prognum(eOprotBRD_t brd, eOprotID32_t id)
{
    eOprotProgNumber_t prog = EOK_uint32dummy;
    
    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
    {
        return(EOK_uint32dummy);
    }
    
    eOprotEntity_t entity = eoprot_ID2entity(id);
    eOprotIndex_t index = eoprot_ID2index(id);
    switch(entity)
    {
        case eomc_entity_joint: 
        {
            prog = index*eoprot_tags_mc_joint_numberof + eoprot_mc_rom_get_prognum(id);
        } break;
        
        case eomc_entity_motor: 
        {
            prog = s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_joint]*eoprot_tags_mc_joint_numberof +
                   index*eoprot_tags_mc_motor_numberof + eoprot_mc_rom_get_prognum(id);
        } break; 
        
        case eomc_entity_controller: 
        {
            prog = s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_joint]*eoprot_tags_mc_joint_numberof +
                   s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_motor]*eoprot_tags_mc_motor_numberof +
                   index*eoprot_tags_mc_controller_numberof + eoprot_mc_rom_get_prognum(id);
        } break;   
        
        default:
        {   
            prog = EOK_uint32dummy;
        } break;    
    }
    
    return(prog);
}


extern void* eoprot_mc_eonvrom_get(/*eOprotBRD_t brd,*/ eOprotID32_t id)
{
    //brd =  brd;
    return(eoprot_mc_rom_get_nvrom(id));
}


extern void* eoprot_mc_ramofvariable_get(eOprotBRD_t brd, eOprotID32_t id)
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

extern void* eoprot_mc_ramofvariable_extract(eOprotBRD_t brd, eOprotID32_t id, void* epram)
{
    uint8_t* startofdata = epram;
    uint16_t offset = s_eoprot_mc_brdid2ramoffset(brd, id);
    
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

static uint16_t s_eoprot_mc_joints_numberof_Get(eOprotBRD_t brd)
{
//    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
//    {
//        return(0);
//    }
    
    return(s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_joint]);
}

static uint16_t s_eoprot_mc_motors_numberof_Get(eOprotBRD_t brd)
{
//    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
//    {
//        return(0);
 //   }
    
    return(s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_motor]);
}

//static uint16_t s_eoprot_mc_controllers_numberof_Get(eOprotBRD_t brd)
//{
////    if((NULL == s_eoprot_mc_board_numberofeachentity) || (NULL == s_eoprot_mc_board_numberofeachentity[brd]))
////    {
////        return(0);
////    }
//    
//    return(s_eoprot_mc_board_numberofeachentity[brd][eomc_entity_controller]);
//}

static uint16_t s_eoprot_mc_brdentityindex2ramoffset(eOprotBRD_t brd, eOprotEntity_t entity, eOprotIndex_t index)
{
    uint16_t offset = 0;
    
    switch(entity)
    {
        case eomc_entity_joint:
        {   // the joints are all displaced at the beginning of the data in every mc endpoint
            offset = 0;
            offset += sizeof(eOmc_joint_t)*index; 
        } break;
        
        case eomc_entity_motor:
        {   // the motors are placed after all the joints. 
            uint16_t numberofjoints = s_eoprot_mc_joints_numberof_Get(brd);
            offset = numberofjoints*sizeof(eOmc_joint_t);  
            offset += sizeof(eOmc_motor_t)*index;
        } break;      

        case eomc_entity_controller:
        {   // the controllers are placed after all the joints and all the motors. 
            uint16_t numberofjoints = s_eoprot_mc_joints_numberof_Get(brd);
            uint16_t numberofmotors = s_eoprot_mc_motors_numberof_Get(brd);
            offset = numberofjoints*sizeof(eOmc_joint_t) + numberofmotors*sizeof(eOmc_motor_t);  
            offset += sizeof(eOmc_controller_t)*index;
        } break;      
                
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;    
    }
    
    return(offset);  
}



static uint16_t s_eoprot_mc_brdid2ramoffset(eOprotBRD_t brd, eOprotID32_t id)
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
        case eomc_entity_joint:
        {   // the joints are all displaced at the beginning of the data in every mc endpoint
            offset = index*sizeof(eOmc_joint_t) + eoprot_mc_rom_joint_get_offset(tag); 
        } break;
        
        case eomc_entity_motor:
        {   // the motors are placed after all the joints. the index of the joints depends on the board
            uint16_t numberofjoints = s_eoprot_mc_joints_numberof_Get(brd);
            offset = numberofjoints*sizeof(eOmc_joint_t) + index*sizeof(eOmc_motor_t) + eoprot_mc_rom_motor_get_offset(tag);  
        } break;      

        case eomc_entity_controller:
        {   // the controller is placed after all joints and motors. their index depends on the board
            uint16_t numberofjoints = s_eoprot_mc_joints_numberof_Get(brd);
            uint16_t numberofmotors = s_eoprot_mc_motors_numberof_Get(brd);
            offset = numberofjoints*sizeof(eOmc_joint_t) + numberofmotors*sizeof(eOmc_motor_t) + index*sizeof(eOmc_controller_t) + eoprot_mc_rom_controller_get_offset(tag);
        } break; 
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;
    
    }
    
    return(offset);  
}


static eObool_t s_eoprot_mc_tag_joint_is_valid(eOprotTag_t tag)
{   // in case of holes in tags ... change the code
    return((tag < eoprot_tags_mc_joint_numberof)?(eobool_true):(eobool_false));
}

static eObool_t s_eoprot_mc_tag_motor_is_valid(eOprotTag_t tag)
{   // in case of holes in tags ... change the code  
    return((tag < eoprot_tags_mc_motor_numberof)?(eobool_true):(eobool_false));
}

static eObool_t s_eoprot_mc_tag_controller_is_valid(eOprotTag_t tag)
{   // in case of holes in tags ... change the code 
    return((tag < eoprot_tags_mc_controller_numberof)?(eobool_true):(eobool_false));
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



