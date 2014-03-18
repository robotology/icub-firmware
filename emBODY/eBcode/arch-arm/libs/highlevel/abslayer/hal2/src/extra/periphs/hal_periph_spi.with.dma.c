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

#ifdef HAL_USE_PERIPH_SPI

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

static void s_hal_spi_hw_init(hal_spi_t id);
static void s_hal_spi_hw_gpio_init(hal_spi_t id, hal_spi_ownership_t ownership);
static void s_hal_spi_hw_enable(hal_spi_t id, const hal_spi_cfg_t* cfg);

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

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const SPI_InitTypeDef s_hal_spi_stm32_cfg =
{
    .SPI_Direction                  = SPI_Direction_2Lines_FullDuplex,
    .SPI_Mode                       = SPI_Mode_Master,                              // param
    .SPI_DataSize                   = SPI_DataSize_8b, 
    .SPI_CPOL                       = SPI_CPOL_High, //SPI_CPOL_High, //SPI_CPOL_Low, //SPI_CPOL_Low, // SPI_CPOL_High high is ok with display and also ok with isr mode
    .SPI_CPHA                       = SPI_CPHA_1Edge, //SPI_CPHA_2Edge,
    .SPI_NSS                        = SPI_NSS_Soft,
    .SPI_BaudRatePrescaler          = SPI_BaudRatePrescaler_64,                      // param: depends on speed
    .SPI_FirstBit                   = SPI_FirstBit_MSB, // SPI_FirstBit_MSB is ok with display, su stm3210c e' indifferente
    .SPI_CRCPolynomial              = 0x0007 // reset value
};

//#warning --> display usa SPI_CPHA_2Edge ma prima avevo messo SPI_CPHA_1Edge
#warning HAL-WIP --> verify if the internal s_hal_spi_stm32_cfg used for spi stm32fx config is the same used in hal1 for encoders

#if     defined(HAL_USE_CPU_FAM_STM32F1)
static SPI_TypeDef* const s_hal_spi_stmSPImap[] = { SPI1, SPI2, SPI3 };
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
static SPI_TypeDef* const s_hal_spi_stmSPImap[] = { SPI1, SPI2, SPI3 };
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 

static const uint32_t s_hal_spi_hw_rcc[] = { RCC_APB2Periph_SPI1, RCC_APB1Periph_SPI2, RCC_APB1Periph_SPI3 };

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
    
    
// #if     defined(HAL_USE_CPU_FAM_STM32F1)
//     #warning --> use low speed bus values .. defined in brdcfg
//     const uint32_t HIGHSPEED = 72000000;
//     const uint32_t LOWSPEED  = 36000000;
// #elif   defined(HAL_USE_CPU_FAM_STM32F4)
//         TODO ...
// #endif
    
    
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_spi_theinternals_t s_hal_spi_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};



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
    
    s_hal_spi_hw_gpio_init(id, cfg->ownership);
    s_hal_spi_hw_init(id);
    s_hal_spi_hw_enable(id, cfg);
    
    
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
        
        
        // - the fifo of tx frames. but only if it needed ... we dont need it if ...
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
     
        // - the fifo of rx frames. but only if it needed ... we dont need it if ...
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


static void s_hal_spi_hw_init(hal_spi_t id)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)

    //uint32_t RCC_APB1Periph_SPIx = (hal_spi1 == id) ? (RCC_APB1Periph_SPI1) : (RCC_APB1Periph_SPI2); RCC_APB1Periph_SPI3
        
    
    if(hal_spi1 == id)
    {
        // spi periph clock enable
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
        
        // the reset and exit from reset is also done by SPI_I2S_DeInit() ..... thus it can be removed from here
        // reset spi periph
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
        
        // release reset 
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);        
    }
    else
    {
        uint32_t RCC_APB1Periph_SPIx = s_hal_spi_hw_rcc[HAL_spi_id2index(id)];
        
        // spi periph clock enable
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPIx, ENABLE);
        
        // the reset and exit from reset is also done by SPI_I2S_DeInit() ..... thus it can be removed from here
        // reset spi periph
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPIx, ENABLE);
        
        // release reset 
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPIx, DISABLE);        
    }
    
// //    // system configuration controller clock
//     #warning --> in stm32f4 removed "RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);" from spi_hw_init() and it still works....
// //    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//     
//     // spi periph clock enable
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPIx, ENABLE);
//     
//     // reset spi periph
//     RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPIx, ENABLE);
//     
//     // release reset 
//     RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPIx, DISABLE);

#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
}


static void s_hal_spi_hw_gpio_init(hal_spi_t id, hal_spi_ownership_t ownership)
{
    
#if     defined(HAL_USE_CPU_FAM_STM32F1)

    static const GPIO_InitTypeDef  s_hal_spi_sckmosi_master_altcfg  =
    {
        .GPIO_Pin       = 0,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_Mode      = GPIO_Mode_AF_PP, // il PP serve per spi del display3 .... GPIO_Mode_AF_OD,
    }; 

    static const GPIO_InitTypeDef  s_hal_spi_miso_master_altcfg  =
    {
        .GPIO_Pin       = 0,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_Mode      = GPIO_Mode_IN_FLOATING, //GPIO_Mode_IPD, // oppure ... GPIO_Mode_IN_FLOATING?
    };     
    
    static const GPIO_InitTypeDef  s_hal_spi_sckmosi_slave_altcfg  =
    {
        .GPIO_Pin       = 0,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_Mode      = GPIO_Mode_IN_FLOATING, //GPIO_Mode_IPD, // oppure ... GPIO_Mode_IN_FLOATING,
    }; 

    static const GPIO_InitTypeDef  s_hal_spi_miso_slave_altcfg  =
    {
        .GPIO_Pin       = 0,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_Mode      = GPIO_Mode_AF_PP, 
    };         
    

    // 1. prepare af.
    // for spi1 (sck, miso, mosi): no-remap if it is (PA5, PA6, PA7). GPIO_Remap_SPI1 if it is (PB3, PB4, PB5).
    // for spi2 (sck, miso, mosi): no remap if it is (PB13, PB14, PB15).
    // for spi3 (sck, miso, mosi): no-remap if it is (PB3, PB4, PB5). GPIO_Remap_SPI3 if it is (PC10, PC11, PC12).
    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t found = hal_false;

    hal_gpio_port_t portsck  = hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_id2index(id)].gpio.port;
    hal_gpio_pin_t  pinsck   = hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_id2index(id)].gpio.pin;      
    hal_gpio_port_t portmiso = hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_id2index(id)].gpio.port;
    hal_gpio_pin_t  pinmiso  = hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_id2index(id)].gpio.pin;
    hal_gpio_port_t portmosi = hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_id2index(id)].gpio.port;
    hal_gpio_pin_t  pinmosi  = hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_id2index(id)].gpio.pin;    
     
    
    if(hal_spi1 == id)
    {        
        if((hal_gpio_portA == portsck) && (hal_gpio_pin5 == pinsck) && (hal_gpio_portA == portmiso) && (hal_gpio_pin6 == pinmiso) && (hal_gpio_portA == portmosi) && (hal_gpio_pin7 == pinmosi))
        {   // PA5, PA6, PA7
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }
        else if((hal_gpio_portB == portsck) && (hal_gpio_pin3 == pinsck) && (hal_gpio_portB == portmiso) && (hal_gpio_pin4 == pinmiso) && (hal_gpio_portB == portmosi) && (hal_gpio_pin5 == pinmosi))
        {   // PB3, PB4, PB5
            afname = GPIO_Remap_SPI1;       afmode = ENABLE;                    found = hal_true;
        }               
    }
    else if(hal_spi2 == id)
    {
        if((hal_gpio_portB == portmiso) && (hal_gpio_portB == portmosi) && (hal_gpio_portB == portsck) && (hal_gpio_pin14 == pinmiso) && (hal_gpio_pin15 == pinmosi) && (hal_gpio_pin13 == pinsck))
        {   // PB13, PB14, PB15
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }        
    }
    else if(hal_spi3 == id)
    {
        if((hal_gpio_portB == portsck) && (hal_gpio_pin3 == pinsck) && (hal_gpio_portB == portmiso) && (hal_gpio_pin4 == pinmiso) && (hal_gpio_portB == portmosi) && (hal_gpio_pin5 == pinmosi))
        {   // PB3, PB4, PB5
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }
        else if((hal_gpio_portC == portsck) && (hal_gpio_pin10 == pinsck) && (hal_gpio_portC == portmiso) && (hal_gpio_pin11 == pinmiso) && (hal_gpio_portC == portmosi) && (hal_gpio_pin12 == pinmosi))
        {   // PC10, PC11, PC12
            afname = GPIO_Remap_SPI3;       afmode = ENABLE;                    found = hal_true;
        }       
    }
    
    if(hal_false == found)
    {
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_spi_init(): incorrect pin mapping");
    }

    hal_gpio_altcfg_t hal_spi_sck_altcfg;
    hal_gpio_altcfg_t hal_spi_miso_altcfg;
    hal_gpio_altcfg_t hal_spi_mosi_altcfg;
    hal_gpio_cfg_t config;
    
    // prepare the altcfg for sck, miso, mosi pins
    if(hal_spi_ownership_master == ownership)
    {
        memcpy(&hal_spi_sck_altcfg, &s_hal_spi_sckmosi_master_altcfg, sizeof(hal_gpio_altcfg_t));
        memcpy(&hal_spi_miso_altcfg, &s_hal_spi_miso_master_altcfg, sizeof(hal_gpio_altcfg_t));
        memcpy(&hal_spi_mosi_altcfg, &s_hal_spi_sckmosi_master_altcfg, sizeof(hal_gpio_altcfg_t));
    }
    else
    {
        memcpy(&hal_spi_sck_altcfg, &s_hal_spi_sckmosi_slave_altcfg, sizeof(hal_gpio_altcfg_t));
        memcpy(&hal_spi_miso_altcfg, &s_hal_spi_miso_slave_altcfg, sizeof(hal_gpio_altcfg_t));
        memcpy(&hal_spi_mosi_altcfg, &s_hal_spi_sckmosi_slave_altcfg, sizeof(hal_gpio_altcfg_t));        
    }
    
    hal_spi_sck_altcfg.afname = hal_spi_miso_altcfg.afname = hal_spi_mosi_altcfg.afname = afname;
    hal_spi_sck_altcfg.afname = hal_spi_miso_altcfg.afmode = hal_spi_mosi_altcfg.afmode = afmode;
    
    // configure miso, mosi, sck pins  
    memcpy(&config, &hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_id2index(id)].config, sizeof(hal_gpio_cfg_t));
    config.altcfg = &hal_spi_sck_altcfg;
    hal_gpio_init(hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_id2index(id)].gpio, &config);
    
    memcpy(&config, &hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_id2index(id)].config, sizeof(hal_gpio_cfg_t));
    config.altcfg = &hal_spi_miso_altcfg;
    hal_gpio_init(hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_id2index(id)].gpio, &config);    

    memcpy(&config, &hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_id2index(id)].config, sizeof(hal_gpio_cfg_t));
    config.altcfg = &hal_spi_mosi_altcfg;
    hal_gpio_init(hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_id2index(id)].gpio, &config);    
    
//     hal_gpio_configure(hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_id2index(id)], &hal_spi_sck_altcfg);
//     hal_gpio_configure(hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_id2index(id)], &hal_spi_miso_altcfg);    
//     hal_gpio_configure(hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_id2index(id)], &hal_spi_mosi_altcfg);


#elif   defined(HAL_USE_CPU_FAM_STM32F4)    

    static const GPIO_InitTypeDef  s_hal_spi_misomosisck_altcfg  =
    {
        .GPIO_Pin       = 0,
        .GPIO_Mode      = GPIO_Mode_AF,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_OType     = GPIO_OType_PP,
        .GPIO_PuPd      = GPIO_PuPd_DOWN
    }; 
    
    
    // 1. prepare af.
    // for spi1 (sck, miso, mosi): sck -> PA5, PB3.         miso -> PA6, PB4.           mosi -> PA7, PB5.
    // for spi2 (sck, miso, mosi): sck -> PB10, PB13, PI1.  miso -> PAB14, PC2, PI2.    mosi -> PB15, PC3, PI3. 
    // for spi3 (sck, miso, mosi): sck -> PB3, PC10.        miso -> PB4, PC11.          mosi -> PB5, PC12.

    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t foundsck  = hal_false;
    hal_bool_t foundmiso = hal_false;
    hal_bool_t foundmosi = hal_false;


    hal_gpio_port_t portsck  = hal_brdcfg_spi__theconfig.gpiomap[HAL_spi_id2index(id)].sck.gpio.port;
    hal_gpio_pin_t  pinsck   = hal_brdcfg_spi__theconfig.gpiomap[HAL_spi_id2index(id)].sck.gpio.pin;        
    hal_gpio_port_t portmiso = hal_brdcfg_spi__theconfig.gpiomap[HAL_spi_id2index(id)].miso.gpio.port;
    hal_gpio_pin_t  pinmiso  = hal_brdcfg_spi__theconfig.gpiomap[HAL_spi_id2index(id)].miso.gpio.pin;
    hal_gpio_port_t portmosi = hal_brdcfg_spi__theconfig.gpiomap[HAL_spi_id2index(id)].mosi.gpio.port;
    hal_gpio_pin_t  pinmosi  = hal_brdcfg_spi__theconfig.gpiomap[HAL_spi_id2index(id)].mosi.gpio.pin;       
   
    
    if(hal_spi1 == id)
    {   
        afname = GPIO_AF_SPI1;      afmode = ENABLE;
        
        if( ((hal_gpio_portA == portsck) && (hal_gpio_pin5 == pinsck))  ||
            ((hal_gpio_portB == portsck) && (hal_gpio_pin3 == pinsck))  )
        {   // PA5, PB3
            foundsck = hal_true;
        }  
        
        if( ((hal_gpio_portA == portmiso) && (hal_gpio_pin6 == pinmiso))  ||
            ((hal_gpio_portB == portmiso) && (hal_gpio_pin4 == pinmiso))  )
        {   // PA6, PB4
            foundmiso = hal_true;
        }
        
        if( ((hal_gpio_portA == portmosi) && (hal_gpio_pin7 == pinmosi))  ||
            ((hal_gpio_portB == portmosi) && (hal_gpio_pin5 == pinmosi))  )
        {   // PA7, PB5
            foundmosi = hal_true;
        }
              
    }
    else if(hal_spi2 == id)
    {   
        afname = GPIO_AF_SPI2;      afmode = ENABLE;
        
        if( ((hal_gpio_portB == portsck) && (hal_gpio_pin10 == pinsck))  ||
            ((hal_gpio_portB == portsck) && (hal_gpio_pin13 == pinsck))  ||
            ((hal_gpio_portI == portsck) && (hal_gpio_pin1 == pinsck)) )
        {   // PB10, PB13, PI1
            foundsck = hal_true;
        }  
        
        if( ((hal_gpio_portB == portmiso) && (hal_gpio_pin14 == pinmiso))  ||
            ((hal_gpio_portC == portmiso) && (hal_gpio_pin2 == pinmiso))  ||
            ((hal_gpio_portI == portmiso) && (hal_gpio_pin2 == pinmiso)) )
        {   // PAB14, PC2, PI2
            foundmiso = hal_true;
        }
        
        if( ((hal_gpio_portB == portmosi) && (hal_gpio_pin15 == pinmosi))  ||
            ((hal_gpio_portC == portmosi) && (hal_gpio_pin3 == pinmosi))  ||
            ((hal_gpio_portI == portmosi) && (hal_gpio_pin3 == pinmosi)) )
        {   // PB15, PC3, PI3
            foundmosi = hal_true;
        }
                
    }
    else if(hal_spi3 == id)
    {
        afname = GPIO_AF_SPI3;      afmode = ENABLE;
        
        if( ((hal_gpio_portB == portsck) && (hal_gpio_pin3 == pinsck))  ||
            ((hal_gpio_portC == portsck) && (hal_gpio_pin10 == pinsck))  )
        {   // PB3, PC10
            foundsck = hal_true;
        }   
        
        if( ((hal_gpio_portB == portmiso) && (hal_gpio_pin4 == pinmiso))  ||
            ((hal_gpio_portC == portmiso) && (hal_gpio_pin11 == pinmiso))  )
        {   // PB4, PC11
            foundmiso = hal_true;
        }
        
        if( ((hal_gpio_portB == portmosi) && (hal_gpio_pin5 == pinmosi))  ||
            ((hal_gpio_portC == portmosi) && (hal_gpio_pin12 == pinmosi))  )
        {   // PB5, PC12
            foundmosi = hal_true;
        }    
    
    }
    
    if((hal_false == foundsck) || (hal_false == foundmiso) || (hal_false == foundmosi))
    {
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_spi_init(): incorrect pin mapping");
    }
    
    
    hal_gpio_cfg_t hal_spi_sck_cfg;
    hal_gpio_cfg_t hal_spi_miso_cfg;
    hal_gpio_cfg_t hal_spi_mosi_cfg;
    hal_gpio_altcfg_t hal_spi_sck_altcfg;   
    hal_gpio_altcfg_t hal_spi_miso_altcfg;
    hal_gpio_altcfg_t hal_spi_mosi_altcfg;

    
    // prepare the altcfg for sck miso mosi pins
    memcpy(&hal_spi_sck_altcfg.gpioext, &s_hal_spi_misomosisck_altcfg, sizeof(GPIO_InitTypeDef));
    memcpy(&hal_spi_miso_altcfg.gpioext, &s_hal_spi_misomosisck_altcfg, sizeof(GPIO_InitTypeDef));
    memcpy(&hal_spi_mosi_altcfg.gpioext, &s_hal_spi_misomosisck_altcfg, sizeof(GPIO_InitTypeDef));
    hal_spi_sck_altcfg.afname = hal_spi_miso_altcfg.afname = hal_spi_mosi_altcfg.afname = afname;
    hal_spi_sck_altcfg.afmode = hal_spi_miso_altcfg.afmode = hal_spi_mosi_altcfg.afmode = afmode;
    
    // the gpiocfgs.
    hal_spi_sck_cfg.dir     = hal_gpio_dirALT;
    hal_spi_sck_cfg.speed   = hal_gpio_speed_default;
    hal_spi_sck_cfg.altcfg  = &hal_spi_sck_altcfg;
    hal_spi_miso_cfg.dir    = hal_gpio_dirALT;
    hal_spi_miso_cfg.speed  = hal_gpio_speed_default;
    hal_spi_miso_cfg.altcfg = &hal_spi_miso_altcfg;    
    hal_spi_mosi_cfg.dir    = hal_gpio_dirALT;
    hal_spi_mosi_cfg.speed  = hal_gpio_speed_default;
    hal_spi_mosi_cfg.altcfg = &hal_spi_mosi_altcfg;
    
    // configure sck miso mosi pins    
    hal_gpio_init(hal_brdcfg_spi__theconfig.gpiomap[HAL_spi_id2index(id)].sck.gpio, &hal_spi_sck_cfg); 
    hal_gpio_init(hal_brdcfg_spi__theconfig.gpiomap[HAL_spi_id2index(id)].miso.gpio, &hal_spi_miso_cfg);    
    hal_gpio_init(hal_brdcfg_spi__theconfig.gpiomap[HAL_spi_id2index(id)].mosi.gpio, &hal_spi_mosi_cfg);    

    
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
}

static void s_hal_spi_hw_enable(hal_spi_t id, const hal_spi_cfg_t* cfg)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    uint16_t prescaler_stm32fx = 0;
    SPI_InitTypeDef spi_cfg;
    
    
    SPI_I2S_DeInit(SPIx);
 
    memcpy(&spi_cfg, &s_hal_spi_stm32_cfg, sizeof(SPI_InitTypeDef));
    // apply the mode
    spi_cfg.SPI_Mode                = (hal_spi_ownership_master == cfg->ownership) ? (SPI_Mode_Master) : (SPI_Mode_Slave);    

        
    // configure speed of spi ...
    
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
                prescaler_stm32fx = (hal_spi1 == id) ? SPI_BaudRatePrescaler_4 : SPI_BaudRatePrescaler_2;    
            } break;
            case hal_spi_prescaler_008: 
            {
                prescaler_stm32fx = (hal_spi1 == id) ? SPI_BaudRatePrescaler_8 : SPI_BaudRatePrescaler_4;
            } break;
            case hal_spi_prescaler_016: 
            {
                prescaler_stm32fx = (hal_spi1 == id) ? SPI_BaudRatePrescaler_16 : SPI_BaudRatePrescaler_8;  
            } break;
            case hal_spi_prescaler_032: 
            { 
                prescaler_stm32fx = (hal_spi1 == id) ? SPI_BaudRatePrescaler_32 : SPI_BaudRatePrescaler_16;   
            } break;
            case hal_spi_prescaler_064: 
            {
                prescaler_stm32fx = (hal_spi1 == id) ? SPI_BaudRatePrescaler_64 : SPI_BaudRatePrescaler_32;   
            } break;
            case hal_spi_prescaler_128: 
            {
                prescaler_stm32fx = (hal_spi1 == id) ? SPI_BaudRatePrescaler_128 : SPI_BaudRatePrescaler_64;  
            } break;
            case hal_spi_prescaler_256: 
            {
                prescaler_stm32fx = (hal_spi1 == id) ? SPI_BaudRatePrescaler_256 : SPI_BaudRatePrescaler_128;  
            }   break;
            default:                    
            {
                prescaler_stm32fx = SPI_BaudRatePrescaler_256;  
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
            case 2:     prescaler_stm32fx = SPI_BaudRatePrescaler_2;    break;  
            case 4:     prescaler_stm32fx = SPI_BaudRatePrescaler_4;    break;
            case 8:     prescaler_stm32fx = SPI_BaudRatePrescaler_8;    break;
            case 16:    prescaler_stm32fx = SPI_BaudRatePrescaler_16;   break;
            case 32:    prescaler_stm32fx = SPI_BaudRatePrescaler_32;   break;
            case 64:    prescaler_stm32fx = SPI_BaudRatePrescaler_64;   break;
            case 128:   prescaler_stm32fx = SPI_BaudRatePrescaler_128;  break;
            case 256:   prescaler_stm32fx = SPI_BaudRatePrescaler_256;  break;
            default:    prescaler_stm32fx = SPI_BaudRatePrescaler_256;  break;
        }
        
    }
    
    // ok ... we have prescaler_stm32fx at last.
    spi_cfg.SPI_BaudRatePrescaler = prescaler_stm32fx;
    
    // can init spi 
    SPI_Init(SPIx, &spi_cfg);
    
    if(hal_spi_act_framebased == cfg->activity)
    {    
        // enable dma rx request
        SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, ENABLE);  
        // enable dma tx request
        SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);   
    }    
    
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
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



#endif//HAL_USE_PERIPH_SPI

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



