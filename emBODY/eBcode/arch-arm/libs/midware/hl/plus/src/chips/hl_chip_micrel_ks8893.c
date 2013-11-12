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

/* @file       hl_chip_micrel_ks8893.c
	@brief      This file implements internals of the chip micrel ks8893.
	@author     marco.accame@iit.it
    @date       11/1/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------


#include "hl_cfg_plus_modules.h"

#if     defined(HL_USE_CHIP_MICREL_KS8893)


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files
#include "hl_eth.h"
#include "hl_arch.h"


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_micrel_ks8893.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_micrel_ks8893_hid.h"


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

#if 0
extern const hl_chip_micrel_ks8893_cfg_t hl_chip_micrel_ks8893_cfg_default  = 
{ 
    .i2cid          = hl_i2c1,
    .resetpin       = 
    {   
        .port = hl_gpio_portNONE,  
        .pin  = hl_gpio_pinNONE
    },
    .resetval      = hl_gpio_valRESET
    .extclockinit   = NULL
};
#endif

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hl_chip_micrel_ks8893_cfg_t        config;
} hl_chip_micrel_ks8893_internal_item_t;

typedef struct
{
    hl_bool_t                                  initted;
    hl_chip_micrel_ks8893_internal_item_t*     items[1];   
} hl_chip_micrel_ks8893_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_chip_micrel_ks8893_initted_set(void);
static hl_bool_t s_hl_chip_micrel_ks8893_initted_is(void);

static hl_result_t s_hl_chip_micrel_ks8893_hw_init(const hl_chip_micrel_ks8893_cfg_t *cfg, hl_chip_micrel_ks8893_internal_item_t *intitem);

static void s_hl_chip_micrel_ks8893_phymode_get(hl_eth_phymode_t* usedphymode);

static hl_result_t s_hl_chip_micrel_ks8893_resetpin_init(hl_gpio_t gpio);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static hl_chip_micrel_ks8893_internal_item_t hl_chip_micrel_ks8893_theonlyitem = { 
    .config     = { .i2cid = hl_i2c1, .resetpin = { .port = hl_gpio_portNONE, .pin  = hl_gpio_pinNONE }, .resetval = hl_gpio_valRESET, .extclockinit = NULL }, 
};  // so far it is the only one.

static hl_chip_micrel_ks8893_theinternals_t s_hl_chip_micrel_ks8893_theinternals =
{
    .initted            = hl_false,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hl_result_t hl_chip_micrel_ks8893_init(const hl_chip_micrel_ks8893_cfg_t *cfg)
{   
    hl_chip_micrel_ks8893_internal_item_t *intitem = s_hl_chip_micrel_ks8893_theinternals.items[0];
    
    if(NULL == cfg)
    {
        hl_sys_on_error(hl_error_missingconfiguration, "cfg is NULL in hl_chip_micrel_ks8893_init");
        return(hl_res_NOK_generic);
    }
    
    if(hl_true == s_hl_chip_micrel_ks8893_initted_is())
    {
        return(hl_res_OK);
    }  
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        //intitem = s_hl_chip_micrel_ks8893_theinternals.items[0] = hl_sys_heap_new(sizeof(hl_chip_micrel_ks8893_internal_item_t));   
        intitem = s_hl_chip_micrel_ks8893_theinternals.items[0] = &hl_chip_micrel_ks8893_theonlyitem;
    } 


    if(hl_res_OK != s_hl_chip_micrel_ks8893_hw_init(cfg, intitem))
    {
        return(hl_res_NOK_generic);
    }
    
    s_hl_chip_micrel_ks8893_initted_set();

    return(hl_res_OK);
}


extern hl_result_t hl_chip_micrel_ks8893_configure(hl_eth_phymode_t targetphymode, hl_eth_phymode_t* usedphymode)
{
    hl_chip_micrel_ks8893_internal_item_t *intitem = s_hl_chip_micrel_ks8893_theinternals.items[0];
    
    //const uint8_t fd100 = 0x60;
    //const uint8_t fd010 = 0x20;
    uint8_t buff_write = 0x60; // FORCE FULL DUPLEX AND 100T
    uint8_t buff_read = 0xFF; 
    volatile uint32_t i = 1;
    hl_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
    
    if(hl_false == s_hl_chip_micrel_ks8893_initted_is())
    {
        return(hl_res_NOK_generic);
    }  

    hl_i2c_t i2cid = intitem->config.i2cid;

    regadr.bytes.one = REG0x01;
    hl_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    if((buff_read&0x01))
    {   // already initted. to be initted again must pass through a reset
        if(NULL != usedphymode)
        {
            s_hl_chip_micrel_ks8893_phymode_get(usedphymode);
        }
        return(hl_res_OK);
    }
    
    
    if(hl_eth_phymode_none == targetphymode)
    {
        if(NULL != usedphymode)
        {
            *usedphymode = hl_eth_phymode_none;    
        }   
        return(hl_res_OK);        
    }


    

#if 0    
    // configure mii
    regadr.bytes.one = 0x06;
    hl_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    buff_write  = buff_read;
    buff_write |= 0x10; 
    hl_i2c_write(i2cid, I2CADDRESS, regadr, &buff_write, 1);
    hl_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
#endif

    // 1. configure  switch's ports 1 and 2
    switch(targetphymode)
    {
        case hl_eth_phymode_auto:                   buff_write = 0x9F; break;
        case hl_eth_phymode_halfduplex10mbps:       buff_write = 0x00; break;
        case hl_eth_phymode_halfduplex100mbps:      buff_write = 0x40; break;
        case hl_eth_phymode_fullduplex10mbps:       buff_write = 0x20; break;
        case hl_eth_phymode_fullduplex100mbps:      buff_write = 0x60; break;
        default:                                    buff_write = 0x00; break;
    }    
    
    
    // port 1
    regadr.bytes.one = REG0x1C;
    hl_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    hl_i2c_write(i2cid, I2CADDRESS, regadr, &buff_write, 1);
    hl_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
     
    // port 2 
    regadr.bytes.one = REG0x2C;
    hl_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    hl_i2c_write(i2cid, I2CADDRESS, regadr, &buff_write, 1);   
    hl_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);

    // 2. start the switch
    buff_write = 0x1;  
    regadr.bytes.one = REG0x01;    
    hl_i2c_write(i2cid, I2CADDRESS, regadr, &buff_write, 1);
    

    // 3. read back to verify
    regadr.bytes.one = REG0x01;
    hl_i2c_read(i2cid, I2CADDRESS, regadr, &buff_read, 1);
    if(!(buff_read&0x01))
    {
        hl_sys_on_error(hl_error_runtimefault, "hl_chip_micrel_ks8893_configure(): SWITCH not configured");
    }
    
    s_hl_chip_micrel_ks8893_phymode_get(usedphymode);

 
    return(hl_res_OK);
}


extern hl_result_t hl_chip_micrel_ks8893_mii_getphymode(hl_eth_phymode_t* usedphymode)
{
    if(hl_false == s_hl_chip_micrel_ks8893_initted_is())
    {
        return(hl_res_NOK_generic);
    }  
    
    s_hl_chip_micrel_ks8893_phymode_get(usedphymode);
    return(hl_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



extern hl_result_t hl_chip_micrel_ks8893_hid_static_memory_init(void)
{
    memset(&s_hl_chip_micrel_ks8893_theinternals, 0, sizeof(s_hl_chip_micrel_ks8893_theinternals));
    return(hl_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hl_chip_micrel_ks8893_initted_set(void)
{
    s_hl_chip_micrel_ks8893_theinternals.initted = hl_true;
}

static hl_bool_t s_hl_chip_micrel_ks8893_initted_is(void)
{
    return(s_hl_chip_micrel_ks8893_theinternals.initted);
}


static hl_result_t s_hl_chip_micrel_ks8893_hw_init(const hl_chip_micrel_ks8893_cfg_t *cfg, hl_chip_micrel_ks8893_internal_item_t *intitem)
{   
    hl_i2c_t i2cid = cfg->i2cid;
    
    
    // 1. init external clock.
    
    if((NULL == cfg->extclockinit) || (hl_res_OK != cfg->extclockinit()))
    {
        return(hl_res_NOK_generic);
    }
    
    // 2. init reset pin
    s_hl_chip_micrel_ks8893_resetpin_init(cfg->resetpin);
 
    // 3. reset the micrel
    hl_gpio_val_t resetval = cfg->resetval;
    hl_gpio_pin_write(cfg->resetpin, resetval);
    hl_sys_delay(10*1000);
    hl_gpio_pin_write(cfg->resetpin, (hl_gpio_valRESET == resetval) ? (hl_gpio_valSET) : (hl_gpio_valRESET));
    hl_sys_delay(100);    
 
    // 4. verify if i2c is initted  
    if(hl_false == hl_i2c_initted_is(i2cid))
    {
        return(hl_res_NOK_generic);
    }
    
    
    // 5. verify that the micrel is responding
    
    if(hl_res_OK != hl_i2c_ping(i2cid, I2CADDRESS))
    {
        return(hl_res_NOK_generic);
    }
    
//#warning INFO --> could add verification that on ic2 the device at that address is really a micrel switch ...    
#if 0        
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    uint8_t data;

    // whoami: value must be WHOAMI_VAL
    regaddr.bytes.one = WHOAMI_ADR;
    if(hl_res_OK != (res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    if((hl_res_OK != res) || (WHOAMI_VAL != data))
    {
        return(hl_res_NOK_generic);
    }
#endif        
    
    
    memcpy(&intitem->config, cfg, sizeof(hl_chip_micrel_ks8893_cfg_t));
    
    return(hl_res_OK);
}



static void s_hl_chip_micrel_ks8893_phymode_get(hl_eth_phymode_t* usedphymode)
{
    hl_chip_micrel_ks8893_internal_item_t *intitem = s_hl_chip_micrel_ks8893_theinternals.items[0];
    hl_i2c_t i2cid = intitem->config.i2cid;
    
    #define MIIHLFD    (1 << 6)
    #define MII10MBS    (1 << 4)
    uint8_t mux = 0;
    uint8_t speed = 0;
    uint8_t read = 0xFF; 
    hl_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
    regadr.bytes.one = 0x06;
    hl_i2c_read(i2cid, I2CADDRESS, regadr, &read, 1);
    if( (MIIHLFD & read) == MIIHLFD)
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
    if((0==mux)&&(0==speed))        *usedphymode = hl_eth_phymode_halfduplex10mbps;
    else if((0==mux)&&(1==speed))   *usedphymode = hl_eth_phymode_halfduplex100mbps; 
    else if((1==mux)&&(0==speed))   *usedphymode = hl_eth_phymode_fullduplex10mbps; 
    else if((1==mux)&&(1==speed))   *usedphymode = hl_eth_phymode_fullduplex100mbps;          
    
}



static hl_result_t s_hl_chip_micrel_ks8893_resetpin_init(hl_gpio_t gpio)
{
    hl_result_t res = hl_res_OK;
    
    static const hl_gpio_init_t outgpioinit = 
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins  = 0,
            .gpio_speed = GPIO_Speed_50MHz,
            .gpio_mode  = GPIO_Mode_Out_PP            
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins  = 0,
            .gpio_mode  = GPIO_Mode_OUT,
            .gpio_speed = GPIO_Speed_50MHz,
            .gpio_otype = GPIO_OType_PP,
            .gpio_pupd  = GPIO_PuPd_UP
        }
#else
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif        
    };
    

    hl_gpio_init_t gpioinit;
    hl_gpio_map_t gpiomap;
    memcpy(&gpioinit, &outgpioinit, sizeof(hl_gpio_init_t));
    gpiomap.gpio.port = gpio.port;
    gpiomap.gpio.pin  = gpio.pin;
    gpiomap.af32      = hl_NA32;
    hl_gpio_fill_init(&gpioinit, &gpiomap);
    res = hl_gpio_init(&gpioinit);  
    // no altcfg ...
   
    return(res);
}


#endif//defined(HL_USE_CHIP_MICREL_KS8893)



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



