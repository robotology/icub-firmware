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

#ifdef HAL_USE_SPI

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_brdcfg.h"
#include "hal_base_hid.h"
#include "hal_mpu_gpio_hid.h"


#include "hal_utility_bits.h"
#include "hal_utility_fifo.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_spi.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mpu_spi_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_spi_port2index(p)           ((uint8_t)(p))

#define HAL_spi_port2stmSPI(p)          (s_hal_spi_stmSPImap[HAL_spi_port2index(p)])

#define RXNE        0x01
#define TXE         0x02
#define BSY         0x80


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hal_spi_cfg_t hal_spi_cfg_default = 
{ 
    .ownership                  = hal_spi_ownership_master, 
    .direction                  = hal_spi_dir_txrx, 
    .activity                   = hal_spi_act_singleframe,
    .speed                      = hal_spi_speed_1125kbps, 
    .sizeoftxframe              = 4, 
    .sizeofrxframe              = 4, 
    .onframetransm              = NULL, 
    .onframereceiv              = NULL,
    .capacityoftxfifoofframes   = 4,
    .capacityofrxfifoofframes   = 4,
    .dummytxvalue               = 0
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

typedef struct
{
    hal_bool_t          locked;
    hal_spi_cfg_t       config;
    uint8_t*            txframe;
    uint8_t             txcount;
    uint8_t*            rxframe;
    uint8_t             rxcount;
    hal_utility_fifo_t  fifotx;
    hal_utility_fifo_t  fiforx;
    uint8_t*            rxbuffer;
    hal_bool_t          txisenabled;
    hal_bool_t          rxisenabled;
    hal_spi_direction_t direction;
    hal_bool_t          stopit;
} hal_spi_internals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_spi_scheduling_suspend(void);
static void s_hal_spi_scheduling_restart(void);

static hal_boolval_t s_hal_spi_supported_is(hal_spi_port_t port);
static void s_hal_spi_initted_set(hal_spi_port_t port);

static hal_bool_t s_hal_spi_is_status_locked(hal_spi_port_t port);

static hal_result_t s_hal_spi_init(hal_spi_port_t port, const hal_spi_cfg_t *cfg);

static void s_hal_spi_hw_init(hal_spi_port_t port);
static void s_hal_spi_hw_gpio_init(hal_spi_port_t port, hal_spi_ownership_t ownership);
static void s_hal_spi_hw_enable(hal_spi_port_t port, const hal_spi_cfg_t* cfg);

static hal_result_t s_hal_spi_put(hal_spi_port_t port, uint8_t* txframe, uint8_t size, hal_bool_t sendnow);

static hal_result_t s_hal_spi_get(hal_spi_port_t port, uint8_t* rxframe, uint8_t* size, uint8_t* remainingrxframes);

static void s_hal_spi_nvic_enable(hal_spi_port_t port);
static void s_hal_spi_nvic_disable(hal_spi_port_t port);

static void s_hal_spi_periph_enable(hal_spi_port_t port);
static void s_hal_spi_periph_disable(hal_spi_port_t port);

static void s_hal_spi_isr_enable(hal_spi_port_t port);
static void s_hal_spi_isr_disable(hal_spi_port_t port);

static hal_result_t s_hal_spi_timeoutexpired(void);

static void s_hal_spi_theisr(SPI_TypeDef* SPIx, hal_spi_internals_t* spixint);

static hal_result_t s_hal_spi_isr_txbyte_get(hal_spi_internals_t* spixint, uint8_t* txbyte, hal_bool_t* itisthelastoftheframe, hal_bool_t* itisthelastinsidefifo);
static hal_result_t s_hal_spi_isr_rxbyte_put(hal_spi_internals_t* spixint, uint8_t rxbyte, hal_bool_t* itisthelastoftheframe, hal_bool_t* fifoisfull);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_spi_initted[hal_spi_ports_number] = { hal_false, hal_false, hal_false };

//static hal_spi_status_t s_hal_spi_status[hal_spi_ports_number] = { {.locked = hal_false, .devaddr = 0 }, {.locked = hal_false, .devaddr = 0 }, {.locked = hal_false, .devaddr = 0 } };

static hal_spi_internals_t s_hal_spi_internals[hal_spi_ports_number] = { 0 };


static const SPI_InitTypeDef   s_hal_spi_stm32_cfg =
{
    .SPI_Direction                  = SPI_Direction_2Lines_FullDuplex,
    .SPI_Mode                       = SPI_Mode_Master,                              // param
    .SPI_DataSize                   = SPI_DataSize_16b, 
    .SPI_CPOL                       = SPI_CPOL_High, //SPI_CPOL_Low, //SPI_CPOL_Low, // SPI_CPOL_High high is ok with display and also ok with isr mode
    .SPI_CPHA                       = SPI_CPHA_1Edge, //SPI_CPHA_2Edge,
    .SPI_NSS                        = SPI_NSS_Soft,
    .SPI_BaudRatePrescaler          = SPI_BaudRatePrescaler_2,                      // param: depends on speed
    .SPI_FirstBit                   = SPI_FirstBit_LSB, // SPI_FirstBit_MSB is ok with display, su stm3210c e' indifferente
    .SPI_CRCPolynomial              = 0x0007 // reset value
};

#warning --> display usa SPI_CPHA_2Edge ma prima avevo messo SPI_CPHA_1Edge

#if     defined(USE_STM32F1)
static SPI_TypeDef* const s_hal_spi_stmSPImap[] = { SPI1, SPI2, SPI3 };
#elif   defined(USE_STM32F4)
static SPI_TypeDef* const s_hal_spi_stmSPImap[] = { SPI1, SPI2, SPI3 };
#endif

static const uint32_t s_hal_spi_hw_rcc[] = { RCC_APB2Periph_SPI1, RCC_APB1Periph_SPI2, RCC_APB1Periph_SPI3 };

static const uint32_t s_hal_spi_timeout_flag = 0x00010000;
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_spi_init(hal_spi_port_t port, const hal_spi_cfg_t *cfg)
{
    return(s_hal_spi_init(port, cfg));
}

extern hal_result_t hal_spi_write(hal_spi_port_t port, uint8_t byte, uint8_t* readbyte)
{
    volatile uint32_t timeout = 0;
    SPI_TypeDef* SPIx = NULL;
    
    if(hal_false == hal_spi_hid_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    SPIx = HAL_spi_port2stmSPI(port);   

    timeout = s_hal_spi_timeout_flag;        
    while(RESET == SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE))
    {
        if(0 == (timeout--)) s_hal_spi_timeoutexpired();
    }         
    
    SPI_I2S_SendData(SPIx, byte);
    
#if 1
    
    timeout = s_hal_spi_timeout_flag;

    while(RESET == SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE))
    {
        if(0 == (timeout--)) s_hal_spi_timeoutexpired();
    }  
    
    uint8_t rb = SPI_I2S_ReceiveData(SPIx);

    if(NULL != readbyte)   
    {
        *readbyte = rb;
    }  

#else
    if(NULL != readbyte)
    {       
        timeout = s_hal_spi_timeout_flag;

        while(RESET == SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE))
        {
            if(0 == (timeout--)) s_hal_spi_timeoutexpired();
        }  
        
        *readbyte = SPI_I2S_ReceiveData(SPIx);        
    }

#endif    
   
    return(hal_res_OK);     
}



// extern hal_result_t hal_spi_direction_set(hal_spi_port_t port, hal_spi_direction_t dir)
// {
//     if(hal_false == hal_spi_hid_initted_is(port))
//     {
//         return(hal_res_NOK_generic);
//     }

// #if 0    
//     s_hal_spi_scheduling_suspend();
//     if(hal_false == s_hal_spi_is_status_locked(port))
//     {
//         s_hal_spi_scheduling_restart();
//         return(hal_res_NOK_generic);
//     }
//     s_hal_spi_status_setlock(); and unlock etc ....
// #endif
//     
//     s_hal_spi_internals[HAL_spi_port2index(port)].direction = dir;
//        
//     return(hal_res_OK);    
// }

extern hal_result_t hal_spi_put(hal_spi_port_t port, uint8_t* txframe, uint8_t size, hal_bool_t sendnow)
{ 
    if(hal_false == hal_spi_hid_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

#if 0    
    s_hal_spi_scheduling_suspend();
    if(hal_false == s_hal_spi_is_status_locked(port))
    {
        s_hal_spi_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_spi_status_setlock(); and unlock etc ....
#endif
       
    if(NULL == txframe)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    if(0 == size)
    {
        return(hal_res_OK);
    }  

    return(s_hal_spi_put(port, txframe, size, sendnow));
}


extern hal_result_t hal_spi_get(hal_spi_port_t port, uint8_t* rxframe, uint8_t* size, uint8_t* remainingrxframes)
{
    if(hal_false == hal_spi_hid_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

#if 0    
    s_hal_spi_scheduling_suspend();
    if(hal_false == s_hal_spi_is_status_locked(port))
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
    
    if(NULL == size)
    {
        return(hal_res_OK);
    }  

    return(s_hal_spi_get(port, rxframe, size, remainingrxframes));       
}

   
extern hal_result_t hal_spi_start(hal_spi_port_t port)
{
    if(hal_false == hal_spi_hid_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

#if 0    
    s_hal_spi_scheduling_suspend();
    if(hal_false == s_hal_spi_is_status_locked(port))
    {
        s_hal_spi_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_spi_status_setlock(); and unlock etc ....
#endif
       
    s_hal_spi_periph_enable(port);
    s_hal_spi_isr_enable(port);
    
    return(hal_res_OK);    
}


extern hal_result_t hal_spi_stop(hal_spi_port_t port)
{
    if(hal_false == hal_spi_hid_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

#if 0    
    s_hal_spi_scheduling_suspend();
    if(hal_false == s_hal_spi_is_status_locked(port))
    {
        s_hal_spi_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_spi_status_setlock(); and unlock etc ....
#endif
       

    #warning --> si disabilita cosi' oppure si setta una variabile che poi disabilita dentro la isr
    s_hal_spi_isr_disable(port);
    
    return(hal_res_OK);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void SPI1_IRQHandler(void)
{
    s_hal_spi_theisr(SPI1, &s_hal_spi_internals[HAL_spi_port2index(hal_spi_port1)]);   
}


void SPI2_IRQHandler(void)
{
    s_hal_spi_theisr(SPI2, &s_hal_spi_internals[HAL_spi_port2index(hal_spi_port2)]);   
}


void SPI3_IRQHandler(void)
{
    s_hal_spi_theisr(SPI3, &s_hal_spi_internals[HAL_spi_port2index(hal_spi_port3)]);
}

// ---- isr of the module: end ------


extern uint32_t hal_spi_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_spi_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }

    // removed dependancy from nzi data
    memset(&s_hal_spi_initted, hal_false, sizeof(s_hal_spi_initted));   
    memset(&s_hal_spi_internals, 0, sizeof(s_hal_spi_internals));

    return(hal_res_OK);
}

extern hal_boolval_t hal_spi_hid_initted_is(hal_spi_port_t port)
{

    if(hal_true == s_hal_spi_initted[HAL_spi_port2index(port)]) 
    {
        return(hal_true);
    }
    
    return(hal_false);

}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//    extern uint32_t txe_number = 0;
//    extern uint32_t rxne_number = 0;

extern uint8_t slatxbytes[80] = {0};
extern uint8_t slatxindex = 0;
extern uint8_t masrxbytes[80] = {0};
extern uint8_t masrxindex = 0;

extern uint8_t mastxbytes[80] = {0};
extern uint8_t mastxindex = 0;
extern uint8_t slarxbytes[80] = {0};
extern uint8_t slarxindex = 0;

static void s_hal_spi_theisr(SPI_TypeDef* SPIx, hal_spi_internals_t* spixint)
{
    volatile uint8_t value = 0;
    
    if(SET == SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_OVR))
    {
        for(;;); // se SPI_I2S_SendData() e' eseguita all'inizio
    }
    
    if(1 == spixint->stopit)
    {
        SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
        spixint->txisenabled = 0;
        spixint->stopit = 0;
        // if one wants to disable spi ... before that he must wait until the activity flag is reset.
        
//         SPI_I2S_SendData(SPIx, 0x66);
//             
//         if(hal_spi_ownership_slave == spixint->config.ownership)
//         {
//             slatxbytes[slatxindex++] = 0x66;
//         }
//         else
//         {
//             mastxbytes[mastxindex++] = 0x66;
//         }       
        
    }
    
    if(SET == SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_RXNE))
    {
        uint8_t rxbyte = (uint8_t)SPI_I2S_ReceiveData(SPIx);
        
        //rxne_number++;
        
        hal_bool_t itisthelastbyteoftheframe = hal_false;
        hal_bool_t fifoisfull = hal_false;
        
        value++;
        
       

        
//                 if(hal_spi_ownership_slave == spixint->config.ownership)
//                 {
//                     SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);
//                 }
        

            if(hal_spi_ownership_slave == spixint->config.ownership)
            {
                slarxbytes[slarxindex++] = rxbyte;
            }  
            else
            {
                masrxbytes[masrxindex++] = rxbyte;
            }    
#if 1              
        if(hal_res_OK == s_hal_spi_isr_rxbyte_put(spixint, rxbyte, &itisthelastbyteoftheframe, &fifoisfull))
        {            
            if((hal_true == itisthelastbyteoftheframe))
            {   
                if((hal_spi_ownership_master == spixint->config.ownership) && (hal_spi_act_singleframe == spixint->config.activity) && (hal_spi_dir_rxonly == spixint->config.direction))
                {
                    //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
                    spixint->stopit = 1;
                    //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
                }
                
                if(NULL != spixint->config.onframereceiv)
                {
                    spixint->config.onframereceiv(NULL);
                }
            } 
            
            if(hal_true == fifoisfull)
            {
                // ok ... good to know 
            }            
        }
        else
        {   // some error occurred ... i disable the transmission
            if(hal_spi_ownership_master == spixint->config.ownership)
            {
                SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
                spixint->txisenabled = 0;
                //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
            }        
        }
#endif
          
    }    
   
    if(SET == SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_TXE))
    {   
        
        //txe_number++;
               
        static volatile uint8_t txbyte = 0;
        hal_bool_t itisthelastbyteoftheframe = hal_false;
        hal_bool_t itisthelastbyteinsidefifo = hal_false;
        
        value++;
        
        
        // both singleframe and multiframe activity get the byte from here inside
 
        if(hal_res_OK == s_hal_spi_isr_txbyte_get(spixint, &txbyte, &itisthelastbyteoftheframe, &itisthelastbyteinsidefifo))
        {   // if ok, then we can send the tx byte and also we evaluate the modified boolean parameters
            
            //hal_sys_delay(100); // con il delay solo qui non funxiona
//            SPI_I2S_SendData(SPIx, txbyte);
            //hal_sys_delay(100); // funziona solo se metto questo delay (oppure se sposto la fne al fondo)
            
            if(hal_spi_ownership_slave == spixint->config.ownership)
            {
                slatxbytes[slatxindex++] = txbyte;
            }
            else
            {
                mastxbytes[mastxindex++] = txbyte;
            }               
            
            if(hal_true == itisthelastbyteoftheframe)
            {                
                if(NULL != spixint->config.onframetransm)                    
                {
                    spixint->config.onframetransm(NULL);
                }
                
                if((hal_spi_ownership_master == spixint->config.ownership) && (hal_spi_act_singleframe == spixint->config.activity))
                {
                    //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
                    spixint->stopit = 1;
                    //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
                }
            }
            
            if(hal_true == itisthelastbyteinsidefifo)
            {
                if((hal_spi_ownership_master == spixint->config.ownership) && (hal_spi_act_multiframe == spixint->config.activity))
                {
                    //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
                    spixint->stopit = 1;
                    //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
                }     
            } 
            
//             hal_sys_delay(100);
             SPI_I2S_SendData(SPIx, txbyte);
//             hal_sys_delay(100);
//            hal_sys_delay(100);
        }
        else
        {   // damn ... we have an error. we disable 
            if(hal_spi_ownership_master == spixint->config.ownership)
            {
                SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
                spixint->txisenabled = 0;
                //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
            }            
        }
       
    }
    
        if(2 == value)
        {
        //    for(;;);
        }

       
}



#if 0
static void s_hal_spi_theisr_safe_but_slow(SPI_TypeDef* SPIx, hal_spi_internals_t* spixint)
//static void s_hal_spi_theisr(SPI_TypeDef* SPIx, hal_spi_internals_t* spixint)
{
    static hal_bool_t lastbyteofframeisintransmission = hal_false;
    static hal_bool_t lastbyteofframeisreceived = hal_false;
    
    if(SET == SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_TXE))
    {   // master tx
        if(NULL == spixint->txframe)
        {   // either i have a new frame or i have finished one at the previous call
            if(hal_true == lastbyteofframeisintransmission)
            {
                lastbyteofframeisintransmission = hal_false;
                if(NULL != spixint->config.onframetransm)
                {
                    spixint->config.onframetransm(NULL);
                }                
            }
            if(hal_false == spixint->forcestop)
            {
                spixint->txframe =  hal_utility_fifo_front(&spixint->fifotx);
                spixint->txcount = 0;
            }
        }
        
        if(NULL != spixint->txframe)
        {   // transmit       
            uint8_t index = spixint->txcount++;
        
            if(index < spixint->config.sizeoftxframe)
            {             
                SPI_I2S_SendData(SPIx, spixint->txframe[index]);              
            }
            
            if(spixint->txcount == spixint->config.sizeoftxframe)
            {   // it is the last byte in the frame ...
                
                hal_utility_fifo_pop(&spixint->fifotx);
                lastbyteofframeisintransmission = hal_true;
                
                spixint->txframe = NULL;
                spixint->txcount = 0;                
            }
        }
        else
        {   // dont have any more frames in fifo-tx
            if((hal_spi_act_continuous == spixint->config.activity) && (hal_false == spixint->forcestop))
            {   // if tx is continuous we just send a zero byte
                SPI_I2S_SendData(SPIx, 0);
            }
//             else if(hal_false == spixint->forcestop)
//             {   // else we disable tx   
// //                spixint->forcestop = hal_false;
// //                SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
//                  SPI_I2S_SendData(SPIx, 0);
//             }
            else
            {
                spixint->forcestop = hal_false;
                SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);                
            }
        }

    }


    if(SET == SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_RXNE))
    {
        //uint8_t rxbyte = (uint8_t)SPI_I2S_ReceiveData(SPIx);
        
        if(NULL == spixint->rxframe)
        {   // either i have a new frame or i have finished one at the previous call
//             if(hal_true == lastbyteofframeisreceived)
//             {
//                 lastbyteofframeisreceived = hal_false;
//                 if(NULL != spixint->config.onframereceiv)
//                 {
//                     spixint->config.onframereceiv(NULL); // address of received byte
//                 }                
//             }
            
            //if(hal_false == spixint->forcestoprx)
            {
                spixint->rxframe = (hal_true ==  hal_utility_fifo_full(&spixint->fiforx)) ? (NULL) : (spixint->rxbuffer);
                spixint->rxcount = 0;
            }
        }  

        if(NULL != spixint->rxframe)
        {   // can put inside the fifo   
            uint8_t index = spixint->rxcount++;
        
            if(index < spixint->config.sizeofrxframe)
            {             
                spixint->rxframe[index] = (uint8_t)SPI_I2S_ReceiveData(SPIx);              
            }
            
            if(spixint->rxcount == spixint->config.sizeofrxframe)
            {   // it is the last byte in the frame ...
                
                hal_utility_fifo_put(&spixint->fiforx, spixint->rxframe);
                lastbyteofframeisreceived = hal_true;
                
                spixint->rxframe = NULL;
                spixint->rxcount = 0;     

                //spixint->forcestop = hal_true;  

                if(NULL != spixint->config.onframereceiv)
                {
                    spixint->config.onframereceiv(NULL); // address of received byte
                }                  
            }
        }
        else
        {   // dont have any more space in fifo-rx
            //if((hal_spi_act_continuous == spixint->config.activity) && (hal_false == spixint->forcestoprx))
            //{   // if tx is continuous we just retrieve the byte but we dont put inside the frame-rx
                uint8_t byte = (uint8_t)SPI_I2S_ReceiveData(SPIx); 
            //}
            //else
            //{   // else we disable rx
            //    spixint->forcestoprx = hal_false;
            //    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
            //}
        }        
       
    }    
    
}
#endif
static hal_boolval_t s_hal_spi_supported_is(hal_spi_port_t port)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_spi__theconfig.supported_mask, HAL_spi_port2index(port)) );
}

static void s_hal_spi_initted_set(hal_spi_port_t port)
{
    s_hal_spi_initted[HAL_spi_port2index(port)] = hal_true;
}


static hal_bool_t s_hal_spi_is_status_locked(hal_spi_port_t port)
{
    return(s_hal_spi_internals[HAL_spi_port2index(port)].locked);
}



static hal_result_t s_hal_spi_init(hal_spi_port_t port, const hal_spi_cfg_t *cfg)
{
    if(NULL == cfg)
    {
        cfg = &hal_spi_cfg_default;
    }
    
//     if(hal_spi_act_continuous == cfg->activity)
//     {
//         return(hal_res_NOK_unsupported);
//     }
    
    if(hal_true != s_hal_spi_supported_is(port))
    {
        return(hal_res_NOK_unsupported);
    }

    if(hal_true == hal_spi_hid_initted_is(port))
    {
        return(hal_res_OK);
    }    
    

    // acemor: very important info.
    // init the miso and mosi gpio before calling hw_init. 
    // because if the spi is already initted and it detects mosi or miso low it sets
    // register SPI_SR2.BUSY to 1, which makes things hang up.
    
    s_hal_spi_hw_gpio_init(port, cfg->ownership);
    s_hal_spi_hw_init(port);
    s_hal_spi_hw_enable(port, cfg);
    
    if((hal_spi_act_singleframe == cfg->activity) || (hal_spi_act_multiframe == cfg->activity))
    {
        s_hal_spi_nvic_enable(port);
    }
        
    
    s_hal_spi_initted_set(port);
    
    memcpy(&s_hal_spi_internals[HAL_spi_port2index(port)].config, cfg, sizeof(hal_spi_cfg_t));
    
    #warning --> remove teh calloc from here 
    hal_utility_fifo_init(&s_hal_spi_internals[HAL_spi_port2index(port)].fifotx, cfg->capacityoftxfifoofframes, cfg->sizeoftxframe, calloc(cfg->capacityoftxfifoofframes*cfg->sizeoftxframe, 1), NULL);
    hal_utility_fifo_init(&s_hal_spi_internals[HAL_spi_port2index(port)].fiforx, cfg->capacityofrxfifoofframes, cfg->sizeofrxframe, calloc(cfg->capacityofrxfifoofframes*cfg->sizeofrxframe, 1), NULL);
    
    s_hal_spi_internals[HAL_spi_port2index(port)].rxbuffer =  calloc(cfg->sizeofrxframe, 1);
    s_hal_spi_internals[HAL_spi_port2index(port)].txframe = NULL;
    s_hal_spi_internals[HAL_spi_port2index(port)].txcount = 0;
    
    s_hal_spi_internals[HAL_spi_port2index(port)].rxframe = NULL;
    s_hal_spi_internals[HAL_spi_port2index(port)].rxcount = 0;
    
    s_hal_spi_internals[HAL_spi_port2index(port)].direction = cfg->direction;
    
    s_hal_spi_internals[HAL_spi_port2index(port)].stopit = 0;
    s_hal_spi_internals[HAL_spi_port2index(port)].txisenabled = 0;
    s_hal_spi_internals[HAL_spi_port2index(port)].rxisenabled = 0;
  
    return(hal_res_OK);
}


static void s_hal_spi_hw_init(hal_spi_port_t port)
{
#if     defined(USE_STM32F1) || defined(USE_STM32F4)

    //uint32_t RCC_APB1Periph_SPIx = (hal_spi_port1 == port) ? (RCC_APB1Periph_SPI1) : (RCC_APB1Periph_SPI2); RCC_APB1Periph_SPI3
        
    
    if(hal_spi_port1 == port)
    {
        // spi periph clock enable
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
        
        // reset spi periph
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
        
        // release reset 
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);        
    }
    else
    {
        uint32_t RCC_APB1Periph_SPIx = s_hal_spi_hw_rcc[HAL_spi_port2index(port)];
        
        // spi periph clock enable
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPIx, ENABLE);
        
        // reset spi periph
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPIx, ENABLE);
        
        // release reset 
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPIx, DISABLE);        
    }
    
// //    // system configuration controller clock
     #warning --> in stm32f4 removed "RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);" from spi_hw_init() and it still works....
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

#endif
}


static void s_hal_spi_hw_gpio_init(hal_spi_port_t port, hal_spi_ownership_t ownership)
{
    
#if     defined(USE_STM32F1)

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

    hal_gpio_port_t portsck  = hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinsck   = hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_port2index(port)].pin;      
    hal_gpio_port_t portmiso = hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinmiso  = hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_port2index(port)].pin;
    hal_gpio_port_t portmosi = hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinmosi  = hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_port2index(port)].pin;    
     
    
    if(hal_spi_port1 == port)
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
    else if(hal_spi_port2 == port)
    {
        if((hal_gpio_portB == portmiso) && (hal_gpio_portB == portmosi) && (hal_gpio_portB == portsck) && (hal_gpio_pin14 == pinmiso) && (hal_gpio_pin15 == pinmosi) && (hal_gpio_pin13 == pinsck))
        {   // PB13, PB14, PB15
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }        
    }
    else if(hal_spi_port3 == port)
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
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_spi_init(): incorrect pin mapping");
    }

    hal_gpio_altcfg_t hal_spi_sck_altcfg;
    hal_gpio_altcfg_t hal_spi_miso_altcfg;
    hal_gpio_altcfg_t hal_spi_mosi_altcfg;
    
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
    hal_gpio_configure(hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_port2index(port)], &hal_spi_sck_altcfg);
    hal_gpio_configure(hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_port2index(port)], &hal_spi_miso_altcfg);    
    hal_gpio_configure(hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_port2index(port)], &hal_spi_mosi_altcfg);


#elif   defined(USE_STM32F4)    

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


    hal_gpio_port_t portsck  = hal_brdcfg_spi__sck[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinsck   = hal_brdcfg_spi__sck[HAL_spi_port2index(port)].pin;        
    hal_gpio_port_t portmiso = hal_brdcfg_spi__miso[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinmiso  = hal_brdcfg_spi__miso[HAL_spi_port2index(port)].pin;
    hal_gpio_port_t portmosi = hal_brdcfg_spi__mosi[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinmosi  = hal_brdcfg_spi__mosi[HAL_spi_port2index(port)].pin;       
   
    
    if(hal_spi_port1 == port)
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
    else if(hal_spi_port2 == port)
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
    else if(hal_spi_port3 == port)
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
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_spi_init(): incorrect pin mapping");
    }
    
    
    hal_gpio_altcfg_t hal_spi_sck_altcfg;   
    hal_gpio_altcfg_t hal_spi_miso_altcfg;
    hal_gpio_altcfg_t hal_spi_mosi_altcfg;

    
    // prepare the altcfg for sck miso mosi pins
    memcpy(&hal_spi_sck_altcfg, &s_hal_spi_misomosisck_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_spi_miso_altcfg, &s_hal_spi_misomosisck_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_spi_mosi_altcfg, &s_hal_spi_misomosisck_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_spi_sck_altcfg.afname = hal_spi_miso_altcfg.afname = hal_spi_mosi_altcfg.afname = afname;
    hal_spi_sck_altcfg.afmode = hal_spi_miso_altcfg.afmode = hal_spi_mosi_altcfg.afmode = afmode;
    
    
    // configure sck miso mosi pins    
    hal_gpio_configure(hal_brdcfg_spi__sck[HAL_spi_port2index(port)], &hal_spi_sck_altcfg); 
    hal_gpio_configure(hal_brdcfg_spi__miso[HAL_spi_port2index(port)], &hal_spi_miso_altcfg);    
    hal_gpio_configure(hal_brdcfg_spi__mosi[HAL_spi_port2index(port)], &hal_spi_mosi_altcfg);    

    
#endif
}

static void s_hal_spi_hw_enable(hal_spi_port_t port, const hal_spi_cfg_t* cfg)
{
#if     defined(USE_STM32F1) || defined(USE_STM32F4)
    
    SPI_TypeDef* SPIx = HAL_spi_port2stmSPI(port);
    
    SPI_I2S_DeInit(SPIx);
    
    SPI_InitTypeDef spi_cfg;
    memcpy(&spi_cfg, &s_hal_spi_stm32_cfg, sizeof(SPI_InitTypeDef));
    // apply the mode
    spi_cfg.SPI_Mode                = (hal_spi_ownership_master == cfg->ownership) ? (SPI_Mode_Master) : (SPI_Mode_Slave);    
    // from SPI_BaudRatePrescaler_2 to SPI_BaudRatePrescaler_128
    #warning --> SPI_BaudRatePrescaler must be configured. it depends on chosen speed and on the speed of the bus of spi1, 2, and 3 
    #warning --> IT IS OK ONLY FOR SPI3 at 18 MBPS

#if     defined(USE_STM32F1)
    const uint32_t HIGHSPEED = 72000000;
    const uint32_t LOWSPEED  = 36000000;
#elif   defined(USE_STM32F4)
        TODO ...
#endif
    
    uint16_t factor = 0;
    switch(port)
    {
        case hal_spi_port1:
        {
            factor = HIGHSPEED / cfg->speed; // spi1 is on the high speed bus 
        } break;
        
        case hal_spi_port2:
        case hal_spi_port3:
        {
            factor = LOWSPEED / cfg->speed; // spi2 and spi3 are on the low speed bus 
        } break;
    }
    
    uint16_t prescaler_stm32fx = 0;
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
    
    spi_cfg.SPI_BaudRatePrescaler = prescaler_stm32fx;
 
    SPI_Init(SPIx, &spi_cfg);
    
#endif
}




static hal_result_t s_hal_spi_timeoutexpired(void)
{
    // call a user-defined function
    #warning --> add a user-def fun
  
    hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "timeout error in spi operations");

    return(hal_res_NOK_generic);
}

static void s_hal_spi_scheduling_suspend(void)
{
    hal_base_hid_osal_scheduling_suspend();
}

static void s_hal_spi_scheduling_restart(void)
{
    hal_base_hid_osal_scheduling_restart();
}

extern void hal_test_spima_spisl(void)
{
    SPI_TypeDef* SPIma = SPI1;
    SPI_TypeDef* SPIsl = SPI2;
    const uint8_t SPIma_Buffer_Tx[7] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    const uint8_t SPIsl_Buffer_Tx[7] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 };
    
    static uint8_t SPIma_Buffer_Rx[7] = { 0 };
    static uint8_t SPIsl_Buffer_Rx[7] = { 0 };
    uint8_t TxIdx = 0;
    uint8_t RxIdx = 0;
    uint8_t BufferSize = sizeof(SPIma_Buffer_Tx);
    
    for(TxIdx=0, RxIdx=0; TxIdx < BufferSize; TxIdx++, RxIdx++)
    {
        /* Wait for SPIma Tx buffer empty */
        while (SPI_I2S_GetFlagStatus(SPIma, SPI_I2S_FLAG_TXE) == RESET);
        /* Send SPIsl data */
        SPI_I2S_SendData(SPIsl, SPIsl_Buffer_Tx[TxIdx]);
        /* Send SPIma data */
        SPI_I2S_SendData(SPIma, SPIma_Buffer_Tx[TxIdx]);

        /* Wait for SPIsl data reception */
        while (SPI_I2S_GetFlagStatus(SPIsl, SPI_I2S_FLAG_RXNE) == RESET);
        /* Read SPIsl received data */
        SPIsl_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPIsl);

        /* Wait for SPIma data reception */
        while (SPI_I2S_GetFlagStatus(SPIma, SPI_I2S_FLAG_RXNE) == RESET);
        /* Read SPIma received data */
        SPIma_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPIma);
    }
    
    TxIdx = TxIdx;

}

static hal_result_t s_hal_spi_put(hal_spi_port_t port, uint8_t* txframe, uint8_t size, hal_bool_t sendnow)
{
    hal_result_t res = hal_res_NOK_generic;
    const hal_spi_cfg_t* cfg = &s_hal_spi_internals[HAL_spi_port2index(port)].config;
 
    // disable so that we can change a data structure which is used by the isr
    s_hal_spi_isr_disable(port);
    res =  hal_utility_fifo_put(&s_hal_spi_internals[HAL_spi_port2index(port)].fifotx, txframe);
    
    if(hal_true == sendnow)
    {
        // enable again
        s_hal_spi_isr_enable(port);   
    }
    
    return(res);
}

static hal_result_t s_hal_spi_get(hal_spi_port_t port, uint8_t* rxframe, uint8_t* size, uint8_t* remainingrxframes)
{
    hal_result_t res = hal_res_NOK_generic;
    const hal_spi_cfg_t* cfg = &s_hal_spi_internals[HAL_spi_port2index(port)].config;
 
    // disable so that we can change a data structure which is used by the isr
    #warning --> devo pensare a come proteggere l'accessso alla fiforx da accessi concorrenti
//    s_hal_spi_isr_disable(port);
    res =  hal_utility_fifo_get(&s_hal_spi_internals[HAL_spi_port2index(port)].fiforx, rxframe, remainingrxframes);
    
    if(NULL != size)
    {
        *size = (hal_res_OK == res) ? (s_hal_spi_internals[HAL_spi_port2index(port)].config.sizeofrxframe) : (0);
    }
        
        // enable again
//        s_hal_spi_isr_enable(port);   

    
    return(res);    
}

static void s_hal_spi_isr_enable(hal_spi_port_t port)
{
    hal_spi_internals_t* spixint = &s_hal_spi_internals[HAL_spi_port2index(port)];
    SPI_TypeDef* SPIx = HAL_spi_port2stmSPI(port); 

    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
    spixint->txisenabled = 1;
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);
    spixint->rxisenabled = 1;
    
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_ERR, ENABLE);
    
    
 
    
//     if(hal_spi_dir_txrx == spixint->direction)
//     {
//         SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
//         SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);
//     }
//     else if(hal_spi_dir_txonly == spixint->direction)
//     {
//         SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);
//     }
//     else if(hal_spi_dir_rxonly == spixint->direction)
//     {
//         SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
//     }    
}


static void s_hal_spi_isr_disable(hal_spi_port_t port)
{
    hal_spi_internals_t* spixint = &s_hal_spi_internals[HAL_spi_port2index(port)];
    SPI_TypeDef* SPIx = HAL_spi_port2stmSPI(port); 
    
    //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE | SPI_I2S_IT_RXNE, DISABLE);    
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
    spixint->txisenabled = 0;
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
    spixint->rxisenabled = 0;
    
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_ERR, DISABLE);
}

static void s_hal_spi_periph_enable(hal_spi_port_t port)
{
    SPI_TypeDef* SPIx = HAL_spi_port2stmSPI(port); 
    SPI_Cmd(SPIx, ENABLE);
}

static void s_hal_spi_periph_disable(hal_spi_port_t port)
{
    SPI_TypeDef* SPIx = HAL_spi_port2stmSPI(port); 
    SPI_Cmd(SPIx, DISABLE);
}


static void s_hal_spi_nvic_enable(hal_spi_port_t port)
{
    IRQn_Type IRQn = (hal_spi_port1 == port) ? (SPI1_IRQn) : ( (hal_spi_port2 == port) ? (SPI2_IRQn) : (SPI3_IRQn) );
    hal_sys_irqn_enable(IRQn);
}

static void s_hal_spi_nvic_disable(hal_spi_port_t port)
{
    IRQn_Type IRQn = (hal_spi_port1 == port) ? (SPI1_IRQn) : ( (hal_spi_port2 == port) ? (SPI2_IRQn) : (SPI3_IRQn) );
    hal_sys_irqn_disable(IRQn);
}


static hal_result_t s_hal_spi_isr_txbyte_get(hal_spi_internals_t* spixint, uint8_t* txbyte, hal_bool_t* itisthelastoftheframe, hal_bool_t* itisthelastinsidefifo)
{
    // if the direction is rx-only we just transmit the dummy byte to clock out the spi   
    static uint8_t incr_mas = 0;
    static uint8_t incr_sla = 0;
    
    if(hal_spi_dir_rxonly == spixint->direction)
    {
        //*txbyte = spixint->config.dummytxvalue;
        if(hal_spi_ownership_slave == spixint->config.ownership)
        {
            *txbyte = spixint->config.dummytxvalue + incr_sla++;
        }
        else
        {
            *txbyte = spixint->config.dummytxvalue + incr_mas++;
        }
        *itisthelastoftheframe = hal_false;
        *itisthelastinsidefifo = hal_false;
        return(hal_res_OK);        
    }
    
    // otherwise if we have any kind of tx activity (txonly or txrx), then we get the byte from the fifo of tx frames
    
    if(NULL == spixint->txframe)
    {
        spixint->txframe =  hal_utility_fifo_front(&spixint->fifotx);
        spixint->txcount = 0;
        
        if(NULL == spixint->txframe)
        {
            //*txbyte = spixint->config.dummytxvalue;
                    if(hal_spi_ownership_slave == spixint->config.ownership)
                    {
                        *txbyte = spixint->config.dummytxvalue + incr_sla++;
                    }
                    else
                    {
                        *txbyte = spixint->config.dummytxvalue + incr_mas++;
                    }
            *itisthelastoftheframe = hal_false;
            *itisthelastinsidefifo = hal_false;  
            if(hal_spi_ownership_master == spixint->config.ownership)
            {   // error ... the master must have a frame to tx
                return(hal_res_NOK_generic);
            }
            else
            {
                return(hal_res_OK);
            }
        }        
    }
       
    *txbyte = spixint->txframe[spixint->txcount++];
    
    if(spixint->txcount == spixint->config.sizeoftxframe)
    {   // it is the last byte in the frame ...
                
        *itisthelastoftheframe = hal_true;
        
        hal_utility_fifo_pop(&spixint->fifotx);
        
        spixint->txframe = hal_utility_fifo_front(&spixint->fifotx);
        spixint->txcount = 0;    

        if(NULL == spixint->txframe)
        {
            *itisthelastinsidefifo = hal_true;
        }
        else
        {
            *itisthelastinsidefifo = hal_false;
        }
    }
    else
    {
        *itisthelastoftheframe = hal_false;
        *itisthelastinsidefifo = hal_false;
    }
    
    return(hal_res_OK);  
}


static hal_result_t s_hal_spi_isr_rxbyte_put(hal_spi_internals_t* spixint, uint8_t rxbyte, hal_bool_t* itisthelastoftheframe, hal_bool_t* fifoisfull)
{
    // if the direction is tx-only, then we dont store the rxbyte 
    
    if(hal_spi_dir_txonly == spixint->direction)
    {
        *itisthelastoftheframe = hal_false;
        *fifoisfull = hal_false;
        return(hal_res_OK);         
    }
    
    // otherwise if we have reception enabled, then we put the byte into the fifo of rx frames 
        
    if(NULL == spixint->rxframe)
    {
        spixint->rxframe = (hal_true == hal_utility_fifo_full(&spixint->fiforx)) ? (NULL) : (spixint->rxbuffer);
        spixint->rxcount = 0;
        
        if(NULL == spixint->rxframe)
        {
            *itisthelastoftheframe = hal_false;
            *fifoisfull = hal_true;
            return(hal_res_OK);  
        }        
    }
    
    
    // store the byte
    spixint->rxframe[spixint->rxcount++] = rxbyte;
    

    if(spixint->rxcount == spixint->config.sizeofrxframe)
    {   // it is the last byte in the frame ...
                
        *itisthelastoftheframe = hal_true;
        
        hal_utility_fifo_put(&spixint->fiforx, spixint->rxframe);
        
        spixint->rxframe = (hal_true == hal_utility_fifo_full(&spixint->fiforx)) ? (NULL) : (spixint->rxbuffer);
        spixint->rxcount = 0;    

        if(NULL == spixint->rxframe)
        {
            *fifoisfull = hal_true;
        }
        else
        {
            *fifoisfull = hal_false;
        }
    }
    else
    {
        *itisthelastoftheframe = hal_false;
        *fifoisfull = hal_false;
    }
    
    return(hal_res_OK);  
}

#endif//HAL_USE_SPI

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



