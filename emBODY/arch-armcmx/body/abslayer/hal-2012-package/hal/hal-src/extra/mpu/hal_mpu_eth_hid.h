/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HAL_MPU_ETH_HID_H_
#define _HAL_MPU_ETH_HID_H_


/* @file       hal_mpu_eth_hid.h
    @brief      This header file implements hidden interface to hal eth.
    @author     marco.accame@iit.it
    @date       06/08/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_gpio.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_eth.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef enum
{   // mif: media interface
    hal_eth_mif_rmii    = 0,
    hal_eth_mif_mii     = 1
} hal_eth_hid_mif_t;

typedef struct
{   // RMII: Reduced Media Independent Interface   
    hal_gpio_cfg_t      ETH_RMII_REF_CLK;           /**< configuration of gpio used for ETH_RMII_REF_CLK pin */
    hal_gpio_cfg_t      ETH_RMII_TX_EN;             /**< configuration of gpio used for ETH_RMII_TX_EN pin */
    hal_gpio_cfg_t      ETH_RMII_TXD0;              /**< configuration of gpio used for ETH_RMII_TXD0 pin */
    hal_gpio_cfg_t      ETH_RMII_TXD1;              /**< configuration of gpio used for ETH_RMII_TXD1 pin */
    hal_gpio_cfg_t      ETH_RMII_CRS_DV;            /**< configuration of gpio used for ETH_RMII_CRS_DV pin */
    hal_gpio_cfg_t      ETH_RMII_RXD0;              /**< configuration of gpio used for ETH_RMII_RXD0 pin */
    hal_gpio_cfg_t      ETH_RMII_RXD1;              /**< configuration of gpio used for ETH_RMII_RXD1 pin */
} hal_eth_hid_gpio_rmii_t;

typedef struct
{   // MII: Media Independent Interface
    uint8_t             tobedone;
} hal_eth_hid_gpio_mii_t;

typedef union
{   // media interface between the mac and a phy chip
    hal_eth_hid_gpio_rmii_t     rmii;
    hal_eth_hid_gpio_mii_t      mii;
} hal_eth_hid_gpio_mif_t;

typedef struct
{   // SMI: Serial Management Interface
    hal_gpio_cfg_t      ETH_MDC;                    /**< configuration of gpio used for ETH_MDC pin */
    hal_gpio_cfg_t      ETH_MDIO;                   /**< configuration of gpio used for ETH_MDIO pin */
} hal_eth_hid_gpio_smi_t;

typedef struct
{
    hal_bool_t                  supported;          /**< hal_true if supported */
    hal_eth_hid_mif_t           mif;                /**< always rmii  */
    hal_eth_hid_gpio_mif_t      gpio_mif;    
    hal_eth_hid_gpio_smi_t      gpio_smi;
//    uint8_t                     numofdmatxbuffers;
//    uint8_t                     numofdmarxbuffers;
} hal_eth_hid_brdcfg_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern hal_result_t hal_eth_hid_static_memory_init(void);

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




