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
#ifndef _EOTHEENCODERREADER2_H_
#define _EOTHEENCODERREADER2_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheEncoderReader2.h
    @brief      this object implements what is required for managing the STRAIN.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheEncoderReader2 Object EOtheEncoderReader2
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"

#include "hal_encoder.h"

#include "EOtheServices.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheEncoderReader2_hid EOtheEncoderReader2;

enum { eo_encoderreader2_maxnumberof_jomos = 4 };

enum { eo_encoderreader2_maxnumberof_encoders = hal_encoders_number }; // hal_encoders_number is 6.


//typedef eOresult_t (*eOencoderreader_onendofoperation_fun_t) (EOtheEncoderReader2* p, eObool_t operationisok);


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern EOtheEncoderReader2* eo_encoderreader2_Initialise(void);

extern EOtheEncoderReader2* eo_encoderreader2_GetHandle(void);


// it verifies if the service as defined in te configuration is possible (is there a good strain board or not?), it executes a callback
// (which may send a confirmation to the entity which asked fot verification), and then it may activate the strain service by calling  eo_encoderreader2_Activate().
extern eOresult_t eo_encoderreader2_Verify(EOtheEncoderReader2 *p, const eOmn_serv_arrayof_4jomodescriptors_t * jomodes, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

// it activates the strain service by loading the service configuration
extern eOresult_t eo_encoderreader2_Activate(EOtheEncoderReader2 *p, const eOmn_serv_arrayof_4jomodescriptors_t * jomodes);

// it deactivates service.
extern eOresult_t eo_encoderreader2_Deactivate(EOtheEncoderReader2 *p);

extern eOresult_t eo_encoderreader2_StartReading(EOtheEncoderReader2 *p);

extern eObool_t eo_encoderreader2_IsReadingAvailable(EOtheEncoderReader2 *p);

extern eOresult_t eo_encoderreader2_Read(EOtheEncoderReader2 *p, uint8_t jomo, hal_encoder_position_t *primary, hal_encoder_position_t *secondary, hal_encoder_errors_flags *errors);



/** @}            
    end of group eo_EOtheEncoderReader2
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


