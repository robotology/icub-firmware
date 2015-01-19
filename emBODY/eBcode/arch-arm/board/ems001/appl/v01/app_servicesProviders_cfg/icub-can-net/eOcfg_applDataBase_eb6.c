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

/* @file       eOcfg_applDataBase_eb6.c
    @brief      This file keeps ems EB6 (left upper leg) can network configuration
    @author     valentina.gaggero@iit.it
    @date       07/10/2012
**/
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "EoCommon.h"
#include "EOconstvector_hid.h" 

#include "emBODYrobot.h"

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

#define FOC_ADDRCAN_1_BID       0
#define FOC_ADDRCAN_2_BID       1
#define FOC_ADDRCAN_3_BID       2
#define FOC_ADDRCAN_4_BID       3
#define STRAIN_ADDRCAN_13_BID   4


static const eOappTheDB_canboardinfo_t s_cfg_appDB_boards[] = 
{
    { // 0 == FOC_ADDRCAN_1_BID 
        .port   = eOcanport1,
        .addr   = 1,
        .type   = eobrd_1foc,
    },
    { // 1 == FOC_ADDRCAN_2_BID 
        .port   = eOcanport1,
        .addr   = 2,
        .type   = eobrd_1foc,
    }, 
    { // 2  == FOC_ADDRCAN_3_BID
        .port   = eOcanport1,
        .addr   = 3,
        .type   = eobrd_1foc,
    },      
    { // 3  == FOC_ADDRCAN_4_BID
        .port   = eOcanport1,
        .addr   = 4,
        .type   = eobrd_1foc,
    },  
    { // 4  == STRAIN_ADDRCAN_13_BID
        .port   = eOcanport2,
        .addr   = 13,
        .type   = eobrd_strain,
    }
};    
    

const EOconstvector  s_eo_cfg_appDB_constvec_boards = 
{
    EO_INIT(.size)              sizeof(s_cfg_appDB_boards)/sizeof(eOappTheDB_canboardinfo_t), 
    EO_INIT(.item_size)         sizeof(eOappTheDB_canboardinfo_t),
    EO_INIT(.item_array_data)   s_cfg_appDB_boards
};

extern const EOconstvector* const eo_cfg_appDB_thecanboards = &s_eo_cfg_appDB_constvec_boards;



// joints

static const eOappTheDB_mapping2canboard_t s_cfg_appDB_joints[] = 
{
    { // 0 
        EO_INIT(.indexofcanboard)               FOC_ADDRCAN_3_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
    },    
    { // 1 
        EO_INIT(.indexofcanboard)               FOC_ADDRCAN_4_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
    }, 
    { // 2 
        EO_INIT(.indexofcanboard)               FOC_ADDRCAN_1_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
    }, 
    { // 3 
        EO_INIT(.indexofcanboard)               FOC_ADDRCAN_2_BID,
        EO_INIT(.indexinsidecanboard)           eo_icubCanProto_jm_index_first,
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

//static const eOappTheDB_mapping2canboard_t  s_cfg_appDB_snsrMais[] = 
//{
//    {// 0
//       .indexofcanboard        = MAIS_ADDRCAN_14_BID,
//        .indexinsidecanboard    = eOappTheDB_canboard_noindex
//    }
//};

const EOconstvector  s_eo_cfg_appDB_constvec_snsrMais = 
{
    EO_INIT(.size)              0, //sizeof(s_cfg_appDB_snsrMais)/sizeof(eOappTheDB_mapping2canboard_t), 
    EO_INIT(.item_size)         0, //sizeof(eOappTheDB_mapping2canboard_t),
    EO_INIT(.item_array_data)   NULL, //s_cfg_appDB_snsrMais
};

extern const EOconstvector* const eo_cfg_appDB_themaises_mapping2canboards = &s_eo_cfg_appDB_constvec_snsrMais;


// - in here we have the mapping of the strain sensors into a given can board

static const eOappTheDB_mapping2canboard_t  s_cfg_appDB_snsrStrain[] =
{
    {// 0
        .indexofcanboard        =   STRAIN_ADDRCAN_13_BID,
        .indexinsidecanboard    =   eOappTheDB_canboard_noindex 
    }
};


const EOconstvector  s_eo_cfg_appDB_constvec_snsrStrain = 
{   
    EO_INIT(.size)              sizeof(s_cfg_appDB_snsrStrain)/sizeof(eOappTheDB_mapping2canboard_t), 
    EO_INIT(.item_size)         sizeof(eOappTheDB_mapping2canboard_t),
    EO_INIT(.item_array_data)   s_cfg_appDB_snsrStrain
};

extern const EOconstvector* const eo_cfg_appDB_thestrains_mapping2canboards = &s_eo_cfg_appDB_constvec_snsrStrain;



// skin
// no skin
const EOconstvector  s_eo_cfg_appDB_constvec_skin = 
{
     EO_INIT(.size)              0, //sizeof(s_cfg_appDB_skin)/sizeof(eOappTheDB_cfg_skinInfo_t), 
     EO_INIT(.item_size)         0, //sizeof(eOappTheDB_cfg_skinInfo_t),
     EO_INIT(.item_array_data)   NULL //s_cfg_appDB_skin
};


extern const EOconstvector* const eo_cfg_appDB_theskins_info = &s_eo_cfg_appDB_constvec_skin;


