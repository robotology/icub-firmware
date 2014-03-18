/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HAL_PERIPH_DMA_HID_H_
#define _HAL_PERIPH_DMA_HID_H_


/* @file       hal_periph_dma_hid.h
	@brief      This file contains hidden api of the hal dma module.
	@author     marco.accame@iit.it
    @date       12/21/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_base_hid.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_dma.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    uint16_t            supported_mask;                 /**< bit in position hal_dmax must be 1 if portx is supported */
} hal_dma_hid_brdcfg_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------


// - declaration of extern hidden functions ---------------------------------------------------------------------------





#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




