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


/*  @file       hal_ethtransceiver.c
	@brief      This file implements internals of a eth ethtransceiver device.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
//#include "hal_middleware_interface.h"

#ifdef HAL_USE_ETHTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_heap.h"
#include "hl_bits.h"
#include "hl_ethtrans.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_ethtransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_ethtransceiver_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_ethtransceiver_id2index(p)           ((uint8_t)((p)))

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
} hal_ethtransceiver_internal_item_t;

typedef struct
{
    uint8_t                                         inittedmask08;
    uint8_t                                         startedmask08;
    hal_ethtransceiver_internal_item_t*             items[hal_ethtransceivers_number];   
} hal_ethtransceiver_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_ethtransceiver_supported_is(hal_ethtransceiver_t id);
static void s_hal_ethtransceiver_initted_set(hal_ethtransceiver_t id);
static hal_bool_t s_hal_ethtransceiver_initted_is(hal_ethtransceiver_t id);
static void s_hal_ethtransceiver_started_set(hal_ethtransceiver_t id);
static hal_bool_t s_hal_ethtransceiver_started_is(hal_ethtransceiver_t id);


static hal_result_t s_hal_ethtransceiver_lowlevel_init(const hal_ethtransceiver_cfg_t *cfg);

static void s_hal_ethtransceiver_prepare_hl_ethtrans_map(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static hl_ethtrans_mapping_t s_hal_ethtransceiver_hl_ethtrans_mapping = { .supported = hl_true };



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_ethtransceiver_theinternals_t s_hal_ethtransceiver_theinternals =
{
    .inittedmask08      = 0,
    .startedmask08      = 0,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_ethtransceiver_init(const hal_ethtransceiver_cfg_t *cfg)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    
    hal_result_t res = hal_res_NOK_generic;
    hal_ethtransceiver_internal_item_t *intitem = s_hal_ethtransceiver_theinternals.items[HAL_ethtransceiver_id2index(id)];

    if(hal_true != s_hal_ethtransceiver_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }

    if(NULL == cfg)
    {
        cfg = &hal_ethtransceiver_cfg_default;
    }
    
    if(hal_true == s_hal_ethtransceiver_initted_is(id))
    {
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_ethtransceiver_cfg_t)))
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
        intitem = s_hal_ethtransceiver_theinternals.items[HAL_ethtransceiver_id2index(id)] = hal_heap_new(sizeof(hal_ethtransceiver_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }    
    memcpy(&intitem->config, cfg, sizeof(hal_ethtransceiver_cfg_t));

    
    res = s_hal_ethtransceiver_lowlevel_init(cfg);

    if(hal_res_OK == res)
    {
        s_hal_ethtransceiver_initted_set(id);
    }

    return(res);
}



extern hal_result_t hal_ethtransceiver_start(hal_ethtransceiver_phymode_t *usedmiiphymode)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    hl_result_t r = hl_res_NOK_generic;    
    hl_ethtrans_phymode_t usedmiiphmo;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK) 
    if(hal_false == s_hal_ethtransceiver_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    if(hal_true == s_hal_ethtransceiver_started_is(id))
    {
        r = hl_ethtrans_chip_getmiiphymode(&usedmiiphmo);
        if(NULL != usedmiiphymode)
        {
            r = hl_ethtrans_getmiiphymode(&usedmiiphmo);
            *usedmiiphymode = (hal_ethtransceiver_phymode_t)usedmiiphmo;
        }
        return((hal_result_t)r);
    }

    r = hl_ethtrans_start(&usedmiiphmo);
    if(hl_res_OK != r)
    {
        return((hal_result_t)r);
    }
    
    if(NULL != usedmiiphymode)
    {
        *usedmiiphymode = (hal_ethtransceiver_phymode_t)usedmiiphmo;
    }

    s_hal_ethtransceiver_started_set(id);

    return(hal_res_OK); 
}

// extern hal_result_t hal_ethtransceiver_start(hal_ethtransceiver_phymode_t *usedmiiphymode)
// {
//     const hal_ethtransceiver_t id = hal_ethtransceiver1;
//     
//     hl_ethtrans_phymode_t usedmiiphmo;

// #if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)  
//     if(hal_false == s_hal_ethtransceiver_initted_is(id))
//     {
//         return(hal_res_NOK_generic);
//     }
// #endif
//     
//     if(hal_true == s_hal_ethtransceiver_started_is(id))
//     {
//         hl_ethtrans_chip_getmiiphymode(&usedmiiphmo);
//         *usedmiiphymode = (hal_ethtransceiver_phymode_t)usedmiiphmo;
//         return(hal_res_OK);
//     }    
//  
//     hl_ethtrans_chip_start(&usedmiiphmo);
//     *usedmiiphymode = (hal_ethtransceiver_phymode_t)usedmiiphmo;

//     s_hal_ethtransceiver_started_set(id);

//     return(hal_res_OK); 
// }


extern hal_bool_t hal_ethtransceiver_initted_is(void)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    return(s_hal_ethtransceiver_initted_is(id));
}


extern hal_bool_t hal_ethtransceiver_started_is(void)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    return(s_hal_ethtransceiver_started_is(id));
}


extern hal_result_t hal_ethtransceiver_phy_numberof(uint8_t *num)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)      
    if(NULL == num)
    {
        return(hal_res_NOK_nullpointer);
    }
#endif
    
    *num = hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].cfg.numofphysicals;

    return(hal_res_OK);       
}

extern hal_result_t hal_ethtransceiver_phy_linkupmask(uint8_t *mask)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)  
    if(hal_false == s_hal_ethtransceiver_started_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)      
    if(NULL == mask)
    {
        return(hal_res_NOK_nullpointer);
    }
#endif       

    hal_result_t res = hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.phylinkupmask(id, mask);
    return(res);    
}


extern hal_result_t hal_ethtransceiver_phy_status(hal_ethtransceiver_phystatus_t *array, uint8_t sizeofarray)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)  
    if(hal_false == s_hal_ethtransceiver_started_is(id))
    {
        return(hal_res_NOK_generic);
    }    
#endif

#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)     
    if(NULL == array)
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    hal_result_t res = hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.phystatus(id, array, sizeofarray);
    return(res);    
}

extern hal_result_t hal_ethtransceiver_phy_errorinfo(uint8_t phynum, hal_ethtransceiver_phyerror_t error, hal_ethtransceiver_phyerrorinfo_t *result)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)  
    if(hal_false == s_hal_ethtransceiver_started_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif

#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)      
    if(NULL == result)
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    hal_result_t res = hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.phyerrorinfo(id, phynum, error, result);
    return(res);        
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

// redefinition of hl_ethtrans_chip_init().
extern hl_result_t hl_ethtrans_chip_init(void *param)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    
    if(NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.init)
    {
        return(hl_res_NOK_generic);
    }
    hal_result_t res = hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.init(id, hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].cfg.initpar);
    return((hl_result_t)res);
}
 
// called by hl_ethtrans_start()
// extern hl_result_t hl_ethtrans_chip_start(hl_ethtrans_phymode_t *usedmiiphymode)
// {
//     const hal_ethtransceiver_t id = hal_ethtransceiver1;
//     
//     if(NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.start)
//     {
//         return(hl_res_NOK_generic);
//     }
//     hal_ethtransceiver_phymode_t usedmiiphmo = hal_ethtransceiver_phymode_none;
//     hal_result_t res = hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.start(id, &usedmiiphmo);
//     *usedmiiphymode = (hl_ethtrans_phymode_t)usedmiiphmo;
//     return((hl_result_t)res);
// }

extern hl_result_t hl_ethtrans_chip_start(hl_ethtrans_phymode_t *usedmiiphymode)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    
    if(NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.start)
    {
        return(hl_res_NOK_generic);
    }
    hal_ethtransceiver_phymode_t usedmiiphmo = hal_ethtransceiver_phymode_none;
    hal_result_t res = hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.start(id, &usedmiiphmo);
    *usedmiiphymode = (hl_ethtrans_phymode_t)usedmiiphmo;
    return((hl_result_t)res);
}


extern hl_result_t hl_ethtrans_chip_getmiiphymode(hl_ethtrans_phymode_t *usedmiiphymode)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    
    if(NULL == usedmiiphymode)
    {
        return(hl_res_NOK_generic);
    }
        
    if(NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.getmiiphymode)
    {
        return(hl_res_NOK_generic);
    }
    hal_ethtransceiver_phymode_t usedmiiphmo = hal_ethtransceiver_phymode_none;
    hal_result_t res = hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.getmiiphymode(id, &usedmiiphmo);
    *usedmiiphymode = (hl_ethtrans_phymode_t)usedmiiphmo;
    return((hl_result_t)res);    
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_ethtransceiver_supported_is(hal_ethtransceiver_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_ethtransceiver__theboardconfig.supportedmask, HAL_ethtransceiver_id2index(id)));
}

static hal_bool_t s_hal_ethtransceiver_initted_is(hal_ethtransceiver_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_ethtransceiver_theinternals.inittedmask08, HAL_ethtransceiver_id2index(id)));
}

static void s_hal_ethtransceiver_initted_set(hal_ethtransceiver_t id)
{
    hl_bits_byte_bitset(&s_hal_ethtransceiver_theinternals.inittedmask08, HAL_ethtransceiver_id2index(id));
}


static hal_bool_t s_hal_ethtransceiver_started_is(hal_ethtransceiver_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_ethtransceiver_theinternals.startedmask08, HAL_ethtransceiver_id2index(id)));
}


static void s_hal_ethtransceiver_started_set(hal_ethtransceiver_t id)
{
     hl_bits_byte_bitset(&s_hal_ethtransceiver_theinternals.startedmask08, HAL_ethtransceiver_id2index(id));
}



static hal_result_t s_hal_ethtransceiver_lowlevel_init(const hal_ethtransceiver_cfg_t *cfg)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    
    if((NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.init)            || 
       (NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.start)           ||
       (NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.getmiiphymode)   ||
       (NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.phylinkupmask)   || 
       (NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.phystatus)       ||
       (NULL == hal_ethtransceiver__theboardconfig.driver[HAL_ethtransceiver_id2index(id)].fun.phyerrorinfo) 
      )
    {
        return(hal_res_NOK_generic);        
    }
    
    // we dont call the user-defined device interface for the ethtransceiver in here, but by means of hl_ethtrans_init()
    // which we must prepare
    s_hal_ethtransceiver_prepare_hl_ethtrans_map();  
    return((hal_result_t)hl_ethtrans_init(NULL));  
}


static void s_hal_ethtransceiver_prepare_hl_ethtrans_map(void)
{
    const hal_ethtransceiver_t id = hal_ethtransceiver1;
    
    // we must initialise hl_ethtrans_map w/ suited values. 
    // we have created a local hl_ethtrans_mapping_t variable (s_hal_ethtransceiver_hl_ethtrans_mapping)
    // which we set according what in hal_ethtransceiver__theboardconfig. then we use the pointer to this variable.
    s_hal_ethtransceiver_hl_ethtrans_mapping.supported = s_hal_ethtransceiver_supported_is(id);
    hl_ethtrans_map = &s_hal_ethtransceiver_hl_ethtrans_mapping;
}


#endif//HAL_USE_ETHTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
