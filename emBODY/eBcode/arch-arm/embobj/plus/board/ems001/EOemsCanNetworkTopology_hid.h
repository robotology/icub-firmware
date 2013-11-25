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
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOEMSCANNETWORKTOPOLOGY_HID_H_
#define _EOEMSCANNETWORKTOPOLOGY_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


// - doxy -------------------------------------------------------------------------------------------------------------

/*  @file       EoemsCanNetworkTopology_hid.h
	@brief      This header file gives 
	@author     valentina.gagger@iit.it
	@date       03/08/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
#include "Eocommon.h"
#include "EoUtilities.h"
#include "EoBoards.h"

#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_as.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EoemsCanNetworkTopology.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

#define MAX_CAN_ADDRESS                     0XF 
#define joint_idNULL                        jointNumberMAX + 1
#define motor_idNULL                        motorNumberMAX + 1

typedef struct
{
    eo_emsCanNetTopo_jointTopoInfo_t     *j_ptr; //is the pointer to the element of vector that describes CAN network topology of this board
    eo_emsCanNetTopo_boardTopoInfo_t     *b_ptr;
} eo_emsCanNetTopo_hashTbl_j_item_t;


typedef struct
{
    eo_emsCanNetTopo_motorTopoInfo_t     *m_ptr; //is the pointer to the element of vector that describes CAN network topology of this board
    eo_emsCanNetTopo_boardTopoInfo_t     *b_ptr;
} eo_emsCanNetTopo_hashTbl_m_item_t;


typedef struct
{
    eo_emsCanNetTopo_sensorTopoInfo_t    *s_ptr;
    eo_emsCanNetTopo_boardTopoInfo_t     *b_ptr;
} eo_emsCanNetTopo_hashTbl_s_item_t; //eo_emsCanNetTopo_hashTbl_jm_item_t;

typedef enum
{
    eo_emsCanNetTopo_hashTbl_itemType__joint        = 0,
    eo_emsCanNetTopo_hashTbl_itemType__motor        = 1,
    eo_emsCanNetTopo_hashTbl_itemType__sensor       = 2,
    eo_emsCanNetTopo_hashTbl_itemType__skin         = 3
} eo_emsCanNetTopo_hashTbl_itemType_t;

typedef struct
{
    eo_emsCanNetTopo_hashTbl_itemType_t itemType;
    uint16_t id;
} eo_emsCanNetTopo_hashTbl_jm_addr2item;

/** @struct     EOemsCanNetTopo_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/   
struct EOemsCanNetTopo_hid 
{
    eo_emsCanNetTopo_cfg_t cfg;

//     eo_emsCanNetTopo_hashTbl_jm_item_t joint_Id2CanLoc_hTbl[jointNumberMAX];
//     eo_emsCanNetTopo_hashTbl_jm_item_t motor_Id2CanLoc_hTbl[motorNumberMAX];  

//     eo_emsCanNetTopo_hashTbl_jm_item_t joint_CanLoc2Id_hTbl[eo_emsCanNetTopo_canports_num][MAX_CAN_ADDRESS][2];
//     eo_emsCanNetTopo_hashTbl_jm_item_t motor_CanLoc2Id_hTbl[eo_emsCanNetTopo_canports_num][MAX_CAN_ADDRESS][2];
//     /* since only one sensor board is connected to ems, i don't need hash table id-->canaddr.
//     moreover i may use only one ptr, inipedeting of type of sensor (mais or strain), but i prefer to use two,
//     in order to be complinet with other appl's objs*/
//     eo_emsCanNetTopo_hashTbl_s_item_t  sensorStrain_hTbl; 
//     eo_emsCanNetTopo_hashTbl_s_item_t  sensorMais_hTbl;

    eo_emsCanNetTopo_hashTbl_j_item_t   joint_Id2CanLoc_hTbl[jointNumberMAX];
    eo_emsCanNetTopo_hashTbl_m_item_t   motor_Id2CanLoc_hTbl[motorNumberMAX];  
    /* since only one sensor board is connected to ems, i don't need hash table id-->canaddr.
    moreover i may use only one ptr, inipedeting of type of sensor (mais or strain), but i prefer to use two,
    in order to be complinet with other appl's objs*/
    eo_emsCanNetTopo_hashTbl_s_item_t   sensorStrain_hTbl; 
    eo_emsCanNetTopo_hashTbl_s_item_t   sensorMais_hTbl;
    
    eOmc_jointId_t      joint_CanLoc2Id_hTbl[eo_emsCanNetTopo_canports_num][MAX_CAN_ADDRESS][eo_emsCanNetTopo_jm_indexInBoard_max];
    eOmc_motorId_t      motor_CanLoc2Id_hTbl[eo_emsCanNetTopo_canports_num][MAX_CAN_ADDRESS][eo_emsCanNetTopo_jm_indexInBoard_max];


};








// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


