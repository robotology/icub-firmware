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
#ifndef _EOAPPDATABASE_H_
#define _EOAPPDATABASE_H_

#ifdef __cplusplus
extern "C" {
#endif


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOappEncodersReader.h
    @brief     This file provides interface to encodersReader.
    @author    valentina.gaggero@iit.it
    @date       09/11/2012
**/
/** @defgroup eo_app_dataBase Object EOappDataBase
    .........     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "eOcommon.h"
#include "EOconstvector.h"
#include "EOnv.h"
#include "EOnvsCfg.h"
#include "eOboards.h"
#include "eOMotionControl.h"
#include "EoAnalogSensors.h"
#include "eOSkin.h"
#include "EOicubCanProto_specifications.h"
#include "EOappEncodersReader.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef struct
{
    eObrd_boardId_t                         belong2board;
    eOicubCanProto_jm_indexinBoard_t        indexinboard;
} eOappTheDB_cfg_jointOrMotorCanLocation_t;

typedef struct
{
    eOcanport_t                             emscanport;
    icubCanProto_canBoardAddress_t          addr;
} eOappTheDB_canBoardCanLocation_t;

typedef eOappTheDB_canBoardCanLocation_t    eOappTheDB_sensorCanLocation_t;

typedef struct
{
    eOappTheDB_cfg_jointOrMotorCanLocation_t    canLoc;
} eOappTheDB_cfg_jointInfo_t;

typedef struct
{
    eOappTheDB_cfg_jointOrMotorCanLocation_t    canLoc;
} eOappTheDB_cfg_motorInfo_t;


typedef struct
{
    eOappTheDB_canBoardCanLocation_t           canLoc;
    eObrd_types_t                              type;
} eOappTheDB_cfg_canBoardInfo_t;



// typedef enum 
// {
//     eOappTheDB_hid_sensortype__mais = 0,
//     eOappTheDB_hid_sensortype__strain = 1
// } eOappTheDB_sensorType_t;

// enum{eOappTheDB_hid_sensorType_MAXnumber = 2};



typedef struct
{
    eObrd_boardId_t                           belong2board;
} eOappTheDB_cfg_snsrMaisInfo_t;

typedef struct
{
    eObrd_boardId_t                           belong2board;
} eOappTheDB_cfg_snsrStrainInfo_t;

typedef struct
{
    eOcanport_t                           	connected2emsport;
} eOappTheDB_cfg_skinInfo_t;


typedef struct
{
    uint8_t     jointVelocityShift;
    uint8_t     jointVelocityEstimationShift;
    uint8_t     jointAccelerationEstimationShift;
    uint8_t     dummy;
    /* put here ather shift values like: 
            uint8_t     motorVelocityEstimationShift;
            uint8_t     motorAccelerationEstimationShift;
            uint8_t     jointVelocityEstimationShift;
            
    */
} eOappTheDB_jointShiftValues_t;       //thi values are usefull only for joint connected to MC4


typedef struct
{
    EOnvsCfg                        *nvsCfg;       /**<  Network Variable configuration          */
    eOnvEP_t                        mc_endpoint;   /**<  motion control endpoint managed by the application */
    eOnvEP_t                        as_endpoint;   /**<  analog sensor endpoint managed by the application */
    eOnvEP_t                        sk_endpoint;   /**<  analog sensor endpoint managed by the application */
    /* if one of endpoints is not present ==> then set endpoint to EOK_uint16dummy */
    const EOconstvector* const      canboardsList; /**< list of CAN boards connected to ems by can */
    const EOconstvector* const      jointsList;    /**< list of joints managed by an EMS board */
    const EOconstvector* const      motorsList;    /**< list of motors managed by an EMS board */
    const EOconstvector* const      snsrMaisList;   /**< list of sensors managed by an EMS board */
    const EOconstvector* const      snsrStrainList; /**< list of sensors managed by an EMS board */
    const EOconstvector* const      skinList;      /**< list of skin managed by an EMS board */
} eOappTheDB_cfg_t;


// typedef struct
// {
// 	struct
// 	{
// 		EOnvsCfg                        *nvsCfg;      /**<  Network Variable configuration          */
// 		eOnvEP_t                        mc_endpoint;   /**<  motion control endopoint managed by the application */
// 		eOnvEP_t                        as_endpoint;   /**<  analog sensor endopoint managed by the application */
// 		eOnvEP_t                        sk_endpoint;   /**<  analog sensor endopoint managed by the application */
// 	}nvsdata;
// 	
// 	struct
// 	{
// 		const EOconstvector* const      canboardsList; /**< list of CAN boars connected to ems by can */
// 		const EOconstvector* const      jointsList;    /**< list of joints managed by an EMS board */
// 		const EOconstvector* const      motorsList;    /**< list of motors managed by an EMS board */
// 		const EOconstvector* const      snrMaisList;   /**< list of sensors managed by an EMS board */
// 		const EOconstvector* const      snrStrainList; /**< list of sensors managed by an EMS board */
// 		const EOconstvector* const      skinList;      /**< list of skin managed by an EMS board */
// 	} connectedStuff;
// 	
// } eOappTheDB_cfg_t;

typedef struct
{
    eOcanport_t                             emscanport;
    icubCanProto_canBoardAddress_t        addr;
    eOicubCanProto_jm_indexinBoard_t        indexinboard;
} eOappTheDB_jointOrMotorCanLocation_t;

typedef enum 
{
    eo_appTheDB_sensortype_mais                 = 0,
    eo_appTheDB_sensortype_strain               = 1
} eOappTheDB_sensortype_t;


typedef eOappTheDB_canBoardCanLocation_t            eOappTheDB_SensorCanLocation_t;

typedef struct
{
    eOcanport_t                             emscanport;
} eOappTheDB_SkinCanLocation_t;


typedef struct EOappTheDB_hid                  EOappTheDB;


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOappTheDB* eo_appTheDB_Initialise(eOappTheDB_cfg_t *cfg);

extern EOappTheDB* eo_appTheDB_GetHandle(void);

//extern eOresult_t eo_appTheDB_SetjointsShiftValues(EOappTheDB *p, eOappTheDB_jointShiftValues_t *shiftval_ptr);

//extern eOresult_t eo_appTheDB_SetJointsBcastpolicyPtr(EOappTheDB *p, icubCanProto_bcastpolicy_t *bcast_ptr);

extern uint16_t eo_appTheDB_GetNumeberOfConnectedJoints(EOappTheDB *p);

extern uint16_t eo_appTheDB_GetNumeberOfConnectedMotors(EOappTheDB *p);

extern uint16_t eo_appTheDB_GetNumeberOfCanboards(EOappTheDB *p);

extern eObool_t	eo_appTheDB_isSkinConnected(EOappTheDB *p);

extern eOresult_t eo_appTheDB_GetTypeOfCanboard(EOappTheDB *p, eObrd_boardId_t bid, eObrd_types_t *type_ptr);

extern eOappEncReader_encoder_t eo_appTheDB_GetEncoderConnected2Joint(EOappTheDB *p, eOmc_jointId_t jId);

extern eOresult_t eo_appTheDB_GetJointId_ByJointCanLocation(EOappTheDB *p, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eOmc_jointId_t *jId_ptr);

extern eOresult_t eo_appTheDB_GetJointCanLocation(EOappTheDB *p, eOmc_jointId_t jId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eObrd_types_t *type_ptr);

extern eOresult_t eo_appTheDB_GetMotorCanLocation(EOappTheDB *p, eOmc_motorId_t mId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eObrd_types_t *type_ptr);

extern eOresult_t eo_appTheDB_GetMotorId_ByMotorCanLocation(EOappTheDB *p, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eOmc_motorId_t *mId_ptr);

extern eOresult_t eo_appTheDB_GetSnsrMaisId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_sensorCanLocation_t *canloc_ptr, eOas_maisId_t *sId_ptr);

extern eOresult_t eo_appTheDB_GetSnsrMaisCanLocation(EOappTheDB *p, eOas_maisId_t sId, eOappTheDB_sensorCanLocation_t *canloc_ptr);

extern eOresult_t eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_sensorCanLocation_t *canloc_ptr, eOas_strainId_t *sId_ptr);

extern eOresult_t eo_appTheDB_GetSnsrStrainCanLocation(EOappTheDB *p, eOas_strainId_t sId, eOappTheDB_sensorCanLocation_t *canloc_ptr);

extern eOresult_t eo_appTheDB_GetSkinCanLocation(EOappTheDB *p, eOsk_skinId_t skId, eOappTheDB_SkinCanLocation_t *canloc_ptr);

extern eOresult_t eo_appTheDB_GetCanBoardCfg(EOappTheDB *p, eObrd_boardId_t bid, eOappTheDB_cfg_canBoardInfo_t **cfg_canbrd_ptr);

extern eOresult_t eo_appTheDB_GetCanBoardId_ByCanLocation(EOappTheDB *p, eOappTheDB_canBoardCanLocation_t *canloc_ptr, eObrd_boardId_t *bid_ptr); 



extern eOresult_t eo_appTheDB_GetJointConfigPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_config_t **jconfig_ptr);
extern eOresult_t eo_appTheDB_GetJointStatusPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_status_t **jstatus_ptr);
extern eOresult_t eo_appTheDB_GetJointInputsPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_inputs_t **jinputs_ptr);
extern eOresult_t eo_appTheDB_GetJointCmdControlmodePtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_controlmode_t **jcmdcontrolmode_ptr);
extern eOresult_t eo_appTheDB_GetShiftValuesOfJointPtr(EOappTheDB *p, eOmc_jointId_t jId, eOappTheDB_jointShiftValues_t **shiftval_ptr);
extern eOresult_t eo_appTheDB_GetJointBcastpolicyPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOicubCanProto_bcastpolicy_t **bcast_ptr);
extern eOresult_t eo_appTheDB_SetJointCurrentControlmode(EOappTheDB *p, eOmc_jointId_t jId, eOmc_controlmode_t curr_mode);
extern eOresult_t eo_appTheDB_GetJointCurrentControlmode(EOappTheDB *p, eOmc_jointId_t jId, eOmc_controlmode_t *curr_mode_ptr);



extern eOresult_t eo_appTheDB_GetMotorConfigPtr(EOappTheDB *p, eOmc_motorId_t mId,  eOmc_motor_config_t **mconfig_ptr);
extern eOresult_t eo_appTheDB_GetMotorStatusPtr(EOappTheDB *p, eOmc_motorId_t mId,  eOmc_motor_status_t **mstatus_ptr);

extern eOresult_t eo_appTheDB_GetSnrMaisConfigPtr(EOappTheDB *p, eOas_maisId_t sId,  eOas_mais_config_t **sconfig_ptr);
extern eOresult_t eo_appTheDB_GetSnrMaisStatusPtr(EOappTheDB *p, eOas_maisId_t sId,  eOas_mais_status_t **sstatus_ptr);

extern eOresult_t eo_appTheDB_GetSnrStrainConfigPtr(EOappTheDB *p, eOas_strainId_t sId,  eOas_strain_config_t **sconfig_ptr);
extern eOresult_t eo_appTheDB_GetSnrStrainStatusPtr(EOappTheDB *p, eOas_strainId_t sId,  eOas_strain_status_t **sstatus_ptr);



// extern eOresult_t eo_appTheDB_GetSkinConfigPtr(EOappTheDB *p,eOsk_skinId_t sId,  eOsk_config_t **skconfig_ptr);
// extern eOresult_t eo_appTheDB_GetSkinStatusPtr(EOappTheDB *p,eOsk_skinId_t sId,  eOsk_status_t **skstatus_ptr);
extern eOresult_t eo_appTheDB_GetSkinCfgSigModePtr(EOappTheDB *p,eOsk_skinId_t skId,  eOsk_sigmode_t **sigmode_ptr);
extern eOresult_t eo_appTheDB_GetSkinStArray10CanFramesPtr(EOappTheDB *p,eOsk_skinId_t skId,  EOarray_of_10canframes **arrayof10canframes_ptr);



extern eOresult_t eo_appTheDB_GetVirtualStrainDataPtr(EOappTheDB *p, uint16_t **virtStrain_ptr);
extern uint8_t eo_appTheDB_IsVirtualStrainDataUpdated(EOappTheDB *p);
extern eOresult_t eo_appTheDB_SetVirtualStrainValue(EOappTheDB *p, eOmc_jointId_t jId, uint16_t torquevalue);

/** @}            
    end of group eo_app_encodersReader
 **/


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


