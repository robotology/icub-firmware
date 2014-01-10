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

#ifndef _STM32F1_FLASH_H_
#define _STM32F1_FLASH_H_

// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint32_t stm32f1_flash_Lock(void);

extern uint32_t stm32f1_flash_Unlock(void);

extern uint32_t stm32f1_flash_SetLatency(uint32_t lat);

extern uint32_t stm32f1_flash_ErasePage(uint32_t addr);

extern uint32_t stm32f1_flash_WriteWord(uint32_t addr, uint32_t data);

extern uint32_t stm32f1_flash_WritePage(uint32_t addr, uint32_t size, uint8_t *buff);


extern uint32_t stm32f1_flash_WriteU16(uint32_t addr, uint16_t hword);


#endif  // include-guard



