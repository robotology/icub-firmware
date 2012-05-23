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

/* @file       eom_emsappl_main.c
	@brief      This file keeps the main of an application on ems using the embobj
	@author     marco.accame@iit.it
    @date       05/21/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#if 0

// abslayer 
#include "hal.h"
#include "hal_trace.h"
#include "osal.h"
#include "ipal.h"


// embobj  
#include "EoCommon.h"
#include "EOMtheSystem.h"
#include "EOVtheSystem.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EOMtheIPnet.h"

#include "EOaction.h"
#include "EOpacket.h"
#include "EOMmutex.h"
#include "EOsocketDatagram.h"


// keeps info on appl
#include "eom_emsappl_info.h"

// keeps specialisation proper to the board function
#include "eom_emsappl_specialise.h"


#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"

#endif



#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "hal_trace.h"

#include "EoCommon.h"
#include "EOMtheSystem.h"
#include "EOVtheSystem.h"
#include "EOtheErrorManager.h"
#include "EOMtheEMSappl.h"

#include "eom_emsappl_info.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




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

static void s_eom_emsappl_main_init(void);

static void s_eom_emsappl_main_startup_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eOerrman_cfg_t  s_eom_emsappl_main_errcfg_startup = 
{
    .extfn.usr_on_error = s_eom_emsappl_main_startup_OnError
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{

    eom_sys_Initialise( &eom_emsappl_info_syscfg,
                        NULL,                         // mempool uses calloc
                        &s_eom_emsappl_main_errcfg_startup,                
                        &eom_timerman_DefaultCfg,
                        &eom_callbackman_DefaultCfg
                      );  
    
    eom_sys_Start(eom_sys_GetHandle(), s_eom_emsappl_main_init);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


/** @fn         static void s_eom_emsappl_main_init(void)
    @brief      It initialises the emsappl 
     @details    bla bla bla.
 **/

static void s_eom_emsappl_main_init(void)
{
    eom_emsappl_Initialise(NULL);
}



static void s_eom_emsappl_main_startup_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
    const char err[4][16] = {"info", "warning", "weak error", "fatal error"};
    char str[128];

    snprintf(str, sizeof(str)-1, "startup: [eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, err[(uint8_t)errtype], info);
    hal_trace_puts(str);

    if(errtype <= eo_errortype_warning)
    {
        return;
    }
    
    eov_sys_Stop(eov_sys_GetHandle());

    for(;;);
}


//////////////////////////////////////////////////////////////////////////////
// old s_eom_emsappl_main_init():
#if 0    
    //uint8_t *ipaddr = (uint8_t*)&(eom_emsappl_info_ipal_cfg->eth_ip);
    eOmipnet_cfg_addr_t* eomipnet_addr = NULL;
    const eEipnetwork_t *ipnet = NULL;

    
    // ----------------------------------------------------------------------------------------------------------------
    // 1. initialise eeprom and the arm-environmemnt

    hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    eo_armenv_Initialise(&eom_emsappl_info_modinfo, NULL);
    eov_env_SharedData_Synchronise(eo_armenv_GetHandle());



    // ----------------------------------------------------------------------------------------------------------------
    // 2. initialise the parameters for ipnet with params taken from the arm-environment (or from ipal-cfg)


    // retrieve the configuration for ipnetwork
#ifndef _FORCE_NETWORK_FROM_IPAL_CFG
    if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
    {
        eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout

        //ipaddr = (uint8_t*)&(eomipnet_addr->ipaddr);
    }

    else
#endif
    {
        eomipnet_addr = NULL;
        //ipaddr = (uint8_t*)&(eom_emsappl_info_ipal_cfg->eth_ip);
    }


    // ----------------------------------------------------------------------------------------------------------------
    // 3. start the ipnet

    eom_ipnet_Initialise(&eom_ipnet_DefaultCfg,
                         eom_emsappl_specialise_ipal_cfg, 
                         eomipnet_addr,
                         &eom_emsappl_specialise_dtgskt_cfg
                         );



    // ----------------------------------------------------------------------------------------------------------------
    // 5. start task-upd-server

    eom_emsappl_specialise_updserver_start();  


    // ----------------------------------------------------------------------------------------------------------------
    // 5. call specialisation function

    eom_emsappl_specialise_otherthings();
#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



