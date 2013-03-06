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

#ifndef _HAL_CHIP_MICREL_KS8893_H_
#define _HAL_CHIP_MICREL_KS8893_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_chip_micrel_ks8893.h
    @brief      This header file implements interface to a micrel switch
    @author     marco.accame@iit.it
    @date       10/24/2012
**/

/** @defgroup arm_hal_chip_micrel_ks8893 HAL DEV_ACC

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 




/** @typedef    typedef enum hal_chip_micrel_ks8893_cfg_t 
    @brief      hal_chip_micrel_ks8893_cfg_t contains the configuration for the switch.
 **/
typedef struct
{
    hal_i2c_t           i2cport;
    hal_gpio_cfg_t      resetpin;
    hal_res_fp_void_t   extclockinit;
} hal_chip_micrel_ks8893_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_chip_micrel_ks8893_cfg_t hal_chip_micrel_ks8893_cfg_default; // = {.i2cport = hal_i2c_port1, etc.};


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_chip_micrel_ks8893_init(const hal_chip_micrel_ks8893_cfg_t *cfg)
    @brief  	this function initializes a lis3dh gyro sensor 
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_chip_micrel_ks8893_init(const hal_chip_micrel_ks8893_cfg_t *cfg);



/** @fn			extern hal_result_t hal_chip_micrel_ks8893_configure(hal_eth_phymode_t targetphymode, hal_eth_phymode_t* usedphymode)
    @brief  	this function reads temperature.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_chip_micrel_ks8893_configure(hal_eth_phymode_t targetphymode, hal_eth_phymode_t* usedphymode);


extern hal_result_t hal_chip_micrel_ks8893_mii_getphymode(hal_eth_phymode_t* usedphymode);



/** @}            
    end of group arm_hal_chip_micrel_ks8893  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



