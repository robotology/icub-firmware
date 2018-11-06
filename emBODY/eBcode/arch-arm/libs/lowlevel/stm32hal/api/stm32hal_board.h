/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _STM32HAL_BOARD_H_
#define _STM32HAL_BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------  
    
#include "stm32hal_define.h"
#include "stm32hal_driver.h"
    
// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types -------------------------------------------------------------------------    
// empty-section
    
    
// - declaration of extern public functions ---------------------------------------------------------------------------
 
// called by stm32hal_init() if stm32hal_config_t::initbsp is true
    
extern void stm32hal_board_init(void);     
    
// - public interface: begin  -----------------------------------------------------------------------------------------  
// it contains whatever cube-mx generates.


#if     defined(STM32HAL_BOARD_NUCLEO64)

#include "../src/config/stm32hal_driver_cfg_of_nucleo64.h"

#include "../src/board/nucleo64/inc/gpio.h"
#include "../src/board/nucleo64/inc/main.h"
#include "../src/board/nucleo64/inc/usart.h"
	
	
#elif   defined(STM32HAL_BOARD_MTB4)	

#include "../src/config/stm32hal_driver_cfg_of_mtb4.h"

#include "../src/board/mtb4/inc/adc.h"
#include "../src/board/mtb4/inc/can.h"
#include "../src/board/mtb4/inc/dma.h"
#include "../src/board/mtb4/inc/gpio.h"
#include "../src/board/mtb4/inc/i2c.h"
#include "../src/board/mtb4/inc/rng.h"
#include "../src/board/mtb4/inc/main.h"
#include "../src/board/mtb4/inc/tim.h"
#include "../src/board/mtb4/inc/usart.h"


#include "../src/board/mtb4/inc/stm32l4xx_it.h"

#elif   defined(STM32HAL_BOARD_STRAIN2)	

#include "../src/config/stm32hal_driver_cfg_of_strain2.h"

#include "../src/board/strain2/inc/adc.h"
#include "../src/board/strain2/inc/can.h"
#include "../src/board/strain2/inc/dma.h"
#include "../src/board/strain2/inc/gpio.h"
#include "../src/board/strain2/inc/i2c.h"
#include "../src/board/strain2/inc/main.h"
#include "../src/board/strain2/inc/rng.h"
#include "../src/board/strain2/inc/tim.h"
#include "../src/board/strain2/inc/usart.h"


#include "../src/board/strain2/inc/stm32l4xx_it.h"

#elif   defined(STM32HAL_BOARD_RFE) 

#include "../src/config/stm32hal_driver_cfg_of_rfe.h"

#include "../src/board/rfe/inc/adc.h"
#include "../src/board/rfe/inc/can.h"
#include "../src/board/rfe/inc/dma.h"
#include "../src/board/rfe/inc/gpio.h"
#include "../src/board/rfe/inc/i2c.h"
#include "../src/board/rfe/inc/main.h"
#include "../src/board/rfe/inc/spi.h"
#include "../src/board/rfe/inc/rng.h"
#include "../src/board/rfe/inc/tim.h"

#include "../src/board/rfe/inc/stm32l4xx_it.h"

#else
    #error STM32HAL: you must define a STM32HAL_BOARD_${BRD}
#endif


// - public interface: end --------------------------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
