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
#ifndef _HAL_STM32_ETH_HID_H_
#define _HAL_STM32_ETH_HID_H_


/* @file       hal_stm32_eth_hid.h
    @brief      This header file implements hidden interface to hal eth.
    @author     marco.accame@iit.it
    @date       06/08/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_eth.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

// removed them .... as they are related to a particula phy used with the micro.
// #define HAL_ETH_PHY_WR_TIMEOUT		    0x10000

// #define HAL_ETH_AUTONEG_MASK			0x0020
// #define HAL_ETH_LINK_UP_MASK			0x0004

// #define HAL_ETH_IS_AUTONEG_DONE(x)		(x & 0x0020)
// #define HAL_ETH_IS_LINK_UP(x) 			(x & 0x0004)
// #define IS_FULL_MODE(x)			        ((x) & 0x0004)
// #define IS_100MBIT_MODE(x)  		    (((x) & 0x0002) == 0)

// - definition of the hidden struct implementing the object ----------------------------------------------------------
// empty-section


// - declaration of extern hidden variables ---------------------------------------------------------------------------


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_eth_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_eth_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);

extern void hal_eth_hid_rmii_prepare(void);
extern void hal_eth_hid_rmii_refclock_init(void);
extern void hal_eth_hid_rmii_tx_init(void);
extern void hal_eth_hid_rmii_rx_init(void);

extern void hal_eth_hid_microcontrollerclockoutput_init(void);


extern void hal_eth_hid_smi_init(void);
extern uint16_t hal_eth_hid_smi_read(uint8_t PHYaddr, uint8_t REGaddr);
extern void hal_eth_hid_smi_write(uint8_t PHYaddr, uint8_t REGaddr, uint16_t value);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




