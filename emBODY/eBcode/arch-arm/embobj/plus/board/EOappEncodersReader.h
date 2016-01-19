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

#include "hal_encoder.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOappEncReader_hid  EOappEncReader;


typedef enum
{
    eo_appEncReader_enc_type_AEA  = 0,  /* AEA encoder (SPI) */ 
    eo_appEncReader_enc_type_AMO  = 1,  /* AMO encoder (SPI) */
    eo_appEncReader_enc_type_INC  = 2,  /* Incremental/Quadrature Encoder */
    eo_appEncReader_enc_type_ADH  = 3,  /* Analogic Hall Effect Encoder */
    eo_appEncReader_enc_type_MAIS = 4,  /* Encoder position coming from MAIS can board. the field port specifies the kind of finger is requested */ 
    eo_appEncReader_enc_type_NONE = 255 /* Encoder NOT DEFINED */
} eo_appEncReader_encoder_type_t;

enum { eo_appEncReader_enc_type_numberof = 5 };


enum 
{ 
    eo_appEncReader_encoder_portFingerTHUMB0        = 0,
    eo_appEncReader_encoder_portFingerTHUMB1        = 1,
    eo_appEncReader_encoder_portFingerINDEXprox     = 2, 
    eo_appEncReader_encoder_portFingerINDEXdist     = 3,
    eo_appEncReader_encoder_portFingerMEDIUMprox    = 4,
    eo_appEncReader_encoder_portFingerMEDIUMdist    = 5,
    eo_appEncReader_encoder_portFingerPINKY         = 6,
    eo_appEncReader_encoder_portMAX = 8, 
    eo_appEncReader_encoder_portNONE = 255 
}; // from 0 to MAX



typedef enum
{
    eo_appEncReader_encoder_place_atjoint   = 0,  
    eo_appEncReader_encoder_place_atmotor   = 1,
    eo_appEncReader_encoder_place_NONE      = 255,
} eo_appEncReader_encoder_place_t;



/** @typedef    struct eOappEncReader_joint_cfg_t
    @brief      contains representation of a joint from the encoder reader module point of view
 **/
typedef struct
{                                       
    eo_appEncReader_encoder_type_t      primary_encoder_type;
    uint8_t                             primary_encoder_port;
    eo_appEncReader_encoder_place_t     primary_encoder_place;
    eo_appEncReader_encoder_type_t      secondary_encoder_type;
    uint8_t                             secondary_encoder_port;
    eo_appEncReader_encoder_place_t     secondary_encoder_place;
} eOappEncReader_jomoconfig_t;


// the value is referred to the maximum number of joint-motors directly managed by this object. for MC4plus, hw limitations lower this number to 4 
enum { eOappEncReader_jomos_maxnumberof = 6 }; 


/** @typedef    struct eOappEncReader_cfg_t
    @brief      contains the configuration used to initialize the EOappEncodersReader application
 **/
typedef struct
{   
    uint8_t                     numofjomos;  
    eOappEncReader_jomoconfig_t jomoconfig[eOappEncReader_jomos_maxnumberof];   
    eOcallback_t                SPI_callbackOnLastRead;                     
    void*                       SPI_callback_arg;                           
} eOappEncReader_cfg_t;


typedef enum
{
    err_onParityError   = 0,
    err_onInvalidValue  = 1,
    err_onReadFromSpi   = 2,
    err_NONE            = 255,
} eOappEncReader_errortype_t;



// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOappEncReader* eo_appEncReader_Initialise(void);

extern EOappEncReader* eo_appEncReader_GetHandle(void);

extern eOresult_t eo_appEncReader_Activate(EOappEncReader *p, eOappEncReader_cfg_t *cfg);

extern eOresult_t eo_appEncReader_Deactivate(EOappEncReader *p);


//extern EOappEncReader* eo_appEncReader_New(eOappEncReader_cfg_t *cfg);

extern eOresult_t eo_appEncReader_StartRead(EOappEncReader *p); 

extern eObool_t eo_appEncReader_isReady(EOappEncReader *p);  


extern eOresult_t eo_appEncReader_GetValue(EOappEncReader *p, uint8_t jomo, uint32_t *primary_value,
                                                uint32_t *extra_value, hal_encoder_errors_flags *flags);


/** @}            
    end of group eo_app_encodersReader
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


