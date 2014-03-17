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


/*  @file       hal_device_ethtransceiver.c
	@brief      This file implements internals of a eth ethtransceiver device.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_ETHTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
//#include "hal_stm32xx_include.h" no need as it is a device
#include "hal_sys.h"

#include "hal_brdcfg.h"

#include "hal_utility_bits.h"

#include "hl_ethtrans.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_ethtransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_ethtransceiver_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_ethtransceiver_id2index(p)           ((uint8_t)((p)))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_ethtransceiver_cfg_t hal_ethtransceiver_cfg_default = 
{ 
    .dummy = 0 
};

extern const hl_ethtrans_mapping_t* hl_ethtrans_map = NULL;

extern void* hl_ethtrans_chip_init_param = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_ethtransceiver_cfg_t                        config;
} hal_device_ethtransceiver_internal_item_t;

typedef struct
{
    uint8_t                                         initted;
    uint8_t                                         started;
    hal_device_ethtransceiver_internal_item_t*      items[hal_ethtransceivers_number];   
} hal_device_ethtransceiver_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_device_ethtransceiver_supported_is(void);
static void s_hal_device_ethtransceiver_initted_set(hal_ethtransceiver_t id);
static hal_bool_t s_hal_device_ethtransceiver_initted_is(hal_ethtransceiver_t id);
static void s_hal_device_ethtransceiver_started_set(hal_ethtransceiver_t id);
static hal_bool_t s_hal_device_ethtransceiver_started_is(hal_ethtransceiver_t id);


static hal_result_t s_hal_device_ethtransceiver_lowlevel_init(const hal_ethtransceiver_cfg_t *cfg);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static hl_ethtrans_mapping_t s_hl_ethtrans_mapping = { .supported = hl_true };



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_device_ethtransceiver_theinternals_t s_hal_device_ethtransceiver_theinternals =
{
    .initted            = 0,
    .started            = 0,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_ethtransceiver_init(const hal_ethtransceiver_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic;


    if(hal_true != s_hal_device_ethtransceiver_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_true == s_hal_device_ethtransceiver_initted_is(hal_ethtransceiver1))
    {
        return(hal_res_OK);
    }

    if(NULL == cfg)
    {
        cfg = &hal_ethtransceiver_cfg_default;
    }

    
    res = s_hal_device_ethtransceiver_lowlevel_init(cfg);

    s_hal_device_ethtransceiver_initted_set(hal_ethtransceiver1);

    return(res);
}


extern hal_result_t hal_ethtransceiver_config(hal_eth_phymode_t *usedmiiphymode)
{
//    hal_result_t res = hal_res_NOK_generic;
    hl_ethtrans_phymode_t usedmiiphmo;

    if(hal_true != s_hal_device_ethtransceiver_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_false == s_hal_device_ethtransceiver_initted_is(hal_ethtransceiver1))
    {
        return(hal_res_NOK_generic);
    }
    

    if(hal_true == s_hal_device_ethtransceiver_started_is(hal_ethtransceiver1))
    {
        hl_ethtrans_chip_getmiiphymode(&usedmiiphmo);
        *usedmiiphymode = (hal_eth_phymode_t)usedmiiphmo;
        return(hal_res_OK);
    }    
 
    //targetphymode = (hal_eth_phymode_auto == targetphymode) ? (hal_brdcfg_device_ethtransceiver__theconfig.devcfg.targetphymode) : (targetphymode);
 
    hl_ethtrans_chip_config(&usedmiiphmo);
    *usedmiiphymode = (hal_eth_phymode_t)usedmiiphmo;

    s_hal_device_ethtransceiver_started_set(hal_ethtransceiver1);

    return(hal_res_OK); 
}


extern hal_bool_t hal_device_ethtransceiver_initted_is(void)
{
    return(s_hal_device_ethtransceiver_initted_is(hal_ethtransceiver1));
}


extern hal_bool_t hal_device_ethtransceiver_started_is(void)
{
    return(s_hal_device_ethtransceiver_started_is(hal_ethtransceiver1));
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern hal_result_t hal_device_ethtransceiver_hid_static_memory_init(void)
{
    memset(&s_hal_device_ethtransceiver_theinternals, 0, sizeof(s_hal_device_ethtransceiver_theinternals));
    return(hal_res_OK);
}

extern hl_result_t hl_ethtrans_chip_init(void *param)
{
    hal_result_t res = hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.init(param);
    return((hl_result_t)res);
}
    
extern hl_result_t hl_ethtrans_chip_config(hl_ethtrans_phymode_t *usedmiiphymode)
{
    hal_eth_phymode_t usedmiiphmo;
    hal_result_t res = hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.config(&usedmiiphmo);
    *usedmiiphymode = (hl_ethtrans_phymode_t)usedmiiphmo;
    return((hl_result_t)res);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_device_ethtransceiver_supported_is(void)
{
    return(hal_brdcfg_device_ethtransceiver__theconfig.supported); 
}

static hal_bool_t s_hal_device_ethtransceiver_initted_is(hal_ethtransceiver_t id)
{
    return(hal_utility_bits_byte_bitcheck(s_hal_device_ethtransceiver_theinternals.initted, HAL_device_ethtransceiver_id2index(id)));
}

static void s_hal_device_ethtransceiver_initted_set(hal_ethtransceiver_t id)
{
    hal_utility_bits_byte_bitset(&s_hal_device_ethtransceiver_theinternals.initted, HAL_device_ethtransceiver_id2index(id));
}


static hal_bool_t s_hal_device_ethtransceiver_started_is(hal_ethtransceiver_t id)
{
    return(hal_utility_bits_byte_bitcheck(s_hal_device_ethtransceiver_theinternals.started, HAL_device_ethtransceiver_id2index(id)));
}


static void s_hal_device_ethtransceiver_started_set(hal_ethtransceiver_t id)
{
     hal_utility_bits_byte_bitset(&s_hal_device_ethtransceiver_theinternals.started, HAL_device_ethtransceiver_id2index(id));
}



static hal_result_t s_hal_device_ethtransceiver_lowlevel_init(const hal_ethtransceiver_cfg_t *cfg)
{
    if((NULL == hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.init)         || 
       (NULL == hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.config)       ||
       (NULL == hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.getphymode) 
      )
    {
        return(hal_res_NOK_generic);        
    }

    // init the hl transceiver ..
    s_hl_ethtrans_mapping.supported = hal_brdcfg_device_ethtransceiver__theconfig.supported;
    hl_ethtrans_map = &s_hl_ethtrans_mapping;
    hl_ethtrans_chip_init_param = hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.initpar;
    //hl_result_t r = hl_ethtrans_chip_init(hl_ethtrans_chip_init_param);
    hl_result_t r = hl_ethtrans_init(NULL);
    
    return((hal_result_t)r);
   
}


#endif//HAL_USE_DEVICE_ETHTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
