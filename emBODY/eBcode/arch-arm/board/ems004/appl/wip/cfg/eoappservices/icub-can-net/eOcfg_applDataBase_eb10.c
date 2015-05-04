/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

/* @file       eOcfg_emsCanNetworkTopology_eb10.c
    @brief      This file keeps ems EB10(only skin) can network configuration
    @author     valentina.gaggero@iit.it
    @date       03/12/2014
**/
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "EoCommon.h"
#include "EOconstvector_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EoappTheDataBase.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//board 

//static const eOappTheDB_canboardinfo_t s_cfg_appDB_boards[] = 
//{
//};

#warning --> marco.accame: add the skins addresses in can2 in here to manage messages from/to them


const EOconstvector  s_eo_cfg_appDB_constvec_boards = 
{
    .capacity       = 0,
    .size           = 0, 
    .item_size      = 0,
    .dummy          = 0,  
    .stored_items   = NULL,
    .functions      = NULL 
};
extern const EOconstvector* const eo_cfg_appDB_thecanboards = &s_eo_cfg_appDB_constvec_boards;




// - in here we have the mapping of the joints into a given can board and its internal index (0, 1). 
//   comments by marco.accame of 08jan2015:
//   the indexofcanboard member keeps the position of the can board inside constarray eo_cfg_appDB_thecanboards
//   and it gives (port, addr, type).
//   the indexinsidecanboard member is either 0 or 1 and is related to can protocol.
//   how to improve: 
//   1. the indexofcanboard can be assigned not with MC4_ADDRCAN_3_BID but with a refrence to teh constarray etc: maybe not
//   2. remove canLoc

//static const eOappTheDB_mapping2canboard_t s_cfg_appDB_joints[] = 
//{
//};


const EOconstvector  s_eo_cfg_appDB_constvec_joints = 
{
    .capacity       = 0,
    .size           = 0, 
    .item_size      = 0,
    .dummy          = 0,  
    .stored_items   = NULL,
    .functions      = NULL
};
extern const EOconstvector* const eo_cfg_appDB_thejoints_mapping2canboards = &s_eo_cfg_appDB_constvec_joints;




// - in here we have the mapping of the motors into a given can board and its internal index (0, 1). 
// - the same as joints
extern const EOconstvector* const eo_cfg_appDB_themotors_mapping2canboards = &s_eo_cfg_appDB_constvec_joints;




// - in here we have the mapping of the mais sensors into a given can board

//static const eOappTheDB_mapping2canboard_t  s_cfg_appDB_snsrMais[] = 
//{
//    {// 0
//        .indexofcanboard        = MAIS_ADDRCAN_14_BID,
//        .indexinsidecanboard    = eOappTheDB_canboard_noindex
//    }
//};


const EOconstvector  s_eo_cfg_appDB_constvec_snsrMais = 
{
    .capacity       = 0,
    .size           = 0, 
    .item_size      = 0,
    .dummy          = 0,  
    .stored_items   = NULL,
    .functions      = NULL
};
extern const EOconstvector* const eo_cfg_appDB_themaises_mapping2canboards = &s_eo_cfg_appDB_constvec_snsrMais;


// - in here we have the mapping of the strain sensors into a given can board


const EOconstvector  s_eo_cfg_appDB_constvec_snsrStrain = 
{   // empty
    .capacity       = 0,
    .size           = 0, 
    .item_size      = 0,
    .dummy          = 0,  
    .stored_items   = NULL,
    .functions      = NULL
};
extern const EOconstvector* const eo_cfg_appDB_thestrains_mapping2canboards = &s_eo_cfg_appDB_constvec_snsrStrain;



// - in here we have the mapping of the skin boards into a given can board
//   marco.accame: so far we havent managed can messages of individual skin boards, thus we only have a global config
//   with the skin bus and number of can boards ...
//   TODO: enhance it

//Note: since EMS doesn't manage skin, but it is only a gataway between skin boards and pc104, 
//the configuration says tahe skin is connected.
static const eOappTheDB_cfg_skinInfo_t  s_cfg_appDB_skin [] = 
{
    {// 0
        EO_INIT(.connected2emsport)                       eOcanport1,
        EO_INIT(.boardAddrStart)                          1,
        EO_INIT(.numofboards)                             7
    },
    //1
    {
        EO_INIT(.connected2emsport)                       eOcanport2,
        EO_INIT(.boardAddrStart)                          8,
        EO_INIT(.numofboards)                             6
    }
};


const EOconstvector  s_eo_cfg_appDB_constvec_skin = 
{
    .capacity       = sizeof(s_cfg_appDB_skin)/sizeof(eOappTheDB_cfg_skinInfo_t),
    .size           = sizeof(s_cfg_appDB_skin)/sizeof(eOappTheDB_cfg_skinInfo_t), 
    .item_size      = sizeof(eOappTheDB_cfg_skinInfo_t),
    .dummy          = 0,  
    .stored_items   = (void*) s_cfg_appDB_skin,
    .functions      = NULL      
};
extern const EOconstvector* const eo_cfg_appDB_theskins_info = &s_eo_cfg_appDB_constvec_skin;

