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

/* @file       hal_chip_st_lis3dh.c
	@brief      This file implements internals of the temperature sensor module.
	@author     marco.accame@iit.it
    @date       10/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef  HAL_USE_CHIP_ST_LIS3DH

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

#include "hal_chip_st_lis3dh.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_chip_st_lis3dh_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define I2CADDRESS          0x30  
  
#define WHOAMI_ADR          0x0F
#define WHOAMI_VAL          0x33

#define TEMP_CFG_REG        0x1F

#define CTRL_REG1           0x20
#define CTRL_REG4           0x23
#define CTRL_REG5           0x24

#define OUT_ADC3_L          0x0C
#define OUT_ADC3_H          0x0D

#define OUT_X_L             0x28
#define OUT_X_H             0x29
#define OUT_Y_L             0x2A
#define OUT_Y_H             0x2B
#define OUT_Z_L             0x2C
#define OUT_Z_H             0x2D

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_chip_st_lis3dh_cfg_t hal_chip_st_lis3dh_cfg_default  = 
{ 
    .i2cport    = hal_i2c_port1,
    .range      = hal_chip_st_lis3dh_range_2g      
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_chip_st_lis3dh_cfg_t    cfg;
    uint32_t                        factor;
    uint8_t                         shift;
} hal_chip_st_lis3dh_info_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_chip_st_lis3dh_initted_set(void);
static hal_boolval_t s_hal_chip_st_lis3dh_initted_is(void);

static hal_result_t s_hal_chip_st_lis3dh_hw_init(const hal_chip_st_lis3dh_cfg_t *cfg, hal_chip_st_lis3dh_info_t* info);

static int32_t s_hal_chip_st_lis3dh_convert(int32_t v);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_chip_st_lis3dh_initted[1] = { hal_false };
static hal_chip_st_lis3dh_info_t s_hal_chip_st_lis3dh_info[1] = { {.cfg = { .i2cport = hal_i2c_port1} } };



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_chip_st_lis3dh_init(const hal_chip_st_lis3dh_cfg_t *cfg)
{
//    hal_result_t res = hal_res_NOK_generic; // dont remove ...
     
    if(NULL == cfg)
    {
        cfg  = &hal_chip_st_lis3dh_cfg_default;
    }
    
    if(hal_true == s_hal_chip_st_lis3dh_initted_is())
    {
        return(hal_res_OK);
    }    

    if(hal_res_OK != s_hal_chip_st_lis3dh_hw_init(cfg, &s_hal_chip_st_lis3dh_info[0]))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_chip_st_lis3dh_initted_set();

    return(hal_res_OK);
}


extern hal_result_t hal_chip_st_lis3dh_temp_get(int16_t* temp)
{
    hal_result_t res = hal_res_NOK_generic; 
    hal_i2c_port_t i2cport = s_hal_chip_st_lis3dh_info[0].cfg.i2cport;

    uint8_t datal = 0;
    uint8_t datah = 0;

    *temp = 0;
    
    if(hal_false == s_hal_chip_st_lis3dh_initted_is())
    {
        return(hal_res_NOK_generic);
    }
    


    hal_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = OUT_ADC3_L;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = OUT_ADC3_H;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datah, 1);
    *temp = (int16_t)((datah << 8) | datal);    
   
    return(res);
}

extern hal_result_t hal_chip_st_lis3dh_accel_get(int32_t* xac, int32_t* yac, int32_t* zac)
{
    hal_result_t res = hal_res_NOK_generic; 
    hal_i2c_port_t i2cport = s_hal_chip_st_lis3dh_info[0].cfg.i2cport;

    uint8_t datal = 0;
    uint8_t datah = 0;

    
    *xac = 0;
    *yac = 0;
    *zac = 0;
    
    if(hal_false == s_hal_chip_st_lis3dh_initted_is())
    {
        return(hal_res_NOK_generic);
    }


    hal_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = OUT_X_L;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = OUT_X_H;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datah, 1);
    *xac = s_hal_chip_st_lis3dh_convert((int16_t)((datah << 8) | datal));    
    
    regaddr.bytes.one = OUT_Y_L;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = OUT_Y_H;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datah, 1);
    *yac = s_hal_chip_st_lis3dh_convert((int16_t)((datah << 8) | datal));    

    regaddr.bytes.one = OUT_Z_L;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = OUT_Z_H;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datah, 1);
    *zac = s_hal_chip_st_lis3dh_convert((int16_t)((datah << 8) | datal));   
   
    return(res);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern hal_result_t hal_chip_st_lis3dh_hid_static_memory_init(void)
{
    memset(s_hal_chip_st_lis3dh_initted, hal_false, sizeof(s_hal_chip_st_lis3dh_initted));
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hal_chip_st_lis3dh_initted_set(void)
{
    s_hal_chip_st_lis3dh_initted[0] = hal_true;
}

static hal_boolval_t s_hal_chip_st_lis3dh_initted_is(void)
{
    return(s_hal_chip_st_lis3dh_initted[0]);
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
    
static hal_result_t s_hal_chip_st_lis3dh_hw_init(const hal_chip_st_lis3dh_cfg_t *cfg, hal_chip_st_lis3dh_info_t* info)
{
    hal_result_t res = hal_res_NOK_generic;   
    uint8_t data;
    hal_i2c_port_t i2cport = cfg->i2cport;
    
 
    // init i2c    
    if(hal_res_OK != (res = hal_i2c_init(i2cport, NULL)))
    {
        return(res);
    }
    
    if(hal_res_OK != hal_i2c_ping(i2cport, I2CADDRESS))
    {
        return(hal_res_NOK_generic);
    }
    
    
    hal_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    // whoami: value must be WHOAMI_VAL
    regaddr.bytes.one = WHOAMI_ADR;
    if(hal_res_OK != (res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    if((hal_res_OK != res) || (WHOAMI_VAL != data))
    {
        return(hal_res_NOK_generic);
    }
    
    // config now ...
    
    regaddr.bytes.one = CTRL_REG1;       
    data = 0x77; // enable x, y, z + power down normal mode + data rate @ 400 hz
    if(hal_res_OK != (res = hal_i2c_write(i2cport, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    hal_i2c_standby(i2cport, I2CADDRESS);
 
 
    regaddr.bytes.one = CTRL_REG4;       
    data = 0x00;    // continuos update of data + lbs @lower address + full scale is +-2G + hi-res mode disabled +self test disable + spi disabled
    if(hal_chip_st_lis3dh_range_2g == cfg->range)
    {
        data |= 0x00;
        info->factor = 125;
        info->shift  = 11; 
    }
    else if(hal_chip_st_lis3dh_range_4g == cfg->range)
    {
        data |= 0x10;
        info->factor = 125;
        info->shift  = 10;         
    }
    else if(hal_chip_st_lis3dh_range_8g == cfg->range)
    {
        data |= 0x20;
        info->factor = 125;
        info->shift  = 9;         
    }    
    else if(hal_chip_st_lis3dh_range_16g == cfg->range)
    {
        data |= 0x30;
        info->factor = 125;
        info->shift  = 8;         
    }        
    if(hal_res_OK != (res = hal_i2c_write(i2cport, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }   
    hal_i2c_standby(i2cport, I2CADDRESS);
 
 
    regaddr.bytes.one = CTRL_REG5;       
    data = 0x40;        // enable fifo
    if(hal_res_OK != (res = hal_i2c_write(i2cport, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    hal_i2c_standby(i2cport, I2CADDRESS);    
        
        
    regaddr.bytes.one = TEMP_CFG_REG;       
    data = 0xC0;        // enable adc + enable tepn sensor
    if(hal_res_OK != (res = hal_i2c_write(i2cport, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    hal_i2c_standby(i2cport, I2CADDRESS);    
    
    memcpy(&info->cfg, cfg, sizeof(hal_chip_st_lis3dh_cfg_t));
    
    // store the i2caddress and the register address.
    return(hal_res_OK);
}


static int32_t s_hal_chip_st_lis3dh_convert(int32_t v)
{

    int32_t factor =   (int32_t)s_hal_chip_st_lis3dh_info[0].factor; // 
    uint8_t  shift =            s_hal_chip_st_lis3dh_info[0].shift; //  

    uint8_t neg = (v < 0) ? (1) : (0);
    int32_t r = (0 == neg) ? (factor*v) : (factor*(-v));
    // now r is positive
    r >>= shift;
    r = (0 == neg) ? (r) : (-r);
    
    return(r);  
}

#endif//HAL_USE_CHIP_ST_LIS3DH


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



