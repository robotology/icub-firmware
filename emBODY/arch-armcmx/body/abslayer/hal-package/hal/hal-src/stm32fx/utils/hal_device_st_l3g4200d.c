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

/* @file       hal_stm32_sensor_gyro.c
	@brief      This file implements internals of the temperature sensor module.
	@author     marco.accame@iit.it
    @date       10/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef  HAL_USE_DEVICE_ST_L3G4200D

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_stm32_base_hid.h" 
#include "hal_brdcfg.h"


#include "stdio.h"

#include "hal_stm32xx_include.h"



 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_st_l3g4200d.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_st_l3g4200d_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_device_st_l3g4200d_cfg_t hal_device_st_l3g4200d_cfg_default  = 
{ 
    .i2cport = hal_i2c_port1 
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_device_st_l3g4200d_initted_set(void);
static hal_boolval_t s_hal_device_st_l3g4200d_initted_is(void);

static hal_result_t s_hal_device_st_l3g4200d_hw_init(const hal_device_st_l3g4200d_cfg_t *cfg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_st_l3g4200d_initted[1] = { hal_false };



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_device_st_l3g4200d_init(const hal_device_st_l3g4200d_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic; // dont remove ...
     
    if(NULL == cfg)
    {
        cfg  = &hal_device_st_l3g4200d_cfg_default;
    }
    
    if(hal_true == s_hal_device_st_l3g4200d_initted_is())
    {
        return(hal_res_OK);
    }    

    if(hal_res_OK != s_hal_device_st_l3g4200d_hw_init(cfg))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_device_st_l3g4200d_initted_set();

    return(hal_res_OK);
}


extern hal_result_t hal_hal_device_st_l3g4200d_temp_get(int8_t* temp)
{
    hal_result_t res = hal_res_NOK_generic; 

    uint8_t data = 0;

    *temp = 0;
    
    if(hal_false == s_hal_device_st_l3g4200d_initted_is())
    {
        return(hal_res_NOK_generic);
    }
    


    hal_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = 0x26;
    res = hal_i2c_read(hal_i2c_port1, 0xd0, regaddr, &data, 1);
    *temp = (int8_t)data;    
   
    return(hal_res_OK);
}

extern hal_result_t hal_hal_device_st_l3g4200d_angrate_get(int16_t* xar, int16_t* yar, int16_t* zar)
{
    hal_result_t res = hal_res_NOK_generic; 

    uint8_t datal = 0;
    uint8_t datah = 0;

    
    *xar = 0;
    *yar = 0;
    *zar = 0;
    
    if(hal_false == s_hal_device_st_l3g4200d_initted_is())
    {
        return(hal_res_NOK_generic);
    }


    hal_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    regaddr.bytes.one = 0x28;
    res = hal_i2c_read(hal_i2c_port1, 0xd0, regaddr, &datal, 1);
    regaddr.bytes.one = 0x29;
    res = hal_i2c_read(hal_i2c_port1, 0xd0, regaddr, &datah, 1);
    *xar = (int16_t)((datah << 8) | datal);    
    
    regaddr.bytes.one = 0x2a;
    res = hal_i2c_read(hal_i2c_port1, 0xd0, regaddr, &datal, 1);
    regaddr.bytes.one = 0x2b;
    res = hal_i2c_read(hal_i2c_port1, 0xd0, regaddr, &datah, 1);
    *yar = (int16_t)((datah << 8) | datal);    

    regaddr.bytes.one = 0x2c;
    res = hal_i2c_read(hal_i2c_port1, 0xd0, regaddr, &datal, 1);
    regaddr.bytes.one = 0x2d;
    res = hal_i2c_read(hal_i2c_port1, 0xd0, regaddr, &datah, 1);
    *zar = (int16_t)((datah << 8) | datal);   
   
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_device_st_l3g4200d_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_device_st_l3g4200d_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }


    memset(s_hal_device_st_l3g4200d_initted, hal_false, sizeof(s_hal_device_st_l3g4200d_initted));
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hal_device_st_l3g4200d_initted_set(void)
{
    s_hal_device_st_l3g4200d_initted[0] = hal_true;
}

static hal_boolval_t s_hal_device_st_l3g4200d_initted_is(void)
{
    return(s_hal_device_st_l3g4200d_initted[0]);
}


static hal_result_t s_hal_device_st_l3g4200d_hw_init(const hal_device_st_l3g4200d_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic;   
    uint8_t data;
    
 
    #warning --> put params in board.c such as ... and then call the init of the s_hal_device_st_l3g4200d_hw_init ...
    // init i2c    
    if(hal_res_OK != (res = hal_i2c_init(cfg->i2cport, NULL)))
    {
        return(res);
    }
    
   
    hal_i2c_regaddr_t regaddr = {.numofbytes = 1, .bytes.one = 0x00 }; 
    
    // whoami: value must be 0xd3
    regaddr.bytes.one = 0x0f;
    if(hal_res_OK != (res = hal_i2c_read(hal_i2c_port1, 0xd0, regaddr, &data, 1)))
    {
        return(res);
    }
    if((hal_res_OK != res) || (0xd3 != data))
    {
        return(hal_res_NOK_generic);
    }
    
    // config now ...
    
    regaddr.bytes.one = 0x20;       
    data = 0x0F; // enable x, y, z and power down normal mode (so that i have temperture readings)
    if(hal_res_OK != (res = hal_i2c_write(hal_i2c_port1, 0xd0, regaddr, &data, 1)))
    {
        return(res);
    }
    hal_i2c_standby(hal_i2c_port1, 0xd0);

#if 0    
    regaddr.bytes.one = 0x23;       
    data = 0x00;
    res = hal_i2c_write(hal_i2c_port1, 0xd0, regaddr, &data, 1);    
    hal_i2c_standby(hal_i2c_port1, 0xd0);
#endif       
 
    regaddr.bytes.one = 0x24;       
    data = 0x40;        // enable fifo
    if(hal_res_OK != (res = hal_i2c_read(hal_i2c_port1, 0xd0, regaddr, &data, 1)))
    {
        return(res);
    }
    hal_i2c_standby(hal_i2c_port1, 0xd0);    
        
    
    // store the i2caddress and the register address.
    return(hal_res_OK);
}


#endif//HAL_USE_DEVICE_ST_L3G4200D


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



