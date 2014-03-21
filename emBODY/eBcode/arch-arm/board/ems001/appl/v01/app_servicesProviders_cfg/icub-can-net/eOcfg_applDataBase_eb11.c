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

/* @file       eOcfg_emsCanNetworkTopology_eb11.c
    @brief      This file keeps ems EB11(only skin) can network configuration
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
//static const eOappTheDB_cfg_canBoardInfo_t s_cfg_appDB_boards[] = {};


const EOconstvector  s_eo_cfg_appDB_constvec_boards = 
{
    EO_INIT(.size)              0, //sizeof(s_cfg_appDB_boards)/sizeof(eOappTheDB_cfg_canBoardInfo_t), 
    EO_INIT(.item_size)         0, //sizeof(eOappTheDB_cfg_canBoardInfo_t),
    EO_INIT(.item_array_data)   NULL //s_cfg_appDB_boards
};

extern const EOconstvector* const eo_cfg_appDB_constvec_boards__ptr = &s_eo_cfg_appDB_constvec_boards;





/*definition of left leg can net topology (ems10) */

//static const eOappTheDB_cfg_jointInfo_t s_cfg_appDB_joints[] = {};


const EOconstvector  s_eo_cfg_appDB_constvec_joints = 
{
    EO_INIT(.size)              0, //sizeof(s_cfg_appDB_joints)/sizeof(eOappTheDB_cfg_jointInfo_t),
    EO_INIT(.item_size)         0, //sizeof(eOappTheDB_cfg_jointInfo_t),
    EO_INIT(.item_array_data)   NULL //s_cfg_appDB_joints
};

extern const EOconstvector* const eo_cfg_appDB_constvec_joints__ptr = &s_eo_cfg_appDB_constvec_joints;



/***********************************************************************************************/
/***********************         M O T O R S             ***************************************/
/***********************************************************************************************/

/*   since motors and joints in this body part are equivalent, we use the same constvecor: I'm lazy! :)   */
extern const EOconstvector* const eo_cfg_appDB_constvec_motors__ptr = &s_eo_cfg_appDB_constvec_joints;



/***********************************************************************************************/
/*****************************        S E N S O R S         ************************************/
/***********************************************************************************************/

//static const eOappTheDB_cfg_snsrMaisInfo_t  s_cfg_appDB_snsrMais[] = {};

//one mais board is coccented to eb2
const EOconstvector  s_eo_cfg_appDB_constvec_snsrMais = 
{
    EO_INIT(.size)              0, //sizeof(s_cfg_appDB_snsrMais)/sizeof(eOappTheDB_cfg_snsrMaisInfo_t), 
    EO_INIT(.item_size)         0, //sizeof(eOappTheDB_cfg_snsrMaisInfo_t),
    EO_INIT(.item_array_data)   NULL //s_cfg_appDB_snsrMais
};


extern const EOconstvector* const eo_cfg_appDB_constvec_snsrMais__ptr = &s_eo_cfg_appDB_constvec_snsrMais;





//static const eOappTheDB_cfg_snsrStrainInfo_t  s_cfg_appDB_snsrStrain[] = 
//{
//    {// 0
//       EO_INIT(.belong2board)                       STRAIN_ADDRCAN_14_BID,
//    }
//};

//no strain board are connected to ems2
const EOconstvector  s_eo_cfg_appDB_constvec_snsrStrain = 
{
    EO_INIT(.size)              0, //sizeof(s_cfg_appDB_snrStrain)/sizeof(eOappTheDB_cfg_snsrStrainInfo_t), 
    EO_INIT(.item_size)         0, //sizeof(eOappTheDB_cfg_snsrStrainInfo_t),
    EO_INIT(.item_array_data)   NULL //s_cfg_appDB_snsrStrain
};


extern const EOconstvector* const eo_cfg_appDB_constvec_snsrStrain__ptr = &s_eo_cfg_appDB_constvec_snsrStrain;


/***********************************************************************************************/
/**********************************        S K I N       ***************************************/
/***********************************************************************************************/
//Note: since EMS doesn't manage skin, but it is only a gataway between skin boards and pc104, 
//the configuration says tahe skin is connected.
static const eOappTheDB_cfg_skinInfo_t  s_cfg_appDB_skin [] = 
{
    {// 0
        EO_INIT(.connected2emsport)                       eOcanport2,
   }
};


const EOconstvector  s_eo_cfg_appDB_constvec_skin = 
{
     EO_INIT(.size)              sizeof(s_cfg_appDB_skin)/sizeof(eOappTheDB_cfg_skinInfo_t), 
     EO_INIT(.item_size)         sizeof(eOappTheDB_cfg_skinInfo_t),
     EO_INIT(.item_array_data)   s_cfg_appDB_skin
};


extern const EOconstvector* const eo_cfg_appDB_constvec_skin__ptr = &s_eo_cfg_appDB_constvec_skin;
