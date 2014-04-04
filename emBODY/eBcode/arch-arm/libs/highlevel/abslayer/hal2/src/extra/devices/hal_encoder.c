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


/* @file       hal_encoder.c
	@brief      This file implements internal implementation of the hal encoder module.
	@author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       02/07/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
//#include "hal_middleware_interface.h"

#ifdef HAL_USE_ENCODER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_gpio.h"
#include "hl_bits.h" 
#include "hal_heap.h"



 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_encoder.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_encoder_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_encoder_id2index(t)              ((uint8_t)((t)))



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
    uint8_t                 rxframe[4]; // it could be 3 ...
} hal_encoder_internal_item_t;


typedef struct
{
    uint32_t                                inittedmask;
    hal_encoder_internal_item_t*            items[hal_encoders_number];   
} hal_encoder_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_encoder_supported_is(hal_encoder_t id);
static void s_hal_encoder_initted_set(hal_encoder_t id);
static hal_boolval_t s_hal_encoder_initted_is(hal_encoder_t id);


static void s_hal_encoder_onreceiv(void* p);

static hal_encoder_position_t s_hal_encoder_frame2position(uint8_t* frame);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const hal_spi_cfg_t s_hal_encoder_spicfg_master =
{
    .ownership                  = hal_spi_ownership_master,
    .direction                  = hal_spi_dir_rxonly,
    .activity                   = hal_spi_act_framebased,
//     .prescaler                  = hal_spi_prescaler_064,
//     .maxspeed                   = 0, 
    .prescaler                  = hal_spi_prescaler_auto,
    .maxspeed                   = 1000*1000,   
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

static hal_encoder_theinternals_t s_hal_encoder_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_encoder_init(hal_encoder_t id, const hal_encoder_cfg_t *cfg)
{
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
    hal_result_t res = hal_res_NOK_generic;

    if(hal_false == s_hal_encoder_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_encoder_cfg_default;
    }
     
    if(hal_true == s_hal_encoder_initted_is(id))
    {
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_encoder_cfg_t)))
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
        intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)] = hal_heap_new(sizeof(hal_encoder_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    } 
    memcpy(&intitem->config, cfg, sizeof(hal_encoder_cfg_t));   
    
    // configure the required mux port and spi port.   
    
    intitem->spiid  = hal_encoder__theboardconfig.spimap[HAL_encoder_id2index(id)].spiid;    
    intitem->muxid  = hal_encoder__theboardconfig.spimap[HAL_encoder_id2index(id)].muxid;
    intitem->muxsel = hal_encoder__theboardconfig.spimap[HAL_encoder_id2index(id)].muxsel;
    intitem->position  = 0;
    
    res = hal_mux_init(intitem->muxid, NULL);
    if(hal_res_OK != res)
    {
        return(res);
    }
    
    // we get the max speed of spi from what specified in hal_encoder__theboardconfig
    hal_spi_cfg_t spicfg;
    memcpy(&spicfg, &s_hal_encoder_spicfg_master, sizeof(hal_spi_cfg_t));
    spicfg.maxspeed = hal_encoder__theboardconfig.spimaxspeed;
    
    res = hal_spi_init(intitem->spiid, &spicfg);
    if(hal_res_OK != res)
    {
        return(res);
    }
 
    s_hal_encoder_initted_set(id);
    return(hal_res_OK);
}


extern hal_result_t hal_encoder_read_start(hal_encoder_t id)
{  
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];   
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_encoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
       
    hal_mux_enable(intitem->muxid, intitem->muxsel);
    
    hal_spi_on_framereceiv_set(intitem->spiid, s_hal_encoder_onreceiv, (void*)id);
    
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
    
    // when the frame is received, then the isr will call s_hal_encoder_onreceiv() to copy the frame into local memory,
    // so that hal_encoder_get_value() can be called to retrieve teh encoder value
    
    return(hal_res_OK);
}



extern hal_result_t hal_encoder_get_value(hal_encoder_t id, hal_encoder_position_t* value)
{
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_encoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

#endif
 
#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)  
    if(NULL == value)
    {
        return(hal_res_NOK_generic); 
    }
#endif    

    *value = intitem->position; 
    
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

static hal_boolval_t s_hal_encoder_supported_is(hal_encoder_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_encoder__theboardconfig.supportedmask, HAL_encoder_id2index(id)) );
}


static void s_hal_encoder_initted_set(hal_encoder_t id)
{
    hl_bits_word_bitset(&s_hal_encoder_theinternals.inittedmask, HAL_encoder_id2index(id));
}


static hal_boolval_t s_hal_encoder_initted_is(hal_encoder_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_encoder_theinternals.inittedmask, HAL_encoder_id2index(id)));
}



static void s_hal_encoder_onreceiv(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_encoder_t id = (hal_encoder_t)tmp;
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
    
    //hal_spi_stop(intitem->spiid);
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


#endif//HAL_USE_ENCODER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



