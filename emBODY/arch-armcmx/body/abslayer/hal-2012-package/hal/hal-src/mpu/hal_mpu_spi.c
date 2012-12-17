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
    .ownership              = hal_spi_ownership_master, 
    .dir                    = hal_spi_dir_txrx, 
    .activity               = hal_spi_act_singleframe,
    .speed                  = hal_spi_speed_1mbps, 
    .sizeoftxframe          = 4, 
    .sizeofrxframe          = 4, 
    .onframetransm          = NULL, 
    .onframereceiv          = NULL,
    .sizeoftxfifoofframes   = 4,
    .sizeofrxfifoofframes   = 4
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

typedef struct
{
    hal_bool_t          locked;
    hal_bool_t          forcestop;
    hal_spi_cfg_t       config;
    uint8_t*            txframe;
    uint8_t             txcount;
    uint8_t*            rxframe;
    uint8_t             rxcount;
    hal_utility_fifo_t  fifotx;
    hal_utility_fifo_t  fiforx;
    uint8_t*            rxbuffer;
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
static void s_hal_spi_hw_gpio_init(hal_spi_port_t port);
static void s_hal_spi_hw_enable(hal_spi_port_t port, const hal_spi_cfg_t* cfg);

static hal_result_t s_hal_spi_put(hal_spi_port_t port, uint8_t* txframe, uint8_t size, hal_bool_t sendnow);

static hal_result_t s_hal_spi_get(hal_spi_port_t port, uint8_t* rxframe, uint8_t* size, uint8_t* remainingrxframes);

static void s_hal_spi_nvic_enable(hal_spi_port_t port);
static void s_hal_spi_nvic_disable(hal_spi_port_t port);

static void s_hal_spi_enable(hal_spi_port_t port);
static void s_hal_spi_disable(hal_spi_port_t port);

static hal_result_t s_hal_spi_timeoutexpired(void);

static void s_hal_spi_theisr(SPI_TypeDef* SPIx, hal_spi_internals_t* spixint);

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
    .SPI_DataSize                   = SPI_DataSize_8b,
    .SPI_CPOL                       = SPI_CPOL_High,
    .SPI_CPHA                       = SPI_CPHA_2Edge,
    .SPI_NSS                        = SPI_NSS_Soft,
    .SPI_BaudRatePrescaler          = SPI_BaudRatePrescaler_2,                      // param: depends on speed
    .SPI_FirstBit                   = SPI_FirstBit_MSB,
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
    
    SPI_I2S_SendData(SPIx, byte);
    
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
   
    return(hal_res_OK);     
}


// extern hal_result_t hal_spi_read(hal_spi_port_t port, uint8_t* byte)
// {
//     if(hal_false == hal_spi_hid_initted_is(port))
//     {
//         return(hal_res_NOK_generic);
//     }

//     *byte = 0;
//     
//     return (hal_res_OK);       
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
    
    if(0 == size)
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
       

    s_hal_spi_internals[HAL_spi_port2index(port)].forcestop = hal_false;
    s_hal_spi_enable(port);
    
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
       

    s_hal_spi_internals[HAL_spi_port2index(port)].forcestop = hal_true;
    //s_hal_spi_disable(port);
    
    return(hal_res_OK);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void SPI1_IRQHandler(void)
{
    static hal_spi_internals_t* spi1int = &s_hal_spi_internals[HAL_spi_port2index(hal_spi_port1)];
    static hal_bool_t lastbyteofframeisintransmission = hal_false;
    
    if(SET == SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE))
    {   // master tx
        if(NULL == spi1int->txframe)
        {   // either i have a new frame or i have finished one at the previous call
            if(hal_true == lastbyteofframeisintransmission)
            {
                lastbyteofframeisintransmission = hal_false;
                if(NULL != spi1int->config.onframetransm)
                {
                    spi1int->config.onframetransm(NULL);
                }                
            }
            if(hal_false == spi1int->forcestop)
            {
                spi1int->txframe =  hal_utility_fifo_front(&spi1int->fifotx);
                spi1int->txcount = 0;
            }
        }
        
        if(NULL != spi1int->txframe)
        {   // transmit
            uint8_t index = spi1int->txcount++;
        
            if(index < spi1int->config.sizeoftxframe)
            {             
                SPI_I2S_SendData(SPI1, spi1int->txframe[index]);              
            }
            
            if(spi1int->txcount == spi1int->config.sizeoftxframe)
            {   // it is the last byte in the frame ...
                
                hal_utility_fifo_pop(&spi1int->fifotx);
                lastbyteofframeisintransmission = hal_true;
                
                spi1int->txframe = NULL;
                spi1int->txcount = 0;                
            }
        }
        else
        {   // dont have any more frame in fifo-tx
            if((hal_spi_act_continuous == spi1int->config.activity) && (hal_false == spi1int->forcestop))
            {   // if tx is continuous we just send a zero byte
                SPI_I2S_SendData(SPI1, 0);
            }
            else
            {   // else we disable tx
                spi1int->forcestop = hal_false;
                SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
            }
        }        

    }  
}


void SPI2_IRQHandler(void)
{
    static hal_spi_internals_t* spi2int = &s_hal_spi_internals[HAL_spi_port2index(hal_spi_port2)];
    static hal_bool_t lastbyteofframeisintransmission = hal_false;
    static hal_bool_t lastbyteofframeisreceived = hal_false;
   
    if(SET == SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE))
    {
        //uint8_t rxbyte = (uint8_t)SPI_I2S_ReceiveData(SPI2);
        
        if(NULL == spi2int->rxframe)
        {   // either i have a new frame or i have finished one at the previous call
            if(hal_true == lastbyteofframeisreceived)
            {
                lastbyteofframeisreceived = hal_false;
                if(NULL != spi2int->config.onframereceiv)
                {
                    spi2int->config.onframereceiv(NULL); // address of received byte
                }                
            }
            
            //if(hal_false == spi2int->forcestoprx)
            //{
                spi2int->rxframe = (hal_true ==  hal_utility_fifo_full(&spi2int->fiforx)) ? (NULL) : (spi2int->rxbuffer);
                spi2int->rxcount = 0;
            //}
        }  

        if(NULL != spi2int->rxframe)
        {   // can put inside the fifo   
            uint8_t index = spi2int->rxcount++;
        
            if(index < spi2int->config.sizeofrxframe)
            {             
                spi2int->rxframe[index] = (uint8_t)SPI_I2S_ReceiveData(SPI2);              
            }
            
            if(spi2int->rxcount == spi2int->config.sizeofrxframe)
            {   // it is the last byte in the frame ...
                
                 hal_utility_fifo_put(&spi2int->fiforx, spi2int->rxframe);
                lastbyteofframeisreceived = hal_true;
                
                spi2int->rxframe = NULL;
                spi2int->rxcount = 0;                
            }
        }
        else
        {   // dont have any more space in fifo-rx
            //if((hal_spi_act_continuous == spi2int->config.activity) && (hal_false == spi2int->forcestoprx))
            //{   // if tx is continuous we just retrieve the byte but we dont put inside the frame-rx
                uint8_t byte = (uint8_t)SPI_I2S_ReceiveData(SPI2); 
            //}
            //else
            //{   // else we disable rx
            //    spi2int->forcestoprx = hal_false;
            //    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
            //}
        }        
    
    
    
//    }
    
 //   if(SET == SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE))
//    {   // master tx
        if(NULL == spi2int->txframe)
        {   // either i have a new frame or i have finished one at the previous call
            if(hal_true == lastbyteofframeisintransmission)
            {
                lastbyteofframeisintransmission = hal_false;
                if(NULL != spi2int->config.onframetransm)
                {
                    spi2int->config.onframetransm(NULL);
                }                
            }
            if(hal_false == spi2int->forcestop)
            {
                spi2int->txframe =  hal_utility_fifo_front(&spi2int->fifotx);
                spi2int->txcount = 0;
            }
        }
        
        if(NULL != spi2int->txframe)
        {   // transmit   
            uint8_t index = spi2int->txcount++;
        
            if(index < spi2int->config.sizeoftxframe)
            {             
                SPI_I2S_SendData(SPI2, spi2int->txframe[index]);              
            }
            
            if(spi2int->txcount == spi2int->config.sizeoftxframe)
            {   // it is the last byte in the frame ...
                
                 hal_utility_fifo_pop(&spi2int->fifotx);
                lastbyteofframeisintransmission = hal_true;
                
                spi2int->txframe = NULL;
                spi2int->txcount = 0;                
            }
        }
        else
        {   // dont have any more frame in fifo-tx
            if((hal_spi_act_continuous == spi2int->config.activity) && (hal_false == spi2int->forcestop))
            {   // if tx is continuous we just send a zero byte
                SPI_I2S_SendData(SPI2, 0);
            }
            else
            {   // else we disable tx
                spi2int->forcestop = hal_false;
                SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
            }
        }
    } 
        
   
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

#if 0
static void s_hal_spi_theisr(SPI_TypeDef* SPIx, hal_spi_internals_t* spixint)
{
    devo rendere piou' snella la procedura.
    chiamo uan vola sola la isr per tramettere un byte e se ho finito non chiamo piu'.
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

//static void s_hal_spi_theisr_safe_but_slow(SPI_TypeDef* SPIx, hal_spi_internals_t* spixint)
static void s_hal_spi_theisr(SPI_TypeDef* SPIx, hal_spi_internals_t* spixint)
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
    
    s_hal_spi_hw_gpio_init(port);
    s_hal_spi_hw_init(port);
    s_hal_spi_hw_enable(port, cfg);
    
    if(hal_spi_act_singleframe == cfg->activity)
    {
        s_hal_spi_nvic_enable(port);
    }
        
    
    s_hal_spi_initted_set(port);
    
    memcpy(&s_hal_spi_internals[HAL_spi_port2index(port)].config, cfg, sizeof(hal_spi_cfg_t));
    
    #warning --> remove teh calloc from here 
    hal_utility_fifo_init(&s_hal_spi_internals[HAL_spi_port2index(port)].fifotx, cfg->sizeoftxfifoofframes, cfg->sizeoftxframe, calloc(cfg->sizeoftxfifoofframes*cfg->sizeoftxframe, 1), NULL);
    hal_utility_fifo_init(&s_hal_spi_internals[HAL_spi_port2index(port)].fiforx, cfg->sizeofrxfifoofframes, cfg->sizeofrxframe, calloc(cfg->sizeofrxfifoofframes*cfg->sizeofrxframe, 1), NULL);
    
    s_hal_spi_internals[HAL_spi_port2index(port)].rxbuffer =  calloc(cfg->sizeofrxframe, 1);
    s_hal_spi_internals[HAL_spi_port2index(port)].txframe = NULL;
    s_hal_spi_internals[HAL_spi_port2index(port)].txcount = 0;
    s_hal_spi_internals[HAL_spi_port2index(port)].forcestop = hal_false;
    
    s_hal_spi_internals[HAL_spi_port2index(port)].rxframe = NULL;
    s_hal_spi_internals[HAL_spi_port2index(port)].rxcount = 0;
  
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


static void s_hal_spi_hw_gpio_init(hal_spi_port_t port)
{
    
#if     defined(USE_STM32F1)

    static const GPIO_InitTypeDef  s_hal_spi_sckmosi_altcfg  =
    {
        .GPIO_Pin       = 0,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_Mode      = GPIO_Mode_AF_PP, // il PP serve per spi del display3 .... GPIO_Mode_AF_OD,
    }; 

    static const GPIO_InitTypeDef  s_hal_spi_miso_altcfg  =
    {
        .GPIO_Pin       = 0,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_Mode      = GPIO_Mode_IPD, // oppure ... GPIO_Mode_IPU?
    };     
    

    // 1. prepare af.
    // for spi1 (miso, mosi): no-remap if it is (PB6, PB7). GPIO_Remap_SPI1 if it is (PB8, PB9).
    // for spi2 (miso, mosi): no remap if it is (PB10, PB11).
    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t found = hal_false;

    
    hal_gpio_port_t portmiso = hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinmiso  = hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_port2index(port)].pin;
    hal_gpio_port_t portmosi = hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinmosi  = hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_port2index(port)].pin;    
    hal_gpio_port_t portsck  = hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinsck   = hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_port2index(port)].pin;       
    
    if(hal_spi_port1 == port)
    {        
       if((hal_gpio_portA == portmiso) && (hal_gpio_portA == portmosi) && (hal_gpio_portA == portsck) && (hal_gpio_pin6 == pinmiso) && (hal_gpio_pin7 == pinmosi) && (hal_gpio_pin5 == pinsck))
        {
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }
        else if((hal_gpio_portB == portmiso) && (hal_gpio_portB == portmosi) && (hal_gpio_portB == portsck) && (hal_gpio_pin4 == pinmiso) && (hal_gpio_pin5 == pinmosi) && (hal_gpio_pin3 == pinsck))
        {
            afname = GPIO_Remap_SPI1;       afmode = ENABLE;                    found = hal_true;
        }               
    }
    else if(hal_spi_port2 == port)
    {
        if((hal_gpio_portB == portmiso) && (hal_gpio_portB == portmosi) && (hal_gpio_portB == portsck) && (hal_gpio_pin14 == pinmiso) && (hal_gpio_pin15 == pinmosi) && (hal_gpio_pin13 == pinsck))
        {
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }        
    }
    else if(hal_spi_port3 == port)
    {
        if((hal_gpio_portB == portmiso) && (hal_gpio_portB == portmosi) && (hal_gpio_portB == portsck) && (hal_gpio_pin4 == pinmiso) && (hal_gpio_pin5 == pinmosi) && (hal_gpio_pin3 == pinsck))
        {
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }
        else if((hal_gpio_portC == portmiso) && (hal_gpio_portC == portmosi) && (hal_gpio_portC == portsck) && (hal_gpio_pin11 == pinmiso) && (hal_gpio_pin12 == pinmosi) && (hal_gpio_pin10 == pinsck))
        {
            afname = GPIO_Remap_SPI3;       afmode = ENABLE;                    found = hal_true;
        }       
    }
    
    if(hal_false == found)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_spi_init(): incorrect pin mapping");
    }

    hal_gpio_altcfg_t hal_spi_miso_altcfg;
    hal_gpio_altcfg_t hal_spi_mosi_altcfg;
    hal_gpio_altcfg_t hal_spi_sck_altcfg;
    
    // prepare the altcfg for miso, mosi, sck pins
    memcpy(&hal_spi_miso_altcfg, &s_hal_spi_miso_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_spi_mosi_altcfg, &s_hal_spi_sckmosi_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_spi_sck_altcfg, &s_hal_spi_sckmosi_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_spi_miso_altcfg.afname = hal_spi_mosi_altcfg.afname = hal_spi_sck_altcfg.afname = afname;
    hal_spi_miso_altcfg.afmode = hal_spi_mosi_altcfg.afmode = hal_spi_sck_altcfg.afname = afmode;
    
    // configure miso, mosi, sck pins
    hal_gpio_configure(hal_brdcfg_spi__theconfig.gpio_miso[HAL_spi_port2index(port)], &hal_spi_miso_altcfg);    
    hal_gpio_configure(hal_brdcfg_spi__theconfig.gpio_mosi[HAL_spi_port2index(port)], &hal_spi_mosi_altcfg);
    hal_gpio_configure(hal_brdcfg_spi__theconfig.gpio_sck[HAL_spi_port2index(port)], &hal_spi_sck_altcfg);

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
    // for spi1 (miso, mosi, sck): miso -> PA6, PB4. mosi -> PA7, PB5. SCK -> PA5, PB3
    // for spi2 (miso, mosi, sck): miso -> PAB14, PC2, PI2. mosi -> PB15, PC3, PI3. SCK -> PB10, PB13, PI1
    // for spi3 (miso, mosi, sck): miso -> PB4, PC11. mosi -> PB5, PC12. SCK -> PB3, PC10

    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t foundmiso = hal_false;
    hal_bool_t foundmosi = hal_false;
    hal_bool_t foundsck  = hal_false;

    
    hal_gpio_port_t portmiso = hal_brdcfg_spi__miso[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinmiso  = hal_brdcfg_spi__miso[HAL_spi_port2index(port)].pin;
    hal_gpio_port_t portmosi = hal_brdcfg_spi__mosi[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinmosi  = hal_brdcfg_spi__mosi[HAL_spi_port2index(port)].pin;       
    hal_gpio_port_t portsck  = hal_brdcfg_spi__sck[HAL_spi_port2index(port)].port;
    hal_gpio_pin_t  pinsck   = hal_brdcfg_spi__sck[HAL_spi_port2index(port)].pin;       
    
    if(hal_spi_port1 == port)
    { 
        afname = GPIO_AF_SPI1;      afmode = ENABLE;
        
        if( ((hal_gpio_portA == portmiso) && (hal_gpio_pin6 == pinmiso))  ||
            ((hal_gpio_portB == portmiso) && (hal_gpio_pin4 == pinmiso))  )
        {
            foundmiso = hal_true;
        }
        
        if( ((hal_gpio_portA == portmosi) && (hal_gpio_pin7 == pinmosi))  ||
            ((hal_gpio_portB == portmosi) && (hal_gpio_pin5 == pinmosi))  )
        {
            foundmosi = hal_true;
        }
        
        if( ((hal_gpio_portA == portsck) && (hal_gpio_pin5 == pinsck))  ||
            ((hal_gpio_portB == portsck) && (hal_gpio_pin3 == pinsck))  )
        {
            foundsck = hal_true;
        }        
    }
    else if(hal_spi_port2 == port)
    {   
        afname = GPIO_AF_SPI2;      afmode = ENABLE;
        
        if( ((hal_gpio_portB == portmiso) && (hal_gpio_pin14 == pinmiso))  ||
            ((hal_gpio_portC == portmiso) && (hal_gpio_pin2 == pinmiso))  ||
            ((hal_gpio_portI == portmiso) && (hal_gpio_pin2 == pinmiso)) )
        {
            foundmiso = hal_true;
        }
        
        if( ((hal_gpio_portB == portmosi) && (hal_gpio_pin15 == pinmosi))  ||
            ((hal_gpio_portC == portmosi) && (hal_gpio_pin3 == pinmosi))  ||
            ((hal_gpio_portI == portmosi) && (hal_gpio_pin3 == pinmosi)) )
        {
            foundmosi = hal_true;
        }
        
        if( ((hal_gpio_portB == portsck) && (hal_gpio_pin10 == pinsck))  ||
            ((hal_gpio_portB == portsck) && (hal_gpio_pin13 == pinsck))  ||
            ((hal_gpio_portI == portsck) && (hal_gpio_pin1 == pinsck)) )
        {
            foundsck = hal_true;
        }          
    }
    else if(hal_spi_port3 == port)
    {
        afname = GPIO_AF_SPI3;      afmode = ENABLE;
        
        if( ((hal_gpio_portB == portmiso) && (hal_gpio_pin4 == pinmiso))  ||
            ((hal_gpio_portC == portmiso) && (hal_gpio_pin11 == pinmiso))  )
        {
            foundmiso = hal_true;
        }
        
        if( ((hal_gpio_portB == portmosi) && (hal_gpio_pin5 == pinmosi))  ||
            ((hal_gpio_portC == portmosi) && (hal_gpio_pin12 == pinmosi))  )
        {
            foundmosi = hal_true;
        }    

        if( ((hal_gpio_portB == portsck) && (hal_gpio_pin3 == pinsck))  ||
            ((hal_gpio_portC == portsck) && (hal_gpio_pin10 == pinsck))  )
        {
            foundsck = hal_true;
        }       
    }
    
    if((hal_false == foundmiso) || (hal_false == foundmosi) || (hal_false == foundsck))
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_spi_init(): incorrect pin mapping");
    }
    
   
    hal_gpio_altcfg_t hal_spi_miso_altcfg;
    hal_gpio_altcfg_t hal_spi_mosi_altcfg;
    hal_gpio_altcfg_t hal_spi_sck_altcfg;
    
    // prepare the altcfg for miso mosi sck pins
    memcpy(&hal_spi_miso_altcfg, &s_hal_spi_misomosisck_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_spi_mosi_altcfg, &s_hal_spi_misomosisck_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_spi_sck_altcfg, &s_hal_spi_misomosisck_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_spi_miso_altcfg.afname = hal_spi_mosi_altcfg.afname = hal_spi_sck_altcfg.afname = afname;
    hal_spi_miso_altcfg.afmode = hal_spi_mosi_altcfg.afmode = hal_spi_sck_altcfg.afmode = afmode;
    
    
    // configure miso mosi sck pins
    hal_gpio_configure(hal_brdcfg_spi__miso[HAL_spi_port2index(port)], &hal_spi_miso_altcfg);    
    hal_gpio_configure(hal_brdcfg_spi__mosi[HAL_spi_port2index(port)], &hal_spi_mosi_altcfg);    
    hal_gpio_configure(hal_brdcfg_spi__sck[HAL_spi_port2index(port)], &hal_spi_sck_altcfg); 
    
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
    spi_cfg.SPI_BaudRatePrescaler   = SPI_BaudRatePrescaler_2;
 
    SPI_Init(SPIx, &spi_cfg);
    
    // spi peripheral enable
    SPI_Cmd(SPIx, ENABLE);
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
    s_hal_spi_disable(port);
    res =  hal_utility_fifo_put(&s_hal_spi_internals[HAL_spi_port2index(port)].fifotx, txframe);
    
    if(hal_true == sendnow)
    {
        s_hal_spi_internals[HAL_spi_port2index(port)].forcestop = hal_false;
        // enable again
        s_hal_spi_enable(port);   
    }
    
    return(res);
}

static hal_result_t s_hal_spi_get(hal_spi_port_t port, uint8_t* rxframe, uint8_t* size, uint8_t* remainingrxframes)
{
    hal_result_t res = hal_res_NOK_generic;
    const hal_spi_cfg_t* cfg = &s_hal_spi_internals[HAL_spi_port2index(port)].config;
 
    // disable so that we can change a data structure which is used by the isr
    #warning --> devo pensare a come proteggere l'accessso alla fiforx da accessi concorrenti
//    s_hal_spi_disable(port);
    res =  hal_utility_fifo_get(&s_hal_spi_internals[HAL_spi_port2index(port)].fiforx, rxframe, size);
    
    if(NULL != remainingrxframes)
    {
        *remainingrxframes = hal_utility_fifo_size(&s_hal_spi_internals[HAL_spi_port2index(port)].fiforx);
    }
        
//        s_hal_spi_internals[HAL_spi_port2index(port)].forcestop = hal_false;
        // enable again
//        s_hal_spi_enable(port);   

    
    return(res);    
}

static void s_hal_spi_enable(hal_spi_port_t port)
{
    SPI_TypeDef* SPIx = HAL_spi_port2stmSPI(port); 
    //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);    
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);
}

static void s_hal_spi_disable(hal_spi_port_t port)
{
    SPI_TypeDef* SPIx = HAL_spi_port2stmSPI(port); 
    //SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE | SPI_I2S_IT_RXNE, DISABLE);    
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
    SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
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



#endif//HAL_USE_SPI

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



