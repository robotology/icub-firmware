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

/* @file       hl_chip_st_lis3x.c
	@brief      This file implements internals of the temperature sensor module.
	@author     marco.accame@iit.it
    @date       10/29/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"

#if     defined(HL_USE_CHIP_ST_LIS3X)


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"





 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_st_lis3x.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_st_lis3x_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define I2CADDRESS                  0x30  
  
#define REGADR_WHOAMI               0x0F
#define REGVAL_WHOAMI_LIS331DLH     0x32
#define REGVAL_WHOAMI_LIS3DH        0x33


#define REGADR_TEMP_CFG             0x1F

#define REGADR_CTRL_REG1            0x20
#define REGADR_CTRL_REG4            0x23
#define REGADR_CTRL_REG5            0x24

#define REGADR_OUT_ADC3_L           0x0C
#define REGADR_OUT_ADC3_H           0x0D

#define REGADR_OUT_X_L              0x28
#define REGADR_OUT_X_H              0x29
#define REGADR_OUT_Y_L              0x2A
#define REGADR_OUT_Y_H              0x2B
#define REGADR_OUT_Z_L              0x2C
#define REGADR_OUT_Z_H              0x2D

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hl_chip_st_lis3x_cfg_t hl_chip_st_lis3x_cfg_default  = 
{ 
    .chip       = hl_chip_st_lis3x_chip_autodetect,
    .i2cid      = hl_i2c1,
    .range      = hl_chip_st_lis3x_range_2g      
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hl_chip_st_lis3x_cfg_t          config;
    uint32_t                        factor;
    uint8_t                         shift;
    hl_chip_st_lis3x_chip_t         chip;
} hl_chip_st_lis3x_internal_item_t;

typedef struct
{
    hl_bool_t                               initted;
    hl_chip_st_lis3x_internal_item_t*       items[1];   
} hl_chip_st_lis3x_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_chip_st_lis3x_initted_set(void);
static hl_bool_t s_hl_chip_st_lis3x_initted_is(void);

static hl_result_t s_hl_chip_st_lis3x_hw_init(const hl_chip_st_lis3x_cfg_t *cfg, hl_chip_st_lis3x_internal_item_t* intitem);

static int32_t s_hl_chip_st_lis3x_convert(int32_t v);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_chip_st_lis3x_internal_item_t hl_chip_st_lis3x_theonlyitem = { 
    .config = { .i2cid = hl_i2c1, .range = hl_chip_st_lis3x_range_2g }, 
    .factor = 0, 
    .shift  = 0,
    .chip   = hl_chip_st_lis3x_chip_lis331dlh
};  // so far it is the only one.

static hl_chip_st_lis3x_theinternals_t s_hl_chip_st_lis3x_theinternals =
{
    .initted            = hl_false,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hl_result_t hl_chip_st_lis3x_init(const hl_chip_st_lis3x_cfg_t *cfg)
{
    hl_chip_st_lis3x_internal_item_t *intitem = s_hl_chip_st_lis3x_theinternals.items[0];
     
    if(NULL == cfg)
    {
        cfg  = &hl_chip_st_lis3x_cfg_default;
    }
    
    if(hl_true == s_hl_chip_st_lis3x_initted_is())
    {
        return(hl_res_OK);
    }  

    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        // those of you scared of heap may use static ram
        //intitem = s_hl_chip_st_lis3x_theinternals.items[0] = hl_sys_heap_new(sizeof(hl_chip_st_lis3x_internal_item_t));
        intitem = s_hl_chip_st_lis3x_theinternals.items[0] = &hl_chip_st_lis3x_theonlyitem;
    }  
    
    if(hl_res_OK != s_hl_chip_st_lis3x_hw_init(cfg, intitem))
    {
        return(hl_res_NOK_generic);
    }
    
    s_hl_chip_st_lis3x_initted_set();

    return(hl_res_OK);
}


extern hl_result_t hl_chip_st_lis3x_temp_get(int16_t* temp)
{
    hl_result_t res = hl_res_NOK_generic; 
    hl_chip_st_lis3x_internal_item_t *intitem = s_hl_chip_st_lis3x_theinternals.items[0];

    uint8_t datal = 0;
    uint8_t datah = 0;

    *temp = 0;

#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    
    if(hl_false == s_hl_chip_st_lis3x_initted_is())
    {
        return(hl_res_NOK_generic);
    }
    
    if(hl_chip_st_lis3x_chip_lis3dh != intitem->chip)
    {
        return(hl_res_NOK_generic);
    }
    
#endif   
    
    hl_i2c_t i2cid = intitem->config.i2cid;

    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = REGADR_OUT_ADC3_L;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = REGADR_OUT_ADC3_H;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datah, 1);
    *temp = (int16_t)((datah << 8) | datal);    
   
    return(res);
}

extern hl_result_t hl_chip_st_lis3x_accel_get(int32_t* xac, int32_t* yac, int32_t* zac)
{
    hl_result_t res = hl_res_NOK_generic; 
    hl_chip_st_lis3x_internal_item_t *intitem = s_hl_chip_st_lis3x_theinternals.items[0];


    uint8_t datal = 0;
    uint8_t datah = 0;

    
    *xac = 0;
    *yac = 0;
    *zac = 0;
    
#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
 
    if(hl_false == s_hl_chip_st_lis3x_initted_is())
    {
        return(hl_res_NOK_generic);
    }
    
#endif   
    
    hl_i2c_t i2cid = intitem->config.i2cid;


    hl_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = REGADR_OUT_X_L;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = REGADR_OUT_X_H;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datah, 1);
    *xac = s_hl_chip_st_lis3x_convert((int16_t)((datah << 8) | datal));    
    
    regaddr.bytes.one = REGADR_OUT_Y_L;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = REGADR_OUT_Y_H;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datah, 1);
    *yac = s_hl_chip_st_lis3x_convert((int16_t)((datah << 8) | datal));    

    regaddr.bytes.one = REGADR_OUT_Z_L;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = REGADR_OUT_Z_H;
    res = hl_i2c_read(i2cid, I2CADDRESS, regaddr, &datah, 1);
    *zac = s_hl_chip_st_lis3x_convert((int16_t)((datah << 8) | datal));   
   
    return(res);
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


static void s_hl_chip_st_lis3x_initted_set(void)
{
    s_hl_chip_st_lis3x_theinternals.initted = hl_true;
}

static hl_bool_t s_hl_chip_st_lis3x_initted_is(void)
{
    return(s_hl_chip_st_lis3x_theinternals.initted);
}

    // the range is +-2G. it means that 32k is mapped into 2000 mG -> 32k/2000 = 16.384
    // if i have 1 mG i must read  16.384
    // to have the measure in milli-G i must multiply the read for F = 1 / 16.384 = 0.06103515625
    // F = 125 / (2*1024) = 0.06103515625 .... i multiply 125 and then i shift 11 times
    
    // if range is +-4G. 32k / 4000 = 8.192 ..... F = 0.1220703125 ....
    // F = 125 / 1024 
    
    // if range is +-8g 
    // F = 125 / 512
    
    // if range is +-16g
    // F = 125 / 256
    
static hl_result_t s_hl_chip_st_lis3x_hw_init(const hl_chip_st_lis3x_cfg_t *cfg, hl_chip_st_lis3x_internal_item_t* intitem)
{
    hl_result_t res = hl_res_NOK_generic;   
    uint8_t data = 0;
    volatile uint8_t dd = 0;
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
    dd = data;
    

    switch(cfg->chip)
    {
        case hl_chip_st_lis3x_chip_lis3dh:
        {
            if(REGVAL_WHOAMI_LIS3DH != dd)
            {
                res = hl_res_NOK_generic;
            }
        } break;

        case hl_chip_st_lis3x_chip_lis331dlh:
        {
            if(REGVAL_WHOAMI_LIS331DLH != dd)
            {
                res = hl_res_NOK_generic;
            }
        } break; 

        case hl_chip_st_lis3x_chip_autodetect:
        {
            if(!((REGVAL_WHOAMI_LIS3DH == dd) || (REGVAL_WHOAMI_LIS331DLH == dd)))
            {
                res = hl_res_NOK_generic;
            }
        } break;

        default:
        {
            res = hl_res_NOK_generic;
        } break;
    }
    
    
    if(hl_res_OK != res)
    {
        return(hl_res_NOK_generic);
    }
    
    
    if(REGVAL_WHOAMI_LIS3DH == dd)
    {
        intitem->chip = hl_chip_st_lis3x_chip_lis3dh;        
    }
    else
    {
        intitem->chip = hl_chip_st_lis3x_chip_lis331dlh;
    }
    
    // config now ...
    
    regaddr.bytes.one = REGADR_CTRL_REG1;       
    data = 0x77; // enable x, y, z + power down normal mode + data rate @ 400 hz
    if(hl_res_OK != (res = hl_i2c_write(i2cid, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    //hl_i2c_is_device_ready(i2cid, I2CADDRESS); it should not be necessary.
 
 
    regaddr.bytes.one = REGADR_CTRL_REG4;       
    data = 0x00;    // continuous update of data + lbs @lower address + full scale is +-2G + hi-res mode disabled +self test disable + spi disabled
    if(hl_chip_st_lis3x_range_2g == cfg->range)
    {
        data |= 0x00;
        intitem->factor = 125;
        intitem->shift  = 11; 
    }
    else if(hl_chip_st_lis3x_range_4g == cfg->range)
    {
        data |= 0x10;
        intitem->factor = 125;
        intitem->shift  = 10;         
    }
    else if(hl_chip_st_lis3x_range_8g == cfg->range)
    {
        if(intitem->chip == hl_chip_st_lis3x_chip_lis3dh)
        {
            data |= 0x20;
        }
        else
        {
            data |= 0x30;
        }
        intitem->factor = 125;
        intitem->shift  = 9;         
    }    
//     else if(hl_chip_st_lis3x_range_16g == cfg->range)
//     {
//         if(intitem->chip == hl_chip_st_lis3x_chip_lis3dh)
//         {
//             data |= 0x30;
//             intitem->factor = 125;
//             intitem->shift  = 8; 
//         }
//         else
//         {   // use 8g
//             data |= 0x30;
//             intitem->factor = 125;
//             intitem->shift  = 9; 
//         }            
//     }  
    
    if(hl_res_OK != (res = hl_i2c_write(i2cid, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    } 
    
    hl_i2c_is_device_ready(i2cid, I2CADDRESS);
 


    if(intitem->chip == hl_chip_st_lis3x_chip_lis3dh)
    {
        regaddr.bytes.one = REGADR_CTRL_REG5;       
        data = 0x40;        // enable fifo
        if(hl_res_OK != (res = hl_i2c_write(i2cid, I2CADDRESS, regaddr, &data, 1)))
        {
            return(res);
        }
        hl_i2c_is_device_ready(i2cid, I2CADDRESS);    
            
            
        regaddr.bytes.one = REGADR_TEMP_CFG;       
        data = 0xC0;        // enable adc + enable temp sensor
        if(hl_res_OK != (res = hl_i2c_write(i2cid, I2CADDRESS, regaddr, &data, 1)))
        {
            return(res);
        }
        hl_i2c_is_device_ready(i2cid, I2CADDRESS);  
    }
    
    memcpy(&intitem->config, cfg, sizeof(hl_chip_st_lis3x_cfg_t));
    
    // store the i2caddress and the register address.
    return(hl_res_OK);
}


static int32_t s_hl_chip_st_lis3x_convert(int32_t v)
{
    hl_chip_st_lis3x_internal_item_t *intitem = s_hl_chip_st_lis3x_theinternals.items[0];

    int32_t factor =   (int32_t)intitem->factor; // 
    uint8_t  shift =            intitem->shift; //  

    uint8_t neg = (v < 0) ? (1) : (0);
    int32_t r = (0 == neg) ? (factor*v) : (factor*(-v));
    // now r is positive
    r >>= shift;
    r = (0 == neg) ? (r) : (-r);
    
    return(r);  
}

#endif//defined(HL_USE_CHIP_ST_LIS3X)


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



