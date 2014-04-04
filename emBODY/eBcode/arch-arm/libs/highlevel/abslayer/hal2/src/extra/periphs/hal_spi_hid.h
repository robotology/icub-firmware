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
#ifndef _HAL_SPI_HID_H_
#define _HAL_SPI_HID_H_


/* @file       hal_spi_hid.h
    @brief      This header file implements hidden interface to a generic hal spi module
    @author     marco.accame@iit.it
    @date       10/29/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"
#include "hal_gpio.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_spi.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    hal_gpio_map_t          sck;        /**<  */
    hal_gpio_map_t          miso;       /**<  */
    hal_gpio_map_t          mosi;       /**<  */
} hal_spi_gpiomap_t;

typedef struct
{
    uint32_t                supportedmask;          /**< bit in position hal_spix must be 1 if portx is supported */
    hal_spi_gpiomap_t       gpiomap[hal_spis_number];
} hal_spi_boardconfig_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_spi_boardconfig_t hal_spi__theboardconfig;

// - declaration of extern hidden functions ---------------------------------------------------------------------------




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




