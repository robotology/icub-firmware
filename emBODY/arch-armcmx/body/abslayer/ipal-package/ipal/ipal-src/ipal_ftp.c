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

/* @file       ipal_ftp.c
	@brief      This file implements internal implementation of the ipal ftp module
	@author     marco.accame@iit.it
    @date       12/12/2011
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_FTP

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

#include "ipal_ftp_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// tells if the service is active or not
extern uint8_t ipal_ftp_hid_active                                      = 0;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_ipal_ftp_internal_ram_reset(void);
static void s_ipal_ftp_internal_ram_init(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

extern ipal_ftp_info_search_t *ipal_ftp_hid_finfo = NULL;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern ipal_bool_t ipal_ftp_isenabled(void)
{
    return(ipal_base_hid_cfg.ftp_enable);
}


extern ipal_bool_t ipal_ftp_isactive(void)
{
    return(ipal_ftp_hid_active);
}

extern ipal_result_t ipal_ftp_stop(void)
{
    if((0 == ipal_sys_hid_started) || (0 == ipal_base_hid_cfg.ftp_enable))
    {
        return(ipal_res_NOK_generic);
    }
    
    // if i am in here, it means that we have teh cfg, but also that the ftp is enabled and initted,
    // thus we have the proper functions to run
    
    ipal_ftp_hid_active = 0;
    return(ipal_res_OK);
}          

extern ipal_result_t ipal_ftp_restart(void)
{
    if((0 == ipal_sys_hid_started) || (0 == ipal_base_hid_cfg.ftp_enable))
    {
        return(ipal_res_NOK_generic);
    }
    
    // if i am in here, it means that we have teh cfg, but also that the ftp is enabled and initted,
    // thus we have the proper functions to run
    
    ipal_ftp_hid_active = 1;
    return(ipal_res_OK);
} 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t ipal_ftp_hid_getsize(const ipal_cfg_t *cfg)
{
    uint32_t size = 0;
    
    if(0 != cfg->ftp_enable)
    {
        size += sizeof(ipal_ftp_info_search_t);
    }
    
    return(size);
}

extern ipal_result_t ipal_ftp_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
    uint8_t *ram08 = (uint8_t*)memory;

    if(0 != cfg->ftp_enable)
    {
        ipal_ftp_hid_finfo = (ipal_ftp_info_search_t*)ram08;
        ram08 += sizeof(ipal_ftp_info_search_t);
    }    
 
    return(ipal_res_OK); 
}

extern ipal_result_t ipal_ftp_hid_init(ipal_cfg_t *cfg)
{
    // something is required
    
    if(1 == cfg->ftp_enable)
    {
        if((NULL == cfg->extfn.fopen) || (NULL == cfg->extfn.fclose) || 
           (NULL == cfg->extfn.fread) || (NULL == cfg->extfn.fwrite) || 
           (NULL == cfg->extfn.fsal_delete) || (NULL == cfg->extfn.fsal_rename) ||
           (NULL == cfg->extfn.fsal_find) )
        {
            ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_ftp_hid_init(): missing ftp functions");
            return(ipal_res_NOK_generic);
        }
        
        ipal_ftp_hid_active = 1; 
 
    } 
 
    return(ipal_res_OK); 
}


extern ipal_result_t ipal_ftp_hid_vars_init(const ipal_cfg_t *cfg)
{
    // in here we dont allocate memory, as it is statically allocated.
    // we just check that values are less than max.

    if( 0
//        (IPAL_FTPMAXSESSIONS < cfg->ftp_connsnum)           ||
      )
    {
        return(ipal_res_NOK_generic);
    }

    // we reset the internally allocated memory and we init internal variables 
    s_ipal_ftp_internal_ram_reset();
    s_ipal_ftp_internal_ram_init();
    
    // we initialise ipiit storage
    ipal_ipiit_storage_ftp_init(cfg);

    
    return(ipal_res_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




static void s_ipal_ftp_internal_ram_reset(void)
{
    ipal_ftp_hid_finfo = NULL;
    ipal_ftp_hid_active = 0;
}

static void s_ipal_ftp_internal_ram_init(void)
{
    // nothing to do
}


#endif//IPAL_USE_FTP


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




