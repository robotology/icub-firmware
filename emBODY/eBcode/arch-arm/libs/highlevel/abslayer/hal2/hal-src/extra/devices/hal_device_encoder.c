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


/* @file       hal_device_encoder.c
	@brief      This file implements internal implementation of the hal encoder module.
	@author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       02/07/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_ENCODER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_gpio.h"
#include "hal_brdcfg.h"
#include "hal_utility_bits.h" 
#include "hal_heap.h"



 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_encoder.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_encoder_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_encoder_id2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_encoder_cfg_t hal_encoder_cfg_default = { .priority = hal_int_priority15, .callback_on_rx = NULL, .arg = NULL };

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_encoder_cfg_t       config;
    hal_mux_t               muxid;
    hal_mux_sel_t           muxsel;
    hal_spi_t               spiid;
    hal_encoder_position_t  position;
    uint8_t                 rxframe[4];
} hal_device_encoder_internal_item_t;


typedef struct
{
    uint16_t                                    initted;
    hal_device_encoder_internal_item_t*         items[hal_encoders_number];   
} hal_device_encoder_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_encoder_supported_is(hal_encoder_t id);
static void s_hal_device_encoder_initted_set(hal_encoder_t id);
static hal_boolval_t s_hal_device_encoder_initted_is(hal_encoder_t id);


static void s_hal_encoder_onreceiv(void* p);

static hal_encoder_position_t s_hal_encoder_frame2position(uint8_t* frame);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const hal_spi_cfg_t s_hal_device_encoder_spicfg_master =
{
    .ownership                  = hal_spi_ownership_master,
    .direction                  = hal_spi_dir_rxonly,
    .activity                   = hal_spi_act_framebased,
    .prescaler                  = hal_spi_prescaler_064,
    .speed                      = hal_spi_speed_dontuse, 
    .sizeofframe                = 3,
    .capacityoftxfifoofframes   = 0,
    .capacityofrxfifoofframes   = 1,
    .dummytxvalue               = 0,    
    .onframetransm              = NULL,
    .argonframetransm           = NULL,
    .onframereceiv              = s_hal_encoder_onreceiv,
    .argonframereceiv           = NULL
};   


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_device_encoder_theinternals_t s_hal_device_encoder_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_encoder_init(hal_encoder_t id, const hal_encoder_cfg_t *cfg)
{
    //hal_result_t res = hal_res_NOK_generic;
    hal_device_encoder_internal_item_t* intitem = s_hal_device_encoder_theinternals.items[HAL_device_encoder_id2index(id)];

    if(hal_false == s_hal_device_encoder_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_encoder_cfg_default;
    }
     
        
    // configure the required mux port and spi port.
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_device_encoder_theinternals.items[HAL_device_encoder_id2index(id)] = hal_heap_new(sizeof(hal_device_encoder_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }       
    
    memcpy(&intitem->config, cfg, sizeof(hal_encoder_cfg_t));   
    intitem->spiid  = hal_brdcfg_device_encoder__theconfig.spimap[HAL_device_encoder_id2index(id)].spiid;    
    intitem->muxid  = hal_brdcfg_device_encoder__theconfig.spimap[HAL_device_encoder_id2index(id)].muxid;
    intitem->muxsel = hal_brdcfg_device_encoder__theconfig.spimap[HAL_device_encoder_id2index(id)].muxsel;
    intitem->position  = 0;
    

    
     
    #warning HAL-WIP --> configure mux and spi. obviously if already initted we dont init the spi and the port anymore.
    
    hal_mux_init(intitem->muxid, NULL);
    
    hal_spi_init(intitem->spiid, &s_hal_device_encoder_spicfg_master);
    
 
    s_hal_device_encoder_initted_set(id);
    return(hal_res_OK);
}


extern hal_result_t hal_encoder_read_start(hal_encoder_t id)
{  
    hal_device_encoder_internal_item_t* intitem = s_hal_device_encoder_theinternals.items[HAL_device_encoder_id2index(id)];   
    
    if(hal_false == s_hal_device_encoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 
    
       
    hal_mux_enable(intitem->muxid, intitem->muxsel);
    
    hal_spi_on_framereceiv_set(intitem->spiid, s_hal_encoder_onreceiv, (void*)id);
    
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
    
    // quando il frame sara' stato ricevuto allora si chiamera' la callback che prima scrive in ram. poi ...
    
    return(hal_res_OK);
}



extern hal_result_t hal_encoder_get_value(hal_encoder_t id, hal_encoder_position_t* value)
{
    hal_device_encoder_internal_item_t* intitem = s_hal_device_encoder_theinternals.items[HAL_device_encoder_id2index(id)];
    
    if(hal_false == s_hal_device_encoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 


    if(NULL != value)
    {
        *value = intitem->position; 
        return(hal_res_OK);
    }
    
    return(hal_res_NOK_generic);
   
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern hal_result_t hal_device_encoder_hid_static_memory_init(void)
{
    memset(&s_hal_device_encoder_theinternals, 0, sizeof(s_hal_device_encoder_theinternals));
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_encoder_supported_is(hal_encoder_t id)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_encoder__theconfig.supported_mask, HAL_device_encoder_id2index(id)) );
}


static void s_hal_device_encoder_initted_set(hal_encoder_t id)
{
    hal_utility_bits_halfword_bitset(&s_hal_device_encoder_theinternals.initted, HAL_device_encoder_id2index(id));
}


static hal_boolval_t s_hal_device_encoder_initted_is(hal_encoder_t id)
{
    return(hal_utility_bits_halfword_bitcheck(s_hal_device_encoder_theinternals.initted, HAL_device_encoder_id2index(id)));
}



static void s_hal_encoder_onreceiv(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversione from pointer to smaller integer
    hal_encoder_t id = (hal_encoder_t)tmp;
    hal_device_encoder_internal_item_t* intitem = s_hal_device_encoder_theinternals.items[HAL_device_encoder_id2index(id)];
    
    hal_spi_stop(intitem->spiid);
    hal_mux_disable(intitem->muxid);
    
    // ok ... now i get the frame of three bytes.
    
    hal_spi_get(intitem->spiid, intitem->rxframe, NULL);
    
    intitem->position = s_hal_encoder_frame2position(intitem->rxframe);
    
    // ok. now i call the callbcak on execution of encoder
    
    if(NULL != intitem->config.callback_on_rx)
    {
        intitem->config.callback_on_rx(intitem->config.arg);
    }
    
}


static hal_encoder_position_t s_hal_encoder_frame2position(uint8_t* frame)
{
    uint32_t pos = 0;
    
    //pos = frame[0] | (frame[1] << 8) | (frame[2] << 16);
    // VALE formatting result
    pos = ((frame[0] & 0x7F) << 16) | (frame[1] << 8) | (frame[2] & 0xE0);
    pos = pos >> 5;
    return(pos);
}


#endif//HAL_USE_DEVICE_ENCODER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



