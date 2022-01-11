/*
 * Copyright (C) 2021 iCub Tech Facility - Istituto Italiano di Tecnologia
 * Author:  German David Rodriguez Arenas
 * email:   german.rodriguez@iit.it
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

#include "rtw_enable_disable_motors.h"
#ifdef STM32HAL_BOARD_AMCBLDC
#include "pwm.h"
#endif /* STM32HAL_BOARD_AMCBLDC */

void rtw_enableMotor(){
    #ifdef STM32HAL_BOARD_AMCBLDC
    pwmPhaseEnable(PWM_PHASE_ALL);
    #endif /* STM32HAL_BOARD_AMCBLDC */
}

void rtw_disableMotor(){
    #ifdef STM32HAL_BOARD_AMCBLDC
    pwmPhaseDisable(PWM_PHASE_ALL);
    #endif /* STM32HAL_BOARD_AMCBLDC */
}