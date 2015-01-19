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
#include "eOboards.h"
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoSkin.h"
#include "EOicubCanProto_specifications.h"

#ifdef USE_PROTO_PROXY
#include "EoProtocol.h"
#include "EOlist.h"
#endif

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { eOappTheDB_canboard_maxnumberof = 30, eOappTheDB_canboard_none = 31 };

enum { eOappTheDB_canboard_noindex = 3 };

typedef struct
{
    uint8_t indexofcanboard : 6;            /**< use values from 0 to eOappTheDB_canboard_maxnumberof */
    uint8_t indexinsidecanboard : 2;        /**< use eOicubCanProto_jm_indexinBoard_t or eOappTheDB_canboard_noindex*/
} eOappTheDB_mapping2canboard_t; 



typedef struct
{
    uint8_t emscanport : 1;                 /**< use eOcanport_t */
    uint8_t addr : 4;                       /**< use icubCanProto_canBoardAddress_t */ 
    uint8_t unused : 3;
} eOappTheDB_board_canlocation_t; //EO_VERIFYsizeof(eOappTheDB_board_canlocation_t, 1);



typedef struct
{
    uint8_t port : 1;               /**< use eOcanport_t */
    uint8_t addr : 4;               /**< use icubCanProto_canBoardAddress_t */ 
    uint8_t type : 3;               /**< use eObrd_cantypes_t */
} eOappTheDB_canboardinfo_t;        //EO_VERIFYsizeof(eOappTheDB_canboardinfo_t, 1);


typedef struct
{   //  experimental: not used for far
    const eOappTheDB_canboardinfo_t*  brd;
    uint8_t indexofcanboard : 6;            /**< use values from 0 to eOappTheDB_canboard_maxnumberof */
    uint8_t indexinsidecanboard : 2;        /**< use eOicubCanProto_jm_indexinBoard_t or eOappTheDB_canboard_noindex*/
} eOappTheDB_mapping2canboard2_t;

typedef struct
{
    eOcanport_t                               connected2emsport;
    uint8_t                                   boardAddrStart;
    uint8_t                                   numofboards;
} eOappTheDB_cfg_skinInfo_t;


typedef struct
{
    uint8_t     jointVelocityShift;
    uint8_t     jointVelocityEstimationShift;
    uint8_t     jointAccelerationEstimationShift;
    uint8_t     dummy;
    /* put here other shift values like: 
            uint8_t     motorVelocityEstimationShift;
            uint8_t     motorAccelerationEstimationShift;
            uint8_t     jointVelocityEstimationShift;
            
    */
} eOappTheDB_jointShiftValues_t;       //thi values are usefull only for joint connected to MC4

#ifdef USE_PROTO_PROXY
typedef struct
{
    eOprotID32_t    id32;
    void            *nvRam_ptr;
    uint8_t         numOfExpectedResp;
    uint8_t         numOfREceivedResp;
} eOappTheDB_hid_ethProtoRequest_t;
#endif

typedef struct
{
    const EOconstvector* const      canboardsList;   /**< list of CAN boards connected to ems by can */
    const EOconstvector* const      jointsList;      /**< list of joints managed by an EMS board */
    const EOconstvector* const      motorsList;      /**< list of motors managed by an EMS board */
    const EOconstvector* const      snsrMaisList;    /**< list of sensors managed by an EMS board */
    const EOconstvector* const      snsrStrainList;  /**< list of sensors managed by an EMS board */
    const EOconstvector* const      skinList;        /**< list of skin managed by an EMS board */
} eOappTheDB_cfg_t;


typedef struct
{
    eOcanport_t                             emscanport;
    icubCanProto_canBoardAddress_t          addr;
    eOicubCanProto_jm_indexinBoard_t        indexinsidecanboard;
} eOappTheDB_jointOrMotorCanLocation_t;


typedef enum 
{
    eo_appTheDB_sensortype_mais                 = 0,
    eo_appTheDB_sensortype_strain               = 1
} eOappTheDB_sensortype_t;



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


extern uint16_t eo_appTheDB_GetNumberOfConnectedJoints(EOappTheDB *p);

// ok, ma can we get this number from the protocol ????
extern uint16_t eo_appTheDB_GetNumberOfConnectedMotors(EOappTheDB *p);

extern uint16_t eo_appTheDB_GetNumberOfCanboards(EOappTheDB *p);

extern eObool_t	eo_appTheDB_isSkinConnected(EOappTheDB *p);


extern eOresult_t eo_appTheDB_GetTypeOfCanboard(EOappTheDB *p, eObrd_boardId_t bid, eObrd_cantype_t *type_ptr);

//extern eOappEncReader_encoder_t eo_appTheDB_GetEncoderConnected2Joint(EOappTheDB *p, eOmc_jointId_t jId);

extern eOresult_t eo_appTheDB_GetJointId_ByJointCanLocation(EOappTheDB *p, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eOmc_jointId_t *jId_ptr);

extern eOresult_t eo_appTheDB_GetJointCanLocation(EOappTheDB *p, eOmc_jointId_t jId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eObrd_cantype_t *type_ptr);

extern eOresult_t eo_appTheDB_GetMotorCanLocation(EOappTheDB *p, eOmc_motorId_t mId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eObrd_cantype_t *type_ptr);

extern eOresult_t eo_appTheDB_GetMotorId_ByMotorCanLocation(EOappTheDB *p, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eOmc_motorId_t *mId_ptr);

extern eOresult_t eo_appTheDB_GetSnsrMaisId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_board_canlocation_t *canloc_ptr, eOas_maisId_t *sId_ptr);

extern eOresult_t eo_appTheDB_GetSnsrMaisCanLocation(EOappTheDB *p, eOas_maisId_t sId, eOappTheDB_board_canlocation_t *canloc_ptr);

extern eOresult_t eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_board_canlocation_t *canloc_ptr, eOas_strainId_t *sId_ptr);

extern eOresult_t eo_appTheDB_GetSnsrStrainCanLocation(EOappTheDB *p, eOas_strainId_t sId, eOappTheDB_board_canlocation_t *canloc_ptr);

extern eOresult_t eo_appTheDB_GetSkinCanLocation(EOappTheDB *p, eOsk_skinId_t skId, eOappTheDB_SkinCanLocation_t *canloc_ptr);

extern eOresult_t eo_appTheDB_GetSkinId_BySkinCanLocation(EOappTheDB *p, eOappTheDB_SkinCanLocation_t *canloc_ptr, eOsk_skinId_t *skId_ptr);

extern eOresult_t eo_appTheDB_GetCanBoardInfo(EOappTheDB *p, eObrd_boardId_t bid,  eOappTheDB_canboardinfo_t **ppcanboardinfo);

extern eOresult_t eo_appTheDB_GetCanBoardId_ByCanLocation(EOappTheDB *p, eOappTheDB_board_canlocation_t *canloc_ptr, eObrd_boardId_t *bid_ptr); 


extern eOresult_t eo_appTheDB_GetShiftValuesOfJointPtr(EOappTheDB *p, eOmc_jointId_t jId, eOappTheDB_jointShiftValues_t **shiftval_ptr);
extern eOresult_t eo_appTheDB_GetJointBcastpolicyPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOicubCanProto_bcastpolicy_t **bcast_ptr);







extern eOresult_t eo_appTheDB_GetSkinConfigPtr(EOappTheDB *p,eOsk_skinId_t sId,  eOappTheDB_cfg_skinInfo_t **skconfig_ptr);



extern eOresult_t eo_appTheDB_GetVirtualStrainDataPtr(EOappTheDB *p, uint16_t **virtStrain_ptr);
extern uint8_t eo_appTheDB_IsVirtualStrainDataUpdated(EOappTheDB *p);
extern void eo_appTheDB_ClearVirtualStrainDataUpdatedFlag(EOappTheDB *p);
extern eOresult_t eo_appTheDB_SetVirtualStrainValue(EOappTheDB *p, eOmc_jointId_t jId, uint16_t torquevalue);


extern eOresult_t eo_appTheDB_setCanBoardReady(EOappTheDB *p, eOappTheDB_board_canlocation_t *canloc_ptr);
extern eObool_t  eo_appTheDB_areConnectedCanBoardsReady(EOappTheDB *p, uint32_t *canBoardsReady);

#ifdef USE_PROTO_PROXY
//eth proto request
extern eOresult_t eo_appTheDB_appendEthProtoRequest(EOappTheDB *p, eOprotEntity_t entity, eOprotIndex_t index, eOappTheDB_hid_ethProtoRequest_t *req);
extern EOlistIter * eo_appTheDB_searchEthProtoRequest(EOappTheDB *p, eOprotID32_t id32);
extern eOresult_t eo_appTheDB_removeEthProtoRequest(EOappTheDB *p, eOprotEntity_t entity, eOprotIndex_t index, EOlistIter* li);
#endif

/** @}            
    end of group eo_app_encodersReader
 **/


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


