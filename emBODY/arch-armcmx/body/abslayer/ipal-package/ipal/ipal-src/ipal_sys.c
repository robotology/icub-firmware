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

/* @file       ipal_sys.c
	@brief      This file implements internal implementation of the ipal xxx module
	@author     marco.accame@iit.it
    @date       12/12/2011
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_SYS


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "ipiit.h"


#include "ipal_base_hid.h"

#include "ipal_ftp_hid.h"
#include "ipal_telnet_hid.h"
#include "ipal_tftp_hid.h"


// - storage for ipal
#include "ipal_ipiit_storage.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_sys_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern uint8_t  ipal_sys_hid_started                                    = 0;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_ipal_sys_internal_ram_reset(void);
static void s_ipal_sys_internal_ram_init(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern ipal_result_t ipal_sys_start(void)
{
    if(0 == ipal_base_hid_cfg.sys_timetick)
    {
        ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_sys_start(): missing configuration");
        return(ipal_res_NOK_generic);
    }

    if(1 == ipal_sys_hid_started)
    {                                                 
        ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_sys_start(): cannot start again");
        return(ipal_res_NOK_generic);
    }

    ipal_sys_hid_started = 1;
    
    // tcpnet: init_TcpNet ();
    init_TcpNet();
    
    return(ipal_res_OK);
}


extern void ipal_sys_process_communication(void)
{                           
    ipal_base_hid_threadsafety_lock();
    
    main_TcpNet();
    
    ipal_base_hid_threadsafety_unlock();
}


extern void ipal_sys_timetick_increment(void)
{
    ipal_base_hid_threadsafety_lock();
    
    timer_tick();
    
    ipal_base_hid_threadsafety_unlock();
}


extern uint32_t ipal_sys_timetick_get(void)
{
    if(0 == ipal_base_hid_cfg.sys_timetick)
    {
        return(0);
    }

    return(ipal_base_hid_cfg.sys_timetick);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t ipal_sys_hid_getsize(const ipal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern ipal_result_t ipal_sys_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
    return(ipal_res_OK); 
}

extern ipal_result_t ipal_sys_hid_init(ipal_cfg_t *cfg)
{
    // nothing is required
    return(ipal_res_OK); 
}


extern ipal_result_t ipal_sys_hid_vars_init(const ipal_cfg_t *cfg)
{
    if( (IPAL_NUMBEROF_SYSMEMPOOLMAXSIZE < cfg->sys_mempoolsize)
//        (IPAL_DNSCACHETABLEMAXSIZE < cfg->dns_tablesize)  not enabled
      )
    {
        return(ipal_res_NOK_generic);
    }
    
    // we reset the internally allocated memory and we init internal variables
    s_ipal_sys_internal_ram_reset();
    s_ipal_sys_internal_ram_init();
    
    // we initialise ipiit storage
    ipal_ipiit_storage_sys_init(cfg);

    
    return(ipal_res_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static void s_ipal_sys_internal_ram_reset(void)
{
    ipal_sys_hid_started = 0;
}


static void s_ipal_sys_internal_ram_init(void)
{
    // nothing to do
}

#endif//IPAL_USE_SYS

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




