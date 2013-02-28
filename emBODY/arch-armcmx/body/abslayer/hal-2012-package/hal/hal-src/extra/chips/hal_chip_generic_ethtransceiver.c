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

/* @file       hal_chip_generic_ethtransceiver.c
	@brief      This file implements internals of the temperature sensor module.
	@author     marco.accame@iit.it
    @date       10/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef  HAL_USE_CHIP_GENERIC_ETHTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_base_hid.h" 
#include "hal_mpu_eth_hid.h" 
#include "hal_brdcfg.h"


#include "stdio.h"

//#include "hal_stm32xx_include.h"



 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_chip_generic_ethtransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_chip_generic_ethtransceiver_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// the phy used by this board is the ST802RT1x. of this phy we just use the Basic Mode Control Register (BMCR) and impose 100mbps full duplex

#define HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_ADR                              0x01
    
#define HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_ADR                         0x00

#define HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_RESET                   (1 << 15)
#define HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_100M                    (1 << 13)
#define HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_POWERDOWN               (1 << 11)
#define HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_FULLDUPLEX              (1 << 8)


#define HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_IS_IN_RESET_STATE(bmrcval)	    ((bmrcval) & (HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_RESET | HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_POWERDOWN))

#define HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_WRITE_TIMEOUT                    0x10000


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_chip_generic_ethtransceiver_cfg_t hal_chip_generic_ethtransceiver_cfg_default  = 
{ 
    .dummy        = 0
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_chip_generic_ethtransceiver_cfg_t    cfg;
} hal_chip_generic_ethtransceiver_info_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_chip_generic_ethtransceiver_initted_set(void);
static hal_boolval_t s_hal_chip_generic_ethtransceiver_initted_is(void);

static hal_result_t s_hal_chip_generic_ethtransceiver_hw_init(const hal_chip_generic_ethtransceiver_cfg_t *cfg, hal_chip_generic_ethtransceiver_info_t* info);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_chip_generic_ethtransceiver_initted[1] = { hal_false };
static hal_chip_generic_ethtransceiver_info_t s_hal_chip_generic_ethtransceiver_info[1] = { {.cfg = { .dummy = 0} } };



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_chip_generic_ethtransceiver_init(const hal_chip_generic_ethtransceiver_cfg_t *cfg)
{
    //hal_result_t res = hal_res_NOK_generic; // dont remove ...
     
    if(NULL == cfg)
    {
        cfg  = &hal_chip_generic_ethtransceiver_cfg_default;
        //return(hal_res_NOK_generic);
    }
    
    if(hal_true == s_hal_chip_generic_ethtransceiver_initted_is())
    {
        return(hal_res_OK);
    }    

    if(hal_res_OK != s_hal_chip_generic_ethtransceiver_hw_init(cfg, &s_hal_chip_generic_ethtransceiver_info[0]))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_chip_generic_ethtransceiver_initted_set();

    return(hal_res_OK);
}


extern hal_result_t hal_chip_generic_ethtransceiver_configure(hal_eth_phymode_t targetphymode, hal_eth_phymode_t* usedphymode)
{
    //hal_result_t res = hal_res_NOK_generic;
    

#if 0
    // configure phy in full duplex and 100MB
    hal_eth_hid_smi_write(HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_ADR, HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_ADR, HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_FULLDUPLEX | HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_100M); 
    
    *phymode = hal_eth_phymode_fullduplex100mbps;;
 
//     uint16_t regv;
//     uint32_t tout;
//     static uint16_t aaa = 0;    
//     regv = hal_eth_hid_smi_read(HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_ADR, 2);    
//     aaa = regv;
//     for(;;);
#else
        
    if((hal_eth_phymode_auto == targetphymode) || (hal_eth_phymode_none == targetphymode))
    {
        if(NULL != usedphymode)
        {
            *usedphymode = hal_eth_phymode_none;
        }   
        return(hal_res_NOK_generic);        
    }
    
    // configure phy according to the target mux and speed
    uint16_t val = 0x0000;
    switch(targetphymode)
    {
        case hal_eth_phymode_halfduplex10mbps:      val = 0x0000; break;
        case hal_eth_phymode_halfduplex100mbps:     val = HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_100M; break;
        case hal_eth_phymode_fullduplex10mbps:      val = HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_FULLDUPLEX; break;
        case hal_eth_phymode_fullduplex100mbps:     val = HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_FULLDUPLEX | HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_100M; break;
        default:                                    val = 0x0000; break;
    }
 
    hal_eth_hid_smi_write(HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_ADR, HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_ADR, val); 
    
//    val = 0;
//    val = hal_eth_hid_smi_read(HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_ADR, HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_ADR); 
//    val = val;
    
    // gives back the used mux and speed
    hal_chip_generic_ethtransceiver_getphymode(usedphymode);

 
    return(hal_res_OK);
    
#endif
    
}


extern hal_result_t hal_chip_generic_ethtransceiver_getphymode(hal_eth_phymode_t* usedphymode)
{

    if(NULL != usedphymode)
    {
        uint16_t val = 0;
        val = hal_eth_hid_smi_read(HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_ADR, HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_ADR); 
        val = val;
        
        // gives back the used mux and speed
    
        uint8_t speed  = ((val & HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_100M) == HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_100M) ? 1 : 0;
        uint8_t mux    = ((val & HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_FULLDUPLEX) == HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_FULLDUPLEX) ? 1 : 0; 
        if((0==mux)&&(0==speed))        *usedphymode = hal_eth_phymode_halfduplex10mbps;
        else if((0==mux)&&(1==speed))   *usedphymode = hal_eth_phymode_halfduplex100mbps; 
        else if((1==mux)&&(0==speed))   *usedphymode = hal_eth_phymode_fullduplex10mbps; 
        else if((1==mux)&&(1==speed))   *usedphymode = hal_eth_phymode_fullduplex100mbps;         
    } 
    else    
    {
        return(hal_res_NOK_generic);
    }
 
    return(hal_res_OK);    
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern hal_result_t hal_chip_generic_ethtransceiver_hid_static_memory_init(void)
{
    memset(s_hal_chip_generic_ethtransceiver_initted, hal_false, sizeof(s_hal_chip_generic_ethtransceiver_initted));
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hal_chip_generic_ethtransceiver_initted_set(void)
{
    s_hal_chip_generic_ethtransceiver_initted[0] = hal_true;
}

static hal_boolval_t s_hal_chip_generic_ethtransceiver_initted_is(void)
{
    return(s_hal_chip_generic_ethtransceiver_initted[0]);
}


static hal_result_t s_hal_chip_generic_ethtransceiver_hw_init(const hal_chip_generic_ethtransceiver_cfg_t *cfg, hal_chip_generic_ethtransceiver_info_t* info)
{
    //hal_result_t res = hal_res_NOK_generic;   

    uint16_t regv;
    uint32_t tout;
    
    // 1. initialises smi
    hal_eth_hid_smi_init();
    
	// reset the phy device (address is 1)
    hal_eth_hid_smi_write(HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_ADR, HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_ADR, HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_VAL_RESET);

	// wait for hardware reset to end 
    for(tout = 0; tout<HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_WRITE_TIMEOUT; tout++) 
    {
        regv = hal_eth_hid_smi_read(HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_ADR, HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_BMRC_ADR);
        if (!HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYDEV_IS_IN_RESET_STATE(regv))
        {
            // reset complete
            break;
        }
    }
    
    
    if(HAL_CHIP_GENERIC_ETHTRANSCEIVER__PHYREG_WRITE_TIMEOUT == tout) //ethernet is still in reset state 
    {
        hal_base_on_fatalerror(hal_fatalerror_runtimefault, "hal_brdcfg_eth__phy_start(): PHY is still in reset state");
        return(hal_res_NOK_generic);
    }  

    
    memcpy(&info->cfg, cfg, sizeof(hal_chip_generic_ethtransceiver_cfg_t));
    
    return(hal_res_OK);
}




#endif//HAL_USE_CHIP_GENERIC_ETHTRANSCEIVER


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



