/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       hl_can_comm.c
	@brief      This file contains internals for hl can communication.
	@author     marco.accame@iit.it
    @date       11/11/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_UTIL_CAN_COMM)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files

#include "hl_gpio.h"
#include "hl_sys.h"

#include "hl_bits.h" 
#include "hl_fifo.h"

#include "hl_arch.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_can_comm.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_can_comm_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#warning --> put these things into hl_can_hid.h

#define HL_can_id2index(p)              ((uint8_t)((p)))

#define HL_can_port2peripheral(p)       ( ( hl_can1 == (p) ) ? (CAN1) : (CAN2) )


//VALE added following macro
#define HL_CAN_IT_ERROR        (CAN_IT_ERR | CAN_IT_EWG | CAN_IT_EPV | CAN_IT_BOF | /*CAN_IT_LEC |*/ CAN_IT_FOV0) 
/*NOTE: 
    - CAN_IT_ERR  ==> Error Interrupt: an interrupt is genereted when an error condition is pending in esr register
    - CAN_IT_EWG  ==> Error warning Interrupt 
    - CAN_IT_EPV  ==> Error passive Interrupt
    - CAN_IT_BOF  ==> Bus-off Interrupt
*/

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



typedef struct
{
    hl_can_comm_cfg_t           config;
    hl_fifo_t*                  txfifo;
    hl_fifo_t*                  rxfifo;
    uint8_t                     enabled;
    uint8_t                     txfifofull;
    uint8_t                     rxfifofull;
    uint8_t                     txisrisenabled;
} hl_can_comm_internal_item_t;


typedef struct
{
    uint8_t                         initted;
    hl_can_comm_internal_item_t*    items[hl_cans_number];   
} hl_can_comm_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_can_comm_initted_set(hl_can_t id);
static void s_hl_can_comm_initted_clr(hl_can_t id);
static hl_boolval_t s_hl_can_comm_initted_is(hl_can_t id);

// nvic
static void s_hl_can_comm_nvic_init(hl_can_t id);
static void s_hl_can_comm_nvic_tx_enable(hl_can_t id);
static void s_hl_can_comm_nvic_tx_disable(hl_can_t id);
static void s_hl_can_comm_nvic_rx_enable(hl_can_t id);
static void s_hl_can_comm_nvic_rx_disable(hl_can_t id);
static void s_hl_can_comm_nvic_err_enable(hl_can_t id);
static void s_hl_can_comm_nvic_err_disable(hl_can_t id);

static void s_hl_can_comm_isr_error(hl_can_t id);

static void s_hl_can_comm_isr_recvframe_canx(hl_can_t id);


static hl_result_t s_hl_can_comm_addframe2fifotx(hl_can_t id, hl_can_comm_frame_t *frame, hl_can_comm_send_mode_t sm);
static void s_hl_can_comm_sendframes_canx(hl_can_t id);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const hl_can_comm_frame_t s_hl_can_comm_defcanframe =
{
    .id         = 0,
    .id_type    = hl_can_comm_frameID_std,
    .frame_type = hl_can_comm_frame_data,
    .size       = 0,
    .unused     = 0,
    .data       = {0}      
};

static hl_can_comm_theinternals_t s_hl_can_comm_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hl_result_t hl_can_comm_deinit(hl_can_t id)
{
    #warning --> the deinit() is to be done yet. it must deallocate memory of item and of fifo. 

    return(hl_res_OK);
}

extern hl_result_t hl_can_comm_init(hl_can_t id, const hl_can_comm_cfg_t *cfg)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];

    if(hl_false == hl_can_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    // dont want init twice unless i call a deinit()
    if(hl_true == s_hl_can_comm_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }    
    
    if(NULL == cfg)
    {
        return(hl_res_NOK_generic);
    }
        
    // give memory to can internal item for this id ... 
    if(NULL == intitem)
    {
        uint8_t capacityofrxfifoofframes = cfg->capacityofrxfifoofframes; 
        uint8_t capacityoftxfifoofframes = cfg->capacityoftxfifoofframes; 
        
        if((0 == capacityofrxfifoofframes) || (0 == capacityoftxfifoofframes))
        {
            hl_sys_on_error(hl_error_incorrectparameter, "hl_can_init(): need non-zero tx and rx fifo sizes");
        }        
        
        // the internal item
        intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)] = hl_sys_heap_new(sizeof(hl_can_comm_internal_item_t));   
        // create the txfifo
        intitem->txfifo = hl_fifo_new(capacityoftxfifoofframes, sizeof(hl_can_comm_frame_t), (uint8_t*)&s_hl_can_comm_defcanframe);
        // create the rxfifo
        intitem->rxfifo = hl_fifo_new(capacityofrxfifoofframes, sizeof(hl_can_comm_frame_t), (uint8_t*)&s_hl_can_comm_defcanframe);       
    }
    
    // copy config
    memcpy(&intitem->config, cfg, sizeof(hl_can_comm_cfg_t)); 
   
    //  reset fifos
    hl_fifo_reset(intitem->txfifo);
    hl_fifo_reset(intitem->rxfifo);    
    
    
    // flags
    intitem->enabled                = 0;
    intitem->txfifofull             = 0;
    intitem->rxfifofull             = 0;
    intitem->txisrisenabled         = 0;
    
    #warning --> see other things in internals
    
    #warning --> add can transceiver somewhere. may be as a hl_can_trans.h 
    
    // init nvic
    s_hl_can_comm_nvic_init(id); 
 
    
    // ok set it as initted
    s_hl_can_comm_initted_set(id);

    return(hl_res_OK);
}



extern hl_result_t hl_can_comm_enable(hl_can_t id)
{
	//hl_result_t res;
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];

    if(hl_false == s_hl_can_comm_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }

    if(NULL == intitem)
    {
    	return(hl_res_NOK_generic);
    }
    
    // disable scheduling
    //hl_base_osal_scheduling_suspend();

    intitem->enabled = 1;

    // configure interrupts on rx (CAN_IT_FMP0 -> FIFO 0 message pending) and tx (CAN_IT_TME -> transmit mailbox empty)
    CAN_ITConfig(HL_can_port2peripheral(id), (CAN_IT_FMP0 | CAN_IT_TME), ENABLE);
    CAN_ITConfig(HL_can_port2peripheral(id), (CAN_IT_FMP0), ENABLE);
    CAN_ITConfig(HL_can_port2peripheral(id), (CAN_IT_TME), ENABLE);
    // configure interrupts on error
    CAN_ITConfig(HL_can_port2peripheral(id), HL_CAN_IT_ERROR,  ENABLE); // VALE
    
    // enable nvic rx
    s_hl_can_comm_nvic_rx_enable(id);
    // dont enable nvic tx
    s_hl_can_comm_nvic_tx_disable(id);
    intitem->txisrisenabled = 0;
    // enable nvic error
    s_hl_can_comm_nvic_err_enable(id);// VALE

    CAN_TypeDef* stm32can = HL_can_port2peripheral(id);
   
    // it is in can driver by K&%L. it was not in hal-1. it seems not be important. 
    stm32can->MCR &= ~(1 << 0);             /* normal operating mode, reset INRQ   */
    while (stm32can->MSR & (1 << 0));


    // enable scheduling 
    //hl_base_osal_scheduling_restart();
    
	return(hl_res_OK);
}


extern hl_result_t hl_can_comm_disable(hl_can_t id) 
{
	//hl_result_t res;
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];

    if(hl_false == s_hl_can_comm_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }

    if(NULL == intitem)
    {
    	return(hl_res_NOK_generic);
    }
    
    // disable scheduling
    //hl_base_osal_scheduling_suspend();

    intitem->enabled = 0;
    
    // de-configure interrupts on rx (CAN_IT_FMP0 -> FIFO 0 message pending) and tx (CAN_IT_TME -> transmit mailbox empty)
    CAN_ITConfig(HL_can_port2peripheral(id), (CAN_IT_FMP0 | CAN_IT_TME), DISABLE);
    //CAN_ITConfig(HL_can_port2peripheral(id), CAN_IT_TME, DISABLE);
    //CAN_ITConfig(HL_can_port2peripheral(id), CAN_IT_FMP0, DISABLE);
    CAN_ITConfig(HL_can_port2peripheral(id), HL_CAN_IT_ERROR,  DISABLE); // VALE
    
    // nvic 
    s_hl_can_comm_nvic_rx_disable(id);
    s_hl_can_comm_nvic_tx_disable(id);
    intitem->txisrisenabled = 0;
    
    s_hl_can_comm_nvic_err_disable(id); // VALE
    
    // enable scheduling 
    //hl_base_osal_scheduling_restart();
    
    
    return(hl_res_OK);
}


extern hl_result_t hl_can_comm_put(hl_can_t id, hl_can_comm_frame_t *frame, hl_can_comm_send_mode_t sm)
{
    return(s_hl_can_comm_addframe2fifotx(id, frame, sm));
}


extern hl_result_t hl_can_comm_transmit(hl_can_t id)
{
    // trigger the tx. the trigger will completely empty the canframes_tx_norm.
    s_hl_can_comm_sendframes_canx(id);
    return(hl_res_OK);
}


extern hl_result_t hl_can_comm_received(hl_can_t id, uint8_t *numberof)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    
    // disable interrupt rx
    s_hl_can_comm_nvic_rx_disable(id);
    
    *numberof = hl_fifo_size(intitem->rxfifo);
    
    // enable interrupt rx
    s_hl_can_comm_nvic_rx_enable(id);
    
    return(hl_res_OK);
}

extern hl_result_t hl_can_comm_outgoing(hl_can_t id, uint8_t *numberof)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    uint8_t reenable_isrtx = 0;
 
    if(1 == intitem->txisrisenabled)
    {
        s_hl_can_comm_nvic_tx_disable(id);
        reenable_isrtx = 1;
    }
       
    *numberof = hl_fifo_size(intitem->txfifo);
    
    if(1 == reenable_isrtx)
    {
        s_hl_can_comm_nvic_tx_enable(id);
    }    
    
    return(hl_res_OK);    
}


extern hl_result_t hl_can_comm_get(hl_can_t id, hl_can_comm_frame_t *frame, uint8_t *remaining)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    hl_result_t res = hl_res_NOK_nodata;
  
    // disable interrupt rx
    s_hl_can_comm_nvic_rx_disable(id);
    
    res = hl_fifo_get16(intitem->rxfifo, (uint8_t*)frame, remaining);

    // enable interrupt rx
    s_hl_can_comm_nvic_rx_enable(id);
    
    return(res);
}


extern hl_result_t hl_can_comm_getstatus(hl_can_t id, hl_can_comm_status_t *status)
{
//    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];

    if(hl_false == s_hl_can_comm_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    if(NULL == status)
    {
        return(hl_res_NOK_nullpointer);
    }
    

    CAN_TypeDef *stm32can =  HL_can_port2peripheral(id); 
       
    status->u.s.hw_status.REC = (stm32can->ESR & CAN_ESR_REC)>>24;          
    status->u.s.hw_status.TEC = (stm32can->ESR & CAN_ESR_TEC)>>16;   ;           
    status->u.s.hw_status.warning = ((stm32can->ESR & CAN_ESR_EWGF) == CAN_ESR_EWGF);     
    status->u.s.hw_status.passive = ((stm32can->ESR & CAN_ESR_EPVF) == CAN_ESR_EPVF);       
    status->u.s.hw_status.busoff = ((stm32can->ESR & CAN_ESR_BOFF) == CAN_ESR_BOFF);          
    status->u.s.hw_status.txqueueisfull = 0; 
    status->u.s.hw_status.rxqueueisfull = ((stm32can->RF0R & CAN_RF0R_FOVR0) == CAN_RF0R_FOVR0); 
    status->u.s.hw_status.dummy3b = 0;
    status->u.s.sw_status.txqueueisfull = (hl_true == hl_can_comm_hid_txfifo_full(id)) ? 1 : 0; 
    status->u.s.sw_status.rxqueueisfull = (hl_true == hl_can_comm_hid_rxfifo_full(id)) ? 1 : 0;  
    
    return(hl_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void CAN1_TX_IRQHandler	(void)
{
    // as we configured with CAN_IT_TME, the isr is triggered when any of the output mailboxes in id-1 peripheral gets empty. 
    s_hl_can_comm_sendframes_canx(hl_can1);
}


void CAN2_TX_IRQHandler	(void)
{
    // as we configured with CAN_IT_TME, the isr is triggered when any of the output mailboxes in can2 peripheral gets empty.
	s_hl_can_comm_sendframes_canx(hl_can2);
}

void CAN1_RX0_IRQHandler(void)
{
    // as we configured with CAN_IT_FMP0, the isr is triggered when a frame is received on fifo-0 of can1 peripheral
	s_hl_can_comm_isr_recvframe_canx(hl_can1);
}


void CAN2_RX0_IRQHandler(void)
{
    // as we configured with CAN_IT_FMP0, the isr is triggered when a frame is received on fifo-0 of can2 peripheral
	s_hl_can_comm_isr_recvframe_canx(hl_can2);
}

// VALE
void CAN1_SCE_IRQHandler(void)
{
    s_hl_can_comm_isr_error(hl_can1);
}
// VALE
void CAN2_SCE_IRQHandler(void)
{
    s_hl_can_comm_isr_error(hl_can2);
}

// ---- isr of the module: end ------


extern hl_boolval_t hl_can_comm_hid_rxfifo_full(hl_can_t id)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];    
    return((hl_boolval_t)intitem->rxfifofull);
}

extern hl_boolval_t hl_can_comm_hid_txfifo_full(hl_can_t id)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];    
    return((hl_boolval_t)intitem->txfifofull);
}

extern void hl_can_comm_hid_rxfifo_full_clr(hl_can_t id)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    intitem->rxfifofull = hl_false;
}

extern void hl_can_comm_hid_txfifo_clr(hl_can_t id)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    intitem->txfifofull = hl_false;
}


extern void hl_can_comm_hid_rxfifo_full_set(hl_can_t id)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    intitem->rxfifofull = hl_true;
}

extern void hl_can_comm_hid_txfifo_set(hl_can_t id)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    intitem->txfifofull = hl_true;
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hl_can_comm_initted_set(hl_can_t id)
{
    hl_bits_byte_bitset(&s_hl_can_comm_theinternals.initted, HL_can_id2index(id));
}

static void s_hl_can_comm_initted_clr(hl_can_t id)
{
    hl_bits_byte_bitclear(&s_hl_can_comm_theinternals.initted, HL_can_id2index(id));
}

static hl_boolval_t s_hl_can_comm_initted_is(hl_can_t id)
{
    return(hl_bits_byte_bitcheck(s_hl_can_comm_theinternals.initted, HL_can_id2index(id)));
}




static void s_hl_can_comm_nvic_init(hl_can_t id)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    IRQn_Type CANx_RX0_IRQn = (hl_can1 == id) ? (CAN1_RX0_IRQn) : (CAN2_RX0_IRQn);
    IRQn_Type CANx_TX_IRQn  = (hl_can1 == id) ? (CAN1_TX_IRQn)  : (CAN2_TX_IRQn);
    IRQn_Type CANx_err      = (hl_can1 == id) ? (CAN1_SCE_IRQn) : (CAN2_SCE_IRQn); // VALE

    if(hl_irqpriorityNONE != intitem->config.priorityrx)
    {
        // enable rx irq in nvic
        hl_sys_irqn_priority_set(CANx_RX0_IRQn, intitem->config.priorityrx);
        hl_sys_irqn_disable(CANx_RX0_IRQn);
    }

    if(hl_irqpriorityNONE != intitem->config.prioritytx)
    {
        // enable tx irq in nvic
        hl_sys_irqn_priority_set(CANx_TX_IRQn, intitem->config.prioritytx);
        hl_sys_irqn_disable(CANx_TX_IRQn);
    }


    if(hl_irqpriorityNONE != intitem->config.priorityerr)
    {    
        // configure error interrupt priority
        hl_sys_irqn_priority_set(CANx_err, intitem->config.priorityerr); //high prio // VALE
    }
}


static void s_hl_can_comm_nvic_tx_enable(hl_can_t id)
{
    IRQn_Type CANx_TX_IRQn  = (hl_can1 == id) ? (CAN1_TX_IRQn) : (CAN2_TX_IRQn);
    hl_sys_irqn_enable(CANx_TX_IRQn);
}

static void s_hl_can_comm_nvic_tx_disable(hl_can_t id)
{
    IRQn_Type CANx_TX_IRQn  = (hl_can1 == id) ? (CAN1_TX_IRQn) : (CAN2_TX_IRQn);
    hl_sys_irqn_disable(CANx_TX_IRQn);
}

static void s_hl_can_comm_nvic_rx_enable(hl_can_t id)
{
    IRQn_Type CANx_RX0_IRQn = (hl_can1 == id) ? (CAN1_RX0_IRQn) : (CAN2_RX0_IRQn);
    hl_sys_irqn_enable(CANx_RX0_IRQn);
}

static void s_hl_can_comm_nvic_rx_disable(hl_can_t id)
{
    IRQn_Type CANx_RX0_IRQn = (hl_can1 == id) ? (CAN1_RX0_IRQn) : (CAN2_RX0_IRQn);
    hl_sys_irqn_disable(CANx_RX0_IRQn);
}


// VALE
static void s_hl_can_comm_nvic_err_enable(hl_can_t id)
{
    IRQn_Type CANx_err = (hl_can1 == id) ? (CAN1_SCE_IRQn) : (CAN2_SCE_IRQn);
    hl_sys_irqn_enable(CANx_err);
}

// VALE
static void s_hl_can_comm_nvic_err_disable(hl_can_t id)
{
    IRQn_Type CANx_err = (hl_can1 == id) ? (CAN1_SCE_IRQn) : (CAN2_SCE_IRQn);
    hl_sys_irqn_disable(CANx_err);
}


static hl_result_t s_hl_can_comm_addframe2fifotx(hl_can_t id, hl_can_comm_frame_t *frame, hl_can_comm_send_mode_t sm)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];

    hl_result_t res = hl_res_NOK_generic;
      
    // disable interrupt in id 1 or 2 for tx depending on value of can: use the nvic
    s_hl_can_comm_nvic_tx_disable(id);
    intitem->txisrisenabled = 0;

    // put frame in txfifo
    res = hl_fifo_put16(intitem->txfifo, (uint8_t*)frame);

    // failed to put in fifo
    if(hl_res_OK != res)
    {   // if i cannot tx means that the queue is full, thus ... 
        intitem->txfifofull = 1; // VALE
        s_hl_can_comm_isr_error(id); // VALE
        
        if(hl_can_comm_send_now == sm)
        {   // if send-now i empty the queue
            s_hl_can_comm_sendframes_canx(id);
        }
        else
        {   // if send-later i dont empty the queue
            ;
        }

        return(hl_res_NOK_busy);    
    }
 
    // if i must send now ... i transmit 
    if(hl_can_comm_send_now == sm)
    {
        s_hl_can_comm_sendframes_canx(id); 
    }

    return(res);    
 
}


static void s_hl_can_comm_isr_error(hl_can_t id)
{
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    CAN_TypeDef *stm32can =  HL_can_port2peripheral(id);
    
    if(NULL != intitem->config.callback_on_err)
    {
        intitem->config.callback_on_err(intitem->config.arg_cb_err);
    }
    
    // clear sw queue is full.
    intitem->txfifofull = 0; 
    intitem->rxfifofull = 0;
    
    //clear all pendig bits
    CAN_ClearITPendingBit(stm32can, CAN_IT_ERR);
    CAN_ClearITPendingBit(stm32can, CAN_IT_LEC);
    CAN_ClearITPendingBit(stm32can, CAN_IT_BOF);
    CAN_ClearITPendingBit(stm32can, CAN_IT_EPV);
    CAN_ClearITPendingBit(stm32can, CAN_IT_EWG);
    CAN_ClearITPendingBit(stm32can, CAN_IT_FOV0);
    
    stm32can->MSR |= CAN_MSR_ERRI;   //reset bit this bit is reseted with 1.
}



static void s_hl_can_comm_sendframes_canx(hl_can_t id)
{
    CanTxMsg TxMessage =            // not static because if can1 interrupts can2 then the variable can be dirty
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
    // uint64_t*  txmsgdata = (uint64_t*) TxMessage.Data;  // cannot use such a cast because .Data[] is not 8-aligned in stm32lib
    
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    hl_fifo_t *txfifo = intitem->txfifo;

    s_hl_can_comm_nvic_tx_disable(id);
    intitem->txisrisenabled = 0;

    while( hl_fifo_size(txfifo) > 0 )
    {
        hl_can_comm_frame_t* pcanframe = (hl_can_comm_frame_t*) hl_fifo_front16(txfifo);
        
        // 
        *txmsgiden      = pcanframe->id & 0x7FF; // acemor: we could remove 0x7ff because CAN_Transmit() shifts it 21 positions up.
        *txmsgsize      = pcanframe->size;
        //*txmsgdata      = *((uint64_t*)pcanframe->data); // cannot use because destination is not 8-aligned
        memcpy(TxMessage.Data, pcanframe->data, 8);

        // CAN_Transmit() returns the number of the mailbox used in transmission or CAN_TxStatus_NoMailBox if there is no empty mailbox
        if(CAN_TxStatus_NoMailBox != CAN_Transmit(HL_can_port2peripheral(id), &TxMessage))
        {   // if the CAN_Transmit() was successful ... remove the sent frame from from fifo-tx and call the user-defined callback
        	hl_fifo_pop(txfifo);
            if(NULL != intitem->config.callback_on_tx)
            {
                intitem->config.callback_on_tx(intitem->config.arg_cb_tx);
            }
        }
        else
        {   // there is no empty mailbox to assign our frame ... we exit the loop without checking if there are still frames in fifo-tx.
            // SEE (@#): s_hl_can_comm_nvic_tx_enable(id);
            break;
        }

    }

    // we exit from the previous loop either if the fifo is empty (thus the following control is useless) but also if CAN_Transmit() fails.
    // in such latter case we need the control in here. (@#) however we could move s_hl_can_comm_nvic_tx_enable(id) just before the break and avoid
    // this call of hl_fifo_size() > 0.
    if(hl_fifo_size(txfifo) > 0)
	{   // we still have some frames to send, thus we enable the isr on tx which triggers as soon any of the transmit mailboxes gets empty.
    	s_hl_can_comm_nvic_tx_enable(id);
        intitem->txisrisenabled = 1;
	}
     
}


static void s_hl_can_comm_isr_recvframe_canx(hl_can_t id)
{
    static hl_can_comm_frame_t canframe =
    {
        .id         = 0,
        .id_type    = hl_can_comm_frameID_std,
        .frame_type = hl_can_comm_frame_data,
        .size       = 0,
        .unused     = 0,
        .data       = {0}      
    };
    
    uint64_t* const data = (uint64_t*) canframe.data;
    
    hl_can_comm_internal_item_t* intitem = s_hl_can_comm_theinternals.items[HL_can_id2index(id)];
    hl_fifo_t* fiforx = intitem->rxfifo;
    CAN_TypeDef *stm32can = HL_can_port2peripheral(id); // VALE
    
    CanRxMsg RxMessage;    
    
    
    /*NOTE: both hw fifo and sw fifo contains always newest messages. so in case of error(fifo overun) don't return, but go on to recive frame*/
    
    //if hardware fifo is in overrun call the error callback and then go to get receive frame
    if((stm32can->RF0R & CAN_RF0R_FOVR0) == CAN_RF0R_FOVR0) // VALE
    {
        s_hl_can_comm_isr_error(id);  
    }
    
    if(hl_true == hl_fifo_full(fiforx))
    {   // remove oldest frame
        hl_fifo_pop(fiforx);      
        intitem->rxfifofull = 1; // VALE
        s_hl_can_comm_isr_error(id); // VALE
    }

       
    // get the message from fifo-0 of stm32can peripheral
    CAN_Receive(HL_can_port2peripheral(id), CAN_FIFO0, &RxMessage);
    
    // build the canframe inside the rx-fifo with the message content
    //pcanframe->id   = RxMessage.StdId;
    //pcanframe->size = RxMessage.DLC;
    //*((uint64_t*)pcanframe->data) = *((uint64_t*)RxMessage.Data);
    canframe.id     = RxMessage.StdId;
    canframe.size   = RxMessage.DLC;
    *data           = *((uint64_t*)RxMessage.Data);
    
    hl_fifo_put16(fiforx, (uint8_t*)&canframe);

    // if a callback is set, invoke it
    if(NULL != intitem->config.callback_on_rx)
    {
    	intitem->config.callback_on_rx(intitem->config.arg_cb_rx);
    }   
    
}

#endif//defined(HL_USE_UTIL_CAN_COMM)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






