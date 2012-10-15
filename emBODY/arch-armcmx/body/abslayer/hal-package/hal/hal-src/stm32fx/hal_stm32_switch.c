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
#include "hal_stm32xx_include.h"
#include "hal_stm32_base_hid.h" 
#include "hal_i2c4hal.h" 

#include "hal_sys.h"
#include "hal_stm32_eth_hid.h"

#include "hal_stm32_i2c4hal_hid.h"

#include "hal_brdcfg.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_switch.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_switch_hid.h" 



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
static void s_hal_switch_started_set(void);
static hal_boolval_t s_hal_switch_started_is(void);

static hal_result_t s_hal_switch_reg_config(void);
static hal_result_t s_hal_switch_lowlevel_init(const hal_switch_cfg_t *cfg);
static hal_result_t s_hal_switch_lowlevel_start(void);





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_switch_cfg_t s_hal_switch_cfg            = {.dummy = 0};
static hal_boolval_t s_hal_switch_initted           =  hal_false;
static hal_boolval_t s_hal_switch_started           =  hal_false;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_switch_init(const hal_switch_cfg_t *cfg)
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

    res = s_hal_switch_lowlevel_init(cfg);

    s_hal_switch_initted_set();

    return(res);
}


extern hal_result_t hal_switch_start(void)
{
    hal_result_t res = hal_res_NOK_generic;


    if(hal_true != s_hal_switch_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_false == s_hal_switch_initted_is())
    {
        return(hal_res_NOK_generic);
    }
    

    if(hal_true == s_hal_switch_started_is())
    {
        return(hal_res_OK);
    }    


    res = s_hal_switch_lowlevel_start();

    s_hal_switch_started_set();

    return(res);
 
}


extern hal_bool_t hal_switch_initted_is(void)
{
    return(s_hal_switch_initted_is());
}


extern hal_bool_t hal_switch_started_is(void)
{
    return(s_hal_switch_started_is());
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
    s_hal_switch_started                =  hal_false;

    return(hal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_switch_supported_is(void)
{
    return(hal_brdcfg_switch__supported); 
}

static hal_boolval_t s_hal_switch_initted_is(void)
{
    return(s_hal_switch_initted);
}

static void s_hal_switch_initted_set(void)
{
    s_hal_switch_initted = hal_true;
}


static hal_boolval_t s_hal_switch_started_is(void)
{
    return(s_hal_switch_started);
}


static void s_hal_switch_started_set(void)
{
    s_hal_switch_started = hal_true;
}





// static hal_result_t s_hal_switch_lowlevel_init_old(const hal_switch_cfg_t *cfg)
// {
//     hal_result_t res;
//     
//     // --- mco: PA8
//     init_mco();
//     
//     // --- reset pin: PB2
//     // clock gpioa as normal gpio, reset pin 2, and set it as: Output mode, max speed 2 MHz + General purpose output push-pull
//     RCC->APB2ENR |= 0x00000008;
//     GPIOB->CRL   &= 0xFFFFF0FF;
//     GPIOB->CRL   |= 0x00000200;    // out 2mhz
//     // put in reset state (low) for some time ... 10 ms according to datasheet.
//     GPIOB->BRR   |= (1 << 2); 
//     hal_sys_delay(10*1000);
//     // put value high to exit from reset state
//     GPIOB->BSRR  |= (1 << 2);
//     // now wait for 100 usec before using i2c etc.
//     hal_sys_delay(100);
//     
//     
//     // --- ETH_RMII_REF_CLK: PA1  [we do that as well, even if eth does that. for now......] 
//     hal_eth_hid_rmii_refclock_init();
//     
//     
// #if 0    
//     
//     // pins: 
//     // PB2 is reset of device (0 -> reset, 1 -> works)
//     // PA1 is the ETH_RMII_REF_CLK. it is configured by eth in reset state and then alternate function eth ... it belongs to eth.c
//     // PA8 is MCO. it must be configured = Output mode, max speed 50 MHz + Alternate function output Push-pull (B)
//     //             also ... the pll must be started and connected to it.
//     //             it is not configured by eth
//     //             it belongs to swicth

//     //enable clock on GPIO port A and B
//     RCC->APB2ENR |= 0x0000000C;

//     //configure pin A1(Reference clock for switch. )
//     GPIOA->CRL   &= 0xFFFFFF0F;
//     GPIOA->CRL   |= 0x00000040;    /* set pin 1 in reset state */

//     
//     // il clock sulla porta A e' abilitato all'inizio nella eth init.
//     GPIOA->CRH   &= 0xFFFFFFF0;
//     GPIOA->CRH   |= 0x0000000B;	/*Pin 8 is congigured in AlterFunc 50MHz*/
//     // Output mode, max speed 50 MHz. + Alternate function output Push-pull
//     // PA8 is MCO pin

//     
//     //config pin of reset state ( pin B2)
//     GPIOB->CRL   &= 0xFFFFF0FF;
//     GPIOB->CRL   |= 0x00000200;    /* GPout 2MHz */ 
//     GPIOB->BSRR  |= 0x00000004;    /* Exit phy from reset state */

// #endif

//     res = s_hal_switch_reg_config();

//     return(res);

// }

// static hal_result_t s_hal_switch_reg_config_old(void)
// {
//     uint8_t buff_write = 0x60; //FORCE FULL DUPLEX AND 100T
//     uint8_t buff_read = 0xFF; 
//     volatile uint32_t i = 1;

//     // just waits for some time that the swicth has exited the reset state ... change with its correct value
//     // from micrel datasheet at pag 111: 
//     // "after the de-assertion of teh reset, it is recommended to wait a minimum of 100 usec before strating programming on teh managed interface (i2c slave, spi slave, smi, mii)"
// //    while(i<6500000) //6500000
// //    {
// //        i++;
// //    }

//    if(hal_false == s_hal_switch_i2c4switch_initted_is())
//    {
//        hal_i2c4hal_init(hal_i2c_port1, NULL); // use default configuration
//        s_hal_switch_i2c4switch_initted_set();
//    }


//     // now ...
//     // 1. configure full duplex 100mbps
//     
//     
//     // 2. start switch
//     
//     
//     // 3. read back to verify
//     
//     
//     //configure  switch's port1 in full duplex and 100T
// //    hal_brdcfg_switch__reg_write_byI2C(&buff_write, 0x1C);
//     //configure  switch's port2 in full duplex and 100T
// //    hal_brdcfg_switch__reg_write_byI2C(&buff_write, 0x2C);

//     //start the switch
//     buff_write = 0x1;    
// //    hal_brdcfg_switch__reg_write_byI2C(&buff_write, 0x01);
//     //TODO: reader from register to verify if it si started

//  //   hal_brdcfg_switch__reg_read_byI2C(&buff_read, 0x1);
//     if(!(buff_read&0x01))
//     {
//         hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "s_hal_switch_reg_config(): SWITCH is still in reset state");
//     }


//      return(hal_res_OK);
// }




// static hal_boolval_t s_hal_switch_i2c4switch_initted_is(void)
// {
//     return(s_hal_switch_i2c4switch_initted);
// }


// static void s_hal_switch_i2c4switch_initted_set(void)
// {
//    s_hal_switch_i2c4switch_initted = hal_true;
// } 


// // old functions
// static hal_bool_t oldmode_switch_initted_is(void)
// {
//     // i dont use s_hal_switch_initted ... but i read from registers. i do that so that across a reset we dont do double initialisation
//     uint8_t read_buff = 0xFF;
//     //first of all verify if switch is in reset state (I have verify state of pin B2)

//     if(!(GPIOB->ODR &0x00000004))
//     {
//         return(hal_false);    
//     }

//     //if I'm here switch is not in reset state, so i try to read its register status.
//     if(hal_false == hal_i2c4hal_hid_initted_is(hal_i2c_port1))
//     {
//         hal_i2c4hal_init(hal_i2c_port1, NULL); //use default configuration
//         s_hal_switch_i2c4switch_initted_set();
//     }

// //    hal_brdcfg_switch__reg_read_byI2C(&read_buff, 0x1);
//     if(!(read_buff&0x01))
//     {
//         return(hal_false);
//     }
//     else
//     {
//         return(hal_true);
//     }

// }

#if 0
// stm32f1
static void init_mco(void)
{
    // this function initialises MCO in order to provide clock ref to switch.
    // PA8 is MCO. it must be configured = Output mode, max speed 50 MHz + Alternate function output Push-pull (B)
    // also, we connect pll3 at 50mhz to it
    
    // clock gpioa as alternate function
    RCC->APB2ENR    |= 0x00000005;
    // init pa8
    GPIOA->CRH   &= 0xFFFFFFF0;
    GPIOA->CRH   |= 0x0000000B;	


    // set pll3 clock output to 50mhz: (25mhz/5)*10 = 50mhz, thus we use multiplier 10
    RCC_PLL3Config(RCC_PLL3Mul_10);
        
    // enable pll3 
    RCC_PLL3Cmd(ENABLE);
    
    // wait until it is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET);
    
    // connect mco on pa8 with pll3
    RCC_MCOConfig(RCC_MCO_PLL3CLK);
}
#endif

static hal_result_t s_hal_switch_init_old(const hal_switch_cfg_t *cfg)
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

    res = s_hal_switch_init_old(cfg);

    s_hal_switch_initted_set();

    return(res);
}


#undef DDDEBUGGG

#if !defined(DDDEBUGGG)
// safe one
static hal_result_t s_hal_switch_lowlevel_init(const hal_switch_cfg_t *cfg)
{
    
#if 1
    hal_brdcfg_switch__initialise();
    return(hal_res_OK);    
#else    
    
    // --- reset pin: PB2
    // clock gpioa as normal gpio, reset pin 2, and set it as: Output mode, max speed 2 MHz + General purpose output push-pull
    RCC->APB2ENR |= 0x00000008;
    GPIOB->CRL   &= 0xFFFFF0FF;
    GPIOB->CRL   |= 0x00000200;    // out 2mhz
    // put in reset state (low) for some time ... 10 ms according to datasheet.
    GPIOB->BRR   |= (1 << 2); 
    hal_sys_delay(10*1000);
    // put value high to exit from reset state
    GPIOB->BSRR  |= (1 << 2);
    // now wait for 100 usec before using i2c etc.
    hal_sys_delay(100);
        
    
    // --- mco: PA8
    init_mco();    

    
    // --- ETH_RMII_REF_CLK: PA1  [we do that as well, even if eth does that. for now......] 
    //hal_eth_hid_rmii_refclock_init(); the switch seems not needing that 
    
    
    // --- i2c for communication 
    hal_i2c4hal_init(hal_i2c_port1, NULL); // use default configuration
    
    return(hal_res_OK);
    
#endif    
}
#else
// tentativo di inizializzare solo se lo switch non si e' spento, ma sfortunatamente si spegne sempre con un sw reset e quindi con 


void reset_assert(void)
{
    // put in reset state (low) for some time ... 10 ms according to datasheet.
    GPIOB->BRR   |= (1 << 2); 
    hal_sys_delay(10*1000);
    // put value high to exit from reset state
    GPIOB->BSRR  |= (1 << 2);
}

static hal_result_t s_hal_switch_lowlevel_init(const hal_switch_cfg_t *cfg)
{
    
    // --- reset pin: PB2
    // clock gpioa as normal gpio, reset pin 2, and set it as: Output mode, max speed 2 MHz + General purpose output push-pull
    RCC->APB2ENR |= 0x00000008;
    GPIOB->CRL   &= 0xFFFFF0FF;
    GPIOB->CRL   |= 0x00000200;    // out 2mhz
    
    GPIOB->BSRR  |= (1 << 2);
    
        
    
    // --- mco: PA8
    init_mco();    

    
    // --- ETH_RMII_REF_CLK: PA1  [we do that as well, even if eth does that. for now......] 
    hal_eth_hid_rmii_refclock_init();
    
    
    // --- i2c for communication 
    hal_i2c4hal_init(hal_i2c_port1, NULL); // use default configuration
    s_hal_switch_i2c4switch_initted_set(); // to be removed later on
    
    
    // better wait for 100 usec before using i2c etc.
    hal_sys_delay(100);
    
    uint8_t buff_read = 0xFF; 
    hal_i2c4hal_read(hal_i2c_port1, 0xBE, 0x01, &buff_read, 1);
    if(!(buff_read&0x01))
    {
        reset_assert();
        
        // now wait for 100 usec before using i2c etc.
        hal_sys_delay(100);
    }
    

    

    return(hal_res_OK);
}


#endif



static hal_result_t s_hal_switch_lowlevel_start(void)
{
    return(s_hal_switch_reg_config());
}

static hal_result_t s_hal_switch_reg_config(void)
{
    const uint8_t fd100 = 0x60;
    const uint8_t fd010 = 0x20;
    uint8_t buff_write = 0x60; // FORCE FULL DUPLEX AND 100T
    uint8_t buff_read = 0xFF; 
    volatile uint32_t i = 1;
    hal_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};

    regadr.bytes.one = 0x01;
    hal_i2c4hal_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
    if((buff_read&0x01))
    {   // already initted. to be initted again must pass through a reset
        return(hal_res_OK);
    }
 
//     // configure mii port at 10mbps. default is 100mbps
//     regadr.bytes.one = 0x06;
//     hal_i2c4hal_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     buff_write = buff_read | 0x10;
//     hal_i2c4hal_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
//     hal_i2c4hal_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
   
    #warning --> set 100mbps back .............. with 0x60
    // 1. configure  switch's ports 1 and 2 in full duplex and 100mbps
    buff_write = fd010;      
    regadr.bytes.one = 0x1C;
    hal_i2c4hal_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
    hal_i2c4hal_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
     
    regadr.bytes.one = 0x2C;
    hal_i2c4hal_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);   
    hal_i2c4hal_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);

    // 2. start the switch
    buff_write = 0x1;  
    regadr.bytes.one = 0x01;    
    hal_i2c4hal_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
    

    // 3. read back to verify
    regadr.bytes.one = 0x01;
    hal_i2c4hal_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
    if(!(buff_read&0x01))
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "s_hal_switch_reg_config(): SWITCH not configured");
    }

    return(hal_res_OK);
}


#endif//HAL_USE_SWITCH

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
