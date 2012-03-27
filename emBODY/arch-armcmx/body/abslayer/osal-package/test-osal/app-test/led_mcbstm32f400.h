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
#ifndef _LED_MCBSTM32F400_H_
#define _LED_MCBSTM32F400_H_

#include "stdint.h"

typedef enum
{
    led_mcbstm32f400_0 = 0,
    led_mcbstm32f400_1, led_mcbstm32f400_2, led_mcbstm32f400_3, led_mcbstm32f400_4, led_mcbstm32f400_5,
    led_mcbstm32f400_6, led_mcbstm32f400_7
} led_mcbstm32f400_t;

extern void led_mcbstm32f400_init (void);
extern void led_mcbstm32f400_on (led_mcbstm32f400_t led);
extern void led_mcbstm32f400_off (led_mcbstm32f400_t led);


#endif



