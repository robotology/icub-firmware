/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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
#ifndef _HAL_STM32_FLASH_HID_H_
#define _HAL_STM32_FLASH_HID_H_


/* @file       hal_stm32_flash_hid.h
    @brief      This header file implements hidden interface to flash.
    @author     marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_flash.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/* @typedef    typedef enum hal_flash_cycles_t 
    @brief      hal_flash_cycles_t contains the number of cycles that FLASH can be configured to have
 **/
typedef enum  
{ 
    hal_flash_0cycles = 0,          /**< 0 wait cycles         */
    hal_flash_1cycles = 1,          /**< 1 wait cycles         */
    hal_flash_2cycles = 2           /**< 0 wait cycles         */
} hal_flash_cycles_t; 

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_flash_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_flash_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);


/** @fn         extern hal_result_t hal_flash_hid_setlatency(hal_flash_cycles_t ncycles)
    @brief      Sets the latency of the FLASH 
    @param      ncycles         The number of waiting cycles. 
    @return     Always hal_res_OK.
 **/
extern hal_result_t hal_flash_hid_setlatency(hal_flash_cycles_t ncycles);



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




