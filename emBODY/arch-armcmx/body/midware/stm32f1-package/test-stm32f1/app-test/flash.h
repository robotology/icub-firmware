
// flash.h


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

extern uint32_t hal_FLASH_Lock(void);

extern uint32_t hal_FLASH_Unlock(void);

extern uint32_t hal_FLASH_SetLatency(uint32_t lat);

extern uint32_t hal_FLASH_ErasePage(uint32_t addr);

extern uint32_t hal_FLASH_WriteWord(uint32_t addr, uint32_t data);

extern uint32_t hal_FLASH_WritePage(uint32_t addr, uint32_t size, uint8_t *buff);


extern uint32_t hal_FLASH_WriteU16(uint32_t addr, uint16_t hword);
