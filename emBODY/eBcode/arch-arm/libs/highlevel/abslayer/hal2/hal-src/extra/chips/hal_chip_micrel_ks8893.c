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

/* @file       hal_chip_micrel_ks8893.c
	@brief      This file implements internals of the temperature sensor module.
	@author     marco.accame@iit.it
    @date       10/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef  HAL_USE_CHIP_MICREL_KS8893

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"


#include "stdio.h"

//#include "hal_stm32xx_include.h"



 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_chip_micrel_ks8893.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_chip_micrel_ks8893_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define I2CADDRESS          0xBE  

#define REG0x01             0x01    
#define REG0x1C             0x1C    
#define REG0x2C             0x2C

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_chip_micrel_ks8893_cfg_t hal_chip_micrel_ks8893_cfg_default  = 
{ 
    .i2cid          = hal_i2c1,
    .resetpin       = 
    {   
        .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
        .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_NONE,   .altcfg = NULL }
    },
    .extclockinit   = NULL
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_chip_micrel_ks8893_cfg_t        config;
} hal_chip_micrel_ks8893_internal_item_t;

typedef struct
{
    hal_bool_t                                  initted;
    hal_chip_micrel_ks8893_internal_item_t*     items[1];   
} hal_chip_micrel_ks8893_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_chip_micrel_ks8893_initted_set(void);
static hal_bool_t s_hal_chip_micrel_ks8893_initted_is(void);

static hal_result_t s_hal_chip_micrel_ks8893_hw_init(const hal_chip_micrel_ks8893_cfg_t *cfg, hal_chip_micrel_ks8893_internal_item_t *intitem);

static void s_hal_chip_micrel_ks8893_phymode_get(hal_eth_phymode_t* usedphymode);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_chip_micrel_ks8893_theinternals_t s_hal_chip_micrel_ks8893_theinternals =
{
    .initted            = hal_false,
    .items              = { NULL }   
};

//#warning --> fix the internals
//static hal_boolval_t s_hal_chip_micrel_ks8893_initted[1] = { hal_false };
//static hal_chip_micrel_ks8893_internals_t s_hal_chip_micrel_ks8893_info[1] = { {.config = { .i2cid = hal_i2c1} } };



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_chip_micrel_ks8893_init(const hal_chip_micrel_ks8893_cfg_t *cfg)
{   
    hal_chip_micrel_ks8893_internal_item_t *intitem = s_hal_chip_micrel_ks8893_theinternals.items[0];
    if(NULL == cfg)
    {
        hal_base_on_fatalerror(hal_fatalerror_missingconfiguration, "in hal_chip_micrel_ks8893_init");
        return(hal_res_NOK_generic);
    }
    
    if(hal_true == s_hal_chip_micrel_ks8893_initted_is())
    {
        return(hal_res_OK);
    }  
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_chip_micrel_ks8893_theinternals.items[0] = hal_heap_new(sizeof(hal_chip_micrel_ks8893_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    } 

    

    if(hal_res_OK != s_hal_chip_micrel_ks8893_hw_init(cfg, intitem))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_chip_micrel_ks8893_initted_set();

    return(hal_res_OK);
}


extern hal_result_t hal_chip_micrel_ks8893_configure(hal_eth_phymode_t targetphymode, hal_eth_phymode_t* usedphymode)
{
    hal_chip_micrel_ks8893_internal_item_t *intitem = s_hal_chip_micrel_ks8893_theinternals.items[0];
    
    //const uint8_t fd100 = 0x60;
    //const uint8_t fd010 = 0x20;
    //uint8_t buff_write = 0x60; //FORCE FULL DUPLEX AND 100T // VALE commented old values
    uint8_t buff_write = 0x88; //AUTONEGOTATION + ADVERTISE FULLDUPLEX 100 // VALE added new values
    uint8_t buff_read = 0xFF; 
    volatile uint32_t i = 1;
    hal_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
    
    if(hal_false == s_hal_chip_micrel_ks8893_initted_is())
    {
        return(hal_res_NOK_generic);
    }  

    hal_i2c_t i2cid = intitem->config.i2cid;

    regadr.bytes.one = REG0x01;
    hal_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    if((buff_read&0x01))
    {   // already initted. to be initted again must pass through a reset
        if(NULL != usedphymode)
        {
            s_hal_chip_micrel_ks8893_phymode_get(usedphymode);
        }
        return(hal_res_OK);
    }
    
    
    if(hal_eth_phymode_none == targetphymode)
    {
        if(NULL != usedphymode)
        {
            *usedphymode = hal_eth_phymode_none;    
        }   
        return(hal_res_OK);        
    }


    

#if 0    
    // configure mii
    regadr.bytes.one = 0x06;
    hal_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    buff_write  = buff_read;
    buff_write |= 0x10; 
    hal_i2c_write(i2cid, I2CADDRESS, regadr, &buff_write, 1);
    hal_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
#endif

    // 1. configure  switch's ports 1 and 2
    switch(targetphymode)
    {
        case hal_eth_phymode_auto:                  buff_write = 0x9F; break;
        case hal_eth_phymode_halfduplex10mbps:      buff_write = 0x00; break;
        case hal_eth_phymode_halfduplex100mbps:     buff_write = 0x40; break;
        case hal_eth_phymode_fullduplex10mbps:      buff_write = 0x20; break;
        case hal_eth_phymode_fullduplex100mbps:     buff_write = 0x60; break;
        default:                                    buff_write = 0x00; break;
    }    
    
    
    // port 1
    regadr.bytes.one = REG0x1C;
    hal_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    hal_i2c_write(i2cid, I2CADDRESS, regadr, &buff_write, 1);
    hal_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
     
    // port 2 
    regadr.bytes.one = REG0x2C;
    hal_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    hal_i2c_write(i2cid, I2CADDRESS, regadr, &buff_write, 1);   
    hal_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);

    // 2. start the switch
    buff_write = 0x1;  
    regadr.bytes.one = REG0x01;    
    hal_i2c_write(i2cid, I2CADDRESS, regadr, &buff_write, 1);
    

    // 3. read back to verify
    regadr.bytes.one = REG0x01;
    hal_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    if(!(buff_read&0x01))
    {
        hal_base_on_fatalerror(hal_fatalerror_runtimefault, "hal_chip_micrel_ks8893_configure(): SWITCH not configured");
    }
    
    s_hal_chip_micrel_ks8893_phymode_get(usedphymode);

 
    return(hal_res_OK);
}


extern hal_result_t hal_chip_micrel_ks8893_mii_getphymode(hal_eth_phymode_t* usedphymode)
{
    if(hal_false == s_hal_chip_micrel_ks8893_initted_is())
    {
        return(hal_res_NOK_generic);
    }  
    
    s_hal_chip_micrel_ks8893_phymode_get(usedphymode);
    return(hal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



extern hal_result_t hal_chip_micrel_ks8893_hid_static_memory_init(void)
{
    memset(&s_hal_chip_micrel_ks8893_theinternals, 0, sizeof(s_hal_chip_micrel_ks8893_theinternals));
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hal_chip_micrel_ks8893_initted_set(void)
{
    s_hal_chip_micrel_ks8893_theinternals.initted = hal_true;
}

static hal_bool_t s_hal_chip_micrel_ks8893_initted_is(void)
{
    return(s_hal_chip_micrel_ks8893_theinternals.initted);
}


static hal_result_t s_hal_chip_micrel_ks8893_hw_init(const hal_chip_micrel_ks8893_cfg_t *cfg, hal_chip_micrel_ks8893_internal_item_t *intitem)
{
    hal_result_t res = hal_res_NOK_generic;   
    
    hal_i2c_t i2cid = cfg->i2cid;
    
    
    // 1. init external clock.
    
    if((NULL == cfg->extclockinit) || (hal_res_OK != cfg->extclockinit()))
    {
        return(hal_res_NOK_generic);
    }
 
    // 2. reset the micrel
    const hal_gpio_map_t *resetpinmap = &cfg->resetpin;
    hal_gpio_init(resetpinmap->gpio, &resetpinmap->config);
    hal_gpio_setval(resetpinmap->gpio, hal_gpio_valLOW);
    hal_sys_delay(10*1000);
    hal_gpio_setval(resetpinmap->gpio, hal_gpio_valHIGH);
    hal_sys_delay(100);    
 
    // 3. init i2c    
    if(hal_res_OK != (res = hal_i2c_init(i2cid, NULL)))
    {
        return(res);
    }
    
    
    // 4. verify that the micrel is responding
    
    if(hal_res_OK != hal_i2c_ping(i2cid, I2CADDRESS))
    {
        return(hal_res_NOK_generic);
    }
    
//#warning INFO --> could add verification that on ic2 the device at that address is really a micrel switch ...    
#if 0        
    hal_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    uint8_t data;

    // whoami: value must be WHOAMI_VAL
    regaddr.bytes.one = WHOAMI_ADR;
    if(hal_res_OK != (res = hal_i2c_read(i2cid, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    if((hal_res_OK != res) || (WHOAMI_VAL != data))
    {
        return(hal_res_NOK_generic);
    }
#endif        
    
    
    memcpy(&intitem->config, cfg, sizeof(hal_chip_micrel_ks8893_cfg_t));
    
    // store the i2caddress and the register address.
    return(hal_res_OK);
}



static void s_hal_chip_micrel_ks8893_phymode_get(hal_eth_phymode_t* usedphymode)
{
    hal_chip_micrel_ks8893_internal_item_t *intitem = s_hal_chip_micrel_ks8893_theinternals.items[0];
    hal_i2c_t i2cid = intitem->config.i2cid;
    
    #define MIIHALFD    (1 << 6)
    #define MII10MBS    (1 << 4)
    uint8_t mux = 0;
    uint8_t speed = 0;
    uint8_t read = 0xFF; 
    hal_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
    regadr.bytes.one = 0x06;
    hal_i2c_read(i2cid, I2CADDRESS, regadr, &read, 1);
    if( (MIIHALFD & read) == MIIHALFD)
    {
        mux = 0;
    }
    else
    {
        mux = 1;
    }
    if( (MII10MBS & read) == MII10MBS)
    {
        speed = 0;
    }
    else
    {
        speed = 1;
    }
    if((0==mux)&&(0==speed))        *usedphymode = hal_eth_phymode_halfduplex10mbps;
    else if((0==mux)&&(1==speed))   *usedphymode = hal_eth_phymode_halfduplex100mbps; 
    else if((1==mux)&&(0==speed))   *usedphymode = hal_eth_phymode_fullduplex10mbps; 
    else if((1==mux)&&(1==speed))   *usedphymode = hal_eth_phymode_fullduplex100mbps;          
    
}


#endif//HAL_USE_CHIP_MICREL_KS8893


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



