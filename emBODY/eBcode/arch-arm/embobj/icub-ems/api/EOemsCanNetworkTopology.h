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
#ifndef _EOEMSCANNETWORKTOPOLOGY_H_
#define _EOEMSCANNETWORKTOPOLOGY_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EoemsCanNetworkTopology.h
	@brief      This header file gives 
	@author     valentina.gagger@iit.it
	@date       03/08/2012
**/

/** @defgroup eo_emsCanNetTopo Object EOemsCanNetTopo
    under costruction......
   
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EoUtilities.h"
#include "EoBoards.h"
#include "EoMotionControl.h"
#include "EoSensors.h"
#include "EoSkin.h"
#include "EOconstvector.h"
#include "EOarray.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOemsCanNetTopo_hid EOemsCanNetTopo
    @brief      .....
 **/  
typedef struct EOemsCanNetTopo_hid EOemsCanNetTopo;


//indice del joint o del motor interno alla board. ogni board gestisce al piu' due motori/joint
typedef enum 
{
    eo_emsCanNetTopo_jm_index_first = 0,
    eo_emsCanNetTopo_jm_index_second = 1
} eo_emsCanNetTopo_jm_indexInBoard; 

EO_VERIFYproposition(isjm_indexInBoard_Compatiblewithicubcanproto, (eo_emsCanNetTopo_jm_index_first == 0));
EO_VERIFYproposition(isjm_indexInBoard_Compatiblewithicubcanproto, (eo_emsCanNetTopo_jm_index_second == 1));



enum {eo_emsCanNetTopo_jm_indexInBoard_max = 2};
/** @typedef    typedef     struct             eo_emsCanNetTopo_jointOrMotorLocation_t
    @brief      contains information to identify a joint or a motor managed by an ems univocally on that ems.
 **/
typedef struct
{
    eOcanport_t                         emscanport;	
    uint8_t                             canaddr;        /**< CAN address of board liked with joint/motor.
                                                            if this struct is used with icubCanProtocol canaddr's values must be complient with eo_icubCanProto_canBoardAddress_t*/
    eo_emsCanNetTopo_jm_indexInBoard    jm_idInBoard;	        /**< number of joint/ motor, said axis generally.
                                             if this struct is used with icubCanProtocol axis's values must be complient with eo_icubCanProto_motorAxis_t*/
} eo_emsCanNetTopo_jointOrMotorCanLocation_t;


/** @typedef    typedef     struct             eo_emsCanNetTopo_jointOrMotorLocation_t
    @brief      contains information to identify a joint or a motor managed by an ems univocally on that ems.
 **/
typedef struct
{
    eOcanport_t         emscanport;	
    uint8_t             canaddr;        /**< CAN address of board liked with joint/motor.
                                             if this struct is used with icubCanProtocol canaddr's values must be complient with eo_icubCanProto_canBoardAddress_t*/
} eo_emsCanNetTopo_sensorCanLocation_t;


/** @typedef    typedef     struct             eo_emsCanNetTopo_cfg_t
    @brief      contains data to configure an EOemsCanNetTopo object.
 **/
typedef struct
{
//    EOnvsCfg                           *nvsCfg;
//    const EOconstLookupTbl /*const*/  *joint2BoardCanLocation_LUTbl_ptr;
//    const EOconstLookupTbl* const   *motorBoardCanLoc2NvsRef_LUTbl_ptr; //can points to the same datastruct
    //if you need, put here other stuff 


    const EOconstvector* const emsCanNetTopo_joints__ptr;  /**< list of joints managed by an EMS board */
    const EOconstvector* const emsCanNetTopo_motors__ptr;  /**< list of motors managed by an EMS board */
    const EOconstvector* const emsCanNetTopo_sensors__ptr; /**< list of sensors managed by an EMS board */
    const EOconstvector* const emsCanNetTopo_skin__ptr;    /**< list of skin managed by an EMS board */
    const EOconstvector* const emsCanNetTopo_canBoards__ptr; /**< list of CAN boars connected to ems by can */
} eo_emsCanNetTopo_cfg_t;


/** @typedef    enum { eo_emsCanNetTopo_canports_num = 2 }
    @brief      contains the number of can port on ems board.
    @warning    pay attention to eOcanport_t define d in eOcommon.h
 **/
enum { eo_emsCanNetTopo_canports_num = 2 }; 

// typedef eOcfg_nvsEP_mc_jointNumber_t eo_emsCanNetTopo_jointNumber_t;
// typedef eOcfg_nvsEP_mc_motorNumber_t eo_emsCanNetTopo_motorNumber_t;
typedef uint16_t                     eo_emsCanNetTopo_boardId_t;



typedef struct
{
    uint8_t                         canaddr;
    eOcanport_t                     emscanport;
    eObrd_types_t                   boardtype;
    eo_emsCanNetTopo_boardId_t      bid;
} eo_emsCanNetTopo_boardTopoInfo_t; 


typedef struct
{
    eo_emsCanNetTopo_jm_indexInBoard    j_idInBoard;
    eo_emsCanNetTopo_boardId_t          bid;
    eOmc_jointId_t                      jid;  
} eo_emsCanNetTopo_jointTopoInfo_t;



typedef struct
{
    eo_emsCanNetTopo_jm_indexInBoard    m_idInBoard;
    eo_emsCanNetTopo_boardId_t          bid;
    eOmc_motorId_t                      mid;  
} eo_emsCanNetTopo_motorTopoInfo_t;


typedef struct
{
    eo_emsCanNetTopo_boardId_t      bid;
    eOsnsr_sensorId_t               sid;
} eo_emsCanNetTopo_sensorTopoInfo_t;






// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
//currently does not exist default config.
//extern const eo_icubCanProto_cfg_t eo_icubCanProto_cfg_default; 

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOemsCanNetTopo* eo_emsCanNetTopo_New(eo_emsCanNetTopo_cfg_t *cfg);

extern eOresult_t eo_emsCanNetTopo_GetJointCanLocation_ByJointId(EOemsCanNetTopo *p, eOmc_jointId_t jId, 
                                                                        eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eObrd_types_t *boardType);

extern eOresult_t eo_emsCanNetTopo_GetMotorCanLocation_ByMotorId(EOemsCanNetTopo *p, eOmc_motorId_t mId, 
                                                                        eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eObrd_types_t *boardType);


extern eOresult_t eo_emsCanNetTopo_GetSensorCanLocation_BySensorId(EOemsCanNetTopo *p, eOsnsr_sensorId_t sId, 
                                                                        eo_emsCanNetTopo_sensorCanLocation_t *location_ptr, eObrd_types_t *boardType);

extern eOresult_t eo_emsCanNetTopo_GetskinCanLocation_BySkinId(EOemsCanNetTopo *p, eOsk_skinId_t skId, eo_emsCanNetTopo_sensorCanLocation_t *location_ptr);



extern eOresult_t eo_emsCanNetTopo_GetJointId_ByJointCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_jointId_t *jId_ptr);

extern eOresult_t eo_emsCanNetTopo_GetMotorId_ByMotorCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_motorId_t *mId_ptr);

extern eOresult_t eo_emsCanNetTopo_GetSensorId_BySensorCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_sensorCanLocation_t *location_ptr, eOsnsr_sensorId_t *sId_ptr);

/* NOTE: these fn eo_emsCanNetTopo_GetConnectedSomething... return nok if one of arg is null, else ok. in case of no "somethig" is connected the fifo size is 0*/
extern eOresult_t eo_emsCanNetTopo_GetConnectedJoints(EOemsCanNetTopo *p, EOarray *connectedJointsList);
extern eOresult_t eo_emsCanNetTopo_GetConnectedMotors(EOemsCanNetTopo *p, EOarray *connectedMotorsList);
extern eOresult_t eo_emsCanNetTopo_GetConnectedSensors(EOemsCanNetTopo *p, EOarray *connectedSensorsList);
extern eOresult_t eo_emsCanNetTopo_GetConnectedSkin(EOemsCanNetTopo *p, EOarray *connectedSkinList);

extern const EOconstvector* eo_emsCanNetTopo_GetCfgCanBoards(EOemsCanNetTopo *p);

/*VALE: aggiungi funzione che dato joint id ritorna il tipo di board che lo gestisce! idem per il kotor. puo' venire utile??*/

// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group eo_emsCanNetTopo  
 **/

 
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


