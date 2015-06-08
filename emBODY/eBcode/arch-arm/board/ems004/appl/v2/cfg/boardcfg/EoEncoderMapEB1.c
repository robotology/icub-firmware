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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "EoCommon.h"
#include "EOappEncodersReader.h"

#include "EOconstvector_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

#define EB1_3_6_8

#if defined(EB1_3_6_8)
    #define encstream0_type         hal_encoder_t1
    #define encstream0_numberof     2
    #define encstream0_encoders0    hal_encoder1
    #define encstream0_encoders1    hal_encoder2
    #define encstream0_encoders2    hal_encoderNONE
    #define encstream0_encoders3    hal_encoderNONE
    #define encstream0_encoders4    hal_encoderNONE
    #define encstream0_encoders5    hal_encoderNONE

    
    #define encstream1_type         hal_encoder_t1
    #define encstream1_numberof     2
    #define encstream1_encoders0    hal_encoder4
    #define encstream1_encoders1    hal_encoder5
    #define encstream1_encoders2    hal_encoderNONE
    #define encstream1_encoders3    hal_encoderNONE
    #define encstream1_encoders4    hal_encoderNONE
    #define encstream1_encoders5    hal_encoderNONE
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


static const eOappEncReader_stream_t s_enc_streams[] = 
{
        {   // stream 0
            .type       = encstream0_type,
            .numberof   = encstream0_numberof,
            .encoders   = { encstream0_encoders0, encstream0_encoders1, encstream0_encoders2, encstream0_encoders3, encstream0_encoders4, encstream0_encoders5 }   
        },
        {   // stream 1
            .type       = encstream1_type,
            .numberof   = encstream1_numberof,
            .encoders   = { encstream1_encoders0, encstream1_encoders1, encstream1_encoders2, encstream1_encoders3, encstream1_encoders4, encstream1_encoders5 }        
        }  
};

EOconstvector s_eo_vectorof_encoderstreams_eb1 = 
{
    .capacity       = sizeof(s_enc_streams)/sizeof(eOappEncReader_stream_t),
    .size           = sizeof(s_enc_streams)/sizeof(eOappEncReader_stream_t),
    .item_size      = sizeof(eOappEncReader_stream_t),
    .dummy          = 0,
    .stored_items   = (void*)s_enc_streams,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_encoderstreams_eb1 = &s_eo_vectorof_encoderstreams_eb1;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





