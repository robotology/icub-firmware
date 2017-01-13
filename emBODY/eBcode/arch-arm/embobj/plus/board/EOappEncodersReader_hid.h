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

#ifndef _EOAPPENCODERSREADER_HID_H_
#define _EOAPPENCODERSREADER_HID_H_


#ifdef __cplusplus
extern "C" {
#endif
    
// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOappEncodersReader.h
    @brief     This file provides hidden interface to encodersReader obj.
    @author    valentina.gaggero@iit.it, davide.pollarolo@iit.it
    @date       02/17/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------


#include "hal_spiencoder.h"
#include "hal_gpio.h"
#include "EOtheLEDpulser.h"



// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOappEncodersReader.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{                                       
    eOmc_encoder_descriptor_t   encoder1des;
    eOmc_encoder_descriptor_t   encoder2des;
} eOappEncReader_jomoconfig_t;


typedef struct
{   
    uint8_t                     numofjomos;  
    eOappEncReader_jomoconfig_t jomoconfig[eOappEncReader_jomos_maxnumberof];                           
} eOappEncReader_cfg_t;


typedef struct 
{
    hal_spiencoder_type_t   type;                               /**< the type of SPI encoders. it must be only one */
    uint8_t                 numberof;                           /**< their number inside the stream */
    uint8_t                 maxsupported;
    volatile eObool_t       isacquiring;                        
    hal_spiencoder_t        id[hal_spiencoder_maxnumber_in_stream+1]; // reading order of encoders 
} eOappEncReader_stream_t;

typedef struct 
{
    float   factors[eOappEncReader_jomos_maxnumberof];
    int32_t offsets[eOappEncReader_jomos_maxnumberof];
} eOappEncReader_hallAdc_conversionData_t;


enum { AEAerror_NONE = 0,  AEAerror_SPI = 1, AEAerror_CRC = 2, AEAerror_CHIP = 3 }; // it goes inside each nibble of par16 
typedef struct
{   // so far only for AEA and primary encoder
    uint8_t             jomomask;
    uint32_t            periodOKreport;
    uint32_t            periodKOreport;
    eOledpulser_led_t   errorled;
    hal_gpio_t          errorgpio;   
} eo_appEncReader_diagnostics_cfg_t;


typedef struct
{   // so far only for AEA and primary encoder
    eo_appEncReader_diagnostics_cfg_t       config;
    uint64_t                                par64;
    uint16_t                                par16;
} eo_appEncReader_diagnostics_t;


struct EOappEncReader_hid
{
    eObool_t                                initted;
    eObool_t                                active;
    uint8_t                                 totalnumberofencoders; // it cannot be higher than eOappEncReader_encoders_maxnumberof
    const hal_spiencoder_stream_map_t*      stream_map;
    eOappEncReader_cfg_t                    config;       
    eOappEncReader_stream_t                 SPI_streams[hal_spiencoder_streams_number];  // SPI streams; must be coherent with what inside cfg
    eo_appEncReader_diagnostics_t           diagnostics;
    float                                   maisCoversionFactors[eOappEncReader_jomos_maxnumberof];
    eOappEncReader_hallAdc_conversionData_t hallAdcConversionData;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



