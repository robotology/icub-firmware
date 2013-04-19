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
#include "hal.h"
#include "EoCommon.h"
#include "EOsm.h"
#include "eOcfg_sm_EMSappl.h"

#include "EOVtheIPnet.h"


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

#include "EOMtheEMSrunner.h"

#include "EOaction_hid.h"

#include "EOMtheEMSapplCfg.h"

#include "EOMtheEMSdiscoverylistener.h"

#include "EOMtheLEDpulser.h"

#ifdef _TEST_SEQNUM_
    #include "eOtheEthLowLevelParser.h"
    #include "eODeb_eoProtoParser.h"
#endif


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
#define emsappl_ledred          hal_led0
#define emsappl_ledgreen        hal_led1 
#define emsappl_ledyellow       hal_led2
#define emsappl_ledorange       hal_led3


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsappl_cfg_t eom_emsappl_DefaultCfg = 
{
    EO_INIT(.emsappinfo)        NULL,
    EO_INIT(.hostipv4addr)      EO_COMMON_IPV4ADDR(10, 0, 1, 200), 
//    EO_INIT(.hostipv4port)      12345
};

uint8_t can_out_queue_full  = 0;
// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_emsappl_environment_init(void);
static void s_eom_emsappl_ipnetwork_init(void);

static void s_eom_emsappl_backdoor_init(void);
static void s_eom_emsappl_thelistener_init(void);
static void s_eom_emsappl_theemssocket_init(void);

static void s_eom_emsappl_theemstransceiver_init(void);

static void s_eom_emsappl_theemserror_init(void);

static void s_eom_emsappl_theemsconfigurator_init(void);

static void s_eom_emsppl_theemsrunner_init(void);

static void s_eom_emsrunner_emsappl_toogleled(osal_timer_t* tmr, void* par);

static void s_eom_emsappl_startBlinkLed(uint32_t counttick);

static void s_eom_emsappl_InitLeds(void);

EO_static_inline eOsmStatesEMSappl_t s_eom_emsappl_GetCurrentState(EOMtheEMSappl *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheEMSappl";

 
static EOMtheEMSappl s_emsappl_singleton = 
{
    EO_INIT(.sm)                NULL,
	EO_INIT(.cfg) 
    {   
        EO_INIT(.hostipv4addr)      EO_COMMON_IPV4ADDR(10, 0, 0, 254), 
//        EO_INIT(.hostipv4port)      12345
    },
    EO_INIT(.timer4led)      NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSappl * eom_emsappl_Initialise(const eOemsappl_cfg_t *emsapplcfg)
{
    char str[96];
    
    if(NULL != s_emsappl_singleton.sm)
    {
        return(&s_emsappl_singleton);
    }
    
    if(NULL == emsapplcfg)
    {
        emsapplcfg = &eom_emsappl_DefaultCfg;
    }

    memcpy(&s_emsappl_singleton.cfg, emsapplcfg, sizeof(eOemsappl_cfg_t));
    
    // tell something
    snprintf(str, sizeof(str)-1, 
             "THE EMS APPLICATION IS %s VER %d.%d BUILT ON: %d/%d/%d at %d:%d", 
             s_emsappl_singleton.cfg.emsappinfo->info.name,
             s_emsappl_singleton.cfg.emsappinfo->info.entity.version.major,
             s_emsappl_singleton.cfg.emsappinfo->info.entity.version.minor,
             s_emsappl_singleton.cfg.emsappinfo->info.entity.builddate.year,
             s_emsappl_singleton.cfg.emsappinfo->info.entity.builddate.month,
             s_emsappl_singleton.cfg.emsappinfo->info.entity.builddate.day,
             s_emsappl_singleton.cfg.emsappinfo->info.entity.builddate.hour,
             s_emsappl_singleton.cfg.emsappinfo->info.entity.builddate.min);  
    eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, str);  

    // do whatever is needed
    
    // 1. init timer 4 blinking led 
    s_eom_emsappl_InitLeds();
#ifdef _TEST_SEQNUM_
    s_eom_emsappl_startBlinkLed(10);
#else
    s_eom_emsappl_startBlinkLed(500);
#endif    

    // 2. create the sm.
    s_emsappl_singleton.sm = eo_sm_New(eo_cfg_sm_EMSappl_Get());
    
    
    // 3. initialise the environment and the ip network.
    s_eom_emsappl_environment_init();
    s_eom_emsappl_ipnetwork_init();

    
    // 5. initialise the EOMtheEMSsocket and the EOMtheEMStransceiver   
    s_eom_emsappl_theemssocket_init();    
    s_eom_emsappl_theemstransceiver_init();
    
    // 4. initialise the listener
    s_eom_emsappl_thelistener_init();
    
    // 4b. initialise the backdoor
    s_eom_emsappl_backdoor_init();    
    
    // 6. initialise the EOMtheEMSerror
    s_eom_emsappl_theemserror_init();    

    // 7. initialise the EOMtheEMSconfigurator
    s_eom_emsappl_theemsconfigurator_init();
    
    // 8. initialise the EOMtheEMSrunner,   
    s_eom_emsppl_theemsrunner_init();

    // call usrdef initialise
    eom_emsappl_hid_userdef_initialise(&s_emsappl_singleton);
    
    // tell things  
    // tells how much ram we have used so far.

    snprintf(str, sizeof(str)-1, "used %d bytes of HEAP out of %d so far", eo_mempool_SizeOfAllocated(eo_mempool_GetHandle()), eom_sys_GetHeapSize(eom_sys_GetHandle()));  
    eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, str);    
 
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

// extern EOsm* eom_emsappl_GetStateMachine(EOMtheEMSappl *p) 
// {
//     if(NULL == p)
//     {
//         return(NULL);
//     }

//     return(s_emsappl_singleton.sm);
// }


extern eOresult_t eom_emsappl_ProcessEvent(EOMtheEMSappl *p, eOsmEventsEMSappl_t ev)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    return(eo_sm_ProcessEvent(s_emsappl_singleton.sm, ev));   
}



extern eOresult_t eom_emsappl_ProcessGo2stateRequest(EOMtheEMSappl *p, eOsmStatesEMSappl_t newstate)
{
    eOsmStatesEMSappl_t     currentstate;
    eOresult_t              res;
    
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    //get current state
    currentstate = s_eom_emsappl_GetCurrentState(p);
    
    if((eo_sm_emsappl_STerr == newstate) || (eo_sm_emsappl_STerr == currentstate))
    {
        return(eores_NOK_unsupported); //currently is not possible go to err with a command or exit from it with a command
    }
    
    if(currentstate == newstate)
    {
        return(eores_OK); 
    }
    
    switch(currentstate)
    {
         case eo_sm_emsappl_STcfg:
        {
            //if i'm here means newstate is eo_emsappl_STrun
            res = eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_go2runner);
        }break;
        
        case eo_sm_emsappl_STrun:
        {
            res = eom_emsrunner_StopAndGoTo(eom_emsrunner_GetHandle(), eo_sm_emsappl_EVgo2cfg); //pay attention: currently is not possible go to err by cmd    
        }break;
        
        // case eo_sm_emsappl_STerr:
        // {
            // res = eores_NOK_unsupported;//currently is inpossible go to any other state!!
        // }break;
        default:
        {
            res = eores_NOK_generic;
        }

    }
    return(res);
}

extern eOresult_t eom_emsappl_GetCurrentState(EOMtheEMSappl *p, eOsmStatesEMSappl_t *currentstate)
{
    if((NULL == p) || (NULL == currentstate))
    {
        return(eores_NOK_nullpointer);
    }    

    *currentstate = s_eom_emsappl_GetCurrentState(p);
    
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

__weak extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p)
{

}

__weak extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p)
{

}

__weak extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p)
{

}

__weak extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p)
{

}

__weak extern void eom_emsappl_hid_userdef_on_exit_ERR(EOMtheEMSappl* p)
{

}

__weak extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p)
{

}

__weak extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p)
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
    
#if 0
    char str[128];
    eOmipnet_cfg_addr_t* eomipnet_addr = NULL;
    uint8_t *ipaddr = NULL;
    const eEipnetwork_t *ipnet = NULL;
    extern const ipal_cfg_t    ipal_cfg;
    const eOmipnet_cfg_dtgskt_t eom_emsappl_specialise_dtgskt_cfg = 
    {   
        .numberofsockets            = 1, 
        .maxdatagramenqueuedintx    = 2         // used to allocate an osal messagequeue to keep tracks of the datagram one wants to tx at a given time. 1 is enough, but 2 does not waste ram
    };

    // ----------------------------------------------------------------------------------------------------------------
    // 2. initialise the parameters for ipnet with params taken from the arm-environment (or from ipal-cfg)


    // retrieve the configuration for ipnetwork
#ifndef _FORCE_NETWORK_FROM_IPAL_CFG
    if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
    {
        eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout
        ipaddr = (uint8_t*)&(eomipnet_addr->ipaddr);
    }
    else
#endif
    {   
        ipnet = ipnet;
        eomipnet_addr = NULL;
        ipaddr = (uint8_t*)&(ipal_cfg.eth_ip);
    }


    // ----------------------------------------------------------------------------------------------------------------
    // 3. start the ipnet

    eom_ipnet_Initialise(&eom_ipnet_DefaultCfg,
                         &ipal_cfg, 
                         eomipnet_addr,
                         &eom_emsappl_specialise_dtgskt_cfg
                         );

    snprintf(str, sizeof(str)-1, "started EOMtheEMSappl::ipnet with IP addr: %d.%d.%d.%d\n\r", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, str);

#else
    
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    
    char str[128];
    eOmipnet_cfg_addr_t* eomipnet_addr = NULL;
    uint8_t *ipaddr = NULL;
    const eEipnetwork_t *ipnet = NULL;


    // ----------------------------------------------------------------------------------------------------------------
    // 2. initialise the parameters for ipnet with params taken from the arm-environment (or from ipal-cfg)



    // retrieve the configuration for ipnetwork
// #ifndef _FORCE_NETWORK_FROM_IPAL_CFG
//     if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
//     {
//         eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout
//         ipaddr = (uint8_t*)&(eomipnet_addr->ipaddr);
//     }
//     else
// #endif
//     {   
//         ipnet = ipnet;
//         eomipnet_addr = NULL;
//         ipaddr = (uint8_t*)&(emscfg->wipnetcfg.ipalcfg->eth_ip);
//     }

    if((eobool_true == emscfg->getipaddrFROMenvironment) && (eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet)))
    {
        eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout
        ipaddr = (uint8_t*)&(eomipnet_addr->ipaddr);
    }
    else
    {   
        ipnet = ipnet;
        eomipnet_addr = NULL;
        ipaddr = (uint8_t*)&(emscfg->wipnetcfg.ipalcfg->eth_ip);
    }
    
    // ----------------------------------------------------------------------------------------------------------------
    // 3. start the ipnet

    eom_ipnet_Initialise(&emscfg->wipnetcfg.ipnetcfg,  //&eom_ipnet_DefaultCfg,
                         emscfg->wipnetcfg.ipalcfg, //&ipal_cfg, 
                         eomipnet_addr,
                         &emscfg->wipnetcfg.dtgskcfg
                         );

    snprintf(str, sizeof(str)-1, "started EOMtheEMSappl::ipnet with IP addr: %d.%d.%d.%d\n\r", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, str);    
    
    
#endif
}


static void s_eom_emsappl_thelistener_init(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    
    eom_emsdiscoverylistener_Initialise(&emscfg->disclistcfg);
}

static void s_eom_emsappl_backdoor_init(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    
    eom_emsbackdoor_Initialise(&emscfg->backdoorcfg);
}


static void s_eom_emsappl_theemssocket_init(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    
    eom_emssocket_Initialise(&emscfg->socketcfg);
    
    // i try connection now, so that if the hostaddress does not change, then during transmission we dont do a connect anymore
    eom_emssocket_Connect(eom_emssocket_GetHandle(), s_emsappl_singleton.cfg.hostipv4addr);
}

static void s_eom_emsappl_theemstransceiver_init(void)
{

    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    #if 1
    eom_emstransceiver_Initialise(&emscfg->transcfg);
    #else
    #warning ------> se si vuole rimuovere questa robaccia ... si deve mettere extern qualcosa nei file ebx.c
    eOemstransceiver_cfg_t transcfg;
    memcpy(&transcfg, &emscfg->transcfg, sizeof(eOemstransceiver_cfg_t));
    eOnvscfg_setof_EPs_t * eps = eom_emsapplcfg_GetEPs(emscfg); 
    transcfg.vectorof_endpoint_cfg = eps->vectorof_endpoint_cfg;
    transcfg.hashfunction_ep2index = eps->hashfunction_ep2index;
    eom_emstransceiver_Initialise(&transcfg);
    #endif
}


static void s_eom_emsappl_theemsconfigurator_init(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    eom_emsconfigurator_Initialise(&emscfg->cfgobjcfg);
}


static void s_eom_emsappl_theemserror_init(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    eom_emserror_Initialise(&emscfg->errobjcfg);
}

static void s_eom_emsppl_theemsrunner_init(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    eom_emsrunner_Initialise(&emscfg->runobjcfg);
}


static void s_eom_emsappl_InitLeds(void)
{
    hal_led_cfg_t cfg = {.dummy=0};
    
    hal_led_init(emsappl_ledred, &cfg);
    hal_led_off(emsappl_ledred);
    hal_led_init(emsappl_ledgreen, &cfg); //led green
    hal_led_off(emsappl_ledgreen);
    hal_led_init(emsappl_ledyellow, &cfg);
    hal_led_off(emsappl_ledyellow);
    hal_led_init(emsappl_ledorange, &cfg);
    hal_led_off(emsappl_ledorange); 

    eOmledpulser_cfg_t ledpulsercfg = 
    {
        .numberofleds   = 3,
        .leds           = { eom_ledpulser_led_zero, eom_ledpulser_led_one, eom_ledpulser_led_two }
    };

    eom_ledpulser_Initialise(&ledpulsercfg);

    //eom_ledpulser_Start(eom_ledpulser_GetHandle(), eom_ledpulser_led_zero, 400*1000, 10);
}

static void s_eom_emsappl_startBlinkLed(uint32_t counttick)
{
    osal_timer_timing_t timing;
    osal_timer_onexpiry_t onexpiry;
    
    timing.startat  = OSAL_abstimeNONE;
    timing.count    = counttick * osal_info_get_tick(); 
    timing.mode     = osal_tmrmodeFOREVER; 

    onexpiry.cbk    = s_eom_emsrunner_emsappl_toogleled;
    onexpiry.par    = &s_emsappl_singleton;        


    if(NULL == s_emsappl_singleton.timer4led)
    {
        s_emsappl_singleton.timer4led = osal_timer_new(); 
    }
    osal_timer_start(s_emsappl_singleton.timer4led, &timing, &onexpiry, osal_callerTSK);
}

static void s_eom_emsrunner_emsappl_toogleled(osal_timer_t* tmr, void* par)
{
    hal_led_toggle(emsappl_ledorange);
}

EO_static_inline eOsmStatesEMSappl_t s_eom_emsappl_GetCurrentState(EOMtheEMSappl *p)
{
    return((eOsmStatesEMSappl_t)eo_sm_GetActiveState(p->sm));
}


#ifdef _TEST_SEQNUM_
extern void my_cbk_onErrorSeqNum(eOethLowLevParser_packetInfo_t *pktInfo_ptr, uint32_t rec_seqNum, uint32_t expected_seqNum);
extern void my_cbk_onNVfound(eOethLowLevParser_packetInfo_t *pktInfo_ptr, eODeb_eoProtoParser_ropAdditionalInfo_t *ropAddInfo_ptr);
//this variable is a function ptr.if is is not null, the isr invoked on rx pkt, will call the function ponted by this variable
extern void (*hal_eth_lowLevelUsePacket_ptr)(uint8_t* pkt_ptr, uint32_t size);
extern void EthLowLevParser_callbackInHAL(uint8_t *packet, uint32_t size);
static void s_eom_emsappl_ethLowLevelParser_configure(void)
{
    
    hal_eth_lowLevelUsePacket_ptr = EthLowLevParser_callbackInHAL;
//     //4.1) init application parser: embObjParser
//     const eODeb_eoProtoParser_cfg_t  deb_eoParserCfg = 
//     {
//         EO_INIT(.checks)
//         {
//             EO_INIT(.seqNum)
//             {
//                 EO_INIT(.cbk_onErrSeqNum)           my_cbk_onErrorSeqNum,
//             },
//             
//             EO_INIT(.nv)                            {0},
//             
//             EO_INIT(.invalidRopFrame)               {0}
//         }
//     };
    
    
        //4.1) init application parser: embObjParser 
    const eODeb_eoProtoParser_cfg_t  deb_eoParserCfg =  
    { 
        EO_INIT(.checks) 
        { 
            EO_INIT(.seqNum) 
            { 
                EO_INIT(.cbk_onErrSeqNum)           my_cbk_onErrorSeqNum, 
            }, 
             
            EO_INIT(.nv) 
            { 
                EO_INIT(.NVs2searchArray) 
                { 
                    EO_INIT(.head) 
                    { 
                        EO_INIT(.capacity)       eODeb_eoProtoParser_maxNV2find, 
                        EO_INIT(.itemsize)       sizeof(eODeb_eoProtoParser_nvidEp_couple_t), 
                        EO_INIT(.size)           1, 
                    }, 
                    EO_INIT(.data) 
                    { 
                        {0x14, 0x9c00} 
                    } 
                 
                }, 
                EO_INIT(.cbk_onNVfound)            my_cbk_onNVfound 
            }, 
             
            EO_INIT(.invalidRopFrame)               {0} 
        } 
    };

    
    eODeb_eoProtoParser_Initialise(&deb_eoParserCfg);
    
    
    //4.2) init low level parser: eOethLowLevParser
/*    const eOethLowLevParser_cfg_t  ethLowLevParserCfg = 
    {
        EO_INIT(.conFiltersData) 
        {
            EO_INIT(.filtersEnable)     0,
            EO_INIT(.filters)           {0}, //use pcap filter
        },
        
        EO_INIT(.appParserData)
        {
            EO_INIT(.func)             eODeb_eoProtoParser_RopFrameDissect
            EO_INIT(.arg)              eODeb_eoProtoParser_GetHandle(),
        }
    };
*/
    //currently use thelow level parser and appl paser separately
    const eOethLowLevParser_cfg_t  ethLowLevParserCfg = {0}; 
    eo_ethLowLevParser_Initialise(&ethLowLevParserCfg);

}
#endif
// --------------------------------------------------------------------------------------------------------------------
// redefinition of functions of state machine

extern void eo_cfg_sm_EMSappl_hid_on_entry_CFG(EOsm *s)
{
    
    EOaction onrx;
#ifdef _TEST_SEQNUM_
    osal_timer_stop(s_emsappl_singleton.timer4led, osal_callerTSK);
    s_eom_emsappl_startBlinkLed(1000); 
    
    s_eom_emsappl_ethLowLevelParser_configure();
    
#else    
    hal_led_on(emsappl_ledgreen);
#endif    
    eo_action_SetEvent(&onrx, emssocket_evt_packet_received, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    // the socket alerts the cfg task
    eom_emssocket_Open(eom_emssocket_GetHandle(), &onrx, NULL);
    
    eom_emsappl_hid_userdef_on_entry_CFG(&s_emsappl_singleton);
}


extern void eo_cfg_sm_EMSappl_hid_on_exit_CFG(EOsm *s)
{
    hal_led_off(emsappl_ledgreen);
    
    eom_emsappl_hid_userdef_on_exit_CFG(&s_emsappl_singleton);
}
#ifdef _TEST_SEQNUM_
static void s_eom_emsrunner_emsappl_toogleallled(osal_timer_t* tmr, void* par)
{

    hal_led_toggle(emsappl_ledgreen);
    hal_led_toggle(emsappl_ledyellow);
    
    if(can_out_queue_full)
    {
        hal_led_toggle(emsappl_ledred);
    }
}
#endif

extern void eo_cfg_sm_EMSappl_hid_on_entry_ERR(EOsm *s)
{
    EOaction onrx;
#ifdef   _TEST_SEQNUM_ 

    osal_timer_timing_t timing;
    osal_timer_onexpiry_t onexpiry;
    
    timing.startat  = OSAL_abstimeNONE;
    timing.count    = 1000 * osal_info_get_tick(); 
    timing.mode     = osal_tmrmodeFOREVER; 

    onexpiry.cbk    = s_eom_emsrunner_emsappl_toogleallled;
    onexpiry.par    = &s_emsappl_singleton;        


    if(NULL == s_emsappl_singleton.timer4led)
    {
        s_emsappl_singleton.timer4led = osal_timer_new(); 
    }
    osal_timer_start(s_emsappl_singleton.timer4led, &timing, &onexpiry, osal_callerTSK);

#else
        hal_led_on(emsappl_ledred);

#endif    
    eo_action_SetEvent(&onrx, emssocket_evt_packet_received, eom_emserror_GetTask(eom_emserror_GetHandle()));
    // the socket alerts the error task
    eom_emssocket_Open(eom_emssocket_GetHandle(), &onrx, NULL);
    
    eom_emsappl_hid_userdef_on_entry_ERR(&s_emsappl_singleton);
    
}


extern void eo_cfg_sm_EMSappl_hid_on_exit_ERR(EOsm *s)
{
    hal_led_off(emsappl_ledyellow);
    
    eom_emsappl_hid_userdef_on_exit_ERR(&s_emsappl_singleton);

 }

extern void eo_cfg_sm_EMSappl_hid_on_entry_RUN(EOsm *s)
{
    EOaction ontxdone;
    //eo_action_Clear(&ontxdone);

#ifdef _TEST_SEQNUM_
    osal_timer_stop(s_emsappl_singleton.timer4led, osal_callerTSK);
    s_eom_emsappl_startBlinkLed(100); 
#else    
        hal_led_on(emsappl_ledyellow);
#endif     

    eo_action_SetCallback(&ontxdone, (eOcallback_t)eom_emsrunner_OnUDPpacketTransmitted, eom_emsrunner_GetHandle(), NULL);
    // the socket does not alert anybody when it receives a pkt, but can alert the sending task
    eom_emssocket_Open(eom_emssocket_GetHandle(), NULL, &ontxdone);
    
    // we activate the runner
    eom_emsrunner_Start(eom_emsrunner_GetHandle());
    
    //eov_ipnet_Deactivate(eov_ipnet_GetHandle());
    
    eom_emsappl_hid_userdef_on_entry_RUN(&s_emsappl_singleton);
}


extern void eo_cfg_sm_EMSappl_hid_on_exit_RUN(EOsm *s)
{
#ifndef _TEST_SEQNUM_    
    hal_led_off(emsappl_ledyellow);
    //eov_ipnet_Activate(eov_ipnet_GetHandle());
#endif    
    eom_emsappl_hid_userdef_on_exit_RUN(&s_emsappl_singleton);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





