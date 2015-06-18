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


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

// for EB1_3_6_8

const eOappEncReader_cfg_t encoder_reader_config_eb6 =
{
    .joints = 
    {   // there are managed 4 joints, all served by an AEA
        {   // pos 0
            .primary_encoder        = eo_appEncReader_enc_type_AEA,
            .primary_enc_position   = (eo_appEncReader_encoder_position_t)hal_encoder1,
            .extra_encoder          = eo_appEncReader_enc_type_NONE,
            .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
        },
        {   // pos 1
            .primary_encoder        = eo_appEncReader_enc_type_AEA,
            .primary_enc_position   = (eo_appEncReader_encoder_position_t)hal_encoder4,
            .extra_encoder          = eo_appEncReader_enc_type_NONE,
            .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
        },
        {   // pos 2
            .primary_encoder        = eo_appEncReader_enc_type_AEA,
            .primary_enc_position   = (eo_appEncReader_encoder_position_t)hal_encoder2,
            .extra_encoder          = eo_appEncReader_enc_type_NONE,
            .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
        },
        {   // pos 3
            .primary_encoder        = eo_appEncReader_enc_type_AEA,
            .primary_enc_position   = (eo_appEncReader_encoder_position_t)hal_encoder5,
            .extra_encoder          = eo_appEncReader_enc_type_NONE,
            .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
        }, 
        {   // pos 4
            .primary_encoder        = eo_appEncReader_enc_type_NONE,
            .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
            .extra_encoder          = eo_appEncReader_enc_type_NONE,
            .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
        },
        {   // pos 5
            .primary_encoder        = eo_appEncReader_enc_type_NONE,
            .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
            .extra_encoder          = eo_appEncReader_enc_type_NONE,
            .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
        }            
    },
    .SPI_streams    =
    {   // at most 2 streams: one over spix and one over spiy
        {
            .type       = hal_encoder_t1, // or hal_encoder_t1 for aea
            .numberof   = 2
        },
        {
            .type       = hal_encoder_t1, // or hal_encoder_t1 for aea
            .numberof   = 2
        }
    },
    .SPI_callbackOnLastRead = NULL,
    .SPI_callback_arg       = NULL       
};

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





