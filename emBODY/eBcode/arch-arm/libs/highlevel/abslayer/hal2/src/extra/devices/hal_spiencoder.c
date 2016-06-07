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
    .reg_address =      NULL, 
    .sdata_precheck =   hal_false
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
    uint8_t                     rxframes[3][4];     // 3 possible frames received. The size of everyone is the maximum possible
    uint16_t                    rxframechain[3];    // 1 frame of 2 words of 16 bits
    hl_chip_ams_as5055a_channel_t chainchannel;
} hal_spiencoder_internal_item_t;


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
    .datacapture                = hal_spi_datacapture_1edge
};

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
    
    // Set the default config if cfc is NULL
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
        // so that hal_spiencoder_get_value() can be called to retrieve teh encoder value
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

extern hal_result_t hal_spiencoder_get_value2(hal_spiencoder_t id, hal_spiencoder_value_t* val)
{
    
    
  

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
        if ((intitem->config.reg_address) == 0x76)
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
        else if ((intitem->config.reg_address) == 0x77)
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
    
    //Dealloc all the associated memory
    hal_heap_delete((void**)&(s_hal_spiencoder_theinternals.items[HAL_encoder_id2index(id)]));
    //hal_heap_delete((void**)&intitem);

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
    
    if (intitem->config.reg_address == NULL)
    {
        return (hal_res_NOK_generic);
    }
        
    //Not const, cause it could change every time...
    uint8_t txframe_rinit[2] = {0x97, intitem->config.reg_address};
    
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
    
    // ok ... now i get the frame of three bytes.
    
    // hal_spi_get collects data directly from the fifo associated to the receveing packets
    // gets all the items inside the fifo (in this case)
    hal_spi_get(intitem->spiid, intitem->rxframes[1], NULL);
    
    // Formatting result and saving in position field
    intitem->position = s_hal_spiencoder_frame2position_t1(intitem->rxframes[1]);
        
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

// Formatting the result for encoder type 1
static hal_spiencoder_position_t s_hal_spiencoder_frame2position_t1(uint8_t* frame)
{
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



