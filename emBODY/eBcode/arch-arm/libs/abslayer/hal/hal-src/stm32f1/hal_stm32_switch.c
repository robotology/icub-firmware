/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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


/*  @file       hal_stm32_encoder.c
	@brief      This file implements communication between spi peripherals and encoders.
	@author     valentina.gaggero@iit.it
    @date       10/18/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_SWITCH

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stm32f1.h"
#include "hal_stm32_base_hid.h" 
#include "hal_i2c4hal.h" //devo usare i2c per accedere allo switch

#include "hal_stm32_switch_hid.h"  //serve.attualemnte non esiste

#include "hal_brdcfg.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_switch.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "hal_stm32_switch_hid.h"  //serve.attualemnte non esiste
#include "hal_stm32_i2c4hal_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_switch_cfg_t hal_switch_cfg_default = 
{ 
    .dummy = 0 
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

//#define HAL_switch_t2index(t)               ((uint8_t)(t))     ho un solo switch

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_switch_supported_is(void);
static void s_hal_switch_initted_set(void);
static hal_boolval_t s_hal_switch_initted_is(void);
static hal_result_t s_hal_switch_reg_config(const hal_switch_cfg_t *cfg);
static hal_result_t s_hal_switch_init(const hal_switch_cfg_t *cfg);
static hal_bool_t s_hal_switch_i2c4switch_initted_is(void);
static void s_hal_switch_i2c4switch_initted_set(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_switch_cfg_t s_hal_switch_cfg = {.dummy = 0};
static hal_boolval_t s_hal_switch_initted =  hal_false ;
static hal_boolval_t s_hal_switch_i2c4switch_initted =  hal_false ;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t hal_switch_init( const hal_switch_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic;


    if(hal_true != s_hal_switch_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_true == s_hal_switch_initted_is())
    {
        return(hal_res_OK);
    }

    if(NULL == cfg)
    {
        cfg = &hal_switch_cfg_default;
    }

    memcpy(&s_hal_switch_cfg, cfg, sizeof(hal_switch_cfg_t));

    res = s_hal_switch_init(cfg);

    s_hal_switch_initted_set();

    return(res);
}



extern hal_bool_t hal_switch_initted_is(void)
{
    return(s_hal_switch_initted_is());
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern uint32_t hal_switch_hid_getsize(const hal_cfg_t *cfg)
{
    return(0);
} 


extern hal_result_t hal_switch_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{

    // remvoed dependency from nzi ram
    memset(&s_hal_switch_cfg, 0, sizeof(s_hal_switch_cfg));
    s_hal_switch_initted                =  hal_false;
    s_hal_switch_i2c4switch_initted     =  hal_false ;

    return(hal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_switch_supported_is(void)
{
    return(hal_brdcfg_switch__supported); 
}


static void s_hal_switch_initted_set(void)
{
    s_hal_switch_initted = hal_true;
    s_hal_switch_initted = s_hal_switch_initted;
}


static hal_boolval_t s_hal_switch_initted_is(void)
{
    // i dont use s_hal_switch_initted ... but i read from registers. i do that so that across a reset we dont do double initialisation
    uint8_t read_buff = 0xFF;
    //first of all verify if switch is in reset state (I have verify state of pin B2)

    if(!(GPIOB->ODR &0x00000004))
    {
        return(hal_false);    
    }

    //if I'm here switch is not in reset state, so i try to read its register status.
    if(hal_false == hal_i2c4hal_hid_initted_is(hal_i2c_port1))
    {
        hal_i2c4hal_init(hal_i2c_port1, NULL); //use default configuration
        s_hal_switch_i2c4switch_initted_set();
    }

    hal_brdcfg_switch__reg_read_byI2C(&read_buff, 0x1);
    if(!(read_buff&0x01))
    {
        return(hal_false);
    }
    else
    {
        return(hal_true);
    }

}

static hal_result_t s_hal_switch_init(const hal_switch_cfg_t *cfg)
{
    hal_result_t res;


    //enable clock on GPIO port A and B
    RCC->APB2ENR |= 0x0000000C;

    //congigure pin A1(Reference clock for switch. )
    GPIOA->CRL   &= 0xFFFFFF0F;
    GPIOA->CRL   |= 0x00000040;    /*set pin 1 in reset state and 2 in alternFunc 50MHz*/
    // il clock sulla porta A e' abilitato all'inizio nella eth init.
    GPIOA->CRH   &= 0xFFFFFFF0;
    GPIOA->CRH   |= 0x0000000B;	/*Pin 8 is congigured in AlterFunc 50MHz*/

    
    //config pin of reset state ( pin B2)
    GPIOB->CRL   &= 0xFFFFF0FF;
    GPIOB->CRL   |= 0x00000200;    /*GPout 2MHz*/ 
    GPIOB->BSRR  |= 0x00000004;  /* Exit phy from reset state */


    res = s_hal_switch_reg_config(cfg);

    return(res);

}

static hal_result_t s_hal_switch_reg_config(const hal_switch_cfg_t *cfg)
{
    //uint8_t buff_write = 0x60; //FORCE FULL DUPLEX AND 100T
    uint8_t buff_write = 0x88; //AUTONEGOTATION + ADVERTISE FULLDUPLEX 100
    uint8_t buff_read = 0xFF; 
    volatile uint32_t i = 1;

    while(i<6500000) //6500000
    {
        i++;
    }

    if(hal_false == s_hal_switch_i2c4switch_initted_is())
    {
        hal_i2c4hal_init(hal_i2c_port1, NULL); //use default configuration
        s_hal_switch_i2c4switch_initted_set();
    }


    //configure  switch's port1 in full duplex and 100T
    hal_brdcfg_switch__reg_write_byI2C(&buff_write, 0x1C);
    //configure  switch's port2 in full duplex and 100T
    hal_brdcfg_switch__reg_write_byI2C(&buff_write, 0x2C);

    //start the switch
    buff_write = 0x1;    
    hal_brdcfg_switch__reg_write_byI2C(&buff_write, 0x01);
    //TODO: reader from register to verify if it si started

    hal_brdcfg_switch__reg_read_byI2C(&buff_read, 0x1);
    if(!(buff_read&0x01))
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "s_hal_switch_reg_config(): SWITCH is still in reset state");
    }


     return(hal_res_OK);
}


static hal_bool_t s_hal_switch_i2c4switch_initted_is(void)
{
    return(s_hal_switch_i2c4switch_initted);
}


static void s_hal_switch_i2c4switch_initted_set(void)
{
   s_hal_switch_i2c4switch_initted = hal_true;
} 
 
#endif//HAL_USE_SWITCH

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
