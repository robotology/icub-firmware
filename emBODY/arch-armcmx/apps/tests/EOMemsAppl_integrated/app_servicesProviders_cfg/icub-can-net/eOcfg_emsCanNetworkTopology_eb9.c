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

/* @file       eOcfg_emsCanNetworkTopology.c
    @brief      This file keeps ems EB7(right lower leg) can network configuration
    @author     valentina.gaggero@iit.it
    @date       08/23/2012
**/
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "EoCommon.h"
#include "EoBoards.h"
#include "EoMotionControl.h"
#include "EoSensors.h"
#include "EOconstvector.h"
#include "EOconstvector_hid.h"
#include "EOemsCanNetworkTopology.h"
#include "EOemsCanNetworkTopology_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_emsCanNetworkTopology.h"



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

static const eo_emsCanNetTopo_boardTopoInfo_t s_cfg_canNetTopo_boards[] = 
{
    { // 0 == FOC_ADDRCAN_1_BID 
        EO_INIT(.canaddr)                 1,
        EO_INIT(.emscanport)              eOcanport1,
        EO_INIT(.boardtype)               eobrd_1foc,
    },

    { // 1 == FOC_ADDRCAN_2_BID
        EO_INIT(.canaddr)                 4,
        EO_INIT(.emscanport)              eOcanport1,
        EO_INIT(.boardtype)               eobrd_1foc,
    }    
};

const EOconstvector  s_eo_cfg_emsCanNetTopo_constvec_boards = 
{
    EO_INIT(.size)              sizeof(s_cfg_canNetTopo_boards)/sizeof(eo_emsCanNetTopo_boardTopoInfo_t), //EOK_cfg_nvsEP_joint_numberof,
    EO_INIT(.item_size)         sizeof(eo_emsCanNetTopo_boardTopoInfo_t),
    EO_INIT(.item_array_data)   s_cfg_canNetTopo_boards
};

extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_boards__ptr = &s_eo_cfg_emsCanNetTopo_constvec_boards;

/*definition of left leg can net topology (ems 9) */

static const eo_emsCanNetTopo_jointTopoInfo_t s_cfg_canNetTopo_joints[] = 
{
   { // 0 
        EO_INIT(.bid)                       FOC_ADDRCAN_1_BID,
        EO_INIT(.j_idInBoard)               eo_emsCanNetTopo_jm_index_first,
        EO_INIT(.jid)                       0
    },    

    { // 1 
        EO_INIT(.bid)                       FOC_ADDRCAN_2_BID,
        EO_INIT(.j_idInBoard)               eo_emsCanNetTopo_jm_index_first,
        EO_INIT(.jid)                       1
    },


cconst EOconstvector  s_eo_cfg_emsCanNetTopo_constvec_joints = 
{
    EO_INIT(.size)              sizeof(s_cfg_canNetTopo_joints)/sizeof(eo_emsCanNetTopo_jointTopoInfo_t),
    EO_INIT(.item_size)         sizeof(eo_emsCanNetTopo_jointTopoInfo_t),
    EO_INIT(.item_array_data)   s_cfg_canNetTopo_joints
};

extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_joints__ptr = &s_eo_cfg_emsCanNetTopo_constvec_joints;


/***********************************************************************************************/
/***********************         M O T O R S             ***************************************/
/***********************************************************************************************/

/*   since motors and joints in this body part are equivalent, we use the same constvecor: I'm lazy! :)   */
extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_motors__ptr = &s_eo_cfg_emsCanNetTopo_constvec_joints;




/***********************************************************************************************/
/*********************** **********        S E N S O R S            ***************************************/
/***********************************************************************************************/
//Note: no analg sensor are connected to ems 9

// const EOconstvector  s_eo_cfg_emsCanNetTopo_constvec_sensors = 
// {
//     EO_INIT(.size)              0,
//     EO_INIT(.item_size)         0,
//     EO_INIT(.item_array_data)   NULL
// };


// extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_sensors__ptr = &s_eo_cfg_emsCanNetTopo_constvec_sensors;
extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_sensors__ptr = NULL;



/***********************************************************************************************/
/**********************************        S K I N       ***************************************/
/***********************************************************************************************/
//eb7 ems doesn't manage skin
// const EOconstvector  s_eo_cfg_emsCanNetTopo_constvec_skin= 
// {
//     EO_INIT(.size)              0,
//     EO_INIT(.item_size)         0
//     EO_INIT(.item_array_data)   NULL
// };


// extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_skin__ptr = &s_eo_cfg_emsCanNetTopo_constvec_skin;
extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_skin__ptr = NULL;