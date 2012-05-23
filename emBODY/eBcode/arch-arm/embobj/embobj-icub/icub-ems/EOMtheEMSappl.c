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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"
#include "EOsm.h"
#include "eOcfg_sm_EMSappl.h"


#include "EOMtheSystem.h"
#include "EOVtheSystem.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EOMtheIPnet.h"


#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"

#include "EOMtheEMSsocket.h"

#include "EOMtheEMStransceiver.h"

#include "EOMtheEMSconfigurator.h"

#include "EOMtheEMSerror.h"

#include "EOaction_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSappl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSappl_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsappl_cfg_t eom_emsappl_DefaultCfg = 
{
    EO_INIT(.hostipv4addr)      EO_COMMON_IPV4ADDR(10, 0, 1, 200), 
    EO_INIT(.hostipv4port)      12345
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_emsappl_environment_init(void);
static void s_eom_emsappl_ipnetwork_init(void);


static void s_eom_emsappl_theemssocket_init(void);

static void s_eom_emsappl_theemstransceiver_init(void);

static void s_eom_emsappl_theemserror_init(void);

static void s_eom_emsappl_theemsconfigurator_init(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSappl";

 
static EOMtheEMSappl s_emsappl_singleton = 
{
    EO_INIT(.sm)                NULL,
	EO_INIT(.cfg) 
    {   
        EO_INIT(.hostipv4addr)      EO_COMMON_IPV4ADDR(10, 0, 1, 200), 
        EO_INIT(.hostipv4port)      12345
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSappl * eom_emsappl_Initialise(const eOemsappl_cfg_t *emsapplcfg)
{
    if(NULL != s_emsappl_singleton.sm)
    {
        return(&s_emsappl_singleton);
    }
    
    if(NULL == emsapplcfg)
    {
        emsapplcfg = &eom_emsappl_DefaultCfg;
    }

    memcpy(&s_emsappl_singleton.cfg, emsapplcfg, sizeof(eOemsappl_cfg_t));

    // do whatever is needed
    
    // 1. create the sm.
    s_emsappl_singleton.sm = eo_sm_New(eo_cfg_sm_EMSappl_Get());
    
    
    // 2. initialise the environment and the ip network.
    s_eom_emsappl_environment_init();
    s_eom_emsappl_ipnetwork_init();
    
    // 3. initialise the EOMtheEMSsocket and the EOMtheEMStransceiver   
    s_eom_emsappl_theemssocket_init();    
    s_eom_emsappl_theemstransceiver_init();
    
    // 4. initialise the EOMtheEMSerror
    s_eom_emsappl_theemserror_init();    
    
    // 5. initialise the EOMtheEMSconfigurator
    s_eom_emsappl_theemsconfigurator_init();
    
    // instanstiate the EOMtheEMSrunner, 
    // they have on their inside the tasks which they need.
    // - the EOMtheEMSconfigurer has an event based task which uses the EOMtheEMSsocket configured to send it an event.
    //   the task retrieves teh packet and passed it to the EOMtheEMStransceiver.
    // - the EOMtheEMSrunner has three tasks: rx, do, tx.   



    // call usrdef initialise
    eom_emsappl_hid_userdef_initialise(&s_emsappl_singleton);
    
       
    // finally ... start the state machine which enters in cfg mode    
    eo_sm_Start(s_emsappl_singleton.sm);
        
    return(&s_emsappl_singleton);
}


extern EOMtheEMSappl* eom_emsappl_GetHandle(void) 
{
    if(NULL != s_emsappl_singleton.sm)
    {
        return(&s_emsappl_singleton);
    }
    else
    {
        return(NULL);
    }
}

extern EOsm* eom_emsappl_GetStateMachine(EOMtheEMSappl *p) 
{
    if(NULL == p)
    {
        return(NULL);
    }

    return(s_emsappl_singleton.sm);
}


extern eOresult_t eom_emsappl_ProcessEvent(EOMtheEMSappl *p, eOsmEventsEMSappl_t ev)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    return(eo_sm_ProcessEvent(s_emsappl_singleton.sm, ev));   
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

__weak extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p)
{

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static void s_eom_emsappl_environment_init(void)
{
   
    // ----------------------------------------------------------------------------------------------------------------
    // 1. initialise eeprom and the arm-environmemnt

    hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    eo_armenv_Initialise(s_emsappl_singleton.cfg.emsappinfo, NULL);
    eov_env_SharedData_Synchronise(eo_armenv_GetHandle());

}


static void s_eom_emsappl_ipnetwork_init(void)
{

    eOmipnet_cfg_addr_t* eomipnet_addr = NULL;
    const eEipnetwork_t *ipnet = NULL;
    extern const ipal_cfg_t    ipal_cfg;
    const eOmipnet_cfg_dtgskt_t eom_emsappl_specialise_dtgskt_cfg = 
    {   
        .numberofsockets            = 1, 
        .maxdatagramenqueuedintx    = 2
    };

    // ----------------------------------------------------------------------------------------------------------------
    // 2. initialise the parameters for ipnet with params taken from the arm-environment (or from ipal-cfg)


    // retrieve the configuration for ipnetwork
#ifndef _FORCE_NETWORK_FROM_IPAL_CFG
    if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
    {
        eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout
    }
    else
#endif
    {   
        ipnet = ipnet;
        eomipnet_addr = NULL;
    }


    // ----------------------------------------------------------------------------------------------------------------
    // 3. start the ipnet

    eom_ipnet_Initialise(&eom_ipnet_DefaultCfg,
                         &ipal_cfg, 
                         eomipnet_addr,
                         &eom_emsappl_specialise_dtgskt_cfg
                         );


}


static void s_eom_emsappl_theemssocket_init(void)
{
    eom_emssocket_Initialise(NULL);
    
    // i try connection now, so that if the hostaddress does not change, then during transmission we dont do a connect anymore
    eom_emssocket_Connect(eom_emssocket_GetHandle(), s_emsappl_singleton.cfg.hostipv4addr);
}

static void s_eom_emsappl_theemstransceiver_init(void)
{
    eOemstransceiver_cfg_t cfg;
    cfg.hostipv4addr = s_emsappl_singleton.cfg.hostipv4addr;
    cfg.hostipv4port = s_emsappl_singleton.cfg.hostipv4port;
    eom_emstransceiver_Initialise(&cfg);
}


static void s_eom_emsappl_theemsconfigurator_init(void)
{
    eom_emsconfigurator_Initialise(NULL);
}


static void s_eom_emsappl_theemserror_init(void)
{
    eom_emserror_Initialise(NULL);
}



// --------------------------------------------------------------------------------------------------------------------
// redefinition of functions of state machine

extern void eo_cfg_sm_EMSappl_hid_on_entry_CFG(EOsm *s)
{
    EOaction onrx;
    eo_action_SetEvent(&onrx, emssocket_evt_packet_received, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    // teh socket alert the cfg task
    eom_emssocket_Open(eom_emssocket_GetHandle(), &onrx);
}

extern void eo_cfg_sm_EMSappl_hid_on_entry_ERR(EOsm *s)
{
    EOaction onrx;
    eo_action_SetEvent(&onrx, emssocket_evt_packet_received, eom_emserror_GetTask(eom_emserror_GetHandle()));
    // the socket alert the error task
    eom_emssocket_Open(eom_emssocket_GetHandle(), &onrx);
}


extern void eo_cfg_sm_EMSappl_hid_on_entry_RUN(EOsm *s)
{
    // the socket does not alert anybody 
    eom_emssocket_Open(eom_emssocket_GetHandle(), NULL);
}
#warning --> quando la sm entra in RUN il skt non avvisa nessun task. il task runRX piglia quello che gli serve. quando si entra in runDO si disabilita il IPnet, lo si riabilita in runTX, si mette il pacchetto nel socket
#warning --> pero' ... 
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





