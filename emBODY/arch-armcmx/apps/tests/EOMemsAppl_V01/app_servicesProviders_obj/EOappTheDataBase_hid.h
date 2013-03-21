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
    eOappEncReader_encoder_t            connectedEnc;       /**< currently i assumed: the encoder connected to joint with jointId=x, has encoderid=x */
    eOmc_motorId_t                      ref2motor;          /**< currently i assumed: the motor connected to joint with jointId=x, has motorid=x */
    //in teoria i seguenti valori dovrebbero dipendere dal giunto
    //ma visto che per ora si usano brasati nel codice 
    //qui inserisco il puntatore alla struttura data allocata dinamicamente
    //una volta sola per tutti a cui tutti puntano.
    eOappTheDB_jointShiftValues_t       *shiftvalues_ptr;   
    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;
    eOmc_controlmode_t                   curr_mode;     /**< ATTENTION: currently not used!!
															 Here current control mode is saved.
                                                             When an update of control mode arrive, in callback I'dont know last currmode, 
                                                             so i can use this filed to compere new mode wthi curr.
                                                             Obviously this file have to be updated at the end of nv callback. */
} eOappTheDB_hid_jointInfo_t;


typedef struct
{
    eOappTheDB_cfg_motorInfo_t   		*cfg_ptr;
    eOmc_jointId_t                      ref2joint;
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
	
	struct
	{
		void ***jnvMap;
		void ***mnvMap;
		void ***snsrMaisnvMap;
		void ***snsrStrainnvMap;
		void ***skinnvMap;
	}nvsrefMaps;


};



typedef enum
{
    eOappTheDB_hid_jointNVindex_jconfig                               = 0,
    eOappTheDB_hid_jointNVindex_jstatus                                = 1,
    eOappTheDB_hid_jointNVindex_jinputs                               = 2,
    eOappTheDB_hid_jointNVindex_jcmmnds__calibration                  = 3,
    eOappTheDB_hid_jointNVindex_jcmmnds__setpoint                     = 4,
    eOappTheDB_hid_jointNVindex_jcmmnds__stoptrajectory               = 5,
    eOappTheDB_hid_jointNVindex_jcmmnds__controlmode                  = 6,
} eOappTheDB_hid_jointNVindex_t;
enum{ eOappTheDB_hid_jointNVindex_TOTALnumber = eOappTheDB_hid_jointNVindex_jcmmnds__controlmode +1 };


typedef enum
{
    eOappTheDB_hid_motorNVindex_mconfig                               = 0,
    eOappTheDB_hid_motorNVindex_mstaus                                = 1
} eOappTheDB_hid_motorNVindex_t;

enum{ eOappTheDB_hid_motorNVindex_TOTALnumber = eOappTheDB_hid_motorNVindex_mstaus +1 };





typedef enum
{
    eOappTheDB_hid_snsrMaisNVindex_mconfig                               = 0,
    eOappTheDB_hid_snsrMaisNVindex_mstatus                               = 1
} eOappTheDB_hid_snsrMaisNVindex_t;

enum{ eOappTheDB_hid_snsrMaisNVindex_TOTALnumber = eOappTheDB_hid_snsrMaisNVindex_mstatus +1 };


typedef enum
{
    eOappTheDB_hid_snsrStrainNVindex_sconfig                               = 0,
    eOappTheDB_hid_snsrStrainNVindex_sstatus                               = 1
} eOappTheDB_hid_snsrStrainNVindex_t;

enum{ eOappTheDB_hid_snsrStrainNVindex_TOTALnumber = eOappTheDB_hid_snsrStrainNVindex_sstatus +1 };


typedef enum
{
    eOappTheDB_hid_skinNVindex_sconfig__sigmode                            = 0,
    eOappTheDB_hid_skinNVindex_sstaus__arrayof10canframe                   = 1
} eOappTheDB_hid_skinNVindex_t;

enum{ eOappTheDB_hid_skinNVindex_TOTALnumber = eOappTheDB_hid_skinNVindex_sstaus__arrayof10canframe +1 };

// - declaration of const ---------------------------------------------------------------------------

static const eOcfg_nvsEP_mc_jointNVindex_t s_eoappTheDB_jointIndexMap[eOappTheDB_hid_jointNVindex_TOTALnumber] = 
{
    jointNVindex_jconfig,
    jointNVindex_jstatus,
    jointNVindex_jinputs,
    jointNVindex_jcmmnds__calibration,
    jointNVindex_jcmmnds__setpoint,
    jointNVindex_jcmmnds__stoptrajectory,
    jointNVindex_jcmmnds__controlmode
};


static const eOcfg_nvsEP_mc_motorNVindex_t s_eoappTheDB_motorIndexMap[eOappTheDB_hid_motorNVindex_TOTALnumber] = 
{
    motorNVindex_mconfig,
    motorNVindex_mstatus
};

static const eOcfg_nvsEP_as_maisNVindex_t s_eoappTheDB_snsrMaisIndexMap[eOappTheDB_hid_snsrMaisNVindex_TOTALnumber] = 
{
    maisNVindex_mconfig,
    maisNVindex_mstatus
};

static const eOcfg_nvsEP_as_strainNVindex_t s_eoappTheDB_snsrStrainIndexMap[eOappTheDB_hid_snsrStrainNVindex_TOTALnumber] = 
{
    strainNVindex_sconfig,
    strainNVindex_sstatus
};


static const eOcfg_nvsEP_sk_skinNVindex_t s_eoappTheDB_skinIndexMap[eOappTheDB_hid_skinNVindex_TOTALnumber] = 
{
    skinNVindex_sconfig__sigmode,
    skinNVindex_sstatus__arrayof10canframe
};



// - declaration of extern hidden functions ---------------------------------------------------------------------------

EO_extern_inline eObrd_boardId_t eo_appTheDB_hid_GetBoardIdWithAddress(EOappTheDB *p, eOicubCanProto_canBoardAddress_t addr)
{
    return(p->canaddressLookuptbl.tbl[addr]);
}

EO_extern_inline eOcfg_nvsEP_mc_jointNVindex_t eo_appTheDB_hid_GetMCjointNVindex(eOappTheDB_hid_jointNVindex_t index)
{
    return(s_eoappTheDB_jointIndexMap[index]);
}


EO_extern_inline eOcfg_nvsEP_mc_motorNVindex_t eo_appTheDB_hid_GetMCmotorNVindex(eOappTheDB_hid_motorNVindex_t index)
{
    return(s_eoappTheDB_motorIndexMap[index]);
}

EO_extern_inline eOcfg_nvsEP_as_maisNVindex_t eo_appTheDB_hid_GetASmaisNVindex(eOappTheDB_hid_snsrMaisNVindex_t index)
{
    return(s_eoappTheDB_snsrMaisIndexMap[index]);
}


EO_extern_inline eOcfg_nvsEP_as_strainNVindex_t eo_appTheDB_hid_GetASstrainNVindex(eOappTheDB_hid_snsrStrainNVindex_t index)
{
    return(s_eoappTheDB_snsrStrainIndexMap[index]);
}


EO_extern_inline eOcfg_nvsEP_sk_skinNVindex_t eo_appTheDB_hid_GetSKskinNVindex(eOappTheDB_hid_skinNVindex_t index)
{
    return(s_eoappTheDB_skinIndexMap[index]);
}



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



