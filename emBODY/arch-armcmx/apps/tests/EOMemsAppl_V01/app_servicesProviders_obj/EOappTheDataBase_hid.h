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
#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_as.h"
#include "eOcfg_nvsEP_sk.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOappTheDataBase.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    eOappTheDB_cfg_canBoardInfo_t       *cfg_ptr;
    struct
    {
        struct
        {
            eOmc_jointId_t              *connectedjoints;
            eOmc_motorId_t              *connectedmotors;
        }jm;
//         eOsnsr_sensorId_t               *connectedsensors;
    }s;
    
    
//    eObrd_boardId                       bid;      Eliminato perche' non so se mi serve!!
} eOappTheDB_hid_canBoardInfo_t;

typedef struct
{
    eOappTheDB_cfg_jointInfo_t   		*cfg_ptr;
    eOappTheDB_jointShiftValues_t       *shiftvalues_ptr;   
    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;
} eOappTheDB_hid_jointInfo_t;


typedef struct
{
    eOappTheDB_cfg_motorInfo_t   		*cfg_ptr;
} eOappTheDB_hid_motorInfo_t;


typedef struct
{
    eOappTheDB_cfg_snsrMaisInfo_t       *cfg_ptr;
} eOappTheDB_hid_snsrMaisInfo_t;

typedef struct
{
    eOappTheDB_cfg_snsrStrainInfo_t       *cfg_ptr;
} eOappTheDB_hid_snsrStrainInfo_t;


typedef struct
{
    eOappTheDB_cfg_skinInfo_t       *cfg_ptr;
} eOappTheDB_hid_skinInfo_t;


// typedef struct
// {
//     uint16_t                            numberofconnectedjoints;
//     uint16_t                            numberofconnectedmotors;
//     uint16_t                            numberofsensormais;
//     uint16_t                            numberofsensorstrain;
//     eOicubCanProto_canBoardAddress_t    maxusedcanaddr4motorboard;
// } eOappTheDB_hid_generaldata_t;

typedef struct
{
    eOmc_joint_t            *jointsList_ptr;    
    eOmc_motor_t            *motorsList_ptr;
    eOmc_controller_t       *thecontroller;
    eOsnsr_mais_t           *maisList_ptr;
    eOsnsr_strain_t         *strainList_ptr;
    eOskin_someskin_t       *someskin_ptr;
} eOappTheDB_hid_nvsRamRef_t;


struct EOappTheDB_hid
{
    eOappTheDB_cfg_t                   cfg;
    eOboolvalues_t                     isinitted;
    EOarray                            *canboardsList;
    EOarray                            *jointsList;
    EOarray                            *motorsList;
    EOarray                            *snsrMaisList;
    EOarray                            *snsrStrainList;
    EOarray                            *skinList;
	struct
	{
		eOsizecntnr_t 		capacity;
		eObrd_boardId_t		*tbl; /* gli indici partono da zero,a nche se l'inidirizzo zero sar'a' sempre usato dalla ems.*/
	}canaddressLookuptbl;

    eOappTheDB_hid_nvsRamRef_t nvsram;

};




// - declaration of const ---------------------------------------------------------------------------


// - declaration of extern hidden functions ---------------------------------------------------------------------------

EO_extern_inline eObrd_boardId_t eo_appTheDB_hid_GetBoardIdWithAddress(EOappTheDB *p, eOicubCanProto_canBoardAddress_t addr)
{
    return(p->canaddressLookuptbl.tbl[addr]);
}


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



