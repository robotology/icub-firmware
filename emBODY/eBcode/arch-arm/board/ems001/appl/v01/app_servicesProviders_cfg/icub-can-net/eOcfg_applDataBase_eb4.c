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

/* @file       eOcfg_emsCanNetworkTopology_eb4.c
    @brief      This file keeps ems EB4(right lower arm) can network configuration
    @author     valentina.gaggero@iit.it
    @date       05/17/2012
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

#define MC4_ADDRCAN_3_BID       0
#define MC4_ADDRCAN_4_BID       1
#define MC4_ADDRCAN_5_BID       2
#define MC4_ADDRCAN_6_BID       3
#define MC4_ADDRCAN_7_BID       4
#define MC4_ADDRCAN_8_BID       5
#define MAIS_ADDRCAN_14_BID     6

static const eOappTheDB_canboardinfo_t s_cfg_appDB_boards[] = 
{
    { // 0 == MC4_ADDRCAN_3_BID 
        .port   = eOcanport1,
        .addr   = 3,
        .type   = eobrd_mc4,
    },
    
    { // 1 == MC4_ADDRCAN_4_BID
        .port   = eOcanport1,
        .addr   = 4,
        .type   = eobrd_mc4,
    },   

    { // 2  == MC4_ADDRCAN_5_BID
        .port   = eOcanport1,
        .addr   = 5,
        .type   = eobrd_mc4,
    },

    { // 3 == MC4_ADDRCAN_6_BID
        .port   = eOcanport1,
        .addr   = 6,
        .type   = eobrd_mc4,
    },
       
    { // 4  == MC4_ADDRCAN_7_BID
        .port   = eOcanport1,
        .addr   = 7,
        .type   = eobrd_mc4,
    },
    
    { // 5 == MC4_ADDRCAN_8_BID
        .port   = eOcanport1,
        .addr   = 8,
        .type   = eobrd_mc4,
    },    

    { // 6  == MAIS_ADDRCAN_14_BID
        .port   = eOcanport1,
        .addr   = 14,
        .type   = eobrd_mais,
    }  
     
};

#warning --> marco.accame: add the skins addresses in can2 in here to manage messages from/to them


const EOconstvector  s_eo_cfg_appDB_constvec_boards = 
{
    EO_INIT(.size)              sizeof(s_cfg_appDB_boards)/sizeof(eOappTheDB_canboardinfo_t), 
    EO_INIT(.item_size)         sizeof(eOappTheDB_canboardinfo_t),
    EO_INIT(.item_array_data)   s_cfg_appDB_boards
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

static const eOappTheDB_mapping2canboard_t s_cfg_appDB_joints[] = 
{
    { // 0 
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_3_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
    },    

    { // 1 

        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_3_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_second,
    },

    { // 2 
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_4_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
    },    
    { // 3
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_4_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_second,
    },
    
    { // 4 
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_5_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
    },    

    { // 5 
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_5_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_second,
    },

    { // 6 
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_6_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
    },    

    { // 7 
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_6_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_second,
    },

    { // 8 
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_7_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
    },    

    { // 9 
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_7_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_second,
    },

    { // 10 
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_8_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
    },    
    { // 11
        EO_INIT(.indexofcanboard)               MC4_ADDRCAN_8_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_second,
    }
};


const EOconstvector  s_eo_cfg_appDB_constvec_joints = 
{
    EO_INIT(.size)              sizeof(s_cfg_appDB_joints)/sizeof(eOappTheDB_mapping2canboard_t),
    EO_INIT(.item_size)         sizeof(eOappTheDB_mapping2canboard_t),
    EO_INIT(.item_array_data)   s_cfg_appDB_joints
};

extern const EOconstvector* const eo_cfg_appDB_thejoints_mapping2canboards = &s_eo_cfg_appDB_constvec_joints;



// - in here we have the mapping of the motors into a given can board and its internal index (0, 1). 
// - the same as joints
extern const EOconstvector* const eo_cfg_appDB_themotors_mapping2canboards = &s_eo_cfg_appDB_constvec_joints;




// - in here we have the mapping of the mais sensors into a given can board

static const eOappTheDB_mapping2canboard_t  s_cfg_appDB_snsrMais[] = 
{
    {// 0
        .indexofcanboard        = MAIS_ADDRCAN_14_BID,
        .indexinsidecanboard    = eOappTheDB_canboard_noindex
    }
};


const EOconstvector  s_eo_cfg_appDB_constvec_snsrMais = 
{
    EO_INIT(.size)              sizeof(s_cfg_appDB_snsrMais)/sizeof(eOappTheDB_mapping2canboard_t), 
    EO_INIT(.item_size)         sizeof(eOappTheDB_mapping2canboard_t),
    EO_INIT(.item_array_data)   s_cfg_appDB_snsrMais
};


extern const EOconstvector* const eo_cfg_appDB_themaises_mapping2canboards = &s_eo_cfg_appDB_constvec_snsrMais;


// - in here we have the mapping of the strain sensors into a given can board


const EOconstvector  s_eo_cfg_appDB_constvec_snsrStrain = 
{   // empty
    EO_INIT(.size)              0, //sizeof(s_cfg_appDB_snrStrain)/sizeof(eOappTheDB_mapping2canboard_t), 
    EO_INIT(.item_size)         0, //sizeof(eOappTheDB_mapping2canboard_t),
    EO_INIT(.item_array_data)   NULL //s_cfg_appDB_snsrStrain
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
        EO_INIT(.connected2emsport)                       eOcanport2,
        EO_INIT(.boardAddrStart)                          8,
        EO_INIT(.numofboards)                             7
    }
};


const EOconstvector  s_eo_cfg_appDB_constvec_skin = 
{
     EO_INIT(.size)              sizeof(s_cfg_appDB_skin)/sizeof(eOappTheDB_cfg_skinInfo_t), 
     EO_INIT(.item_size)         sizeof(eOappTheDB_cfg_skinInfo_t),
     EO_INIT(.item_array_data)   s_cfg_appDB_skin
};


extern const EOconstvector* const eo_cfg_appDB_theskins_info = &s_eo_cfg_appDB_constvec_skin;

