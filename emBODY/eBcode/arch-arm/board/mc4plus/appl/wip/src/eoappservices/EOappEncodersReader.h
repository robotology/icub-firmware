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

#include "eOcommon.h"

#include "hal_encoder.h"
#include "hal_quad_enc.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOappEncReader_hid  EOappEncReader;

/** @typedef    struct eOappEncReader_stream_t
    @brief      contains representation of an SPI stream of hal encoders. They all must be of the same type.
 **/
typedef struct 
{
    hal_encoder_type        type;                               /**< the type of SPI encoders. They must be all homogeneous */
    uint8_t                 numberof;                           /**< their number inside encoders[] */
    //hal_encoder_t           encoders[hal_encoders_number];    /**< the IDs of the encoders belonging to the stream without holes in the array */    
} eOappEncReader_stream_t;

typedef enum
{
    eo_appEncReader_stream0     = 0,  /* SPI stream 0 */ 
    eo_appEncReader_stream1     = 1,  /* SPI stream 1 */
    eo_appEncReader_streamNONE  = 255 /* SPI stream NOT DEFINED */     
} eo_appEncReader_stream_number_t;

enum { eo_appEncReader_streams_numberof = 2 };

typedef enum
{
    eo_appEncReader_stream_position0    = 0,  
    eo_appEncReader_stream_position1    = 1,  
    eo_appEncReader_stream_position2    = 2,
    eo_appEncReader_stream_positionNONE = 255,    
} eo_appEncReader_stream_position_t;

enum { eo_appEncReader_stream_position_numberof     = 3 };

typedef enum
{
    eo_appEncReader_enc_type_AEA  = 0,  /* AEA encoder (SPI) */ 
    eo_appEncReader_enc_type_AMO  = 1,  /* AMO encoder (SPI) */
    eo_appEncReader_enc_type_INC  = 2,  /* Incremental/Quadrature Encoder */
    eo_appEncReader_enc_type_ADH  = 3,  /* Analogic Hall Effect Encoder*/
    //eo_appEncReader_enc_type_MAIS = 4,  /* Encoder position coming from MAIS Can board */ // --> it will be probably managed at another level
    eo_appEncReader_enc_type_NONE = 255 /* Encoder NOT DEFINED */
} eo_appEncReader_enc_type_t;

typedef enum
{
    eo_appEncReader_encoder_position0    = 0,  
    eo_appEncReader_encoder_position1    = 1,  
    eo_appEncReader_encoder_position2    = 2,
    eo_appEncReader_encoder_position3    = 3,
    eo_appEncReader_encoder_position4    = 4,
    eo_appEncReader_encoder_position5    = 5,
    eo_appEncReader_encoder_positionNONE = 255,
} eo_appEncReader_encoder_position_t;

enum { eo_appEncReader_encoder_position_numberof = 6 }; //6 for AEA and AMO, 4 for INC

/** @typedef    struct eOappEncReader_joint_cfg_t
    @brief      contains representation of a joint from the encoder reader module point of view
 **/
typedef struct
{
    eo_appEncReader_enc_type_t          primary_encoder;
    eo_appEncReader_encoder_position_t  primary_enc_position;
    eo_appEncReader_enc_type_t          extra_encoder;
    eo_appEncReader_encoder_position_t  extra_enc_position;
} eOappEncReader_joint_t;


typedef enum
{
    eo_appEncReader_joint_position0     = 0,
    eo_appEncReader_joint_position1     = 1,
    eo_appEncReader_joint_position2     = 2,
    eo_appEncReader_joint_position3     = 3,
    eo_appEncReader_joint_position4     = 4,
    eo_appEncReader_joint_position5     = 5,
    eo_appEncReader_joint_positionNONE  = 255
} eo_appEncReader_joint_position_t;

// the value is referred to the maximum number of joint position directly managed by the board (CAN boards like MAIS or STRAIN are not considered) 
enum {eOappEncReader_joint_numberof = 6}; // max number is referred to EMS. For MC4 plus the number is 4


/** @typedef    struct eOappEncReader_cfg_t
    @brief      contains the configuration used to initialize the EOappEncodersReader application
 **/
typedef struct
{                  
    eOappEncReader_joint_t   joints[eOappEncReader_joint_numberof];          // array of joints representing the joints positions directly managed by the board; the order defines the position of the joint
    eOappEncReader_stream_t  SPI_streams[eo_appEncReader_streams_numberof];  // SPI streams; must be coherent with joints encoders definition
    eOcallback_t             SPI_callbackOnLastRead;                         // callback called when the last encoder has been read 
    void*                    SPI_callback_arg;                               // argument of callback
} eOappEncReader_cfg_t;

// maintaned for backward compatibility
typedef enum
{
    eOeOappEncReader_encoder0 = 0,   /**< is the encoder connected to P6 on ems board */  /**< is the encoder connected to P10 on mc4plus board */
    eOeOappEncReader_encoder1 = 1,   /**< is the encoder connected to P7 on ems board */  /**< is the encoder connected to P11 on mc4plus board */
    eOeOappEncReader_encoder2 = 2,   /**< is the encoder connected to P8 on ems board */
    eOeOappEncReader_encoder3 = 3,   /**< is the encoder connected to P9 on ems board */
    eOeOappEncReader_encoder4 = 4,   /**< is the encoder connected to P10 on ems board */ 
    eOeOappEncReader_encoder5 = 5,   /**< is the encoder connected to P11 on ems board */
    eOeOappEncReader_encoderUnused = 0xFF /**< this vaules indicates a null value */
} eOappEncReader_encoder_t;

/* OLD DIAGNOSTICS DEFINITIONS */
typedef enum
{
    err_onParityError   = 0,
    err_onInvalidValue  = 1,
    err_onReadFromSpi   = 2
} eOappEncReader_errortype_t;

enum {eOappEncReader_errtype_MaxNum = 3};

typedef struct
{
    uint16_t enclist[eOappEncReader_joint_numberof][eOappEncReader_errtype_MaxNum];
    uint16_t count;
} eOappEncReader_diagnosticsinfo_t;

// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
extern EOappEncReader* eo_appEncReader_New(eOappEncReader_cfg_t *cfg);
extern eOresult_t eo_appEncReader_StartRead(EOappEncReader *p); 
extern eOresult_t eo_appEncReader_getValuesRaw(EOappEncReader *p, uint32_t *data_ptr); //dataptr must be vector of[6]

EO_extern_inline eOboolvalues_t eo_appEncReader_isReady(EOappEncReader *p);  //p is not checked
EO_extern_inline eOboolvalues_t eo_appEncReader_isReadySPI_stream0(EOappEncReader *p);
EO_extern_inline eOboolvalues_t eo_appEncReader_isReadySPI_stream1(EOappEncReader *p);

extern uint64_t eo_appEncReader_startSPI_stream0(EOappEncReader *p);
extern uint64_t eo_appEncReader_startSPI_stream1(EOappEncReader *p);

extern uint32_t eo_appEncReader_deltaSPI_stream0(EOappEncReader *p);
extern uint32_t eo_appEncReader_deltaSPI_stream1(EOappEncReader *p);

extern eOresult_t  eo_appEncReader_GetJointValue(EOappEncReader *p, eo_appEncReader_joint_position_t joint_number, uint32_t *primary_value,
                                                uint32_t *extra_value, hal_encoder_errors_flags *flags);

// maintaned for backward compatibility (the values are returned only in case of SPI encoder)
extern eOresult_t  eo_appEncReader_GetValue(EOappEncReader *p, eOappEncReader_encoder_t enc, uint32_t *value,hal_encoder_errors_flags *flags);

//extern eOappEncReader_diagnosticsinfo_t* eo_appEncReader_GetDiagnosticsHandle(EOappEncReader *p);

/** @}            
    end of group eo_app_encodersReader
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


