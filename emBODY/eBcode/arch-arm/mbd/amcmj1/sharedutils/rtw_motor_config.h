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

#ifndef RTW_MOTOR_CONFIG_MOTORS
#define RTW_MOTOR_CONFIG_MOTORS

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

#include "stdint.h"

/*inizialize the motor encoder using the data read in the configuration files (yarprobotinterface) 
swapBC = 1
hall_sens_offset = 120 *65536/360 ; icubDeg*/
void rtw_configMotor(uint8_t motor_id, uint8_t has_quad_enc, int16_t rotor_enc_resolution, uint8_t pole_pairs,
                     uint8_t has_hall_sens, uint8_t swapBC,
                     uint16_t hall_sens_offset);

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */        

#endif /* RTW_MOTOR_CONFIG_MOTORS */