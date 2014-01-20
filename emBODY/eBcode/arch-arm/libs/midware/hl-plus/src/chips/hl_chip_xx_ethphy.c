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

/* @file       hl_chip_xx_ethphy.c
	@brief      This file implements internals of the temperature sensor module.
	@author     marco.accame@iit.it
    @date       11/11/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_CHIP_XX_ETHPHY)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files
#include "hl_eth.h"

#include "hl_arch.h"


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_xx_ethphy.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_xx_ethphy_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// if HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_AUTONEGOTIATION_ENABLED is set, then bits 8, 9 and 13 are ignored 

#define HL_CHIP_XX_ETHPHY__PHYDEV_ADR                               0x01
    
#define HL_CHIP_XX_ETHPHY__REGADR_CNTRL                             0x00

#define HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_RESET                       (1 << 15)
#define HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_LOOBACK                     (1 << 14)
#define HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_SPEED100M                   (1 << 13)
#define HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_AUTONEGOTIATION_ENABLED     (1 << 12)
#define HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_POWERDOWN                   (1 << 11)
#define HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_AUTONEGOTIATION_RESTART     (1 << 9)
#define HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_FULLDUPLEX                  (1 << 8)


#define HL_CHIP_XX_ETHPHY__PHYDEV_IS_IN_RESET_STATE(ctrlregval)     ((ctrlregval) & (HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_RESET | HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_POWERDOWN))

#define HL_CHIP_XX_ETHPHY__PHYREG_WRITE_TIMEOUT                     0x10000


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hl_chip_xx_ethphy_cfg_t hl_chip_xx_ethphy_cfg_default  = 
{ 
    .chip = hl_chip_xx_ethphy_chip_autodetect
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hl_chip_xx_ethphy_cfg_t                 config;
} hl_chip_xx_ethphy_internal_item_t;

typedef struct
{
    hl_bool_t                               initted;
    hl_chip_xx_ethphy_internal_item_t*      items[1];   
} hl_chip_xx_ethphy_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_chip_xx_ethphy_initted_set(void);
static hl_bool_t s_hl_chip_xx_ethphy_initted_is(void);

static hl_result_t s_hl_chip_xx_ethphy_hw_init(const hl_chip_xx_ethphy_cfg_t *cfg, hl_chip_xx_ethphy_internal_item_t* intitem);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static hl_chip_xx_ethphy_internal_item_t s_hl_chip_xx_ethphy_theonlyitem = { .config.chip = hl_chip_xx_ethphy_chip_autodetect };   // so far it is the only one.

static hl_chip_xx_ethphy_theinternals_t s_hl_chip_xx_ethphy_theinternals =
{
    .initted            = hl_false,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hl_result_t hl_chip_xx_ethphy_init(const hl_chip_xx_ethphy_cfg_t *cfg)
{
    hl_chip_xx_ethphy_internal_item_t *intitem = s_hl_chip_xx_ethphy_theinternals.items[0];
     
    if(NULL == cfg)
    {
        cfg  = &hl_chip_xx_ethphy_cfg_default;
        //return(hl_res_NOK_generic);
    }
    
    if(hl_true == s_hl_chip_xx_ethphy_initted_is())
    {
        return(hl_res_OK);
    }  

    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        //intitem = s_hl_chip_xx_ethphy_theinternals.items[0] = hl_sys_heap_new(sizeof(hl_chip_xx_ethphy_internal_item_t)); 
        intitem = s_hl_chip_xx_ethphy_theinternals.items[0] = &s_hl_chip_xx_ethphy_theonlyitem;        
    }    

    if(hl_res_OK != s_hl_chip_xx_ethphy_hw_init(cfg, intitem))
    {
        return(hl_res_NOK_generic);
    }
    
    s_hl_chip_xx_ethphy_initted_set();

    return(hl_res_OK);
}


extern hl_result_t hl_chip_xx_ethphy_configure(hl_ethtrans_phymode_t targetphymode, hl_ethtrans_phymode_t* usedphymode)
{
    //hl_result_t res = hl_res_NOK_generic;
    
    if(hl_false == s_hl_chip_xx_ethphy_initted_is())
    {
        return(hl_res_NOK_generic);
    }     
    

#if 0
    // configure phy in full duplex and 100MB
    hl_eth_smi_write(HL_CHIP_XX_ETHPHY__PHYDEV_ADR, HL_CHIP_XX_ETHPHY__REGADR_CNTRL, HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_FULLDUPLEX | HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_SPEED100M); 
    
    *phymode = hl_ethtrans_phymode_fullduplex100mbps;;
 
//     uint16_t regv;
//     uint32_t tout;
//     static uint16_t aaa = 0;    
//     regv = hl_eth_smi_read(HL_CHIP_XX_ETHPHY__PHYDEV_ADR, 2);    
//     aaa = regv;
//     for(;;);
#else
       
    #warning --> now we dont support the hl_ethtrans_phymode_auto: change it 
    if((hl_ethtrans_phymode_auto == targetphymode) || (hl_ethtrans_phymode_none == targetphymode))
    {
        if(NULL != usedphymode)
        {
            *usedphymode = hl_ethtrans_phymode_none;
        }   
        return(hl_res_NOK_generic);        
    }
    
    // configure phy according to the target mux and speed
    uint16_t val = 0x0000;
    switch(targetphymode)
    {
        case hl_ethtrans_phymode_halfduplex10mbps:      val = 0x0000; break;
        case hl_ethtrans_phymode_halfduplex100mbps:     val = HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_SPEED100M; break;
        case hl_ethtrans_phymode_fullduplex10mbps:      val = HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_FULLDUPLEX; break;
        case hl_ethtrans_phymode_fullduplex100mbps:     val = HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_FULLDUPLEX | HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_SPEED100M; break;
        default:                                        val = 0x0000; break;
    }
 
    hl_eth_smi_write(HL_CHIP_XX_ETHPHY__PHYDEV_ADR, HL_CHIP_XX_ETHPHY__REGADR_CNTRL, val); 
    
    
    // gives back the used mux and speed
    hl_chip_xx_ethphy_getphymode(usedphymode);

 
    return(hl_res_OK);
    
#endif
    
}


extern hl_result_t hl_chip_xx_ethphy_getphymode(hl_ethtrans_phymode_t* usedphymode)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    if(NULL == usedphymode)
    {
         return(hl_res_NOK_generic);
    }
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    
    uint16_t val = 0;
    val = hl_eth_smi_read(HL_CHIP_XX_ETHPHY__PHYDEV_ADR, HL_CHIP_XX_ETHPHY__REGADR_CNTRL); 
    val = val;
    
    // gives back the used mux and speed

    uint8_t speed  = ((val & HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_SPEED100M) == HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_SPEED100M) ? 1 : 0;
    uint8_t mux    = ((val & HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_FULLDUPLEX) == HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_FULLDUPLEX) ? 1 : 0; 
    if((0==mux)&&(0==speed))        *usedphymode = hl_ethtrans_phymode_halfduplex10mbps;
    else if((0==mux)&&(1==speed))   *usedphymode = hl_ethtrans_phymode_halfduplex100mbps; 
    else if((1==mux)&&(0==speed))   *usedphymode = hl_ethtrans_phymode_fullduplex10mbps; 
    else if((1==mux)&&(1==speed))   *usedphymode = hl_ethtrans_phymode_fullduplex100mbps;         
  
    return(hl_res_OK);    
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hl_chip_xx_ethphy_initted_set(void)
{
    s_hl_chip_xx_ethphy_theinternals.initted = hl_true;
}

static hl_bool_t s_hl_chip_xx_ethphy_initted_is(void)
{
    return(s_hl_chip_xx_ethphy_theinternals.initted);
}


static hl_result_t s_hl_chip_xx_ethphy_hw_init(const hl_chip_xx_ethphy_cfg_t *cfg, hl_chip_xx_ethphy_internal_item_t* intitem)
{
    //hl_result_t res = hl_res_NOK_generic;   

    uint16_t regv;
    uint32_t tout;
    
    #warning --> chi inizializza lo smi? chi ne ha bisogno? 
    // 1. initialises smi
    hl_eth_smi_init();
    
	// reset the phy device (address is 1)
    hl_eth_smi_write(HL_CHIP_XX_ETHPHY__PHYDEV_ADR, HL_CHIP_XX_ETHPHY__REGADR_CNTRL, HL_CHIP_XX_ETHPHY__REGVAL_CNTRL_RESET);

	// wait for hardware reset to end 
    for(tout = 0; tout<HL_CHIP_XX_ETHPHY__PHYREG_WRITE_TIMEOUT; tout++) 
    {
        regv = hl_eth_smi_read(HL_CHIP_XX_ETHPHY__PHYDEV_ADR, HL_CHIP_XX_ETHPHY__REGADR_CNTRL);
        if (!HL_CHIP_XX_ETHPHY__PHYDEV_IS_IN_RESET_STATE(regv))
        {
            // reset complete
            break;
        }
    }
    
    
    if(HL_CHIP_XX_ETHPHY__PHYREG_WRITE_TIMEOUT == tout) // ethernet is still in reset state 
    {
        hl_sys_on_error(hl_error_runtimefault, "s_hl_chip_xx_ethphy_hw_init(): PHY is still in reset state");
        return(hl_res_NOK_generic);
    }  

    
    memcpy(&intitem->config, cfg, sizeof(hl_chip_xx_ethphy_cfg_t));
    
    return(hl_res_OK);
}




#endif//defined(HL_USE_CHIP_XX_ETHPHY)


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



