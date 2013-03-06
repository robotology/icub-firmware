/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       hal_periph_can.c
	@brief      This file implements internal implementation of the hal can module in optimized way.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       02/16/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_PERIPH_CAN

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"

#include "hal_middleware_interface.h"

#include "string.h"

#include "hal_brdcfg.h"
#include "hal_base_hid.h" 
#include "hal_periph_gpio_hid.h" 
#include "hal_utility_fifo.h"
#include "hal_utility_bits.h" 
#include "hal_heap.h"
#include "hal_cantransceiver.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_can.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_periph_can_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_can_id2index(p)           ((uint8_t)((p)))

#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
#define HAL_can_port2peripheral(p)      ( ( hal_can1 == (p) ) ? (CAN1) : (CAN2) )
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 

#if     defined(HAL_USE_CPU_FAM_STM32F1)
    // the clock APB1 is 36mhz, half the max frequency of 72 mhz. (see page 49/1096 of stm32f1x reference manual Doc ID 13902 Rev 14) 
    // the clock APB2 operates at full speed of 72 mhz. (see page 49/1096 of stm32f1x reference manual Doc ID 13902 Rev 14)
    // can is on APB1, aka the slow bus
    // we give a total of HAL_MPU_CAN_TQ_TOTAL = 9 time quanta for the duration of a can bit. 
    // we split the 9 time quanta in 5+1+3 = HAL_MPU_CAN_TQ_BS1+1+HAL_MPU_CAN_TQ_BS2
    // we allow to stretch the bit duration in order to re-synch by maximum HAL_MPU_CAN_TQ_SJW = 3 time quanta 
    #define HAL_MPU_CAN_CLK	  		        (hal_brdcfg_cpu__theconfig.speeds.slowbus)
    #define HAL_MPU_CAN_TQ_TOTAL            9
    #define HAL_MPU_CAN_TQ_SJW              CAN_SJW_3tq
    #define HAL_MPU_CAN_TQ_BS1              CAN_BS1_5tq
    #define HAL_MPU_CAN_TQ_BS2              CAN_BS2_3tq
#elif    defined(HAL_USE_CPU_FAM_STM32F4)
    // the clock APB1 is 42mhz, a fourth the max frequency of 168 mhz. (see page 23/180 of stm32f4x datasheet Doc ID 022152 Rev 3)
    // the clock APB2 is 84mhz, a half the max frequency of 168 mhz. (see page 23/180 of stm32f4x datasheet Doc ID 022152 Rev 3)
    // can is on APB1, aka the slow bus
    // we give a total of HAL_MPU_CAN_TQ_TOTAL = 7 time quanta for the duration of a can bit. 
    // we split the 7 time quanta in 4+1+2 = HAL_MPU_CAN_TQ_BS1+1+HAL_MPU_CAN_TQ_BS2
    // we allow to stretch the bit duration in order to re-synch by maximum HAL_MPU_CAN_TQ_SJW = 3 time quanta     
    #define HAL_MPU_CAN_CLK	  			    (hal_brdcfg_cpu__theconfig.speeds.slowbus)
    #define HAL_MPU_CAN_TQ_TOTAL            7
    #define HAL_MPU_CAN_TQ_SJW              CAN_SJW_3tq
    #define HAL_MPU_CAN_TQ_BS1              CAN_BS1_4tq
    #define HAL_MPU_CAN_TQ_BS2              CAN_BS2_2tq    
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_can_cfg_t hal_can_cfg_default =
{
    .runmode                    = hal_can_runmode_isr_1txq1rxq,
    .baudrate                   = hal_can_baudrate_1mbps, 
    .priorx                     = hal_int_priority06,
    .priotx                     = hal_int_priority06,
    .capacityofrxfifoofframes   = 4,
    .capacityoftxfifoofframes   = 4,
    .capacityoftxfifohighprio   = 0,
    .callback_on_rx             = NULL,
    .arg_cb_rx                  = NULL,
    .callback_on_tx             = NULL,
    .arg_cb_tx                  = NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_can_cfg_t               config;
    hal_utility_fifo_t          canframes_rx_norm;
    hal_utility_fifo_t          canframes_tx_norm;    
    uint8_t                     enabled;
    uint8_t                     txisrisenabled;
} hal_can_internal_item_t;

typedef struct
{
    uint8_t                     initted;
    hal_can_internal_item_t*    items[hal_cans_number];   
} hal_can_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_can_supported_is(hal_can_t id);
static void s_hal_can_initted_set(hal_can_t id);
static hal_boolval_t s_hal_can_initted_is(hal_can_t id);


// hw related initialisation which may change with different versions of stm32fx mpus
static void s_hal_can_hw_gpio_init(hal_can_t id);
static void s_hal_can_hw_clock_init(hal_can_t id);
static void s_hal_can_hw_nvic_init(hal_can_t id);
static hal_result_t s_hal_can_hw_registers_init(hal_can_t id);

// trasmission
static void s_hal_can_isr_sendframes_canx(hal_can_t id);
static hal_result_t s_hal_can_tx_normprio(hal_can_t id, hal_can_frame_t *frame, hal_can_send_mode_t sm);

// reception
static void s_hal_can_isr_recvframe_canx(hal_can_t id);

// interrupt
static void s_hal_can_isr_tx_enable(hal_can_t id);
static void s_hal_can_isr_tx_disable(hal_can_t id);
static void s_hal_can_isr_rx_enable(hal_can_t id);
static void s_hal_can_isr_rx_disable(hal_can_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HAL_USE_CPU_FAM_STM32F1)

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

#elif   defined(HAL_USE_CPU_FAM_STM32F4)

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

#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 


static const hal_can_frame_t s_hal_can_defcanframe =
{
    .id         = 0,
    .id_type    = hal_can_frameID_std,
    .frame_type = hal_can_frame_data,
    .size       = 0,
    .unused     = 0,
    .data       = {0}      
};

static const hal_cantransceiver_t s_hal_can_cantransceivers[] = {
    hal_cantransceiver1,
    hal_cantransceiver2
};


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_can_theinternals_t s_hal_can_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_can_init(hal_can_t id, const hal_can_cfg_t *cfg)
{
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];

    if(hal_false == s_hal_can_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
//    if(hal_true == s_hal_can_initted_is(id))
//    {
//        return(hal_res_NOK_generic);
//    }    

    if(NULL == cfg)
    {
        cfg  = &hal_can_cfg_default;
    }

    if(hal_can_runmode_isr_1txq1rxq != cfg->runmode)
    {
        hal_base_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "hal_can_init(): wrong runmode");
        return(hal_res_NOK_unsupported);  
    }
    
    
    // give memory to can internal item for this id ...   
    if(NULL == intitem)
    {
        void* tmpram = NULL;
        uint8_t capacityofrxfifoofframes = cfg->capacityofrxfifoofframes; 
        uint8_t capacityoftxfifoofframes = cfg->capacityoftxfifoofframes; 
        
        if((0 == capacityofrxfifoofframes) || (0 == capacityoftxfifoofframes))
        {
            hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_can_init(): need non-zero tx and rx fifo sizes");
        }
        
        // the internal item
        intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)] = hal_heap_new(sizeof(hal_can_internal_item_t));
        //intitem = s_hal_can_internals[HAL_can_id2index(id)] = hal_heap_new(sizeof(hal_can_internals_t));
        // minimal configuration of the internal item
        // the rx buffer
        tmpram = hal_heap_new( sizeof(hal_can_frame_t) * capacityofrxfifoofframes );
        hal_utility_fifo_init(&intitem->canframes_rx_norm, capacityofrxfifoofframes, sizeof(hal_can_frame_t), tmpram, (uint8_t*)&s_hal_can_defcanframe);
        // the tx buffer
        tmpram = hal_heap_new( sizeof(hal_can_frame_t) * capacityoftxfifoofframes );
        hal_utility_fifo_init(&intitem->canframes_tx_norm, capacityoftxfifoofframes, sizeof(hal_can_frame_t), tmpram, (uint8_t*)&s_hal_can_defcanframe);      
    }
    
    // set config
    memcpy(&intitem->config, cfg, sizeof(hal_can_cfg_t));
   
    //  reset fifos
    hal_utility_fifo_reset(&intitem->canframes_rx_norm);
    hal_utility_fifo_reset(&intitem->canframes_tx_norm);    
    
    // clear other fields of internal item
    intitem->enabled = 0;
    intitem->txisrisenabled = 0;    
    
        
    // init the phy of id
    hal_cantransceiver_init(s_hal_can_cantransceivers[HAL_can_id2index(id)], NULL);
    
    // enable the phy of id
    hal_cantransceiver_enable(s_hal_can_cantransceivers[HAL_can_id2index(id)]); 
    
    // init gpios
    s_hal_can_hw_gpio_init(id);
    
    // init clock
    s_hal_can_hw_clock_init(id);
    

    s_hal_can_hw_nvic_init(id); 


    s_hal_can_initted_set(id);

    return(hal_res_OK);
}


extern hal_result_t hal_can_enable(hal_can_t id)
{
	hal_result_t res;
	//hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];

    if(hal_false == s_hal_can_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == intitem)
    {
        return(hal_res_NOK_generic);
    }

    res = s_hal_can_hw_registers_init(id); // hardware setup

    if(res != hal_res_OK)
    {
    	return(hal_res_NOK_generic);
    }


    // disable scheduling
    hal_base_osal_scheduling_suspend();

    intitem->enabled = 1;

    // configure interrupts on rx (CAN_IT_FMP0 -> FIFO 0 message pending) and tx (CAN_IT_TME -> transmit mailbox empty)
    CAN_ITConfig(HAL_can_port2peripheral(id), (CAN_IT_FMP0 | CAN_IT_TME), ENABLE);

    // nvic 
    s_hal_can_isr_rx_enable(id);
    // dont enable the nvic for the tx
    //s_hal_can_tx_enable(id);
    s_hal_can_isr_tx_disable(id);
    intitem->txisrisenabled = 0;

    // enable scheduling 
    hal_base_osal_scheduling_restart();

	return(hal_res_OK);
}


extern hal_result_t hal_can_disable(hal_can_t id) 
{
    //hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];

    if(hal_false == s_hal_can_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == intitem)
    {
        return(hal_res_NOK_generic);
    }

    // disable scheduling
    hal_base_osal_scheduling_suspend();

    intitem->enabled = 0;

    // deconfigure interrupts on rx (CAN_IT_FMP0 -> FIFO 0 message pending) and tx (CAN_IT_TME -> transmit mailbox empty)
    CAN_ITConfig(HAL_can_port2peripheral(id), (CAN_IT_FMP0 | CAN_IT_TME), DISABLE);

    // nvic 
    s_hal_can_isr_rx_disable(id);
    // dont disable the nvic for the tx. it was never enabled
    s_hal_can_isr_tx_disable(id);
    intitem->txisrisenabled = 0;
    
    // enable scheduling 
    hal_base_osal_scheduling_restart();

    return(hal_res_OK);
}

// the function doesn't check hal_can_send_mode: the function will behave ok only if sm == hal_can_send_normprio_now
extern hal_result_t hal_can_put(hal_can_t id, hal_can_frame_t *frame, hal_can_send_mode_t sm) 
{
    return (s_hal_can_tx_normprio(id, frame, sm));
}



extern hal_result_t hal_can_transmit(hal_can_t id)
{
    // trigger the tx. the trigger will completely empty the canframes_tx_norm.
    s_hal_can_isr_sendframes_canx(id);
    return(hal_res_OK);
}


extern hal_result_t hal_can_received(hal_can_t id, uint8_t *numberof) 
{
    //hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];
    // disable interrupt rx
    s_hal_can_isr_rx_disable(id);
    
    //*numberof = hal_canfifo_hid_size(&intitem->canframes_rx_norm);
    *numberof = hal_utility_fifo_size(&intitem->canframes_rx_norm);
    
    // enable interrupt rx
    s_hal_can_isr_rx_enable(id);
    
    return(hal_res_OK);
}


extern hal_result_t hal_can_get(hal_can_t id, hal_can_frame_t *frame, uint8_t *remaining) 
{
    //hal_can_internals_t *intitem  =   s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];
    hal_utility_fifo_t* fiforx = &intitem->canframes_rx_norm;
    hal_result_t res = hal_res_NOK_nodata;
  
    // disable interrupt rx
    s_hal_can_isr_rx_disable(id);
    
    res = hal_utility_fifo_get16(fiforx, (uint8_t*)frame, remaining);

    // enable interrupt rx
    s_hal_can_isr_rx_enable(id);
    
    return(res);
    
//     hal_can_internals_t *intitem  =   s_hal_can_internals[HAL_can_id2index(id)];
//     hal_canfifo_item_t *fifoframe_ptr   =   NULL;
//   
//     // disable interrupt rx
//     s_hal_can_isr_rx_disable(id);

//     fifoframe_ptr = hal_canfifo_hid_front(&intitem->canframes_rx_norm);
//     if(NULL == fifoframe_ptr)
//     {
//         // enable interrupt rx
//         s_hal_can_isr_rx_enable(id);
//         return(hal_res_NOK_nodata); //the fifo is empty
//     }
//     
//     memcpy(frame, fifoframe_ptr, sizeof(hal_canfifo_item_t));

//     hal_canfifo_hid_pop(&intitem->canframes_rx_norm);
//     // enable interrupt rx
//     s_hal_can_isr_rx_enable(id);
//     
//     
//     if(NULL != remaining)
//     {
//         *remaining = hal_canfifo_hid_size(&intitem->canframes_rx_norm);
//     }

//     return(hal_res_OK);    
}


extern hal_result_t hal_can_receptionfilter_set(hal_can_t id, uint8_t mask_num, uint32_t mask_val, uint8_t identifier_num,
                                                uint32_t identifier_val, hal_can_frameID_format_t idformat)
{
    //CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    
    if(hal_false == s_hal_can_initted_is(id))
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
//	if(hal_can1 == id)
//	{
//		CAN_FilterInitStructure.CAN_FilterNumber = mask_num;
//		CAN_FilterInit(&CAN_FilterInitStructure);
//	}
//	else
//	{
//    	CAN_FilterInitStructure.CAN_FilterNumber = 14 + mask_num;
//    	CAN_FilterInit(&CAN_FilterInitStructure);
//	}

    //hal_base_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "hal_can_receptionfilter_set(): not supported");
    return(hal_res_NOK_unsupported);
}

extern hal_result_t hal_can_out_get(hal_can_t id, uint8_t *numberof) 
{
    //hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];
    uint8_t reenable_isrtx = 0;
    
    if(NULL == numberof)
    {
        return(hal_res_NOK_generic);
    }

    // disable interrupt tx
    if(1 == intitem->txisrisenabled)
    {
        s_hal_can_isr_tx_disable(id);
        reenable_isrtx = 1;
    }
    
    //*numberof = hal_canfifo_hid_size(&intitem->canframes_tx_norm);
    *numberof = hal_utility_fifo_size(&intitem->canframes_tx_norm);
    
    // enable interrupt tx
    if(1 == reenable_isrtx)
    {
        s_hal_can_isr_tx_enable(id);
    }
    
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void CAN1_TX_IRQHandler	(void)
{
    // as we configured with CAN_IT_TME, the isr is triggered when any of the output mailboxes in id-1 peripheral gets empty. 
    s_hal_can_isr_sendframes_canx(hal_can1);
}


void CAN2_TX_IRQHandler	(void)
{
    // as we configured with CAN_IT_TME, the isr is triggered when any of the output mailboxes in can2 peripheral gets empty.
	s_hal_can_isr_sendframes_canx(hal_can2);
}

void CAN1_RX0_IRQHandler(void)
{
    // as we configured with CAN_IT_FMP0, the isr is triggered when a frame is received on fifo-0 of can1 peripheral
	s_hal_can_isr_recvframe_canx(hal_can1);
}


void CAN2_RX0_IRQHandler(void)
{
    // as we configured with CAN_IT_FMP0, the isr is triggered when a frame is received on fifo-0 of can2 peripheral
	s_hal_can_isr_recvframe_canx(hal_can2);
}

// ---- isr of the module: end ------


extern hal_result_t hal_can_hid_static_memory_init(void)
{
    
    memset(&s_hal_can_theinternals, 0, sizeof(s_hal_can_theinternals));

//     memset(s_hal_can_internals, 0, sizeof(s_hal_can_internals));
//     s_hal_can_initted = 0;

    return(hal_res_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_can_supported_is(hal_can_t id)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_can__theconfig.supported_mask, HAL_can_id2index(id)));
}

static void s_hal_can_initted_set(hal_can_t id)
{
    hal_utility_bits_byte_bitset(&s_hal_can_theinternals.initted, HAL_can_id2index(id));
}

static hal_boolval_t s_hal_can_initted_is(hal_can_t id)
{
    return(hal_utility_bits_byte_bitcheck(s_hal_can_theinternals.initted, HAL_can_id2index(id)));
}


/*
  * @brief  called by ISR for transmission.
  * @param  id identifies CAN 
  * @retval none
  */
static void s_hal_can_isr_sendframes_canx(hal_can_t id)
{
    static CanTxMsg TxMessage =
    {
        .StdId  = 0,                // can be changed. it is a uint32_t
        .ExtId  = 0,                // fixed: not used as frame-id is always std
        .IDE    = CAN_Id_Standard,  // fixed: always std
        .RTR    = CAN_RTR_Data,     // fixed: always data frames
        .DLC    = 0,                // can be changed. it is a uint8_t
        .Data   = {0}               // can be changed. it is ... 8 bytes        
    }; 
    
    uint32_t* const txmsgiden = &TxMessage.StdId;
    uint8_t*  const txmsgsize = &TxMessage.DLC;
    uint64_t* const txmsgdata = (uint64_t*) TxMessage.Data;  
    
    //hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];
    hal_utility_fifo_t *fifotx = &intitem->canframes_tx_norm;

    s_hal_can_isr_tx_disable(id);
    intitem->txisrisenabled = 0;

    while( (hal_utility_fifo_size(fifotx) > 0) )
    {
        hal_can_frame_t* pcanframe = (hal_can_frame_t*) hal_utility_fifo_front16(fifotx);
        
        // 
        *txmsgiden      = pcanframe->id & 0x7FF; // acemor: we could remove 0x7ff because CAN_Transmit() shifts it 21 positions up.
        *txmsgsize      = pcanframe->size;
        *txmsgdata      = *((uint64_t*)pcanframe->data);

        // CAN_Transmit() returns the number of the mailbox used in transmission or CAN_TxStatus_NoMailBox if there is no empty mailbox
        if(CAN_TxStatus_NoMailBox != CAN_Transmit(HAL_can_port2peripheral(id), &TxMessage))
        {   // if the CAN_Transmit() was succesful ... remove the sent frame from from fifo-tx and call the user-defined callback
        	hal_utility_fifo_pop(fifotx);
            if(NULL != intitem->config.callback_on_tx)
            {
                intitem->config.callback_on_tx(intitem->config.arg_cb_tx);
            }
        }
        else
        {   // there is no empty mailbox to assign our frame ... we exit the loop without checking if there are still frames in fifo-tx.
            // SEE (@#): s_hal_can_isr_tx_enable(id);
            break;
        }

    }

    // we exit from teh previous loop either if the fifo is empty (thus the following control is useless) but also if CAN_Transmit() fails.
    // in such latter case we need the control in here. (@#) however we could move s_hal_can_isr_tx_enable(id) just before the break and avoid
    // this call of hal_utility_fifo_size() > 0.
    if(hal_utility_fifo_size(fifotx) > 0)
	{   // we still have some frames to send, thus we enable the isr on tx which triggers as soon any of the transmit mailboxes gets empty.
    	s_hal_can_isr_tx_enable(id);
        intitem->txisrisenabled = 1;
	}
    
    
//     hal_canfifo_item_t *canframe_ptr;
//     CanTxMsg TxMessage =
//     {
//         .IDE   = CAN_ID_STD,        // only stdid are managed
//         .ExtId = 0,                 // since frame-id is std it is not used by stm32lib
//         .RTR   = CAN_RTR_DATA       // only data frame are managed    
//     };

//     hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];

//     s_hal_can_isr_tx_disable(id);

//     while( (hal_canfifo_hid_size(&intitem->canframes_tx_norm) > 0) )
//     {
//         uint8_t res = 0;
//         
//         canframe_ptr = hal_canfifo_hid_front(&intitem->canframes_tx_norm);
//         TxMessage.StdId = canframe_ptr->id & 0x7FF;
//         TxMessage.DLC = canframe_ptr->size;
//         *(uint64_t*)TxMessage.Data = *((uint64_t*)canframe_ptr->data);

//         // CAN_Transmit() returns the number of mailbox used in transmission or CAN_TxStatus_NoMailBox if there is no empty mailbox
//        	res = CAN_Transmit(HAL_can_port2peripheral(id), &TxMessage);

//         if(res != CAN_TxStatus_NoMailBox)
//         {   // if the CAN_Transmit() was succesful ... remove the sent from from fifo-tx and call the user-defined callback
//         	hal_canfifo_hid_pop(&intitem->canframes_tx_norm);
//             if(NULL != intitem->config.callback_on_tx)
//             {
//                 intitem->config.callback_on_tx(intitem->config.arg_cb_tx);
//             }
//         }
//         else
//         {   // there is no empty mailbox to assign our frame ... we exit the loop without checking if there are still frames in fifo-tx.
//             break;
//         }

//     }

//     if(hal_canfifo_hid_size(&intitem->canframes_tx_norm) > 0)
// 	{   // we still have some frames to send, thus we enable the isr on tx which triggers as soon any of the transmit mailboxes gets empty.
//     	s_hal_can_isr_tx_enable(id);
// 	}
 
}




/*
  * @brief  called by ISR for reception.
  * @param  id identifies CAN
  * @retval none
  */
static void s_hal_can_isr_recvframe_canx(hal_can_t id)
{
    static hal_can_frame_t canframe =
    {
        .id         = 0,
        .id_type    = hal_can_frameID_std,
        .frame_type = hal_can_frame_data,
        .size       = 0,
        .unused     = 0,
        .data       = {0}      
    };
    
    uint64_t* const data = (uint64_t*) canframe.data;
    
    //hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];
    hal_utility_fifo_t* fiforx = &intitem->canframes_rx_norm;
    
    //hal_can_frame_t* pcanframe =  NULL;
    CanRxMsg RxMessage;    
    
    if(hal_true == hal_utility_fifo_full(fiforx))
    {   // remove oldest frame
        hal_utility_fifo_pop(fiforx);        
    }

    //pcanframe = hal_utility_fifo_next_free(fiforx); // the pointer of the position where a new canframe would be put.
    
        
    // get the message from fifo-0 of canx peripheral
    CAN_Receive(HAL_can_port2peripheral(id), CAN_FIFO0, &RxMessage);
    
    // build the canframe inside the rx-fifo with the message content
    //pcanframe->id   = RxMessage.StdId;
    //pcanframe->size = RxMessage.DLC;
    //*((uint64_t*)pcanframe->data) = *((uint64_t*)RxMessage.Data);
    canframe.id     = RxMessage.StdId;
    canframe.size   = RxMessage.DLC;
    *data           = *((uint64_t*)RxMessage.Data);
    
    hal_utility_fifo_put16(fiforx, (uint8_t*)&canframe);

    // if a callback is set, invoke it
    if(NULL != intitem->config.callback_on_rx )
    {
    	intitem->config.callback_on_rx(intitem->config.arg_cb_rx);
    }   
    
    
//     hal_canfifo_item_t *canframe_ptr;
//     CanRxMsg RxMessage;
//     hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];

//     canframe_ptr = hal_canfifo_hid_getFirstFree(&intitem->canframes_rx_norm);
//     if(NULL == canframe_ptr)
//     {   // rx-fifo is full ...  we remove oldest frame
//         hal_canfifo_hid_pop(&intitem->canframes_rx_norm); //remove the oldest frame
//         canframe_ptr = hal_canfifo_hid_getFirstFree(&intitem->canframes_rx_norm);
//     }
//     
//     // get the message from fifo-0 of canx peripheral
//     CAN_Receive(HAL_can_port2peripheral(id), CAN_FIFO0, &RxMessage);
//     
//     // build the canframe inside the rx-fifo with the message content
//     canframe_ptr->id = RxMessage.StdId;
//     canframe_ptr->size = RxMessage.DLC;
//     *((uint64_t*)canframe_ptr->data) = *((uint64_t*)RxMessage.Data);

//     // if a callback is set, invoke it
//     if(NULL != intitem->config.callback_on_rx )
//     {
//     	intitem->config.callback_on_rx(intitem->config.arg_cb_rx);
//     }

}


static void s_hal_can_hw_gpio_init(hal_can_t id)
{
    
#if     defined(HAL_USE_CPU_FAM_STM32F1)

#if 0
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    // enable gpio clock for can1 tx and rx and its afio clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | hal_brdcfg_can__gpio_clock_canx_rx[HAL_can_id2index(id)] | hal_brdcfg_can__gpio_clock_canx_tx[HAL_can_id2index(id)], ENABLE);
    
    // configure rx can
    memcpy(&GPIO_InitStructure, &s_hal_can_gpio_inittypeded_canx_rx, sizeof(GPIO_InitTypeDef));
    GPIO_InitStructure.GPIO_Pin = hal_brdcfg_can__gpio_pin_canx_rx[HAL_can_id2index(id)];
    GPIO_Init(hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)], &GPIO_InitStructure);
    // configure tx can
    memcpy(&GPIO_InitStructure, &s_hal_can_gpio_inittypeded_canx_tx, sizeof(GPIO_InitTypeDef));
    GPIO_InitStructure.GPIO_Pin = hal_brdcfg_can__gpio_pin_canx_tx[HAL_can_id2index(id)];
    GPIO_Init(hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)], &GPIO_InitStructure);  

    // Remap GPIOs 
    GPIO_PinRemapConfig((hal_can1 == id) ? (GPIO_Remap2_CAN1) : (GPIO_Remap_CAN2), ENABLE);    
#else

    // 1. prepare af.
    // for can1 (rx, tx): no-remap if it is (PA11, PA12). GPIO_Remap1_CAN1 if it is (PB8, PB9). GPIO_Remap2_CAN1 if it is (PD0, PD1) 
    // for can2 (tx, rx): no remap if it is (PB12, PB13). GPIO_Remap_CAN2 if it is (PB5, PB6)
    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t found = hal_false;

    
    hal_gpio_port_t portrx = hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)].gpio.port;
    hal_gpio_pin_t  pinrx  = hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)].gpio.pin;
    hal_gpio_port_t porttx = hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)].gpio.port;
    hal_gpio_pin_t  pintx  = hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)].gpio.pin;    
    
    if(hal_can1 == id)
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
    else if(hal_can2 == id)
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
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_can_init(): incorrect pin mapping");
    }

    hal_gpio_altcfg_t hal_can_canx_rx_altcfg;
    hal_gpio_altcfg_t hal_can_canx_tx_altcfg;
    hal_gpio_cfg_t config;
    
    // prepare the altcfg for rx and tx pins
    memcpy(&hal_can_canx_rx_altcfg, &s_hal_can_canx_rx_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_can_canx_tx_altcfg, &s_hal_can_canx_tx_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_can_canx_rx_altcfg.afname = hal_can_canx_tx_altcfg.afname = afname;
    hal_can_canx_rx_altcfg.afmode = hal_can_canx_tx_altcfg.afmode = afmode;
    
    // configure rx and tx pins
    memcpy(&config, &hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)].config, sizeof(hal_gpio_cfg_t));
    config.altcfg = &hal_can_canx_rx_altcfg;
    hal_gpio_init(hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)].gpio, &config);
    
    memcpy(&config, &hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)].config, sizeof(hal_gpio_cfg_t));
    config.altcfg = &hal_can_canx_tx_altcfg;
    hal_gpio_init(hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)].gpio, &config);
    
    
//     hal_gpio_configure(hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)], &hal_can_canx_rx_altcfg);    
//     hal_gpio_configure(hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)], &hal_can_canx_tx_altcfg);

#endif
#elif   defined(HAL_USE_CPU_FAM_STM32F4)    
 
#if 0
 
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    // enable gpio clock for can1 tx and rx
    RCC_AHB1PeriphClockCmd(hal_brdcfg_can__gpio_clock_canx_rx[HAL_can_id2index(id)] | hal_brdcfg_can__gpio_clock_canx_tx[HAL_can_id2index(id)], ENABLE);

    // what about ?
    // afio periph clock enable
    // RCC_APB2PeriphClockCmd(s_stm32i2c_generics.i2cx_cfg.gpio_remap_clock, ENABLE);
    
    // configure rx can
    memcpy(&GPIO_InitStructure, &s_hal_can_gpio_inittypeded_canx_rx, sizeof(GPIO_InitTypeDef));
    GPIO_InitStructure.GPIO_Pin = hal_brdcfg_can__gpio_pin_canx_rx[HAL_can_id2index(id)];
    GPIO_Init(hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)], &GPIO_InitStructure);
    // configure tx can
    memcpy(&GPIO_InitStructure, &s_hal_can_gpio_inittypeded_canx_tx, sizeof(GPIO_InitTypeDef));
    GPIO_InitStructure.GPIO_Pin = hal_brdcfg_can__gpio_pin_canx_tx[HAL_can_id2index(id)];
    GPIO_Init(hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)], &GPIO_InitStructure);
    
    // connect can pins to afio
    GPIO_PinAFConfig(hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)], hal_brdcfg_can__gpio_pinsource_canx_rx[HAL_can_id2index(id)], GPIO_AF_CAN1); or GPIO_AF_CAN2
    GPIO_PinAFConfig(hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)], hal_brdcfg_can__gpio_pinsource_canx_tx[HAL_can_id2index(id)], GPIO_AF_CAN1); or GPIO_AF_CAN2

#else

    // 1. prepare af.
    // for can1 (rx, tx): rx -> PI9, PA11, PD0, PB8.    tx ->PA12, PH13, PD1, PB9. 
    // for can2 (tx, rx): rx -> PB5, PB12.              tx ->PB13, PB6. 
    
    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t foundrx = hal_false;
    hal_bool_t foundtx = hal_false;

    
    hal_gpio_port_t portrx = hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)].gpio.port;
    hal_gpio_pin_t  pinrx  = hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)].gpio.pin;
    hal_gpio_port_t porttx = hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)].gpio.port;
    hal_gpio_pin_t  pintx  = hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)].gpio.pin;    
    
    if(hal_can1 == id)
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
    else if(hal_can2 == id)
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
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_can_init(): incorrect pin mapping");
    }

    hal_gpio_altcfg_t hal_can_canx_rx_altcfg;
    hal_gpio_altcfg_t hal_can_canx_tx_altcfg;
    
    // prepare the altcfg for rx and tx pins
    memcpy(&hal_can_canx_rx_altcfg, &s_hal_can_canx_rx_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_can_canx_tx_altcfg, &s_hal_can_canx_tx_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_can_canx_rx_altcfg.afname = hal_can_canx_tx_altcfg.afname = afname;
    hal_can_canx_rx_altcfg.afmode = hal_can_canx_tx_altcfg.afmode = afmode;
    
    // configure rx and tx pins
    hal_gpio_configure(hal_brdcfg_can__theconfig.gpio_rx[HAL_can_id2index(id)], &hal_can_canx_rx_altcfg);    
    hal_gpio_configure(hal_brdcfg_can__theconfig.gpio_tx[HAL_can_id2index(id)], &hal_can_canx_tx_altcfg);

#endif
    
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
}


static void s_hal_can_hw_clock_init(hal_can_t id)
{
    RCC_APB1PeriphClockCmd((hal_can1 == id) ? (RCC_APB1Periph_CAN1) : (RCC_APB1Periph_CAN2), ENABLE);
}




static void s_hal_can_hw_nvic_init(hal_can_t id)
{
    //hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];
    IRQn_Type CANx_RX0_IRQn = (hal_can1 == id) ? (CAN1_RX0_IRQn) : (CAN2_RX0_IRQn);
    IRQn_Type CANx_TX_IRQn  = (hal_can1 == id) ? (CAN1_TX_IRQn) : (CAN2_TX_IRQn);

    if(hal_int_priorityNONE != intitem->config.priorx)
    {
        // enable rx irq in nvic
        hal_sys_irqn_priority_set(CANx_RX0_IRQn, intitem->config.priorx);
        hal_sys_irqn_disable(CANx_RX0_IRQn);
    }

    if(hal_int_priorityNONE != intitem->config.priotx)
    {
        // enable tx irq in nvic
        hal_sys_irqn_priority_set(CANx_TX_IRQn, intitem->config.priotx);
        hal_sys_irqn_disable(CANx_TX_IRQn);
    }
}


static hal_result_t s_hal_can_hw_registers_init(hal_can_t id)
{
    //hal_can_internals_t*    intitem  = s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t*        intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];
    CAN_TypeDef*                    CANx   = HAL_can_port2peripheral(id); 
	CAN_InitTypeDef                 CAN_InitStructure;
	CAN_FilterInitTypeDef           CAN_FilterInitStructure;
	uint32_t			            baudrate;   

    switch(intitem->config.baudrate)
    {
        case hal_can_baudrate_1mbps:    baudrate = 1000000;     break;
        case hal_can_baudrate_500kbps:  baudrate =  500000;     break;
        default:                        baudrate =  500000;     break;
    }

    CAN_DeInit(CANx);


	CAN_StructInit(&CAN_InitStructure);
    
    // CAN_Prescaler is the prescaler to apply to HAL_MPU_CAN_CLK so that the bit has HAL_MPU_CAN_TQ_TOTAL time quanta (or ticks of the clock)
    // also ... HAL_MPU_CAN_TQ_TOTAL = HAL_MPU_CAN_TQ_BS1 + 1 + HAL_MPU_CAN_TQ_BS2
    CAN_InitStructure.CAN_Prescaler = (HAL_MPU_CAN_CLK / HAL_MPU_CAN_TQ_TOTAL) / baudrate;  
    CAN_InitStructure.CAN_Mode      = CAN_Mode_Normal;      // operating mode
    CAN_InitStructure.CAN_SJW       = HAL_MPU_CAN_TQ_SJW; // max num of time quanta the hw is allowed to stretch a bit in order to re-synch
	CAN_InitStructure.CAN_BS1       = HAL_MPU_CAN_TQ_BS1; // number of time quanta in bit-segment-1 (the one before the sampling time quantum)
	CAN_InitStructure.CAN_BS2       = HAL_MPU_CAN_TQ_BS2; // number of time quanta in bit-segment-2 (the one after the sampling time quantum)
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
    CAN_FilterInitStructure.CAN_FilterNumber            = (hal_can1 == id) ? (FILTERNUM_CAN1) : (FILTERNUM_CAN2);

    CAN_FilterInit(&CAN_FilterInitStructure);
   
	return(hal_res_OK);
}



static hal_result_t s_hal_can_tx_normprio(hal_can_t id, hal_can_frame_t *frame, hal_can_send_mode_t sm)
{
    //hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];
    hal_can_internal_item_t* intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)];

    hal_result_t res = hal_res_NOK_generic;
   
    
    // disable interrupt in id 1 or 2 for tx depending on value of can: use the nvic
    s_hal_can_isr_tx_disable(id);
    intitem->txisrisenabled = 0;

    // put frame in fifo out normal priority
    res = hal_utility_fifo_put16(&intitem->canframes_tx_norm, (uint8_t*)frame);

    // failed to put in fifo
    if(hal_res_OK != res)
    {   // if i cannot tx means that the queue is full, thus ... 
        if(hal_can_send_normprio_now == sm)
        {   // if send-now i empty the queue
            s_hal_can_isr_sendframes_canx(id); //s_hal_can_sendframes_canx(id);
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
        s_hal_can_isr_sendframes_canx(id); //s_hal_can_sendframes_canx(id);
    }

    return(res);    
    
//     hal_can_internals_t *intitem = s_hal_can_internals[HAL_can_id2index(id)];
//
//     hal_result_t res = hal_res_NOK_generic;
//    
//     
//     // disable interrupt in id 1 or 2 for tx depending on value of id: use the nvic
//     s_hal_can_isr_tx_disable(id);
//
//     // put frame in fifo out normal priority
//     res = hal_canfifo_hid_put(&intitem->canframes_tx_norm, frame->id, frame->size, frame->data);
//
//     // failed to put in fifo
//     if(hal_res_OK != res)
//     {   // if i cannot tx means that the queue is full, thus ... 
//         if(hal_can_send_normprio_now == sm)
//         {   // if send-now i empty the queue
//             s_hal_can_isr_sendframes_canx(id); //s_hal_can_sendframes_canx(id);
//         }
//         else
//         {   // if send-later i dont empty the queue
//             ;
//         }
//
//         return(hal_res_NOK_busy);
//
//     	/* Non posso abilitare qui interrupt in tx, perche' nel caso in cui la CAN_put sia 
//     	gia' stata invocata con flag "send now", vengono spediti anche i pachetti aggiunti 
//     	successivamente in coda con flag "send later" */
//     
//     
//         /* Se mi e' stato chiesto di spedire chiamo isr indipendentemente all'esito della put.
//         Se la put ritorna ok, allora significa che ha inserito il frame in input e lo ha spedito;
//         altrimenti significa che la coda e' piena e quindi ho spedito i frame presenti in coda e
//         il frame passato in input e' andato perso.
//         Per spedirlo occorre invocare un'altra volta la can_put */
//     
//     }
//     else
//     {   // i could put a frame inside the queue .
//         // good for you
//     }
//
//     // if i must send now ... i transmit 
//     if(hal_can_send_normprio_now == sm)
//     {
//         s_hal_can_isr_sendframes_canx(id); //s_hal_can_sendframes_canx(id);
//     }
//
//     return(res);
}


static void s_hal_can_isr_tx_enable(hal_can_t id)
{
    IRQn_Type CANx_TX_IRQn  = (hal_can1 == id) ? (CAN1_TX_IRQn) : (CAN2_TX_IRQn);
    hal_sys_irqn_enable(CANx_TX_IRQn);
}

static void s_hal_can_isr_tx_disable(hal_can_t id)
{
    IRQn_Type CANx_TX_IRQn  = (hal_can1 == id) ? (CAN1_TX_IRQn) : (CAN2_TX_IRQn);
    hal_sys_irqn_disable(CANx_TX_IRQn);
}

static void s_hal_can_isr_rx_enable(hal_can_t id)
{
    IRQn_Type CANx_RX0_IRQn = (hal_can1 == id) ? (CAN1_RX0_IRQn) : (CAN2_RX0_IRQn);
    hal_sys_irqn_enable(CANx_RX0_IRQn);
}

static void s_hal_can_isr_rx_disable(hal_can_t id)
{
    IRQn_Type CANx_RX0_IRQn = (hal_can1 == id) ? (CAN1_RX0_IRQn) : (CAN2_RX0_IRQn);
    hal_sys_irqn_disable(CANx_RX0_IRQn);
}


#endif//HAL_USE_PERIPH_CAN

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


// deleted functions:

#if 0
static void s_hal_can_RCC_conf(hal_can_t id)
{

	/* CAN1 and CAN2 Periph clocks enable */
	if(hal_can1 == id)
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
static void s_hal_can_GPIO_conf(hal_can_t id)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	if(hal_can1 == id)/* Configure CAN1 */
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








