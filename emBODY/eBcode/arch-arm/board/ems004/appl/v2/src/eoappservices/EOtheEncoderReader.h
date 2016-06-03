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

#include "hal_spiencoder.h"

#include "EOtheServices.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheEncoderReader_hid EOtheEncoderReader;



typedef enum
{
    encreader_err_NONE      = 0,
    encreader_err_READING   = 1,
    encreader_err_PARITY    = 2,
    encreader_err_CHIP      = 3
} eOencoderreader_errortype_t;

typedef struct
{
    uint32_t                                value[eomn_serv_mc_sensor_nuomofcomp_max];
    eOencoderreader_errortype_t             errortype;
    eOmn_serv_mc_sensor_nuomofcomponents_t  composedof;
} eOencoderreader_valueInfo_t;

//typedef eOresult_t (*eOencoderreader_onendofoperation_fun_t) (EOtheEncoderReader* p, eObool_t operationisok);


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern EOtheEncoderReader* eo_encoderreader_Initialise(void);

extern EOtheEncoderReader* eo_encoderreader_GetHandle(void);



// it verifies if the service as defined in te configuration is possible (is there a good strain board or not?), it executes a callback
// (which may send a confirmation to the entity which asked fot verification), and then it may activate the strain service by calling  eo_encoderreader_Activate().
extern eOresult_t eo_encoderreader_Verify(EOtheEncoderReader *p, const eOmn_serv_arrayof_4jomodescriptors_t * jomodes, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

// it activates the service by loading the service configuration
extern eOresult_t eo_encoderreader_Activate(EOtheEncoderReader *p, const eOmn_serv_arrayof_4jomodescriptors_t * jomodes);

// it deactivates service.
extern eOresult_t eo_encoderreader_Deactivate(EOtheEncoderReader *p);

extern eOresult_t eo_encoderreader_SendReport(EOtheEncoderReader *p);

extern eOresult_t eo_encoderreader_StartReading(EOtheEncoderReader *p);

extern eObool_t eo_encoderreader_IsReadingAvailable(EOtheEncoderReader *p);

extern eOresult_t eo_encoderreader_Read(EOtheEncoderReader *p, uint8_t position, eOencoderreader_valueInfo_t *primary, eOencoderreader_valueInfo_t *secondary);

extern eOresult_t eo_encoderreader_Diagnostics_Tick(EOtheEncoderReader* p);

extern eOresult_t eo_encoderreader_GetPrimaryEncoder(EOtheEncoderReader *p, uint8_t position, eOmn_serv_mc_sensor_t *encoder);


/** @}            
    end of group eo_EOtheEncoderReader
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


