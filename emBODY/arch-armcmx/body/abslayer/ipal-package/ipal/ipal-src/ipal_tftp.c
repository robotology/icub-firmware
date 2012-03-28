/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       ipal_tftpc
	@brief      This file implements internal implementation of the ipal tftp module
	@author     marco.accame@iit.it
    @date       12/12/2011
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_TFTP


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "ipiit.h"

#include "ipal_base_hid.h"
#include "ipal_sys_hid.h"

// - storage for ipal
#include "ipal_ipiit_storage.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_tftp_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern uint8_t ipal_tftp_hid_active                                     = 0;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_ipal_tftp_internal_ram_reset(void);
static void s_ipal_tftp_internal_ram_init(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern ipal_bool_t ipal_tftp_isenabled(void)
{
    return(ipal_base_hid_cfg.tftp_enable);
}


extern ipal_bool_t ipal_tftp_isactive(void)
{
    return(ipal_tftp_hid_active);
}


extern ipal_result_t ipal_tftp_stop(void)
{
    if((0 == ipal_sys_hid_started) || (0 == ipal_base_hid_cfg.tftp_enable))
    {
        return(ipal_res_NOK_generic);
    }
    
    // if i am in here, it means that we have teh cfg, but also that the tftp is enabled and initted,
    // thus we have the proper functions to run
    
    ipal_tftp_hid_active = 0;
    return(ipal_res_OK);
}          


extern ipal_result_t ipal_tftp_restart(void)
{
    if((0 == ipal_sys_hid_started) || (0 == ipal_base_hid_cfg.tftp_enable))
    {
        return(ipal_res_NOK_generic);
    }
    
    // if i am in here, it means that we have teh cfg, but also that the ftp is enabled and initted,
    // thus we have the proper functions to run
    
    ipal_tftp_hid_active = 1;
    return(ipal_res_OK);
} 



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t ipal_tftp_hid_getsize(const ipal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern ipal_result_t ipal_tftp_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
    return(ipal_res_OK); 
}

extern ipal_result_t ipal_tftp_hid_init(ipal_cfg_t *cfg)
{
    // something is required
    
    if(1 == cfg->tftp_enable)
    {
        if((NULL == cfg->extfn.fopen) || (NULL == cfg->extfn.fclose) || 
           (NULL == cfg->extfn.fread) || (NULL == cfg->extfn.fwrite) || 
           (NULL == cfg->extfn.fseek) || (NULL == cfg->extfn.ftell)    )
        {
            ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_tftp_hid_init(): missing tftp functions");
            return(ipal_res_NOK_generic);
        }
        
        ipal_tftp_hid_active = 1;
        
    }    
    
    return(ipal_res_OK); 
}


extern ipal_result_t ipal_tftp_hid_vars_init(const ipal_cfg_t *cfg)
{
    if( 0
//        (IPAL_TFTPMAXSESSIONS < cfg->tftp_connsnum)         ||
      )
    {
        return(ipal_res_NOK_generic);
    }
 
    // we reset the internally allocated memory and we init internal variables 
    s_ipal_tftp_internal_ram_reset();
    s_ipal_tftp_internal_ram_init();
    
    // we initialise ipiit storage
    ipal_ipiit_storage_tftp_init(cfg);
    
    return(ipal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_ipal_tftp_internal_ram_reset(void)
{
    ipal_tftp_hid_active = 0;
}

static void s_ipal_tftp_internal_ram_init(void)
{
    // nothing to do
}


#endif//IPAL_USE_TFTP


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




