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

/* @file       hl_chip_st_l3g4200d.c
	@brief      This file implements internals of a ST L3G4200D gyroscope
	@author     marco.accame@iit.it
    @date       10/29/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"

#if     defined(HL_USE_CHIP_ST_L3G4200D)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "stdio.h"


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_st_l3g4200d.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_st_l3g4200d_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define I2CADDRESS              0xD0

#define REGADR_WHOAMI           0x0F
#define REGVAL_WHOAMI           0xD3

#define REGADR_TEMP             0x26

#define REGADR_ARXL             0x28
#define REGADR_ARXH             0x29
#define REGADR_ARYL             0x2a
#define REGADR_ARYH             0x2b
#define REGADR_ARZL             0x2c
#define REGADR_ARZH             0x2d

#define REGADR_CTR1             0x20
#define REGADR_CTR2             0x21
#define REGADR_CTR3             0x22
#define REGADR_CTR4             0x23
#define REGADR_CTR5             0x24


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hl_chip_st_l3g4200d_cfg_t hl_chip_st_l3g4200d_cfg_default  = 
{ 
    .i2cid      = hl_i2c1,
    .range      = hl_chip_st_l3g4200d_range_250dps    
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hl_chip_st_l3g4200d_cfg_t       config;
    uint32_t                        factor;
    uint8_t                         shift;
} hl_chip_st_l3g4200d_internal_item_t;

typedef struct
{
    hl_bool_t                               initted;
    hl_chip_st_l3g4200d_internal_item_t*    items[1];   
} hl_chip_st_l3g4200d_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_chip_st_l3g4200d_initted_set(void);
static hl_boolval_t s_hl_chip_st_l3g4200d_initted_is(void);

static hl_result_t s_hl_chip_st_l3g4200d_hw_init(const hl_chip_st_l3g4200d_cfg_t *cfg, hl_chip_st_l3g4200d_internal_item_t* intitem);

static int32_t s_hl_chip_st_l3g4200d_convert(int32_t v);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_chip_st_l3g4200d_internal_item_t hl_chip_st_l3g4200d_theonlyitem = { 
    .config = { .i2cid = hl_i2c1, .range = hl_chip_st_l3g4200d_range_250dps }, 
    .factor = 0, 
    .shift = 0 
};  // so far it is the only one.

static hl_chip_st_l3g4200d_theinternals_t s_hl_chip_st_l3g4200d_theinternals =
{
    .initted            = hl_false,
    .items              = { NULL }   
};





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hl_result_t hl_chip_st_l3g4200d_init(const hl_chip_st_l3g4200d_cfg_t *cfg)
{
    hl_chip_st_l3g4200d_internal_item_t *intitem = s_hl_chip_st_l3g4200d_theinternals.items[0];
     
    if(NULL == cfg)
    {
        cfg  = &hl_chip_st_l3g4200d_cfg_default;
    }
    
    if(hl_true == s_hl_chip_st_l3g4200d_initted_is())
    {
        return(hl_res_OK);
    }  

    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        // those of you scared of heap may use static ram
        //intitem = s_hl_chip_st_l3g4200d_theinternals.items[0] = hl_sys_heap_new(sizeof(hl_chip_st_l3g4200d_internal_item_t));  
        intitem = s_hl_chip_st_l3g4200d_theinternals.items[0] = &hl_chip_st_l3g4200d_theonlyitem;
    }  
    

    if(hl_res_OK != s_hl_chip_st_l3g4200d_hw_init(cfg, intitem))
    {
        return(hl_res_NOK_generic);
    }
    
    
    s_hl_chip_st_l3g4200d_initted_set();

    return(hl_res_OK);
}


extern hl_result_t hl_chip_st_l3g4200d_temp_get(int8_t* temp)
{
    hl_result_t res = hl_res_NOK_generic; 
    hl_chip_st_l3g4200d_internal_item_t *intitem = s_hl_chip_st_l3g4200d_theinternals.items[0];
    hl_i2c_t i2cid = intitem->config.i2cid;

    uint8_t data = 0;

    *temp = 0;
#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    if(hl_false == s_hl_chip_st_l3g4200d_initted_is())
    {
        return(hl_res_NOK_generic);
    }
#endif         
    
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = REGADR_TEMP;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &data, 1);
    *temp = (int8_t)data;   

    res = res;    
   
    return(hl_res_OK);
}

extern hl_result_t hl_chip_st_l3g4200d_angrate_get(int32_t* xar, int32_t* yar, int32_t* zar)
{
    hl_result_t res = hl_res_NOK_generic; 
    hl_chip_st_l3g4200d_internal_item_t *intitem = s_hl_chip_st_l3g4200d_theinternals.items[0];
    hl_i2c_t i2cid = intitem->config.i2cid;

    uint8_t datal = 0;
    uint8_t datah = 0;
    int32_t tmp;

    
    *xar = 0;
    *yar = 0;
    *zar = 0;

#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    if(hl_false == s_hl_chip_st_l3g4200d_initted_is())
    {
        return(hl_res_NOK_generic);
    }
#endif  

    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = REGADR_ARXL;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = REGADR_ARXH;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datah, 1);
    tmp = (int16_t)((datah << 8) | datal);    
    *xar = s_hl_chip_st_l3g4200d_convert(tmp);
    
    regaddr.bytes.one = REGADR_ARYL;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = REGADR_ARYH;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datah, 1);
    tmp = (int16_t)((datah << 8) | datal);    
    *yar = s_hl_chip_st_l3g4200d_convert(tmp);    

    regaddr.bytes.one = REGADR_ARZL;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = REGADR_ARZH;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datah, 1);
    tmp = (int16_t)((datah << 8) | datal);    
    *zar = s_hl_chip_st_l3g4200d_convert(tmp);
    
    res = res;
   
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


static void s_hl_chip_st_l3g4200d_initted_set(void)
{
    s_hl_chip_st_l3g4200d_theinternals.initted = hl_true;
}

static hl_bool_t s_hl_chip_st_l3g4200d_initted_is(void)
{
    return(s_hl_chip_st_l3g4200d_theinternals.initted);
}


// caso 2000 dps mappato su 32k -> 32*1024/2000 = 16.384. quindi ... ogni valore letto uguale a 16 io ho 1 dps.
// se moltiplico 16.384 per 2000 ottengo 32k. 
// 1 bit vale .... 1dps/16.384 = 0.06103515625 
// infatti ... se moltiplico per 32k ottengo 2000.
// se voglio milli-dps ... 1 bit vale 1000*0.06103515625 = 61.03515625
// la prova e' che 32*1024*61.03515625 = 200mila.
// scopongo 61.03515625 come 62500 / 1024 -> factor 62500 shift 10.

// caso 500.
// il fattore e' 61.03515625 / 4 = 15.2587890625
// scompongo come 15625 / 1024 

// caso 250 
// il fattore e' 15.2587890625 /2 = 7.62939453125
// scompongo come 15625 / 2048



static hl_result_t s_hl_chip_st_l3g4200d_hw_init(const hl_chip_st_l3g4200d_cfg_t *cfg, hl_chip_st_l3g4200d_internal_item_t *intitem)
{
    hl_result_t res = hl_res_NOK_generic;   
    uint8_t data;
    hl_i2c_t i2cid = cfg->i2cid;
    
 
    // is i2c bus already initialised?
    if(hl_false == hl_i2c_initted_is(i2cid))
    {
        return(hl_res_NOK_generic);
    }   
    
    if(hl_res_OK != hl_i2c_ping(i2cid, I2CADDRESS))
    {
        return(hl_res_NOK_generic);
    }    
   
    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    // whoami: value must be REGVAL_WHOAMI
    regaddr.bytes.one = REGADR_WHOAMI;
    if(hl_res_OK != (res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    if((hl_res_OK != res) || (REGVAL_WHOAMI != data))
    {
        return(hl_res_NOK_generic);
    }
    
    // config now ...
    
    regaddr.bytes.one = REGADR_CTR1;       
    data = 0x0F; // enable x, y, z and power down normal mode (so that i have temperature readings). bandwidth is 100 hz w/ 12.5 cutoff
    if(hl_res_OK != (res = hl_i2c_write(i2cid, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    //hl_i2c_is_device_ready(i2cid, I2CADDRESS); // it should not be necessary

    regaddr.bytes.one = REGADR_CTR4;       
    data = 0x00;        // continuous update of data + lbs @lower address + full scale is 250 dps + self test disable + spi disabled
    if(hl_chip_st_l3g4200d_range_250dps == cfg->range)
    {
        data |= 0x00;
        intitem->factor = 15625;
        intitem->shift  = 11; 
    }
    else if(hl_chip_st_l3g4200d_range_500dps == cfg->range)
    {
        data |= 0x10;
        intitem->factor = 15625;
        intitem->shift  = 10;         
    }
    else if(hl_chip_st_l3g4200d_range_2000dps == cfg->range)
    {
        data |= 0x20;
        intitem->factor = 62500;
        intitem->shift  = 10;         
    }
    if(hl_res_OK != (res = hl_i2c_write(i2cid, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }  
    //hl_i2c_is_device_ready(i2cid, I2CADDRESS); // it should not be necessary
 
    regaddr.bytes.one = REGADR_CTR5;       
    data = 0x40;        // enable fifo
    if(hl_res_OK != (res = hl_i2c_write(i2cid, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    //hl_i2c_is_device_ready(i2cid, I2CADDRESS); // it should not be necessary   
    
    
    memcpy(&intitem->config, cfg, sizeof(hl_chip_st_l3g4200d_cfg_t));
        

    return(hl_res_OK);
}


static int32_t s_hl_chip_st_l3g4200d_convert(int32_t v)
{
    hl_chip_st_l3g4200d_internal_item_t *intitem = s_hl_chip_st_l3g4200d_theinternals.items[0];

    int32_t factor =   (int32_t)intitem->factor; // 
    uint8_t  shift =            intitem->shift; //  

    uint8_t neg = (v < 0) ? (1) : (0);
    int32_t r = (0 == neg) ? (factor*v) : (factor*(-v));
    // now r is positive
    r >>= shift;
    r = (0 == neg) ? (r) : (-r);
    
    return(r);  
}


#endif//defined(HL_USE_CHIP_ST_L3G4200D)


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



