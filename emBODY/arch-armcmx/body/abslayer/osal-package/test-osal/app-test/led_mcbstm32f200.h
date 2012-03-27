/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

#ifndef _LED_MCBSTM32F200_H_
#define _LED_MCBSTM32F200_H_

#include "stdint.h"

typedef enum
{
    led_mcbstm32f200_0 = 0,
    led_mcbstm32f200_1, led_mcbstm32f200_2, led_mcbstm32f200_3, led_mcbstm32f200_4, led_mcbstm32f200_5,
    led_mcbstm32f200_6, led_mcbstm32f200_7
} led_mcbstm32f200_t;

extern void led_mcbstm32f200_init (void);
extern void led_mcbstm32f200_on (led_mcbstm32f200_t led);
extern void led_mcbstm32f200_off (led_mcbstm32f200_t led);


#endif



