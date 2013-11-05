/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _STM32F1_H_
#define _STM32F1_H_


/** @file       stm32f1.h
	@brief      This header file implements public interface to the stm32f1 midware from stmicroelectronics 
	@author     marco.accame@iit.it
	@date       07/29/2010
**/

/* @defgroup stm32f1 HW library for STM32F1x series 
    
    @todo   maybe to place in here some decription of the api used for the stm32f1 ...
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

// use this from cmsis to have access to most of hw registers
#include "../../cmsis/api/cmsis_stm32f1.h"


// use those from stm32f10x standard peripheral driver library to have access to other features.
#include "../src/stm32f10x__misc.h"
#include "../src/stm32f10x_adc.h"
#include "../src/stm32f10x_bkp.h"
#include "../src/stm32f10x_can.h"
#include "../src/stm32f10x_cec.h"
#include "../src/stm32f10x_crc.h"
#include "../src/stm32f10x_dac.h"
#include "../src/stm32f10x_dbgmcu.h"
#include "../src/stm32f10x_dma.h"
#include "../src/stm32f10x_exti.h"
#include "../src/stm32f10x_flash.h"
#include "../src/stm32f10x_fsmc.h"
#include "../src/stm32f10x_gpio.h"
#include "../src/stm32f10x_i2c.h"
#include "../src/stm32f10x_iwdg.h"
#include "../src/stm32f10x_pwr.h"
#include "../src/stm32f10x_rcc.h"
#include "../src/stm32f10x_rtc.h"
#include "../src/stm32f10x_sdio.h"
#include "../src/stm32f10x_spi.h"
#include "../src/stm32f10x_tim.h"
#include "../src/stm32f10x_usart.h"
#include "../src/stm32f10x_wwdg.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



/* @}            
    end of group stm32f1  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

