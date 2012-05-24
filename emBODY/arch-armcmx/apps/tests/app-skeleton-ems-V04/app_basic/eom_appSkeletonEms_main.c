/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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



/* @file       eom_appSkeletonEms_main.c
	@brief      This file keeps the main of an skeleton application of ems.
                This application implements real time execution of application.
                Here there is a small part of configuration file.
                This application should be used for test purpose.
	@author     valentina.gaggero@iit.it
    @date       02/20/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h"
#include "stdlib.h"

// abslayer 
#include "hal.h"
#include "osal.h"
#include "ipal.h"


// embobj  
#include "EoCommon.h"
#include "EOMtheSystem.h"
#include "EOVtheSystem.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EOMtheIPnet.h"




// keeps info on appl
#include "eom_appSkeletonEms_info.h"
#include "eom_appSkeletonEms_body.h"

// keeps specialisation proper to the board function
//#include "eom_applbasic_specialise.h"


#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"





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
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_appSkeletonEms_main_init(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void)
{
    eom_sys_Initialise( &eom_appSkeletonEms_info_syscfg,
                        NULL,                                           // mempool uses calloc
                        &eom_appSkeletonEms_body_errcfg,               // errman
                        &eom_timerman_DefaultCfg,
                        &eom_callbackman_DefaultCfg
                      );  
    
    #error compile with EO_NVSCFG_USE_CACHED_NVS not defined in file EOnvsCfg.c
    eom_sys_Start(eom_sys_GetHandle(), s_eom_appSkeletonEms_main_init);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


/** @fn         s_eom_appSkeletonEms_main_init(void)
    @brief      It initialises services such arm-environment and tcp/ip. then it calls specific function of this application
    @details    
 **/

static void s_eom_appSkeletonEms_main_init(void)
{
    eOmipnet_cfg_addr_t* eomipnet_addr = NULL;
//    const eEipnetwork_t *ipnet = NULL;

    
    // ----------------------------------------------------------------------------------------------------------------
    // 1. initialise eeprom and the arm-environmemnt

//    hal_eeprom_init(hal_eeprom_i2c_01, NULL);
//    eo_armenv_Initialise(&eom_appSkeletonEms_info_modinfo, NULL);
//    eov_env_SharedData_Synchronise(eo_armenv_GetHandle());



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
        extern const ipal_cfg_t                 *ipal_cfgMINE;
        eOmipnet_cfg_addr_t eomipnet_addr_ipalCfg;
        eomipnet_addr_ipalCfg.macaddr = ipal_cfgMINE->eth_mac;                               
        eomipnet_addr_ipalCfg.ipaddr = ipal_cfgMINE->eth_ip;
        eomipnet_addr_ipalCfg.ipmask = ipal_cfgMINE->eth_mask;    
        eomipnet_addr = &eomipnet_addr_ipalCfg;
        //ipaddr = (uint8_t*)&(eom_applprotoc_info_ipal_cfg->eth_ip);
    }


    // ----------------------------------------------------------------------------------------------------------------
    // 3. start the ipnet

    eom_ipnet_Initialise(&eom_ipnet_DefaultCfg,
                         eom_appSkeletonEms_body_ipal_cfg, 
                         eomipnet_addr,
                         &eom_appSkeletonEms_body_dtgskt_cfg
                         );


    // ----------------------------------------------------------------------------------------------------------------
    // 5. start application core/body

    /* this function initialized all needed services. 
    they are provided by object depended on application,
    but also by object of embObj framework */
    eom_appSkeletonEms_body_services_init();

    /* This function initialized all application's actors.
        An actor is somethis that can do some action. */
    eom_appSkeletonEms_body_actors_init();

    eom_appSkeletonEms_body_actors_start();

}







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



