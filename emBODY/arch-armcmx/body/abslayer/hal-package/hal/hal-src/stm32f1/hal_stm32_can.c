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

#include "stm32f1.h"

#include "string.h"

#include "hal_brdcfg.h"
#include "hal_stm32_base_hid.h" 
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

#define HAL_can_stm32_framedata         CAN_RTR_DATA        //0x00
#define HAL_can_stm32_frameremote       CAN_RTR_REMOTE      //0x02


#define CAN_CLK	  			36000000

//CAN 1 pins
#define RCC_APB2Periph_GPIO_CAN1    RCC_APB2Periph_GPIOD
#define GPIO_REMAP_CAN1             GPIO_Remap2_CAN1
#define GPIO_CAN1                   GPIOD  
#define GPIO_Pin_CAN1_RX            GPIO_Pin_0
#define GPIO_Pin_CAN1_TX            GPIO_Pin_1

//CAN 2 pins
#define RCC_APB2Periph_GPIO_CAN2    RCC_APB2Periph_GPIOB
#define GPIO_REMAP_CAN2             GPIO_Remap_CAN2
#define GPIO_CAN2                   GPIOB  
#define GPIO_Pin_CAN2_RX            GPIO_Pin_5
#define GPIO_Pin_CAN2_TX            GPIO_Pin_6


///enable and disable interupt on receive
#define CAN_IT_RX_ENA(p) 	CAN_ITConfig( ( ( hal_can_port1 == (p) ) ? (CAN1) : (CAN2) ), CAN_IT_FMP0, ENABLE);
#define CAN_IT_RX_DISA(p)	CAN_ITConfig( ( ( hal_can_port1 == (p) ) ? (CAN1) : (CAN2) ), CAN_IT_FMP0, DISABLE);

///enable and disable interupt on trasmission
#define CAN_IT_TX_ENA(p)	CAN_ITConfig( ( ( hal_can_port1 == (p) ) ? (CAN1) : (CAN2) ), CAN_IT_TME, ENABLE);
#define CAN_IT_TX_DISA(p)	CAN_ITConfig( ( ( hal_can_port1 == (p) ) ? (CAN1) : (CAN2) ), CAN_IT_TME, DISABLE);






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
    .arg                = NULL
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

// init and start CAN
static void s_hal_can_RCC_conf(hal_can_port_t port);
static void s_hal_can_GPIO_conf(hal_can_port_t port);
static void s_hal_can_NVIC_conf(hal_can_port_t port);
static hal_result_t s_hal_can_register_conf(hal_can_port_t port);

// trasmission
static void s_hal_can_isr_sendframes_canx(hal_can_port_t port);
static hal_result_t s_hal_can_tx_normprio(hal_can_port_t port, hal_can_frame_t *frame, hal_can_send_mode_t sm);

// reception
static void s_hal_can_isr_recvframe_canx(hal_can_port_t port);

//interrupt
static void s_hal_can_isr_tx_enable(hal_can_port_t port);
static void s_hal_can_isr_tx_disable(hal_can_port_t port);
static void s_hal_can_isr_rx_enable(hal_can_port_t port);
static void s_hal_can_isr_rx_disable(hal_can_port_t port);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_can_portdatastructure_t *s_hal_can_portdatastruct_ptr[hal_can_ports_num] = {NULL, NULL};

static hal_boolval_t s_hal_can_initted[hal_can_ports_num] = { hal_false };


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
    
    // init low level
    hal_brdcfg_can__phydevices_enable(port); /* Enable physical can device */

    s_hal_can_RCC_conf(port); /* System clocks configuration */

    s_hal_can_GPIO_conf(port); /* GPIO configuration */

    s_hal_can_NVIC_conf(port); /* NVIC configuration */

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

    res = s_hal_can_register_conf(port); /* hardware setup */

    if(res != hal_res_OK)
    {
    	return (hal_res_NOK_generic);
    }


    // disable scheduling
    hal_base_hid_osal_scheduling_suspend();

    cport->enabled = 1;

    // registers
    CAN_IT_RX_ENA(port);
    CAN_IT_TX_ENA(port);

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

    // registers
    CAN_IT_RX_DISA(port);
    CAN_IT_TX_DISA(port);

    // nvic 
    s_hal_can_isr_rx_disable(port);
    // dont disable the nvic for the tx. it was never enabled
    s_hal_can_isr_tx_disable(port);
    
    // enable scheduling 
    hal_base_hid_osal_scheduling_restart();

    return(hal_res_OK);
}

/*the function does't check hal_can_send_mode: the function will behave ok only if sm == hal_can_send_normprio_now*/
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

    frame->id = fifoframe_ptr->StdId;
    frame->size = fifoframe_ptr->DLC;
    *((uint64_t*)frame->data) = *((uint64_t*)fifoframe_ptr->Data);
    
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




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void CAN1_TX_IRQHandler	(void)
{
//  if (CAN1->TSR & (1 << 0)) {                     /* request completed mbx 0 */
//    CAN1->TSR |= (1 << 0);                   /* reset request complete mbx 0 */
//
//	//s_hal_can_isr_sendframes_canx(hal_can_port1);
//
//    }
    s_hal_can_isr_sendframes_canx(hal_can_port1);
}


void CAN2_TX_IRQHandler	(void)
{
//  if (CAN2->TSR & (1 << 0)) {                     /* request completed mbx 0 */
//    CAN2->TSR |= (1 << 0);                   /* reset request complete mbx 0 */
//
//	//s_hal_can_isr_sendframes_canx(hal_can_port2);
//
//    }
	s_hal_can_isr_sendframes_canx(hal_can_port2);
}

void CAN1_RX0_IRQHandler(void)
{
	s_hal_can_isr_recvframe_canx(hal_can_port1);
}


void CAN2_RX0_IRQHandler(void)
{
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
  * @brief  Interrupt service routine for trasmiting.
  * @param  port identifies CAN port 
  * @retval none
  */
static void s_hal_can_isr_sendframes_canx(hal_can_port_t port)
{
    hal_canfifo_item_t *canframe_ptr;
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
	uint8_t res = 0;

    s_hal_can_isr_tx_disable(port);

    while( (hal_canfifo_hid_size(&cport->canframes_tx_norm) > 0) && (res != CAN_NO_MB) )
    {

        canframe_ptr = hal_canfifo_hid_front(&cport->canframes_tx_norm);

       	res = CAN_Transmit( ((hal_can_port1 == port) ? (CAN1) : (CAN2)), (CanTxMsg*)canframe_ptr);

        if(res != CAN_NO_MB)
        {
        	hal_canfifo_hid_pop(&cport->canframes_tx_norm);
        }

    }

    if(hal_canfifo_hid_size(&cport->canframes_tx_norm) > 0)
	{
    	s_hal_can_isr_tx_enable(port);
 //       CAN_ITConfig( CAN1, CAN_IT_TME, ENABLE);
	}
 
    //s_hal_can_isr_tx_disable(port);
    
}






/*
  * @brief  Interrupt service routine for receiving.
  * @param  port identifies CAN port 
  * @retval none
  */
static void s_hal_can_isr_recvframe_canx(hal_can_port_t port)
{
    hal_canfifo_item_t *canframe_ptr;
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    canframe_ptr = hal_canfifo_hid_getFirstFree(&cport->canframes_rx_norm);
    if(NULL == canframe_ptr)
    {
        return; //the fifo is full
    }
    CAN_Receive( ((hal_can_port1 == port) ? (CAN1) : (CAN2)), /*FIFO0*/0, canframe_ptr);


    //if call back is set, invoke it
    if(NULL != cport->cfg.callback_on_rx )
    {
    	cport->cfg.callback_on_rx(cport->cfg.arg);
    }

}


/*
  * @brief  This function configures clock for CAN.
  * 		Doesn't check input validity.
  * @param  port identifies CAN port
  * @retval None
  */
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


/*
  * @brief  This function configures GPI0 for CAN.
  * 		Doesn't check input validity.
  * @param  port identifies CAN port
  * @retval None
  */
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

/*
  * @brief  This function configures Nested Vectored Interrupt Controller for CAN.
  * 		Doesn't check input validity.
  * @param  port identifies CAN port
  * @retval None
  */
static void s_hal_can_NVIC_conf(hal_can_port_t port)
{
#if 1
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

#else
    // old mode....
//    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

	NVIC_InitTypeDef  NVIC_InitStructure;

    #warning --> acemor removed it to maintain 16 priorities                                                        
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    //#warning -> so far the priorities of rx and tx can are not used (subprio 0/1 and 2/3 are used instead)
    // they are available in cport->priorx and cport->priotx

	if(hal_can_port1 == port) /* Configure CAN1 */
	{
	  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	  NVIC_Init(&NVIC_InitStructure);

	  NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;
	  NVIC_Init(&NVIC_InitStructure);

	}
	else /* Configure CAN2 */
	{
	  NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
	  NVIC_Init(&NVIC_InitStructure);

	  NVIC_InitStructure.NVIC_IRQChannel = CAN2_TX_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x3;
	  NVIC_Init(&NVIC_InitStructure);
	}
#endif
}


/*
  * @brief  This function setups CAN hardware and configure baud rate.
  * 		Doesn't check input validity.
  * @param  port identifies CAN port
  * @retval hal_res_NOK_generic in case of error, else hal_res_OK
  */
static hal_result_t s_hal_can_register_conf(hal_can_port_t port)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	uint8_t				   res;
	uint32_t			   baudrate;

	//init bit timing with default values for baudrate 1Mbps
	uint8_t				   CAN_sjw = CAN_SJW_1tq;
	uint8_t				   CAN_bs1 = CAN_BS1_3tq;
	uint8_t				   CAN_bs2 = CAN_BS2_5tq;
	uint32_t			   brp  = 4;

    switch(cport->cfg.baudrate)
    {
        case hal_can_baudrate_1mbps:    baudrate = 1000000;     break;
        case hal_can_baudrate_500kbps:  baudrate =  500000;     break;
        default:                        baudrate =  500000;     break;
    }

	if(hal_can_port1 == port)
	{
		CAN_DeInit(CAN1);
		//baudrate = hal_base_hid_params.can1_baudrate;
	}
	else
	{
		CAN_DeInit(CAN2);
		//baudrate = hal_base_hid_params.can2_baudrate;
	}

	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = ENABLE; //hardware manages tx frames as fifo

	// set baudrate
	if(baudrate <= 500000  )
	{
		brp =  (CAN_CLK / 18) / baudrate;
		/* TSEG1 = 12, TSEG2 = 5, SJW = 4 => 1 CAN bit = 18 TQ, sample at 72%    */
		CAN_bs1 = 12;
		CAN_bs2 = 5;
		CAN_sjw = 4;
	}
	else if(baudrate <= 1000000)
	{
		brp  = (CAN_CLK / 9) / baudrate;
		/* TSEG1 = 5, TSEG2 = 3, SJW = 3 => 1 CAN bit = 9 TQ, sample at 66%      */
		CAN_bs1 = 5;
		CAN_bs2 = 3;
		CAN_sjw = 3;
	}

	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_sjw-1;;
	CAN_InitStructure.CAN_BS1 = CAN_bs1-1;
	CAN_InitStructure.CAN_BS2 = CAN_bs2-1;
	CAN_InitStructure.CAN_Prescaler = brp;

	if(hal_can_port1 == port)
	{
		res = CAN_Init(CAN1, &CAN_InitStructure);
	}
	else
	{
		res = CAN_Init(CAN2, &CAN_InitStructure);
	}

	if(CANINITFAILED == res)
	{
		return(hal_res_NOK_generic);
	}

	//TODO: rendere configurabile i filtri
#warning VALE->filter doesn't work!!!

//NOTE VALE: in order to receive msg, i had to init filter fore receive all.


	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;

	if(hal_can_port1 == port)
	{
		CAN_FilterInitStructure.CAN_FilterNumber = 0;
		CAN_FilterInit(&CAN_FilterInitStructure);
	}
	else
	{
    	CAN_FilterInitStructure.CAN_FilterNumber = 14;
    	CAN_FilterInit(&CAN_FilterInitStructure);
	}

	return(hal_res_OK);
}



static hal_result_t s_hal_can_tx_normprio(hal_can_port_t port, hal_can_frame_t *frame, hal_can_send_mode_t sm)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    hal_result_t res = hal_res_NOK_generic;
   
    
    // disable interrupt in can 1 or 2 for tx depending on value of port
    //CAN_IT_TX_DISA(port);
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



