/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _HAL_STM32_ENCODER_HID_H_
#define _HAL_STM32_ENCODER_HID_H_


/* @file       hal_stm32_encoder_hid.h
    @brief      This header file implements hidden interface to hal encoder.
    @author     marco.accame@iit.it
    @date       09/09/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_stm32_spi4encoder_hid.h" 


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "hal_encoder.h" 



// - #define used with hidden struct ----------------------------------------------------------------------------------




// - definition of the hidden struct implementing the object ----------------------------------------------------------

//this struct let you to format the read value from encoder.
//for more information see AS5045 data sheet
struct hal_encoder_value_struct_hid_t
{
    union
    {
        struct
        {
            uint32_t evenParity:1;
            uint32_t magDec:1;
            uint32_t magInc:1;
            uint32_t linearityAlarm:1;
            uint32_t cordicOverflow:1;
            uint32_t offsetCompFinished:1;
            uint32_t data:12;
        }s;
        uint32_t val;
    }u;
};




// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section


// - declaration of extern hidden macros ------------------------------------------------------------------------------    



// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_encoder_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_encoder_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);

extern hal_encoder_t hal_encoder_hid_from_spiportmux_to_encoder(hal_spi_port_t spiport, hal_spi_mux_t spimux);



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




