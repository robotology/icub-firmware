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
#define REG0x06             0x06  
#define REG0x1C             0x1C    
#define REG0x2C             0x2C

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#if 0
extern const hl_chip_micrel_ks8893_cfg_t hl_chip_micrel_ks8893_cfg_default  = 
{ 
    .i2cid          = hl_i2c1,
    .resetpin       = { .port = hl_gpio_portNONE, .pin  = hl_gpio_pinNONE },
    .resetval       = hl_gpio_valRESET,
    .extclockinit   = NULL,
    .targetphymode  = hl_ethtrans_phymode_auto
};
#endif

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hl_chip_micrel_ks8893_cfg_t                 config;
} hl_chip_micrel_ks8893_internal_item_t;

typedef struct
{
    hl_bool_t                                   initted;
    hl_chip_micrel_ks8893_internal_item_t*      items[1];   
} hl_chip_micrel_ks8893_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_chip_micrel_ks8893_initted_set(void);
static hl_bool_t s_hl_chip_micrel_ks8893_initted_is(void);

static hl_result_t s_hl_chip_micrel_ks8893_hw_init(const hl_chip_micrel_ks8893_cfg_t *cfg, hl_chip_micrel_ks8893_internal_item_t *intitem);

static void s_hl_chip_micrel_ks8893_phymode_set(hl_ethtrans_phymode_t targetphymode);

static void s_hl_chip_micrel_ks8893_mii_phymode_get(hl_ethtrans_phymode_t* usedmiiphymode);

static void s_hl_chip_micrel_ks8893_start(const hl_chip_micrel_ks8893_cfg_t *cfg);

static void s_hl_chip_micrel_ks8893_reset(const hl_chip_micrel_ks8893_cfg_t *cfg);

static hl_result_t s_hl_chip_micrel_ks8893_linkupmask(uint8_t* linkmask);

static void s_hl_chip_micrel_ks8893_phy_onestatus(hl_ethtrans_phystatus_t* phystatus, uint8_t phypos);
static hl_result_t s_hl_chip_micrel_ks8893_phy_status(hl_ethtrans_phystatus_t* phyarray, uint8_t arraysize);

static uint8_t s_hl_chip_micrel_ks8893_phy_errorcode_get(uint8_t phynum, hl_ethtrans_phyerror_t errortype);

static hl_result_t s_hl_chip_micrel_ks8893_phy_errorinfo(uint8_t phynum, hl_ethtrans_phyerror_t errortype, hl_ethtrans_phyerrorinfo_t *result);

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


extern hl_result_t hl_chip_micrel_ks8893_configure(hl_ethtrans_phymode_t* usedmiiphymode)
{
    hl_chip_micrel_ks8893_internal_item_t *intitem = s_hl_chip_micrel_ks8893_theinternals.items[0];
    
    
    //const uint8_t fd100 = 0x60;
    //const uint8_t fd010 = 0x20;
//    uint8_t buff_write = 0x60; // FORCE FULL DUPLEX AND 100T
    uint8_t buff_read = 0xFF; 
    volatile uint32_t i = 1;
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0};
    
    if(hl_false == s_hl_chip_micrel_ks8893_initted_is())
    {
        return(hl_res_NOK_generic);
    }  
    
    hl_ethtrans_phymode_t targetphymode = intitem->config.targetphymode;

    hl_i2c_t i2cid = intitem->config.i2cid;

    regaddr.bytes.one = REG0x01;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);
    if((buff_read&0x01))
    {   // already initted. to be initted again must pass through a reset
        if(NULL != usedmiiphymode)
        {
            s_hl_chip_micrel_ks8893_mii_phymode_get(usedmiiphymode);
        }
        return(hl_res_OK);
    }
    
    
    if(hl_ethtrans_phymode_none == targetphymode)
    {
        if(NULL != usedmiiphymode)
        {
            *usedmiiphymode = hl_ethtrans_phymode_none;    
        }   
        return(hl_res_OK);        
    }

    // we execute here only if the micrel is not initted yet.
    // however that does not happen

    // 1. config the phy modes
    s_hl_chip_micrel_ks8893_phymode_set(targetphymode);

    // 2. start the switch
    s_hl_chip_micrel_ks8893_start(&intitem->config);
    
    // 3. get the phy mode
    s_hl_chip_micrel_ks8893_mii_phymode_get(usedmiiphymode);

 
    return(hl_res_OK);
}


extern hl_result_t hl_chip_micrel_ks8893_mii_getphymode(hl_ethtrans_phymode_t* usedmiiphymode)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)    
    if(hl_false == s_hl_chip_micrel_ks8893_initted_is())
    {
        return(hl_res_NOK_generic);
    }  
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    
    s_hl_chip_micrel_ks8893_mii_phymode_get(usedmiiphymode);
    return(hl_res_OK);
}


extern hl_result_t hl_chip_micrel_ks8893_linkupmask(uint8_t* linkmask)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)    
    if(hl_false == s_hl_chip_micrel_ks8893_initted_is())
    {
        return(hl_res_NOK_generic);
    }  
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    
    return(s_hl_chip_micrel_ks8893_linkupmask(linkmask));     
}

extern hl_result_t hl_chip_micrel_ks8893_phy_status(hl_ethtrans_phystatus_t* phyarray, uint8_t arraysize)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)    
    if(hl_false == s_hl_chip_micrel_ks8893_initted_is())
    {
        return(hl_res_NOK_generic);
    }  
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    
    return(s_hl_chip_micrel_ks8893_phy_status(phyarray, arraysize));  
}

extern hl_result_t hl_chip_micrel_ks8893_phy_errorinfo(uint8_t phynum, hl_ethtrans_phyerror_t errortype, hl_ethtrans_phyerrorinfo_t *result)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)    
    if(hl_false == s_hl_chip_micrel_ks8893_initted_is())
    {
        return(hl_res_NOK_generic);
    }  
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    
    return(s_hl_chip_micrel_ks8893_phy_errorinfo(phynum, errortype, result));  
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
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0 };
    uint8_t data;
    
    data = 0;
    regaddr.numofbytes = 0;
    data = data;
    regaddr.numofbytes = regaddr.numofbytes;
    
    // 1. init external clock.
    
    if((NULL == cfg->extclockinit) || (hl_res_OK != cfg->extclockinit()))
    {
        return(hl_res_NOK_generic);
    }
    
    // 3. reset the chip
    s_hl_chip_micrel_ks8893_reset(cfg);
    
    // init anyway
    hl_i2c_init(i2cid, NULL);
    
    // 3. verify if i2c is initted  
    if(hl_false == hl_i2c_initted_is(i2cid))
    {
        return(hl_res_NOK_generic);
    }
    
    
    // 4. verify that the micrel is responding   
    if(hl_res_OK != hl_i2c_ping(i2cid, I2CADDRESS))
    {
        return(hl_res_NOK_generic);
    }
    
    
    memcpy(&intitem->config, cfg, sizeof(hl_chip_micrel_ks8893_cfg_t));
    
//#warning INFO --> could add verification that on ic2 the device at that address is really a micrel switch ...    
#if 0        
//    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
//    uint8_t data;

    // whoami: value must be WHOAMI_VAL
    regaddr.numofbytes = 1;
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

//     // read the reg01 register
//     regaddr.numofbytes = 1;
//     regaddr.bytes.one = REG0x01;
//     hl_i2c_read(i2cid, I2CADDRESS, regaddr, &data, 1); 
//     data = data;   

//     // be sure the switch is stopped
//     data = 0x0;  
//     regaddr.bytes.one = REG0x01;    
//     hl_i2c_write(i2cid, I2CADDRESS, regaddr, &data, 1);   

    
    // impose the phy mode     
    s_hl_chip_micrel_ks8893_phymode_set(cfg->targetphymode); //hl_ethtrans_phymode_auto   
    
//     // be sure the switch is stopped
//     data = 0x0;  
//     regaddr.bytes.one = REG0x01;    
//     hl_i2c_write(i2cid, I2CADDRESS, regaddr, &data, 1);   
    
    
    // start the micrel
    s_hl_chip_micrel_ks8893_start(cfg);    
    
    
    return(hl_res_OK);
}

static void s_hl_chip_micrel_ks8893_phymode_set(hl_ethtrans_phymode_t targetphymode)
{
    hl_chip_micrel_ks8893_internal_item_t *intitem = s_hl_chip_micrel_ks8893_theinternals.items[0];
    hl_i2c_t i2cid = intitem->config.i2cid;
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0 };
    uint8_t buff_write;
    uint8_t buff_read;
       

    // mii will be kept in auto
#if 0    
    // configure rmii (port 0).
    regaddr.bytes.one = REG0x06;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);
    buff_write  = buff_read;
    buff_write |= 0x10; 
    hl_i2c_write(i2cid, I2CADDRESS, regaddr, &buff_write, 1);
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);
#endif    
         
    // 1. configure  switch's ports 1 and 2
    switch(targetphymode)
    {
        case hl_ethtrans_phymode_auto:                  buff_write = 0x9F; break;
        case hl_ethtrans_phymode_halfduplex10mbps:      buff_write = 0x00; break;
        case hl_ethtrans_phymode_halfduplex100mbps:     buff_write = 0x40; break;
        case hl_ethtrans_phymode_fullduplex10mbps:      buff_write = 0x20; break;
        case hl_ethtrans_phymode_fullduplex100mbps:     buff_write = 0x60; break;
        default:                                        buff_write = 0x00; break;
    }    
    
    
    // port 1
    regaddr.numofbytes = 1;
    regaddr.bytes.one = REG0x1C;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);
    hl_i2c_write(i2cid, I2CADDRESS, regaddr, &buff_write, 1);
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);
     
    // port 2 
    regaddr.numofbytes = 1;
    regaddr.bytes.one = REG0x2C;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);
    hl_i2c_write(i2cid, I2CADDRESS, regaddr, &buff_write, 1);   
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);    
}


static void s_hl_chip_micrel_ks8893_mii_phymode_get(hl_ethtrans_phymode_t* usedmiiphymode)
{
    hl_chip_micrel_ks8893_internal_item_t *intitem = s_hl_chip_micrel_ks8893_theinternals.items[0];
    hl_i2c_t i2cid = intitem->config.i2cid;
    
    #define MIIHLFD    (1 << 6)
    #define MII10MBS    (1 << 4)
    uint8_t mux = 0;
    uint8_t speed = 0;
    uint8_t read = 0xFF; 
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0};
    regaddr.bytes.one = REG0x06;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &read, 1);
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
    if((0==mux)&&(0==speed))        *usedmiiphymode = hl_ethtrans_phymode_halfduplex10mbps;
    else if((0==mux)&&(1==speed))   *usedmiiphymode = hl_ethtrans_phymode_halfduplex100mbps; 
    else if((1==mux)&&(0==speed))   *usedmiiphymode = hl_ethtrans_phymode_fullduplex10mbps; 
    else if((1==mux)&&(1==speed))   *usedmiiphymode = hl_ethtrans_phymode_fullduplex100mbps;          
    
}


static hl_result_t s_hl_chip_micrel_ks8893_linkupmask(uint8_t* linkmask)
{
    // we do it by smi ...
    // 0 is port 0, 1 is port 1, 2 is rmii
    
    uint16_t status_link;
    uint8_t  PHYaddr = 0x1;
    uint8_t  REGaddr = 0x1;
    #define  LINK_IS_UP 0x0004
    
    if(NULL == linkmask)
    {
        return(hl_res_NOK_nullpointer);
    }
    
    // at first assign all links down
    *linkmask = 0;
    
    // now verify link port 0
    PHYaddr = 0x1;
    status_link = hl_eth_smi_read(PHYaddr, REGaddr);
    if((status_link & LINK_IS_UP) == LINK_IS_UP)
    {
       *linkmask |= 0x01;
    }
    
    // now verify link port 1
    PHYaddr = 0x2;
    status_link = hl_eth_smi_read(PHYaddr, REGaddr);
    if((status_link & LINK_IS_UP) == LINK_IS_UP)
    {
        *linkmask |= 0x02;
    }
    
    return(hl_res_OK);    
    
}

static void s_hl_chip_micrel_ks8893_phy_onestatus(hl_ethtrans_phystatus_t* phystatus, uint8_t phypos)
{   
    hl_chip_micrel_ks8893_internal_item_t *intitem = s_hl_chip_micrel_ks8893_theinternals.items[0];
    hl_i2c_t i2cid = intitem->config.i2cid;
    uint16_t regaddr_status0, regaddr_status1;
    uint8_t  buff_read = 0xFF;
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0};
    
    switch(phypos)
    {
        case 0:
        {
            regaddr_status0 = 0x1E;
            regaddr_status1 = 0x1F;
        } break;
        
        case 1:
        {
            regaddr_status0 = 0x2E;
            regaddr_status1 = 0x2F;
        } break;
        
        case 2:
        {
            regaddr_status0 = 0x0;  //  reserved, not applied to port 3. (see datasheet)
            regaddr_status1 = 0x3F;
        } break;
        
        default:
        {
            return;
        }
    }

    
    memset(phystatus, 0, sizeof(hl_ethtrans_phystatus_t));
    
    if(phypos < 2)
    {       
        regaddr.numofbytes = 1;
        regaddr.bytes.one = regaddr_status0;
        hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);           
        
        if(buff_read&0x0040)    // autoneg completed
        {
            phystatus->autoNeg_done = 1;
        }
        if(buff_read&0x0020)    // link is good
        {
            phystatus->linkisgood = 1;
        }
    }

    buff_read = 0;

    regaddr.numofbytes = 1;
    regaddr.bytes.one = regaddr_status1;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);       
    
    if(buff_read&0x0004)        // link speed 1==>100
    {
        phystatus->linkspeed = 1;
    }
    if(buff_read&0x0002)        // duplex 1==>full
    {
        phystatus->linkduplex = 1;
    }
    
}

static hl_result_t s_hl_chip_micrel_ks8893_phy_status(hl_ethtrans_phystatus_t* phyarray, uint8_t arraysize)
{
    uint8_t i; 
    
    if((NULL == phyarray) || (arraysize > 3))
    {
        return(hl_res_NOK_nullpointer);
    }

    for(i=0; (i<arraysize); i++)
    {
        s_hl_chip_micrel_ks8893_phy_onestatus(&phyarray[i], i);
    }

    return(hl_res_OK);
    
}


static uint8_t s_hl_chip_micrel_ks8893_phy_errorcode_get(uint8_t phynum, hl_ethtrans_phyerror_t errortype)
{
    switch(phynum)
    {
        case 0:
        {
            return(errortype);
        }
        
        case 1:
        {
            return(0x20+errortype);
        }
        
        case 2:
        {
            return(0x40+errortype);
        }
        default:
        {
            return(0);
        }
    };

}

static hl_result_t s_hl_chip_micrel_ks8893_phy_errorinfo(uint8_t phynum, hl_ethtrans_phyerror_t errortype, hl_ethtrans_phyerrorinfo_t *result)
{
    hl_chip_micrel_ks8893_internal_item_t *intitem = s_hl_chip_micrel_ks8893_theinternals.items[0];
    hl_i2c_t i2cid = intitem->config.i2cid;
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0 };

    uint8_t errorcode;
    uint8_t buff_write = 0x1c; // read MIB counters selected
    uint8_t buff_read[4] = {0};
    
    
    if(phynum > 2)
    {
        return(hl_res_NOK_nodata);
    }
    
    if(NULL == result)
    {
        return(hl_res_NOK_nullpointer);
    }
   
    errorcode = s_hl_chip_micrel_ks8893_phy_errorcode_get(phynum, errortype);
 
    buff_write = 0x1c;
    regaddr.bytes.one = 0x79;    
    hl_i2c_write(i2cid, I2CADDRESS, regaddr, &buff_write, 1);
    

    buff_write = errorcode;
    regaddr.bytes.one = 0x7A;    
    hl_i2c_write(i2cid, I2CADDRESS, regaddr, &buff_write, 1);    
   

    
    // read bits 24-31
    regaddr.bytes.one = 0x80;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read[3], 1);    
    
    // read bits 16-23
    regaddr.bytes.one = 0x81;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read[2], 1); 

    // read bits 8-15
    regaddr.bytes.one = 0x82;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read[1], 1);     
 
    // read bits 0-7
    regaddr.bytes.one = 0x83;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read[0], 1);     

   
    
    result->value = (((uint32_t)(buff_read[3]&0x3F))<< 24) | (((uint32_t)buff_read[2])<<16) | (((uint32_t)buff_read[1])<<8) | buff_read[0];

    
    if((buff_read[3]&0x80) == 0x80)
    {
        result->counteroverflow = 1;
    }
    else
    {
        result->counteroverflow = 0;
    }
    
    if((buff_read[3]&0x40) == 0x40)  
    {
        result->validvalue = 1;
    }
    else
    {
        result->validvalue = 0;
    }
    return(hl_res_OK);
}  


static void s_hl_chip_micrel_ks8893_start(const hl_chip_micrel_ks8893_cfg_t *cfg)
{
    //hl_chip_micrel_ks8893_internal_item_t *intitem = s_hl_chip_micrel_ks8893_theinternals.items[0];
    hl_i2c_t i2cid = cfg->i2cid;
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0 };
    uint8_t buff_write = 0;
    uint8_t buff_read = 0;
         
    // 2. start the switch
    buff_write = 0x1;  
    regaddr.bytes.one = REG0x01;    
    hl_i2c_write(i2cid, I2CADDRESS, regaddr, &buff_write, 1);
    

    // 3. read back to verify
    regaddr.bytes.one = REG0x01;
    hl_i2c_read(i2cid, I2CADDRESS, regaddr, &buff_read, 1);
    if(!(buff_read&0x01))
    {
        hl_sys_on_error(hl_error_runtimefault, "hl_chip_micrel_ks8893_configure(): SWITCH not configured");
    }
}


static void s_hl_chip_micrel_ks8893_reset(const hl_chip_micrel_ks8893_cfg_t *cfg)
{
    // 2. init reset pin
    hl_gpio_pin_output_init(cfg->resetpin);
 
    // 3. reset the micrel
    hl_gpio_val_t resetval = cfg->resetval;
    hl_gpio_pin_write(cfg->resetpin, resetval);
    hl_sys_delay(100*1000); // former was 10k
    hl_gpio_pin_write(cfg->resetpin, (hl_gpio_valRESET == resetval) ? (hl_gpio_valSET) : (hl_gpio_valRESET));
    hl_sys_delay(1000);  //former was 100  
}

#endif//defined(HL_USE_CHIP_MICREL_KS8893)



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



