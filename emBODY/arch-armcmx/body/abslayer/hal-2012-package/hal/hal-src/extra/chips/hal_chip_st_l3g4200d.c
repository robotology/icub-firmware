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

/* @file       hal_chip_st_l3g4200d.c
	@brief      This file implements internals of the temperature sensor module.
	@author     marco.accame@iit.it
    @date       10/25/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef  HAL_USE_CHIP_ST_L3G4200D

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

#include "hal_chip_st_l3g4200d.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_chip_st_l3g4200d_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define I2CADDRESS          0xD0

#define WHOAMI_ADR          0x0F
#define WHOAMI_VAL          0xD3

#define TEMP_ADR            0x26

#define ARXL_ADR            0x28
#define ARXH_ADR            0x29
#define ARYL_ADR            0x2a
#define ARYH_ADR            0x2b
#define ARZL_ADR            0x2c
#define ARZH_ADR            0x2d

#define CTR1_ADR            0x20
#define CTR2_ADR            0x21
#define CTR3_ADR            0x22
#define CTR4_ADR            0x23
#define CTR5_ADR            0x24


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_chip_st_l3g4200d_cfg_t hal_chip_st_l3g4200d_cfg_default  = 
{ 
    .i2cport    = hal_i2c_port1,
    .range      = hal_chip_st_l3g4200d_range_250dps    
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_chip_st_l3g4200d_cfg_t      cfg;
    uint32_t                        factor;
    uint8_t                         shift;
} hal_chip_st_l3g4200d_info_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_chip_st_l3g4200d_initted_set(void);
static hal_boolval_t s_hal_chip_st_l3g4200d_initted_is(void);

static hal_result_t s_hal_chip_st_l3g4200d_hw_init(const hal_chip_st_l3g4200d_cfg_t *cfg, hal_chip_st_l3g4200d_info_t* info);

static int32_t s_hal_chip_st_l3g4200d_convert(int32_t v);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_chip_st_l3g4200d_initted[1] = { hal_false };
static hal_chip_st_l3g4200d_info_t s_hal_chip_st_l3g4200d_info[1] = { {.cfg = { .i2cport = hal_i2c_port1} } };



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_chip_st_l3g4200d_init(const hal_chip_st_l3g4200d_cfg_t *cfg)
{
    //hal_result_t res = hal_res_NOK_generic; 
     
    if(NULL == cfg)
    {
        cfg  = &hal_chip_st_l3g4200d_cfg_default;
    }
    
    if(hal_true == s_hal_chip_st_l3g4200d_initted_is())
    {
        return(hal_res_OK);
    }    

    if(hal_res_OK != s_hal_chip_st_l3g4200d_hw_init(cfg, &s_hal_chip_st_l3g4200d_info[0]))
    {
        return(hal_res_NOK_generic);
    }
    
    
    s_hal_chip_st_l3g4200d_initted_set();

    return(hal_res_OK);
}


extern hal_result_t hal_chip_st_l3g4200d_temp_get(int8_t* temp)
{
    hal_result_t res = hal_res_NOK_generic; 
    hal_i2c_port_t i2cport = s_hal_chip_st_l3g4200d_info[0].cfg.i2cport;

    uint8_t data = 0;

    *temp = 0;
    
    if(hal_false == s_hal_chip_st_l3g4200d_initted_is())
    {
        return(hal_res_NOK_generic);
    }
    


    hal_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = TEMP_ADR;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &data, 1);
    *temp = (int8_t)data;   

    res = res;    
   
    return(hal_res_OK);
}

extern hal_result_t hal_chip_st_l3g4200d_angrate_get(int32_t* xar, int32_t* yar, int32_t* zar)
{
    hal_result_t res = hal_res_NOK_generic; 
    hal_i2c_port_t i2cport = s_hal_chip_st_l3g4200d_info[0].cfg.i2cport;

    uint8_t datal = 0;
    uint8_t datah = 0;
    int32_t tmp;

    
    *xar = 0;
    *yar = 0;
    *zar = 0;
    
    if(hal_false == s_hal_chip_st_l3g4200d_initted_is())
    {
        return(hal_res_NOK_generic);
    }


    hal_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = ARXL_ADR;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = ARXH_ADR;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datah, 1);
    tmp = (int16_t)((datah << 8) | datal);    
    *xar = s_hal_chip_st_l3g4200d_convert(tmp);
    
    regaddr.bytes.one = ARYL_ADR;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = ARYH_ADR;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datah, 1);
    tmp = (int16_t)((datah << 8) | datal);    
    *yar = s_hal_chip_st_l3g4200d_convert(tmp);    

    regaddr.bytes.one = ARZL_ADR;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datal, 1);
    regaddr.bytes.one = ARZH_ADR;
    res = hal_i2c_read(i2cport, I2CADDRESS, regaddr, &datah, 1);
    tmp = (int16_t)((datah << 8) | datal);    
    *zar = s_hal_chip_st_l3g4200d_convert(tmp);
    
    res = res;
   
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_chip_st_l3g4200d_hid_getsize(const hal_base_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_chip_st_l3g4200d_hid_setmem(const hal_base_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }


    memset(s_hal_chip_st_l3g4200d_initted, hal_false, sizeof(s_hal_chip_st_l3g4200d_initted));
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hal_chip_st_l3g4200d_initted_set(void)
{
    s_hal_chip_st_l3g4200d_initted[0] = hal_true;
}

static hal_boolval_t s_hal_chip_st_l3g4200d_initted_is(void)
{
    return(s_hal_chip_st_l3g4200d_initted[0]);
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



static hal_result_t s_hal_chip_st_l3g4200d_hw_init(const hal_chip_st_l3g4200d_cfg_t *cfg, hal_chip_st_l3g4200d_info_t* info)
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
    
    regaddr.bytes.one = CTR1_ADR;       
    data = 0x0F; // enable x, y, z and power down normal mode (so that i have temperature readings). bandwidth is 100 hx w/ 12.5 cutoff
    if(hal_res_OK != (res = hal_i2c_write(i2cport, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    hal_i2c_standby(i2cport, I2CADDRESS);

    regaddr.bytes.one = CTR4_ADR;       
    data = 0x00;        // continuos update of data + lbs @lower address + full scalse is 250 dps + self test disable + spi disabled
    if(hal_chip_st_l3g4200d_range_250dps == cfg->range)
    {
        data |= 0x00;
        info->factor = 15625;
        info->shift  = 11; 
    }
    else if(hal_chip_st_l3g4200d_range_500dps == cfg->range)
    {
        data |= 0x10;
        info->factor = 15625;
        info->shift  = 10;         
    }
    else if(hal_chip_st_l3g4200d_range_2000dps == cfg->range)
    {
        data |= 0x20;
        info->factor = 62500;
        info->shift  = 10;         
    }
    if(hal_res_OK != (res = hal_i2c_write(i2cport, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }  
    hal_i2c_standby(i2cport, I2CADDRESS);  
 
    regaddr.bytes.one = CTR5_ADR;       
    data = 0x40;        // enable fifo
    if(hal_res_OK != (res = hal_i2c_write(i2cport, I2CADDRESS, regaddr, &data, 1)))
    {
        return(res);
    }
    hal_i2c_standby(i2cport, I2CADDRESS);    
    
    
    memcpy(&info->cfg, cfg, sizeof(hal_chip_st_l3g4200d_cfg_t));
        
    
    // store the i2caddress and the register address.
    return(hal_res_OK);
}


static int32_t s_hal_chip_st_l3g4200d_convert(int32_t v)
{

    int32_t factor =   (int32_t)s_hal_chip_st_l3g4200d_info[0].factor; // 
    uint8_t  shift =            s_hal_chip_st_l3g4200d_info[0].shift; //  

    uint8_t neg = (v < 0) ? (1) : (0);
    int32_t r = (0 == neg) ? (factor*v) : (factor*(-v));
    // now r is positive
    r >>= shift;
    r = (0 == neg) ? (r) : (-r);
    
    return(r);  
}


#endif//HAL_USE_CHIP_ST_L3G4200D


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



