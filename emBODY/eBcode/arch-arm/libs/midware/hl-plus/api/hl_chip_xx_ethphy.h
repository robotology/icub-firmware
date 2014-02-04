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

#ifndef _HL_CHIP_XX_ETHPHY_H_
#define _HL_CHIP_XX_ETHPHY_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_chip_xx_ethphy.h
    @brief      This header file implements interface to a number of ethernet phy chips
    @author     marco.accame@iit.it
    @date       11/11/2013
**/

/** @defgroup arm_hl_chip_xx_ethphy HL DEV_PHY

    The HL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_ethtrans.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hl_chip_xx_ethphy_chip_t 
    @brief      contains every supported PHY chip
 **/ 
typedef enum  
{ 
    hl_chip_xx_ethphy_chip_ti_dp83848       = 0,    /**< capacity 8kbytes; page size 4 bytes; i2c speeds: 400khz. it is also wireless, but we don't care. */
    hl_chip_xx_ethphy_chip_st_802rt1a       = 1,
    hl_chip_xx_ethphy_chip_autodetect       = 15,   /**< it detects the chip */
    hl_chip_xx_ethphy_chip_none             = 255   /**< just no device ... yet. */
} hl_chip_xx_ethphy_chip_t;



/** @typedef    typedef enum hl_chip_xx_ethphy_cfg_t 
    @brief      hl_chip_xx_ethphy_cfg_t contains the configuration for the phy.
 **/
typedef struct
{
    hl_chip_xx_ethphy_chip_t            chip;
    hl_ethtrans_phymode_t               targetphymode;
} hl_chip_xx_ethphy_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_chip_xx_ethphy_cfg_t hl_chip_xx_ethphy_cfg_default; // = {.chip = hl_chip_xx_ethphy_chip_autodetect};


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hl_result_t hl_chip_xx_ethphy_init(const hl_chip_xx_ethphy_cfg_t *cfg)
    @brief  	this function initializes a eth phy chip 
    @param  	cfg 	        pointer to configuration
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_xx_ethphy_init(const hl_chip_xx_ethphy_cfg_t *cfg);


/** @fn			extern hl_result_t hl_chip_xx_ethphy_configure(hl_ethtrans_phymode_t targetphymode, hl_ethtrans_phymode_t* usedphymode)
    @brief  	this function configures the phy.
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_xx_ethphy_configure(hl_ethtrans_phymode_t* usedphymode);


extern hl_result_t hl_chip_xx_ethphy_getphymode(hl_ethtrans_phymode_t* usedphymode);


/** @}            
    end of group arm_hl_chip_xx_ethphy  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



