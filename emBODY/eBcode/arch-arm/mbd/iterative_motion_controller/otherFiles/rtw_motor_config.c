/*
 * Copyright (C) 2022 iCub Tech Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

#include "rtw_motor_config.h"

#if defined(STM32HAL_BOARD_AMCFOC_1CM7)
#include "embot_hw_motor_bldc.h"
#endif

void rtw_configMotor(uint8_t has_quad_enc, int16_t rotor_enc_resolution, uint8_t pole_pairs,
                     uint8_t has_hall_sens, uint8_t swapBC,
                     uint16_t hall_sens_offset)
{
#if defined(STM32HAL_BOARD_AMCFOC_1CM7)

    const embot::hw::motor::bldc::Config cfg { 
        rotor_enc_resolution, pole_pairs, hall_sens_offset, 
        (0 == has_quad_enc) ? false : true, 
        (0 == has_hall_sens) ? false : true, 
        (0 == swapBC) ? false : true 
    };
    embot::hw::motor::bldc::configure(embot::hw::MOTOR::one, cfg);
    
    #warning ATTENZIONE: rtw_configMotor() deve avere info su quale motore usiamo

#endif
}