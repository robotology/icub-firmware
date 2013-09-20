/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Alessandro Scalzo
 * email:   marco.accame@iit.it, alessandro.scalzo@iit.it
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

/* @file       eom-eupdater-main.c
	@brief      This file implements the e-updater using the embobj
	@author     marco.accame@iit.it, alessandro.scalzo@iit.it
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

#if !defined(_MAINTAINER_APPL_)
#include "eupdater-info.h"
#include "eupdater_cangtw.h"
#else
#include "emaintainer-info.h"
#endif

#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"


#include "eupdater_parser.h"

#include "EOtimer.h"


#include "eEsharedServices.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


extern void task_ethcommand(void *p);

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if     defined(_DEBUG_MODE_FULL_)
    #warning --> we are in _DEBUG_MODE_FULL_ and use macro _DEBUG_MODE_IPADDR_FROM_IPAL_CFG_ 
    #define _DEBUG_MODE_IPADDR_FROM_IPAL_CFG_
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


static void s_udpnode_errman_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);

static void s_eom_eupdater_main_init(void);

static void s_ethcommand_startup(EOMtask *p, uint32_t t);
static void s_ethcommand_run(EOMtask *p, uint32_t t);


static eObool_t s_eom_eupdater_main_connected2host(EOpacket *rxpkt, EOsocketDatagram *skt);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// task eth command

static EOsocketDatagram*        s_skt_ethcmd                = NULL;
static EOpacket*                s_rxpkt_ethcmd              = NULL;
static EOpacket*                s_txpkt_ethcmd              = NULL;
static EOMtask*                 s_task_ethcommand           = NULL;
static EOaction*                s_action_ethcmd             = NULL;

static const eOipv4port_t       s_ethcmd_port               = 3333; 
static const eOmessage_t        s_message_from_skt_ethcmd   = 0x00000001;




static const eOerrman_cfg_t  errmancfg = 
{
    .extfn.usr_on_error = s_udpnode_errman_OnError
};

#if !defined(_MAINTAINER_APPL_)
static const eOmsystem_cfg_t* psyscfg = &eupdater_syscfg;
#else
static const eOmsystem_cfg_t* psyscfg = &emaintainer_syscfg;
#endif

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
    eom_sys_Initialise( psyscfg,
                        &memcfg,                           // mempool
                        &errmancfg,                     // errman
                        &eom_timerman_DefaultCfg,
                        &eom_callbackman_DefaultCfg
                      );  

    eom_sys_Start(eom_sys_GetHandle(), s_eom_eupdater_main_init);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


void osal_on_idle(void)
{
    static uint32_t cnt = 0;

    for(;;)
    {
        cnt++;
    }
}




extern void task_ethcommand(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
}  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_udpnode_errman_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
    const char err[4][16] = {"info", "warning", "weak error", "fatal error"};
    char str[128];

    snprintf(str, sizeof(str), "[eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, err[(uint8_t)errtype], info);
    hal_trace_puts(str);

    if(errtype <= eo_errortype_warning)
    {
        return;
    }

    for(;;);
}


static void s_eom_eupdater_main_init(void)
{
    const ipal_cfg_t* ipalcfg = NULL;
    uint8_t *ipaddr = NULL;
    eOmipnet_cfg_addr_t* eomipnet_addr;
#ifndef _DEBUG_MODE_IPADDR_FROM_IPAL_CFG_
    const eEipnetwork_t *ipnet = NULL;
#endif    
    char str[96];

    const eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_MyCfg = 
    { 
#if !defined(_MAINTAINER_APPL_)    
        .numberofsockets            = 2,    // one for eth command and one for can gateway
        .maxdatagramenqueuedintx    = 32    // can gatweay needs to be able to manage many small packets
#else
        .numberofsockets            = 2, 
        .maxdatagramenqueuedintx    = 2
#endif
    };

#if !defined(_MAINTAINER_APPL_)
    ipalcfg = eupdater_ipal_cfg;
#else
    ipalcfg = emaintainer_ipal_cfg;
#endif    
    
    
    ipaddr  = (uint8_t*)&(ipalcfg->eth_ip);
    
    
#if !defined(_MAINTAINER_APPL_)     
    snprintf(str, sizeof(str), "starting EOMeUpdater:: \n\r");
#else
    snprintf(str, sizeof(str), "starting EOMeMaintainer:: \n\r");
#endif  
    hal_trace_puts(str);

    // eeprom is used for shared services but is initted also inside there
    hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    // flash MUST be unlocked if one wants to erase code space (program application, updater, loader)
    hal_flash_unlock();
    
    
#if !defined(_MAINTAINER_APPL_)    
    eo_armenv_Initialise(&eupdater_modinfo, NULL);
#else
    eo_armenv_Initialise(&emaintainer_modinfo, NULL);
#endif    
    eov_env_SharedData_Synchronise(eo_armenv_GetHandle());
    
    
#ifndef _DEBUG_MODE_IPADDR_FROM_IPAL_CFG_
    if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
    {
        eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout

        ipaddr = (uint8_t*)&(eomipnet_addr->ipaddr);
    }
    else
#endif        
    {
        eomipnet_addr = NULL;
        ipaddr = (uint8_t*)&(ipalcfg->eth_ip);
    }


    // start the ipnet
    snprintf(str, sizeof(str), "starting ::ipnet with IP addr: %d.%d.%d.%d\n\r", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    hal_trace_puts(str);    

    eom_ipnet_Initialise(&eom_ipnet_DefaultCfg,
                         ipalcfg, 
                         eomipnet_addr,
                         &eom_ipnet_dtgskt_MyCfg
                         );
    
    // init the leds   
    hal_led_init(hal_led0, NULL);
    hal_led_init(hal_led1, NULL);
    hal_led_init(hal_led2, NULL);
    hal_led_init(hal_led3, NULL);
    hal_led_init(hal_led4, NULL);
    hal_led_init(hal_led5, NULL);   
    
    

    // the eth command task 
    snprintf(str, sizeof(str), "starting ::taskethcommand");
    hal_trace_puts(str);                         

    s_task_ethcommand = eom_task_New(eom_mtask_MessageDriven, 101, 2*1024, s_ethcommand_startup, s_ethcommand_run,  16, 
                                    eok_reltimeINFINITE, NULL, 
                                    task_ethcommand, "ethcommand");
    s_task_ethcommand = s_task_ethcommand;
    
    
#if !defined(_MAINTAINER_APPL_)
    // there is also the can gateway
    snprintf(str, sizeof(str), "starting ::cangateway");
    hal_trace_puts(str);    
    eupdater_cangtw_init();
#endif

    // eval if and when jumping
    snprintf(str, sizeof(str), "calling ::evalwhenjumping");
    hal_trace_puts(str);  
    eupdater_parser_evalwhenjumping(); 
}


static void s_ethcommand_startup(EOMtask *p, uint32_t t)
{
    char str[96];
 
    // init the rx and tx packets 
    s_rxpkt_ethcmd = eo_packet_New(1024);  
    s_txpkt_ethcmd = eo_packet_New(1024);

    // init the action used for various tasks
    s_action_ethcmd = eo_action_New();  

    // initialise the socket 
    s_skt_ethcmd = eo_socketdtg_New(  2, 1024, eom_mutex_New(), // input queue
                                      2, 1024, eom_mutex_New()  // output queue
                                   );   

    snprintf(str, sizeof(str), "opening a txrx socket on port %d for eth messages\n\r", s_ethcmd_port);
    hal_trace_puts(str);


    // set the rx action on socket to be a message s_message_from_skt_ethcmd to this task object
    eo_action_SetMessage(s_action_ethcmd, s_message_from_skt_ethcmd, p);
    eo_socketdtg_Open(s_skt_ethcmd, s_ethcmd_port, eo_sktdir_TXRX, eobool_false, NULL, s_action_ethcmd, NULL);
      
}

static void s_ethcommand_run(EOMtask *p, uint32_t t)
{
    // read the packet.
    eOresult_t res;
    EOsocketDatagram *socket = NULL;
    eObool_t (*parser)(EOpacket*, EOpacket *) = NULL;

    // the message that we have received
    eOmessage_t msg = (eOmessage_t)t;

    switch(msg)
    {
        case s_message_from_skt_ethcmd:
        {
            socket = s_skt_ethcmd;
            parser = eupdater_parser_process_ethcmd;
        } break;

        default:
        {
            socket = NULL;
            parser = NULL;
        } break;

    }

    if((NULL == socket) || (NULL == parser))
    {
        return;
    }


    //hal_led_toggle(hal_led1);

    res = eo_socketdtg_Get(socket, s_rxpkt_ethcmd, eok_reltimeINFINITE);

    if((eores_OK == res) && (eobool_true == s_eom_eupdater_main_connected2host(s_rxpkt_ethcmd, socket)))
    {

        if(eobool_true == parser(s_rxpkt_ethcmd, s_txpkt_ethcmd))
        {
                // transmit a pkt back to the host
                eo_socketdtg_Put(socket, s_txpkt_ethcmd);
        }
 
    }

}


static eObool_t s_eom_eupdater_main_connected2host(EOpacket *rxpkt, EOsocketDatagram *skt)
{
    static eObool_t     host_connected = eobool_false;
    static eOipv4addr_t host_ipaddress = 0;

    eOipv4addr_t remaddr = 0;
    eOipv4port_t remport = 0;

    // print stats of rx packet
    eo_packet_Destination_Get(rxpkt, &remaddr, &remport);
    
    if((eobool_false == host_connected) || (remaddr != host_ipaddress))
    {
        host_ipaddress = remaddr;
        host_connected = eobool_false;
        
        // attempt connection for 1 second. no more. 
        if(eores_OK == eo_socketdtg_Connect(skt, remaddr, eok_reltime1sec))
        {
            host_connected = eobool_true;
        }
        else
        {
            host_connected = eobool_false;
            //printf("not connecetd after %d ms. i shall try again at next reception\n\r", eok_reltime1sec/1000);
        }
    }

    return(host_connected);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



