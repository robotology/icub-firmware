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

/* @file       hl_can_err.c
	@brief      This file contains internals for hl can communication.
	@author     marco.accame@iit.it
    @date       11/11/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_UTIL_CAN_ERR)

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

#include "hl_can_err.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_can_err_hid.h" 


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
    hl_can_err_cfg_t           config;
} hl_can_comm_internal_item_t;


typedef struct
{
    uint8_t                         initted;
    hl_can_err_internal_item_t*     items[hl_cans_number];   
} hl_can_comm_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_can_err_initted_set(hl_can_t id);
static void s_hl_can_err_initted_clr(hl_can_t id);
static hl_boolval_t s_hl_can_err_initted_is(hl_can_t id);

// nvic
static void s_hl_can_err_nvic_err_enable(hl_can_t id);
static void s_hl_can_err_nvic_err_disable(hl_can_t id);

static void s_hl_can_err_isr_error(hl_can_t id);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_can_err_theinternals_t s_hl_can_err_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hl_result_t hl_can_comm_deinit(hl_can_t id)
{
    #warning --> the deinit() must deallocate memory of item and of fifo. 

    return(hl_res_OK);
}

extern hl_result_t hl_can_err_init(hl_can_t id, const hl_can_err_cfg_t *cfg)
{
    hl_can_err_internal_item_t* intitem = s_hl_can_err_theinternals.items[HL_can_id2index(id)];

    if(hl_false == hl_can_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    // dont want init twice unless i call a deinit()
    if(hl_false == s_hl_can_err_initted_is(id))
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
        // the internal item
        intitem = s_hl_can_err_theinternals.items[HL_can_id2index(id)] = hl_sys_heap_new(sizeof(hl_can_err_internal_item_t));   
    }
    
    // copy config
    memcpy(&intitem->config, cfg, sizeof(hl_can_err_cfg_t)); 
   
        // init nvic
    s_hl_can_err_nvic_init(id); 
 
    
    // ok set it as initted
    s_hl_can_err_initted_set(id);

    return(hl_res_OK);
}



extern hl_result_t hl_can_err_enable(hl_can_t id)
{
	//hl_result_t res;
    hl_can_err_internal_item_t* intitem = s_hl_can_err_theinternals.items[HL_can_id2index(id)];

    if(hl_false == s_hl_can_err_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }

    if(NULL == intitem)
    {
    	return(hl_res_NOK_generic);
    }
    
    // configure interrupts on error
    CAN_ITConfig(HL_can_port2peripheral(id), HL_CAN_IT_ERROR,  ENABLE); // VALE
    
    // enable nvic error
    s_hl_can_err_nvic_err_enable(id);// VALE


    // enable scheduling 
    //hl_base_osal_scheduling_restart();
    
	return(hl_res_OK);
}


extern hl_result_t hl_can_err_disable(hl_can_t id) 
{
	//hl_result_t res;
    hl_can_err_internal_item_t* intitem = s_hl_can_err_theinternals.items[HL_can_id2index(id)];

    if(hl_false == s_hl_can_err_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }

    
    // disable scheduling
    //hl_base_osal_scheduling_suspend();

    CAN_ITConfig(HL_can_port2peripheral(id), HL_CAN_IT_ERROR,  DISABLE); // VALE
    
    // nvic 
    s_hl_can_comm_nvic_err_disable(id); // VALE
    
    // enable scheduling 
    //hl_base_osal_scheduling_restart();
    
    
    return(hl_res_OK);
}


extern hl_result_t hl_can_err_getstatus(hl_can_t id, hl_can_err_status_t *status)
{
    hl_can_err_internal_item_t* intitem = s_hl_can_err_theinternals.items[HL_can_id2index(id)];

    if(hl_false == s_hl_can_err_initted_is(id))
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
    status->u.s.sw_status.txqueueisfull = (hl_bool_true == hl_can_comm_hid_txfifo_full(id)) ? 1 : 0; 
    status->u.s.sw_status.rxqueueisfull = (hl_bool_true == hl_can_comm_hid_rxfifo_full(id)) ? 1 : 0;  
    
    return(hl_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----


// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_hl_can_err_initted_set(hl_can_t id)
{
    hl_bits_byte_bitset(&s_hl_can_err_theinternals.initted, HL_can_id2index(id));
}

static void s_hl_can_err_initted_clr(hl_can_t id)
{
    hl_bits_byte_bitclear(&s_hl_can_err_theinternals.initted, HL_can_id2index(id));
}

static hl_boolval_t s_hl_can_err_initted_is(hl_can_t id)
{
    return(hl_bits_byte_bitcheck(s_hl_can_err_theinternals.initted, HL_can_id2index(id)));
}


// VALE
static void s_hl_can_err_nvic_err_enable(hl_can_t id)
{
    IRQn_Type CANx_err = (hl_can1 == id) ? (CAN1_SCE_IRQn) : (CAN2_SCE_IRQn);
    hl_sys_irqn_enable(CANx_err);
}

// VALE
static void s_hl_can_err_nvic_err_disable(hl_can_t id)
{
    IRQn_Type CANx_err = (hl_can1 == id) ? (CAN1_SCE_IRQn) : (CAN2_SCE_IRQn);
    hl_sys_irqn_disable(CANx_err);
}


static void s_hl_can_err_isr_error(hl_can_t id)
{
    hl_can_err_internal_item_t* intitem = s_hl_can_err_theinternals.items[HL_can_id2index(id)];
    CAN_TypeDef *stm32can =  HL_can_port2peripheral(id);
    
    if(NULL != intitem->config.callback_on_err)
    {
        intitem->config.callback_on_err(intitem->config.arg_cb_err);
    }
    
    // clear sw queue is full.
    hl_can_comm_hid_rxfifo_full_clr(id); 
    hl_can_comm_hid_txfifo_full_clr(id)
    
    //clear all pendig bits
    CAN_ClearITPendingBit(stm32can, CAN_IT_ERR);
    CAN_ClearITPendingBit(stm32can, CAN_IT_LEC);
    CAN_ClearITPendingBit(stm32can, CAN_IT_BOF);
    CAN_ClearITPendingBit(stm32can, CAN_IT_EPV);
    CAN_ClearITPendingBit(stm32can, CAN_IT_EWG);
    CAN_ClearITPendingBit(stm32can, CAN_IT_FOV0);
    
    stm32can->MSR |= CAN_MSR_ERRI;   //reset bit this bit is reseted with 1.
}



#endif//defined(HL_USE_UTIL_CAN_ERR)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






