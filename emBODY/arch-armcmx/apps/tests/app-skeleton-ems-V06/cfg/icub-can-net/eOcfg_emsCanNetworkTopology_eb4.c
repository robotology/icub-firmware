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
    @brief      This file keeps ems EB4 can network configuration
    @author     valentina.gaggero@iit.it
    @date       05/17/2012
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

/*definition of left leg can net topology (ems 1) */

static const eo_emsCanNetTopo_jointOrMotorTopoInfo_t s_cfg_canNetTopo_joints[] = 
{
    { // 0 
        EO_INIT(.boardAddr)                 3,
        EO_INIT(.axis)                      0,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        0
    },    

    { // 1 
        EO_INIT(.boardAddr)                 3,
        EO_INIT(.axis)                      1,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        1
    },

    { // 2 
        EO_INIT(.boardAddr)                 4,
        EO_INIT(.axis)                      0,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        2
    },    

    { // 3 
        EO_INIT(.boardAddr)                 4,
        EO_INIT(.axis)                      1,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        3
    },

    { // 4 
        EO_INIT(.boardAddr)                 5,
        EO_INIT(.axis)                      0,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        4
    },    

    { // 5 
        EO_INIT(.boardAddr)                 5,
        EO_INIT(.axis)                      1,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        5
    },

    { // 6 
        EO_INIT(.boardAddr)                 6,
        EO_INIT(.axis)                      0,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        6
    },    

    { // 7 
        EO_INIT(.boardAddr)                 6,
        EO_INIT(.axis)                      1,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        7
    },

    { // 8 
        EO_INIT(.boardAddr)                 7,
        EO_INIT(.axis)                      0,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        8
    },    

    { // 9 
        EO_INIT(.boardAddr)                 7,
        EO_INIT(.axis)                      1,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        9
    },

    { // 10 
        EO_INIT(.boardAddr)                 8,
        EO_INIT(.axis)                      0,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        10
    },    

    { // 11 
        EO_INIT(.boardAddr)                 8,
        EO_INIT(.axis)                      1,
        EO_INIT(.canPort)                   eOcanport1,
        EO_INIT(.boardType)                 eobrd_mc4,
        EO_INIT(.id)                        11
    }
};


const EOconstvector  s_eo_cfg_emsCanNetTopo_constvec_joints = 
{
    EO_INIT(.size)              sizeof(s_cfg_canNetTopo_joints)/sizeof(eo_emsCanNetTopo_jointOrMotorTopoInfo_t), //EOK_cfg_nvsEP_joint_numberof,
    EO_INIT(.item_size)         sizeof(eo_emsCanNetTopo_jointOrMotorTopoInfo_t),
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
//Note: no analg sensor are connected to ems
//static const eo_emsCanNetTopo_sensorTopoInfo_t  s_cfg_canNetTopo_sensors[] = 
//{
//    {// 0
//        EO_INIT(.boardAddr)                 1,
//        EO_INIT(.canPort)                   eOcanport2,
//        EO_INIT(.boardType)                 eobrd_strain,
//        EO_INIT(.id)                        1
//    
//   }
//};


const EOconstvector  s_eo_cfg_emsCanNetTopo_constvec_sensors = 
{
    EO_INIT(.size)              0,
    EO_INIT(.item_size)         0,
    EO_INIT(.item_array_data)   NULL
};


extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_sensors__ptr = &s_eo_cfg_emsCanNetTopo_constvec_sensors;




/***********************************************************************************************/
/**********************************        S K I N       ***************************************/
/***********************************************************************************************/
//Note: since EMS doesn't manage skin, but it is only a gataway between skin boards and pc104, 
//the configuration says tahe skin is connected.
static const eo_emsCanNetTopo_sensorTopoInfo_t  s_cfg_canNetTopo_skin [] = 
{
    {// 0
        EO_INIT(.boardAddr)                 0,              //unused
        EO_INIT(.canPort)                   eOcanport2,     //important!!!
        EO_INIT(.boardType)                 eobdr_skin,     //obvious information
        EO_INIT(.id)                        0               //important!!!
   }
};


const EOconstvector  s_eo_cfg_emsCanNetTopo_constvec_skin = 
{
    EO_INIT(.size)              sizeof(s_cfg_canNetTopo_skin)/sizeof(eo_emsCanNetTopo_sensorTopoInfo_t), //EOK_cfg_nvsEP_joint_numberof,
    EO_INIT(.item_size)         sizeof(eo_emsCanNetTopo_sensorTopoInfo_t),
    EO_INIT(.item_array_data)   s_cfg_canNetTopo_skin
};


extern const EOconstvector* const eo_cfg_emsCanNetTopo_constvec_skin__ptr = &s_eo_cfg_emsCanNetTopo_constvec_skin;
