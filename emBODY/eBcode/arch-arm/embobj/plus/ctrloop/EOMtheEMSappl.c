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
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOMtheIPnet.h"


#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"

#include "EOMtheEMSsocket.h"

#include "EOMtheEMStransceiver.h"

#include "EOMtheEMSconfigurator.h"

#include "EOMtheEMSerror.h"

#include "EOMtheEMSrunner.h"

#include "EOaction.h"

#include "EOMtheEMSapplCfg.h"

#include "EOMtheEMSdiscoverylistener.h"

#include "EOtheInfoDispatcher.h"

#include "EoProtocol.h"
#include "EoProtocolMN.h"

#include "EOMtheEMStransceiver.h"

#include "EOtheLEDpulser.h"


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


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsappl_cfg_t eom_emsappl_DefaultCfg = 
{
    EO_INIT(.emsappinfo)        NULL,
    EO_INIT(.hostipv4addr)      EO_COMMON_IPV4ADDR(10, 0, 1, 200)
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

extern void initDiagnostic();

static void s_eom_emsappl_environment_init(void);
static void s_eom_emsappl_ipnetwork_init(void);

static void s_eom_emsappl_backdoor_init(void);
static void s_eom_emsappl_thelistener_init(void);
static void s_eom_emsappl_theemssocket_init(void);
static void s_eom_emsappl_theemssocket_defaultopen(void);

static void s_eom_emsappl_theemstransceiver_init(void);

//static void s_eom_emsappl_theinfodispatcher_init(void);

static void s_eom_emsappl_theemserror_init(void);

static void s_eom_emsappl_theemsconfigurator_init(void);

static void s_eom_emsppl_theemsrunner_init(void);


static void s_eom_emsappl_errormamager_customise(void);

static void s_eom_emsappl_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);


EO_static_inline eOsmStatesEMSappl_t s_eom_emsappl_GetCurrentState(EOMtheEMSappl *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheEMSappl";

 
static EOMtheEMSappl s_emsappl_singleton = 
{
    EO_INIT(.sm)            NULL,
    EO_INIT(.cfg)
    { 
        EO_INIT(.emsappinfo)        NULL,
        EO_INIT(.hostipv4addr)      EO_COMMON_IPV4ADDR(10, 0, 1, 254)
    },
    EO_INIT(.initted)               0,
    EO_INIT(.blockingsemaphore)     NULL,
    EO_INIT(.onerrormutex)          NULL    
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
    
    char str[64];
    snprintf(str, sizeof(str), "inside _Initialise()");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);  
      
    
    // i put in here this instruction because from here onward the initialisation cannot fails and ... in this way
    // any function here inside which calls eom_emsappl_GetHandle() shall have a valid handle.
    s_emsappl_singleton.initted = 1;
    
    if(NULL == emsapplcfg)
    {
        emsapplcfg = &eom_emsappl_DefaultCfg;
    }

    memcpy(&s_emsappl_singleton.cfg, emsapplcfg, sizeof(eOemsappl_cfg_t));    

    // do whatever is needed
    
    // 2. create the sm.
    s_emsappl_singleton.sm = eo_sm_New(eo_cfg_sm_EMSappl_Get());
    
    
    // 3. initialise the environment and the ip network.
    s_eom_emsappl_environment_init();
    s_eom_emsappl_ipnetwork_init();

    // 3b. initialise the backdoor
    s_eom_emsappl_backdoor_init();  
    
    // 4. initialise the EOMtheEMSsocket and the EOMtheEMStransceiver   
    s_eom_emsappl_theemssocket_init();    
    s_eom_emsappl_theemstransceiver_init();
    
    // 5. initialise the listener
    s_eom_emsappl_thelistener_init();
    
    
//    // 5bis. initialise the EOtheInfoDispatcher    
//    s_eom_emsappl_theinfodispatcher_init();
    
    // 6. initialise the EOMtheEMSerror
    s_eom_emsappl_theemserror_init();    

    // 7. initialise the EOMtheEMSconfigurator
    s_eom_emsappl_theemsconfigurator_init();
    
    // 8. initialise the EOMtheEMSrunner,   
    s_eom_emsppl_theemsrunner_init();
		
		initDiagnostic();
    
        
    // redefine the error manager so that it dispatches errors to the socket ...
    s_eom_emsappl_errormamager_customise(); 

    // also, we need to open the socket, so that communication is active.
    s_eom_emsappl_theemssocket_defaultopen();
    
    snprintf(str, sizeof(str), "_Initialise() calls eom_emsappl_hid_userdef_initialise()");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);  
    
    // call usrdef initialise. it is the place where to start new services in init task 
    eom_emsappl_hid_userdef_initialise(&s_emsappl_singleton);
    
    snprintf(str, sizeof(str), "_Initialise() has finished eom_emsappl_hid_userdef_initialise()");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);  
    
    // tell things  
    // tells how much ram we have used so far.

    
//    //snprintf(str, sizeof(str), "used %d bytes of HEAP out of %d so far", eo_mempool_SizeOfAllocated(eo_mempool_GetHandle()), eom_sys_GetHeapSize(eom_sys_GetHandle()));  
//    //eo_errman_Info(eo_errman_GetHandle(), str, s_eobj_ownname, &eo_errman_DescrRunningHappily);    
//    eOerrmanDescriptor_t desc = {0};
//    memcpy(&desc, &eo_errman_DescrRunningHappily, sizeof(desc));
//    desc.param = 1;
//    desc.par64 = 0;
//    eo_errman_Info(eo_errman_GetHandle(), NULL, s_eobj_ownname, &desc); 
    
  
    // finally ... start the state machine which enters in cfg mode    
    eo_sm_Start(s_emsappl_singleton.sm);
        

    snprintf(str, sizeof(str), "_Initialise() has started its state machine w/ CFG, RUN, ERR states. Now it quits");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);  
    
    return(&s_emsappl_singleton);
}


extern EOMtheEMSappl* eom_emsappl_GetHandle(void) 
{
    if(s_emsappl_singleton.initted)
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


extern eOresult_t eom_emsappl_SM_ProcessEvent(EOMtheEMSappl *p, eOsmEventsEMSappl_t ev)
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

    // get current state
    currentstate = s_eom_emsappl_GetCurrentState(p);
    
    if(currentstate == newstate)
    {   // we are already in the requested state
        return(eores_OK); 
    }    
    
//    if((eo_sm_emsappl_STerr == newstate) || (eo_sm_emsappl_STerr == currentstate))
//    {   //currently is not possible go to err with a command or exit from it with a command
//        return(eores_NOK_unsupported); 
//    }
    
    
    if((eo_sm_emsappl_STerr == currentstate))
    {   // currently it is not possible to exit from error
        return(eores_NOK_unsupported); 
    }    
    
    eOsmEventsEMSappl_t emsevt = eo_sm_emsappl_EVdummy;
    switch(newstate)
    {
        case eo_sm_emsappl_STcfg:   emsevt = eo_sm_emsappl_EVgo2cfg;            break;
        case eo_sm_emsappl_STrun:   emsevt = eo_sm_emsappl_EVgo2run;            break;
        case eo_sm_emsappl_STerr:   emsevt = eo_sm_emsappl_EVgo2err;            break;
        default:                    emsevt = eo_sm_emsappl_EVdummy;             break;
    }
        
    switch(currentstate)
    {
        case eo_sm_emsappl_STcfg:
        {   //if i'm here newstate can be only eo_emsappl_STrun (or err)
            //eOevent_t evt = emsconfigurator_evt_go2runner;
            //res = eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), evt);
            res = eom_emsconfigurator_GracefulStopAndGoTo(eom_emsconfigurator_GetHandle(), emsevt);
        } break;
        
        case eo_sm_emsappl_STrun:
        {   // if i am here newstate can be only eo_sm_emsappl_STcfg (or err). i dont send an event, but i call a function which will smmotly go back to cfg
            res = eom_emsrunner_GracefulStopAndGoTo(eom_emsrunner_GetHandle(), emsevt);     
        } break;
        
        // case eo_sm_emsappl_STerr:
        // {
            // res = eores_NOK_unsupported;//currently is inpossible go to any other state!!
        // }break;
        default:
        {
            res = eores_NOK_generic;
        } break;

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


extern eOresult_t eom_emsappl_SendTXRequest(EOMtheEMSappl *p)
{
    eOsmStatesEMSappl_t     currentstate;
    eOresult_t              res;
        
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    // get current state
    currentstate = s_eom_emsappl_GetCurrentState(p);
    
        
    switch(currentstate)
    {
        case eo_sm_emsappl_STcfg:
        {   // the configurator state transmits only on request
            res = eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_ropframeTx);
        } break;
        
        case eo_sm_emsappl_STrun:
        {   // the runner always transmits at the end of teh cycle, thus no request is required
            res = eores_OK; 
        } break;

        case eo_sm_emsappl_STerr:
        {
            // the error state transmits only on request
            res = eom_task_SetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emserror_evt_ropframeTx);
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;

    }
    return(res);
}


extern eOresult_t eom_emsappl_Transmit_OccasionalROP(EOMtheEMSappl *p, eOropdescriptor_t *ropdes)
{
    if((NULL == p) || (NULL == ropdes))
    {
        return(eores_NOK_nullpointer);
    } 

    eOresult_t res = eores_NOK_generic;   

    res = eo_transceiver_OccasionalROP_Load(eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()), ropdes); 
    
    eom_emsappl_SendTXRequest(p);
    
    return(res);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

EO_weak extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p)
{
    p = p;
}

EO_weak extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p)
{

}

EO_weak extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p)
{

}

EO_weak extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p)
{

}

EO_weak extern void eom_emsappl_hid_userdef_on_exit_ERR(EOMtheEMSappl* p)
{

}

EO_weak extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p)
{

}

EO_weak extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p)
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
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();    
    eOmipnet_cfg_addr_t* eomipnet_addr = NULL;
    const eEipnetwork_t *ipnet = NULL;


    // ----------------------------------------------------------------------------------------------------------------
    // 1. initialise the parameters for ipnet with params taken from the arm-environment (or from ipal-cfg)

    if((eobool_true == emscfg->getipaddrFROMenvironment) && (eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet)))
    {
        eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout
        //ipaddr = (uint8_t*)&(eomipnet_addr->ipaddr);
    }
    else
    {   
        ipnet = ipnet;
        eomipnet_addr = NULL;
        //ipaddr = (uint8_t*)&(emscfg->wipnetcfg.ipalcfg->eth_ip);
    }
    
    // ----------------------------------------------------------------------------------------------------------------
    // 2. start the ipnet

    eom_ipnet_Initialise(&emscfg->wipnetcfg.ipnetcfg,  //&eom_ipnet_DefaultCfg,
                         emscfg->wipnetcfg.ipalcfg, //&ipal_cfg, 
                         eomipnet_addr,
                         &emscfg->wipnetcfg.dtgskcfg
                         );

    
    



    // 3. attempt to resolve host
    
    char str[128];    
    char ipv4str[20];
    eo_common_ipv4addr_to_string(emscfg->applcfg.hostipv4addr, ipv4str, sizeof(ipv4str));    
    snprintf(str, sizeof(str), "about to call eom_ipnet_ResolveIP() to host %s w/ timeout 5sec", ipv4str);
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);   
      
    // set the led reserved for link
    eOresult_t res = eom_ipnet_ResolveIP(eom_ipnet_GetHandle(), emscfg->applcfg.hostipv4addr, 5*EOK_reltime1sec);
    
    if(eores_OK == res)
    {
        // set led0 to ON
       eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    }
    else
    {
        eo_ledpulser_Off(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    }
    
    snprintf(str, sizeof(str), "eom_ipnet_ResolveIP() is over. Link w/ host %s is %s", ipv4str, (eores_OK == res) ? "ON" : "OFF");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);      
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

    char str[128];    
    char ipv4str[20];    
    eo_common_ipv4addr_to_string(emscfg->applcfg.hostipv4addr, ipv4str, sizeof(ipv4str));  

    snprintf(str, sizeof(str), "about to call eom_emssocket_Connect() to host %s w/ timeout 500ms", ipv4str);
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);   
    
    // i try connection now, so that if the hostaddress does not change, then during transmission we dont do a connect anymore
    eOresult_t res = eom_emssocket_Connect(eom_emssocket_GetHandle(), s_emsappl_singleton.cfg.hostipv4addr, 5*EOK_reltime100ms);

    
    if(eores_OK == res)
    {
        // set led0 to ON
       eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    }
    else
    {
        eo_ledpulser_Off(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    }
    
    snprintf(str, sizeof(str), "eom_emssocket_Connect is over. Link w/ host %s is %s", ipv4str, (eores_OK == res) ? "ON" : "OFF");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);   
}


static void s_eom_emsappl_theemssocket_defaultopen(void)
{   
    // we also open the socket, so that we can tx or rx straight away. for now we direct towards the configurator task
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetEvent(onrx, emssocket_evt_packet_received, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    // the socket alerts the cfg task for any newly received packet
    eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL);
}

static void s_eom_emsappl_theemstransceiver_init(void)
{

    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    eom_emstransceiver_Initialise(&emscfg->transcfg);
}


static void s_eom_emsappl_theemsconfigurator_init(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    eom_emsconfigurator_Initialise(&emscfg->cfgobjcfg);
}

//static void s_eom_emsappl_theinfodispatcher_init(void)
//{
//    //EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
//    eOinfodispatcher_cfg_t config = {0};
//    config.capacity     = 8; // or eo_sizecntnr_dynamic ....
//    config.transmitter  = eo_transceiver_GetTransmitter(eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()));
//    eo_infodispatcher_Initialise(&config);    
//}

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



static void s_eom_emsappl_errormamager_customise(void)
{
    s_emsappl_singleton.blockingsemaphore = osal_semaphore_new(2, 0);
    s_emsappl_singleton.onerrormutex = osal_mutex_new();
    eo_errman_SetOnErrorHandler(eo_errman_GetHandle(), s_eom_emsappl_OnError);    
}

EO_static_inline eOsmStatesEMSappl_t s_eom_emsappl_GetCurrentState(EOMtheEMSappl *p)
{
    return((eOsmStatesEMSappl_t)eo_sm_GetActiveState(p->sm));
}


static void s_eom_emsappl_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    // i want to protect this function vs concurrent access. for instance it may be called by a timer callback and by teh control loop.
    // i use a mutex but only where it is required. for sure snprintf is reentrant and it uses memory on the stack ....
    const char empty[] = "EO?";
    const char *err = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
    const char *eobjstr = (NULL == caller) ? (empty) : ((NULL == caller->eobjstr) ? (empty) : (caller->eobjstr));
    const uint32_t taskid = (NULL == caller) ? (0) : (caller->taskid);

    EOMtheEMSapplCfg *emsapplcfg = eom_emsapplcfg_GetHandle();

    if(emsapplcfg->errmng_haltrace_enabled)
    {        
        char text[256];
        
        uint64_t tt = eov_sys_LifeTimeGet(eov_sys_GetHandle());
        uint32_t sec = tt/(1000*1000);
        uint32_t tmp = tt%(1000*1000);
        uint32_t msec = tmp / 1000;
        uint32_t usec = tmp % 1000;
        
        if(eo_errortype_trace == errtype)
        {   // it is a trace
            
            if(NULL != info)
            {
                snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> %s.", eobjstr, sec, msec, usec, info); 
            }
            else
            {
                snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> ...", eobjstr, sec, msec, usec); 
            }
            hal_trace_puts(text);
            return;            
        }
        
        text[0] = 0; // i put a '0' terminator
        
        if((NULL == des) && (NULL == info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @s%dm%du%d)-> ...", err, eobjstr, taskid, sec, msec, usec);            
        }
        else if((NULL != des) && (NULL != info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @s%dm%du%d)-> {0x%x p16 0x%04x, p64 0x%016llx, dev %d, adr %d}: %s. INFO = %s", err, eobjstr, taskid, sec, msec, usec, des->code, des->par16, des->par64, des->sourcedevice, des->sourceaddress, eoerror_code2string(des->code), info);  
        }
        else if((NULL != des) && (NULL == info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @s%dm%du%d)-> {0x%x, p16 0x%04x, p64 0x%016llx, dev %d, adr %d}: %s.", err, eobjstr, taskid, sec, msec, usec, des->code, des->par16, des->par64, des->sourcedevice, des->sourceaddress, eoerror_code2string(des->code));               
        }
        else if((NULL == des) && (NULL != info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @s%dm%du%d)-> {}: ... INFO = %s", err, eobjstr, taskid, sec, msec, usec, info);  
        }
         
        
        // i dont care is trace is interrupted ... thus NO MUTEX in here
        hal_trace_puts(text);
    }
    
    // now i use the info-dispatcher
    
    if(eo_errortype_trace == errtype)
    {   // we dont transmit the trace
        return;
    }
    
    // from eOerrmanErrorType_t to eOmn_info_type_t ...
    uint8_t eee = errtype;
    eee --;
    eOmn_info_type_t type = (eOmn_info_type_t)eee;
    
    eOmn_info_source_t source = (NULL != des) ? ((eOmn_info_source_t)des->sourcedevice) : (eomn_info_source_board);
    uint8_t  address = (NULL != des) ? (des->sourceaddress) : (0);
    eOmn_info_extraformat_t extraformat = (NULL == info) ? (eomn_info_extraformat_none) : (eomn_info_extraformat_verbal);
    
    
    eOmn_info_properties_t props = {0};
    props.code          = (NULL != des) ? (des->code) : (0);
    props.par16         = (NULL != des) ? (des->par16) : (0);
    props.par64         = (NULL != des) ? (des->par64) : (0);

    EOMN_INFO_PROPERTIES_FLAGS_set_type(props.flags, type);
    EOMN_INFO_PROPERTIES_FLAGS_set_source(props.flags, source);
    EOMN_INFO_PROPERTIES_FLAGS_set_address(props.flags, address);
    EOMN_INFO_PROPERTIES_FLAGS_set_extraformat(props.flags, extraformat);
    EOMN_INFO_PROPERTIES_FLAGS_set_futureuse(props.flags, 0);
    
    // well, i DONT want these functions to be interrupted
    osal_mutex_take(s_emsappl_singleton.onerrormutex, osal_reltimeINFINITE);
    
    // the call eo_infodispatcher_Put() is only in here, thus we can either protect with a mutex in here or put put a mutex inside. WE USE MUTEX IN HERE
    eo_infodispatcher_Put(eo_infodispatcher_GetHandle(), &props, info); 
    // the call eom_emsappl_SendTXRequest() either does nothing and is reentrant (when in run mode) or sends an event to a task with osal_eventflag_set() which is reentrant.
    // it is called also by the can protocol parser in case of proxy, which however is used in run mode (thus reentrant).
    // i protect both functions in here
    eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());
    
    osal_mutex_release(s_emsappl_singleton.onerrormutex);
    
    //eom_emserror_OnError_userdefined_call(errtype, info, caller, des);
    
    if(errtype <= eo_errortype_error)
    {
        return;
    }
    

    
    //#warning --> marco.accame: in case of fatal error, shall we: (1) go smoothly to error state, (2) force immediate transition to error state?
    
    // if in here tehre is a serious error. but we dont care about concurrency
    //osal_mutex_take(s_emsappl_singleton.onerrormutex, osal_reltimeINFINITE);    
    
    // i am going to error state, thus i set the correct state in eOmn_appl_status_t variable, which is used by robotInterface
    // to understand the status of the ems: cfg, run, err.
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = (eOmn_appl_status_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    status->currstate = applstate_error;
    
//#if 1        
    // this call forces immediate transition to error state. then we stop the calling task, so that 
    // it does not do anything damaging anymore.
    // it seems a good approach because we should immediately stop upon fatal error. 

    eom_emserror_SetFatalError(eom_emserror_GetHandle(), des);
    eom_emsappl_SM_ProcessEvent(eom_emsappl_GetHandle(), eo_sm_emsappl_EVgo2err);  
    eom_task_SetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emserror_evt_fatalerror);    
    

    // if the caller is not the error state, then we block it execution.
    // however, we cannot block the execution of the error state otherwise ... we lose operativity
    // of error task and amongst others we lose communication with the remote host.
    // when we are in error state the remote host must be able to know it.
 
    eOsmStatesEMSappl_t state = eo_sm_emsappl_STcfg;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &state);
    
    if(eo_sm_emsappl_STerr != state)
    {
        // this call makes the calling task wait in here forever.
        osal_semaphore_decrement(s_emsappl_singleton.blockingsemaphore, OSAL_reltimeINFINITE);
        // the forever loop should not be necessary.
        for(;;);
    }
    
//#else
//    // in this situation, the transition to error state is done by the active task, thus we require that the execution
//    // continues. the good thing is that we exit in a clean way from the state, but the high risk is to execute dangerous instructions.
//    // example: some object inside the runner detects that some pointer that is going to be used is NULL. if we keep on running this
//    // code, then teh NULL pointer is deferenced and teh application crashes....    
//    eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STerr);
//    return;
//#endif    
}



// --------------------------------------------------------------------------------------------------------------------
// redefinition of functions of state machine

extern void eo_cfg_sm_EMSappl_hid_on_entry_CFG(EOsm *s)
{
    // set the correct state 
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = (eOmn_appl_status_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    status->currstate = applstate_config;      
    
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetEvent(onrx, emssocket_evt_packet_received, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    // the socket alerts the cfg task for any newly received packet
    eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL);
    
    // if any rx packets already in socket then alert the cfg task
    if(0 != eom_emssocket_NumberOfReceivedPackets(eom_emssocket_GetHandle()))
    {
        eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emssocket_evt_packet_received);        
    }
     
    // exec any user-defined activity
    eom_emsappl_hid_userdef_on_entry_CFG(&s_emsappl_singleton);
}


extern void eo_cfg_sm_EMSappl_hid_on_exit_CFG(EOsm *s)
{ 
    eom_emsappl_hid_userdef_on_exit_CFG(&s_emsappl_singleton);
}



extern void eo_cfg_sm_EMSappl_hid_on_entry_ERR(EOsm *s)
{    
    // set the correct state 
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = (eOmn_appl_status_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    status->currstate = applstate_error;    
   
    // redirect the socket
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetEvent(onrx, emssocket_evt_packet_received, eom_emserror_GetTask(eom_emserror_GetHandle()));
    // the socket alerts the error task for any newly received packets
    eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL);
    
    // if any rx packets already in socket then alert the err task
    if(0 != eom_emssocket_NumberOfReceivedPackets(eom_emssocket_GetHandle()))
    {
        eom_task_SetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emssocket_evt_packet_received);        
    }
    
    // exec any user-defined activity
    eom_emsappl_hid_userdef_on_entry_ERR(&s_emsappl_singleton);   
}


extern void eo_cfg_sm_EMSappl_hid_on_exit_ERR(EOsm *s)
{   
    eom_emsappl_hid_userdef_on_exit_ERR(&s_emsappl_singleton);
}


extern void eo_cfg_sm_EMSappl_hid_on_entry_RUN(EOsm *s)
{
    // set the correct state 
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = (eOmn_appl_status_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    status->currstate = applstate_running;       
    
    EOaction_strg astg = {0};
    EOaction *ontxdone = (EOaction*)&astg;
    //eo_action_Clear(&ontxdone);

    eo_action_SetCallback(ontxdone, (eOcallback_t)eom_emsrunner_OnUDPpacketTransmitted, eom_emsrunner_GetHandle(), NULL);
    // the socket does not alert anybody when it receives a pkt, but will alert the sending task, so that it knows that it can stop wait.
    // the alert is done by a callback, eom_emsrunner_OnUDPpacketTransmitted(), which executed by the sender of the packet directly.
    // this funtion is executed with eo_action_Execute(s->socket->ontransmission) inside the EOMtheIPnet object
    // for this reason we call eo_action_SetCallback(....., exectask = NULL_); IT MUST NOT BE the callback-manager!!!!
    eom_emssocket_Open(eom_emssocket_GetHandle(), NULL, ontxdone);
    
    // we activate the runner
    eom_emsrunner_Start(eom_emsrunner_GetHandle());
    
    // exec any user-defined activity
    eom_emsappl_hid_userdef_on_entry_RUN(&s_emsappl_singleton);
}


extern void eo_cfg_sm_EMSappl_hid_on_exit_RUN(EOsm *s)
{
    //#warning --> it is good thing to attempt to stop the hal timers in here as well. see comment below.
    // marco.accame: if we exit from the runner in an un-expected way with a fatal error, then we dont 
    // stop teh timers smoothly. thus we do it in here as well.
    
    // in normal case instead, the stop of the emsrunner is not executed in one function, but in steps 
    // inside its rx-do-tx tasks.
    
    
    eom_emsrunner_Stop(eom_emsrunner_GetHandle());
    eom_emsappl_hid_userdef_on_exit_RUN(&s_emsappl_singleton);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

