/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
#ifndef _EOTHEENCODERREADER_H_
#define _EOTHEENCODERREADER_H_

#include "embot_app_eth_Encoder.h"


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheEncoderReader.h
    @brief      this object implements what is required for managing the STRAIN.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheEncoderReader Object EOtheEncoderReader
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EOconstarray.h"    

#if defined(USE_EMBOT_theServices)  
//#include "embot_app_eth_Service.h"
#error dont use me
#else
#include "EOtheServices.h"
#endif



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheEncoderReader_hid EOtheEncoderReader;

// we keep eOencoderreader_errortype_t in here but dependent from embot::app::eth::encoder::v1::Error

typedef enum
{
    encreader_err_NONE                  = embot::core::tointegral(embot::app::eth::encoder::v1::Error::NONE),                   // 0
    encreader_err_AEA_READING           = embot::core::tointegral(embot::app::eth::encoder::v1::Error::AEA_READING),            // 1,
    encreader_err_AEA_PARITY            = embot::core::tointegral(embot::app::eth::encoder::v1::Error::AEA_PARITY),             // 2,
    encreader_err_AEA_CHIP              = embot::core::tointegral(embot::app::eth::encoder::v1::Error::AEA_CHIP),               // 3,
    encreader_err_QENC_GENERIC          = embot::core::tointegral(embot::app::eth::encoder::v1::Error::QENC_GENERIC),           // 4,
    encreader_err_ABSANALOG_GENERIC     = embot::core::tointegral(embot::app::eth::encoder::v1::Error::ABSANALOG_GENERIC),      // 5,
    encreader_err_MAIS_GENERIC          = embot::core::tointegral(embot::app::eth::encoder::v1::Error::MAIS_GENERIC),           // 6,
    encreader_err_SPICHAINOF2_GENERIC   = embot::core::tointegral(embot::app::eth::encoder::v1::Error::SPICHAINOF2_GENERIC),    // 7,
    encreader_err_SPICHAINOF3_GENERIC   = embot::core::tointegral(embot::app::eth::encoder::v1::Error::SPICHAINOF3_GENERIC),    // 8,
    encreader_err_AMO_GENERIC           = embot::core::tointegral(embot::app::eth::encoder::v1::Error::AMO_GENERIC),            // 9,
    encreader_err_PSC_GENERIC           = embot::core::tointegral(embot::app::eth::encoder::v1::Error::PSC_GENERIC),            // 10,
    encreader_err_POS_GENERIC           = embot::core::tointegral(embot::app::eth::encoder::v1::Error::POS_GENERIC),            // 11,
    encreader_err_GENERIC               = embot::core::tointegral(embot::app::eth::encoder::v1::Error::GENERIC),                // 14,
    encreader_err_NOTCONNECTED          = embot::core::tointegral(embot::app::eth::encoder::v1::Error::NOTCONNECTED),           // 15, 
    // marco.accame: any error beyond 15 cannot be managed by diagnostics messages because it will not fit inside a nibble
    encreader_err_AKSIM2_INVALID_DATA   = embot::core::tointegral(embot::app::eth::encoder::v1::Error::AKSIM2_INVALID_DATA),    // 16,
    encreader_err_AKSIM2_CLOSE_TO_LIMITS= embot::core::tointegral(embot::app::eth::encoder::v1::Error::AKSIM2_CLOSE_TO_LIMITS), // 17,
    encreader_err_AKSIM2_CRC_ERROR      = embot::core::tointegral(embot::app::eth::encoder::v1::Error::AKSIM2_CRC_ERROR),       // 18,
    encreader_err_AKSIM2_GENERIC        = embot::core::tointegral(embot::app::eth::encoder::v1::Error::AKSIM2_GENERIC)          // 19
} eOencoderreader_errortype_t;

typedef struct
{
    uint32_t                                value[eomc_encoders_maxnumberofcomponents];
    eOencoderreader_errortype_t             errortype;
    uint8_t                                 composedof;
    eOmc_position_t                         position;
} eOencoderreader_valueInfo_t;

typedef struct
{   // out = input * 1/fabs(scale) - offset
    float scale;
    int32_t offset;    
} eOencoderreader_Scaler;

typedef enum
{
    eo_encreader_pos_one = 0,
    eo_encreader_pos_two = 1,
    eo_encreader_pos_any = 14,
    eo_encreader_pos_none = 15   
} eOencoderreader_Position_t;    


// The following enum and structs are the correspondent to the the ones in 
// embot::app::eth::encoder::experimental in embot_app_eth_Encoder.h

// enum for the maximum number of jomo components
enum { eOencoderreader_maxnumberofencoderperjomo = 3 };

// raw data for single component, raw value and diagnostic
typedef struct
{
    int32_t val;
    int32_t diagnInfo;
} eOencoderreader_RawValueEncoder_t; 

// struct for raw data and validity flag for entire jomo
typedef struct
{
    eOencoderreader_RawValueEncoder_t rawvalues[eOencoderreader_maxnumberofencoderperjomo];
    bool areValidReadings[eOencoderreader_maxnumberofencoderperjomo];
} eOencoderreader_RawValuesOfJomo_t;
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern EOtheEncoderReader* eo_encoderreader_Initialise(void);

extern EOtheEncoderReader* eo_encoderreader_GetHandle(void);


// it verifies if the service as defined in the configuration is possible, it executes a callback
// (which may send a confirmation to the entity which asked fot verification), and then it may activate the service by calling  eo_encoderreader_Activate().
extern eOresult_t eo_encoderreader_Verify(EOtheEncoderReader *p, EOconstarray * jomodes, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify, eOmn_serv_diagn_cfg_t dc);

// it activates the service by loading the service configuration
extern eOresult_t eo_encoderreader_Activate(EOtheEncoderReader *p, EOconstarray * jomodes, eOmn_serv_diagn_cfg_t dc);

// it deactivates service.
extern eOresult_t eo_encoderreader_Deactivate(EOtheEncoderReader *p);

extern eOresult_t eo_encoderreader_SendReport(EOtheEncoderReader *p);

extern eOresult_t eo_encoderreader_StartReading(EOtheEncoderReader *p);

extern eObool_t eo_encoderreader_IsReadingAvailable(EOtheEncoderReader *p);

extern eOresult_t eo_encoderreader_Read(EOtheEncoderReader *p, uint8_t jomo, eOencoderreader_valueInfo_t *primary, eOencoderreader_valueInfo_t *secondary);

extern eOresult_t eo_encoderreader_Diagnostics_Tick(EOtheEncoderReader* p);

// position is only: 0 (primary) or 1 (secondary)
extern eOmc_encoder_t eo_encoderreader_GetType(EOtheEncoderReader* p, uint8_t jomo, eOencoderreader_Position_t position);

extern eOresult_t eo_encoderreader_Scale(EOtheEncoderReader* p, uint8_t jomo, eOencoderreader_Position_t position, eOencoderreader_Scaler *scaler);

// Correlated to IFreader
extern eOresult_t eo_encoderreader_GetRaw(EOtheEncoderReader *p, uint8_t jomo, eOencoderreader_RawValuesOfJomo_t *rawValuesArray);

/** @}            
    end of group eo_EOtheEncoderReader
 **/

 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


