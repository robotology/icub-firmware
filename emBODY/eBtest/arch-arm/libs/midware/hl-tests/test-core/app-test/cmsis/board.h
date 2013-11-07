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

#ifndef _BRD_MCBSTM32X_H_
#define _BRD_MCBSTM32X_H_

#include "stdint.h"

#if     defined(HL_USE_BRD_MCBSTM32)
    
    #include "cmsis_stm32f1.h"
    
#elif   defined(HL_USE_BRD_MCBSTM32_C)

    #include "cmsis_stm32f1.h"
    
#elif   defined(HL_USE_BRD_EMS001)

    #include "cmsis_stm32f1.h"
    
#elif   defined(HL_USE_BRD_MCBSTM32_F200)

    #include "cmsis_stm32f2.h"

#elif   defined(HL_USE_BRD_MCBSTM32_F400)
    
    #include "cmsis_stm32f4.h" 

#else
    #error --> define a MCBSTM32 board
#endif

typedef enum
{
    board_led_0 = 0,
    board_led_1, board_led_2, board_led_3, board_led_4, board_led_5,
    board_led_6, board_led_7
} board_led_t;

extern void board_led_init(void);
extern void board_led_on(board_led_t led);
extern void board_led_off(board_led_t led);


#endif



