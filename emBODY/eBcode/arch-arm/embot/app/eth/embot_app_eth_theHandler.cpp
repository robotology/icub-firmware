
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theHandler.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServices.h"
#include "embot_app_eth_theErrorManager.h"

#include "EOnvSet.h"
#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSsocket.h"
#include "EoProtocolMN.h"

//#include "EOtheBOARDtransceiver.h"

#include "EOMtheEMSconfigurator.h"
#include "EOMtheEMSrunner.h"
#include "EOMtheEMSerror.h"

#include "EOsm.h"
#include "eOcfg_sm_EMSappl.h"

#include "theHandler_Config.h"

#include "embot_app_eth_theETHmonitor.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::eth::theHandler::Impl
{       
    Config _config {};  
        
    State _state {State::IDLE};
    
    EOsm *thesm {nullptr};

    Impl(); 
    
    bool initialise(const Config &cfg);    
    
    bool transmit();
    bool transmit(const eOropdescriptor_t &ropdes);
    
    bool moveto(State state);
    State state() const { return _state; } ;
    
    bool process(Command cmd);
    
    
private:

    void init_ctrl_socket(); 
    void init_states();
    void redefine_errorhandler(); 
    void theemssocket_defaultopen();

};

embot::app::eth::theHandler::Impl::Impl()
{
}

bool embot::app::eth::theHandler::Impl::initialise(const Config &cfg)
{
    _config = cfg;    
    
    init_ctrl_socket();
    init_states();
    redefine_errorhandler();
    theemssocket_defaultopen();
    
    embot::app::eth::theETHmonitor::getInstance().initialise(theHandler_theETHmonitor_Config);
    
    // now call all the other things inside eom_emsappl_hid_userdef_initialise() defined inside overridden_appl.c 
#if 0
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);     
    // do whatever is needed to start services.
    s_overridden_appl_initialise_services();   ->
    {
    eOipv4addr_t ipaddress = eom_ipnet_GetIPaddress(eom_ipnet_GetHandle());
    eo_services_Initialise(ipaddress);        
    }
#endif     
    
    // led pulser ???
    embot::app::eth::theServices::getInstance().initialise({});    
        
    // create the state machine and start it
    thesm = eo_sm_New(eo_cfg_sm_EMSappl_Get());
    eo_sm_Start(thesm);
        
        
    // tell the world that we have started
    embot::app::eth::emit(sevTRACE, {"testThread", nullptr}, {}, "embot::app::eth::theHandler::initialise() has started its state machine w/ CFG, RUN, ERR");   
        
    return true;
} 

#warning TODO: must develop embot::app::eth::theServices ... 

//bool embot::app::eth::theHandler::Impl::process(Event ev)
//{   
//    return true;
//}

bool embot::app::eth::theHandler::Impl::process(Command cmd)
{   
    // this asks to the runner, configurator etc to go gracefully a new state
    // it is the old eom_emsappl_ProcessGo2stateRequest() 
    return true;
}


bool embot::app::eth::theHandler::Impl::transmit()
{
    eom_emssocket_TransmissionRequest(eom_emssocket_GetHandle());
    return true;
}


bool embot::app::eth::theHandler::Impl::transmit(const eOropdescriptor_t &ropdes)
{    
   eom_emstransceiver_Transmit_OccasionalROP(eom_emstransceiver_GetHandle(), const_cast<eOropdescriptor_t*>(&ropdes));
   return true; 
}


bool embot::app::eth::theHandler::Impl::moveto(State state)
{
    // this makes the sm evolve to a new state. it is called by runner etc
    // it is the old eom_emsappl_SM_ProcessEvent
    return true;
}


// - in here we start all the required services.

void embot::app::eth::theHandler::Impl::init_ctrl_socket()
{        
    eom_emssocket_Initialise(&theHandler_EOMtheEMSsocket_Config);   
    eom_emstransceiver_Initialise(&theHandler_EOMtheEMStransceiver_Config);

    // i try connection now, so that if the host address does not change, then during transmission we dont do a connect anymore
    eom_emssocket_Connect(eom_emssocket_GetHandle(), theHandler_EOMtheEMStransceiver_Config.hostipv4addr, 5*EOK_reltime100ms); 

    #warning ... we should switch the red LED on if connected and off if not: THINK of it 
        
//    
//    if(eores_OK == res)
//    {
//        // set led0 to ON
//       eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
//    }
//    else
//    {
//        eo_ledpulser_Off(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
//    }
//    
//    snprintf(str, sizeof(str), "eom_emssocket_Connect is over. Link w/ host %s is %s", ipv4str, (eores_OK == res) ? "ON" : "OFF");
//    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);       
}


void embot::app::eth::theHandler::Impl::init_states()
{
    eom_emsconfigurator_Initialise(&theHandler_EOMtheEMSconfigurator_Config);
    eom_emsrunner_Initialise(&theHandler_EOMtheEMSrunner_Config);
    eom_emserror_Initialise(&theHandler_EOMtheEMSerror_Config);
}

void embot::app::eth::theHandler::Impl::redefine_errorhandler()
{
#if 1
    #warning add for DIAGNOSTIC2_enabled
    // or not ...
#else    
    embot_cif_diagnostic_Init();
    // this calls the legacy or the new one depending on internal macros 
    eo_errman_SetOnErrorHandler(eo_errman_GetHandle(), embot_cif_diagnostic_OnError);
#endif
    

//#if !defined(DIAGNOSTIC2_enabled)
//    s_emsappl_singleton.blockingsemaphore = osal_semaphore_new(2, 0);
//    s_emsappl_singleton.onerrormutex = osal_mutex_new();
//    eo_errman_SetOnErrorHandler(eo_errman_GetHandle(), s_eom_emsappl_OnError);
//#else    
//    // init the embot::core and diagnostic
//    static const embot_cif_core_Config ccfg = { .timeinit = NULL, .timeget = osal_system_abstime_get, .print = hal_trace_puts};
//    embot_cif_core_Init(&ccfg);    
//    embot_cif_diagnostic_Init();
//    // this calls the legacy or the new one depending on internal macros 
//    eo_errman_SetOnErrorHandler(eo_errman_GetHandle(), embot_cif_diagnostic_OnError);
//#endif        
}

void embot::app::eth::theHandler::Impl::theemssocket_defaultopen()
{   
    // we also open the socket, so that we can tx or rx straight away. for now we direct towards the configurator task
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetEvent(onrx, emssocket_evt_packet_received, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    // the socket alerts the cfg task for any newly received packet
    eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL, NULL);
}


// --------------------------------------------------------------------------------------------------------------------


embot::app::eth::theHandler& embot::app::eth::theHandler::getInstance()
{
    static theHandler* p = new theHandler();
    return *p;
}

embot::app::eth::theHandler::theHandler()
{
    pImpl = std::make_unique<Impl>();
}  
    
embot::app::eth::theHandler::~theHandler() { }
        
bool embot::app::eth::theHandler::initialise(const Config &cfg)
{
    return pImpl->initialise(cfg);
}

//bool embot::app::eth::theHandler::process(Event ev)
//{
//    return pImpl->process(ev);
//}
 

bool embot::app::eth::theHandler::transmit()
{
   return pImpl->transmit(); 
}

bool embot::app::eth::theHandler::transmit(const eOropdescriptor_t &ropdes)
{
   return pImpl->transmit(ropdes); 
}

bool embot::app::eth::theHandler::moveto(State state)
{
    return pImpl->moveto(state);
}

bool embot::app::eth::theHandler::process(Command cmd)
{
    return pImpl->process(cmd);
}
 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


