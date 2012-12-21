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
#include "EOnv.h"
#include "EoManagement.h"

//objs of app body
#include "EOappTheDataBase.h"
#include "EOappCanServicesProvider.h"
#include "EOappEncodersReader.h"
#include "EOemsController.h"
#include "EOappMeasuresConverter.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheEMSapplBody_hid  EOtheEMSapplBody;


typedef struct
{
    eOicubCanProto_protocolVersion_t    icubcanprotoimplementedversion;
    uint16_t                            connectedEncodersMask;
    struct
    {
        emsBoardType_t                  emsboard_type;
    }emsControllerCfg;
    struct
    {
        eOnvEP_t                        mc_endpoint;  /**<  motion control endopoint managed by the application    */
        eOnvEP_t                        as_endpoint;  /**<  analog sensor endopoint managed by the application    */
        eOnvEP_t                        sk_endpoint;  /**<  skin endopoint managed by the application    */

    } endpoints;
    
    struct
    {
        struct
        {
            uint8_t                     jointVelocityShift;
            uint8_t                     jointVelocityEstimationShift;
            uint8_t                     jointAccelerationEstimationShift;
        } shiftvalues;       /**<  values of shifts to send to motor can board (MC4 only, because 2foc use a different way to coding data)    */  
        
        struct
        {
            uint8_t                     val2bcastList[4];
        } bcastpolicy;       /**<  broadcast policy to set to motor can boards (MC4 only, because 2FOC don't use icubCanProto)    */
    } configdataofMC4boards; /**<  in actual fact, all config data of mc4 boards are about joints: they should be one for each joint managed by mc4 board;
                                   anyway these data are not configured by pc104, so we use one data for any joint.    */

} eOtheEMSappBody_cfg_t;

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheEMSapplBody* eo_emsapplBody_Initialise(const eOtheEMSappBody_cfg_t *cfg);

extern EOtheEMSapplBody* eo_emsapplBody_GetHandle(void);

extern const eOtheEMSappBody_cfg_t* eo_emsapplBody_GetConfig(EOtheEMSapplBody *p);

extern EOappTheDB* eo_emsapplBody_GetDataBaseHandle(EOtheEMSapplBody *p);

extern EOappCanSP* eo_emsapplBody_GetCanServiceHandle(EOtheEMSapplBody *p);
    
extern EOappEncReader* eo_emsapplBody_GetEncoderReaderHandle(EOtheEMSapplBody *p);

extern EOemsController* eo_emsapplBody_GetEmsControllerHandle(EOtheEMSapplBody *p);

extern eOmn_appl_runMode_t eo_emsapplBody_GetAppRunMode(EOtheEMSapplBody *p);

extern EOappMeasConv* eo_emsapplBody_GetMeasuresConverterHandle(EOtheEMSapplBody *p);

extern eOresult_t eo_emsapplBody_EnableTxAllJointOnCan(EOtheEMSapplBody *p);

extern eOresult_t eo_emsapplBody_DisableTxAllJointOnCan(EOtheEMSapplBody *p);

/** @}            
    end of group eo_TheEmsAppBody
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


