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


/* @file       hal_eth.c
    @brief      This file implements internal implementation of the hal eth module.
    @author     marco.accame@iit.it
    @date       05/27/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
#include "hal_middleware_interface.h"

#ifdef HAL_USE_ETH

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hl_common.h" 
#include "hl_eth.h" 
#include "hl_bits.h" 

#include "hal_ethtransceiver.h"
#include "hal_heap.h"



//#define HAL_USE_EVENTVIEWER_ETH

#if defined(HAL_USE_EVENTVIEWER_ETH)
#include "eventviewer.h"
#include "hal_arch_arm.h"
#endif 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_eth.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_eth_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_eth_id2index(p)           ((uint8_t)((p)))


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// extern hal_eth_hid_debug_support_t hal_eth_hid_DEBUG_support =
// {
//     .fn_inside_eth_isr  = NULL
// };

// it must be defined in order to use hl_eth.
extern const hl_eth_mapping_t* hl_eth_map = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_eth_cfg_t               config;  
    hal_eth_onframereception_t  onframerx; 
    hl_eth_cfg_t                hl_eth_config;    
} hal_eth_internal_item_t;


typedef struct
{
    uint32_t                    inittedmask;
    hal_eth_internal_item_t*    items[hal_eths_number];
} hal_eth_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_eth_supported_is(void);
static void s_hal_eth_initted_set(hal_eth_t id);
static hal_boolval_t s_hal_eth_initted_is(hal_eth_t id);

static void s_hal_eth_prepare_hl_eth_map(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const hal_eth_network_functions_t s_hal_eth_fns = 
{
    .eth_init           = hal_eth_init, 
    .eth_enable         = hal_eth_enable, 
    .eth_disable        = hal_eth_disable, 
    .eth_sendframe      = hal_eth_sendframe
};


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_eth_theinternals_t s_hal_eth_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_eth_init(const hal_eth_cfg_t *cfg)
{
    const hal_eth_t id = hal_eth1;
    hal_eth_internal_item_t* intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)];
    hal_result_t res = hal_res_NOK_generic;

    if(hal_true != s_hal_eth_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(NULL == cfg)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    if(hal_true == s_hal_eth_initted_is(id))
    {
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_eth_cfg_t)))
        {   // ok only if the previously used config is the same as the current one
            return(hal_res_OK);
        }
        else
        {
            return(hal_res_NOK_generic);
        }
    }    
    
    
    if(NULL == intitem)
    {
        intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)] = hal_heap_new(sizeof(hal_eth_internal_item_t));    
    }

    #warning TODO --> make ipal use a cfg w/ capacityoftxfifoofframes and capacityofrxfifoofframes. so far we use hal_eth__theboardconfig
    uint8_t capacityoftxfifoofframes = hal_eth__theboardconfig.txdmafifocapacity;    
    uint8_t capacityofrxfifoofframes = hal_eth__theboardconfig.rxdmafifocapacity;    
    
    if(hal_NA08 == capacityoftxfifoofframes)
    {
        capacityoftxfifoofframes = cfg->capacityoftxfifoofframes;
    }
    if(hal_NA08 == capacityofrxfifoofframes)
    {
        capacityofrxfifoofframes = cfg->capacityofrxfifoofframes;
    }    
    if(0 == capacityoftxfifoofframes)
    {
        hal_base_on_warning("hal_eth_init() found txdmacapacity at 0. set it to 2"); 
        capacityoftxfifoofframes = 2;
    }
    if(0 == capacityofrxfifoofframes)
    {
        hal_base_on_warning("hal_eth_init() found rxdmacapacity at 0. set it to 2");
        capacityofrxfifoofframes = 2;
    }      
    
    memcpy(&intitem->config, cfg, sizeof(hal_eth_cfg_t));
    
    memcpy(&intitem->onframerx, cfg->onframerx, sizeof(hal_eth_onframereception_t));    

    // initialise the eth transceiver.
    if(hal_res_OK != hal_ethtransceiver_init(NULL))
    {
       hal_base_on_fatalerror(hal_fatalerror_generic, "hal_ethtransceiver_init() failed");
    }   

    
    intitem->hl_eth_config.macaddress                = intitem->config.macaddress;
    intitem->hl_eth_config.behaviour                 = hl_eth_beh_dmatxrx;
    intitem->hl_eth_config.priority                  = (hl_irqpriority_t)intitem->config.priority;
    intitem->hl_eth_config.capacityoftxfifoofframes  = capacityoftxfifoofframes;
    intitem->hl_eth_config.capacityofrxfifoofframes  = capacityofrxfifoofframes;

    // we must initialise hl_eth_map w/ suited values. 
    s_hal_eth_prepare_hl_eth_map();
    
    hl_result_t r = hl_eth_init(&intitem->hl_eth_config);
    
    if(hl_res_OK != r)
    {
        return(hal_res_NOK_generic);
    }
    
    // now start the hal_ethtransceiver
    res = hal_ethtransceiver_start(NULL);
    res = res;
    s_hal_eth_initted_set(id);
    
    // also init smi
    hal_eth_smi_init();
 
    return(hal_res_OK);
}


extern hal_result_t hal_eth_enable(void) 
{
    return((hal_result_t)hl_eth_enable());
}


extern hal_result_t hal_eth_disable(void) 
{
    return((hal_result_t)hl_eth_disable());
}


extern hal_result_t hal_eth_sendframe(hal_eth_frame_t *frame) 
{
    return((hal_result_t)hl_eth_sendframe((hl_eth_frame_t*)frame));
}


extern const hal_eth_network_functions_t * hal_eth_get_network_functions(void)
{
    return(&s_hal_eth_fns);
}



extern void hal_eth_smi_init(void)
{
    hl_eth_smi_init();
}

extern uint16_t hal_eth_smi_read(uint8_t PHYaddr, uint8_t REGaddr)
{
    return(hl_eth_smi_read(PHYaddr, REGaddr));
}

// writes the 16 bits of value in register REGaddr in the physical with address PHYaddr. both REGaddr and PHYaddr range is 0-31
extern void hal_eth_smi_write(uint8_t PHYaddr, uint8_t REGaddr, uint16_t value)
{
    hl_eth_smi_write(PHYaddr, REGaddr, value);
}



extern hal_result_t hal_eth_check_links(uint8_t *linkst_mask, uint8_t *links_num)
{
    hal_result_t res = hal_res_NOK_generic;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)     
    if((NULL == linkst_mask) || (NULL == links_num))
    {
        return(hal_res_NOK_nullpointer);
    }
#endif
    
    // i get the number ... resuult is always res_OK if argument is not NULL.
    res = hal_ethtransceiver_phy_numberof(links_num); 
    
    res = hal_ethtransceiver_phy_linkupmask(linkst_mask);
    
    return(res);
}


// must be sure that the layout of hal_eth_phy_status_t is equal to the layout of hal_ethtransceiver_phystatus_t
// it is type definition
hl_VERIFYproposition(xxx, sizeof(hal_eth_phy_status_t) == sizeof(hal_ethtransceiver_phystatus_t))

extern hal_result_t hal_eth_get_links_status(hal_eth_phy_status_t* link_list, uint8_t links_num)
{
    hal_result_t res = hal_res_NOK_generic;    
    res = hal_ethtransceiver_phy_status((hal_ethtransceiver_phystatus_t*)link_list, links_num);    
    return(res);
}   


// must be sure that the layout of hal_eth_phy_errorsinfo_t is equal to the layout of hal_ethtransceiver_phyerrorinfo_t
// also check about values of hal_eth_phy_errors_info_type_t and hal_ethtransceiver_phyerror_t
// they are type definitions
hl_VERIFYproposition(xxx, sizeof(hal_eth_phy_errorsinfo_t) == sizeof(hal_ethtransceiver_phyerrorinfo_t))
hl_VERIFYproposition(xxx, rxCrcError == hal_ethtransceiver_phyerror_rxCrc)
hl_VERIFYproposition(xxx, rxUnicast == hal_ethtransceiver_phyerror_rxUnicast)
hl_VERIFYproposition(xxx, rx64Octets == hal_ethtransceiver_phyerror_rx64Octets)
hl_VERIFYproposition(xxx, txUnicast == hal_ethtransceiver_phyerror_txUnicast)

extern hal_result_t hal_eth_get_errors_info(uint8_t phynum, hal_eth_phy_errors_info_type_t errortype, hal_eth_phy_errorsinfo_t *result)
{
    hal_result_t res = hal_res_NOK_generic;
    res = hal_ethtransceiver_phy_errorinfo(phynum, (hal_ethtransceiver_phyerror_t)errortype, (hal_ethtransceiver_phyerrorinfo_t*)result);
    return(res);
    
} 

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
//  the ETH_IRQHandler() is in hl_eth.c
// ---- isr of the module: end ------

// redefinition of functions used inside ETH_IRQHandler() as defined in hl_eth.c

extern hl_eth_frame_t* hl_eth_frame_new(uint32_t len)
{
    const hal_eth_t id = hal_eth1;
    hal_eth_internal_item_t* intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)];   
    hl_eth_frame_t* ret = (hl_eth_frame_t*)intitem->onframerx.frame_new(len);    
    return(ret);
}

extern void hl_eth_on_frame_received(hl_eth_frame_t* frame)
{
    const hal_eth_t id = hal_eth1;
    hal_eth_internal_item_t* intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)];   

    intitem->onframerx.frame_movetohigherlayer((hal_eth_frame_t*)frame);
	
	// acemor on 03mar2014: it is possible to move up more than one frame inside the ETH isr handler
	// but it is better to alert the receiving task only once

//     if(NULL != intitem->onframerx.frame_alerthigherlayer)
//     {
//         intitem->onframerx.frame_alerthigherlayer();
//     }    
}


extern void hl_eth_alert(void)
{
    const hal_eth_t id = hal_eth1;
    hal_eth_internal_item_t* intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)];   

    if(NULL != intitem->onframerx.frame_alerthigherlayer)
    {
        intitem->onframerx.frame_alerthigherlayer();
    }    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_eth_supported_is(void)
{
    return(hal_eth__theboardconfig.supported);
}

static void s_hal_eth_initted_set(hal_eth_t id)
{
    hl_bits_word_bitset(&s_hal_eth_theinternals.inittedmask, HAL_eth_id2index(id));
}

static hal_boolval_t s_hal_eth_initted_is(hal_eth_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_eth_theinternals.inittedmask, HAL_eth_id2index(id)));
}


static void s_hal_eth_prepare_hl_eth_map(void)
{
    // we must initialise hl_eth_map w/ suited values. 
    // we have built hal_eth__theboardconfig to have compatible layout, but we verify it anyway
    hl_VERIFYproposition(xxx, sizeof(hl_eth_mapping_t) == sizeof(hal_eth_boardconfig_t));       
    hl_eth_map = (hl_eth_mapping_t*)&hal_eth__theboardconfig;
}


#endif//HAL_USE_ETH



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



