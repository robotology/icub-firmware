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
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "EoCommon.h"
#include "EoBoards.h"
#include "EoMotionControl.h"
#include "EoSensors.h"
#include "EOicubCanProto.h" 
#include "eOcfg_nvsEP_joint_con.h"
#include "EOconstLookupTbl.h"
#include "EOconstLookupTbl_hid.h"
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
static const eo_emsCanNetTopo_hid_LUTbl_item_boardNvsReferences_t s_boardCanLoc2NvsRef_port1_list[] = 
{
    {   // 0
        EO_INIT(.boardAddr)                 4,
        EO_INIT(.axis)                      eo_icubCanProto_mAxis_0,
        EO_INIT(.boardType)                 eo_icubCanProto_boardType__2foc,
        EO_INIT(.joint_nvid_list)
        {
            {  // 0 nvsType_joint_cfg
                EO_INIT(.ep)                    EOK_cfg_nvsEP_joint_endpoint,
                EO_INIT(.id)                    EOK_cfg_nvsEP_joint_NVID__cfg
            },
            {  // 1 nvsType_setPoint
                EO_INIT(.ep)                    EOK_cfg_nvsEP_joint_endpoint,
                EO_INIT(.id)                    EOK_cfg_nvsEP_joint_NVID__setPoint
            },
            {  // 2 nvsType_setPoint
                EO_INIT(.ep)                    EOK_cfg_nvsEP_joint_endpoint,
                EO_INIT(.id)                    EOK_cfg_nvsEP_joint_NVID__status
            }
        }
    }

};

//non vale, messo solo per compilare!!!
static const eo_emsCanNetTopo_hid_LUTbl_item_boardNvsReferences_t s_boardCanLoc2NvsRef_port2_list[] = 
{
 //port 2
    {   // 0
        EO_INIT(.boardAddr)                 4,
        EO_INIT(.axis)                      eo_icubCanProto_mAxis_0,
        EO_INIT(.boardType)                 eo_icubCanProto_boardType__2foc,
        EO_INIT(.joint_nvid_list)
        {
            {  // 0 nvsType_joint_cfg
                EO_INIT(.ep)                    EOK_cfg_nvsEP_joint_endpoint,
                EO_INIT(.id)                    EOK_cfg_nvsEP_joint_NVID__cfg
            },
            {  // 1 nvsType_setPoint
                EO_INIT(.ep)                    EOK_cfg_nvsEP_joint_endpoint,
                EO_INIT(.id)                    EOK_cfg_nvsEP_joint_NVID__setPoint
            },
            {  // 2 nvsType_setPoint
                EO_INIT(.ep)                    EOK_cfg_nvsEP_joint_endpoint,
                EO_INIT(.id)                    EOK_cfg_nvsEP_joint_NVID__status
            }
        }
    }

};

static const EOconstLookupTbl  s_eo_emsCanNetTopo_boardCanLoc2NvsRef_port1_LUTbl = 
{
    EO_INIT(.capacity)           1,
    EO_INIT(.offset)             0,
    EO_INIT(.exceptionMngFn)     NULL,
    EO_INIT(.itemsList)          s_boardCanLoc2NvsRef_port1_list
};


static const EOconstLookupTbl  s_eo_emsCanNetTopo_boardCanLoc2NvsRef_port2_LUTbl = 
{
    EO_INIT(.capacity)           0,
    EO_INIT(.offset)             0,
    EO_INIT(.exceptionMngFn)     NULL,
    EO_INIT(.itemsList)          s_boardCanLoc2NvsRef_port2_list
};

extern const EOconstLookupTbl* const emsCanNetTopo_motorBoardCanLoc2NvsRef_LUTbl__ptr[eo_emsCanNetTopo_canports_num] =
{
    { &s_eo_emsCanNetTopo_boardCanLoc2NvsRef_port1_LUTbl},
    { &s_eo_emsCanNetTopo_boardCanLoc2NvsRef_port2_LUTbl},
};








/***************************************************************************/
/*          look up tbl :  joint id ==> board can location                 */
/***************************************************************************/


static const eo_emsCanNetTopo_hid_LUTbl_item_joint2BoardCanLocation_t  s_joint2BoardCanLocation_list[] = 
{
    { // 0
        EO_INIT(.canPort)               eOcanport1,
        EO_INIT(.boardAddr)             4,
        EO_INIT(.axis4board)            eo_icubCanProto_mAxis_0
    }
};     
static const EOconstLookupTbl    s_joint2BoardCanLocation_LUTbl = 
{
    EO_INIT(.capacity)           1,
    EO_INIT(.offset)             0,
    EO_INIT(.exceptionMngFn)     NULL,
    EO_INIT(.itemsList)          s_joint2BoardCanLocation_list
};

extern const EOconstLookupTbl *emsCanNetTopo_joint2BoardCanLocation_LUTbl__ptr = &s_joint2BoardCanLocation_LUTbl;
