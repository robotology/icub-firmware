/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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



// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_UTIL_ETHTRANS)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files

#include "hl_gpio.h"
#include "hl_arch.h"
#include "hl_sys.h"

#include "hl_bits.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_ethtrans.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_ethtrans_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hl_ethtrans_cfg_t hl_ethtrans_cfg_default = 
{ 
    .dummy = 0 
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hl_ethtrans_cfg_t                       config;
} hl_ethtrans_internal_item_t;

typedef struct
{
    uint8_t                                 initted;
    uint8_t                                 started;
    hl_ethtrans_internal_item_t*            items[1];   
} hl_ethtrans_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hl_bool_t s_hl_ethtrans_supported_is(void);
static void s_hl_ethtrans_initted_set(void);
static hl_bool_t s_hl_ethtrans_initted_is(void);
static void s_hl_ethtrans_started_set(void);
static hl_bool_t s_hl_ethtrans_started_is(void);


//static hl_result_t s_hl_ethtrans_lowlevel_init(const hl_ethtrans_cfg_t *cfg);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_ethtrans_theinternals_t s_hl_ethtrans_theinternals =
{
    .initted            = 0,
    .started            = 0,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hl_result_t hl_ethtrans_init(const hl_ethtrans_cfg_t *cfg)
{
    hl_result_t res = hl_res_NOK_generic;


    if(hl_true != s_hl_ethtrans_supported_is())
    {
        return(hl_res_NOK_unsupported);
    }


    if(hl_true == s_hl_ethtrans_initted_is())
    {
        return(hl_res_OK);
    }

    if(NULL == cfg)
    {
        cfg = &hl_ethtrans_cfg_default;
    }

    
    res = hl_ethtrans_chip_init(hl_ethtrans_chip_init_param);

    s_hl_ethtrans_initted_set();

    return(res);
}


extern hl_result_t hl_ethtrans_config(hl_ethtrans_phymode_t *usedmiiphymode)
{

    if(hl_true != s_hl_ethtrans_supported_is())
    {
        return(hl_res_NOK_unsupported);
    }


    if(hl_false == s_hl_ethtrans_initted_is())
    {
        return(hl_res_NOK_generic);
    }
    

    if(hl_true == s_hl_ethtrans_started_is())
    {
        return(hl_ethtrans_getmiiphymode(usedmiiphymode));
    }    
 
//    targetphymode = (hl_eth_phymode_auto == targetphymode) ? (hl_brdcfg_device_ethtrans__theconfig.devcfg.targetphymode) : (targetphymode);
 
//    hl_brdcfg_device_ethtrans__theconfig.devcfg.chipif.config(targetphymode, usedphymode);

    hl_ethtrans_chip_config(usedmiiphymode);

    s_hl_ethtrans_started_set();

    return(hl_res_OK); 
}


extern hl_result_t hl_ethtrans_getmiiphymode(hl_ethtrans_phymode_t *usedmiiphymode)
{

    if(hl_true != s_hl_ethtrans_supported_is())
    {
        return(hl_res_NOK_unsupported);
    }


    if(hl_false == s_hl_ethtrans_initted_is())
    {
        return(hl_res_NOK_generic);
    }
    

    if(hl_false == s_hl_ethtrans_started_is())
    {
        return(hl_res_NOK_generic);
    }    
 

    hl_ethtrans_chip_getmiiphymode(usedmiiphymode);

    return(hl_res_OK); 
}

extern hl_bool_t hl_ethtrans_initted_is(void)
{
    return(s_hl_ethtrans_initted_is());
}


extern hl_bool_t hl_ethtrans_started_is(void)
{
    return(s_hl_ethtrans_started_is());
}

__weak extern hl_result_t hl_ethtrans_chip_init(void* param)
{
    return(hl_res_OK);
}


__weak extern hl_result_t hl_ethtrans_chip_config(hl_ethtrans_phymode_t *usedmiiphymode)
{
    *usedmiiphymode = hl_ethtrans_phymode_none;
    
    return(hl_res_OK);
}


__weak extern hl_result_t hl_ethtrans_chip_getmiiphymode(hl_ethtrans_phymode_t *usedmiiphymode)
{
    if(NULL == usedmiiphymode)
    {
        return(hl_res_NOK_generic);
    }
    
    *usedmiiphymode = hl_ethtrans_phymode_none;
    
    return(hl_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hl_bool_t s_hl_ethtrans_supported_is(void)
{
    if(NULL == hl_ethtrans_map)
    {
        return(hl_false);
    }
    return(hl_ethtrans_map->supported); 
}

static hl_bool_t s_hl_ethtrans_initted_is(void)
{
    return(s_hl_ethtrans_theinternals.initted);
}

static void s_hl_ethtrans_initted_set(void)
{
    s_hl_ethtrans_theinternals.initted = hl_true;
}


static hl_bool_t s_hl_ethtrans_started_is()
{
    return(s_hl_ethtrans_theinternals.started);
}


static void s_hl_ethtrans_started_set(void)
{
     s_hl_ethtrans_theinternals.started = hl_true;
}



// static hl_result_t s_hl_ethtrans_lowlevel_init(const hl_ethtrans_cfg_t *cfg)
// {
// //    if((NULL != hl_brdcfg_device_ethtrans__theconfig.devcfg.chipif.init)   && 
// //       (NULL != hl_brdcfg_device_ethtrans__theconfig.devcfg.chipif.config) &&
// //        (NULL != hl_brdcfg_device_ethtrans__theconfig.devcfg.chipif.getphymode) )
// //    {
// //        return(hl_brdcfg_device_ethtrans__theconfig.devcfg.chipif.init(hl_brdcfg_device_ethtrans__theconfig.devcfg.chipif.initpar));
// //    }

//     return(hl_res_NOK_generic);    
// }


#endif//HL_USE_UTIL_ETHTRANS

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
