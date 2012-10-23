    /*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       hal_stm32_can._optimizedc
	@brief      This file implements internal implementation of the hal can module in optimized way.
	@author     valentina.gaggero@iit.it
    @date       02/16/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_CAN

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"

#include "hal_stm32xx_include.h"

#include "string.h"

#include "hal_brdcfg.h"
#include "hal_stm32_base_hid.h" 
#include "hal_stm32_gpio_hid.h" 
#include "hal_stm32_sys_hid.h"
#include "hal_stm32_canfifo_hid.h"

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_can.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_can_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_can_port2index(p)           ((uint8_t)((p)))


#if     defined(USE_STM32F1)
    // the clock APB1 is 36mhz, half the max frequency of 72 mhz. (see page 49/1096 of stm32f1x reference manual Doc ID 13902 Rev 14) 
    // the clock APB2 operates at full speed of 72 mhz. (see page 49/1096 of stm32f1x reference manual Doc ID 13902 Rev 14)
    // we give a total of 9 time quanta for the duration of a can bit. we split in 1+5+3
    #define HAL_STM32_CAN_CLK	  		    36000000
    #define HAL_STM32_CAN_TQ_TOTAL          9
    #define HAL_STM32_CAN_TQ_SJW            CAN_SJW_3tq
    #define HAL_STM32_CAN_TQ_BS1            CAN_BS1_5tq
    #define HAL_STM32_CAN_TQ_BS2            CAN_BS2_3tq
#elif    defined(USE_STM32F4)
    // the clock APB1 is 42mhz, a fourth the max frequency of 168 mhz. (see page 23/180 of stm32f4x datasheet Doc ID 022152 Rev 3)
    // the clock APB2 is 84mhz, a halh the max frequency of 168 mhz. (see page 23/180 of stm32f4x datasheet Doc ID 022152 Rev 3)
    // we give a total of 7 time quanta for the duration of a can bit. we split in 1+4+2
    #define HAL_STM32_CAN_CLK	  			42000000
    #define HAL_STM32_CAN_TQ_TOTAL          7
    #define HAL_STM32_CAN_TQ_SJW            CAN_SJW_3tq
    #define HAL_STM32_CAN_TQ_BS1            CAN_BS1_4tq
    #define HAL_STM32_CAN_TQ_BS2            CAN_BS2_2tq    
#endif

#define HAL_can_port2peripheral(p)     ( ( hal_can_port1 == (p) ) ? (CAN1) : (CAN2) )



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_can_cfg_t hal_can_cfg_default =
{
    .runmode            = hal_can_runmode_isr_1txq1rxq,
    .baudrate           = hal_can_baudrate_1mbps, 
    .priorx             = hal_int_priority06,
    .priotx             = hal_int_priority06,
    .callback_on_rx     = NULL,
    .arg_cb_rx          = NULL,
    .callback_on_tx     = NULL,
    .arg_cb_tx          = NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_can_cfg_t               cfg;
    hal_canfifo_t               canframes_rx_norm;
    hal_canfifo_t               canframes_tx_norm;
    uint8_t                     enabled;
} hal_can_portdatastructure_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_can_supported_is(hal_can_port_t port);
static void s_hal_can_initted_set(hal_can_port_t port);
static hal_boolval_t s_hal_can_initted_is(hal_can_port_t port);


// hw related initialisation whcih may change with different versions of stm32fx mpus
static void s_hal_can_hw_gpio_init(hal_can_port_t port);
static void s_hal_can_hw_clock_init(hal_can_port_t port);
static void s_hal_can_hw_nvic_init(hal_can_port_t port);
static hal_result_t s_hal_can_hw_registers_init(hal_can_port_t port);

// trasmission
static void s_hal_can_isr_sendframes_canx(hal_can_port_t port);
static hal_result_t s_hal_can_tx_normprio(hal_can_port_t port, hal_can_frame_t *frame, hal_can_send_mode_t sm);

// reception
static void s_hal_can_isr_recvframe_canx(hal_can_port_t port);

// interrupt
static void s_hal_can_isr_tx_enable(hal_can_port_t port);
static void s_hal_can_isr_tx_disable(hal_can_port_t port);
static void s_hal_can_isr_rx_enable(hal_can_port_t port);
static void s_hal_can_isr_rx_disable(hal_can_port_t port);

// enable or disab


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_can_portdatastructure_t *s_hal_can_portdatastruct_ptr[hal_can_ports_num] = {NULL, NULL};

static hal_boolval_t s_hal_can_initted[hal_can_ports_num] = { hal_false };


#if     defined(USE_STM32F1)
static const hal_gpio_altcfg_t s_hal_can_canx_rx_altcfg         = 
{
    .gpioext    =
    {
        .GPIO_Pin       = 0,
        .GPIO_Speed     = GPIO_Speed_2MHz,
        .GPIO_Mode      = GPIO_Mode_IPU,  
    },
    .afname     = HAL_GPIO_AFNAME_NONE,
    .afmode     = HAL_GPIO_AFMODE_NONE
};
static const hal_gpio_altcfg_t s_hal_can_canx_tx_altcfg = 
{
    .gpioext    =
    {
        .GPIO_Pin       = 0,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_Mode      = GPIO_Mode_AF_PP,  
    },
    .afname     = HAL_GPIO_AFNAME_NONE,
    .afmode     = HAL_GPIO_AFMODE_NONE
};


// static const GPIO_InitTypeDef s_hal_can_gpio_inittypeded_canx_rx      =
// {
//     .GPIO_Pin       = 0,
//     .GPIO_Mode      = GPIO_Mode_IPU,
//     .GPIO_Speed     = GPIO_Speed_2MHz
// };
// static const GPIO_InitTypeDef s_hal_can_gpio_inittypeded_canx_tx      =
// {
//     .GPIO_Pin       = 0,
//     .GPIO_Mode      = GPIO_Mode_AF_PP,
//     .GPIO_Speed     = GPIO_Speed_50MHz
// };
#elif   defined(USE_STM32F4)

static const hal_gpio_altcfg_t s_hal_can_canx_rx_altcfg         = 
{
    .gpioext    =
    {
        .GPIO_Pin       = 0,
        .GPIO_Mode      = GPIO_Mode_AF,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_OType     = GPIO_OType_PP,
        .GPIO_PuPd      = GPIO_PuPd_UP
    },
    .afname     = HAL_GPIO_AFNAME_NONE,
    .afmode     = HAL_GPIO_AFMODE_NONE
};
static const hal_gpio_altcfg_t s_hal_can_canx_tx_altcfg = 
{
    .gpioext    =
    {
        .GPIO_Pin       = 0,
        .GPIO_Mode      = GPIO_Mode_AF,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_OType     = GPIO_OType_PP,
        .GPIO_PuPd      = GPIO_PuPd_UP
    },
    .afname     = HAL_GPIO_AFNAME_NONE,
    .afmode     = HAL_GPIO_AFMODE_NONE
};

// static const GPIO_InitTypeDef s_hal_can_gpio_inittypeded_canx_rx      =
// {
//     .GPIO_Pin       = 0,
//     .GPIO_Mode      = GPIO_Mode_AF,
//     .GPIO_Speed     = GPIO_Speed_50MHz,
//     .GPIO_OType     = GPIO_OType_PP,
//     .GPIO_PuPd      = GPIO_PuPd_UP
// };
// static const GPIO_InitTypeDef s_hal_can_gpio_inittypeded_canx_tx      =
// {
//     .GPIO_Pin       = 0,
//     .GPIO_Mode      = GPIO_Mode_AF,
//     .GPIO_Speed     = GPIO_Speed_50MHz,
//     .GPIO_OType     = GPIO_OType_PP,
//     .GPIO_PuPd      = GPIO_PuPd_UP
// };
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t hal_can_init(hal_can_port_t port, const hal_can_cfg_t *cfg)
{
	hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    if(hal_false == s_hal_can_supported_is(port))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == cport)
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == cfg)
    {
        cfg  = &hal_can_cfg_default;
    }

    if(hal_can_runmode_isr_1txq1rxq != cfg->runmode)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "hal_can_init():wrong runmode");
        return(hal_res_NOK_unsupported);  
    }
   
    // set config
    memcpy(&cport->cfg, cfg, sizeof(hal_can_cfg_t));

    
    //  reset fifos
    hal_canfifo_hid_reset(&cport->canframes_rx_norm);
    hal_canfifo_hid_reset(&cport->canframes_tx_norm);
    
    // init the phy of can
    hal_brdcfg_can__phydevices_init(port);
    
    // enable the phy of can
    hal_brdcfg_can__phydevices_enable(port); 
    
    // init gpios
    s_hal_can_hw_gpio_init(port);
    
    // init clock
    s_hal_can_hw_clock_init(port);
    

    s_hal_can_hw_nvic_init(port); /* NVIC configuration */

    cport->enabled = 0;

    s_hal_can_initted_set(port);

    return(hal_res_OK);
}


extern hal_result_t hal_can_enable(hal_can_port_t port)
{
	hal_result_t res;
	hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == cport)
    {
        return(hal_res_NOK_generic);
    }

    res = s_hal_can_hw_registers_init(port); // hardware setup

    if(res != hal_res_OK)
    {
    	return(hal_res_NOK_generic);
    }


    // disable scheduling
    hal_base_hid_osal_scheduling_suspend();

    cport->enabled = 1;

    // configure interrupts on rx (CAN_IT_FMP0 -> FIFO 0 message pending) and tx (CAN_IT_TME -> transmit mailbox empty)
    CAN_ITConfig(HAL_can_port2peripheral(port), (CAN_IT_FMP0 | CAN_IT_TME), ENABLE);

    // nvic 
    s_hal_can_isr_rx_enable(port);
    // dont enable the nvic for the tx
    //s_hal_can_tx_enable(port);
    s_hal_can_isr_tx_disable(port);

    // enable scheduling 
    hal_base_hid_osal_scheduling_restart();

	return(hal_res_OK);
}


extern hal_result_t hal_can_disable(hal_can_port_t port) 
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == cport)
    {
        return(hal_res_NOK_generic);
    }

    // disable scheduling
    hal_base_hid_osal_scheduling_suspend();

    cport->enabled = 0;

    // deconfigure interrupts on rx (CAN_IT_FMP0 -> FIFO 0 message pending) and tx (CAN_IT_TME -> transmit mailbox empty)
    CAN_ITConfig(HAL_can_port2peripheral(port), (CAN_IT_FMP0 | CAN_IT_TME), DISABLE);

    // nvic 
    s_hal_can_isr_rx_disable(port);
    // dont disable the nvic for the tx. it was never enabled
    s_hal_can_isr_tx_disable(port);
    
    // enable scheduling 
    hal_base_hid_osal_scheduling_restart();

    return(hal_res_OK);
}

// the function doesn't check hal_can_send_mode: the function will behave ok only if sm == hal_can_send_normprio_now
extern hal_result_t hal_can_put(hal_can_port_t port, hal_can_frame_t *frame, hal_can_send_mode_t sm) 
{
    return (s_hal_can_tx_normprio(port, frame, sm));
}



extern hal_result_t hal_can_transmit(hal_can_port_t port)
{
    // trigger the tx. the trigger will completely empty the canframes_tx_norm.
    s_hal_can_isr_sendframes_canx(port);
    return(hal_res_OK);
}


extern hal_result_t hal_can_received(hal_can_port_t port, uint8_t *numberof) 
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    // disable interrupt rx
    s_hal_can_isr_rx_disable(port);
    
    *numberof = hal_canfifo_hid_size(&cport->canframes_rx_norm);
    
    // enable interrupt rx
    s_hal_can_isr_rx_enable(port);
    
    return(hal_res_OK);
}


extern hal_result_t hal_can_get(hal_can_port_t port, hal_can_frame_t *frame, uint8_t *remaining) 
{
    hal_can_portdatastructure_t *cport  =   s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    hal_canfifo_item_t *fifoframe_ptr   =   NULL;
  
    // disable interrupt rx
    s_hal_can_isr_rx_disable(port);

    fifoframe_ptr = hal_canfifo_hid_front(&cport->canframes_rx_norm);
    if(NULL == fifoframe_ptr)
    {
        // enable interrupt rx
        s_hal_can_isr_rx_enable(port);
        return(hal_res_NOK_nodata); //the fifo is empty
    }
    
    memcpy(frame, fifoframe_ptr, sizeof(hal_canfifo_item_t));

    hal_canfifo_hid_pop(&cport->canframes_rx_norm);
    // enable interrupt rx
    s_hal_can_isr_rx_enable(port);
    
    
    if(NULL != remaining)
    {
        *remaining = hal_canfifo_hid_size(&cport->canframes_rx_norm);
    }

    return(hal_res_OK);
}


extern hal_result_t hal_can_receptionfilter_set(hal_can_port_t port, uint8_t mask_num, uint32_t mask_val, uint8_t identifier_num,
                                                uint32_t identifier_val, hal_can_frameID_format_t idformat)
{
    //CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    
    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }


//	/* CAN filter init */
//	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
//
//    if(hal_can_frameID_std == idformat)
//    {
//    	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
//    }
//    else
//    {
//        CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
//    }
//
//	CAN_FilterInitStructure.CAN_FilterIdHigh = (identifier_val & 0xFFFF0000) >> 8;
//	CAN_FilterInitStructure.CAN_FilterIdLow = (identifier_val & 0x0000FFFF);
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (mask_val & 0xFFFF0000) >> 8;
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (mask_val & 0x0000FFFF);
//
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
//	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
//
//	if(hal_can_port1 == port)
//	{
//		CAN_FilterInitStructure.CAN_FilterNumber = mask_num;
//		CAN_FilterInit(&CAN_FilterInitStructure);
//	}
//	else
//	{
//    	CAN_FilterInitStructure.CAN_FilterNumber = 14 + mask_num;
//    	CAN_FilterInit(&CAN_FilterInitStructure);
//	}

    //hal_base_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "hal_can_receptionfilter_set(): not supported");
    return(hal_res_NOK_unsupported);
}

extern hal_result_t hal_can_out_get(hal_can_port_t port, uint8_t *numberof) 
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    
    if(NULL == numberof)
    {
        return(hal_res_NOK_generic);
    }

    // disable interrupt rx
    s_hal_can_isr_rx_disable(port);
    
    *numberof = hal_canfifo_hid_size(&cport->canframes_tx_norm);
    
    // enable interrupt rx
    s_hal_can_isr_rx_enable(port);
    
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void CAN1_TX_IRQHandler	(void)
{
    // as we configured with CAN_IT_TME, the isr is triggered when any of the output mailboxes in can-1 peripheral gets empty. 
    s_hal_can_isr_sendframes_canx(hal_can_port1);
}


void CAN2_TX_IRQHandler	(void)
{
    // as we configured with CAN_IT_TME, the isr is triggered when any of the output mailboxes in can2 peripheral gets empty.
	s_hal_can_isr_sendframes_canx(hal_can_port2);
}

void CAN1_RX0_IRQHandler(void)
{
    // as we configured with CAN_IT_FMP0, the isr is triggered when a frame is received on fifo-0 of can1 peripheral
	s_hal_can_isr_recvframe_canx(hal_can_port1);
}


void CAN2_RX0_IRQHandler(void)
{
    // as we configured with CAN_IT_FMP0, the isr is triggered when a frame is received on fifo-0 of can2 peripheral
	s_hal_can_isr_recvframe_canx(hal_can_port2);
}

// ---- isr of the module: end ------

 
extern uint32_t hal_can_hid_getsize(const hal_cfg_t *cfg)
{
    uint32_t size = 0;
    
    if(0 != cfg->can1_enable)
    {
        size += sizeof(hal_can_portdatastructure_t);
        size += (cfg->can1_rxqnorm_num * sizeof(hal_canfifo_item_t));
        size += (cfg->can1_txqnorm_num * sizeof(hal_canfifo_item_t));         
    }
    
    if(0 != cfg->can2_enable)
    {
        size += sizeof(hal_can_portdatastructure_t);
        size += (cfg->can2_rxqnorm_num * sizeof(hal_canfifo_item_t));
        size += (cfg->can2_txqnorm_num * sizeof(hal_canfifo_item_t));         
    }

    return(size);
}


extern hal_result_t hal_can_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    uint8_t *ram08 = (uint8_t*)memory;
    uint8_t i=0;

    // removed dependancy from NZI ram
    for(i=0; i<hal_can_ports_num; i++)
    {
        s_hal_can_portdatastruct_ptr[i] = NULL;
    }
    memset(s_hal_can_initted, hal_false, hal_can_ports_num);


    if((0 != cfg->can1_enable) || (0 != cfg->can2_enable))
    {
        if(NULL == memory)
        {
            hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_can_hid_setmem(): memory missing");
            return(hal_res_NOK_generic);
        }
    }

    if(0 != cfg->can1_enable)
    {
        s_hal_can_portdatastruct_ptr[0] = (hal_can_portdatastructure_t*)ram08;
        ram08 += sizeof(hal_can_portdatastructure_t);
        
        hal_canfifo_hid_init(&s_hal_can_portdatastruct_ptr[0]->canframes_rx_norm, cfg->can1_rxqnorm_num, ram08);
        ram08 += (cfg->can1_rxqnorm_num * sizeof(hal_canfifo_item_t));
        
        hal_canfifo_hid_init(&s_hal_can_portdatastruct_ptr[0]->canframes_tx_norm, cfg->can1_txqnorm_num, ram08);
        ram08 += (cfg->can1_txqnorm_num * sizeof(hal_canfifo_item_t));
    }
    
    if(0 != cfg->can2_enable)
    {
        s_hal_can_portdatastruct_ptr[1] = (hal_can_portdatastructure_t*)ram08;
        ram08 += sizeof(hal_can_portdatastructure_t);
 
        hal_canfifo_hid_init(&s_hal_can_portdatastruct_ptr[1]->canframes_rx_norm, cfg->can2_rxqnorm_num, ram08);
        ram08 += (cfg->can2_rxqnorm_num * sizeof(hal_canfifo_item_t));
        
        hal_canfifo_hid_init(&s_hal_can_portdatastruct_ptr[1]->canframes_tx_norm, cfg->can2_txqnorm_num, ram08);
        ram08 += (cfg->can2_txqnorm_num * sizeof(hal_canfifo_item_t));
    }

    return(hal_res_OK);

}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_can_supported_is(hal_can_port_t port)
{
    return(hal_base_hid_byte_bitcheck(hal_brdcfg_can__supported_mask, HAL_can_port2index(port)) );
}

static void s_hal_can_initted_set(hal_can_port_t port)
{
    s_hal_can_initted[HAL_can_port2index(port)] = hal_true;
}

static hal_boolval_t s_hal_can_initted_is(hal_can_port_t port)
{
    return(s_hal_can_initted[HAL_can_port2index(port)]);
}


/*
  * @brief  called by ISR for transmission.
  * @param  port identifies CAN port 
  * @retval none
  */
static void s_hal_can_isr_sendframes_canx(hal_can_port_t port)
{
    hal_canfifo_item_t *canframe_ptr;
    CanTxMsg TxMessage =
    {
        .IDE   = CAN_ID_STD,        // only stdid are managed
        .ExtId = 0,                 // since frame-id is std it is not used by stm32lib
        .RTR   = CAN_RTR_DATA       // only data frame are managed    
    };

    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    s_hal_can_isr_tx_disable(port);

    while( (hal_canfifo_hid_size(&cport->canframes_tx_norm) > 0) )
    {
        uint8_t res = 0;
        
        canframe_ptr = hal_canfifo_hid_front(&cport->canframes_tx_norm);
        TxMessage.StdId = canframe_ptr->id & 0x7FF;
        TxMessage.DLC = canframe_ptr->size;
        *(uint64_t*)TxMessage.Data = *((uint64_t*)canframe_ptr->data);

        // CAN_Transmit() returns the number of mailbox used in transmission or CAN_TxStatus_NoMailBox if there is no empty mailbox
       	res = CAN_Transmit(HAL_can_port2peripheral(port), &TxMessage);

        if(res != CAN_TxStatus_NoMailBox)
        {   // if the CAN_Transmit() was succesful ... remove the sent from from fifo-tx and call the user-defined callback
        	hal_canfifo_hid_pop(&cport->canframes_tx_norm);
            if(NULL != cport->cfg.callback_on_tx)
            {
                cport->cfg.callback_on_tx(cport->cfg.arg_cb_tx);
            }
        }
        else
        {   // there is no empty mailbox to assign our frame ... we exit the loop without checking if there are still frames in fifo-tx.
            break;
        }

    }

    if(hal_canfifo_hid_size(&cport->canframes_tx_norm) > 0)
	{   // we still have some frames to send, thus we enable the isr on tx which triggers as soon any of the transmit mailboxes gets empty.
    	s_hal_can_isr_tx_enable(port);
	}
 
}




/*
  * @brief  called by ISR for reception.
  * @param  port identifies CAN port 
  * @retval none
  */
static void s_hal_can_isr_recvframe_canx(hal_can_port_t port)
{
    hal_canfifo_item_t *canframe_ptr;
    CanRxMsg RxMessage;
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    canframe_ptr = hal_canfifo_hid_getFirstFree(&cport->canframes_rx_norm);
    if(NULL == canframe_ptr)
    {   // rx-fifo is full ...  we remove oldest frame
        hal_canfifo_hid_pop(&cport->canframes_rx_norm); //remove the oldest frame
        canframe_ptr = hal_canfifo_hid_getFirstFree(&cport->canframes_rx_norm);
    }
    
    // get the message from fifo-0 of canx peripheral
    CAN_Receive(HAL_can_port2peripheral(port), CAN_FIFO0, &RxMessage);
    
    // build the canframe inside the rx-fifo with the message content
    canframe_ptr->id = RxMessage.StdId;
    canframe_ptr->size = RxMessage.DLC;
    *((uint64_t*)canframe_ptr->data) = *((uint64_t*)RxMessage.Data);

    // if a callback is set, invoke it
    if(NULL != cport->cfg.callback_on_rx )
    {
    	cport->cfg.callback_on_rx(cport->cfg.arg_cb_rx);
    }

}


static void s_hal_can_hw_gpio_init(hal_can_port_t port)
{
    
#if     defined(USE_STM32F1)

#if 0
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    // enable gpio clock for can1 tx and rx and its afio clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | hal_brdcfg_can__gpio_clock_canx_rx[HAL_can_port2index(port)] | hal_brdcfg_can__gpio_clock_canx_tx[HAL_can_port2index(port)], ENABLE);
    
    // configure rx can
    memcpy(&GPIO_InitStructure, &s_hal_can_gpio_inittypeded_canx_rx, sizeof(GPIO_InitTypeDef));
    GPIO_InitStructure.GPIO_Pin = hal_brdcfg_can__gpio_pin_canx_rx[HAL_can_port2index(port)];
    GPIO_Init(hal_brdcfg_can__gpio_port_canx_rx[HAL_can_port2index(port)], &GPIO_InitStructure);
    // configure tx can
    memcpy(&GPIO_InitStructure, &s_hal_can_gpio_inittypeded_canx_tx, sizeof(GPIO_InitTypeDef));
    GPIO_InitStructure.GPIO_Pin = hal_brdcfg_can__gpio_pin_canx_tx[HAL_can_port2index(port)];
    GPIO_Init(hal_brdcfg_can__gpio_port_canx_tx[HAL_can_port2index(port)], &GPIO_InitStructure);  

    // Remap GPIOs 
    GPIO_PinRemapConfig((hal_can_port1 == port) ? (GPIO_Remap2_CAN1) : (GPIO_Remap_CAN2), ENABLE);    
#else

    // 1. prepare af.
    // for can1 (rx, tx): no-remap if it is (PA11, PA12). GPIO_Remap1_CAN1 if it is (PB8, PB9). GPIO_Remap2_CAN1 if it is (PD0, PD1) 
    // for can2 (tx, rx): no remap if it is (PB12, PB13). GPIO_Remap_CAN2 if it is (PB5, PB6)
    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t found = hal_false;

    
    hal_gpio_port_t portrx = hal_brdcfg_can__gpio_canx_rx[HAL_can_port2index(port)].port;
    hal_gpio_pin_t  pinrx  = hal_brdcfg_can__gpio_canx_rx[HAL_can_port2index(port)].pin;
    hal_gpio_port_t porttx = hal_brdcfg_can__gpio_canx_tx[HAL_can_port2index(port)].port;
    hal_gpio_pin_t  pintx  = hal_brdcfg_can__gpio_canx_tx[HAL_can_port2index(port)].pin;    
    
    if(hal_can_port1 == port)
    {        
        if((hal_gpio_portA == portrx) && (hal_gpio_portA == porttx) && (hal_gpio_pin11 == pinrx) && (hal_gpio_pin12 == pintx))
        {
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }
        else if((hal_gpio_portB == portrx) && (hal_gpio_portB == porttx) && (hal_gpio_pin8 == pinrx) && (hal_gpio_pin9 == pintx))
        {
            afname = GPIO_Remap1_CAN1;      afmode = ENABLE;                    found = hal_true;
        }
        else if((hal_gpio_portD == portrx) && (hal_gpio_portD == porttx) && (hal_gpio_pin0 == pinrx) && (hal_gpio_pin1 == pintx))
        {
            afname = GPIO_Remap2_CAN1;      afmode = ENABLE;                    found = hal_true;
        }               
    }
    else if(hal_can_port2 == port)
    {
       if((hal_gpio_portB == portrx) && (hal_gpio_portB == porttx) && (hal_gpio_pin12 == pinrx) && (hal_gpio_pin13 == pintx))
        {
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }
        else if((hal_gpio_portB == portrx) && (hal_gpio_portB == porttx) && (hal_gpio_pin5 == pinrx) && (hal_gpio_pin6 == pintx))
        {
            afname = GPIO_Remap_CAN2;       afmode = ENABLE;                    found = hal_true;
        }    
    }
    
    if(hal_false == found)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_can_init(): incorrect pin mapping");
    }

    hal_gpio_altcfg_t hal_can_canx_rx_altcfg;
    hal_gpio_altcfg_t hal_can_canx_tx_altcfg;
    
    // prepare the altcfg for rx and tx pins
    memcpy(&hal_can_canx_rx_altcfg, &s_hal_can_canx_rx_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_can_canx_tx_altcfg, &s_hal_can_canx_tx_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_can_canx_rx_altcfg.afname = hal_can_canx_tx_altcfg.afname = afname;
    hal_can_canx_rx_altcfg.afmode = hal_can_canx_tx_altcfg.afmode = afmode;
    
    // configure rx and tx pins
    hal_gpio_configure(hal_brdcfg_can__gpio_canx_rx[HAL_can_port2index(port)], &hal_can_canx_rx_altcfg);    
    hal_gpio_configure(hal_brdcfg_can__gpio_canx_tx[HAL_can_port2index(port)], &hal_can_canx_tx_altcfg);

#endif
#elif   defined(USE_STM32F4)    
 
#if 0
 
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    // enable gpio clock for can1 tx and rx
    RCC_AHB1PeriphClockCmd(hal_brdcfg_can__gpio_clock_canx_rx[HAL_can_port2index(port)] | hal_brdcfg_can__gpio_clock_canx_tx[HAL_can_port2index(port)], ENABLE);

    // what about ?
    // afio periph clock enable
    // RCC_APB2PeriphClockCmd(s_stm32i2c_generics.i2cx_cfg.gpio_remap_clock, ENABLE);
    
    // configure rx can
    memcpy(&GPIO_InitStructure, &s_hal_can_gpio_inittypeded_canx_rx, sizeof(GPIO_InitTypeDef));
    GPIO_InitStructure.GPIO_Pin = hal_brdcfg_can__gpio_pin_canx_rx[HAL_can_port2index(port)];
    GPIO_Init(hal_brdcfg_can__gpio_port_canx_rx[HAL_can_port2index(port)], &GPIO_InitStructure);
    // configure tx can
    memcpy(&GPIO_InitStructure, &s_hal_can_gpio_inittypeded_canx_tx, sizeof(GPIO_InitTypeDef));
    GPIO_InitStructure.GPIO_Pin = hal_brdcfg_can__gpio_pin_canx_tx[HAL_can_port2index(port)];
    GPIO_Init(hal_brdcfg_can__gpio_port_canx_tx[HAL_can_port2index(port)], &GPIO_InitStructure);
    
    // connect can pins to afio
    GPIO_PinAFConfig(hal_brdcfg_can__gpio_port_canx_rx[HAL_can_port2index(port)], hal_brdcfg_can__gpio_pinsource_canx_rx[HAL_can_port2index(port)], GPIO_AF_CAN1); or GPIO_AF_CAN2
    GPIO_PinAFConfig(hal_brdcfg_can__gpio_port_canx_tx[HAL_can_port2index(port)], hal_brdcfg_can__gpio_pinsource_canx_tx[HAL_can_port2index(port)], GPIO_AF_CAN1); or GPIO_AF_CAN2

#else

    // 1. prepare af.
    // for can1 (rx, tx): rx -> PI9, PA11, PD0, PB8.    tx ->PA12, PH13, PD1, PB9. 
    // for can2 (tx, rx): rx -> PB5, PB12.              tx ->PB13, PB6. 
    
    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t foundrx = hal_false;
    hal_bool_t foundtx = hal_false;

    
    hal_gpio_port_t portrx = hal_brdcfg_can__gpio_canx_rx[HAL_can_port2index(port)].port;
    hal_gpio_pin_t  pinrx  = hal_brdcfg_can__gpio_canx_rx[HAL_can_port2index(port)].pin;
    hal_gpio_port_t porttx = hal_brdcfg_can__gpio_canx_tx[HAL_can_port2index(port)].port;
    hal_gpio_pin_t  pintx  = hal_brdcfg_can__gpio_canx_tx[HAL_can_port2index(port)].pin;    
    
    if(hal_can_port1 == port)
    { 
        afname = GPIO_AF_CAN1;  afmode = ENABLE;
        
        if( ((hal_gpio_portI == portrx) && (hal_gpio_pin9 == pinrx))  ||
            ((hal_gpio_portA == portrx) && (hal_gpio_pin11 == pinrx)) ||
            ((hal_gpio_portD == portrx) && (hal_gpio_pin0 == pinrx))  ||
            ((hal_gpio_portB == portrx) && (hal_gpio_pin8 == pinrx))  )
        {
            foundrx = hal_true;
        }
        
        if( ((hal_gpio_portA == porttx) && (hal_gpio_pin12 == pintx))  ||
            ((hal_gpio_portH == porttx) && (hal_gpio_pin13 == pintx)) ||
            ((hal_gpio_portD == porttx) && (hal_gpio_pin1 == pintx))  ||
            ((hal_gpio_portB == porttx) && (hal_gpio_pin9 == pintx))  )
        {
            foundtx = hal_true;
        }
    }
    else if(hal_can_port2 == port)
    {
    
        afname = GPIO_AF_CAN2;
        
        if( ((hal_gpio_portB == portrx) && (hal_gpio_pin5 == pinrx))  ||
            ((hal_gpio_portB == portrx) && (hal_gpio_pin12 == pinrx))  )
        {
            foundrx = hal_true;
        }
        
        if( ((hal_gpio_portB == porttx) && (hal_gpio_pin13 == pintx))  ||
            ((hal_gpio_portB == porttx) && (hal_gpio_pin6 == pintx))   )
        {
            foundtx = hal_true;
        }
    }
    
    if((hal_false == foundrx) || (hal_false == foundtx))
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_can_init(): incorrect pin mapping");
    }

    hal_gpio_altcfg_t hal_can_canx_rx_altcfg;
    hal_gpio_altcfg_t hal_can_canx_tx_altcfg;
    
    // prepare the altcfg for rx and tx pins
    memcpy(&hal_can_canx_rx_altcfg, &s_hal_can_canx_rx_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_can_canx_tx_altcfg, &s_hal_can_canx_tx_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_can_canx_rx_altcfg.afname = hal_can_canx_tx_altcfg.afname = afname;
    hal_can_canx_rx_altcfg.afmode = hal_can_canx_tx_altcfg.afmode = afmode;
    
    // configure rx and tx pins
    hal_gpio_configure(hal_brdcfg_can__gpio_canx_rx[HAL_can_port2index(port)], &hal_can_canx_rx_altcfg);    
    hal_gpio_configure(hal_brdcfg_can__gpio_canx_tx[HAL_can_port2index(port)], &hal_can_canx_tx_altcfg);

#endif
    
#endif
}


static void s_hal_can_hw_clock_init(hal_can_port_t port)
{
    RCC_APB1PeriphClockCmd((hal_can_port1 == port) ? (RCC_APB1Periph_CAN1) : (RCC_APB1Periph_CAN2), ENABLE);
}




static void s_hal_can_hw_nvic_init(hal_can_port_t port)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    IRQn_Type CANx_RX0_IRQn = (hal_can_port1 == port) ? (CAN1_RX0_IRQn) : (CAN2_RX0_IRQn);
    IRQn_Type CANx_TX_IRQn  = (hal_can_port1 == port) ? (CAN1_TX_IRQn) : (CAN2_TX_IRQn);

    if(hal_int_priorityNONE != cport->cfg.priorx)
    {
        // enable rx irq in nvic
        hal_sys_irqn_priority_set(CANx_RX0_IRQn, cport->cfg.priorx);
        hal_sys_irqn_disable(CANx_RX0_IRQn);
    }

    if(hal_int_priorityNONE != cport->cfg.priotx)
    {
        // enable tx irq in nvic
        hal_sys_irqn_priority_set(CANx_TX_IRQn, cport->cfg.priotx);
        hal_sys_irqn_disable(CANx_TX_IRQn);
    }
}


static hal_result_t s_hal_can_hw_registers_init(hal_can_port_t port)
{
    hal_can_portdatastructure_t*    cport  = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    CAN_TypeDef*                    CANx   = HAL_can_port2peripheral(port); 
	CAN_InitTypeDef                 CAN_InitStructure;
	CAN_FilterInitTypeDef           CAN_FilterInitStructure;
	uint32_t			            baudrate;   

    switch(cport->cfg.baudrate)
    {
        case hal_can_baudrate_1mbps:    baudrate = 1000000;     break;
        case hal_can_baudrate_500kbps:  baudrate =  500000;     break;
        default:                        baudrate =  500000;     break;
    }

    CAN_DeInit(CANx);


	CAN_StructInit(&CAN_InitStructure);
    
    // CAN_Prescaler is the prescaler to apply to HAL_STM32_CAN_CLK so that the bit has HAL_STM32_CAN_TQ_TOTAL time quanta (or ticks of the clock)
    // also ... HAL_STM32_CAN_TQ_TOTAL = HAL_STM32_CAN_TQ_BS1 + 1 + HAL_STM32_CAN_TQ_BS2
    CAN_InitStructure.CAN_Prescaler = (HAL_STM32_CAN_CLK / HAL_STM32_CAN_TQ_TOTAL) / baudrate;  
    CAN_InitStructure.CAN_Mode      = CAN_Mode_Normal;      // operating mode
    CAN_InitStructure.CAN_SJW       = HAL_STM32_CAN_TQ_SJW; // max num of time quanta the hw is allowed to stretch a bit in order to re-synch
	CAN_InitStructure.CAN_BS1       = HAL_STM32_CAN_TQ_BS1; // number of time quanta in bit-segment-1 (the one before the sampling time quantum)
	CAN_InitStructure.CAN_BS2       = HAL_STM32_CAN_TQ_BS2; // number of time quanta in bit-segment-2 (the one after the sampling time quantum)
	CAN_InitStructure.CAN_TTCM      = DISABLE;              // time-triggered communication mode
	CAN_InitStructure.CAN_ABOM      = DISABLE;              // automatic bus-off
	CAN_InitStructure.CAN_AWUM      = DISABLE;              // automatic wake-up
	CAN_InitStructure.CAN_NART      = DISABLE;              // no-automatic retransmission mode 
	CAN_InitStructure.CAN_RFLM      = DISABLE;              // receive fifo locked mode
	CAN_InitStructure.CAN_TXFP      = ENABLE;               // transmit fifo priority (if ENABLEd, priority amongst the pending mailboxes is driven by the request order)
    

	if(CAN_InitStatus_Failed == CAN_Init(CANx, &CAN_InitStructure))
	{
		return(hal_res_NOK_generic);
	}

	// TODO: rendere configurabile i filtri
    // #warning VALE->filter doesn't work!!!
    // acemor on 19-oct-2012: FILTERNUM_CAN2 era 14 che e' un valore non valido ...
    //                        quindi lascio i filtri 0->6 per il can1 ed i filtri 7->13 per il can2
    #define FILTERNUM_CAN1                              0
    #define FILTERNUM_CAN2                              7

    // NOTE VALE: in order to receive msg, i had to init filter for receive all.
	// CAN filter init
	CAN_FilterInitStructure.CAN_FilterMode              = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale             = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh            = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow             = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh        = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow         = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment    = 0;
	CAN_FilterInitStructure.CAN_FilterActivation        = ENABLE;
    CAN_FilterInitStructure.CAN_FilterNumber            = (hal_can_port1 == port) ? (FILTERNUM_CAN1) : (FILTERNUM_CAN2);

    CAN_FilterInit(&CAN_FilterInitStructure);
   
	return(hal_res_OK);
}



static hal_result_t s_hal_can_tx_normprio(hal_can_port_t port, hal_can_frame_t *frame, hal_can_send_mode_t sm)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    hal_result_t res = hal_res_NOK_generic;
   
    
    // disable interrupt in can 1 or 2 for tx depending on value of port: use the nvic
    s_hal_can_isr_tx_disable(port);

    // put frame in fifo out normal priority
    res = hal_canfifo_hid_put(&cport->canframes_tx_norm, frame->id, frame->size, frame->data);

    // failed to put in fifo
    if(hal_res_OK != res)
    {   // if i cannot tx means that the queue is full, thus ... 
        if(hal_can_send_normprio_now == sm)
        {   // if send-now i empty the queue
            s_hal_can_isr_sendframes_canx(port); //s_hal_can_sendframes_canx(port);
        }
        else
        {   // if send-later i dont empty the queue
            ;
        }

        return(hal_res_NOK_busy);

    	/* Non posso abilitare qui interrupt in tx, perche' nel caso in cui la CAN_put sia 
    	gia' stata invocata con flag "send now", vengono spediti anche i pachetti aggiunti 
    	successivamente in coda con flag "send later" */
    
    
        /* Se mi e' stato chiesto di spedire chiamo isr indipendentemente all'esito della put.
        Se la put ritorna ok, allora significa che ha inserito il frame in input e lo ha spedito;
        altrimenti significa che la coda e' piena e quindi ho spedito i frame presenti in coda e
        il frame passato in input e' andato perso.
        Per spedirlo occorre invocare un'altra volta la can_put */
    
    }
    else
    {   // i could put a frame inside the queue .
        // good for you
    }

    // if i must send now ... i transmit 
    if(hal_can_send_normprio_now == sm)
    {
        s_hal_can_isr_sendframes_canx(port); //s_hal_can_sendframes_canx(port);
    }

    return(res);

}


static void s_hal_can_isr_tx_enable(hal_can_port_t port)
{
    IRQn_Type CANx_TX_IRQn  = (hal_can_port1 == port) ? (CAN1_TX_IRQn) : (CAN2_TX_IRQn);
    hal_sys_irqn_enable(CANx_TX_IRQn);
}

static void s_hal_can_isr_tx_disable(hal_can_port_t port)
{
    IRQn_Type CANx_TX_IRQn  = (hal_can_port1 == port) ? (CAN1_TX_IRQn) : (CAN2_TX_IRQn);
    hal_sys_irqn_disable(CANx_TX_IRQn);
}

static void s_hal_can_isr_rx_enable(hal_can_port_t port)
{
    IRQn_Type CANx_RX0_IRQn = (hal_can_port1 == port) ? (CAN1_RX0_IRQn) : (CAN2_RX0_IRQn);
    hal_sys_irqn_enable(CANx_RX0_IRQn);
}

static void s_hal_can_isr_rx_disable(hal_can_port_t port)
{
    IRQn_Type CANx_RX0_IRQn = (hal_can_port1 == port) ? (CAN1_RX0_IRQn) : (CAN2_RX0_IRQn);
    hal_sys_irqn_disable(CANx_RX0_IRQn);
}


#endif//HAL_USE_CAN

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


// deleted functions:

#if 0
static void s_hal_can_RCC_conf(hal_can_port_t port)
{

	/* CAN1 and CAN2 Periph clocks enable */
	if(hal_can_port1 == port)
	{
    	/* GPIOD and AFIO clocks enable */
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIO_CAN1, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	}
	else
	{
    	/* GPIOB and AFIO clocks enable */
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIO_CAN2, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	}
}
#endif


#if 0
static void s_hal_can_GPIO_conf(hal_can_port_t port)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	if(hal_can_port1 == port)/* Configure CAN1 */
	{
		/* RX pin */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN1_RX;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //Input pull-up
		GPIO_Init(GPIO_CAN1, &GPIO_InitStructure);

		/* TX pin */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN1_TX;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //Alternate function push-pull
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIO_CAN1, &GPIO_InitStructure);

		/* Remap GPIOs */
		GPIO_PinRemapConfig(GPIO_REMAP_CAN1 , ENABLE);
	}
	else /* Configure CAN2 */
	{
		/* RX pin */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN2_RX;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIO_CAN2, &GPIO_InitStructure);


		/* TX pin */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN2_TX;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIO_CAN2, &GPIO_InitStructure);

		/* Remap GPIOs */
		GPIO_PinRemapConfig(GPIO_REMAP_CAN2, ENABLE);
	}

}
#endif








