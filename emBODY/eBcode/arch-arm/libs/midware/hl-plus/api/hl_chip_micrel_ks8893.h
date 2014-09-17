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

#ifndef _HL_CHIP_MICREL_KS8893_H_
#define _HL_CHIP_MICREL_KS8893_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_chip_micrel_ks8893.h
    @brief      This header file implements interface to a micrel eth switch KS8893
    @author     marco.accame@iit.it
    @date       11/11/2013
**/

/** @defgroup arm_hl_chip_micrel_ks8893 HL DEV_ACC

    The HL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_ethtrans.h"
#include "hl_i2c.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 




/** @typedef    typedef enum hl_chip_micrel_ks8893_cfg_t 
    @brief      hl_chip_micrel_ks8893_cfg_t contains the configuration for the eth switch.
 **/
typedef struct
{
    hl_i2c_t                i2cid;          /**< the I2C bus used by the chip */
    hl_gpio_t               resetpin;       /**< the pinout of the reset pin */
    hl_gpio_val_t           resetval;       /**< the value which puts the chip in reset mode */
    hl_res_fp_void_t        extclockinit;   /**< the function which initialises the external clock used by the chip */
    hl_ethtrans_phymode_t   targetphymode;
    hl_ethtrans_xcorr_t     xcorrection;
} hl_chip_micrel_ks8893_cfg_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hl_result_t hl_chip_micrel_ks8893_init(const hl_chip_micrel_ks8893_cfg_t *cfg)
    @brief  	this function initializes a micrel ks8893 
    @param  	cfg 	        pointer to configuration data. it cannot be NULL
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_micrel_ks8893_init(const hl_chip_micrel_ks8893_cfg_t *cfg);


/** @fn			extern hl_result_t hl_chip_micrel_ks8893_start(hl_ethtrans_phymode_t* usedmiiphymode)
    @brief  	this function starts the switch as previously initted and retrieve the used phy mode for mii.
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_micrel_ks8893_start(hl_ethtrans_phymode_t* usedmiiphymode);


/** @fn			extern hl_result_t hl_chip_micrel_ks8893_mii_getphymode(hl_ethtrans_phymode_t* usedphymode)
    @brief  	this function retrieves the used phy mode.
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_micrel_ks8893_mii_getphymode(hl_ethtrans_phymode_t* usedphymode);


/** @fn			extern hl_result_t hl_chip_micrel_ks8893_linkupmask(uint8_t* linkmask)
    @brief  	this function retrieves the binary status of the links (on = 1b / off = 0b).
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_micrel_ks8893_linkupmask(uint8_t* linkmask);

extern hl_result_t hl_chip_micrel_ks8893_phy_status(hl_ethtrans_phystatus_t* phyarray, uint8_t arraysize);

extern hl_result_t hl_chip_micrel_ks8893_phy_errorinfo(uint8_t phynum, hl_ethtrans_phyerror_t errortype, hl_ethtrans_phyerrorinfo_t *result);



/** @}            
    end of group arm_hl_chip_micrel_ks8893  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



