/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Davide Pollarolo
 * email:   valentina.gaggero@iit.it, davide.pollarolo@iit.it
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

#ifndef _EOAPPENCODERSREADER_H_
#define _EOAPPENCODERSREADER_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOappEncodersReader.h
    @brief      This file provides interface to encodersReader.
    @author     valentina.gaggero@iit.it, davide.pollarolo@iit.it
    @date       02/17/2012
**/
/** @defgroup eo_app_encodersReader Object EOappEncReader
    .........     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoManagement.h"
#include "EOconstarray.h"    
#include "EOtheEncoderReader.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOappEncReader_hid  EOappEncReader;


// the value is referred to the maximum number of joint-motors directly managed by this object.
enum { eOappEncReader_jomos_maxnumberof = 7 }; 

// the value is referred to the maximum number of encoders directly managed by this object. 
enum { eOappEncReader_encoders_maxnumberof = 2*eOappEncReader_jomos_maxnumberof }; 


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOappEncReader* eo_appEncReader_Initialise(void);

extern EOappEncReader* eo_appEncReader_GetHandle(void);

extern eOresult_t eo_appEncReader_Activate(EOappEncReader *p, EOconstarray *arrayofjomodes, eOmn_serv_diagn_cfg_t dc);

extern eOresult_t eo_appEncReader_Deactivate(EOappEncReader *p);


extern eOresult_t eo_appEncReader_StartRead(EOappEncReader *p); 

extern eObool_t eo_appEncReader_isReady(EOappEncReader *p);  

extern eOresult_t eo_appEncReader_GetValue(EOappEncReader *p, uint8_t jomo, eOencoderreader_valueInfo_t *value1, eOencoderreader_valueInfo_t *value2);

extern eOresult_t eo_appEncReader_Diagnostics_Enable(EOappEncReader *p, eObool_t on);

extern eOresult_t eo_appEncReader_Diagnostics_Tick(EOappEncReader *p);

extern eOresult_t eo_appEncReader_GetEncoderType(EOappEncReader *p, uint8_t jomo, eOmc_encoder_t *primary, eOmc_encoder_t *secondary);

extern eOresult_t eo_appEncReader_UpdatedMaisConversionFactors(EOappEncReader *p, uint8_t jomo, float convFactor);
extern eOresult_t eo_appEncReader_UpdatedHallAdcConversionFactors(EOappEncReader *p, uint8_t jomo, float convFactor);
extern eOresult_t eo_appEncReader_UpdatedHallAdcOffset(EOappEncReader *p, uint8_t jomo, int32_t offset);

// IFreader
extern eOresult_t eo_appEncReader_GetRawValue(EOappEncReader *p, uint8_t jomo, eOencoderreader_RawValuesOfJomo_t *rawValuesArray);

/** @}            
    end of group eo_app_encodersReader
 **/
 
 
//#ifdef __cplusplus
//}       // closing brace for extern "C"
//#endif 

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


