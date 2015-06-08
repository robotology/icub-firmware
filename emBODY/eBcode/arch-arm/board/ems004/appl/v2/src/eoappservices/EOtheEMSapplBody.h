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

/** @defgroup eo_TheEmsAppBody Object eOTheemapplBody
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoManagement.h"

#include "EoBoards.h"
#include "EOappEncodersReader.h"
#include "EOemsController.h"
#include "EOtheMeasuresConverter.h"

#include "EOtheCANmapping.h"


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



// values of shifts to send to motor can board (MC4 only, because 2foc use a different way to coding data) 
typedef struct
{
    uint8_t             jointVelocityShift;
    uint8_t             jointVelocityEstimationShift;
    uint8_t             jointAccelerationEstimationShift;
} eo_emsapplbody_can_shiftvalues_t;   

// broadcast policy to set to motor can boards (MC4 only, because 2FOC don't use icubCanProto)  
enum {eoemsapplbody_bcastpolicylistsize = 4};
typedef struct
{
    uint8_t             val2bcastList[eoemsapplbody_bcastpolicylistsize];
} eo_emsapplbody_can_bcastpolicy_t;       
    




typedef struct
{
    eo_emsapplbody_can_shiftvalues_t    shiftvalues;    
    eo_emsapplbody_can_bcastpolicy_t    bcastpolicy;      
} eo_emsapplbody_configMC4boards_t;
    

typedef struct 
{
    uint8_t major;
    uint8_t minor;
} eOtmp_protocolVersion_t;

typedef struct
{
    uint8_t estimShiftJointVel;
    uint8_t estimShiftJointAcc;
    uint8_t estimShiftMotorVel;
    uint8_t estimShiftMotorAcc;
} eOtmp_estimShift_t;

typedef struct
{
    eOappEncReader_stream_t             encoderstreams[eo_appEncReader_streams_numberof];
    eo_emsapplbody_configMC4boards_t    configdataofMC4boards;
} eOemsapplbody_cfg_t;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheEMSapplBody* eo_emsapplBody_Initialise(const eOemsapplbody_cfg_t *cfg);


extern EOtheEMSapplBody* eo_emsapplBody_GetHandle(void);

extern const eOemsapplbody_cfg_t* eo_emsapplBody_GetConfig(EOtheEMSapplBody *p);
   
extern EOappEncReader* eo_emsapplBody_GetEncoderReader(EOtheEMSapplBody *p);

extern eOmn_appl_runMode_t eo_emsapplBody_GetAppRunMode(EOtheEMSapplBody *p);


extern eOresult_t eo_emsapplBody_EnableTxAllJointOnCan(EOtheEMSapplBody *p);

extern eOresult_t eo_emsapplBody_DisableTxAllJointOnCan(EOtheEMSapplBody *p);


extern eOmn_appl_runMode_t eo_emsapplBody_GetAppRunMode(EOtheEMSapplBody *p);


extern eOresult_t eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p);


/** @}            
    end of group eo_TheEmsAppBody
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


