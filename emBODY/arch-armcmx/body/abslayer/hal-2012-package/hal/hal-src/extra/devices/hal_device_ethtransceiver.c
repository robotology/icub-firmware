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
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_ethtransceiver_cfg_t hal_ethtransceiver_cfg_default = 
{ 
    .dummy = 0 
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_device_ethtransceiver_supported_is(void);
static void s_hal_device_ethtransceiver_initted_set(void);
static hal_bool_t s_hal_device_ethtransceiver_initted_is(void);
static void s_hal_device_ethtransceiver_started_set(void);
static hal_bool_t s_hal_device_ethtransceiver_started_is(void);


static hal_result_t s_hal_device_ethtransceiver_lowlevel_init(const hal_ethtransceiver_cfg_t *cfg);






// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_ethtransceiver_cfg_t s_hal_device_ethtransceiver_cfg      = {.dummy = 0};
static hal_bool_t s_hal_device_ethtransceiver_initted                    =  hal_false;
static hal_bool_t s_hal_device_ethtransceiver_started                    =  hal_false;


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


    if(hal_true == s_hal_device_ethtransceiver_initted_is())
    {
        return(hal_res_OK);
    }

    if(NULL == cfg)
    {
        cfg = &hal_ethtransceiver_cfg_default;
    }

    memcpy(&s_hal_device_ethtransceiver_cfg, cfg, sizeof(hal_ethtransceiver_cfg_t));

    res = s_hal_device_ethtransceiver_lowlevel_init(cfg);

    s_hal_device_ethtransceiver_initted_set();

    return(res);
}


extern hal_result_t hal_ethtransceiver_config(hal_eth_phymode_t targetphymode, hal_eth_phymode_t *usedphymode)
{
//    hal_result_t res = hal_res_NOK_generic;


    if(hal_true != s_hal_device_ethtransceiver_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_false == s_hal_device_ethtransceiver_initted_is())
    {
        return(hal_res_NOK_generic);
    }
    

    if(hal_true == s_hal_device_ethtransceiver_started_is())
    {
        hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.getphymode(usedphymode);
        return(hal_res_OK);
    }    
 
    targetphymode = (hal_eth_phymode_auto == targetphymode) ? (hal_brdcfg_device_ethtransceiver__theconfig.devcfg.targetphymode) : (targetphymode);
 
    hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.config(targetphymode, usedphymode);

    s_hal_device_ethtransceiver_started_set();

    return(hal_res_OK); 
}


extern hal_bool_t hal_device_ethtransceiver_initted_is(void)
{
    return(s_hal_device_ethtransceiver_initted_is());
}


extern hal_bool_t hal_device_ethtransceiver_started_is(void)
{
    return(s_hal_device_ethtransceiver_started_is());
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern uint32_t hal_device_ethtransceiver_hid_getsize(const hal_base_cfg_t *cfg)
{
    return(0);
} 


extern hal_result_t hal_device_ethtransceiver_hid_setmem(const hal_base_cfg_t *cfg, uint32_t *memory)
{

    // removed dependency from nzi ram
    memset(&s_hal_device_ethtransceiver_cfg, 0, sizeof(s_hal_device_ethtransceiver_cfg));
    s_hal_device_ethtransceiver_initted                =  hal_false;
    s_hal_device_ethtransceiver_started                =  hal_false;

    return(hal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_device_ethtransceiver_supported_is(void)
{
    return(hal_brdcfg_device_ethtransceiver__theconfig.supported); 
}

static hal_bool_t s_hal_device_ethtransceiver_initted_is(void)
{
    return(s_hal_device_ethtransceiver_initted);
}

static void s_hal_device_ethtransceiver_initted_set(void)
{
    s_hal_device_ethtransceiver_initted = hal_true;
}


static hal_bool_t s_hal_device_ethtransceiver_started_is(void)
{
    return(s_hal_device_ethtransceiver_started);
}


static void s_hal_device_ethtransceiver_started_set(void)
{
    s_hal_device_ethtransceiver_started = hal_true;
}



static hal_result_t s_hal_device_ethtransceiver_lowlevel_init(const hal_ethtransceiver_cfg_t *cfg)
{
    if((NULL != hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.init)   && 
       (NULL != hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.config) &&
        (NULL != hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.getphymode) )
    {
        return(hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.init(hal_brdcfg_device_ethtransceiver__theconfig.devcfg.chipif.initpar));
    }

    return(hal_res_NOK_generic);    
}


#endif//HAL_USE_DEVICE_ETHTRANSCEIVER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
