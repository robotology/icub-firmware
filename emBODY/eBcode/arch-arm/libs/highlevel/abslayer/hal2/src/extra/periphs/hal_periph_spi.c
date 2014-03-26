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

#warning --> to be reviewd with isr no dma

/* @file       hal_spi.c
    @brief      This header file implements internals of a generic hal spi module
    @author     marco.accame@iit.it
    @date       10/29/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"


#ifdef HAL_USE_SPI

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_brdcfg.h"
#include "hal_base_hid.h"
#include "hal_periph_gpio_hid.h"
#include "hal_dma.h"


#include "hl_bits.h"
#include "hal_utility_fifo.h"
#include "hal_heap.h"
#include "hl_spi.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_spi.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_periph_spi_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_spi_id2index(p)             ((uint8_t)(p))

#define HAL_spi_id2stmSPI(p)            (s_hal_spi_stmSPImap[HAL_spi_id2index(p)])

#undef HAL_MPU_SPI_DEBUG_MODE




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hal_spi_cfg_t hal_spi_cfg_default = 
{ 
    .ownership                  = hal_spi_ownership_master,
    .direction                  = hal_spi_dir_rxonly,
    .activity                   = hal_spi_act_framebased,      
    .prescaler                  = hal_spi_prescaler_128,    // it is 562500 on stm32f1 @ 72mhz fast bus, and 656250 on stm32f4 @ 84mhz fast bus
    .speed                      = hal_spi_speed_dontuse,           
    .sizeofframe                = 4,
    .capacityoftxfifoofframes   = 0,
    .capacityofrxfifoofframes   = 2,
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
    uint8_t*            dmatxframe;
    uint8_t*            dmarxframe;
    hal_utility_fifo_t  fifotx;
    hal_utility_fifo_t  fiforx;
    hal_spi_t           id;
    uint8_t             frameburstcountdown;
    hal_bool_t          forcestop;
    hal_bool_t          dmaisenabled;
} hal_spi_internal_item_t;


typedef struct
{
    uint8_t                     initted;
    hal_spi_internal_item_t*    items[hal_spis_number];   
} hal_spi_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#if 0  
static void s_hal_spi_scheduling_suspend(void);
static void s_hal_spi_scheduling_restart(void);
#endif

static hal_boolval_t s_hal_spi_supported_is(hal_spi_t id);

static void s_hal_spi_initted_set(hal_spi_t id);
static hal_boolval_t s_hal_spi_initted_is(hal_spi_t id);

static void s_hal_spi_set_dma_enabled(hal_spi_t id, hal_bool_t value);
static hal_bool_t s_hal_spi_is_dma_enabled(hal_spi_t id);

static hal_result_t s_hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg);

static hal_bool_t s_hal_spi_is_speed_correct(int32_t speed);


static hal_result_t s_hal_spi_put(hal_spi_t id, uint8_t* txframe);

static hal_result_t s_hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes);


static void s_hal_spi_periph_dma_enable(hal_spi_t id);
static void s_hal_spi_periph_dma_disable(hal_spi_t id);

static void s_hal_spi_periph_dma_isr_enable(hal_spi_t id);

static void s_hal_spi_periph_dma_isr_disable(hal_spi_t id);

static void s_hal_spi_dma_init(hal_spi_t id, const hal_spi_cfg_t *cfg);

static hal_result_t s_hal_spi_timeoutexpired(void);


static void s_hal_spi_dma_on_tranfer_done_tx(void* p);
static void s_hal_spi_dma_on_tranfer_done_rx(void* p);

static hl_spi_prescaler_t s_hal_spi_get_hl_prescaler(hal_spi_t id, const hal_spi_cfg_t* cfg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------


#if     defined(HAL_USE_CPU_FAM_STM32F1)
static SPI_TypeDef* const s_hal_spi_stmSPImap[] = { SPI1, SPI2, SPI3 };
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
static SPI_TypeDef* const s_hal_spi_stmSPImap[] = { SPI1, SPI2, SPI3 };
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 

static const uint32_t s_hal_spi_timeout_flag = 0x00010000;

#if     defined(HAL_USE_CPU_FAM_STM32F1)
    // on stm32f1 connectivity line: spi1rx/tx is on dma1chn2/chn3,  spi2rx/tx is on dma1chn4/chn5,  spi3rx/tx is on dma2chn1/chn2
    static const hal_dma_t s_hal_spi_dma_port2use_rx[hal_spis_number] = { hal_dma2, hal_dma4, hal_dma8 };
    static const hal_dma_t s_hal_spi_dma_port2use_tx[hal_spis_number] = { hal_dma3, hal_dma5, hal_dma9 };
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
    static const hal_dma_t s_hal_spi_dma_port2use_rx[hal_spis_number] = { hal_dmaNONE, hal_dma4, hal_dma1 };
    static const hal_dma_t s_hal_spi_dma_port2use_tx[hal_spis_number] = { hal_dmaNONE, hal_dma5, hal_dma6 };   
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
    
    
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_spi_theinternals_t s_hal_spi_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};


// it must be defined in order to use hl_spi.
extern const hl_spi_mapping_t* hl_spi_map = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_spi_init(const hal_spi_t id, const hal_spi_cfg_t *cfg)
{
    return(s_hal_spi_init(id, cfg));
}

extern hal_result_t hal_spi_raw_master_writeread(hal_spi_t id, uint8_t byte, uint8_t* readbyte)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    hal_spi_cfg_t* cfg = NULL;
    volatile uint32_t timeout = 0;
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    
    
    if(hal_false == hal_spi_hid_initted_is(id))
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



extern hal_result_t hal_spi_put(hal_spi_t id, uint8_t* txframe)
{ 
    if(hal_false == hal_spi_hid_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

#if 0    
    s_hal_spi_scheduling_suspend();
    if(hal_false == s_hal_spi_is_status_locked(id))
    {
        s_hal_spi_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_spi_status_locked_set(id, ); and unlock etc ....
#endif
       
    if(NULL == txframe)
    {
        return(hal_res_NOK_nullpointer);
    }
    

    return(s_hal_spi_put(id, txframe));
}


extern hal_result_t hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes)
{
    if(hal_false == hal_spi_hid_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

#if 0    
    s_hal_spi_scheduling_suspend();
    if(hal_false == s_hal_spi_is_status_locked(id))
    {
        s_hal_spi_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_spi_status_setlock(); and unlock etc ....
#endif
       
    if(NULL == rxframe)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    return(s_hal_spi_get(id, rxframe, remainingrxframes));       
}

   
extern hal_result_t hal_spi_start(hal_spi_t id, uint8_t lengthofburst)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    
    if(hal_false == hal_spi_hid_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

#if 0    
    s_hal_spi_scheduling_suspend();
    if(hal_false == s_hal_spi_is_status_locked(id))
    {
        s_hal_spi_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_spi_status_setlock(); and unlock etc ....
#endif
    
   
    if(hal_spi_act_framebased != intitem->config.activity)
    {
        return(hal_res_NOK_generic);
    }    
    
    uint8_t num2use = 0;
    
    if(hal_spi_ownership_slave == intitem->config.ownership)
    {   // slave ...
        num2use = 255;
    }
    else if((hal_spi_ownership_master == intitem->config.ownership) && (0 == lengthofburst))
    {   // master ... only the size of the tx buffer
        num2use = hal_utility_fifo_size(&intitem->fifotx);
        if(0 == num2use)
        {
            return(hal_res_NOK_generic);
        }
    }
    else
    {
        num2use = lengthofburst;
    }
    

    // protect
    s_hal_spi_periph_dma_disable(id);
    
    // tells how many frames to use
    intitem->frameburstcountdown = num2use;
    
    // load tx frame into dma
    if(hal_spi_dir_rxonly != intitem->config.direction)
    {
        if(hal_res_OK != hal_utility_fifo_get(&(intitem->fifotx), intitem->dmatxframe, NULL))
        {   // put the dummy one.
            memcpy(intitem->dmatxframe, intitem->dummytxframe, intitem->config.sizeofframe);
        } 
    }
        
    s_hal_spi_periph_dma_enable(id);
    
    return(hal_res_OK);    
}


extern hal_result_t hal_spi_stop(hal_spi_t id)
{
    if(hal_false == hal_spi_hid_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

#if 0    
    s_hal_spi_scheduling_suspend();
    if(hal_false == s_hal_spi_is_status_locked(id))
    {
        s_hal_spi_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_spi_status_setlock(); and unlock etc ....
#endif


    s_hal_spi_periph_dma_disable(id);
           
    return(hal_res_OK);    
}

extern hal_result_t hal_spi_on_framereceiv_set(hal_spi_t id, hal_callback_t onframereceiv, void* arg)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)]; 
    
    if(hal_false == hal_spi_hid_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

   
    
    intitem->config.onframereceiv       = onframereceiv;
    intitem->config.argonframereceiv    = arg;
    
    return(hal_res_OK);    
}


extern hal_result_t hal_spi_on_frametransm_set(hal_spi_t id, hal_callback_t onframetransm, void* arg)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)]; 
    
    if(hal_false == hal_spi_hid_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
   
    
    intitem->config.onframetransm       = onframetransm;
    intitem->config.argonframetransm    = arg;
    
    return(hal_res_OK);    
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

// none ...

// ---- isr of the module: end ------


extern hal_boolval_t hal_spi_hid_initted_is(hal_spi_t id)
{   
    return(s_hal_spi_initted_is(id));
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static hal_boolval_t s_hal_spi_supported_is(hal_spi_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(hal_brdcfg_spi__theconfig.supported_mask, HAL_spi_id2index(id)) );
}


static void s_hal_spi_initted_set(hal_spi_t id)
{
    hl_bits_byte_bitset(&s_hal_spi_theinternals.initted, HAL_spi_id2index(id));
}


static hal_boolval_t s_hal_spi_initted_is(hal_spi_t id)
{   
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_spi_theinternals.initted, HAL_spi_id2index(id)));
}


static void s_hal_spi_set_dma_enabled(hal_spi_t id, hal_bool_t value)
{
    s_hal_spi_theinternals.items[HAL_spi_id2index(id)]->dmaisenabled = value;
}


static hal_bool_t s_hal_spi_is_dma_enabled(hal_spi_t id)
{
    return(s_hal_spi_theinternals.items[HAL_spi_id2index(id)]->dmaisenabled);
}


static hal_result_t s_hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    hal_spi_cfg_t *usedcfg = NULL;
    uint8_t* tmpbuffer = NULL;
    
    if(NULL == cfg)
    {
        cfg = &hal_spi_cfg_default;
    }
    
    
    if(hal_true != s_hal_spi_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }

    if(hal_true == hal_spi_hid_initted_is(id))
    {
        return(hal_res_OK);
    }   

    // mild verification of the config: speed, activity, sizeofframe only
    
    if(hal_false == s_hal_spi_is_speed_correct((int32_t)cfg->speed))
    {
        return(hal_res_NOK_generic);
    }
 
    
    if(hal_spi_act_raw == cfg->activity)
    {
        // verify only ownership: it can be only master
        if(hal_spi_ownership_master != cfg->ownership)
        {
            return(hal_res_NOK_generic);
        }
    }
    else if(hal_spi_act_framebased == cfg->activity)
    {
        // verify only size of frame
        if(0 == cfg->sizeofframe)
        {
            return(hal_res_NOK_generic);
        }        
    }
    

    // acemor: very important info.
    // init the miso and mosi gpio before calling hw_init. 
    // because if the spi is already initted and it detects mosi or miso low it sets
    // register SPI_SR2.BUSY to 1, which makes things hang up.
    
    //s_hal_spi_hw_gpio_init(id, cfg->ownership);
    //s_hal_spi_hw_init(id);
    //s_hal_spi_hw_enable(id, cfg);
    
    hl_spi_cfg_t hlcfg =
    {
        .mode       = hl_spi_mode_master,
        .prescaler  = hl_spi_prescaler_064, 
        .advcfg     = NULL                
    };
    hlcfg.mode = (hal_spi_ownership_master == cfg->ownership) ? (hl_spi_mode_master) : (hl_spi_mode_slave); 
    hlcfg.prescaler = s_hal_spi_get_hl_prescaler(id, cfg);;

    // we must initialise hl_spi_map w/ suited values. 
    // we have built hal_brdcfg_spi__theconfig to have the same layout, but we verify it anyway
    hl_VERIFYproposition(xxx, sizeof(hl_spi_mapping_t) == sizeof(hal_spi_hid_brdcfg_t));
    hl_spi_map = (hl_spi_mapping_t*)&hal_brdcfg_spi__theconfig;
    
    
    hl_spi_init((hl_spi_t)id, &hlcfg);      // the gpio, the clock
    hl_spi_enable((hl_spi_t)id);            // the SPI_Init(), but not the DMA ...

    // the dma is here
    if(hal_spi_act_framebased == cfg->activity)
    {  
        SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);        
        // enable dma rx request
        SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, ENABLE);  
        // enable dma tx request
        SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);   
    }    
    
    
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
    usedcfg = &intitem->config;       
    memcpy(usedcfg, cfg, sizeof(hal_spi_cfg_t));
    
    if(hal_spi_act_raw == usedcfg->activity)
    {
        // the rest is not used ... however i initialise it anyway ...    
        intitem->dummytxframe           = NULL;
        intitem->dmatxframe             = NULL;
        intitem->dmarxframe             = NULL;
        intitem->dmatxframe             = NULL;
        hal_utility_fifo_init(&intitem->fifotx, 0, 0, NULL, NULL);
        hal_utility_fifo_init(&intitem->fiforx, 0, 0, NULL, NULL);
        intitem->id                   = id;
        intitem->frameburstcountdown    = 0;
        intitem->forcestop              = 0;        
        intitem->dmaisenabled           = hal_false;
    }
    else if(hal_spi_act_framebased == usedcfg->activity)
    {
    
        // - the dummy tx frame
        intitem->dummytxframe = (uint8_t*)hal_heap_new(usedcfg->sizeofframe);
#if     defined(HAL_MPU_SPI_DEBUG_MODE )
        uint8_t i;
        for(i=0; i<usedcfg->sizeofframe; i++) { intitem->dummytxframe[i] = usedcfg->dummytxvalue + i; }
#else   
        memset(intitem->dummytxframe, usedcfg->dummytxvalue, usedcfg->sizeofframe);
#endif  

        // - the dma tx frame   
        intitem->dmatxframe = (uint8_t*)hal_heap_new(usedcfg->sizeofframe);
        memcpy(intitem->dmatxframe, intitem->dummytxframe, usedcfg->sizeofframe);

        // - the dma rx frame  
        intitem->dmarxframe = (uint8_t*)hal_heap_new(usedcfg->sizeofframe);    
        
        
        // - the fifo of tx frames. but only if it is needed ... we dont need it if ...
        if((0 == usedcfg->capacityoftxfifoofframes) || (hal_spi_dir_rxonly == usedcfg->direction))
        {
            usedcfg->capacityoftxfifoofframes = 0;
            hal_utility_fifo_init(&intitem->fifotx, 0, 0, NULL, NULL);
        }
        else
        {
            tmpbuffer = (uint8_t*)hal_heap_new(usedcfg->capacityoftxfifoofframes*usedcfg->sizeofframe);
            hal_utility_fifo_init(&intitem->fifotx, usedcfg->capacityoftxfifoofframes, usedcfg->sizeofframe, tmpbuffer, NULL);
        }
     
        // - the fifo of rx frames. but only if it is needed ... we dont need it if ...
        if((0 == usedcfg->capacityofrxfifoofframes) || (hal_spi_dir_txonly == usedcfg->direction))
        {
            usedcfg->capacityofrxfifoofframes = 0;
            hal_utility_fifo_init(&intitem->fiforx, 0, 0, NULL, NULL);
        }
        else
        {
            tmpbuffer = (uint8_t*) hal_heap_new(usedcfg->capacityofrxfifoofframes*usedcfg->sizeofframe);
            hal_utility_fifo_init(&intitem->fiforx, usedcfg->capacityofrxfifoofframes, usedcfg->sizeofframe, tmpbuffer, NULL);
        }
     
        // - the id
        intitem->id = id;

        // - frameburstcountdown
        intitem->frameburstcountdown = 0;

        // - forcestop    
        intitem->forcestop = 0;
        
        // -- locked
        intitem->dmaisenabled = hal_false;
                
    }
    
    
    // now ... init the dma (only for frame-based activity)
    
    if(hal_spi_act_framebased == usedcfg->activity)
    {
        s_hal_spi_dma_init(id, usedcfg);
    }
    
           
    // ok, it is initted
    s_hal_spi_initted_set(id);
         
    return(hal_res_OK);
}

static hal_bool_t s_hal_spi_is_speed_correct(int32_t speed)
{
    if(hal_spi_speed_dontuse == speed)
    {
        return(hal_true);
    }
#if     defined(HAL_USE_CPU_FAM_STM32F1) 
    int32_t v = speed % (int32_t)hal_spi_speed_0562kbps;
    return((0 == v) ? (hal_true) : (hal_false));
#elif   defined(HAL_USE_CPU_FAM_STM32F4)  
    int32_t v = speed % (int32_t)hal_spi_speed_0652kbps;
    return((0 == v) ? (hal_true) : (hal_false));
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
    
}




static hl_spi_prescaler_t s_hal_spi_get_hl_prescaler(hal_spi_t id, const hal_spi_cfg_t* cfg)
{
    hl_spi_prescaler_t hlprescaler = hl_spi_prescaler_064;
    
    return(hlprescaler);
    
    if((hal_spi_speed_dontuse == cfg->speed) && (hal_spi_prescaler_dontuse == cfg->prescaler))
    {
         hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_spi_init(): use one of speed or prescaler");
    }
    if((hal_spi_speed_dontuse != cfg->speed) && (hal_spi_prescaler_dontuse != cfg->prescaler))
    {
         hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_spi_init(): use either speed or prescaler, not both");
    }
    
    if(hal_spi_prescaler_dontuse != cfg->prescaler)
    {   // ok, we have the prescaler. need only to convert it in stm32fx format
        
        switch(cfg->prescaler)
        {   // remember that hal_spi_prescaler_xxx is referred to high speed bus, 
            // and prescaler_stm32fx to high speed bus for spi1 but to low speed for spi2 and spi3
            case hal_spi_prescaler_004: 
            { 
                hlprescaler = hl_spi_prescaler_004;    
            } break;
            case hal_spi_prescaler_008: 
            {
                hlprescaler = hl_spi_prescaler_008;
            } break;
            case hal_spi_prescaler_016: 
            {
                hlprescaler = hl_spi_prescaler_016;  
            } break;
            case hal_spi_prescaler_032: 
            { 
                hlprescaler = hl_spi_prescaler_032;   
            } break;
            case hal_spi_prescaler_064: 
            {
                hlprescaler = hl_spi_prescaler_064;   
            } break;
            case hal_spi_prescaler_128: 
            {
                hlprescaler = hl_spi_prescaler_128;  
            } break;
            case hal_spi_prescaler_256: 
            {
               hlprescaler = hl_spi_prescaler_256;  
            } break;
            default:                    
            {
                hlprescaler = hl_spi_prescaler_256; 
            } break;
        }        
    }    
    else if(hal_spi_speed_dontuse != cfg->speed)
    {   // ok, use the speed to compute the prescaler in stm32fx format
        
        // remember that hal_spi_prescaler_xxx is referred to high speed bus, 
        // and prescaler_stm32fx to high speed bus for spi1 but to low speed for spi2 and spi3
        uint16_t factor = (hal_spi1 == id) ? (hal_brdcfg_cpu__theconfig.speeds.fastbus / cfg->speed) : (hal_brdcfg_cpu__theconfig.speeds.slowbus / cfg->speed);
        
        switch(factor)
        {
            case 2:     hlprescaler = hl_spi_prescaler_002;  break;  
            case 4:     hlprescaler = hl_spi_prescaler_004;  break;
            case 8:     hlprescaler = hl_spi_prescaler_008;  break;
            case 16:    hlprescaler = hl_spi_prescaler_016;  break;
            case 32:    hlprescaler = hl_spi_prescaler_032;  break;
            case 64:    hlprescaler = hl_spi_prescaler_064;  break;
            case 128:   hlprescaler = hl_spi_prescaler_128;  break;
            case 256:   hlprescaler = hl_spi_prescaler_256;  break;
            default:    hlprescaler = hl_spi_prescaler_256;  break;
        }        
    } 


    return(hlprescaler);    
}



static hal_result_t s_hal_spi_timeoutexpired(void)
{
    hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "timeout error in spi raw operations");
    return(hal_res_NOK_generic);
}

#if 0  
static void s_hal_spi_scheduling_suspend(void)
{
    hal_base_osal_scheduling_suspend();
}

static void s_hal_spi_scheduling_restart(void)
{
    hal_base_osal_scheduling_restart();
}
#endif

static hal_result_t s_hal_spi_put(hal_spi_t id, uint8_t* txframe)
{
    hal_result_t res = hal_res_NOK_generic;
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    hal_spi_cfg_t* cfg = &intitem->config;
    
    
    if(hal_spi_act_framebased != cfg->activity)
    {
        return(hal_res_NOK_generic);
    }  
    
    if(hal_spi_dir_rxonly != cfg->direction)
    {
        // disable so that we can change a data structure which is used by the isr

        hal_bool_t dmaisenabled = s_hal_spi_is_dma_enabled(id);
        if(hal_true == dmaisenabled)
        {
            s_hal_spi_periph_dma_isr_disable(id);
        }
        
        res =  hal_utility_fifo_put(&intitem->fifotx, txframe);
        
        if(hal_true == dmaisenabled)
        {
            s_hal_spi_periph_dma_isr_enable(id);
        }
    }
           
//    if(hal_true == sendnow)
//    {
//        // enable again
//        s_hal_spi_periph_dma_enable(id);   
//    }
    
    return(res);
}

static hal_result_t s_hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes)
{
    hal_result_t res = hal_res_NOK_generic;
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    hal_spi_cfg_t* cfg = &intitem->config;
    
    
    if(hal_spi_act_framebased != cfg->activity)
    {
        return(hal_res_NOK_generic);
    }      

    if(hal_spi_dir_txonly != intitem->config.direction)
    {
        
        hal_bool_t dmaisenabled = s_hal_spi_is_dma_enabled(id);
        if(hal_true == dmaisenabled)
        {
            s_hal_spi_periph_dma_isr_disable(id);
        } 
        
        res =  hal_utility_fifo_get(&intitem->fiforx, rxframe, remainingrxframes);
        
        if(hal_true == dmaisenabled)
        {
            s_hal_spi_periph_dma_isr_enable(id);
        }   
    }

    
//    if(NULL != size)
//    {
//        *size = (hal_res_OK == res) ? (s_hal_spi_internals[HAL_spi_id2index(id)].config.sizeofframe) : (0);
//    }
        
        // enable again
//        s_hal_spi_isr_enable(id);   
    
    return(res);    
}


static void s_hal_spi_periph_dma_isr_enable(hal_spi_t id)
{
    hal_dma_isr_enable(s_hal_spi_dma_port2use_rx[HAL_spi_id2index(id)]);
    hal_dma_isr_enable(s_hal_spi_dma_port2use_tx[HAL_spi_id2index(id)]);
}

static void s_hal_spi_periph_dma_isr_disable(hal_spi_t id)
{  
    hal_dma_isr_disable(s_hal_spi_dma_port2use_rx[HAL_spi_id2index(id)]);
    hal_dma_isr_disable(s_hal_spi_dma_port2use_tx[HAL_spi_id2index(id)]);
}

static void s_hal_spi_periph_dma_enable(hal_spi_t id)
{
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id); 
    SPI_Cmd(SPIx, ENABLE);  
    
    s_hal_spi_set_dma_enabled(id, hal_true);

    hal_dma_enable(s_hal_spi_dma_port2use_rx[HAL_spi_id2index(id)]);
    hal_dma_enable(s_hal_spi_dma_port2use_tx[HAL_spi_id2index(id)]);
}

static void s_hal_spi_periph_dma_disable(hal_spi_t id)
{
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id); 
    SPI_Cmd(SPIx, DISABLE);    
    
    hal_dma_disable(s_hal_spi_dma_port2use_rx[HAL_spi_id2index(id)]);
    hal_dma_disable(s_hal_spi_dma_port2use_tx[HAL_spi_id2index(id)]);
    
    s_hal_spi_set_dma_enabled(id, hal_false);
}




static void s_hal_spi_dma_init(hal_spi_t id, const hal_spi_cfg_t *cfg)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    
    hal_dma_cfg_t s_hal_spi_dma_cfg =
    {
        .transfer               = hal_dma_transfer_mem2per,     // it changes
        .mode                   = hal_dma_mode_oneshot,         // fixed
        .intpriority            = hal_int_priority06,           // fixed
        .datasize               = cfg->sizeofframe,             // fixed
        .source                 = 0,                            // it changes
        .destin                 = 0,                            // it changes
        .cbk_on_transfer_done   = NULL,                         // it changes
        .arg_on_transfer_done   = NULL                          // it changes
    };
    
    
    s_hal_spi_dma_cfg.transfer                  = hal_dma_transfer_per2mem;
    s_hal_spi_dma_cfg.source                    = (void*)&s_hal_spi_stmSPImap[HAL_spi_id2index(id)]->DR;
    s_hal_spi_dma_cfg.destin                    = intitem->dmarxframe;
    s_hal_spi_dma_cfg.cbk_on_transfer_done      = s_hal_spi_dma_on_tranfer_done_rx;
    s_hal_spi_dma_cfg.arg_on_transfer_done      = intitem;
    hal_dma_init(s_hal_spi_dma_port2use_rx[HAL_spi_id2index(id)], &s_hal_spi_dma_cfg);

    // init dma for tx
    s_hal_spi_dma_cfg.transfer                  = hal_dma_transfer_mem2per;
//    s_hal_spi_dma_cfg.source                    = intitem->dmatxframe;
//    s_hal_spi_dma_cfg.destin                    = &s_hal_spi_stmSPImap[HAL_spi_id2index(id)]->DR;  
    s_hal_spi_dma_cfg.source                    = (void*)&s_hal_spi_stmSPImap[HAL_spi_id2index(id)]->DR;
    s_hal_spi_dma_cfg.destin                    = intitem->dmatxframe;    
    s_hal_spi_dma_cfg.cbk_on_transfer_done      = s_hal_spi_dma_on_tranfer_done_tx;  
    s_hal_spi_dma_cfg.arg_on_transfer_done      = intitem;    
    hal_dma_init(s_hal_spi_dma_port2use_tx[HAL_spi_id2index(id)], &s_hal_spi_dma_cfg);    
}



static void s_hal_spi_dma_on_tranfer_done_rx(void* p)
{
    hal_spi_internal_item_t* intitem = (hal_spi_internal_item_t*)p;
    
    // 1. copy the rx frame into fifo  
    
    if(hal_spi_dir_txonly != intitem->config.direction)
    {   
        if(hal_true == hal_utility_fifo_full(&(intitem->fiforx)))
        {
            hal_utility_fifo_pop(&(intitem->fiforx));
        }
        hal_utility_fifo_put(&(intitem->fiforx), intitem->dmarxframe); 
    }

    // 2. verify if we need to stop
    hal_bool_t stopit = hal_false;
    
    if(hal_spi_ownership_slave == intitem->config.ownership)
    {
        stopit = hal_false;
    }    
    else if(hal_spi_ownership_master == intitem->config.ownership)
    {
        if(255 == intitem->frameburstcountdown)
        {
            stopit = hal_false;
        }
        else if(0 == intitem->frameburstcountdown)
        {
            stopit = hal_true;
        }
        else
        {
            intitem->frameburstcountdown --;
            
            if(0 == intitem->frameburstcountdown)
            {
                stopit = hal_true;
            }
            else
            {
                stopit = hal_false;
            }
        }       
    }

    
    if(hal_true == stopit)
    {
        // stop
    }
    else
    {         
        if(hal_spi_dir_rxonly != intitem->config.direction)
        {
            // copy into txbuffer a new frame
            if(hal_res_OK != hal_utility_fifo_get(&(intitem->fifotx), intitem->dmatxframe, NULL))
            {   // put the dummy one.
                memcpy(intitem->dmatxframe, intitem->dummytxframe, intitem->config.sizeofframe);
            }      
        }    
        
        // retrigger tx
        hal_dma_retrigger(s_hal_spi_dma_port2use_tx[HAL_spi_id2index(intitem->id)]);
        
        // retrigger rx
        hal_dma_retrigger(s_hal_spi_dma_port2use_rx[HAL_spi_id2index(intitem->id)]);                     
    } 

    if(hal_spi_dir_txonly != intitem->config.direction)
    {
        // alert about a reception
        hal_callback_t onframereceiv = intitem->config.onframereceiv;
        if(NULL != onframereceiv)
        {
            onframereceiv(intitem->config.argonframereceiv);
        }
    }
       
}

static void s_hal_spi_dma_on_tranfer_done_tx(void* p)
{
    hal_spi_internal_item_t* intitem = (hal_spi_internal_item_t*)p;
    
    hal_dma_dontdisable(s_hal_spi_dma_port2use_tx[HAL_spi_id2index(intitem->id)]);
       
    // alert about a transmission done
    if(hal_spi_dir_rxonly != intitem->config.direction)
    {
        hal_callback_t onframetransm = intitem->config.onframetransm;
        if(NULL != onframetransm)
        {
            onframetransm(intitem->config.argonframetransm);
        }    
    }
}



#endif//HAL_USE_SPI

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



