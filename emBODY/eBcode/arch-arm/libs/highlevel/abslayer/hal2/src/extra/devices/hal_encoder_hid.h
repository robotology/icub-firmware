/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

#ifndef _HAL_ENCODER_HID_H_
#define _HAL_ENCODER_HID_H_


/* @file       hal_encoder_hid.h
    @brief      This header file implements hidden interface to a encoder
    @author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       02/07/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"

#include "hal_mux.h"
#include "hal_spi.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_encoder.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    hal_spi_t               spiid;          /**< which spi port is used for each encoder */
    hal_mux_t               muxid;          /**< which mux id is used for each encoder */
    hal_mux_sel_t           muxsel;         /**< which mux selection is used of the mux port */    
} hal_encoder_spimap_t;


typedef struct
{
    uint16_t                    supported_mask;
    hal_encoder_spimap_t        spimap[hal_encoders_number];
} hal_encoder_hid_brdcfg_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_encoder_hid_brdcfg_t hal_brdcfg_encoder__theconfig;

// - declaration of extern hidden functions ---------------------------------------------------------------------------



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




