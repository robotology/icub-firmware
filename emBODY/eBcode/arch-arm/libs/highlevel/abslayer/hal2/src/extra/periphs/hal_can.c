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

/* @file       hal_can.c
	@brief      This file implements internal implementation of the hal can module in optimized way.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       02/16/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_CAN

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"

#include "hal_middleware_interface.h"

#include "string.h"

#include "hal_brdcfg.h"
#include "hal_base_hid.h" 
//#include "hal_periph_gpio_hid.h" 
#include "hl_bits.h" 
#include "hal_heap.h"
#include "hal_cantransceiver.h"


#include "hl_can.h"
#include "hl_can_comm.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_can.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_can_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_can_id2index(p)           ((uint8_t)((p)))

#if     defined(HAL_USE_MPU_TYPE_STM32F1) || defined(HAL_USE_MPU_TYPE_STM32F4)
#define HAL_can_port2peripheral(p)      ( ( hal_can1 == (p) ) ? (CAN1) : (CAN2) )
#else //defined(HAL_USE_MPU_TYPE_*)
    #error ERR --> choose a HAL_USE_MPU_TYPE_*
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
    .arg_cb_tx                  = NULL,
    .callback_on_err            = NULL, //VALE added field 
    .arg_cb_err                 = NULL  //VALE added field 
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_can_cfg_t               config;
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

static void s_hal_can_prepare_hl_can_map(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------



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


// it must be defined in order to use hl_can.
extern const hl_can_mapping_t* hl_can_map = NULL;



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

    if(NULL == cfg)
    {
        cfg  = &hal_can_cfg_default;
    }
    
    
    // give memory to can internal item for this id ...   
    if(NULL == intitem)
    {
        uint8_t capacityofrxfifoofframes = cfg->capacityofrxfifoofframes; 
        uint8_t capacityoftxfifoofframes = cfg->capacityoftxfifoofframes; 
        
        if((0 == capacityofrxfifoofframes) || (0 == capacityoftxfifoofframes))
        {
            hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_can_init(): need non-zero tx and rx fifo sizes");
        }
        
        // the internal item
        intitem = s_hal_can_theinternals.items[HAL_can_id2index(id)] = hal_heap_new(sizeof(hal_can_internal_item_t));
    }
    
    // set config
    memcpy(&intitem->config, cfg, sizeof(hal_can_cfg_t));
    
           
    // init the phy of id
    hal_cantransceiver_init(s_hal_can_cantransceivers[HAL_can_id2index(id)], NULL);
    
    // enable the phy of id
    hal_cantransceiver_enable(s_hal_can_cantransceivers[HAL_can_id2index(id)]); 
    
    hl_can_cfg_t cancfg;
    cancfg.baudrate = (hal_can_baudrate_1mbps == cfg->baudrate) ? hl_can_baudrate_1mbps : hl_can_baudrate_500kbps;
    cancfg.advcfg   = NULL;  
    
    // we must initialise hl_can_map w/ suited values.
    s_hal_can_prepare_hl_can_map();
    
    hl_can_init((hl_can_t)id, &cancfg);

    
    hl_can_comm_cfg_t cancomcfg;
    cancomcfg.capacityofrxfifoofframes      = cfg->capacityofrxfifoofframes;
    cancomcfg.priorityrx                    = (hl_irqpriority_t)cfg->priorx;
    cancomcfg.callback_on_rx                = cfg->callback_on_rx;                 
    cancomcfg.arg_cb_rx                     = cfg->arg_cb_rx;                      
    cancomcfg.capacityoftxfifoofframes      = cfg->capacityoftxfifoofframes;
    cancomcfg.prioritytx                    = (hl_irqpriority_t)cfg->priotx;
    cancomcfg.callback_on_tx                = cfg->callback_on_tx;
    cancomcfg.arg_cb_tx                     = cfg->arg_cb_tx;
    cancomcfg.priorityerr                   = hl_irqpriority01;
    cancomcfg.callback_on_err               = cfg->callback_on_tx;
    cancomcfg.arg_cb_err                    = cfg->arg_cb_err;
    

    hl_can_comm_init((hl_can_t)id, &cancomcfg);


    s_hal_can_initted_set(id);

    return(hal_res_OK);
}


extern hal_result_t hal_can_enable(hal_can_t id)
{
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
    
    hl_can_enable((hl_can_t)id);
    hl_can_comm_enable((hl_can_t)id);

    // enable scheduling 
    hal_base_osal_scheduling_restart();

	return(hal_res_OK);
}


extern hal_result_t hal_can_disable(hal_can_t id) 
{
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

    hl_can_disable((hl_can_t)id);
    hl_can_comm_disable((hl_can_t)id);
    
    // enable scheduling 
    hal_base_osal_scheduling_restart();

    return(hal_res_OK);
}

// the function doesn't check hal_can_send_mode: the function will behave ok only if sm == hal_can_send_normprio_now
extern hal_result_t hal_can_put(hal_can_t id, hal_can_frame_t *frame, hal_can_send_mode_t sm) 
{
    hl_result_t r = hl_can_comm_put((hl_can_t)id, (hl_can_comm_frame_t*)frame, (hal_can_send_normprio_later == sm) ? (hl_can_comm_send_later) : (hl_can_comm_send_now));
    return((hal_result_t)r);
}



extern hal_result_t hal_can_transmit(hal_can_t id)
{
    hl_result_t r = hl_can_comm_transmit((hl_can_t)id);
    return((hal_result_t)r);
}


extern hal_result_t hal_can_received(hal_can_t id, uint8_t *numberof) 
{
    hl_result_t r = hl_can_comm_received((hl_can_t)id, numberof);
    return((hal_result_t)r);
}


extern hal_result_t hal_can_get(hal_can_t id, hal_can_frame_t *frame, uint8_t *remaining) 
{
    hl_result_t r = hl_can_comm_get((hl_can_t)id, (hl_can_comm_frame_t*)frame, remaining);
    return((hal_result_t)r);
}


extern hal_result_t hal_can_receptionfilter_set(hal_can_t id, uint8_t mask_num, uint32_t mask_val, uint8_t identifier_num,
                                                uint32_t identifier_val, hal_can_frameID_format_t idformat)
{
    if(hal_false == s_hal_can_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    return(hal_res_NOK_unsupported);
}

extern hal_result_t hal_can_out_get(hal_can_t id, uint8_t *numberof) 
{
    if(NULL == numberof)
    {
        return(hal_res_NOK_generic);
    }

    hl_can_comm_outgoing((hl_can_t)id, numberof);
    
    return(hal_res_OK);
}

// VALE
extern hal_result_t hal_can_getstatus(hal_can_t id, hal_can_status_t *status)
{    
    hl_result_t r = hl_can_comm_getstatus((hl_can_t)id, (hl_can_comm_status_t*)status);
    return((hal_result_t)r);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----


// ---- isr of the module: end ------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_can_supported_is(hal_can_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(hal_brdcfg_can__theconfig.supported_mask, HAL_can_id2index(id)));
}

static void s_hal_can_initted_set(hal_can_t id)
{
    hl_bits_byte_bitset(&s_hal_can_theinternals.initted, HAL_can_id2index(id));
}

static hal_boolval_t s_hal_can_initted_is(hal_can_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_can_theinternals.initted, HAL_can_id2index(id)));
}


static void s_hal_can_prepare_hl_can_map(void)
{
    // we must initialise hl_can_map w/ suited values. 
    // we have built hal_brdcfg_can__theconfig to have the same layout, but we verify it anyway
    hl_VERIFYproposition(xxx, sizeof(hl_can_mapping_t) == sizeof(hal_can_hid_brdcfg_t));
    hl_can_map = (hl_can_mapping_t*)&hal_brdcfg_can__theconfig;
}

#endif//HAL_USE_CAN

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------










