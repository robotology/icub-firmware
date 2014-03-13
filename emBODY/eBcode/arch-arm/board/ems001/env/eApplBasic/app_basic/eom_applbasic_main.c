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

/* @file       eom_appl_basic_main.c
	@brief      This file keeps the main of an application using the embobj
	@author     marco.accame@iit.it
    @date       01/11/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h"
#include "stdlib.h"

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
#include "eom_applbasic_info.h"

// keeps specialisation proper to the board function
#include "eom_applbasic_specialise.h"


#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"

#include "eEsharedServices.h" 



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

#if     defined(_USE_IPADDR_FROM_IPAL_CFG_)
    #warning --> the ip address is taken from ipal cfg, not from eeprom. to get it from eeprom undef _USE_IPADDR_FROM_IPAL_CFG_
#else
    #warning --> the ip address is taken from eeprom. to get it from ipal cfg please define _USE_IPADDR_FROM_IPAL_CFG_
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_applbasic_main_init(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



static const eOmempool_cfg_t memcfg =
{
    .mode               = eo_mempool_alloc_dynamic,
    .memallocator       = osal_base_memory_new
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{

    eom_sys_Initialise( &eom_applbasic_info_syscfg,
                        &memcfg,                                        // mempool 
                        &eom_applbasic_specialise_errcfg,               // errman
                        &eom_timerman_DefaultCfg,
                        &eom_callbackman_DefaultCfg
                      );  
    
    eom_sys_Start(eom_sys_GetHandle(), s_eom_applbasic_main_init);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


/** @fn         static void s_eom_applbasic_main_init(void)
    @brief      It initialises services such arm-environment and tcp/ip. then it calls a specific specialisation 
                function.
    @details    bla bla bla.
 **/

static void s_eom_applbasic_main_init(void)
{
    //uint8_t *ipaddr = (uint8_t*)&(eom_applbasic_info_ipal_cfg->eth_ip);
    eOmipnet_cfg_addr_t* eomipnet_addr = NULL;
    const eEipnetwork_t *ipnet = NULL;

    
    // ----------------------------------------------------------------------------------------------------------------
    // 1. initialise eeprom and the arm-environmemnt

    hal_eeprom_init(hal_eeprom_i2c_01, NULL);
      
    eo_armenv_Initialise(&eom_applbasic_info_modinfo, NULL);
    eov_env_SharedData_Synchronise(eo_armenv_GetHandle());



    // ----------------------------------------------------------------------------------------------------------------
    // 2. initialise the parameters for ipnet with params taken from the arm-environment (or from ipal-cfg)


    // retrieve the configuration for ipnetwork
#ifndef _USE_IPADDR_FROM_IPAL_CFG_
    if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
    {
        eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout

        //ipaddr = (uint8_t*)&(eomipnet_addr->ipaddr);
    }

    else
#endif
    {
        eomipnet_addr = NULL;
        //ipaddr = (uint8_t*)&(eom_applbasic_info_ipal_cfg->eth_ip);
    }


    // ----------------------------------------------------------------------------------------------------------------
    // 3. start the ipnet

    eom_ipnet_Initialise(&eom_ipnet_DefaultCfg,
                         eom_applbasic_specialise_ipal_cfg, 
                         eomipnet_addr,
                         &eom_applbasic_specialise_dtgskt_cfg
                         );



    // ----------------------------------------------------------------------------------------------------------------
    // 5. start task-upd-server

    eom_applbasic_specialise_updserver_start();  


    // ----------------------------------------------------------------------------------------------------------------
    // 5. call specialisation function

    eom_applbasic_specialise_otherthings();
    
    hal_trace_puts("quitting the s_eom_applbasic_main_init() function");

}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



