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
#include "EOconstvector.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOemsCanNetTopo_hid EOemsCanNetTopo
    @brief      .....
 **/  
typedef struct EOemsCanNetTopo_hid EOemsCanNetTopo;


/** @typedef    typedef     struct             eo_emsCanNetTopo_jointOrMotorLocation_t
    @brief      contains information to identify a joint or a motor managed by an ems univocally on that ems.
 **/
typedef struct
{
    eOcanport_t         emscanport;	
    uint8_t             canaddr;        /**< CAN address of board liked with joint/motor.
                                             if this struct is used with icubCanProtocol canaddr's values must be complient with eo_icubCanProto_canBoardAddress_t*/
    uint8_t             axis;	        /**< number of joint/ motor, said axis generally.
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
} eo_emsCanNetTopo_cfg_t;


/** @typedef    enum { eo_emsCanNetTopo_canports_num = 2 }
    @brief      contains the number of can port on ems board.
    @warning    pay attention to eOcanport_t define d in eOcommon.h
 **/
enum { eo_emsCanNetTopo_canports_num = 2 }; 
 
 
    
typedef struct
{
    uint8_t                 boardAddr;
    uint8_t                 axis;
    eOcanport_t             canPort;
    eObrd_types_t           boardType;
    uint32_t                id; //id del motore o del joint
} eo_emsCanNetTopo_jointOrMotorTopoInfo_t;

typedef struct
{
    uint8_t                 boardAddr;
    eOcanport_t             canPort;
    uint8_t                 boardType;
    uint32_t                id; //sensor id
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



extern eOresult_t eo_emsCanNetTopo_GetJointId_ByJointCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_jointId_t *jId_ptr);

extern eOresult_t eo_emsCanNetTopo_GetMotorId_ByMotorCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_motorId_t *mId_ptr);

extern eOresult_t eo_emsCanNetTopo_GetSensorId_ByMotorCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_sensorCanLocation_t *location_ptr, eOsnsr_sensorId_t *sId_ptr);

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


