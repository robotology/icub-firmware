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
#ifndef _EOAPPENCODERSREADER_H_
#define _EOAPPENCODERSREADER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOappEncodersReader.h
    @brief     This file provides interface to encodersReader.
    @author    valentina.gaggero@iit.it
    @date       02/17/2012
**/
/** @defgroup eo_app_encodersReader Object EOappEncReader
    .........     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "eOcommon.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOappEncReader_hid  EOappEncReader;


typedef struct
{
    uint16_t                    connectedEncodersMask;  /**< bitmask that rappresents connected encoders*/
    void (*callbackOnLastRead)(void *arg);              /**< callback called when the last encoder has been read     */
    void* callback_arg;                                 /**< argument of callback */
} eOappEncReader_cfg_t;

typedef enum
{
    eOeOappEncReader_encoder0 = 0,   /**< is the encoder connected to P6 on ems board */
    eOeOappEncReader_encoder1 = 1,   /**< is the encoder connected to P7 on ems board */
    eOeOappEncReader_encoder2 = 2,   /**< is the encoder connected to P8 on ems board */
    eOeOappEncReader_encoder3 = 3,   /**< is the encoder connected to P9 on ems board */
    eOeOappEncReader_encoder4 = 4,   /**< is the encoder connected to P10 on ems board: currently is not connected */ 
    eOeOappEncReader_encoder5 = 5    /**< is the encoder connected to P11 on ems board: currently is not connected */
} eOeOappEncReader_encoder_t;

enum {eOeOappEncReader_encoderMaxNum = 6};


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
extern EOappEncReader* eo_appEncReader_New(eOappEncReader_cfg_t *cfg);
extern eOresult_t eo_appEncReader_StartRead(EOappEncReader *p);
extern eOresult_t eo_appEncReader_getValues(EOappEncReader *p, uint32_t *data_ptr); //dataptr must be vector of[6]
__inline extern eOboolvalues_t eo_appEncReader_isReady(EOappEncReader *p);  //p is not checked

extern eOresult_t  eo_appEncReader_GetValue(EOappEncReader *p, eOeOappEncReader_encoder_t enc, uint32_t *value);
/** @}            
    end of group eo_app_encodersReader
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


