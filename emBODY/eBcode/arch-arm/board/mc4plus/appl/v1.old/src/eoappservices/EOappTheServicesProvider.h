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
#ifndef _EOAPPTHESERVICESPROVIDER_H_
#define _EOAPPTHESERVICESPROVIDER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOappTheServicesProvider.h
    @brief     This file provides interface to the singleton services provider.
    @author    valentina.gaggero@iit.it
    @date       07/16/2012
**/
/** @defgroup appTheSP Object EOappTheSP
    .........     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "eOcommon.h"
#include "EOnv.h"

//services
#include "EOappCanServicesProvider.h"
#include "EOappTheCanBoardsManager.h"
#include "EOappTheNVmapRef-NEW.h"
#include "EOappEncodersReader.h"
#include "EOemsController.h"
#include "EOappMeasuresConverter.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOappTheSP_hid  EOappTheSP;


typedef struct
{
    uint16_t                    connectedEncodersMask;
    struct
    {
        uint8_t                 numofmotors;
        emsBoardType_t          emsboard_type;
    }emsControllerCfg;
    
    eOnvEP_t                 mc_endpoint;  /**<  motion control endopoint managed by the application    */
    eOnvEP_t                 as_endpoint;  /**<  analog sensor endopoint managed by the application    */
    eOnvEP_t                 sk_endpoint;  /**<  skin endopoint managed by the application    */
    eOappMeasConv_cfg_t      measConvCfg;  /**<  configuration of mesures converter    */
} eOappTheSP_cfg_t;

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOappTheSP* eo_appTheSP_Initialise(const eOappTheSP_cfg_t *cfg);

extern EOappTheSP* eo_appTheSP_GetHandle(void);

extern EOappCanSP* eo_appTheSP_GetCanServiceHandle(EOappTheSP *p);
    
extern EOappTheCanBrdsMng* eo_appTheSP_GetTheCanBrdsMngHandle(EOappTheSP *p);
    
extern EOappTheNVmapRef* eo_appTheSP_GetTheNVmapRefHandle(EOappTheSP *p);

extern EOappEncReader* eo_appTheSP_GetEncoderReaderHandle(EOappTheSP *p);

extern EOemsController* eo_appTheSP_GetEmsControllerHandle(EOappTheSP *p);

extern eOmn_appl_runMode_t eo_appTheSP_GetAppRunMode(EOappTheSP *p);

extern EOappMeasConv* eo_appTheSP_GetMeasuresConverterHandle(EOappTheSP *p);
/** @}            
    end of group appTheSP
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


