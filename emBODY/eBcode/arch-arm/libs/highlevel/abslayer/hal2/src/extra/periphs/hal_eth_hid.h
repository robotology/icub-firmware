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
#ifndef _HAL_ETH_HID_H_
#define _HAL_ETH_HID_H_


/* @file       hal_eth_hid.h
    @brief      This header file implements hidden interface to hal eth.
    @author     marco.accame@iit.it
    @date       06/08/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"
#include "hal_gpio.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_eth.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef enum
{   // mif: media interface
    hal_eth_mif_rmii    = 0,
    hal_eth_mif_mii     = 1
} hal_eth_mif_t;

typedef struct
{   // RMII: Reduced Media Independent Interface   
    hal_gpio_map_t      ETH_RMII_REF_CLK;           /**< mapping of gpio used for ETH_RMII_REF_CLK pin */
    hal_gpio_map_t      ETH_RMII_TX_EN;             /**< mapping of gpio used for ETH_RMII_TX_EN pin */
    hal_gpio_map_t      ETH_RMII_TXD0;              /**< mapping of gpio used for ETH_RMII_TXD0 pin */
    hal_gpio_map_t      ETH_RMII_TXD1;              /**< mapping of gpio used for ETH_RMII_TXD1 pin */
    hal_gpio_map_t      ETH_RMII_CRS_DV;            /**< mapping of gpio used for ETH_RMII_CRS_DV pin */
    hal_gpio_map_t      ETH_RMII_RXD0;              /**< mapping of gpio used for ETH_RMII_RXD0 pin */
    hal_gpio_map_t      ETH_RMII_RXD1;              /**< mapping of gpio used for ETH_RMII_RXD1 pin */
} hal_eth_gpio_rmii_t;

typedef struct
{   // MII: Media Independent Interface
    uint8_t                     tobedone;
} hal_eth_gpio_mii_t;

typedef union
{   // media interface between the mac and a phy chip
    hal_eth_gpio_rmii_t         rmii;
    hal_eth_gpio_mii_t          mii;
} hal_eth_gpio_mif_t;

typedef struct
{   // SMI: Serial Management Interface
    hal_gpio_map_t              ETH_MDC;        /**< mapping of gpio used for ETH_MDC pin */
    hal_gpio_map_t              ETH_MDIO;       /**< mapping of gpio used for ETH_MDIO pin */
} hal_eth_gpio_smi_t;

typedef struct
{
    hal_eth_mif_t               mif;            /**< always rmii  */
    hal_eth_gpio_mif_t          gpio_mif;    
    hal_eth_gpio_smi_t          gpio_smi;
} hal_eth_gpiomap_t;


typedef struct
{
    hal_bool_t          supported;          /**< hal_true if supported */
    uint8_t             rxdmafifocapacity;  /**< if non hal_NA08 use them, otherwise use the ones from hal_eth_cfg_t param of hal_eth_init() */
    uint8_t             txdmafifocapacity;  /**< if non hal_NA08 use them, otherwise use the ones from hal_eth_cfg_t param of hal_eth_init() */
    hal_eth_gpiomap_t   gpiomap;            /**< the gpio mapping */
} hal_eth_boardconfig_t;


// typedef struct
// {
//     void (*fn_inside_eth_isr)(uint8_t*, uint16_t);          /**< accepts the pointer to the received payload and its size */    
// } hal_eth_hid_debug_support_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_eth_boardconfig_t hal_eth__theboardconfig;

// marco.accame: now the ETH_IRQHandler() is inside hl_eth.c, thus the definition of this variable does not call the function.
//extern hal_eth_hid_debug_support_t hal_eth_hid_DEBUG_support; 

// - declaration of extern hidden functions ---------------------------------------------------------------------------


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




