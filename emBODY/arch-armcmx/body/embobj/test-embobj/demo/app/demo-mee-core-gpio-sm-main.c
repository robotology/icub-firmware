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

/* @file       demo-mee-core-gpio-sm-main.c
	@brief      This file implements a test for BASE/GPIO/SM in embOBJ built upon to its basic distribution for MEE
	@author     marco.accame@iit.it
    @date       09/02/2011
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

// abslayer 
#include "hal.h"
#include "osal.h"


// embobj
//#include "EoCommon.h"
#include "EOMtheSystem.h"
//#include "EOtheMemoryPool.h"
//#include "EOtheErrormanager.h"


#include "demo-mee-core-gpio-sm-appl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------






// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_test_mee_core_gpio_errman_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);
static void s_test_mee_core_gpio_fill_syscfg(eOmsystem_cfg_t *scfg);

static void dummy(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static eOmsystem_cfg_t sys_cfg =
{   // must have non-NULL values (apart fsalcfg) otherwise the system will not run
    // thus it is a non-cont variable to be filled inside the main()
    .halcfg     = NULL,
    .osalcfg    = NULL,
    .fsalcfg    = NULL
};


static const eOerrman_cfg_t errman_cfg = 
{
    .extfn = 
    {
        .usr_on_error   = s_test_mee_core_gpio_errman_OnError
    }
};


static const eOmempool_cfg_t mempool_cfg = 
{   // change it as u like
    .mode           = eo_mempool_alloc_dynamic,
    .size08         = 0,
    .data08         = NULL,
    .size16         = 0,
    .data16         = NULL,    
    .size32         = 0,
    .data32         = NULL,    
    .size64         = 0,
    .data64         = NULL
};


static const eOmtimerman_cfg_t tmrman_cfg = 
{   // change it as u like
    .priority           = 240, 
    .stacksize          = 512, 
    .messagequeuesize   = 8
};


static const eOmcallbackman_cfg_t cbkman_cfg = 
{   // change it as u like
    .priority   = 202, 
    .stacksize  = 512, 
    .queuesize  = 8
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{
    // fill configuration of the system
    s_test_mee_core_gpio_fill_syscfg(&sys_cfg);

   
    // init the system
    eom_sys_Initialise  (   &sys_cfg,                       // sys
                            NULL,                           // mempool
                            &errman_cfg,                    // errman
                            &eom_timerman_DefaultCfg,       // timerman
                            &eom_callbackman_DefaultCfg     // callbackman
                        );  
    
    // start the system with a user-defined function
    eom_sys_Start(eom_sys_GetHandle(), demo_mee_core_gpio_sm_appl_init00);

    return(0);  // never in here.
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_test_mee_core_gpio_errman_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
    const char err[4][16] = {"info", "warning", "weak error", "fatal error"};
    char str[128];

    snprintf(str, sizeof(str)-1, "[eobj: %s, tsk: %d] %s: %s\n\r", eobjstr, taskid, err[(uint8_t)errtype], info);
    
    hal_trace_puts(str);

    if(errtype <= eo_errortype_warning)
    {
        return;
    }

    for(;;);
}


static void s_test_mee_core_gpio_fill_syscfg(eOmsystem_cfg_t *scfg)
{
    extern const hal_cfg_t *hal_cfgMINE;
    //extern const fsal_params_cfg_t *fsal_params_cfgMINE;
    extern const osal_cfg_t *osal_cfgMINE;

    scfg->halcfg     = hal_cfgMINE;
    scfg->osalcfg    = osal_cfgMINE;
    scfg->fsalcfg    = NULL;

    dummy();
}

static void dummy(void)
{
    static uint8_t a = 0;
    if((NULL == &mempool_cfg) && (NULL == &tmrman_cfg) && (NULL == &cbkman_cfg))
    {
        a++;
        a = a;
    }


    {
//        static volatile eOversion_t    eov = {0};   
//        static volatile uint32_t eov32 = 0;  
//        
//        eov32 = eok_u32version;
//        eov32 = eov32;
//        
//        memcpy(&eov, &eok_version, sizeof(eOversion_t));
//        eov.minor = eov.minor;

    }

    {
        static uint64_t aa = EO_COMMON_MACADDR(1, 2, 3, 4, 5, 6);
        static uint32_t bb = EO_COMMON_IPV4ADDR(1, 2, 3, 4);
        aa = aa;
        bb = bb;




    }
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



