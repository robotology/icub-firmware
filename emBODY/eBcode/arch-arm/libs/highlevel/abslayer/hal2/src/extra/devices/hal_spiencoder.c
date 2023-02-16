/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       hal_spiencoder.c
    @brief      This file implements internal implementation of the hal encoder module.
    @author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       02/07/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
//#include "hal_middleware_interface.h"

#ifdef HAL_USE_SPIENCODER
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_gpio.h"
#include "hl_bits.h" 
#include "hal_heap.h"
#include "hal_sys.h"


#include "hl_chip_ams_as5055a.h"

#include "hal_spi_hid.h" // to see hal_spi__theboardconfig
#include "hal_mux_hid.h" // to see hal_mux__theboardconfig

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_spiencoder.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_spiencoder_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_encoder_id2index(t)              ((uint8_t)((t)))

#define HAL_SPIENCODER_xCHAINED_USE_RAWMODE

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_spiencoder_cfg_t hal_spiencoder_cfg_default =
{ 
    .priority =         hal_int_priority15, 
    .callback_on_rx =   NULL,
    .arg =              NULL, 
    .type =             hal_spiencoder_typeNONE, 
    .sdata_precheck =   hal_false,
    .reg_addresses =    {0, 0} 
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_spiencoder_cfg_t        config;
    hal_spiencoder_t            encid;
    hal_mux_t                   muxid;
    hal_gpio_t                  chip_sel;
    hal_mux_sel_t               muxsel;
    hal_spi_t                   spiid;
    hal_spiencoder_position_t   position;
    uint8_t                     rxframes[3][6];     // 3 possible frames received. The size of everyone is the maximum possible
    uint16_t                    rxframechain[3];    // 1 frame of 2 words of 16 bits
    hl_chip_ams_as5055a_channel_t chainchannel;
    volatile uint8_t            amo_regaddr;
    volatile uint8_t            counter;
    uint16_t                    multiturncounter;
    uint8_t                     status_bits;
    uint8_t                     crc;
} hal_spiencoder_internal_item_t;


typedef struct
{
    uint16_t                    multiturncounter;
    hal_spiencoder_position_t   position;
    uint8_t                     status_bits;
    uint8_t                     crc;
} hal_spiencoder_aksim2_data;

typedef struct
{
    uint32_t                                inittedmask;
    hal_spiencoder_internal_item_t*         items[hal_spiencoders_number];   
} hal_spiencoder_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_spiencoder_supported_is(hal_spiencoder_t id);
static void s_hal_spiencoder_initted_set(hal_spiencoder_t id);
static void s_hal_spiencoder_initted_reset(hal_spiencoder_t id);
static hal_boolval_t s_hal_spiencoder_initted_is(hal_spiencoder_t id);

static hal_result_t s_hal_spiencoder_read_sdad_status_t2(hal_spiencoder_t id);
static hal_result_t s_hal_spiencoder_read_sensor_t2(hal_spiencoder_t id);
static hal_result_t s_hal_spiencoder_read_register_init_t2(hal_spiencoder_t id);
static hal_result_t s_hal_spiencoder_read_register_execute_t2(hal_spiencoder_t id);

//Static callback functions
static void s_hal_spiencoder_onreceiv(void* p);
#if !defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)
static void s_hal_spiencoder_onreceived_daisychain_prepare(void* p);
static void s_hal_spiencoder_onreceived_daisychain(void* p);
static void s_hal_spiencoder_2chained_askvalues(hal_spiencoder_internal_item_t* intitem, hal_callback_t callback, void* arg, uint8_t *txframe, uint8_t sizeofframe);
#endif

static void s_hal_spiencoder_onreceiv_sdad_status(void* p);
static void s_hal_spiencoder_onreceiv_sensor_data(void* p);
static void s_hal_spiencoder_onreceiv_reg_init(void* p);
static void s_hal_spiencoder_onreceiv_reg_data(void* p);

static hal_spiencoder_position_t s_hal_spiencoder_frame2position_t1(uint8_t* frame);
static hal_spiencoder_position_t s_hal_spiencoder_frame2position_t2(uint8_t* frame);
#if defined(AEA3_SUPPORT)
static hal_spiencoder_position_t s_hal_spiencoder_frame2position_t4(uint8_t* frame);
#endif
static void s_hal_spiencoder_parse_data_aksim2(uint8_t* frame, hal_spiencoder_aksim2_data* data);

static uint8_t CRC_SPI_97_64bit(uint64_t dw_InputData);






// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const hal_spi_cfg_t s_hal_spiencoder_spicfg_master =
{
    .ownership                  = hal_spi_ownership_master,
    .direction                  = hal_spi_dir_rxonly,
    .activity                   = hal_spi_act_framebased,
    .prescaler                  = hal_spi_prescaler_auto,
    .maxspeed                   = 1000*1000,   
    .datasize                   = hal_spi_datasize_8bit,
    .maxsizeofframe             = 3, // 3 is for aea.
    .capacityoftxfifoofframes   = 0,
    .capacityofrxfifoofframes   = 1,
    .onframestransmitted        = NULL,
    .argonframestransmitted     = NULL,
    .onframesreceived           = NULL,
    .argonframesreceived        = NULL,
    .cpolarity                  = hal_spi_cpolarity_high,
    .datacapture                = hal_spi_datacapture_1edge,
    .gpio_cfg_flags             = hal_spi_gpio_cfg_sckmosi_pullup
};

//Lookup table for polynome = 0x97
static const uint8_t ab_CRC8_LUT[256] = {
    0x00, 0x97, 0xB9, 0x2E, 0xE5, 0x72, 0x5C, 0xCB, 0x5D, 0xCA, 0xE4, 0x73, 0xB8, 0x2F, 0x01, 0x96,
    0xBA, 0x2D, 0x03, 0x94, 0x5F, 0xC8, 0xE6, 0x71, 0xE7, 0x70, 0x5E, 0xC9, 0x02, 0x95, 0xBB, 0x2C,
    0xE3, 0x74, 0x5A, 0xCD, 0x06, 0x91, 0xBF, 0x28, 0xBE, 0x29, 0x07, 0x90, 0x5B, 0xCC, 0xE2, 0x75, 
    0x59, 0xCE, 0xE0, 0x77, 0xBC, 0x2B, 0x05, 0x92, 0x04, 0x93, 0xBD, 0x2A, 0xE1, 0x76, 0x58, 0xCF, 
    0x51, 0xC6, 0xE8, 0x7F, 0xB4, 0x23, 0x0D, 0x9A, 0x0C, 0x9B, 0xB5, 0x22, 0xE9, 0x7E, 0x50, 0xC7, 
    0xEB, 0x7C, 0x52, 0xC5, 0x0E, 0x99, 0xB7, 0x20, 0xB6, 0x21, 0x0F, 0x98, 0x53, 0xC4, 0xEA, 0x7D, 
    0xB2, 0x25, 0x0B, 0x9C, 0x57, 0xC0, 0xEE, 0x79, 0xEF, 0x78, 0x56, 0xC1, 0x0A, 0x9D, 0xB3, 0x24, 
    0x08, 0x9F, 0xB1, 0x26, 0xED, 0x7A, 0x54, 0xC3, 0x55, 0xC2, 0xEC, 0x7B, 0xB0, 0x27, 0x09, 0x9E, 
    0xA2, 0x35, 0x1B, 0x8C, 0x47, 0xD0, 0xFE, 0x69, 0xFF, 0x68, 0x46, 0xD1, 0x1A, 0x8D, 0xA3, 0x34, 
    0x18, 0x8F, 0xA1, 0x36, 0xFD, 0x6A, 0x44, 0xD3, 0x45, 0xD2, 0xFC, 0x6B, 0xA0, 0x37, 0x19, 0x8E, 
    0x41, 0xD6, 0xF8, 0x6F, 0xA4, 0x33, 0x1D, 0x8A, 0x1C, 0x8B, 0xA5, 0x32, 0xF9, 0x6E, 0x40, 0xD7,
    0xFB, 0x6C, 0x42, 0xD5, 0x1E, 0x89, 0xA7, 0x30, 0xA6, 0x31, 0x1F, 0x88, 0x43, 0xD4, 0xFA, 0x6D, 
    0xF3, 0x64, 0x4A, 0xDD, 0x16, 0x81, 0xAF, 0x38, 0xAE, 0x39, 0x17, 0x80, 0x4B, 0xDC, 0xF2, 0x65, 
    0x49, 0xDE, 0xF0, 0x67, 0xAC, 0x3B, 0x15, 0x82, 0x14, 0x83, 0xAD, 0x3A, 0xF1, 0x66, 0x48, 0xDF, 
    0x10, 0x87, 0xA9, 0x3E, 0xF5, 0x62, 0x4C, 0xDB, 0x4D, 0xDA, 0xF4, 0x63, 0xA8, 0x3F, 0x11, 0x86, 
    0xAA, 0x3D, 0x13, 0x84, 0x4F, 0xD8, 0xF6, 0x61, 0xF7, 0x60, 0x4E, 0xD9, 0x12, 0x85, 0xAB, 0x3C};

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_spiencoder_theinternals_t s_hal_spiencoder_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_boolval_t hal_spiencoder_supported_is(hal_spiencoder_t id)
{
    return(s_hal_spiencoder_supported_is(id));
}

extern hal_boolval_t hal_spiencoder_initted_is(hal_spiencoder_t id)
{
    return(s_hal_spiencoder_initted_is(id));
}


extern const hal_spiencoder_stream_map_t* hal_spiencoder_stream_map_get(void)
{
    return(&hal_spiencoder__theboardconfig.streammap);
}


extern hal_result_t hal_spiencoder_init(hal_spiencoder_t id, const hal_spiencoder_cfg_t *cfg)
{
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    hal_result_t res = hal_res_NOK_generic;

    // If encoder not supported return not OK
    if(hal_false == s_hal_spiencoder_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    // Set the default config if cfg is NULL
    if(NULL == cfg)
    {
        cfg = &hal_spiencoder_cfg_default;
    }
    
    // Check initialization
    if(hal_true == s_hal_spiencoder_initted_is(id))
    {
        //Memory comparison
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_spiencoder_cfg_t)))
        {   // ok only if the previously used config is the same as the current one
            return(hal_res_OK);
        }
        else
        {
            return(hal_res_NOK_generic);
        }
    }   
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)] = hal_heap_new(sizeof(hal_spiencoder_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }
    // Copy configuration in a protected memory
    memcpy(&intitem->config, cfg, sizeof(hal_spiencoder_cfg_t));   
    intitem->counter = 0; 
    
    // configure the required mux port and spi port (using the configuration of the board for the selected encoder)
    // "hal_spiencoder__theboardconfig" struct is defined inside the board modules 
    intitem->encid      = id;
    intitem->spiid      = hal_spiencoder__theboardconfig.spimap[HAL_encoder_id2index(id)].spiid;    
    intitem->muxid      = hal_spiencoder__theboardconfig.spimap[HAL_encoder_id2index(id)].muxid;
    intitem->muxsel     = hal_spiencoder__theboardconfig.spimap[HAL_encoder_id2index(id)].muxsel;
    intitem->position   = 0;
    
    hal_bool_t initMUX = hal_true;
    
#if defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)
    if((hal_spiencoder_typeCHAINof2 == intitem->config.type) || (hal_spiencoder_typeCHAINof3 == intitem->config.type))
    {
        initMUX = hal_false;
    }
#endif    
    
    if(hal_true == initMUX)
    {
        res = hal_mux_init(intitem->muxid, NULL);
        if(hal_res_OK != res)
        {
            return(res);
        }
        hal_mux_get_cs(intitem->muxid, &(intitem->chip_sel));  // per i rawmodes 0 ed 1 il cs non serve
    }
    else
    {
        intitem->chip_sel = hal_mux__theboardconfig.gpiomap[intitem->muxid].gpio_enable.gpio;        
    }
      
       
    hal_bool_t initSPI = hal_true;    
    hal_spi_cfg_t spicfg;
    memcpy(&spicfg, &s_hal_spiencoder_spicfg_master, sizeof(hal_spi_cfg_t));
    
    // we get the max speed of spi from what specified in hal_spiencoder__theboardconfig
    spicfg.maxspeed = hal_spiencoder__theboardconfig.spimaxspeed;
    
        
    if(hal_spiencoder_typeAMO == intitem->config.type)
    {
        // We use the master SPI configuration for encoder type 2 (AMO)
        // This sizeofframe is used as an upper bound for reserving heap memory during the initialization (hal_spi level) 
        spicfg.maxsizeofframe = 4;
        spicfg.cpolarity = hal_spi_cpolarity_low;
        
        initSPI = hal_true;
    }
    else if(hal_spiencoder_typeAEA == intitem->config.type)
    {
        // no change from default
        initSPI = hal_true;        
    }
#if defined(AEA3_SUPPORT)
    else if(hal_spiencoder_typeAEA3 == intitem->config.type) 
    {
        // 14 MSB + 2 LSB_zero_padded --> 2 Byte
        spicfg.cpolarity = hal_spi_cpolarity_low;
        spicfg.datacapture = hal_spi_datacapture_2edge;
        spicfg.gpio_cfg_flags = hal_spi_gpio_cfg_sckmosi_pulldown;
        spicfg.maxsizeofframe = 2;                  // each frame is done of two words (TODO: probably 1 is better)
        spicfg.datasize = hal_spi_datasize_16bit;   // and each word is of 16 bits
        
        initSPI = hal_true;
    }
#endif
    else if(hal_spiencoder_typeAksIM2 == intitem->config.type)
    {
        // 19 MSB + 3 LSB_zero_padded --> 2 Byte
        spicfg.cpolarity = hal_spi_cpolarity_low;
        spicfg.datacapture = hal_spi_datacapture_2edge;
        spicfg.gpio_cfg_flags = hal_spi_gpio_cfg_sckmosi_pulldown;
        spicfg.maxsizeofframe = 6;
        spicfg.datasize = hal_spi_datasize_8bit;
        
        initSPI = hal_true;
    }
    else if(hal_spiencoder_typeCHAINof2 == intitem->config.type)
    {     
        spicfg.capacityofrxfifoofframes = 1;        // we need to manage only one frame at a time
        spicfg.maxsizeofframe = 2;                  // each frame is done of two words
        spicfg.datasize = hal_spi_datasize_16bit;   // and each word is of 16 bits
                                                    // all the rest is equal to default 
        spicfg.cpolarity = hal_spi_cpolarity_low;
        spicfg.datacapture = hal_spi_datacapture_2edge;    

        initSPI = hal_true;
#if defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)
        initSPI = hal_false;
#endif        
    }
    else if(hal_spiencoder_typeCHAINof3 == intitem->config.type)
    {     
        spicfg.capacityofrxfifoofframes = 1;        // we need to manage only one frame at a time
        spicfg.maxsizeofframe = 3;                  // each frame is done of three words
        spicfg.datasize = hal_spi_datasize_16bit;   // and each word is of 16 bits
                                                    // all the rest is equal to default 
        spicfg.cpolarity = hal_spi_cpolarity_low;
        spicfg.datacapture = hal_spi_datacapture_2edge;    

        initSPI = hal_true;
#if defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)
        initSPI = hal_false;
#endif        
    }
    
    
    if(hal_true == initSPI)
    {    
        // Initialize the SPI with the correct config
        res = hal_spi_init(intitem->spiid, &spicfg);
        if(hal_res_OK != res)
        {
            return(res);
        }
    }

#if defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)
    
    if(hal_spiencoder_typeCHAINof2 == intitem->config.type)
    {  
        // need to init the spi inside the chip ...
        
        static hl_chip_ams_as5055a_spicfg_t as5055a_spicfg =
        {
            .spimap2use = (hl_spi_mapping_t*)&hal_spi__theboardconfig,
            .prescaler = hl_spi_prescaler_064        
        };

        hl_chip_ams_as5055a_cfg_t as5055a_config = 
        {
            .spiid              = hl_spi1,
            .numberofchained    = 2,
            .initthegpios       = hl_true,
            .nsel               = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE },
            .nint               = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE },              
            .spicfg             = &as5055a_spicfg, 
        };        
                
        hl_chip_ams_as5055a_channel_t chn = (hal_spi2 == intitem->spiid) ? (hl_chip_ams_as5055a_channel2) : (hl_chip_ams_as5055a_channel1);
        as5055a_config.spiid = (hl_spi_t) intitem->spiid;
        as5055a_config.nsel.port = (hl_gpio_port_t)intitem->chip_sel.port;
        as5055a_config.nsel.pin = (hl_gpio_pin_t)intitem->chip_sel.pin;

        hl_chip_ams_as5055a_init(chn, &as5055a_config);  

        intitem->chainchannel = chn;       
    }
    else if(hal_spiencoder_typeCHAINof3 == intitem->config.type)
    {  
        // need to init the spi inside the chip ...
        
        static hl_chip_ams_as5055a_spicfg_t as5055a_spicfg =
        {
            .spimap2use = (hl_spi_mapping_t*)&hal_spi__theboardconfig,
            .prescaler = hl_spi_prescaler_064        
        };

        hl_chip_ams_as5055a_cfg_t as5055a_config = 
        {
            .spiid              = hl_spi1,
            .numberofchained    = 3,
            .initthegpios       = hl_true,
            .nsel               = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE },
            .nint               = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE },              
            .spicfg             = &as5055a_spicfg, 
        };        
                
        hl_chip_ams_as5055a_channel_t chn = (hal_spi2 == intitem->spiid) ? (hl_chip_ams_as5055a_channel2) : (hl_chip_ams_as5055a_channel1);
        as5055a_config.spiid = (hl_spi_t) intitem->spiid;
        as5055a_config.nsel.port = (hl_gpio_port_t)intitem->chip_sel.port;
        as5055a_config.nsel.pin = (hl_gpio_pin_t)intitem->chip_sel.pin;

        hl_chip_ams_as5055a_init(chn, &as5055a_config);  

        intitem->chainchannel = chn;       
    }
#else    
       
    if(hal_spiencoder_typeCHAINof2 == intitem->config.type)
    {  
        // need to init the chip ...
        hl_chip_ams_as5055a_cfg_t as5055a_config =
        {
            .spiid              = hl_spi2,
            .numberofchained    = 2,
            .initthegpios       = hl_false,     // the nsel is already initted by the mux (it is the chip_sel ...
            .nsel               = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE }, // get the cs of the mux
            .nint               = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE },
            .spicfg             = NULL,         // the spi initialisation is done externally to the chip (just a few lines above actually)...
        };
        hl_chip_ams_as5055a_channel_t chn = (hal_spi2 == intitem->spiid) ? (hl_chip_ams_as5055a_channel2) : (hl_chip_ams_as5055a_channel1);
        as5055a_config.spiid = (hl_spi_t) intitem->spiid;
        as5055a_config.nsel.port = (hl_gpio_port_t)intitem->chip_sel.port;
        as5055a_config.nsel.pin = (hl_gpio_pin_t)intitem->chip_sel.pin;

        hl_chip_ams_as5055a_init(chn, &as5055a_config);  

        intitem->chainchannel = chn;         
            
    }  
    #error  marco.accame: hal_spiencoder_typeCHAINof3 only supports macro HAL_SPIENCODER_xCHAINED_USE_RAWMODE enabled
#endif    
 
    // Flag to set the encoder initialized
    s_hal_spiencoder_initted_set(id);
    return(hal_res_OK);
}

// Start reading for the encoders
extern hal_result_t hal_spiencoder_read_start(hal_spiencoder_t id)
{  
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_spiencoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    
    if(intitem->config.type == hal_spiencoder_typeAEA) // Encoder type 1 (AEA)
    {
        static const uint8_t txframe_dummy[3] = {0x00, 0x00, 0x00};
        // Mux enabled
        hal_mux_enable(intitem->muxid, intitem->muxsel);

        // SPI: set the callback function
        hal_spi_on_framesreceived_set(intitem->spiid, s_hal_spiencoder_onreceiv, (void*)id);

        //Added 26/11/2014
        //----------------
        // SPI: set the sizeofframe for this transmission (3)
        hal_spi_set_sizeofframe(intitem->spiid, 3);

        // SPI: set the isrtxframe
        hal_spi_set_isrtxframe(intitem->spiid, txframe_dummy);
        //----------------

        // SPI start to receive (only one frame)
        hal_spi_start(intitem->spiid, 1); // 1 solo frame ...

        // when the frame is received, then the isr will call s_hal_spiencoder_onreceiv() to copy the frame into local memory,
        // so that hal_spiencoder_get_value() can be called to retrieve the encoder value
    }
#if defined(AEA3_SUPPORT)
    else if(intitem->config.type == hal_spiencoder_typeAEA3) // Encoder type 4 (AEA3)
    {        
        static const uint8_t txframe_dummy_aea3[2] = {0x00,0x00};
        // Mux enabled
        hal_mux_enable(intitem->muxid, intitem->muxsel);
        
        // SPI: set the callback function
        hal_spi_on_framesreceived_set(intitem->spiid, s_hal_spiencoder_onreceiv, (void*)id);
        
        // SPI: set the sizeofframe for this transmission
        hal_spi_set_sizeofframe(intitem->spiid, 1);
        
        // SPI: set the isrtxframe
        hal_spi_set_isrtxframe(intitem->spiid, txframe_dummy_aea3);
        //----------------

        // SPI start to receive (only one frame)
        hal_spi_start(intitem->spiid, 1);
        
        // when the frame is received, then the isr will call s_hal_spiencoder_onreceiv() to copy the frame into local memory,
        // so that hal_spiencoder_get_value() can be called to retrieve the encoder value
    }
#endif
    else if(intitem->config.type == hal_spiencoder_typeAksIM2)
    {
        static const uint8_t txframe_dummy_aksim2[6] = {0x00};
        // Mux enabled
        hal_mux_enable(intitem->muxid, intitem->muxsel);
        
        // SPI: set the callback function
        hal_spi_on_framesreceived_set(intitem->spiid, s_hal_spiencoder_onreceiv, (void*)id);
        
        // SPI: set the sizeofframe for this transmission (48 bits --> 6 Bytes for AksIM2)
        hal_spi_set_sizeofframe(intitem->spiid, 6);
        
        // SPI: set the isrtxframe
        hal_spi_set_isrtxframe(intitem->spiid, txframe_dummy_aksim2);
        
        // SPI start to receive (only one frame)
        hal_spi_start(intitem->spiid, 1);
    }
    else if(intitem->config.type == hal_spiencoder_typeAMO)    // Encoder type 2 (AMO)
    {        
        // Enabling the MUX
        hal_mux_enable(intitem->muxid, intitem->muxsel);

        
        if(hal_true == intitem->config.sdata_precheck)
        {   // Precheck needed
            s_hal_spiencoder_read_sdad_status_t2 (id);
        }        
        else    
        {   // Precheck not needed
            intitem->rxframes[0][0] = 0xF5;
            intitem->rxframes[0][1] = UINT8_MAX;
            s_hal_spiencoder_read_sensor_t2 (id);
        }
    }
    else if(hal_spiencoder_typeCHAINof2 == intitem->config.type)
    {
#if defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)

    // following is OK, but i wnat to try the reaidng in two steps to avoid 600 usec delay
    //    hl_chip_ams_as5055a_read_angulardata(intitem->chainchannel, hl_chip_ams_as5055a_readmode_start_wait_read, &intitem->rxframechain[0], &intitem->rxframechain[1], NULL);
    //        
    //    intitem->position = intitem->rxframechain[0] | (intitem->rxframechain[1] << 16);
    //    
    //    if(NULL != intitem->config.callback_on_rx)
    //    {
    //        intitem->config.callback_on_rx(intitem->config.arg);
    //    } 
    //
            
        hl_chip_ams_as5055a_read_angulardata(intitem->chainchannel, hl_chip_ams_as5055a_readmode_start_only, NULL, NULL, NULL);      
            
        if(NULL != intitem->config.callback_on_rx)
        {
            intitem->config.callback_on_rx(intitem->config.arg);
        }         

#else        
        // only in mc2plus ...
        static const uint16_t txframe_as5055a_angulardata[3] = {0xFFFF, 0xFFFF, 0xFFFF}; // it is ok also for teh cse of chainof3
        s_hal_spiencoder_2chained_askvalues(intitem, s_hal_spiencoder_onreceived_daisychain_prepare, (void*)id, (uint8_t*)txframe_as5055a_angulardata, 2);    
#endif        
    }
    else if(hal_spiencoder_typeCHAINof3 == intitem->config.type)
    {
#if defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)

    // following is OK, but i wnat to try the reaidng in two steps to avoid 600 usec delay
    //    hl_chip_ams_as5055a_read_angulardata(intitem->chainchannel, hl_chip_ams_as5055a_readmode_start_wait_read, &intitem->rxframechain[0], &intitem->rxframechain[1], NULL);
    //        
    //    intitem->position = intitem->rxframechain[0] | (intitem->rxframechain[1] << 16);
    //    
    //    if(NULL != intitem->config.callback_on_rx)
    //    {
    //        intitem->config.callback_on_rx(intitem->config.arg);
    //    } 
    //
            
        hl_chip_ams_as5055a_read_angulardata(intitem->chainchannel, hl_chip_ams_as5055a_readmode_start_only, NULL, NULL, NULL);      
            
        if(NULL != intitem->config.callback_on_rx)
        {
            intitem->config.callback_on_rx(intitem->config.arg);
        }         

#else        
        // only in mc2plus ...
        static const uint16_t txframe_as5055a_angulardata[3] = {0xFFFF, 0xFFFF, 0xFFFF};
        s_hal_spiencoder_2chained_askvalues(intitem, s_hal_spiencoder_onreceived_daisychain_prepare, (void*)id, (uint8_t*)txframe_as5055a_angulardata, 3);    
#endif        
    }    
    
    return(hal_res_OK);
}

//Start reading for encoder t2
extern hal_result_t hal_spiencoder_read_start_t2(hal_spiencoder_t id, uint8_t reg_address, hal_bool_t sdata_check)
{
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];   
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_spiencoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if (intitem->config.type != hal_spiencoder_typeAMO)
    {
        return (hal_res_NOK_generic);
    }
#endif
    // Saving the status register address to be read in intitem
    //intitem->act_reg_address = reg_address;
    
    //Enabling the MUX
    hal_mux_enable(intitem->muxid, intitem->muxsel);
    
    // Check if we can skip the first phase or not
    if(hal_true == sdata_check)
    {
        // If a check is needed, start with the first phase
        // This launch a chain of execution for performing a complete reading of the sensor data
        s_hal_spiencoder_read_sdad_status_t2 (id);
    }
    else
    {
        // Check not needed, we can start from the reading of the sensor
        intitem->rxframes[0][0] = 0xF5;
        intitem->rxframes[0][1] = UINT8_MAX;
        s_hal_spiencoder_read_sensor_t2 (id);
    }
    return(hal_res_OK);
}

static uint8_t CRC_SPI_97_64bit(uint64_t dw_InputData)
{
     uint8_t b_Index = 0;
     uint8_t b_CRC = 0;
     b_Index = (uint8_t)((dw_InputData >> 56u) & (uint64_t)0x000000FFu);
     b_CRC = (uint8_t)((dw_InputData >> 48u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 40u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 32u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 24u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 16u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 8u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)(dw_InputData & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = ab_CRC8_LUT[b_Index];
     
     return b_CRC; 
}


extern hal_result_t hal_spiencoder_get_value2(hal_spiencoder_t id, hal_spiencoder_position_t* pos, hal_spiencoder_diagnostic_t* diagn)
{
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_spiencoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
        
#if !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)    
    if((NULL == pos) || (NULL == diagn))
    {
        return(hal_res_NOK_nullpointer);
    }
#endif
    
    // for now we clear the diagnostic type and info. then, depending on the type of encoder we fill something
    diagn->type = hal_spiencoder_diagnostic_type_none;
    diagn->info.value = 0;
    
    //Encoder type 1 (AEA)
    if (intitem->config.type == hal_spiencoder_typeAEA)
    {
        diagn->type = hal_spiencoder_diagnostic_type_none; 
        // for the AEA we use legacy diagnostic mode w/ flags even if it is not used by higher layers
        
        // Check if SPI connection is working (if not, is answering 0xFF in all the received bytes)
        // Here I check the bits which should validate the data. In fact, AEA frame consists of 18 bits:
        // - first 12 MSB bits of positional data
        // - last 6 LSB bits of validity checks
        // In particular I check OCF, COF and LIN, which must have particular values (1,0,0) to validate the frame
        // Attention: since the SPI polarity is High, the message is shifted to the right of one bit (first discarded)
        if((intitem->rxframes[1][1] & 0x07) != 0x04)
        {
            //Everything 0xFF, SPI is not working
            if (((intitem->rxframes[1][0]) == 0xFF) && ((intitem->rxframes[1][1]) == 0xFF) && ((intitem->rxframes[1][2]) == 0xFF))
            { 
                diagn->type = hal_spiencoder_diagnostic_type_flags;                
                diagn->info.flags.tx_error = 1;
                return(hal_res_NOK_generic);
            }
            if (((intitem->rxframes[1][1] & 0x01) != 0x00) || ((intitem->rxframes[1][1] & 0x02) != 0x00))
            {
                diagn->type = hal_spiencoder_diagnostic_type_flags;
                diagn->info.flags.data_error = 1;
            }
            if ((intitem->rxframes[1][1] & 0x04) != 0x04)
            {
                diagn->type = hal_spiencoder_diagnostic_type_flags;
                diagn->info.flags.data_notready = 1;
            }
        }
        
        *pos = intitem->position;
    }
#if defined(AEA3_SUPPORT)
    else if (intitem->config.type == hal_spiencoder_typeAEA3)
    {
        diagn->type = hal_spiencoder_diagnostic_type_none;
        
        // AEA3 frame consists of 16 bits:
        // - first 14 MSB bits of positional data
        // - last 2 LSB bits zero padded
        
        //Everything 0xFF, SPI is not working
        if (((intitem->rxframes[1][0]) == 0xFF) && ((intitem->rxframes[1][1]) == 0xFF))
        { 
            diagn->type = hal_spiencoder_diagnostic_type_flags;
            diagn->info.flags.tx_error = 1;
            return(hal_res_NOK_generic);
        }
        
        // Check for zero padded bits (This could be a rendundant check, beacuse it already exists in EOappEncoreReader)
        // Simone Girardi 28/01/22 after some tests the zero padded bit is not reliable due to the (noise) flickering 
        //if ((intitem->rxframes[1][0] & 0x01) != 0x00)
        //{
        //    diagn->type = hal_spiencoder_diagnostic_type_flags;
        //    diagn->info.flags.data_error = 1;
        //}

        *pos = intitem->position;
    }
#endif
    else if (intitem->config.type == hal_spiencoder_typeAksIM2)
    {
        diagn->type = hal_spiencoder_diagnostic_type_none;
        
        // aksim2_status_crc description, they are 3 bits:
        // 100 --> the position data is not valid
        // 010 --> the position data is valid, but some operating conditions are close to limits.
        // 001 --> Inverted CRC is invalid
        diagn->info.aksim2_status_crc = 0;
        
        // check status bits
        if(0x0 == intitem->status_bits || 0x1 == intitem->status_bits)
        {
            // Error - the position data is not valid
            diagn->type = hal_spiencoder_diagnostic_type_aksim2_invalid_data;
            diagn->info.aksim2_status_crc |= 0x04;
        }    
        if(0x2 == intitem->status_bits)
        {
            // Warning - the position data is valid, but some operating conditions are close to limits
            diagn->type = hal_spiencoder_diagnostic_type_aksim2_close_to_limits;
            diagn->info.aksim2_status_crc |= 0x02;
        }
        
        // check for CRC errors
        uint64_t dw_CRCinputData = 0;
        uint8_t calculated_crc = 0;
        
        dw_CRCinputData = ((uint64_t)intitem->rxframes[1][0] << 32) + ((uint64_t)intitem->rxframes[1][1] << 24) +
                          ((uint64_t)intitem->rxframes[1][2] << 16) + ((uint64_t)intitem->rxframes[1][3] << 8) +
                          ((uint64_t)intitem->rxframes[1][4]);
        
        calculated_crc = ~(CRC_SPI_97_64bit(dw_CRCinputData))& 0xFF; //inverted CRC
        
        if(calculated_crc != intitem->crc)
        {
            // Error - invalid crc
            diagn->type = hal_spiencoder_diagnostic_type_aksim2_crc_error;
            diagn->info.aksim2_status_crc |= 0x01;
        }
        
        
        // Check for SPI reading errors: if all data are FF then the encoder is not connected or the SPI is not working.
        if (intitem->multiturncounter == 0xFFFF && intitem->position == 0x7FFFF && intitem->status_bits == 0x03 && intitem->crc == 0xFF)
        {
            // TODO: check if it can be manage it, or remove everything berfore the return because these diagnostic is not currently used.
            diagn->type = hal_spiencoder_diagnostic_type_aksim2_not_connected;
            diagn->info.value = 0;
            return hal_res_NOK_generic;
        }
        
        *pos = intitem->position;
    }
    else if (intitem->config.type == hal_spiencoder_typeAMO)
    {
        diagn->type = hal_spiencoder_diagnostic_type_none;
        // for the AMO we use new diagnostic mode
        
        // Check if SPI connection is working
        // Here I check only the first bytes received for all the communications, which should be = to the different
        // opcode used
        if((intitem->rxframes[0][0] != 0xF5) || (intitem->rxframes[1][0] != 0xA6) || (intitem->rxframes[2][0] != 0xAD))
        {
            if (((intitem->rxframes[1][0]) == 0xFF) && ((intitem->rxframes[1][1]) == 0xFF) && ((intitem->rxframes[1][2]) == 0xFF) && ((intitem->rxframes[1][3]) == 0xFF))
            {
                diagn->type = hal_spiencoder_diagnostic_type_amo_notconn;
                diagn->info.value = 0;
                return(hal_res_NOK_generic);    
            }
        }
        // check the value of the status register: 
        if(0x76 == intitem->amo_regaddr)
        {   
            // STATUS0
            // datasheeet (Rev F1, Page 58/69) shows only bits [4, 0], so i mask them 
            uint8_t regval = 0xFF & intitem->rxframes[2][2];
            // if 0 -> no errors
            if(0 != regval)
            {
                diagn->type = hal_spiencoder_diagnostic_type_amo_status0;
                diagn->info.value = regval;                
            }            
        }
        else if(0x77 == intitem->amo_regaddr)
        {
            // STATUS1
            // datasheeet (Rev F1, Page 58/69) shows the 8 bits are used 
            uint8_t regval = 0xFF & intitem->rxframes[2][2];
            // if 0 -> no errors
            if(0 != regval)
            {
                diagn->type = hal_spiencoder_diagnostic_type_amo_status1;
                diagn->info.value = regval;                
            }            
        }
        
        *pos = intitem->position;
    }
    else if(hal_spiencoder_typeCHAINof2 == intitem->config.type)
    {
        hl_chip_ams_as5055a_read_angulardata(intitem->chainchannel, hl_chip_ams_as5055a_readmode_read_only, &intitem->rxframechain[0], &intitem->rxframechain[1], NULL);        
        intitem->position = intitem->rxframechain[0] | (intitem->rxframechain[1] << 16);  
        
        *pos = intitem->position; 
    }
    else if(hal_spiencoder_typeCHAINof3 == intitem->config.type)
    {
        hl_chip_ams_as5055a_read_angulardata(intitem->chainchannel, hl_chip_ams_as5055a_readmode_read_only, &intitem->rxframechain[0], &intitem->rxframechain[1], &intitem->rxframechain[2]);        

        
        pos[0] = intitem->rxframechain[0];
        pos[1] = intitem->rxframechain[1]; 
        pos[2] = intitem->rxframechain[2]; 
    }  
    
    return(hal_res_OK);      
}

// Get the last value saved with a read_start
extern hal_result_t hal_spiencoder_get_value(hal_spiencoder_t id, hal_spiencoder_position_t* pos, hal_spiencoder_errors_flags* e_flags)
{
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_spiencoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
        
#if !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)    
    if((NULL == pos) || (NULL == e_flags))
    {
        return(hal_res_NOK_nullpointer);
    }
#endif
    
    //Encoder type 1 (AEA)
    if (intitem->config.type == hal_spiencoder_typeAEA)
    {
        // Check if SPI connection is working (if not, is answering 0xFF in all the received bytes)
        // Here I check the bits which should validate the data. In fact, AEA frame consists of 18 bits:
        // - first 12 MSB bits of positional data
        // - last 6 LSB bits of validity checks
        // In particular I check OCF, COF and LIN, which must have particular values (1,0,0) to validate the frame
        // Attention: since the SPI polarity is High, the message is shifted to the right of one bit (first discarded)
        if((intitem->rxframes[1][1] & 0x07) != 0x04)
        {
            //Everything 0xFF, SPI is not working
            if (((intitem->rxframes[1][0]) == 0xFF) && ((intitem->rxframes[1][1]) == 0xFF) && ((intitem->rxframes[1][2]) == 0xFF))
            {    
                e_flags->tx_error = 1;
                return(hal_res_NOK_generic);
            }
            if (((intitem->rxframes[1][1] & 0x01) != 0x00) || ((intitem->rxframes[1][1] & 0x02) != 0x00))
            {
                e_flags->data_error = 1;
            }
            if ((intitem->rxframes[1][1] & 0x04) != 0x04)
            {
                e_flags->data_notready = 1;
            }
        }
        *pos = intitem->position;
    }
    else if (intitem->config.type == hal_spiencoder_typeAMO)
    {
        // Check if SPI connection is working
        // Here I check only the first bytes received for all the communications, which should be = to the different
        // opcode used
        if((intitem->rxframes[0][0] != 0xF5) || (intitem->rxframes[1][0] != 0xA6) || (intitem->rxframes[2][0] != 0xAD))
        {
            if (((intitem->rxframes[1][0]) == 0xFF) && ((intitem->rxframes[1][1]) == 0xFF) && ((intitem->rxframes[1][2]) == 0xFF) && ((intitem->rxframes[1][3]) == 0xFF))
            {
                e_flags->tx_error = 1;
                return(hal_res_NOK_generic);    
            }
        }
        //Check errors for reg: 0x76
        if ((intitem->amo_regaddr) == 0x76)
        {                
            if(((intitem->rxframes[2][2]) & 0x0F) != 0x00)
            {
                e_flags->data_error = 1;
            }
            if(((intitem->rxframes[2][2]) & 0x10) != 0x00)
            {
                e_flags->data_notready = 1;
            }
        }
        //Check errors for reg: 0x77
        else if ((intitem->amo_regaddr) == 0x77)
        {
            if(((intitem->rxframes[2][2]) & 0x08) != 0x00)
            {
                e_flags->data_error = 1;
            }
            if(((intitem->rxframes[2][2]) & 0xC0) != 0x00)
            {
                e_flags->chip_error = 1;
            }
        }
        *pos = intitem->position;
    }
    else if(hal_spiencoder_typeCHAINof2 == intitem->config.type)
    {
        hl_chip_ams_as5055a_read_angulardata(intitem->chainchannel, hl_chip_ams_as5055a_readmode_read_only, &intitem->rxframechain[0], &intitem->rxframechain[1], NULL);        
        intitem->position = intitem->rxframechain[0] | (intitem->rxframechain[1] << 16);  
        
        *pos = intitem->position; 
    }
    else if(hal_spiencoder_typeCHAINof3 == intitem->config.type)
    {
        hl_chip_ams_as5055a_read_angulardata(intitem->chainchannel, hl_chip_ams_as5055a_readmode_read_only, &intitem->rxframechain[0], &intitem->rxframechain[1], &intitem->rxframechain[2]);        

        
        pos[0] = intitem->rxframechain[0];
        pos[1] = intitem->rxframechain[1]; 
        pos[2] = intitem->rxframechain[2]; 
    }    
    return(hal_res_OK);        
}

// Get the last values saved with a read_start_t2
extern hal_result_t hal_spiencoder_get_value_t2(hal_spiencoder_t id, hal_spiencoder_position_t* pos, hal_bool_t* val, uint16_t* reg)
{
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];

#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_spiencoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
        if((NULL == pos) || (NULL == val) || (NULL == reg))
    {
        return(hal_res_NOK_generic); 
    }
#endif
    // Check if SPI connection is working (if not, is answering 0xFF in all the received bytes)
    // Here I check only the first bytes received for all the communications, which should be = to the different
    // opcode used. If not, I put the validity to false
    if((intitem->rxframes[0][0] != 0xF5) || (intitem->rxframes[1][0] != 0xA6) || (intitem->rxframes[2][0] != 0xAD))
    {
        *val = hal_false;
        return(hal_res_NOK_generic); 
    }
        
    *pos = intitem->position;
        
    // True also if we didn't check...
    if((intitem->rxframes[0][1] == 0x80) || (intitem->rxframes[0][1] == UINT8_MAX))
    {
        *val = hal_true;
    }
    else
    {
        *val = hal_false;
    }

    *reg = (((intitem->rxframes[2][1] << 8 ) & 0xFF00) | (intitem->rxframes[2][2])); 
        
    return(hal_res_OK);
}

//Get the single bytes inside the array rxframes[1] = sensor data
// used only for debugging AMO
extern hal_result_t hal_spiencoder_get_frame(hal_spiencoder_t id, uint8_t* bytes)
{
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];

#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_spiencoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    if((NULL == bytes))
    {
        return(hal_res_NOK_generic); 
    }
#endif
    
    memcpy(bytes, intitem->rxframes[1], 4*sizeof(uint8_t));
    return(hal_res_OK);
}

extern hal_result_t hal_spiencoder_get_spi(hal_spiencoder_t id, hal_spi_t* spiid)
{
    //returns the spiid even if the encoder is not initialized...it's a static information, always valid
 
    if(hal_false == s_hal_spiencoder_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    //if supported, copy the info
    *spiid = hal_spiencoder__theboardconfig.spimap[HAL_encoder_id2index(id)].spiid;        
    return(hal_res_OK);
}

extern hal_result_t hal_spiencoder_deinit(hal_spiencoder_t id)
{
    // Here I should remove everything connected to the specified encoder
    // In this way, I should be able to initialize e deinitialize the encoders programmatically
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
    //If the encoder is not initialized, return and do nothing
    if(hal_false == s_hal_spiencoder_initted_is(id))
        return hal_res_NOK_generic;
    
    //SPI & MUX DEINIT HANDLE
    // I should deinit the SPI and the MUX associated (same id...)
    // only if the other muxsel are not used, cause other encoders could use the same SPI&MUX
    // So:
    // 1 - call a function to detect if the current SPI are used by other encoders
    // 2 - if not used, deinit both SPI & MUX
    // 3 - if used, leave the SPI & MUX config as it is
    uint8_t i = 0;
    hal_result_t res;
    hal_spiencoder_internal_item_t* extitem;

    for (i = 0; i<hal_spiencoders_number; i++)
    {
        extitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(i)];
        if(extitem != NULL)
        {
            //At least one encoder connected to the same muxid, I can go on 
            if((extitem->muxid == intitem->muxid) && (extitem != intitem))
            {
                break;
            }
        }
        if (i == hal_spiencoders_number -1)
        {
            // If I'm here, it means that no encoder is connected to the same muxid == spiid
            // So I must deinit both MUX & SPI, but before it's safer to disable something that may be still running
            hal_spi_rx_isr_disable(intitem->spiid);
            hal_spi_periph_disable(intitem->spiid);
            res = hal_spi_deinit(intitem->spiid);
            if (hal_res_NOK_generic == res)
            {
                return hal_res_NOK_generic;
            }
            
            hal_mux_disable(intitem->muxid);
            res = hal_mux_deinit(intitem->muxid);
            if (hal_res_NOK_generic == res)
            {
                return hal_res_NOK_generic;
            }
        }            
    }
    
    //Reset the flag associated to the encoder
    s_hal_spiencoder_initted_reset(id);

#if !defined(SPIENC_DEINIT_DEALLOCATE_HEAP)
#else    
    //Dealloc all the associated memory
    hal_heap_delete((void**)&(s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)]));
    //hal_heap_delete((void**)&intitem);
#endif

    return(hal_res_OK);
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

static hal_result_t s_hal_spiencoder_read_sdad_status_t2(hal_spiencoder_t id)
{
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    static const uint8_t txframe_sdad[2] = {0xF5, 0x00};
        
    // SPI: set the callback function
    hal_spi_on_framesreceived_set(intitem->spiid, s_hal_spiencoder_onreceiv_sdad_status, (void*)id);
    
    // SPI: set the sizeofframe for this transmission (2)
    hal_spi_set_sizeofframe(intitem->spiid, 2);
    
    // SPI: set the isrtxframe
    hal_spi_set_isrtxframe(intitem->spiid, txframe_sdad);
        
    // SPI start to receive (only one frame)
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
    
    return (hal_res_OK);
}

static hal_result_t s_hal_spiencoder_read_sensor_t2(hal_spiencoder_t id)
{    
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    static const uint8_t txframe_sdata[4] = {0xA6, 0x00, 0x00, 0x00};
    
    //Enable CHIPSELECT
    hal_gpio_setval(intitem->chip_sel, hal_gpio_valLOW);
    
    // SPI: set the callback function
    hal_spi_on_framesreceived_set(intitem->spiid, s_hal_spiencoder_onreceiv_sensor_data, (void*)id);
    
    // SPI: set the sizeofframe for this transmission (4)
    hal_spi_set_sizeofframe(intitem->spiid, 4);
    
    // SPI: set the isrtxframe
    hal_spi_set_isrtxframe(intitem->spiid, txframe_sdata);
        
    // SPI: start to receive (only one frame)
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
    
    return (hal_res_OK);
}


static hal_result_t s_hal_spiencoder_read_register_init_t2(hal_spiencoder_t id)
{
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
    
    intitem->counter ++;
    intitem->amo_regaddr = intitem->config.reg_addresses[intitem->counter%2];
    
    if(0 == intitem->amo_regaddr)
    {
        // marco.accame on 24 may 2021: i prefer to impose reading of a register than to return.
        // i fear that s_hal_spiencoder_onreceiv_reg_init() is not invoked and that ... eventually
        // the intitem->config.callback_on_rx(intitem->config.arg); is never called.
        // so: i force teh reading anyways
        intitem->amo_regaddr = 0x76;
        //return(hal_res_NOK_generic);
    }
        
    //Not const, cause it could change every time...
    uint8_t txframe_rinit[2] = {0x97, intitem->amo_regaddr};
    
    //Enable CHIPSELECT
    hal_gpio_setval(intitem->chip_sel, hal_gpio_valLOW);
    
    // SPI: set the callback function
    hal_spi_on_framesreceived_set(intitem->spiid, s_hal_spiencoder_onreceiv_reg_init, (void*)id);
    
    // SPI: set the sizeofframe for this transmission (2)
    hal_spi_set_sizeofframe(intitem->spiid, 2);
    
    // SPI: set the isrtxframe
    hal_spi_set_isrtxframe(intitem->spiid, txframe_rinit);
        
    // SPI: start to receive (only one frame)
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
    
    return (hal_res_OK);
}

static hal_result_t s_hal_spiencoder_read_register_execute_t2(hal_spiencoder_t id)
{
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    static const uint8_t txframe_rdata[3] = {0xAD, 0x00, 0x00};
    
    //Enable CHIPSELECT
    hal_gpio_setval(intitem->chip_sel, hal_gpio_valLOW);
    
    // SPI: set the callback function
    hal_spi_on_framesreceived_set(intitem->spiid, s_hal_spiencoder_onreceiv_reg_data, (void*)id);
    
    // SPI: set the sizeofframe for this transmission (3)
    hal_spi_set_sizeofframe(intitem->spiid, 3);
    
    // SPI: set the isrtxframe
    hal_spi_set_isrtxframe(intitem->spiid, txframe_rdata);
        
    // SPI: start to receive (only one frame)
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
    
    return (hal_res_OK);
}

static hal_boolval_t s_hal_spiencoder_supported_is(hal_spiencoder_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_spiencoder__theboardconfig.supportedmask, HAL_encoder_id2index(id)) );
}

static void s_hal_spiencoder_initted_set(hal_spiencoder_t id)
{
    hl_bits_word_bitset(&s_hal_spiencoder_theinternals.inittedmask, HAL_encoder_id2index(id));
}

static void s_hal_spiencoder_initted_reset(hal_spiencoder_t id)
{
    hl_bits_word_bitclear(&s_hal_spiencoder_theinternals.inittedmask, HAL_encoder_id2index(id));
}

static hal_boolval_t s_hal_spiencoder_initted_is(hal_spiencoder_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_spiencoder_theinternals.inittedmask, HAL_encoder_id2index(id)));
}

static void s_hal_spiencoder_onreceiv(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_spiencoder_t id = (hal_spiencoder_t)tmp;
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
    //hal_spi_stop(intitem->spiid);
    hal_mux_disable(intitem->muxid);
    
    // ok ... now i get the frame of "framesize*numberOfFrame" bytes.
    
    // hal_spi_get collects data directly from the fifo associated to the receveing packets
    // gets all the items inside the fifo (in this case)
    hal_spi_get(intitem->spiid, intitem->rxframes[1], NULL);
    
    // Formatting result and saving in position field
    if(intitem->config.type == hal_spiencoder_typeAEA)
    {
        intitem->position = s_hal_spiencoder_frame2position_t1(intitem->rxframes[1]);
    }
#if defined(AEA3_SUPPORT)
    else if(intitem->config.type == hal_spiencoder_typeAEA3)
    {
        intitem->position = s_hal_spiencoder_frame2position_t4(intitem->rxframes[1]);
    }
#endif
    else if(intitem->config.type == hal_spiencoder_typeAksIM2)
    {
        // parse the data received
        hal_spiencoder_aksim2_data data;
        s_hal_spiencoder_parse_data_aksim2(intitem->rxframes[1], &data);
        
        // save them into the internal item
        intitem->multiturncounter = data.multiturncounter;
        intitem->position = data.position;
        intitem->status_bits = data.status_bits;
        intitem->crc = data.crc;
    }
    else 
    {
        intitem->position = 0;
    }
        
    // ok. now i call the callbcak on execution of encoder
    
    if(NULL != intitem->config.callback_on_rx)
    {
        intitem->config.callback_on_rx(intitem->config.arg);
    }
}

#if !defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)
static void s_hal_spiencoder_onreceived_daisychain(void* p)
{
    int32_t tmp = (int32_t)p;                   
    hal_spiencoder_t id = (hal_spiencoder_t)tmp;
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
    hal_mux_disable(intitem->muxid);    
    
    // ok, now i get the frame .. the chainof2 it has two words with 2 uint16_t. thus ... 4 bytes
    hal_spi_get(intitem->spiid, (uint8_t*)&intitem->rxframechain, NULL);
    
    // so far i get the two readings in raw format inside the 32 bits output
    // the formatting in each uint16_t value is: (see pag 15)
    // bit 0:       parity                  parity = value & 0x0001      // calculated as parity even calculated on upper 15 bits 
    // bit 1:       errorframe              errorframe = (value >> 1) & 0x0001
    // bit 2-15:    data                    data = (value >> 2) & 0x3fff
    // in data, assuming it is now shifted down in bits 0:15 (see pag 21)
    // bit 0-11:    position                position = data & 0x0fff // or (value >> 2) & 0x0fff
    // bit 13:      alarmlow                alarmlow = (data >> 13) & 0x0001
    // bit 12:      alarmhigh               alarmhigh = (data >> 12) & 0x0001    
    intitem->position = intitem->rxframechain[0] | (intitem->rxframechain[1] << 16);
    
    if(NULL != intitem->config.callback_on_rx)
    {
        intitem->config.callback_on_rx(intitem->config.arg);
    }    
}
#endif

#if !defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)
static void s_hal_spiencoder_onreceived_daisychain_prepare(void* p)
{
    int32_t tmp = (int32_t)p;                   
    hal_spiencoder_t id = (hal_spiencoder_t)tmp;
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
    hal_mux_disable(intitem->muxid);    
    
    // ok, now i get the frame .. 
    // for case daisy-2: it has two words of uint16_t each. thus ... 4 bytes
    // for case daisy-3: it has 3 words of uint16_t each. thus ... 6 bytes
    // i get it just to remove it from spi buffer.
    hal_spi_get(intitem->spiid, (uint8_t*)&intitem->rxframechain, NULL);
    
    // how much should we wait before we enable the mux again? is 1 microsec enough?   
    //hal_sys_delay(1); 
    
    static const uint16_t txframe_as5055a_nop[3] = {0x0000, 0x0000, 0x0000}; // i use three items for teh case of chainof3
    s_hal_spiencoder_2chained_askvalues(intitem, s_hal_spiencoder_onreceived_daisychain, p, (uint8_t*)txframe_as5055a_nop, (hal_spiencoder_typeCHAINof2 == intitem->config.type) ? 2 : 3);
}
#endif

static void s_hal_spiencoder_onreceiv_sdad_status(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_spiencoder_t id = (hal_spiencoder_t)tmp;
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
    //Disable chip select
    hal_gpio_setval(intitem->chip_sel, hal_gpio_valHIGH);
    
    // ok ... now i get the frame of 2 bytes (only 1 is used)
    // hal_spi_get collects data directly from the fifo associated to the receveing packets
    // gets all the items inside the fifo (in this case)
    hal_spi_get(intitem->spiid, intitem->rxframes[0], NULL);

    //Invoke the reading of the sensor data
    s_hal_spiencoder_read_sensor_t2 (id);
}

static void s_hal_spiencoder_onreceiv_sensor_data(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_spiencoder_t id = (hal_spiencoder_t)tmp;
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
        
    //Disable chip select
    hal_gpio_setval(intitem->chip_sel, hal_gpio_valHIGH);
    
    // ok ... now i get the frame of 4 bytes (3 are used)
    // hal_spi_get collects data directly from the fifo associated to the receveing packets
    // gets all the items inside the fifo (in this case)
    hal_spi_get(intitem->spiid, intitem->rxframes[1], NULL);
    
    //Formatting the result and saving in position field
    intitem->position = s_hal_spiencoder_frame2position_t2(intitem->rxframes[1]);
    
    //Invoke the register initialization
    s_hal_spiencoder_read_register_init_t2 (id);
}

static void s_hal_spiencoder_onreceiv_reg_init(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_spiencoder_t id = (hal_spiencoder_t)tmp;
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
    //hal_spi_stop(intitem->spiid);
    //Disable chip select
    hal_gpio_setval(intitem->chip_sel, hal_gpio_valHIGH);
    uint8_t rxregdummy [3];
    //No bytes of this transmission are used
    hal_spi_get(intitem->spiid, rxregdummy, NULL);
    //Invoke the register reading
    s_hal_spiencoder_read_register_execute_t2(id);
}

static void s_hal_spiencoder_onreceiv_reg_data(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_spiencoder_t id = (hal_spiencoder_t)tmp;
    hal_spiencoder_internal_item_t* intitem = s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)];
    
    //hal_spi_stop(intitem->spiid);
    hal_mux_disable(intitem->muxid);
    
    //Save the bytes of transmission
    hal_spi_get(intitem->spiid, intitem->rxframes[2], NULL);
    
    //Now I can invoke the callback function for retrieve the data (at application level)
    if(NULL != intitem->config.callback_on_rx)
    {
        intitem->config.callback_on_rx(intitem->config.arg);
    }
}

// Formatting the result for encoder type 1 (AEA)
static hal_spiencoder_position_t s_hal_spiencoder_frame2position_t1(uint8_t* frame)
{
    // simone girardi 09/17/2022: This function returns the whole rawValue (18 bits), not only the position
    uint32_t pos = 0;
    // pos = frame[0] | (frame[1] << 8) | (frame[2] << 16);
    // VALE formatting result
    pos = ((frame[0] & 0x7F) << 16) | (frame[1] << 8) | (frame[2] & 0xE0);
    pos = pos >> 5;
    pos = pos & 0x03FFFF; 
    return(pos);
}

// Formatting the result for encoder type 2
static hal_spiencoder_position_t s_hal_spiencoder_frame2position_t2(uint8_t* frame)
{
    uint32_t pos = 0;
    pos = ((frame[1] << 16) | (frame[2] << 8) | (frame[3]));
    pos = pos >> 4;
    pos = pos & 0x0FFFFF;
    return(pos);
}

// Formatting the result for encoder type 4 (AEA3)
#if defined(AEA3_SUPPORT)
static hal_spiencoder_position_t s_hal_spiencoder_frame2position_t4(uint8_t* frame)
{
    // AEA3 offers 14 bit of resolution
    uint16_t pos = (((frame[1] & 0x7F) << 8 )| frame[0]); // in this way we are reading 16 bits (1 bit dummy + 14 bit valid + 1 bit zero padded). The dummy bit has been masked.
    
    return pos;
}
#endif

// Formatting the result for encoder type 5 (AksIM2)
static void s_hal_spiencoder_parse_data_aksim2(uint8_t* frame, hal_spiencoder_aksim2_data* data)
{
    // Encoder position data structure
    // -------------------------------
    // b47 : b32 Multiturn counter (if specified in part number) - Left aligned, MSB first. 
    // b31 : b10 Encoder position + zero padding bits - Left aligned, MSB first. 
    // b9 Error - If low, the position data is not valid. 
    // b8 Warning - If low, the position data is valid, but some operating conditions are close to limits. 
    // b7 : b0 Inverted CRC, 0x97 polynomial
    
    data->multiturncounter = (frame[0] << 8) | frame[1];
    data->position = (frame[2] << 11) | (frame[3] << 3) | ((frame[4] & 0xe0) >> 5); // the last shift eliminates the non-position values (2) + zero padded bits (3)
    data->status_bits = (frame[4] & 0x03);
    data->crc = frame[5];
}

#if !defined(HAL_SPIENCODER_xCHAINED_USE_RAWMODE)
static void s_hal_spiencoder_2chained_askvalues(hal_spiencoder_internal_item_t* intitem, hal_callback_t callback, void* arg, uint8_t *txframe, uint8_t sizeofframe)
{   
    hal_mux_enable(intitem->muxid, intitem->muxsel);

    hal_spi_on_framesreceived_set(intitem->spiid, callback, arg);

    hal_spi_set_sizeofframe(intitem->spiid, sizeofframe); // or 3 if chainof3 ....

    hal_spi_set_isrtxframe(intitem->spiid, txframe);

    hal_spi_start(intitem->spiid, 1); // 1 frame 
}
#endif


#endif//HAL_USE_SPIENCODER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



