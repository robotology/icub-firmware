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
#ifndef _EOAPPDATABASE_HID_H_
#define _EOAPPDATABASE_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOappEncodersReader.h
    @brief     This file provides hidden interface to encodersReader obj.
    @author    valentina.gaggero@iit.it
    @date      09/11/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------


// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOappTheDataBase.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    eOappTheDB_canboardinfo_t           basicboardinfo;
    eOmc_jointId_t                      connectedjoints[eOicubCanProto_jm_indexInBoard_max];
    eOmc_motorId_t                      connectedmotors[eOicubCanProto_jm_indexInBoard_max];
    eObool_t                            isready; // the board is ready when it responds to get_fw_ver can msg
} eOappTheDB_hid_canBoardInfo_t;

typedef struct
{
    eOappTheDB_mapping2canboard_t 		mapping2canboard;
    eOappTheDB_jointShiftValues_t       *shiftvalues_ptr;   
    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;
#ifdef USE_PROTO_PROXY
    EOlist                              *ethProtoReq_list;  // list of eOappTheDB_hid_ethProtoRequest_t items
#endif
} eOappTheDB_hid_jointInfo_t;


typedef struct
{
    eOappTheDB_mapping2canboard_t 		mapping2canboard;
#ifdef USE_PROTO_PROXY
    EOlist                              *ethProtoReq_list;  // list of eOappTheDB_hid_ethProtoRequest_t items
#endif
} eOappTheDB_hid_motorInfo_t;


typedef struct
{
    eOappTheDB_mapping2canboard_t       *cfg_ptr;
} eOappTheDB_hid_snsrMaisInfo_t;

typedef struct
{
    eOappTheDB_mapping2canboard_t       *cfg_ptr;
} eOappTheDB_hid_snsrStrainInfo_t;


typedef struct
{
    eOappTheDB_cfg_skinInfo_t       *cfg_ptr;
} eOappTheDB_hid_skinInfo_t;



typedef struct
{
    uint16_t values[6];
    uint8_t  isupdated;
}eOappTheDB_hid_virtualStrainData_t;


typedef struct
{
    eOmc_joint_t            *jointsList_ptr;    
    eOmc_motor_t            *motorsList_ptr;
    eOmc_controller_t       *thecontroller;
    eOas_mais_t             *maisList_ptr;
    eOas_strain_t           *strainList_ptr;
    eOsk_skin_t             *skin_ptr;
} eOappTheDB_hid_nvsRamRef_t;

typedef struct
{
		eOsizecntnr_t 	    capacity;
		eObrd_boardId_t		*tbl; // gli indici partono da zero, anche se l'indirizzo zero sara' sempre usato dalla ems
} eOappTheDB_hid_canaddressLookuptbl;

struct EOappTheDB_hid
{
    eOappTheDB_cfg_t                    cfg;
    eObool_t                            isinitted;
    EOarray                             *canboardsInfo;     // array of items eOappTheDB_hid_canBoardInfo_t
    EOarray                             *jointsInfo;        // array of items eOappTheDB_hid_jointInfo_t. 
    EOarray                             *motorsInfo;        // array of items eOappTheDB_hid_motorInfo_t  
    EOarray                             *maisesInfo;        // array of items eOappTheDB_hid_snsrMaisInfo_t          
    EOarray                             *strainsInfo;       // array of items eOappTheDB_hid_snsrStrainInfo_t
    EOarray                             *skinsInfo;         // array of items eOappTheDB_hid_skinInfo_t
	eOappTheDB_hid_canaddressLookuptbl  canaddressLookuptbl;
    eOappTheDB_hid_virtualStrainData_t  virtualStrainData;
};



// - declaration of const ---------------------------------------------------------------------------


// - declaration of extern hidden functions ---------------------------------------------------------------------------

EO_extern_inline eObrd_boardId_t eo_appTheDB_hid_GetBoardIdWithAddress(EOappTheDB *p, icubCanProto_canBoardAddress_t addr)
{
    return(p->canaddressLookuptbl.tbl[addr]);
}


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



