/*
 * Copyright (C) 2021 iCub Tech Facility - Istituto Italiano di Tecnologia
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

#if defined(STM32HAL_BOARD_AMCBLDC) || defined(STM32HAL_BOARD_AMC2C)

#ifndef __cplusplus
    #error this file must be compiled in C++ mode when deployed on the target
#endif
    
#include "embot_hw_motor.h"
    
#else
    #warning: choose a STM32HAL_BOARD_*    
#endif

void rtw_configMotor(uint8_t has_quad_enc, int16_t rotor_enc_resolution, uint8_t pole_pairs, uint8_t has_hall_sens, uint8_t swapBC, uint16_t hall_sens_offset)
{
#if defined(STM32HAL_BOARD_AMCBLDC) || defined(STM32HAL_BOARD_AMC2C)

//    embot::hw::motor::config(
//        embot::hw::MOTOR::one,
//        has_quad_enc,    
//        rotor_enc_resolution, 
//        pole_pairs, 
//        has_hall_sens,
//        swapBC,             // it was 1, but better use its param 
//        hall_sens_offset);  // it was 120*65536/360, but better using its param  
//                            // because: supervisor calls rtw_configMotor( , swapBC = 1, hall_sens_offset = 21845) and 120*65536/360 = 21845
    
    // marco.accame: but now we use embot::hw::motor::Config
    const embot::hw::motor::Config cfg { 
        rotor_enc_resolution, pole_pairs, hall_sens_offset, 
        (0 == has_quad_enc) ? false : true, 
        (0 == has_hall_sens) ? false : true, 
        (0 == swapBC) ? false : true 
    };
    embot::hw::motor::configure(embot::hw::MOTOR::one, cfg);
     
#else
    #warning: choose a STM32HAL_BOARD_*    
#endif
}