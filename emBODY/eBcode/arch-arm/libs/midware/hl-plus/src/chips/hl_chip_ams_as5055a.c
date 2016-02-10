/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       hl_chip_ams_as5055a.c
    @brief      This file implements internals of a ST L3G4200D gyroscope
    @author     marco.accame@iit.it
    @date       10/29/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"

#define HL_USE_CHIP_AMS_AS5055A

#if     defined(HL_USE_CHIP_AMS_AS5055A)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "stdio.h"

#include "hl_bits.h"
#include "hl_sys.h"

#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files

#include "hl_arch.h"

#include "hal_as5048.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_ams_as5055a.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_ams_as5055a_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HL_chip_channel2index(p)              ((uint8_t)((p)))


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hl_chip_ams_as5055a_cfg_t               config;
    uint16_t                                values[hl_chip_ams_as5055a_max_chips_in_channel];
} hl_chip_ams_as5055a_internal_item_t;

typedef struct
{
    uint32_t                                inittedmask;
    hl_chip_ams_as5055a_internal_item_t*    items[hl_chip_ams_as5055a_channels_numberof];   
} hl_chip_ams_as5055a_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_chip_ams_as5055a_initted_set(hl_chip_ams_as5055a_channel_t chn);
//static void s_hl_chip_ams_as5055a_initted_reset(hl_chip_ams_as5055a_channel_t chn);
static hl_boolval_t s_hl_chip_ams_as5055a_initted_is(hl_chip_ams_as5055a_channel_t chn);

static hl_result_t s_hl_chip_ams_as5055a_hw_init(const hl_chip_ams_as5055a_cfg_t *cfg, hl_chip_ams_as5055a_internal_item_t* intitem);


static hl_result_t s_hl_chip_ams_as5055a_software_reset(hl_chip_ams_as5055a_channel_t chn);

static hl_result_t s_hl_chip_ams_as5055a_master_reset(hl_chip_ams_as5055a_channel_t chn);

static hl_result_t s_hl_chip_ams_as5055a_clearerrorflag(hl_chip_ams_as5055a_channel_t chn);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

// it the advanced spi config to use. 
static const hl_spi_advcfg_t s_hl_chip_ams_as5055a_spiadvancedconfig =
{
    .SPI_Direction          = SPI_Direction_2Lines_FullDuplex,
    .SPI_Mode               = SPI_Mode_Master,                            
    .SPI_DataSize           = SPI_DataSize_16b, 
    .SPI_CPOL               = SPI_CPOL_Low, 
    .SPI_CPHA               = SPI_CPHA_2Edge,
    .SPI_NSS                = SPI_NSS_Soft,
    .SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_64, 
    .SPI_FirstBit           = SPI_FirstBit_MSB, 
    .SPI_CRCPolynomial      = 0x0007           
};
        

static const uint16_t s_Command_READ_ClearErrorFlag = 0xE700; // it is 0x3380 shifted up by 1 with the bit  15 = 1 and crc bit adjusted to 0.
static const uint16_t s_Command_READ_ErrorStatus = 0xE6B5; // it is 0x335A of Figure 22 at page 19, shifted up by 1 bit, with the bit 15 = 1 (READs and with bit 0 adjusted for parity 
static const uint16_t s_Command_READ_AngularData = 0xFFFF; // it is 0x3fff of Figure 22 at page 19, shifted up by 1 bit, with the bit 15 = 1 (READs and with bit 0 adjusted for parity 
static const uint16_t s_Command_NOP = 0x0000;
static const uint16_t s_Command_WRITE_MasterReset = 0x674A; // it is 0x33A5 of Figure 22 at page 19, shifted up by 1 bit, with bit 15 = 0.
static const uint16_t s_Command_WRITE_SoftwareReset = 0x7800; // it is 0x3C00 of Figure 22 at page 19, shifted up by 1 bit, with bit 15 = 0.

static const hl_reltime_t s_Treadout = 500;  // 500 usec


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// spiconfig = { .mode = hl_spi_mode_master, .prescaler = hl_spi_prescaler_128, .advcfg = NULL }
static hl_chip_ams_as5055a_internal_item_t hl_chip_ams_as5055a_items[hl_chip_ams_as5055a_channels_numberof] = 
{ 
    {
        .config = { .spiid = hl_spi1, .numberofchained = 0, .spicfg = NULL, .nsel = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE }, .nint = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE } }, 
        .values = {0}
    }
}; 

static hl_chip_ams_as5055a_theinternals_t s_hl_chip_ams_as5055a_theinternals =
{
    .inittedmask        = hl_false,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------





extern hl_result_t hl_chip_ams_as5055a_init(hl_chip_ams_as5055a_channel_t chn, const hl_chip_ams_as5055a_cfg_t *cfg)
{
    if(chn >= hl_chip_ams_as5055a_channels_numberof)
    {
        return(hl_res_NOK_generic);
    } 
     
    if(NULL == cfg)
    {
        return(hl_res_NOK_generic);
    }
    
    if((0 == cfg->numberofchained) || (cfg->numberofchained > hl_chip_ams_as5055a_max_chips_in_channel))
    {
        return(hl_res_NOK_generic);
    }
    
    if(hl_true == s_hl_chip_ams_as5055a_initted_is(chn))
    {
        return(hl_res_OK);
    }  
    
    hl_chip_ams_as5055a_internal_item_t *intitem = s_hl_chip_ams_as5055a_theinternals.items[HL_chip_channel2index(chn)];    

    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        // those of you scared of heap may use static ram
        //intitem = s_hl_chip_ams_as5055a_theinternals.items[HL_chip_channel2index(chn)] = hl_sys_heap_new(sizeof(hl_chip_ams_as5055a_internal_item_t));  
        intitem = s_hl_chip_ams_as5055a_theinternals.items[HL_chip_channel2index(chn)] = &hl_chip_ams_as5055a_items[HL_chip_channel2index(chn)];
    }  
    

    if(hl_res_OK != s_hl_chip_ams_as5055a_hw_init(cfg, intitem))
    {
        return(hl_res_NOK_generic);
    }
    
    hl_sys_delay(s_Treadout);
    
    
    s_hl_chip_ams_as5055a_initted_set(chn);
    
    // keep them after s_hl_chip_ams_as5055a_initted_set() otherwise they dont execute
    hl_chip_ams_as5055a_reset(chn, hl_chip_ams_as5055a_resetmode_software_plus_spiregisters); // or hl_chip_ams_as5055a_resetmode_master
    hl_chip_ams_as5055a_reset(chn, hl_chip_ams_as5055a_resetmode_clearerrorflag);
        

    return(hl_res_OK);
}


extern hl_result_t hl_chip_ams_as5055a_reset(hl_chip_ams_as5055a_channel_t chn, hl_chip_ams_as5055a_resetmode_t resetmode)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    if(hl_false == s_hl_chip_ams_as5055a_initted_is(chn))
    {
        return(hl_res_NOK_generic);
    }
#endif  
    
    hl_result_t res = hl_res_NOK_generic;
    
    if(hl_chip_ams_as5055a_resetmode_master == resetmode)
    {
        res = s_hl_chip_ams_as5055a_master_reset(chn);
    }
    else if(hl_chip_ams_as5055a_resetmode_software_plus_spiregisters == resetmode)
    {
        res = s_hl_chip_ams_as5055a_software_reset(chn);
    }
    else if(hl_chip_ams_as5055a_resetmode_clearerrorflag == resetmode)
    {
        res = s_hl_chip_ams_as5055a_clearerrorflag(chn);
    }

    return(res);
}


extern hl_result_t hl_chip_ams_as5055a_read_errorstatus(hl_chip_ams_as5055a_channel_t chn, uint16_t* value0, uint16_t* value1, uint16_t* value2)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    if(hl_false == s_hl_chip_ams_as5055a_initted_is(chn))
    {
        return(hl_res_NOK_generic);
    }
#endif  


    const uint32_t tick = 1;
    uint16_t tmpvalues[hl_chip_ams_as5055a_max_chips_in_channel] = {0};
    hl_chip_ams_as5055a_internal_item_t *intitem = s_hl_chip_ams_as5055a_theinternals.items[HL_chip_channel2index(chn)];
    hl_spi_t spiid = intitem->config.spiid;
    uint8_t nchained = intitem->config.numberofchained;
    uint8_t i = 0;


    // enable spi
    hl_spi_enable(spiid);
    
    // step 1: start    
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valRESET);
//    hl_sys_delay(tick); 

    for(i=0; i<nchained; i++)
    {
        hl_spi_send_receive_raw(spiid, s_Command_READ_ErrorStatus, &tmpvalues[hl_chip_ams_as5055a_max_chips_in_channel-1-i]);
//        hl_sys_delay(tick);
    }
    
    hl_spi_wait_until_completion(spiid);
    
//    hl_sys_delay(tick);
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valSET);

    
    // step 2: wait     either wait for the pin in intitem->config.nint.gpio to become high or ... wait for the maximum time of 500 usec.
    
    hl_sys_delay(s_Treadout);
    
    
    // step 3: read
    // marco.accame: instead of s_Command_READ_AngularData we may send the null command 0x0000 .... maybe it work better because it does not triggers new readings ..
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valRESET);
//    hl_sys_delay(tick);
    
    for(i=0; i<nchained; i++)
    {
        hl_spi_send_receive_raw(spiid, s_Command_NOP, &intitem->values[hl_chip_ams_as5055a_max_chips_in_channel-1-i]);
//        hl_sys_delay(2*s_Treadout);
    }
       
    hl_spi_wait_until_completion(spiid);
     
//    hl_sys_delay(tick);
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valSET);    
    
    
    // disable spi
    hl_spi_disable(spiid);    
 

    
    // step 4: copy results back
    if(NULL != value0)
    {
        *value0 = intitem->values[2];
    }

    if(NULL != value1)
    {
        *value1 = intitem->values[1];
    }

    if(NULL != value2)
    {
        *value2 = intitem->values[0];
    }
    
    return(hl_res_OK);
}


extern hl_result_t hl_chip_ams_as5055a_read_angulardata(hl_chip_ams_as5055a_channel_t chn, hl_chip_ams_as5055a_readmode_t readmode, uint16_t* value0, uint16_t* value1, uint16_t* value2)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    if(hl_false == s_hl_chip_ams_as5055a_initted_is(chn))
    {
        return(hl_res_NOK_generic);
    }
#endif  

    if(readmode != hl_chip_ams_as5055a_readmode_start_wait_read)
    {
        return(hl_res_NOK_generic);
    }

    const uint32_t tick = 1;
    uint16_t tmpvalues[hl_chip_ams_as5055a_max_chips_in_channel] = {0};
    hl_chip_ams_as5055a_internal_item_t *intitem = s_hl_chip_ams_as5055a_theinternals.items[HL_chip_channel2index(chn)];
    hl_spi_t spiid = intitem->config.spiid;
    uint8_t nchained = intitem->config.numberofchained;
    uint8_t i = 0;

    // now i write the read-angle command (0xffff), i wait for the safe time (or poll until the nint pin is high) for a number of times
    // specified by nchained.
    // here is example with nchained = 2 taken from page "AS5055A – 18" of "ams Datasheet: 2014-Apr-28 [v2-02]"
    // 
    //  MOSI    <0xFFFF>    <0xFFFF>        <0xFFFF>    <0xFFFF>   
    //
    /// MISO    <      >    <      >        <angle1>    <angle0>    
    //
    //  NSEL --|____________________|------|____________________|------   
    //
    //  NINT _______________________|---|_______________________|---|__   
    //
    // EXPLANATION:
    // we write the read-angle commands on MOSI with NSEL driven low. the two commands propagate to first and then second chip in daisy chain.
    // from the datasheet at pag 10: 
    // > Sending a READ ANGLE command through the SPI interface automatically powers up the chip, drives INT/ high and starts another angle measurement. 
    // > The completion of the angle measurement is indicated by driving the INT/ output low and clearing the WOW flag in the error status register. The
    // > microcontroller can respond to the interrupt by reading the angle value from the AS5055A over the SPI interface. (See Figure 11).
    // > A READ ANGLE command must not be sent while a measurement is being performed as indicated by INT/ driven high or WOW = 1.
    // thus: after having sent two first two <0xFFFF> we must wait until NINT is low again, and then send other two <0xFFFF>.
    // The values read on MISO are the two values, where the first read is the angle from the more distant chip in the chain.
    // Question: how much does the NINT stay high? The answer is in datasheet at page 10:
    // > After the INT/ output is driven low the device goes into low-power mode. If the microcontroller doesn't monitor the
    // > INT/ output a minimum guard time (treadout) must be inserted before the next READ ANGLE command can be sent.
    // At page 6, figure 7 (system parameters):
    // >    Symbol          Parameter           Conditions                                          Min     Typ             Max         Units
    // >    treadout        Readout rate        Time between READ ANGLE command and INTERRUPT       -       -               500         μs
    // >


#if defined(TEST_AMS_AS5055A_AS_MAGGIALI)

    // --- enable spi3
    SPI_Cmd(SPI3, ENABLE);
    
    
    // -- this time we prepare

    // reset nsel
    GPIO_ResetBits(SPI3_NSEL_GPIO_PORT, SPI3_NSEL_PIN);
    //hl_sys_delay(tick);

    // send first command and retrieve data
    SPI_I2S_SendData(SPI3, 0xffff);   //send a dummy value 
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)==0);
	tmpvalues[2] = SPI_I2S_ReceiveData(SPI3);
    //hl_sys_delay(2*s_Treadout);
    // send second command and retrieve data
    SPI_I2S_SendData(SPI3, 0xffff);   //send a dummy value 
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)==0);
	tmpvalues[1] = SPI_I2S_ReceiveData(SPI3);
    
    // set nsel
    //hl_sys_delay(tick);
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));
    GPIO_SetBits(SPI3_NSEL_GPIO_PORT, SPI3_NSEL_PIN);    
    
    // -- we wait for some time so that nint goes low
//    hl_sys_delay(2*s_Treadout);
    
    
    // -- this time we read again to get the final values
    
    // reset nsel
    GPIO_ResetBits(SPI3_NSEL_GPIO_PORT, SPI3_NSEL_PIN);
    //hl_sys_delay(tick);

    
    // send first command and retrieve data
    SPI_I2S_SendData(SPI3, 0xffff);   //send a dummy value 
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)==0);
	intitem->values[2] = SPI_I2S_ReceiveData(SPI3);
    //hl_sys_delay(2*s_Treadout);
    // send second command and retrieve data
    SPI_I2S_SendData(SPI3, 0xffff);   //send a dummy value 
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)==0);
	intitem->values[1] = SPI_I2S_ReceiveData(SPI3);
    
    // set nsel
    //hl_sys_delay(tick);
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));
    GPIO_SetBits(SPI3_NSEL_GPIO_PORT, SPI3_NSEL_PIN);        
    

    // --- disable spi
    hl_spi_disable(spiid);  

#else

    // enable spi
    hl_spi_enable(spiid);
    
    // step 1: start    
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valRESET);
//    hl_sys_delay(tick); 

    for(i=0; i<nchained; i++)
    {  
        uint16_t data = s_Command_READ_AngularData; // or 0x000 
        hl_spi_send_receive_raw(spiid, data, &tmpvalues[hl_chip_ams_as5055a_max_chips_in_channel-1-i]);
//        hl_sys_delay(tick);
    }
    
    hl_spi_wait_until_completion(spiid);
    
//    hl_sys_delay(tick);
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valSET);

    
    // step 2: wait     either wait for the pin in intitem->config.nint.gpio to become high or ... wait for the maximum time of 500 usec.
    
    hl_sys_delay(s_Treadout);
    
    
    // step 3: read
    // marco.accame: instead of s_Command_READ_AngularData we may send the null command 0x0000 .... maybe it work better because it does not triggers new readings ..
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valRESET);
//    hl_sys_delay(tick);
    
    for(i=0; i<nchained; i++)
    {
        uint16_t data = 0x0000; // or s_Command_READ_AngularData
        hl_spi_send_receive_raw(spiid, data, &intitem->values[hl_chip_ams_as5055a_max_chips_in_channel-1-i]);
//        hl_sys_delay(2*s_Treadout);
    }
       
    hl_spi_wait_until_completion(spiid);
     
//    hl_sys_delay(tick);
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valSET);    
    
    
    // disable spi
    hl_spi_disable(spiid);    
 
#endif 
    
    // step 4: copy results back
    if(NULL != value0)
    {
        *value0 = intitem->values[2];
    }

    if(NULL != value1)
    {
        *value1 = intitem->values[1];
    }

    if(NULL != value2)
    {
        *value2 = intitem->values[0];
    }
    
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

static void s_hl_chip_ams_as5055a_initted_set(hl_chip_ams_as5055a_channel_t chn)
{
    hl_bits_word_bitset(&s_hl_chip_ams_as5055a_theinternals.inittedmask, HL_chip_channel2index(chn));
}


static hl_boolval_t s_hl_chip_ams_as5055a_initted_is(hl_chip_ams_as5055a_channel_t chn)
{
    return(hl_bits_word_bitcheck(s_hl_chip_ams_as5055a_theinternals.inittedmask, HL_chip_channel2index(chn)));
}


static hl_result_t s_hl_chip_ams_as5055a_hw_init(const hl_chip_ams_as5055a_cfg_t *cfg, hl_chip_ams_as5055a_internal_item_t *intitem)
{
    hl_spi_t spiid = cfg->spiid;
    
#if defined(TEST_AMS_AS5055A_AS_MAGGIALI) 
    
    as5048_init(0);
    as5048_init(1);
    
#else    
    
    
    if(NULL != cfg->spicfg)
    {   // must init spi with this field. 
        
        // 1. init hl spimap: it is needed by hl_spi to find sck, miso, mosi
        hl_spi_map = cfg->spicfg->spimap2use;
        
        // 2. define a proper hl_spi_cfg_t        
        hl_spi_cfg_t spiconfig = {0};
        spiconfig.mode = hl_spi_mode_master;
        spiconfig.prescaler = cfg->spicfg->prescaler;
        spiconfig.advcfg = &s_hl_chip_ams_as5055a_spiadvancedconfig;
        
        // 3. call hl_spi_init
        hl_result_t r = hl_spi_init(cfg->spiid, &spiconfig);      // the gpio, the clock, the peripheral: everything apart isr and start
        if(hl_res_OK != r)
        {
            return(r);
        }        
    }
    
    
    
    // is spi bus already initialised?
    if(hl_false == hl_spi_initted_is(spiid))
    {
        return(hl_res_NOK_generic);
    }   
    
    // config spi ...
    
    
    // config nsel as output
    static const hl_gpio_init_t outgpioinit = 
    {
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins  = 0,
            .gpio_mode  = GPIO_Mode_OUT,
            .gpio_speed = GPIO_Speed_50MHz,
            .gpio_otype = GPIO_OType_PP,
            .gpio_pupd  = GPIO_PuPd_NOPULL
        }      
    };
    
    hl_gpio_init_t gpioinit;
    hl_gpio_map_t gpiomap;
    memcpy(&gpioinit, &outgpioinit, sizeof(hl_gpio_init_t));
    gpiomap.gpio.port = cfg->nsel.port;
    gpiomap.gpio.pin  = cfg->nsel.pin;
    gpiomap.af32      = hl_NA32;
    hl_gpio_fill_init(&gpioinit, &gpiomap);
    hl_gpio_init(&gpioinit);  
    
    // config nint as input
    
    static const hl_gpio_init_t inpgpioinit = 
    {
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins  = 0,
            .gpio_mode  = GPIO_Mode_IN,
            .gpio_speed = GPIO_Speed_50MHz,
            .gpio_otype = GPIO_OType_PP,
            .gpio_pupd  = GPIO_PuPd_NOPULL
        }      
    };

    
    if((hl_gpio_portNONE != cfg->nint.port) && (hl_gpio_pinNONE != cfg->nint.pin))
    {
        memcpy(&gpioinit, &inpgpioinit, sizeof(hl_gpio_init_t));
        gpiomap.gpio.port = cfg->nint.port;
        gpiomap.gpio.pin  = cfg->nint.pin;
        gpiomap.af32      = hl_NA32;
        hl_gpio_fill_init(&gpioinit, &gpiomap);
        hl_gpio_init(&gpioinit);  
    }
    
#endif
    
    memcpy(&intitem->config, cfg, sizeof(hl_chip_ams_as5055a_cfg_t));
    
    memset(&intitem->values, 0, sizeof(intitem->values));

    return(hl_res_OK);
}


static hl_result_t s_hl_chip_ams_as5055a_software_reset(hl_chip_ams_as5055a_channel_t chn)
{
    const uint32_t tick = 1;
    uint16_t tmpvalues[2*hl_chip_ams_as5055a_max_chips_in_channel] = {0};
    hl_chip_ams_as5055a_internal_item_t *intitem = s_hl_chip_ams_as5055a_theinternals.items[HL_chip_channel2index(chn)];
    hl_spi_t spiid = intitem->config.spiid;    
    uint8_t nchained = intitem->config.numberofchained;
    uint8_t i = 0;
    
   // enable spi
    hl_spi_enable(spiid);
    
    // step 1: start
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valRESET);
//    hl_sys_delay(tick); 
    
    const uint16_t DataPackage = 0x0006; // it resets spi register

    for(i=0; i<nchained; i++)
    {
        hl_spi_send_receive_raw(spiid, s_Command_WRITE_SoftwareReset, &tmpvalues[2*hl_chip_ams_as5055a_max_chips_in_channel-1 -2*i]);
        hl_spi_send_receive_raw(spiid, DataPackage, &tmpvalues[2*hl_chip_ams_as5055a_max_chips_in_channel-1 -2*i-1]);
    }
        
    hl_spi_wait_until_completion(spiid);
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valSET);
    
    // wait either wait for the pin in intitem->config.nint.gpio to become high or ... wait for the maximum time of 500 usec.
    
    hl_sys_delay(s_Treadout);
        
    // disable spi
    hl_spi_disable(spiid);        

    return(hl_res_NOK_generic);  
}


static hl_result_t s_hl_chip_ams_as5055a_master_reset(hl_chip_ams_as5055a_channel_t chn)
{
    const uint32_t tick = 1;
    uint16_t tmpvalues[hl_chip_ams_as5055a_max_chips_in_channel] = {0};
    hl_chip_ams_as5055a_internal_item_t *intitem = s_hl_chip_ams_as5055a_theinternals.items[HL_chip_channel2index(chn)];
    hl_spi_t spiid = intitem->config.spiid;    
    uint8_t nchained = intitem->config.numberofchained;
    uint8_t i = 0;
    
    // enable spi
    hl_spi_enable(spiid);
    
    // step 1: start
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valRESET);
//    hl_sys_delay(tick); 
    
    for(i=0; i<nchained; i++)
    {
        hl_spi_send_receive_raw(spiid, s_Command_WRITE_MasterReset, &tmpvalues[hl_chip_ams_as5055a_max_chips_in_channel-1 -i]);
    }
    
    hl_spi_wait_until_completion(spiid);
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valSET);
    
    // wait either wait for the pin in intitem->config.nint.gpio to become high or ... wait for the maximum time of 500 usec.
    
    hl_sys_delay(s_Treadout);
        
    // disable spi
    hl_spi_disable(spiid);        

    return(hl_res_NOK_generic);  
}


static hl_result_t s_hl_chip_ams_as5055a_clearerrorflag(hl_chip_ams_as5055a_channel_t chn)
{
    const uint32_t tick = 1;
    uint16_t tmpvalues[hl_chip_ams_as5055a_max_chips_in_channel] = {0};
    hl_chip_ams_as5055a_internal_item_t *intitem = s_hl_chip_ams_as5055a_theinternals.items[HL_chip_channel2index(chn)];
    hl_spi_t spiid = intitem->config.spiid;
    uint8_t nchained = intitem->config.numberofchained;
    uint8_t i = 0;


    // enable spi
    hl_spi_enable(spiid);
    
    // step 1: start    
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valRESET);
//    hl_sys_delay(tick); 

    for(i=0; i<nchained; i++)
    {
        hl_spi_send_receive_raw(spiid, s_Command_READ_ClearErrorFlag, &tmpvalues[hl_chip_ams_as5055a_max_chips_in_channel-1-i]);
//        hl_sys_delay(tick);
    }
    
    hl_spi_wait_until_completion(spiid);
    
//    hl_sys_delay(tick);
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valSET);

    
    // step 2: wait     either wait for the pin in intitem->config.nint.gpio to become high or ... wait for the maximum time of 500 usec.
    
    hl_sys_delay(s_Treadout);
    
    
    // step 3: read
    // marco.accame: instead of s_Command_READ_AngularData we may send the null command 0x0000 .... maybe it work better because it does not triggers new readings ..
    
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valRESET);
//    hl_sys_delay(tick);
    
    for(i=0; i<nchained; i++)
    {
        hl_spi_send_receive_raw(spiid, s_Command_NOP, &intitem->values[hl_chip_ams_as5055a_max_chips_in_channel-1-i]);
//        hl_sys_delay(2*s_Treadout);
    }
       
    hl_spi_wait_until_completion(spiid);
     
//    hl_sys_delay(tick);
    hl_gpio_pin_write(intitem->config.nsel, hl_gpio_valSET);    
    
    
    // disable spi
    hl_spi_disable(spiid);     
    

    return(hl_res_NOK_generic);         
}


#endif//defined(HL_USE_CHIP_AMS_AS5055A)


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



