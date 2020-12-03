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

#ifdef __cplusplus
extern "C" {
#endif

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
#include "hal_spiencoder.h"

#include "EOtheServices.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheEncoderReader_hid EOtheEncoderReader;



typedef enum
{
    encreader_err_NONE                  = 0,
    encreader_err_AEA_READING           = 1,
    encreader_err_AEA_PARITY            = 2,
    encreader_err_AEA_CHIP              = 3,
    encreader_err_QENC_GENERIC          = 4,
    encreader_err_ABSANALOG_GENERIC     = 5,
    encreader_err_MAIS_GENERIC          = 6,
    encreader_err_SPICHAINOF2_GENERIC   = 7,
    encreader_err_SPICHAINOF3_GENERIC   = 8,
    encreader_err_AMO_GENERIC           = 9,
    encreader_err_PSC_GENERIC           = 10,  
    encreader_err_POS_GENERIC           = 11,    
    encreader_err_GENERIC               = 14,    
    encreader_err_NOTCONNECTED          = 15 /* this error happens when the encoder type is none or encoder is not local, for example it is connected to 2foc board */
} eOencoderreader_errortype_t;

typedef struct
{
    uint32_t                                value[eomc_encoders_maxnumberofcomponents];
    eOencoderreader_errortype_t             errortype;
    uint8_t                                 composedof;
    eOmc_position_t                         position;
} eOencoderreader_valueInfo_t;

//typedef eOresult_t (*eOencoderreader_onendofoperation_fun_t) (EOtheEncoderReader* p, eObool_t operationisok);


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern EOtheEncoderReader* eo_encoderreader_Initialise(void);

extern EOtheEncoderReader* eo_encoderreader_GetHandle(void);


// it verifies if the service as defined in te configuration is possible (is there a good strain board or not?), it executes a callback
// (which may send a confirmation to the entity which asked fot verification), and then it may activate the strain service by calling  eo_encoderreader_Activate().
extern eOresult_t eo_encoderreader_Verify(EOtheEncoderReader *p, const eOmc_arrayof_4jomodescriptors_t * jomodes, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

// it activates the service by loading the service configuration
extern eOresult_t eo_encoderreader_Activate(EOtheEncoderReader *p, const eOmc_arrayof_4jomodescriptors_t * jomodes);

// it deactivates service.
extern eOresult_t eo_encoderreader_Deactivate(EOtheEncoderReader *p);

extern eOresult_t eo_encoderreader_SendReport(EOtheEncoderReader *p);

extern eOresult_t eo_encoderreader_StartReading(EOtheEncoderReader *p);

extern eObool_t eo_encoderreader_IsReadingAvailable(EOtheEncoderReader *p);

extern eOresult_t eo_encoderreader_Read(EOtheEncoderReader *p, uint8_t position, eOencoderreader_valueInfo_t *primary, eOencoderreader_valueInfo_t *secondary);

extern eOresult_t eo_encoderreader_Diagnostics_Tick(EOtheEncoderReader* p);

extern eOresult_t eo_encoderreader_GetPrimaryEncoder(EOtheEncoderReader *p, uint8_t position, eOmc_encoder_descriptor_t *encoder);


/** @}            
    end of group eo_EOtheEncoderReader
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


