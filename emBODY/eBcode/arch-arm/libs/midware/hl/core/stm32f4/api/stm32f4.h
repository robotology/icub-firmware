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
#ifndef _STM32F4_H_
#define _STM32F4_H_


/** @file       stm32f4.h
	@brief      This header file implements public interface to the stm32f4 midware from stmicroelectronics 
	@author     marco.accame@iit.it
	@date       11/18/2013
**/

/* @defgroup stm32f4 HW library for STM32F1x series 
    
    @todo   maybe to place in here some description of the api used for the stm32f4...
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

// use this from cmsis to have access to most of hw registers
#include "../../cmsis/api/cmsis_stm32f4.h"


// use those from stm32f4xx standard peripheral driver library to have access to other features.
#include "../src/stm32f4xx__misc.h"
#include "../src/stm32f4xx_adc.h"
#include "../src/stm32f4xx_can.h"
#include "../src/stm32f4xx_crc.h"
#include "../src/stm32f4xx_cryp.h"
#include "../src/stm32f4xx_dac.h"
#include "../src/stm32f4xx_dbgmcu.h"
#include "../src/stm32f4xx_dcmi.h"
#include "../src/stm32f4xx_dma.h"
#include "../src/stm32f4xx_dma2d.h"
#include "../src/stm32f4xx_exti.h"
#include "../src/stm32f4xx_flash.h"
#include "../src/stm32f4xx_fsmc.h"
#include "../src/stm32f4xx_gpio.h"
#include "../src/stm32f4xx_hash.h"
#include "../src/stm32f4xx_i2c.h"
#include "../src/stm32f4xx_iwdg.h"
#include "../src/stm32f4xx_ltdc.h"
#include "../src/stm32f4xx_pwr.h"
#include "../src/stm32f4xx_rcc.h"
#include "../src/stm32f4xx_rng.h"
#include "../src/stm32f4xx_rtc.h"
#include "../src/stm32f4xx_sai.h"
#include "../src/stm32f4xx_sdio.h"
#include "../src/stm32f4xx_spi.h"
#include "../src/stm32f4xx_syscfg.h"
#include "../src/stm32f4xx_tim.h"
#include "../src/stm32f4xx_usart.h"
#include "../src/stm32f4xx_wwdg.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



/* @}            
    end of group stm32f4  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

