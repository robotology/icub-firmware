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

// Cast from t to uint8_t
#define HAL_encoder_id2index(t)              ((uint8_t)((t)))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_encoder_cfg_t hal_encoder_cfg_default = { .priority = hal_int_priority15, .callback_on_rx = NULL, .arg = NULL, .type = hal_encoder_tundefined};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_encoder_cfg_t       config;
    hal_mux_t               muxid;
		hal_gpio_t							chip_sel;
    hal_mux_sel_t           muxsel;
    hal_spi_t               spiid;
    hal_encoder_position_t  position;
    uint8_t                 rxframes[3][4]; //3 possible frames received. The size of everyone is the maximum possible
		uint8_t									act_reg_add;
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

static hal_result_t s_hal_encoder_read_sdad_status_t2(hal_encoder_t id);
static hal_result_t s_hal_encoder_read_sensor_t2(hal_encoder_t id);
static hal_result_t s_hal_encoder_read_register_init_t2(hal_encoder_t id);
static hal_result_t s_hal_encoder_read_register_execute_t2(hal_encoder_t id);

//Static callback functions
static void s_hal_encoder_onreceiv(void* p);
static void s_hal_encoder_onreceiv_sdad_status(void* p);
static void s_hal_encoder_onreceiv_sensor_data(void* p);
static void s_hal_encoder_onreceiv_reg_init(void* p);
static void s_hal_encoder_onreceiv_reg_data(void* p);

static hal_encoder_position_t s_hal_encoder_frame2position_t1(uint8_t* frame);
static hal_encoder_position_t s_hal_encoder_frame2position_t2(uint8_t* frame);


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
    .dummytxvalue               = 0x00, //removable?
		.starttxvalue								= 0x00,	//removable?
    .onframetransm              = NULL,
    .argonframetransm           = NULL,
    .onframereceiv              = s_hal_encoder_onreceiv,
    .argonframereceiv           = NULL,
		.cpolarity									= hal_spi_cpolarity_high,
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

//Initialization
extern hal_result_t hal_encoder_init(hal_encoder_t id, const hal_encoder_cfg_t *cfg)
{
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
    hal_result_t res = hal_res_NOK_generic;

		// If encoder not supported return not OK
    if(hal_false == s_hal_encoder_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
		// Set the default config if cfc is NULL
    if(NULL == cfg)
    {
        cfg = &hal_encoder_cfg_default;
    }
    
		// Check initialization
    if(hal_true == s_hal_encoder_initted_is(id))
    {
				//Memory comparison
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
		// Copy configuration in a protected memory
    memcpy(&intitem->config, cfg, sizeof(hal_encoder_cfg_t));   
    
    // configure the required mux port and spi port (using the configuration of the board for the selected encoder)
		// "hal_encoder__theboardconfig" struct is defined inside the board modules 
    intitem->spiid  = hal_encoder__theboardconfig.spimap[HAL_encoder_id2index(id)].spiid;    
    intitem->muxid  = hal_encoder__theboardconfig.spimap[HAL_encoder_id2index(id)].muxid;
    intitem->muxsel = hal_encoder__theboardconfig.spimap[HAL_encoder_id2index(id)].muxsel;
    intitem->position  = 0;
		intitem->act_reg_add = 0x00;
    
    res = hal_mux_init(intitem->muxid, NULL);
    if(hal_res_OK != res)
    {
        return(res);
    }
    
    hal_spi_cfg_t spicfg;
		memcpy(&spicfg, &s_hal_encoder_spicfg_master, sizeof(hal_spi_cfg_t));
		
		if (intitem->config.type == hal_encoder_t2)
		{
				//We use the master SPI configuration for encoder type 2
				// This sizeofframe is used as an upper bound for reserving heap memory (hal_spi) 
			  spicfg.sizeofframe = 4;
				spicfg.cpolarity = hal_spi_cpolarity_low;
				hal_mux_get_cs(intitem->muxid, &intitem->chip_sel);
		}
		
		//we get the max speed of spi from what specified in hal_encoder__theboardconfig
		spicfg.maxspeed = hal_encoder__theboardconfig.spimaxspeed;
    
		//Initialize the SPI with the just set config
    res = hal_spi_init(intitem->spiid, &spicfg);
    if(hal_res_OK != res)
    {
        return(res);
    }
 
		// Flag to set the encoder initialized
    s_hal_encoder_initted_set(id);
    return(hal_res_OK);
}

//Start reading for encoder t1
extern hal_result_t hal_encoder_read_start(hal_encoder_t id)
{  
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
		static const uint8_t txframe[3] = {0x00, 0x00, 0x00};
    
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_encoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
		if (intitem->config.type != hal_encoder_t1)
		{
		return (hal_res_NOK_generic);
		}
#endif
    // Mux enabled
    hal_mux_enable(intitem->muxid, intitem->muxsel);
    
		// SPI: set the callback function
    hal_spi_on_framereceiv_set(intitem->spiid, s_hal_encoder_onreceiv, (void*)id);
    
		//Added 26/11/2014
		
		// SPI: set the sizeofframe for this transmission (3)
		hal_spi_set_sizeofframe(intitem->spiid, 3);
	
		// SPI: set the isrtxframe
		hal_spi_set_isrtxframe(intitem->spiid, txframe);
		
		// SPI start to receive (only one frame)
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
    
    // when the frame is received, then the isr will call s_hal_encoder_onreceiv() to copy the frame into local memory,
    // so that hal_encoder_get_value() can be called to retrieve teh encoder value
    
    return(hal_res_OK);
}

//Start reading for encoder t2
extern hal_result_t hal_encoder_read_start_t2(hal_encoder_t id, uint8_t reg_address)
{
		hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];   
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_encoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
		if (intitem->config.type != hal_encoder_t2)
		{
		return (hal_res_NOK_generic);
		}
#endif
		// Saving the address to be read in intitem
		intitem->act_reg_add = reg_address;
		
		//Enabling the MUX
		hal_mux_enable(intitem->muxid, intitem->muxsel);
		
		// 1 - Reading of SDAD status (we can avoid it in the final application...)
		// This launch a chain of execution for performing a complete reading of the sensor data
		// (we don't need to launch phase 2 and 3 no more)
		s_hal_encoder_read_sdad_status_t2 (id);
		
		//2 - Reading of sensor data
		//while (phase !=  2);
		//hal_sys_delay(0);
		//s_hal_encoder_read_sensor_t2 (id);
		
		//3 - Reading of the register
		//while (phase !=  3);
		//hal_sys_delay(0);
		//s_hal_encoder_read_register_init_t2 (id);
    return(hal_res_OK);
}

// Get the last value saved with a read_start
extern hal_result_t hal_encoder_get_value(hal_encoder_t id, hal_encoder_position_t* value)
{
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];

#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_encoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
		if(NULL == value)
    {
        return(hal_res_NOK_generic); 
    }
#endif
    *value = intitem->position;
    
    return(hal_res_OK);
}

// Get the last values saved with a read_start_t2
extern hal_result_t hal_encoder_get_value_t2(hal_encoder_t id, hal_encoder_position_t* pos, hal_bool_t* val, uint16_t* reg)
{
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];

#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_encoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
		if((NULL == pos) || (NULL == val) || (NULL == reg))
    {
        return(hal_res_NOK_generic); 
    }
#endif
    *pos = intitem->position;
		
    if(intitem->rxframes[0][1] == 0x80)
		{
			*val = hal_true;
		}
		else
		{
			*val = hal_false;
		}

		*reg = (((intitem->rxframes[2][1] << 8 ) & 0xFF00)  | (intitem->rxframes[2][2])); 
		
    return(hal_res_OK);
}

//Get the single bytes inside the array rxframes[1] = sensor data
extern hal_result_t hal_encoder_get_frame(hal_encoder_t id, uint8_t* rec_byte1, uint8_t* rec_byte2, uint8_t* rec_byte3, uint8_t* rec_byte4)
{
		hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];

#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_encoder_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
		if((NULL == rec_byte1) || (NULL == rec_byte2) || (NULL == rec_byte3) || (NULL == rec_byte4))
		{
        return(hal_res_NOK_generic); 
    }
#endif
		*rec_byte1 = intitem->rxframes[1][0];
		*rec_byte2 = intitem->rxframes[1][1];
		*rec_byte3 = intitem->rxframes[1][2];
		*rec_byte4 = intitem->rxframes[1][3];
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
static hal_result_t s_hal_encoder_read_sdad_status_t2(hal_encoder_t id)
{
		hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
	  static const uint8_t txframe_sdad[2] = {0xF5, 0x00};
		
		// SPI: set the callback function
    hal_spi_on_framereceiv_set(intitem->spiid, s_hal_encoder_onreceiv_sdad_status, (void*)id);
    
		// SPI: set the sizeofframe for this transmission (2)
		hal_spi_set_sizeofframe(intitem->spiid, 2);
	
		// SPI: set the isrtxframe
		hal_spi_set_isrtxframe(intitem->spiid, txframe_sdad);
		
		// SPI start to receive (only one frame)
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
	
		return (hal_res_OK);
}

static hal_result_t s_hal_encoder_read_sensor_t2(hal_encoder_t id)
{	
		hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
		static const uint8_t txframe_sdata[4] = {0xA6, 0x00, 0x00, 0x00};
	
		//Enable CHIPSELECT
		hal_gpio_setval(intitem->chip_sel, hal_gpio_valLOW);
    
		// SPI: set the callback function
    hal_spi_on_framereceiv_set(intitem->spiid, s_hal_encoder_onreceiv_sensor_data, (void*)id);
    
		// SPI: set the sizeofframe for this transmission (4)
		hal_spi_set_sizeofframe(intitem->spiid, 4);
	
		// SPI: set the isrtxframe
		hal_spi_set_isrtxframe(intitem->spiid, txframe_sdata);
		
		// SPI: start to receive (only one frame)
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
	
		return (hal_res_OK);
}


static hal_result_t s_hal_encoder_read_register_init_t2(hal_encoder_t id)
{
		hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
	
		//Not const, cause it could change every time...
		uint8_t txframe_rinit[2] = {0x97, intitem->act_reg_add};
	
		//Enable CHIPSELECT
		hal_gpio_setval(intitem->chip_sel, hal_gpio_valLOW);
    
		// SPI: set the callback function
    hal_spi_on_framereceiv_set(intitem->spiid, s_hal_encoder_onreceiv_reg_init, (void*)id);
    
		// SPI: set the sizeofframe for this transmission (4)
		hal_spi_set_sizeofframe(intitem->spiid, 2);
	
		// SPI: set the isrtxframe
		hal_spi_set_isrtxframe(intitem->spiid, txframe_rinit);
		
		// SPI: start to receive (only one frame)
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
	
		return (hal_res_OK);
}

static hal_result_t s_hal_encoder_read_register_execute_t2(hal_encoder_t id)
{
		hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
		static const uint8_t txframe_rdata[3] = {0xAD, 0x00, 0x00};
	
		//Enable CHIPSELECT
		hal_gpio_setval(intitem->chip_sel, hal_gpio_valLOW);
    
		// SPI: set the callback function
    hal_spi_on_framereceiv_set(intitem->spiid, s_hal_encoder_onreceiv_reg_data, (void*)id);
    
		// SPI: set the sizeofframe for this transmission (3)
		hal_spi_set_sizeofframe(intitem->spiid, 3);
	
		// SPI: set the isrtxframe
		hal_spi_set_isrtxframe(intitem->spiid, txframe_rdata);
		
		// SPI: start to receive (only one frame)
    hal_spi_start(intitem->spiid, 1); // 1 solo frame ...
	
		return (hal_res_OK);
}

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
    
		// hal_spi_get collects data directly from the fifo associated to the receveing packets
		// gets all the items inside the fifo (in this case)
    hal_spi_get(intitem->spiid, intitem->rxframes[1], NULL);
    
		if (intitem->config.type == hal_encoder_t1)
		{
			intitem->position = s_hal_encoder_frame2position_t1(intitem->rxframes[1]);
		}
		else if (intitem->config.type == hal_encoder_t2)
		{
			intitem->position = s_hal_encoder_frame2position_t2(intitem->rxframes[1]);
		}
    // ok. now i call the callbcak on execution of encoder
    
    if(NULL != intitem->config.callback_on_rx)
    {
      intitem->config.callback_on_rx(intitem->config.arg);
    }
}

static void s_hal_encoder_onreceiv_sdad_status(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_encoder_t id = (hal_encoder_t)tmp;
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
    
    //hal_mux_disable(intitem->muxid);
		//Disable chip select
		hal_gpio_setval(intitem->chip_sel, hal_gpio_valHIGH);
    
    // ok ... now i get the frame of 2 bytes (only 1 is used)
		// hal_spi_get collects data directly from the fifo associated to the receveing packets
		// gets all the items inside the fifo (in this case)
    hal_spi_get(intitem->spiid, intitem->rxframes[0], NULL);
	
		//phase = 2;
		//Invoke the reading of the sensor data
		s_hal_encoder_read_sensor_t2 (id);
}

static void s_hal_encoder_onreceiv_sensor_data(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_encoder_t id = (hal_encoder_t)tmp;
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
    
		//Disable chip select
		hal_gpio_setval(intitem->chip_sel, hal_gpio_valHIGH);
    
    // ok ... now i get the frame of 4 bytes (3 are used)
		// hal_spi_get collects data directly from the fifo associated to the receveing packets
		// gets all the items inside the fifo (in this case)
    hal_spi_get(intitem->spiid, intitem->rxframes[1], NULL);
	
		intitem->position = s_hal_encoder_frame2position_t2(intitem->rxframes[1]);
	
		//Invoke the register initialization
		s_hal_encoder_read_register_init_t2 (id);
}

static void s_hal_encoder_onreceiv_reg_init(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_encoder_t id = (hal_encoder_t)tmp;
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
    
    //hal_spi_stop(intitem->spiid);
		//Disable chip select
		hal_gpio_setval(intitem->chip_sel, hal_gpio_valHIGH);
    uint8_t rxregdummy [3];
		//No bytes of this transmission are used
		hal_spi_get(intitem->spiid, rxregdummy, NULL);
		//Invoke the register reading
		s_hal_encoder_read_register_execute_t2(id);
}
static void s_hal_encoder_onreceiv_reg_data(void* p)
{
    int32_t tmp = (int32_t)p;                   // tmp is used just to remove a warning about conversion from pointer to smaller integer
    hal_encoder_t id = (hal_encoder_t)tmp;
    hal_encoder_internal_item_t* intitem = s_hal_encoder_theinternals.items[HAL_encoder_id2index(id)];
    
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
static hal_encoder_position_t s_hal_encoder_frame2position_t1(uint8_t* frame)
{
    uint32_t pos = 0;
    //pos = frame[0] | (frame[1] << 8) | (frame[2] << 16);
    //VALE formatting result
    pos = ((frame[0] & 0x7F) << 16) | (frame[1] << 8) | (frame[2] & 0xE0);
    pos = pos >> 5;
    return(pos);
}

// Formatting the result for encoder type 2
static hal_encoder_position_t s_hal_encoder_frame2position_t2(uint8_t* frame)
{
    uint32_t pos = 0;
    pos = ((frame[1] << 16) | (frame[2] << 8) | (frame[3]));
		pos = pos >> 4;
		pos = pos & 0x0FFFFF;
    return(pos);
}

#endif//HAL_USE_ENCODER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



