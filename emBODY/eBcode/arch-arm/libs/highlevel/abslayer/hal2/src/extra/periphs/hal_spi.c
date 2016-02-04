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
#include "hal_5v.h"

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

// HAL_MANAGE_ISRFRAMES_WITH_FIFO
// marco.accame: info abouth macro HAL_MANAGE_ISRFRAMES_WITH_FIFO (introduced on 29 jan 2016)
// by defining this macro we manage spi communication using fifo objects to manage the tx and rx frames.
// if we undef this, we use uint8_t* buffers. 
// both ways manage hal_spi_datasize_8bit and hal_spi_datasize_16bit, for number of frames = 1 or higher.
// prior to 29 jan 2016 we used used uint8_t* buffers but managed only teh case hal_spi_datasize_8bit and 1 frame only

#define HAL_MANAGE_ISRFRAMES_WITH_FIFO


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
    .datasize                   = hal_spi_datasize_8bit,    
    .maxsizeofframe             = 4,
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
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


typedef struct
{
    hal_spi_cfg_t       config;
    hal_spi_t           id;
    hal_bool_t          isrisenabled;
    uint8_t             sizeofframe;            // cannot be higher than config.maxsizeofframe. it can be configured for each transmission
    uint8_t             sizeofword;             // it is 1 or 2 depending on hal_spi_datasize_8bit or hal_spi_datasize_16bit
    hl_fifo_t*          fiforx;                 // it is a fifo of rx frames. the basic item is a frame, whose size if cfg->maxsizeofframe*1 or cfg->maxsizeofframe*2 depending on hal_spi_datasize_8bit or hal_spi_datasize_16bit
    uint8_t             rxWORDScounter;         // it forward counts the rx words until a frame is reached. its range is [0, sizeoframeinuse-1]
    uint8_t             rxFRAMEScountdown;      // it backard counts the rx frames. it is initted by hal_spi_start() and reding continues until it reaches zero.
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)
    hl_fifo_t*          rxFIFOisrframe; 
    hl_fifo_t*          txFIFOisrframe;
    uint8_t*            userdeftxframe;
#else
    uint8_t*            rxBUFFERisrframe;
    uint8_t*            txBUFFERisrframe;   
#endif    
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
static void s_hal_spi_initted_reset(hal_spi_t id);
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
const hl_spi_mapping_t* hl_spi_map = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_boolval_t hal_spi_supported_is(hal_spi_t id)
{
    return(s_hal_spi_supported_is(id));
}


// this is not implemented in here.
// /** @fn            extern hal_result_t hal_spi_send(hal_spi_t id, uint8_t* txframe, uint8_t size)
//     @brief      this function adds the frame to the transmission buffer.  
//                 if the id is configured having activity hal_spi_act_continuous, then the sendnow enables or disables the activity.
//                 the txframe, if its size is not zero, is sent as soon as a new frame starts. otherwise, frames are taken from the 
//                 txfifoofframes or are just zero data.
//     @param      id            the id
//     @param      txframe         the frame to transmit
//     @return     hal_res_OK or hal_res_NOK_generic on failure
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

// keep it just in case. it is a very simple way of spi operations using stm32 api. 
//#if 0
//extern hal_result_t hal_spi_writeread(hal_spi_t id, uint8_t byte, uint8_t* readbyte)
//{
//    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
//    hal_spi_cfg_t* cfg = NULL;
//    volatile uint32_t timeout = 0;
//    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
//    
//    
//    if(hal_false == hal_spi_initted_is(id))
//    {
//        return(hal_res_NOK_generic);
//    }
//    
//    cfg = &intitem->config;    
//        
//    if(hal_spi_act_raw != cfg->activity)
//    {
//        return(hal_res_NOK_generic);
//    }
//    
//    // before we write we need to wait for the spi has txe set
//    timeout = s_hal_spi_timeout_flag;        
//    while(RESET == SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE))
//    {
//        if(0 == (timeout--)) s_hal_spi_timeoutexpired();
//    }         
//    
//    // ok. we send the byte
//    SPI_I2S_SendData(SPIx, byte);
//    
//    // we need to wait for a reply from the slave
//    timeout = s_hal_spi_timeout_flag;
//    while(RESET == SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE))
//    {
//        if(0 == (timeout--)) s_hal_spi_timeoutexpired();
//    }  
//    
//    // ok. here it is
//    uint8_t rb = SPI_I2S_ReceiveData(SPIx);

//    // if we want to retrieve it we copy into return value
//    if(NULL != readbyte)   
//    {
//        *readbyte = rb;
//    }  
//  
//    return(hal_res_OK);     
//}
//#endif


extern hal_result_t hal_spi_raw_enable(hal_spi_t id)
{
//    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
//    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);

    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_spi_periph_enable(id);
    
    return(hal_res_OK);   
}

extern hal_result_t hal_spi_raw_disable(hal_spi_t id)
{
//    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
//    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);

    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_spi_periph_disable(id);
    
    return(hal_res_OK);   
}


extern hal_result_t hal_spi_raw_writeread(hal_spi_t id, uint16_t writedata, uint16_t* dataread)
{
//    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    // marco.accame: later on put back a timeout and give hal_res_NOK_generic upon timeout
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPIx, writedata);    


    // we need to wait for a reply from the slave
//    timeout = 10000;
    while(RESET == SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE));
//    {
//        if(0 == (timeout--)) break;
//    }  

    // ok. here it is
    uint16_t rb = SPI_I2S_ReceiveData(SPIx);

    // if we want to retrieve it we copy into return value
    if(NULL != dataread)   
    {
        *dataread = rb;
    }  

    return(hal_res_OK);     
}


   
extern hal_result_t hal_spi_start(hal_spi_t id, uint8_t numberofframes)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    
    //SPI low level identifier
    SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);


#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    } 
    if(0 == numberofframes)
    {
        return(hal_res_NOK_generic);
    }
#endif    

//#if 0    
//    if(hal_spi_act_framebased != intitem->config.activity)
//    {
//        return(hal_res_NOK_generic);
//    }    
//#endif
      
    // protect ... may be not needed
    s_hal_spi_rx_isr_disable(id);       
    
    // tells how many frames to use
    intitem->rxFRAMEScountdown = numberofframes;
        
    // clear rx frame + rxWORDScounter 
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)    
    hl_fifo_clear(intitem->rxFIFOisrframe);
    intitem->rxWORDScounter = 0;
#else    
    memset(intitem->rxBUFFERisrframe, 0, intitem->config.maxsizeofframe * intitem->sizeofword);
    intitem->rxWORDScounter = 0;
#endif

    // need to send the first value as someone loaded into tx frame
   
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)
    uint16_t data = 0;
    if(1 == intitem->sizeofword)
    {
        uint8_t d01 = 0;
        hl_fifo_get(intitem->txFIFOisrframe, &d01, NULL);
        data = d01;
    }
    else
    {
        hl_fifo_get(intitem->txFIFOisrframe, (uint8_t*)&data, NULL);
    }   // if someone does not reload the tx frame each time, then we transmit 0
#else
    uint16_t data = intitem->txBUFFERisrframe[0];
#endif
    
    s_hal_spi_rx_isr_enable(id);        // enable interrupt rx

    s_hal_spi_periph_enable(id);        // enable spi peripheral
    
    // wait till spi is ready
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPIx, data);
    
    // and now processing is done by the ISR
    
    return(hal_res_OK);    
}

extern hal_bool_t hal_spi_active_is(hal_spi_t id)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    //SPI_TypeDef* SPIx = HAL_spi_id2stmSPI(id);
    
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_false);
    }  
  
    return((0 != intitem->rxFRAMEScountdown) ? (hal_true) : (hal_false));     
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

// /** @fn            extern hal_result_t hal_spi_stop(hal_spi_t id)
//     @brief      this function stops communication. 
//     @param      id                the id
//     @return     hal_res_OK or hal_res_NOK_generic on failure
//   */
/*
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
*/

extern hal_result_t hal_spi_on_framesreceived_set(hal_spi_t id, hal_callback_t onframesreceived, void* arg)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)]; 

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    intitem->config.onframesreceived       = onframesreceived;
    intitem->config.argonframesreceived    = arg;
    
    return(hal_res_OK);    
}

extern hal_result_t hal_spi_rx_isr_enable(hal_spi_t id)
{
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    s_hal_spi_rx_isr_enable(id);
    return (hal_res_OK);
}

extern hal_result_t hal_spi_rx_isr_disable(hal_spi_t id)
{
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    s_hal_spi_rx_isr_disable(id);
    return (hal_res_OK);
}

extern hal_result_t hal_spi_periph_enable(hal_spi_t id)
{
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    s_hal_spi_periph_enable(id);
    return (hal_res_OK);
}

extern hal_result_t hal_spi_periph_disable(hal_spi_t id)
{
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    s_hal_spi_periph_disable(id);
    return (hal_res_OK);
}

extern hal_result_t hal_spi_set_isrtxframe(hal_spi_t id, const uint8_t* txframe)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)]; 
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)
    if(NULL == txframe)
    {
        memset(intitem->userdeftxframe, 0, intitem->config.maxsizeofframe * intitem->sizeofword);
        hl_fifo_clear(intitem->txFIFOisrframe);         
    }
    else 
    {
        memcpy(intitem->userdeftxframe, txframe, intitem->config.maxsizeofframe * intitem->sizeofword);
        hl_fifo_load(intitem->txFIFOisrframe, txframe); 
    }
#else  
    if(NULL == txframe)
    {
        memset(intitem->txBUFFERisrframe, 0, intitem->config.maxsizeofframe * intitem->sizeofword);
    }
    else
    {
        memcpy(intitem->txBUFFERisrframe, txframe, intitem->config.maxsizeofframe * intitem->sizeofword);
    }
#endif    
        
    return(hal_res_OK);
}

extern hal_result_t hal_spi_set_sizeofframe(hal_spi_t id, uint8_t framesize)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)]; 
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    if(framesize > intitem->config.maxsizeofframe)
    {
        return(hal_res_NOK_generic);
    }

    intitem->sizeofframe = framesize;
    
    return(hal_res_OK);
}

extern hal_result_t hal_spi_deinit(hal_spi_t id)
{
#if   !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)      
    if(hal_false == hal_spi_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
            
    hl_result_t res;
    res = hl_spi_deinit((hl_spi_t)id);
            
    if ( hl_res_NOK_generic == res )
    {
        return hal_res_NOK_generic;
    }
    
    s_hal_spi_initted_reset(id);
    
    hal_heap_delete((void**)&(s_hal_spi_theinternals.items[HAL_spi_id2index(id)]));
    //hal_heap_delete((void**)&intitem);
    
    return (hal_res_OK);
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
     
    uint16_t rxword = SPI_I2S_ReceiveData(SPIx);
     
    if(intitem->rxWORDScounter < intitem->sizeofframe)
    {
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)        
        hl_fifo_put(intitem->rxFIFOisrframe, (uint8_t*)&rxword);
#else        
        memcpy(&intitem->rxBUFFERisrframe[intitem->rxWORDScounter * intitem->sizeofword], &rxword, intitem->sizeofword); // this is generic
        //        intitem->rxBUFFERisrframe[intitem->rxWORDScounter] = (uint8_t) rxword; // marco.accame: this is good only if datasize is 8 bit
#endif        
        intitem->rxWORDScounter ++;
    }
     
    if(intitem->rxWORDScounter == intitem->sizeofframe)
    {   // received a full frame.
        
        // reset word counter and decrement the frame counter
        intitem->rxWORDScounter = 0;
        intitem->rxFRAMEScountdown --;
        
        // eval if to stop activity
        hal_bool_t stopActivity = (0 == intitem->rxFRAMEScountdown) ? (hal_true) : (hal_false);

//        if(hal_true == stopActivity)
//        {
////            // 1. stop spi 
////            s_hal_spi_periph_disable(id);           // disable periph
////            s_hal_spi_rx_isr_disable(id);           // disable interrupt rx
////            
////            // clear tx frame
////#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)
////            hl_fifo_clear(intitem->txFIFOisrframe);
////#else
////            memset(intitem->txBUFFERisrframe, 0, intitem->config.maxsizeofframe * intitem->sizeofword);  
////#endif              
//        }
                
       
        // always: copy rx frame into the rx fifo of all the frames     
        
        if(hl_true == hl_fifo_full(intitem->fiforx))
        {   // if the fifo is full, drop first element
            hl_fifo_pop(intitem->fiforx);
        }

        uint8_t *data = NULL;
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)        
        data = hl_fifo_front(intitem->rxFIFOisrframe); // front because we want to retrieve the whole frame
#else        
        data = intitem->rxBUFFERisrframe; 
#endif        
        hl_fifo_put(intitem->fiforx, data);  

         
        // always: clear rx frame
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)         
        hl_fifo_clear(intitem->rxFIFOisrframe);
#else        
        memset(intitem->rxBUFFERisrframe, 0, intitem->config.maxsizeofframe * intitem->sizeofword);
#endif  

        if(hal_true == stopActivity)
        {
            // 1. stop spi 
            s_hal_spi_periph_disable(id);           // disable periph
            s_hal_spi_rx_isr_disable(id);           // disable interrupt rx
            
            // 2. clear tx frame
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)
            hl_fifo_clear(intitem->txFIFOisrframe);
            memset(intitem->userdeftxframe, 0, intitem->config.maxsizeofframe * intitem->sizeofword);
#else
            memset(intitem->txBUFFERisrframe, 0, intitem->config.maxsizeofframe * intitem->sizeofword);  
#endif              
            
            // now manage the callback
            hal_callback_t onframesreceived = intitem->config.onframesreceived;
            void *arg = intitem->config.argonframesreceived;     
            if(NULL != onframesreceived)
            {
                onframesreceived(arg);
            }
        }
        else
        {
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)
            // reload the txFIFOisrframe with what the user previously set
            hl_fifo_load(intitem->txFIFOisrframe, intitem->userdeftxframe);
#endif            
        }
        
        // ok: we have done
    }
    else
    {
        // transmit one dummy word to trigger another reception
        uint16_t data = 0;
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)         
        if(1 == intitem->sizeofword)
        {
            uint8_t d01 = 0;
            hl_fifo_get(intitem->txFIFOisrframe, &d01, NULL);
            data = d01;
        }
        else
        {
            hl_fifo_get(intitem->txFIFOisrframe, (uint8_t*)&data, NULL);
        }   // if someone does not reload the tx frame each time, then we transmit 0     
#else 
        uint8_t* dd = &intitem->txBUFFERisrframe[intitem->rxWORDScounter * intitem->sizeofword];
        data = *dd;
#endif            
        SPI_I2S_SendData(SPIx, data);
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

static void s_hal_spi_initted_reset(hal_spi_t id)
{
    hl_bits_word_bitclear(&s_hal_spi_theinternals.inittedmask, HAL_spi_id2index(id));
}

static hal_boolval_t s_hal_spi_initted_is(hal_spi_t id)
{   
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_spi_theinternals.inittedmask, HAL_spi_id2index(id)));
}


static hal_result_t s_hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg)
{
    hal_spi_internal_item_t* intitem = s_hal_spi_theinternals.items[HAL_spi_id2index(id)];
    
    // default config if cfg == NULL
    if(NULL == cfg)
    {
        cfg = &hal_spi_cfg_default;
    }
       
    // if not supported, return unsupported code
    if(hal_true != s_hal_spi_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }
    
    // check if already initialized
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
    
    
    hal_5v_init(NULL);
    hal_5v_on();
    
    // acemor: very important info.
    // init the miso and mosi gpio before calling hw_init. 
    // because if the spi is already initted and it detects mosi or miso low it sets
    // register SPI_SR2.BUSY to 1, which makes things hang up.
    
    //const hl_spi_advcfg_t hl_spi_advcfg_ems4rd =
    hl_spi_advcfg_t hl_spi_advcfg_ems4rd =
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
        
    hl_spi_advcfg_ems4rd.SPI_CPOL = (hal_spi_cpolarity_low == cfg->cpolarity) ? (SPI_CPOL_Low) : (SPI_CPOL_High);

    hl_spi_advcfg_ems4rd.SPI_CPHA = (hal_spi_datacapture_1edge == cfg->datacapture) ? (SPI_CPHA_1Edge) : (SPI_CPHA_2Edge);

    hl_spi_advcfg_ems4rd.SPI_DataSize = (hal_spi_datasize_16bit == cfg->datasize) ? (SPI_DataSize_16b) : (SPI_DataSize_8b);

    //SPI1 has a different clock
    // marco.accame: yes, it is correct that SPI1 has a difefrent speed, ...
    //               but inside hl_spi_init() we always use the value in hlcfg.prescaler 
    //               moreover: we use s_hal_spi_get_hl_prescaler() which correctly computes the prescaler depending on maxspeed and the used SPI port
    //               thus: no need to change value of hl_spi_advcfg_ems4rd.SPI_BaudRatePrescaler with SPI_BaudRatePrescaler_128 !!!!!!
    if(hal_spi1 == id)
    {
        hl_spi_advcfg_ems4rd.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    }
        

    hl_spi_cfg_t hlcfg =
    {
        .mode       = hl_spi_mode_master,
        .prescaler  = hl_spi_prescaler_064, 
        .advcfg     = &hl_spi_advcfg_ems4rd                
    };
    hlcfg.mode = (hal_spi_ownership_master == cfg->ownership) ? (hl_spi_mode_master) : (hl_spi_mode_slave); 
        
    // compute the prescaler and substitute it inside hlcfg structure
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
    intitem->sizeofframe = usedcfg->maxsizeofframe;
    intitem->sizeofword = (hal_spi_datasize_16bit == usedcfg->datasize) ? (2) : (1);
 
    volatile uint8_t sss = intitem->sizeofword;
    sss = sss;
    
    // - the isr rx frame (heap allocation)  
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)    
    intitem->rxFIFOisrframe = hl_fifo_new(usedcfg->maxsizeofframe, intitem->sizeofword, NULL);
#else    
    intitem->rxBUFFERisrframe = (uint8_t*)hal_heap_new(usedcfg->maxsizeofframe * intitem->sizeofword);    
#endif 
    // reset counter
    intitem->rxWORDScounter = 0;
        
    // - the isr tx frame (heap allocation)
#if defined(HAL_MANAGE_ISRFRAMES_WITH_FIFO)      
    intitem->txFIFOisrframe = hl_fifo_new(usedcfg->maxsizeofframe, intitem->sizeofword, NULL);
    intitem->userdeftxframe = (uint8_t*)hal_heap_new(usedcfg->maxsizeofframe * intitem->sizeofword);
#else    
    intitem->txBUFFERisrframe = (uint8_t*)hal_heap_new(usedcfg->maxsizeofframe * intitem->sizeofword);    
#endif

    // - the fifo of rx frames. but only if it is needed ... we dont need it if ...
    intitem->fiforx = hl_fifo_new(usedcfg->capacityofrxfifoofframes, usedcfg->maxsizeofframe * intitem->sizeofword, NULL);
        
    // - the id
    intitem->id = id;

    // - rxFRAMEScountdown
    intitem->rxFRAMEScountdown = 0;
   
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
    // mild verification of the config: ownership, activity, maxsizeofframe, direction, capacity of frames.
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

    if(0 == cfg->maxsizeofframe)
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



