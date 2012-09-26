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
#include "EOappDataBase.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    eOappTheDB_cfg_canBoardInfo_t         *cfg_ptr;
    union
    {
        struct
        {
            eOmc_jointId_t                    *connectedjoints;
            eOmc_motorId_t                    *connectedmotors;
        }jm;
        eOsnsr_sensorId_t                 *connectedsensors;
    }u;
//    eObrd_boardId                       bid;      Eliminato perche' non so se mi serve!!
} eOappTheDB_hid_canBoardInfo_t;

typedef struct
{
    eOappTheDB_cfg_jointInfo_t   		*cfg_ptr;
    eOappEncReader_encoder_t            connectedEnc;  /**< currently i assumed: the encoder connected to joint with jointId=x, has encoderid=x */
    eOmc_motorId_t                      ref2motor;     /**< currently i assumed: the motor connected to joint with jointId=x, has motorid=x */
} eOappTheDB_hid_jointInfo_t;


typedef struct
{
    eOappTheDB_cfg_motorInfo_t   		*cfg_ptr;
    eOmc_jointId_t                      ref2joint;
} eOappTheDB_hid_motorInfo_t;




typedef struct
{
    eOappTheDB_cfg_snrMaisInfo_t       *cfg_ptr;
} eOappTheDB_hid_snrMaisInfo_t;

typedef struct
{
    eOappTheDB_cfg_snrMaisInfo_t       *cfg_ptr;
} eOappTheDB_hid_snrStrainInfo_t;


typedef struct
{
    eOappTheDB_cfg_SkinInfo_t       *cfg_ptr;
} eOappTheDB_hid_skinInfo_t;


typedef struct
{
    uint16_t                            numberofconnectedjoints;
    uint16_t                            numberofconnectedmotors;
    uint16_t                            numberofsensormais;
    uint16_t                            numberofsensorstrain;
    eOicubCanProto_canBoardAddress_t    maxusedcanaddr4motorboard;
} eOappTheDB_hid_generaldata_t;



struct EOappTheDB_hid
{
    eOboolvalues_t                     isinitted;
    EOarray                            *canboardsList;
    EOarray                            *jointsList;
    EOarray                            *motorsList;
    EOarray                            *snrMaisList;
    EOarray                            *snrStrainList;
    EOarray                            *skinList;
	struct
	{
		eOsizecntnr_t 		capacity;
		eObdr_boardId_t		*tbl; /* gli indici partono da zero,a nche se l'inidirizzo zero sar'a' sempre usato dalla ems.*/
	}canaddressLookuptbl;

};


// - declaration of extern hidden functions ---------------------------------------------------------------------------

__inline eObrd_boardId_t eo_appTheDB_hid_GetBoardIdWithAddress(EOappTheDB *p, eOicubCanProto_canBoardAddress_t addr)
{
    return(p->canaddressLookuptbl.tbl[canloc_ptr->addr]);
}



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



