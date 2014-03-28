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


/* @file       hal_i2c.c
	@brief      This file implements internal implementation of the hal i2c module.
	@author     valentina.gaggero@iit.it
    @date       28/02/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_I2C

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_brdcfg.h"
#include "hal_base_hid.h"


#include "hal_heap.h"


#include "hl_bits.h" 


#include "hl_i2c.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_i2c.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_i2c_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define USE_HL_I2C_IN_HAL2

#define HAL_i2c_id2index(p)           ((uint8_t)(p))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hal_i2c_cfg_t hal_i2c_cfg_default = 
{ 
    .mode           = hal_i2c_mode_master, 
    .speed          = hal_i2c_speed_400kbps, //hal_i2c_speed_400kbps, 
    .ownaddress     = 0
};

// it must be defined in order to use hl_i2c.
extern const hl_i2c_mapping_t* hl_i2c_map = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_bool_t          locked;
    hal_i2c_devaddr_t   devaddr;   
} hal_i2c_internal_item_t;

typedef struct
{
    uint8_t                     initted;
    hal_i2c_internal_item_t*    items[hal_i2cs_number];   
} hal_i2c_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_i2c_scheduling_suspend(void);
static void s_hal_i2c_scheduling_restart(void);

static hal_boolval_t s_hal_i2c_supported_is(hal_i2c_t id);
static void s_hal_i2c_initted_set(hal_i2c_t id);
static hal_boolval_t s_hal_i2c_initted_is(hal_i2c_t id);

static void s_hal_i2c_status_set(hal_i2c_t id, hal_bool_t locked, hal_i2c_devaddr_t devaddr);

static hal_bool_t s_hal_i2c_is_status_locked(hal_i2c_t id);


static hal_result_t s_hal_i2c_init(hal_i2c_t id, const hal_i2c_cfg_t *cfg);

static hal_result_t s_i2c_read(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


static hal_result_t s_hal_i2c_write(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);

static hal_result_t s_hal_i2c_ping(hal_i2c_t id, hal_i2c_devaddr_t devaddr);

static hal_result_t s_hal_i2c_standby(hal_i2c_t id, hal_i2c_devaddr_t devaddr);

static void s_hal_i2c_prepare_hl_i2c_map(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_i2c_theinternals_t s_hal_i2c_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};


    
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_i2c_init(hal_i2c_t id, const hal_i2c_cfg_t *cfg)
{
    return(s_hal_i2c_init(id, cfg));
}


extern hal_bool_t hal_i2c_initted_is(hal_i2c_t id)
{
    return(s_hal_i2c_initted_is(id));
}


extern hal_result_t hal_i2c_read(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    hal_result_t res;

    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;

    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(hal_res_NOK_unsupported);
    }  
    
    if(NULL == data)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    if(0 == size)
    {
        return(hal_res_OK);
    }
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(id, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_i2c_read(id, devaddr, regaddr, data, size);
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(id, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);
}


extern hal_result_t hal_i2c_write(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    hal_result_t res;
    
    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }   

    devaddr &= ~0x01;    
    
    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(hal_res_NOK_unsupported);
    }  
    
    if(NULL == data)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    if(0 == size)
    {
        return(hal_res_OK);
    }
    
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(id, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_hal_i2c_write(id, devaddr, regaddr, data, size);
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(id, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);    
}

extern hal_result_t hal_i2c_ping(hal_i2c_t id, hal_i2c_devaddr_t devaddr)
{
    hal_result_t res;
    
    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;
    
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(id, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_hal_i2c_ping(id, devaddr);
    
    if(hal_res_OK == res)
    {
        s_hal_i2c_standby(id, devaddr);
    }
    else
    {
        
    }
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(id, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);            
}


extern hal_result_t hal_i2c_standby(hal_i2c_t id, hal_i2c_devaddr_t devaddr)      
{
    hal_result_t res;
    
    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;
    
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(id, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_hal_i2c_standby(id, devaddr);
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(id, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);    
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

static hal_boolval_t s_hal_i2c_supported_is(hal_i2c_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(hal_brdcfg_i2c__theconfig.supported_mask, HAL_i2c_id2index(id)) );
}

static void s_hal_i2c_initted_set(hal_i2c_t id)
{
    hl_bits_byte_bitset(&s_hal_i2c_theinternals.initted, HAL_i2c_id2index(id));
}

static hal_boolval_t s_hal_i2c_initted_is(hal_i2c_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_i2c_theinternals.initted, HAL_i2c_id2index(id)));
}

static void s_hal_i2c_status_set(hal_i2c_t id, hal_bool_t locked, hal_i2c_devaddr_t devaddr)
{
//    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)]->locked   = locked;
    s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)]->devaddr  = devaddr;
//    s_hal_i2c_scheduling_restart();
}

static hal_bool_t s_hal_i2c_is_status_locked(hal_i2c_t id)
{
    return(s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)]->locked);
}

// static hal_i2c_devaddr_t s_hal_i2c_status_devaddr_get(hal_i2c_t id)
// {
//     return(s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)]->devaddr);
// }


static hal_result_t s_hal_i2c_init(hal_i2c_t id, const hal_i2c_cfg_t *cfg)
{
    hal_i2c_internal_item_t* intitem = s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)];
    
    if(NULL == cfg)
    {
        cfg = &hal_i2c_cfg_default;
    }
    
    if(hal_i2c_mode_slave == cfg->mode)
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(hal_true != s_hal_i2c_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }

    if(hal_true == hal_i2c_initted_is(id))
    {
        return(hal_res_OK);
    }  

   // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)] = hal_heap_new(sizeof(hal_i2c_internal_item_t));
        // minimal initialisation of the internal item
        s_hal_i2c_status_set(id, hal_false, 0);      
    }           
    
    // we must initialise hl_i2c_map w/ suited values. 
    s_hal_i2c_prepare_hl_i2c_map();
    
    hl_i2c_init((hl_i2c_t)id, NULL);
     
    
    s_hal_i2c_initted_set(id);
    
    s_hal_i2c_status_set(id, hal_false, 0);
    
    
    return(hal_res_OK);
}
 

static hal_result_t s_i2c_read(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{

    hl_result_t r;    
    hl_i2c_regaddr_t ra;
    ra.numofbytes = regaddr.numofbytes;
    memcpy(&ra, &regaddr, sizeof(hl_i2c_regaddr_t));
    r = hl_i2c_read((hl_i2c_t)id, devaddr, ra, data, size);
    return((hal_result_t)r);    
}



static hal_result_t s_hal_i2c_write(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    hl_result_t r;    
    hl_i2c_regaddr_t ra;
    ra.numofbytes = regaddr.numofbytes;
    memcpy(&ra, &regaddr, sizeof(hl_i2c_regaddr_t));
    r = hl_i2c_write((hl_i2c_t)id, devaddr, ra, data, size);
    return((hal_result_t)r);    
}



static hal_result_t s_hal_i2c_ping(hal_i2c_t id, hal_i2c_devaddr_t devaddr)      
{
    hl_result_t r;    
    r = hl_i2c_ping((hl_i2c_t)id,  devaddr);
    return((hal_result_t)r); 
}


static hal_result_t s_hal_i2c_standby(hal_i2c_t id, hal_i2c_devaddr_t devaddr)      
{
    hl_result_t r;    
    r = hl_i2c_is_device_ready((hl_i2c_t)id, devaddr);
    return((hal_result_t)r); 
}


static void s_hal_i2c_scheduling_suspend(void)
{
    hal_base_osal_scheduling_suspend();
}

static void s_hal_i2c_scheduling_restart(void)
{
    hal_base_osal_scheduling_restart();
}

static void s_hal_i2c_prepare_hl_i2c_map(void)
{
    // we must initialise hl_i2c_map w/ suited values. 
    // we have built hal_brdcfg_i2c__theconfig to have the same layout, but we verify it anyway
    hl_VERIFYproposition(xxx, sizeof(hl_i2c_mapping_t) == sizeof(hal_i2c_hid_brdcfg_t));   
    hl_i2c_map = (hl_i2c_mapping_t*)&hal_brdcfg_i2c__theconfig;
}

#endif//HAL_USE_I2C

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



