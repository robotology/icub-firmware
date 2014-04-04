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


/* @file       hal_spi.c
    @brief      This header file implements internals of a generic hal spi module
    @author     marco.accame@iit.it
    @date       10/29/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
#include "hal_middleware_interface.h"


#ifdef HAL_USE_SPI


//#warning --> BEWARE: HAL_USE_SPI supports only master mode, rx only, isr-mode, no dma, one frame at a time

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_sys.h"




#include "hl_bits.h"
#include "hl_fifo.h"
#include "hal_heap.h"
#include "hl_spi.h"
#include "hl_arch.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_spi.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_spi_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_spi_id2index(p)             ((uint8_t)(p))

#define HAL_spi_id2stmSPI(p)            (s_hal_spi_stmSPImap[HAL_spi_id2index(p)])



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hal_spi_cfg_t hal_spi_cfg_default = 
{ 
    .ownership                  = hal_spi_ownership_master,
    .direction                  = hal_spi_dir_rxonly,
    .activity                   = hal_spi_act_framebased,      
    .prescaler                  = hal_spi_prescaler_064,   
    .maxspeed                   = 0,           
    .sizeofframe                = 4,
    .capacityoftxfifoofframes   = 0,
    .capacityofrxfifoofframes   = 1,
    .dummytxvalue               = 0x00,
    .onframetransm              = NULL,
    .argonframetransm           = NULL,
    .onframereceiv              = NULL,
    .argonframereceiv           = NULL        
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

typedef struct
{
    hal_spi_cfg_t       config;
    uint8_t*            dummytxframe;
    uint8_t*            isrrxframe;
    uint8_t             isrrxcounter;
    hl_fifo_t*          fiforx;
    hal_spi_t           id;
    uint8_t             frameburstcountdown;
    hal_bool_t          isrisenabled;
} hal_spi_internal_item_t;


typedef struct
{
    uint32_t                    inittedmask;
    hal_spi_internal_item_t*    items[hal_spis_number];   
} hal_spi_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_spi_supported_is(hal_spi_t id);

static void s_hal_spi_initted_set(hal_spi_t id);
static hal_boolval_t s_hal_spi_initted_is(hal_spi_t id);


static hal_result_t s_hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg);

//static hal_bool_t s_hal_spi_is_speed_correct(hal_spi_prescaler_t prescaler, uint32_t speed);

static hal_result_t s_hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes);

static hal_bool_t s_hal_spi_config_is_correct(hal_spi_t id, const hal_spi_cfg_t *cfg);

static hl_spi_prescaler_t s_hal_spi_get_hl_prescaler(hal_spi_t id, const hal_spi_cfg_t* cfg);

static void s_hal_spi_read_isr(hal_spi_t id);

static void s_hal_spi_isr_init(hal_spi_t id, const hal_spi_cfg_t *cfg);
static void s_hal_spi_rx_isr_enable(hal_spi_t id);
static void s_hal_spi_rx_isr_disable(hal_spi_t id);

static void s_hal_spi_periph_enable(hal_spi_t id);
static void s_hal_spi_periph_disable(hal_spi_t id); 

static void s_hal_spi_prepare_hl_spi_map(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static SPI_TypeDef* const s_hal_spi_stmSPImap[] = { SPI1, SPI2, SPI3 };

  
    
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_spi_theinternals_t s_hal_spi_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};


// it must be defined in order to use hl_spi.
extern const hl_spi_mapping_t* hl_spi_map = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// this is not implemented in here.
// /** @fn			extern hal_result_t hal_spi_send(hal_spi_t id, uint8_t* txframe, uint8_t size)
//     @brief  	this function adds the frame to the transmission buffer.  
//                 if the id is configured having activity hal_spi_act_continuous, then the sendnow enables or disables the activity.
//                 the txframe, if its size is not zero, is sent as soon as a new frame starts. otherwise, frames are taken from the 
//                 txfifoofframes or are just zero data.
//     @param  	id	        the id
//     @param  	txframe 	    the frame to transmit
//     @return 	hal_res_OK or hal_res_NOK_generic on failure
//   */
// extern hal_result_t hal_spi_put(hal_spi_t id, uint8_t* txframe);
//
// extern hal_result_t hal_spi_on_frametransm_set(hal_spi_t id, hal_callback_t onframetransm, void* arg); 
// 
// used in master raw-mode only. the master sends the byte and it blocks until it receives another back.
// extern hal_result_t hal_spi_writeread(hal_spi_t id, uint8_t byte, uint8_t* readbyte);
//

extern uint32_t hal_spi_speedofbus_get(hal_spi_t id)
{   
    s_hal_spi_prepare_hl_spi_map(); 
    return(hl_spi_speedofbus_get((hl_spi_t)id));   
}

extern hal_result_t hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg)
{
    s_hal_spi_prepare_hl_spi_map();
    return(s_hal_spi_init(id, cfg));
}

extern hal_boolval_t hal_spi_initted_is(hal_spi_t id)
{   
    return(s_hal_spi_initted_is(id));
}

#if 0
extern hal_result_t hal_spi_writeread(hal_spi_t id, uint8_t byte, uint8_t* readbyte)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    hal_spi_cfg_t* cfg = NULL;
    volatile uint32_t timeout = 0;
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    
    
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    cfg = &intitem->config;    
        
    if(hal_spi_act_raw != cfg->activity)
    {
        return(hal_res_NOK_generic);
    }
    
    // before we write we need to wait for the spi has txe set
    timeout = s_hal_spi_timeout_flag;        
    while(RESET == SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE))
    {
        if(0 == (timeout--)) s_hal_spi_timeoutexpired();
    }         
    
    // ok. we send the byte
    SPI_I2S_SendData(SPIx, byte);
    
    // we need to wait for a reply from the slave
    timeout = s_hal_spi_timeout_flag;
    while(RESET == SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE))
    {
        if(0 == (timeout--)) s_hal_spi_timeoutexpired();
    }  
    
    // ok. here it is
    uint8_t rb = SPI_I2S_ReceiveData(SPIx);

    // if we want to retrieve it we copy into return value
    if(NULL != readbyte)   
    {
        *readbyte = rb;
    }  
  
    return(hal_res_OK);     
}
#endif

   
extern hal_result_t hal_spi_start(hal_spi_t id, uint8_t numberofframes)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    const uint8_t num2use = 1;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    } 
#endif    

#if 0    
    if(hal_spi_act_framebased != intitem->config.activity)
    {
        return(hal_res_NOK_generic);
    }    
#endif
      

    // protect ... may be not needed
    s_hal_spi_rx_isr_disable(id);       
    
    // tells how many frames to use
    intitem->frameburstcountdown = num2use;
    
    // reset isrrxframe and its counter  
    memset(intitem->isrrxframe, 0, intitem->config.sizeofframe);
    intitem->isrrxcounter = 0;
    
    s_hal_spi_rx_isr_enable(id);        // enable interrupt rx

    s_hal_spi_periph_enable(id);        // enable spi peripheral
    
	// in order to read data I have to write a dummy byte. the channel is configured in full duplex mode.
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	
	SPI_I2S_SendData(SPIx, intitem->config.dummytxvalue);    
    
    // ok, the isr shall read n frames of m bytes each and then issue a callback to me.
    // n = intitem->frameburstcountdown (=1 for encoder), and m = intitem->config.sizeofframe (=3 for encoder)
    
    return(hal_res_OK);    
}


extern hal_bool_t hal_spi_active_is(hal_spi_t id)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    //SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_true);
    }  
  
    return((0 == intitem->frameburstcountdown) ? (hal_true) : (hal_false));     
}


extern hal_result_t hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes)
{
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif    
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)    
    if(NULL == rxframe)
    {
        return(hal_res_NOK_nullpointer);
    }
#endif
    
    return(s_hal_spi_get(id, rxframe, remainingrxframes));       
}

// /** @fn			extern hal_result_t hal_spi_stop(hal_spi_t id)
//     @brief  	this function stops communication. 
//     @param  	id	            the id
//     @return 	hal_res_OK or hal_res_NOK_generic on failure
//   */
// extern hal_result_t hal_spi_stop(hal_spi_t id);
// extern hal_result_t hal_spi_stop(hal_spi_t id)
// {
//     if(hal_false == hal_spi_initted_is(id))
//     {
//         return(hal_res_NOK_generic);
//     }


//     // empty ...
//     #warning --> it must be called only if the spi has already stopped inside the isr
//     
//     //s_hal_spi_periph_dma_disable(id);
//            
//     return(hal_res_OK);    
// }


extern hal_result_t hal_spi_on_framereceiv_set(hal_spi_t id, hal_callback_t onframereceiv, void* arg)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)]; 

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    intitem->config.onframereceiv       = onframereceiv;
    intitem->config.argonframereceiv    = arg;
    
    return(hal_res_OK);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void SPI1_IRQHandler(void)
{
	s_hal_spi_read_isr(hal_spi1);
}


void SPI2_IRQHandler(void)
{
	s_hal_spi_read_isr(hal_spi2);
}


void SPI3_IRQHandler(void)
{
	s_hal_spi_read_isr(hal_spi3);
}

// ---- isr of the module: end ------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hal_spi_periph_enable(hal_spi_t id) 
{
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);                                   
    SPI_Cmd(SPIx, ENABLE);
}


static void s_hal_spi_periph_disable(hal_spi_t id) 
{
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);    // wait until it's free	                                   
    SPI_Cmd(SPIx, DISABLE);
}

static void s_hal_spi_read_isr(hal_spi_t id)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)]; 
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
     
    uint8_t rxbyte = SPI_I2S_ReceiveData(SPIx);
     
    if(intitem->isrrxcounter < intitem->config.sizeofframe)
    {
        intitem->isrrxframe[intitem->isrrxcounter] = rxbyte;
        intitem->isrrxcounter ++;
    }
     
    if(intitem->isrrxcounter == intitem->config.sizeofframe)
    {   // ok. the frame is finished
    
        // 1. stop spi 
        s_hal_spi_periph_disable(id);           // disable periph                                        	     
		s_hal_spi_rx_isr_disable(id);           // disable interrupt rx
        
        // set back to zero the frame burst
        intitem->frameburstcountdown = 0;
        // set rx counter to zero again
        intitem->isrrxcounter = 0;
        
        // copy the rxframe into the rx fifo
        
        if(hl_true == hl_fifo_full(intitem->fiforx))
        {
            hl_fifo_pop(intitem->fiforx);
        }
        hl_fifo_put(intitem->fiforx, intitem->isrrxframe);         
        
        // now manage the callback
        hal_callback_t onframereceiv = intitem->config.onframereceiv;
        void *arg = intitem->config.argonframereceiv;     
        if(NULL != onframereceiv)
        {
            onframereceiv(arg);
        }        
    }
    else
    {
        // transmit one dummy byte to trigger yet another reception
        SPI_I2S_SendData(SPIx, intitem->config.dummytxvalue);    
    }
 
}

static hal_boolval_t s_hal_spi_supported_is(hal_spi_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_spi__theboardconfig.supportedmask, HAL_spi_id2index(id)) );
}


static void s_hal_spi_initted_set(hal_spi_t id)
{
    hl_bits_word_bitset(&s_hal_spi_theinternals.inittedmask, HAL_spi_id2index(id));
}


static hal_boolval_t s_hal_spi_initted_is(hal_spi_t id)
{   
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_spi_theinternals.inittedmask, HAL_spi_id2index(id)));
}


static hal_result_t s_hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    
    if(NULL == cfg)
    {
        cfg = &hal_spi_cfg_default;
    }
       
    if(hal_true != s_hal_spi_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(hal_true == hal_spi_initted_is(id))
    {
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_spi_cfg_t)))
        {   // ok only if the previously used config is the same as the current one
            return(hal_res_OK);
        }
        else
        {
            return(hal_res_NOK_generic);
        }
    }       
 

    // mild verification of the config:    
    if(hal_false == s_hal_spi_config_is_correct(id, cfg))
    {
        return(hal_res_NOK_generic);
    }
    

    // acemor: very important info.
    // init the miso and mosi gpio before calling hw_init. 
    // because if the spi is already initted and it detects mosi or miso low it sets
    // register SPI_SR2.BUSY to 1, which makes things hang up.
    

    const hl_spi_advcfg_t hl_spi_advcfg_ems4rd =
    {   
        .SPI_Direction          = SPI_Direction_2Lines_FullDuplex,
        .SPI_Mode               = SPI_Mode_Master,                              // param
        .SPI_DataSize           = SPI_DataSize_8b, 
        .SPI_CPOL               = SPI_CPOL_High, //SPI_CPOL_High, //SPI_CPOL_Low, //SPI_CPOL_Low, // SPI_CPOL_High high is ok with display and also ok with isr mode
        .SPI_CPHA               = SPI_CPHA_1Edge, //SPI_CPHA_2Edge,
        .SPI_NSS                = SPI_NSS_Soft,
        .SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_64, // param: depends on speed. with 64 it it is 42/64 = 0.65625 mhz (or 1.3125 w/ 32)
        .SPI_FirstBit           = SPI_FirstBit_MSB, // SPI_FirstBit_MSB is ok with display, su stm3210c e' indifferente
        .SPI_CRCPolynomial      = 0x0007 // reset value
    };
    
    hl_spi_cfg_t hlcfg =
    {
        .mode       = hl_spi_mode_master,
        .prescaler  = hl_spi_prescaler_064, 
        .advcfg     = &hl_spi_advcfg_ems4rd                
    };
    hlcfg.mode = (hal_spi_ownership_master == cfg->ownership) ? (hl_spi_mode_master) : (hl_spi_mode_slave); 
    hlcfg.prescaler = s_hal_spi_get_hl_prescaler(id, cfg);
    
    
    hl_result_t r = hl_spi_init((hl_spi_t)id, &hlcfg);      // the gpio, the clock
    if(hl_res_OK != r)
    {
        return((hal_result_t)r);
    }
    hl_spi_enable((hl_spi_t)id);            // the SPI_Init()

    
    // --------------------------------------------------------------------------------------
    // init the spi internals data structure
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)] = hal_heap_new(sizeof(hal_spi_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }      
    
    // - the config   
    memcpy(&intitem->config, cfg, sizeof(hal_spi_cfg_t));
    hal_spi_cfg_t *usedcfg = NULL;
    usedcfg = &intitem->config;    
    

    // only frame-based
    
    // - the isr rx frame  
    intitem->isrrxframe = (uint8_t*)hal_heap_new(usedcfg->sizeofframe);    
    intitem->isrrxcounter = 0;
 
    // - the fifo of rx frames. but only if it is needed ... we dont need it if ...
    intitem->fiforx = hl_fifo_new(usedcfg->capacityofrxfifoofframes, usedcfg->sizeofframe, NULL);

    #warning MUSTTTTTTTTTTTTTTTTTTTTTT redefine hl_sys_heap_new() etc....
 
    // - the id
    intitem->id = id;

    // - frameburstcountdown
    intitem->frameburstcountdown = 0;

   
    // -- locked
    intitem->isrisenabled = hal_false;

        
    // now ... init the isr (only for frame-based activity)
    s_hal_spi_isr_init(id, usedcfg);

              
    // ok, it is initted
    s_hal_spi_initted_set(id);
         
    return(hal_res_OK);
}


static hal_bool_t s_hal_spi_config_is_correct(hal_spi_t id, const hal_spi_cfg_t *cfg)
{
    // mild verification of the config: ownership, activity, sizeofframe, direction, capacity of frames.
    // so far we support only master, framebase, rxonly   
    
//     if(hal_false == s_hal_spi_is_speed_correct(cfg->prescaler, cfg->maxspeed))
//     {
//         return(hal_false);
//     }
 
    if(hal_spi_ownership_master != cfg->ownership)
    {
        return(hal_false);
    }
        
    if(hal_spi_act_framebased != cfg->activity)
    {
        return(hal_false);
    }

    if(0 == cfg->sizeofframe)
    {
        return(hal_false);
    }    

    if(hal_spi_dir_rxonly != cfg->direction)  
    {
        return(hal_false);
    }   

    if(0 == cfg->capacityofrxfifoofframes)
    {
        return(hal_false);
    }    

    return(hal_true);
}


static hl_spi_prescaler_t s_hal_spi_get_hl_prescaler(hal_spi_t id, const hal_spi_cfg_t* cfg)
{
    hl_spi_prescaler_t hlprescaler = hl_spi_prescaler_064;
    
    if(hal_spi_prescaler_auto != cfg->prescaler)
    {   // we keep the specified prescaler
        hlprescaler = (hl_spi_prescaler_t)cfg->prescaler;
    }   
    else
    {   // we compute the prescaler according to the specified max speed and to the speed of bus       
        if(0 == cfg->maxspeed)
        {   // dont have specified a max speed: use a safe prescaler
            hlprescaler = hl_spi_prescaler_064;
        }
        else
        {   // must compute the hlprescaler to match the max speed.
            uint32_t busspeed = hl_spi_speedofbus_get((hl_spi_t)id);
            uint32_t div = busspeed / cfg->maxspeed;
            uint32_t rem = busspeed % cfg->maxspeed;
            if((div < 2) || ((2 == div) && (0 == rem)))
            {   // it is ok even if div is zero,
                hlprescaler = hl_spi_prescaler_002;
            }
            else if((div < 4) || ((4 == div) && (0 == rem)))
            {
                hlprescaler = hl_spi_prescaler_004;
            }
            else if((div < 8) || ((8 == div) && (0 == rem)))
            {
                hlprescaler = hl_spi_prescaler_008;
            }   
            else if((div < 16) || ((16 == div) && (0 == rem)))
            {
                hlprescaler = hl_spi_prescaler_016;
            }
            else if((div < 32) || ((32 == div) && (0 == rem)))
            {
                hlprescaler = hl_spi_prescaler_032;
            }    
            else if((div < 64) || ((64 == div) && (0 == rem)))
            {
                hlprescaler = hl_spi_prescaler_064;
            }
            else if((div < 128) || ((128 == div) && (0 == rem)))
            {
                hlprescaler = hl_spi_prescaler_128;
            }   
            else
            {
                hlprescaler = hl_spi_prescaler_256;
            }
        }
    }
    
    
    return(hlprescaler); 
}




static hal_result_t s_hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];

    return((hal_result_t)hl_fifo_get(intitem->fiforx, rxframe, remainingrxframes));      
}



static void s_hal_spi_isr_init(hal_spi_t id, const hal_spi_cfg_t *cfg)
{
//    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];

    IRQn_Type SPIx_IT_IRQn = SPI1_IRQn;
    hal_interrupt_priority_t priority = hal_int_priority02;
    
    switch(id)
    {
        case hal_spi1:  { SPIx_IT_IRQn = SPI1_IRQn; } break; 
        case hal_spi2:  { SPIx_IT_IRQn = SPI2_IRQn; } break;
        case hal_spi3:  { SPIx_IT_IRQn = SPI3_IRQn; } break;
        default:        { SPIx_IT_IRQn = SPI1_IRQn; } break; 
    }

    if(hal_int_priorityNONE != priority)
    {
        // enable irq in nvic
        hal_sys_irqn_priority_set(SPIx_IT_IRQn, priority);
        hal_sys_irqn_enable(SPIx_IT_IRQn);
    }
    
}

static void s_hal_spi_rx_isr_enable(hal_spi_t id)
{
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
}

static void s_hal_spi_rx_isr_disable(hal_spi_t id)
{
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
}

static void s_hal_spi_prepare_hl_spi_map(void)
{
    // we must initialise hl_spi_map w/ suited values. 
    // we have built hal_spi__theboardconfig to have the same layout, but we verify it anyway
    hl_VERIFYproposition(xxx, sizeof(hl_spi_mapping_t) == sizeof(hal_spi_boardconfig_t));
    hl_spi_map = (hl_spi_mapping_t*)&hal_spi__theboardconfig;
}

#endif//HAL_USE_SPI

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



