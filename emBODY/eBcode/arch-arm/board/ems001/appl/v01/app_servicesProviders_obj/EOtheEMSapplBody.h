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
#ifndef _EOTHEEMASAPPLBODY_H_
#define _EOTHEEMASAPPLBODY_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheEMSApplBody.h
    @brief      This file provides interface to ems application body: it let acess to all obj 
                that form application body.
    @author     valentina.gaggero@iit.it
    @date       09/19/2012
**/

/** @defgroup eo_TheemapplBody Object eOTheemapplBody
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "eOcommon.h"
#include "EoManagement.h"

#include "hal_encoder.h"


#include "EOappTheDataBase.h"
#include "EOappCanServicesProvider.h"
#include "EOappEncodersReader.h"
#include "EOemsController.h"
#include "EOappMeasuresConverter.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheEMSapplBody_hid  EOtheEMSapplBody;

typedef enum
{
    eo_emsapplbody_deviceid_skin        = 0,
    eo_emsapplbody_deviceid_mc4         = 1,
    eo_emsapplbody_deviceid_2foc        = 2    
} eo_emsapplbody_deviceid_t;

enum {eo_emsapplbody_deviceid_numberof = 3};


/** @typedef    struct eo_emsapplbody_encoder_stream_t
    @brief      contains representation of a stream of hal encoders. they all must be of the same type.
 **/
typedef struct 
{
    hal_encoder_type_t      type;                           /**< the type of encoders. they must be all homogeneous */
    uint8_t                 numberof;                       /**< their number inside encoders[] */
    hal_encoder_t           encoders[hal_encoders_number];  /**< the IDs of the encoders belonging to the stream without holes in the array */    
} eo_emsapplbody_encoder_stream_t;


// values of shifts to send to motor can board (MC4 only, because 2foc use a different way to coding data) 
typedef struct
{
    uint8_t             jointVelocityShift;
    uint8_t             jointVelocityEstimationShift;
    uint8_t             jointAccelerationEstimationShift;
} eo_emsapplbody_can_shiftvalues_t;   

// broadcast policy to set to motor can boards (MC4 only, because 2FOC don't use icubCanProto)  
typedef struct
{
    uint8_t             val2bcastList[4];
} eo_emsapplbody_can_bcastpolicy_t;       
    


// marco.accame: comment by valentina.gaggero referred to the following struct, which earlier was un-named and inside eOemsapplbody_cfg_t
// -->
// in actual fact, all config data of mc4 boards are about joints: they should be one for each joint managed by mc4 board;
// anyway these data are not configured by pc104, so we use one data for any joint. 

typedef struct
{
    eo_emsapplbody_can_shiftvalues_t    shiftvalues;    /**< for mc4 */    
    eo_emsapplbody_can_bcastpolicy_t    bcastpolicy;      
} eo_emsapplbody_configMC4boards_t;
    


typedef struct
{
    eo_emsapplbody_encoder_stream_t     encoderstream0;
    eo_emsapplbody_encoder_stream_t     encoderstream1;
    eObool_t                            hasdevice[eo_emsapplbody_deviceid_numberof];
    eOicubCanProto_protocolVersion_t    icubcanprotoimplementedversion;
    uint16_t                            connectedEncodersMask;    
    eo_emsapplbody_configMC4boards_t    configdataofMC4boards;
} eOemsapplbody_cfg_t;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const eOemsapplbody_cfg_t eo_emsapplbody_cfg_default;

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheEMSapplBody* eo_emsapplBody_Initialise(const eOemsapplbody_cfg_t *cfg);


extern EOtheEMSapplBody* eo_emsapplBody_GetHandle(void);

extern const eOemsapplbody_cfg_t* eo_emsapplBody_GetConfig(EOtheEMSapplBody *p);

extern EOappTheDB* eo_emsapplBody_GetDataBaseHandle(EOtheEMSapplBody *p);

extern EOappCanSP* eo_emsapplBody_GetCanServiceHandle(EOtheEMSapplBody *p);
    
extern EOappEncReader* eo_emsapplBody_GetEncoderReaderHandle(EOtheEMSapplBody *p);

//extern EOemsController* eo_emsapplBody_GetEmsControllerHandle(EOtheEMSapplBody *p);

extern eOmn_appl_runMode_t eo_emsapplBody_GetAppRunMode(EOtheEMSapplBody *p);

extern EOappMeasConv* eo_emsapplBody_GetMeasuresConverterHandle(EOtheEMSapplBody *p);

extern eOresult_t eo_emsapplBody_EnableTxAllJointOnCan(EOtheEMSapplBody *p);

extern eOresult_t eo_emsapplBody_DisableTxAllJointOnCan(EOtheEMSapplBody *p);

extern eObool_t eo_emsapplBody_HasDevice(EOtheEMSapplBody *p, eo_emsapplbody_deviceid_t dev);

extern eOresult_t eo_emsapplBody_checkCanBoardsAreReady(EOtheEMSapplBody *p, uint32_t canBoardsMask);

extern eOresult_t eo_emsapplBody_setCanBoardsAreReady(EOtheEMSapplBody *p);

//Note: eo_emsapplBody_sendConfig2canboards function send only one time the configuration; so the second call of this func will send anything!
extern eOresult_t eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p);

extern eOresult_t eo_emsapplBody_StopSkin(EOtheEMSapplBody *p);


/** @}            
    end of group eo_TheEmsAppBody
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


