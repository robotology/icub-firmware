
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

#include "EOMtheEMSconfigurator.h"
#include "EOMtheEMSrunner.h"
#include "EOMtheEMSerror.h"

#include "EOsm.h"
#include "eOcfg_sm_EMSappl.h"
#include "eOcfg_sm_EMSappl_hid.h"

#include "theHandler_Config.h"

#include "embot_app_eth_theETHmonitor.h"

#include "EOtheCANservice.h"
#include "EOtheCANdiscovery2.h"
#include "EOtheCANprotocol.h"
#include "embot_app_theLEDmanager.h"

#include "embot_app_eth_theServices.h"
//#include "embot_app_eth_theFTservice.h"
#include "embot_app_eth_theServiceFT.h"
#include "embot_app_eth_theServiceMC.h"

#include "EOtheEntities.h"


#include "EOMtheEMSrunner_hid.h" 

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// this struct wraps the use of EOMtheEMSsocket + EOMtheEMStransceiver inside theHandler
struct theCTRLsocket
{ 
    theCTRLsocket() = default;
    
    static bool init()
    {
        eom_emssocket_Initialise(&embot::app::eth::theHandler_EOMtheEMSsocket_Config);   
        eom_emstransceiver_Initialise(&embot::app::eth::theHandler_EOMtheEMStransceiver_Config);

        // i try connection now, so that if the host address does not change, then during transmission we dont do a connect anymore
        eOresult_t res = eom_emssocket_Connect(eom_emssocket_GetHandle(), embot::app::eth::theHandler_EOMtheEMStransceiver_Config.hostipv4addr, 5*EOK_reltime100ms); 

        if(eores_OK == res)
        {
            embot::app::theLEDmanager::getInstance().get(embot::hw::LED::one).on();
        }
        else
        {
            embot::app::theLEDmanager::getInstance().get(embot::hw::LED::one).off();
        }

        // we can create a connection_tick() which sets the red LED on if ... connected and then we call it regularly
        // and what if we detach the cable? can we do that inside theETHmonitor?    
        // a method  setmonitor(ip, led) follows the things ... i think that is much better      

        return true;        
    }
    
    static bool set(embot::app::eth::theHandler::State s)
    {
        EOaction_strg astg = {0};
        EOMtask *tsk2alert {nullptr};
        
        if(embot::app::eth::theHandler::State::IDLE == s)
        {
            // open/reopen the ems socket so that it must alert the EOMtheEMSconfigurator upon RX of packets
            EOaction *onrx = reinterpret_cast<EOaction*>(&astg);
            tsk2alert = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
            eo_action_SetEvent(onrx, emssocket_evt_packet_received, tsk2alert);
            eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL, NULL);
            
            // if any rx packets is already in the socket then alert the cfg task
            if(0 != eom_emssocket_NumberOfReceivedPackets(eom_emssocket_GetHandle()))
            {
                eom_task_SetEvent(tsk2alert, emssocket_evt_packet_received);        
            }
            
            // also send a tx request just in case. because cfg state transmit only if requested an we dont want to have missed a previous request.
            eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_ropframeTx);             
            
        }
        else if(embot::app::eth::theHandler::State::RUN == s)
        {
            // open/reopen the ems socket so that it must alert the EOMtheEMSrunner upon end of TX of packets
            EOaction *ontxdone = reinterpret_cast<EOaction*>(&astg);
            eo_action_SetCallback(ontxdone, (eOcallback_t)eom_emsrunner_OnUDPpacketTransmitted, eom_emsrunner_GetHandle(), NULL);
            // the socket does not alert anybody when it receives a pkt, but will alert the sending task, so that it knows that it can stop wait.
            // the alert is done by a callback, eom_emsrunner_OnUDPpacketTransmitted(), which executed by the sender of the packet directly.
            // this funtion is executed with eo_action_Execute(s->socket->ontransmission) inside the EOMtheIPnet object
            // for this reason we call eo_action_SetCallback(....., exectask = NULL_); IT MUST NOT BE the callback-manager!!!!
            eom_emssocket_Open(eom_emssocket_GetHandle(), NULL, ontxdone, NULL);            
        }
        else if(embot::app::eth::theHandler::State::FATALERROR == s)
        {
            // open/reopen the ems socket so that it must alert the EOMtheEMSerror upon RX of packets
            EOaction *onrx = reinterpret_cast<EOaction*>(&astg);
            tsk2alert = eom_emserror_GetTask(eom_emserror_GetHandle());
            eo_action_SetEvent(onrx, emssocket_evt_packet_received, tsk2alert);
            eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL, NULL);
            
            // if any rx packets already in socket then alert the err task
            if(0 != eom_emssocket_NumberOfReceivedPackets(eom_emssocket_GetHandle()))
            {
                eom_task_SetEvent(tsk2alert, emssocket_evt_packet_received);        
            }
            
            // also send a tx request just in case
            eom_task_SetEvent(tsk2alert, emssocket_evt_packet_received);                 
        }
        
        return true;
    }
    
    static bool transmissionrequest()
    {
        eom_emssocket_TransmissionRequest(eom_emssocket_GetHandle());
        return true;
    }
    
    static bool transmit(const eOropdescriptor_t &ropdes)
    {
       eom_emstransceiver_Transmit_OccasionalROP(eom_emstransceiver_GetHandle(), const_cast<eOropdescriptor_t*>(&ropdes));
       return true;         
    }
    
};


// this struct wraps:
// - the state machine of the application w/ IDLE, RUN and FATALERROR states,
// - the customisation of the associated on entry, exit and transition functions
// - the management of objects which model the states + their user-defined functions:
//   - EOMtheEMSconfigurator (IDLE),
//   - EOMtheEMSrunner (RUN),
//   - EOMtheEMSerror (FATALERROR)
// - the user-defined functions of the above objects 
// 
struct theSM
{ 
    static constexpr embot::hw::LED statusLED {embot::hw::LED::four};

    theSM() = default; 
    
    static bool initialise(EOsm *sm)
    {
        if(nullptr == sm)
        {
            return false;
        }
        
        // State::IDLE
        eom_emsconfigurator_Initialise(&embot::app::eth::theHandler_EOMtheEMSconfigurator_Config);
        // State::RUN
        eom_emsrunner_Initialise(&embot::app::eth::theHandler_EOMtheEMSrunner_Config);
        // state::FATALERROR
        eom_emserror_Initialise(&embot::app::eth::theHandler_EOMtheEMSerror_Config);
        
        // some services in here such as
        
        {    
        eo_entities_Initialise();
        eo_canmap_Initialise(NULL);
        eo_canprot_Initialise(NULL);
        }
        
        {   // C.  can services and discovery.
        // so far i do in here what i need without any container
             
        // can-services
        eOcanserv_cfg_t config;   
        
        config.mode                 = eocanserv_mode_straight;
        config.canstabilizationtime = 7*eok_reltime1sec;
        config.rxqueuesize[0]       = 64;
        config.rxqueuesize[1]       = 64;
        config.txqueuesize[0]       = 64;
        config.txqueuesize[1]       = 64;  
        config.onrxcallback[0]      = s_can_cbkonrx; 
        config.onrxargument[0]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
        config.onrxcallback[1]      = s_can_cbkonrx; 
        config.onrxargument[1]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 

            
        // inside eo_canserv_Initialise() it is called hal_can_supported_is(canx) to see if we can init the can bus as requested.
        eo_canserv_Initialise(&config);   
        
        // can-discovery
        eo_candiscovery2_Initialise(NULL);  
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetEvent(act, emsconfigurator_evt_userdef01, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
        eo_candiscovery2_SetTicker(eo_candiscovery2_GetHandle(), act);
      
        } 
        
        // create and start the state machine
        eo_sm_Start(sm);
        
        // led manager is already initialised. i pulse LED::four at 20 hz
        embot::app::theLEDmanager::getInstance().get(statusLED).pulse(embot::core::time1second/20, 0);
        
        return true;
    }
    
    static void set(eOmn_appl_state_t s)
    {
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
        eOmn_appl_status_t *status = reinterpret_cast<eOmn_appl_status_t*>(eoprot_variable_ramof_get(eoprot_board_localboard, id32));
        status->currstate = applstate_config;          
    }
    
    static void ticker_ethmonitor_idle(void *p)
    {
        eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_userdef02);
    }

    static void ticker_ethmonitor_fatalerror(void *p)
    {
        eom_task_SetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emserror_evt_userdef02);
    }   

    static void ticker_candiscovery2_idle(void *p)
    {
        eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_userdef01);
    }    

    static void s_can_cbkonrx(void *arg)
    {
        EOMtask *task = (EOMtask *)arg;
        eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
    }
    
    static void stateIDLE_onentry()
    {
        // set the correct protocol state 
        set(applstate_config);

        theCTRLsocket::set(embot::app::eth::theHandler::State::IDLE);    
           
        // EOtheCANservice: set straight mode and force parsing of all packets in the RX queues.
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
        eo_canserv_ParseAll(eo_canserv_GetHandle());
        
        // can discovery
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetEvent(act, emsconfigurator_evt_userdef01, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
        eo_candiscovery2_SetTicker(eo_candiscovery2_GetHandle(), act);

        // tell embot::app::eth::theETHmonitor to alert thread of configurator w/ event emsconfigurator_evt_userdef02    
        embot::app::eth::theETHmonitor::getInstance().set({ticker_ethmonitor_idle, nullptr});
        
        // pulse LED::four forever at 0.50 hz.       
        embot::app::theLEDmanager::getInstance().get(statusLED).pulse(2*embot::core::time1second, 0);
        
        #warning -> sm on entry cfg user defined ??? 
        // exec any user-defined activity
        //sm_userdef_on_entry_CFG();
    }

    static void stateRUN_onentry()
    {
        // set the correct protocol state 
        set(applstate_running);

        theCTRLsocket::set(embot::app::eth::theHandler::State::RUN);  

        // we activate the runner
        eom_emsrunner_Start(eom_emsrunner_GetHandle());
        
        // EOtheCANservice: set on demand mode. then tx all canframes remained in the tx queues
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_ondemand);    
        eo_canserv_TXstartAll(eo_canserv_GetHandle(), NULL, NULL);
        eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), 5*eok_reltime1ms);
        
        // can discovery
        eo_candiscovery2_SetTicker(eo_candiscovery2_GetHandle(), nullptr);

        // tell the theETHmonitor to alert no task, because the runner will tick it now at every cycle
        embot::app::eth::theETHmonitor::getInstance().set({nullptr, nullptr});

        // pulse LED::four forever at 1 hz.
        embot::app::theLEDmanager::getInstance().get(statusLED).pulse(1*embot::core::time1second, 0);

        // we dont start services  
    }
    
    static void stateFATALERROR_onentry()        
    {
        // set the correct protocol state 
        set(applstate_error);  

        theCTRLsocket::set(embot::app::eth::theHandler::State::FATALERROR);    

        // pulse LED::four forever at 4 hz.
        embot::app::theLEDmanager::getInstance().get(statusLED).pulse(embot::core::time1second/4, 0);
      

        // EOtheCANservice: set straight mode and force parsing of all packets in the RX queues.
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
        eo_canserv_ParseAll(eo_canserv_GetHandle());  
        
        // can discovery
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetEvent(act, emsconfigurator_evt_userdef01, eom_emserror_GetTask(eom_emserror_GetHandle()));
        eo_candiscovery2_SetTicker(eo_candiscovery2_GetHandle(), act);
        
        // tell embot::app::eth::theETHmonitor to alert thread of error w/ event emsconfigurator_evt_userdef02    
        embot::app::eth::theETHmonitor::getInstance().set({ticker_ethmonitor_fatalerror, nullptr});
       
        
        // stop and deactivate all the services which may have been started 
        embot::app::eth::theServices::getInstance().stop();                 
    }

    static void stateIDLE_onexit()
    {
        // tell the ethmonitor to alert no task. the relevant on_entry_xxx function will set a new action
        //eo_ethmonitor_SetAlert(eo_ethmonitor_GetHandle(), NULL, 0);        
    }
    
 
    static void stateRUN_onexit()
    {
        //#warning --> it is good thing to attempt to stop the hal timers in here as well. see comment below.
        // marco.accame: if we exit from the runner in an un-expected way with a fatal error, then we dont 
        // stop teh timers smoothly. thus we do it in here as well.
        
        // in normal case instead, the stop of the emsrunner is not executed in one function, but in steps 
        // inside its rx-do-tx tasks.
        
        
        eom_emsrunner_Stop(eom_emsrunner_GetHandle());

        // EOtheCANservice: set straigth mode and force parsing of all packets in the RX queues.
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
        eo_canserv_ParseAll(eo_canserv_GetHandle());  
              
        // stop and deactivate all the services which may have been started 
        embot::app::eth::theServices::getInstance().stop();        
    }
 

    static void stateFATALERROR_onexit()
    {
        // it cannot happen, but if it did ....
    }
    

    static void objectIDLE_on_evt(embot::os::Event evt)
    {
        switch(evt)
        {
            case emsconfigurator_evt_userdef00:
            {
                eo_canserv_ParseAll(eo_canserv_GetHandle());
            } break;
             
            case emsconfigurator_evt_userdef01:
            {
                eo_candiscovery2_Tick(eo_candiscovery2_GetHandle());
            } break;
             
            case emsconfigurator_evt_userdef02:
            {
                embot::app::eth::theETHmonitor::getInstance().tick();
            } break;
            
            default: {} break;                                  
        }
        
    }
    
    static void objectRUN_RX_afterUDPparsing()
    {
        // i tick the can-discovery. 
        // this function does something only if a discovery is active and if the search timer period has expired.
        eo_candiscovery2_Tick(eo_candiscovery2_GetHandle());
        
        // i manage the can-bus reception. i parse everything. 
        // it will be the can parser functions which will call the relevant objects which will do what they must.
        // as an example, the broadcast skin can frames are always parsed and are given to EOtheSKIN which will decide what to do with them
        eo_canserv_ParseAll(eo_canserv_GetHandle());    
    
#if defined(TESTRTC_IS_ACTIVE)     
        testRTC_RUN_tick();
#elif defined(enableTHESERVICETESTER)
        servicetester_runtick();
#endif          
    }
    
    static void objectRUN_DO_activity()
    {
        embot::app::eth::theETHmonitor::getInstance().tick();
        
        eObool_t prevTXhadRegulars = eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle());
        
        // we tick the services ...
        // see if we can use a single call embot::app::eth::theServices.tick() which ticks all the relevant ones.
        // otherwise, better moving it into a configurable function
    
        embot::app::eth::theServices::getInstance().tick(); 
        
    //    embot::app::eth::theFTservice::getInstance().Tick(); 
        
    //    eo_motioncontrol_Tick(eo_motioncontrol_GetHandle());
    //    eo_mais_Tick(eo_mais_GetHandle());
    //    eo_strain_Tick(eo_strain_GetHandle());   
    //    eo_psc_Tick(eo_psc_GetHandle());
    //    eo_pos_Tick(eo_pos_GetHandle());        
    }   

    static void objectRUN_TX_beforeUDPtx()
    {   
        uint8_t txcan1frames = 0;
        uint8_t txcan2frames = 0;

        eo_canserv_TXstartAll(eo_canserv_GetHandle(), &txcan1frames, &txcan2frames);    
        eom_emsrunner_Set_TXcanframes(eom_emsrunner_GetHandle(), txcan1frames, txcan2frames);        
    }    

    static void objectRUN_TX_afterUDPtx()
    {
    //    eObool_t prevTXhadRegulars = eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle());
    
    // ticks some services ... 
    // marco.accame: i put them in here just after tx phase. however, we can move it even in eom_emsrunner_hid_userdef_taskDO_activity() 
    // because eom_emsrunner_CycleHasJustTransmittedRegulars() keeps memory of previous tx cycle.
//    eo_skin_Tick(eo_skin_GetHandle(), prevTXhadRegulars); 
    
//#if defined(TESTRTC_IS_ACTIVE)
//#warning ---------------> just for test
//    prevTXhadRegulars = eobool_true;
//#endif
    
//    eo_inertials2_Tick(eo_inertials2_GetHandle(), prevTXhadRegulars); 
//    
//    eo_inertials3_Tick(eo_inertials3_GetHandle(), prevTXhadRegulars); 
//    
//    eo_temperatures_Tick(eo_temperatures_GetHandle(), prevTXhadRegulars);
// 
    
        // ABSOLUTELY KEEP IT LAST: wait until can tx started by eo_canserv_TXstartAll() in objectRUN_TX_beforeUDPtx() is all done
        const eOreltime_t timeout = 3*EOK_reltime1ms;
        eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), timeout);   
    }

    static void moveto(EOsm *sm, embot::app::eth::theHandler::State state)
    {
        static constexpr eOsmEventsEMSappl_t map2evt[3] = {eo_sm_emsappl_EVgo2cfg, eo_sm_emsappl_EVgo2run, eo_sm_emsappl_EVgo2err};    
        eOsmEventsEMSappl_t smevt = map2evt[embot::core::tointegral(state)];
        eo_sm_ProcessEvent(sm, smevt);
    }        
//        
//    private:
//    ~theSM() {}        
};
    


struct embot::app::eth::theHandler::Impl
{       
    Config _config {};  
        
    State _state {State::IDLE};
    
    
    EOsm *_sm {nullptr};

    Impl(); 
    
    bool initialise(const Config &cfg);    
    
    bool transmit();
    bool transmit(const eOropdescriptor_t &ropdes);
    
    bool moveto(State state);
    State state() { _state = static_cast<State>(eo_sm_GetActiveState(_sm)); return _state; } ;
    
    bool process(Command cmd);
    
    
private:

    void redefine_errorhandler(); 

};

embot::app::eth::theHandler::Impl::Impl()
{
}

bool embot::app::eth::theHandler::Impl::initialise(const Config &cfg)
{
    _config = cfg;   

    embot::app::eth::theETHmonitor::getInstance().initialise(theHandler_theETHmonitor_Config);    
    
    theCTRLsocket::init();
    _sm = eo_sm_New(eo_cfg_sm_EMSappl_Get());
    theSM::initialise(_sm);

    redefine_errorhandler();
    theCTRLsocket::set(embot::app::eth::theHandler::State::IDLE);
    
    
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
    
    
    embot::app::eth::theServices::getInstance().initialise({});   
        
    embot::app::eth::theServiceFT::getInstance().initialise({});          
    embot::app::eth::theServices::getInstance().load(embot::app::eth::theServiceFT::getInstance().service());

    embot::app::eth::theServiceMC::getInstance().initialise({});          
    embot::app::eth::theServices::getInstance().load(embot::app::eth::theServiceMC::getInstance().service());  
        
//    // create the state machine and start it
//    // its configuration comes from eo_cfg_sm_EMSappl_Get()
//    // which requires some external function such as eo_cfg_sm_EMSappl_hid_on_entry_CFG() etc.
//    // which are defined in this cpp file
//    thesm = eo_sm_New(eo_cfg_sm_EMSappl_Get());
//    eo_sm_Start(thesm);
        
        
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

    if(Command::dummy == cmd)
    {
        return false;
    }
    
    State s = state();
    State rq = static_cast<State>(cmd); 
    
    if(s == rq)
    {
        // already in requested state
        return true;
    }
    
    if(s == State::FATALERROR)
    {   // currently it is not possible to exit from error
        return false;
    }
    
    eOresult_t res {eores_NOK_generic};
    
    static constexpr eOsmEventsEMSappl_t map2evt[3] = {eo_sm_emsappl_EVgo2cfg, eo_sm_emsappl_EVgo2run, eo_sm_emsappl_EVgo2err};
    
    eOsmEventsEMSappl_t smevt = map2evt[embot::core::tointegral(rq)];
        
    switch(s)
    {
        case State::IDLE:
        {   // from here i can go only into RUN or FATALERROR
            res = eom_emsconfigurator_GracefulStopAndGoTo(eom_emsconfigurator_GetHandle(), smevt);
        } break;
        
        case State::RUN:
        {   // if i am here newstate can be only eo_sm_emsappl_STcfg (or err). i dont send an event, but i call a function which will smmotly go back to cfg
            res = eom_emsrunner_GracefulStopAndGoTo(eom_emsrunner_GetHandle(), smevt);     
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
       
    return res == eores_OK;
}


bool embot::app::eth::theHandler::Impl::transmit()
{
    return theCTRLsocket::transmissionrequest();
}


bool embot::app::eth::theHandler::Impl::transmit(const eOropdescriptor_t &ropdes)
{    
    return theCTRLsocket::transmit(ropdes); 
}


bool embot::app::eth::theHandler::Impl::moveto(State state)
{
    // this makes the sm evolve to a new state. it is called by runner etc
    // it is the old eom_emsappl_SM_ProcessEvent
    theSM::moveto(_sm, state);
    return true;
}


// - in here we start all the required services.

//void embot::app::eth::theHandler::Impl::init_ctrl_socket()
//{        
//    eom_emssocket_Initialise(&theHandler_EOMtheEMSsocket_Config);   
//    eom_emstransceiver_Initialise(&theHandler_EOMtheEMStransceiver_Config);

//    // i try connection now, so that if the host address does not change, then during transmission we dont do a connect anymore
//    eOresult_t res = eom_emssocket_Connect(eom_emssocket_GetHandle(), theHandler_EOMtheEMStransceiver_Config.hostipv4addr, 5*EOK_reltime100ms); 

//    if(eores_OK == res)
//    {
//        embot::app::theLEDmanager::getInstance().get(embot::hw::LED::one).on();
//    }
//    else
//    {
//        embot::app::theLEDmanager::getInstance().get(embot::hw::LED::one).off();
//    }

//// we can create a connection_tick() which sets the red LED on if ... connected and then we call it regularly
//// and what if we detach the cable? can we do that inside theETHmonitor?    
//// a method  setmonitor(ip, led) follows the things ... i think that is much better
//            
//}


//void embot::app::eth::theHandler::Impl::init_states()
//{
//    eom_emsconfigurator_Initialise(&theHandler_EOMtheEMSconfigurator_Config);
//    eom_emsrunner_Initialise(&theHandler_EOMtheEMSrunner_Config);
//    eom_emserror_Initialise(&theHandler_EOMtheEMSerror_Config);
//}

extern void xxx_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes);

#include "embot_os_rtos.h"
embot::os::rtos::mutex_t* onerrormutex {nullptr};
embot::os::rtos::semaphore_t* blockingsemaphore {nullptr};

void embot::app::eth::theHandler::Impl::redefine_errorhandler()
{
    onerrormutex = embot::os::rtos::mutex_new();
    blockingsemaphore = embot::os::rtos::semaphore_new(2, 0);
    
    eo_errman_SetOnErrorHandler(eo_errman_GetHandle(), xxx_OnError);


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

#include "EoError.h"
#include "EOtheInfoDispatcher.h"

static void s_manage_haltrace(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *edes);
static void s_manage_dispatch(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);
static void s_manage_fatal(const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);



extern void xxx_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes)
{
    //s_manage_haltrace(errtype, info, caller, errdes);
    if(eo_errortype_trace == errtype)
    {   // we dont transmit the trace
        return;
    }
    
    // ok, now we dispatch all but trace 
    s_manage_dispatch(errtype, info, caller, errdes);
        
    // if fatal we manage it in a particular way
    if(eo_errortype_fatal == errtype)
    {
        s_manage_fatal(info, caller, errdes);
    }    
}


static void s_manage_haltrace(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    char text[256];
    text[0] = 0; // i put a '0' terminator. just in case 
    
    const char empty[] = "EO?";
    const char *err = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
    const char *eobjstr = (NULL == caller) ? (empty) : ((NULL == caller->eobjstr) ? (empty) : (caller->eobjstr));
    const uint32_t taskid = (NULL == caller) ? (0) : (caller->taskid);
    
    embot::core::TimeFormatter tf {embot::core::now()};
    
    if(eo_errortype_trace == errtype)
    {
        if(nullptr != info)
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @%s)-> %s.", eobjstr, tf.to_string().c_str(), info); 
        }
        else
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @%s)-> ...", eobjstr, tf.to_string().c_str()); 
        }
            
    }
    else
    {        
        if((NULL == des) && (NULL == info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @%s)-> ...", err, eobjstr, taskid, tf.to_string().c_str());            
        }
        else if((NULL != des) && (NULL != info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @%s)-> {0x%x p16 0x%04x, p64 0x%016llx, dev %d, adr %d}: %s. INFO = %s", err, eobjstr, taskid, tf.to_string().c_str(), des->code, des->par16, des->par64, des->sourcedevice, des->sourceaddress, eoerror_code2string(des->code), info);  
        }
        else if((NULL != des) && (NULL == info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @%s)-> {0x%x, p16 0x%04x, p64 0x%016llx, dev %d, adr %d}: %s.", err, eobjstr, taskid, tf.to_string().c_str(), des->code, des->par16, des->par64, des->sourcedevice, des->sourceaddress, eoerror_code2string(des->code));               
        }
        else if((NULL == des) && (NULL != info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @%s)-> {}: ... INFO = %s", err, eobjstr, taskid, tf.to_string().c_str(), info);  
        }
    }   

    embot::core::print(text);     
}


static void s_manage_dispatch(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
   
    
//#if defined(DIAGNOSTIC2_send_to_yarprobotinterface)

    // old c code used so far
    
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
    embot::os::rtos::mutex_take(onerrormutex, embot::core::reltimeWaitForever);
    
    // the call eo_infodispatcher_Put() is only in here, thus we can either protect with a mutex in here or put put a mutex inside. WE USE MUTEX IN HERE
    eo_infodispatcher_Put(eo_infodispatcher_GetHandle(), &props, info); 
    eom_emssocket_TransmissionRequest(eom_emssocket_GetHandle());
    
    // the call eom_emsappl_SendTXRequest() either does nothing and is reentrant (when in run mode) or sends an event to a task with osal_eventflag_set() which is reentrant.
    // it is called also by the can protocol parser in case of proxy, which however is used in run mode (thus reentrant).
    // i protect both functions in here
//    eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());
#warning add a eom_emsappl_SendTXRequest()
    
    embot::os::rtos::mutex_release(onerrormutex);   

//#endif

}

static void s_manage_fatal(const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    // manage fatal error (go in error state, start periodic tx of error status)  
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
    #warning MANAGE eom_emsappl_SM_ProcessEvent
//    eom_emsappl_SM_ProcessEvent(eom_emsappl_GetHandle(), eo_sm_emsappl_EVgo2err);  
    eom_task_SetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emserror_evt_fatalerror);    
    

    // if the caller is not the error state, then we block it execution.
    // however, we cannot block the execution of the error state otherwise ... we lose operativity
    // of error task and amongst others we lose communication with the remote host.
    // when we are in error state the remote host must be able to know it.
    
    for(;;);
 
//    eOsmStatesEMSappl_t state = eo_sm_emsappl_STcfg;
    #warning MANAGE eom_emsappl_GetCurrentState
//    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &state);
    
//    if(eo_sm_emsappl_STerr != state)
//    {
//        // this call makes the calling task wait in here forever.
//        osal_semaphore_decrement(blockingsemaphore, OSAL_reltimeINFINITE);
//        // the forever loop should not be necessary.
//        for(;;);
//    }
    
//#else
//    // in this situation, the transition to error state is done by the active task, thus we require that the execution
//    // continues. the good thing is that we exit in a clean way from the state, but the high risk is to execute dangerous instructions.
//    // example: some object inside the runner detects that some pointer that is going to be used is NULL. if we keep on running this
//    // code, then teh NULL pointer is deferenced and teh application crashes....    
//    eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STerr);
//    return;
//#endif    
}

//void embot::app::eth::theHandler::Impl::theemssocket_defaultopen()
//{   
//    // we also open the socket, so that we can tx or rx straight away. for now we direct towards the configurator task
//    EOaction_strg astg = {0};
//    EOaction *onrx = (EOaction*)&astg;
//    eo_action_SetEvent(onrx, emssocket_evt_packet_received, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
//    // the socket alerts the cfg task for any newly received packet
//    eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL, NULL);
//}


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

embot::app::eth::theHandler::State embot::app::eth::theHandler::state()
{
    return pImpl->state();
}
 

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


// - extern functions required by some of the objects in here .... 

// - eOcfg_sm_EMSappl: configuration of the state machine ...

#include "eOcfg_sm_EMSappl_hid.h"

// -- init

extern void eo_cfg_sm_EMSappl_hid_init(EOsm *s)
{
    // does nothing
}  

// -- reset

extern void eo_cfg_sm_EMSappl_hid_reset(EOsm *s)
{
    // does nothing
}  


// -- on entry

// CFG
extern void eo_cfg_sm_EMSappl_hid_on_entry_CFG(EOsm *s)
{
    theSM::stateIDLE_onentry();
}
//{
//    // set the correct state 
//    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
//    eOmn_appl_status_t *status = reinterpret_cast<eOmn_appl_status_t*>(eoprot_variable_ramof_get(eoprot_board_localboard, id32));
//    status->currstate = applstate_config;  

//    theCTRLsocket::set(embot::app::eth::theHandler::State::IDLE);    
//    
////    // tell the ems socket that it must alert the EOMtheEMSconfigurator upon RX of packets
////    EOaction_strg astg = {0};
////    EOaction *onrx = reinterpret_cast<EOaction*>(&astg);
////    eo_action_SetEvent(onrx, emssocket_evt_packet_received, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
////    eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL, NULL);
////    
////    // if any rx packets is already in the socket then alert the cfg task
////    if(0 != eom_emssocket_NumberOfReceivedPackets(eom_emssocket_GetHandle()))
////    {
////        eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emssocket_evt_packet_received);        
////    }
////    
////    // also send a tx request just in case. because cfg state transmit only if requested an we dont want to have missed a previous request.
////    eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_ropframeTx);    
//       
//    // EOtheCANservice: set straight mode and force parsing of all packets in the RX queues.
//    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
//    eo_canserv_ParseAll(eo_canserv_GetHandle());

//    #warning -> carefully check if also embot::app::eth::theETHmonitor must alert w/ emsconfigurator_evt_userdef02 ...   
//    // well, it must if we use a tick .... maybe yes....
//    
//    // tell the ethmonitor to alert the task of the configurator
////    eo_ethmonitor_SetAlert(eo_ethmonitor_GetHandle(), eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_userdef02);
//    

//    // pulse LED::four forever at 0.50 hz.       
//    embot::app::theLEDmanager::getInstance().get(statusLED).pulse(2*embot::core::time1second, 0);
//    
//    #warning -> sm on entry cfg user defined ??? 
//    // exec any user-defined activity
//    //sm_userdef_on_entry_CFG();
//}  

// RUN
extern void eo_cfg_sm_EMSappl_hid_on_entry_RUN(EOsm *s)
{
    theSM::stateRUN_onentry();
}
//{
//    // set the correct state 
//    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
//    eOmn_appl_status_t *status = reinterpret_cast<eOmn_appl_status_t*>(eoprot_variable_ramof_get(eoprot_board_localboard, id32));
//    status->currstate = applstate_running;       


//    theCTRLsocket::set(embot::app::eth::theHandler::State::RUN);  
//    
////    EOaction_strg astg = {0};
////    EOaction *ontxdone = reinterpret_cast<EOaction*>(&astg);
////    // tell the ems socket that it must alert the EOMtheEMSconfigurator upon RX of packets
////    eo_action_SetCallback(ontxdone, (eOcallback_t)eom_emsrunner_OnUDPpacketTransmitted, eom_emsrunner_GetHandle(), NULL);
////    // the socket does not alert anybody when it receives a pkt, but will alert the sending task, so that it knows that it can stop wait.
////    // the alert is done by a callback, eom_emsrunner_OnUDPpacketTransmitted(), which executed by the sender of the packet directly.
////    // this funtion is executed with eo_action_Execute(s->socket->ontransmission) inside the EOMtheIPnet object
////    // for this reason we call eo_action_SetCallback(....., exectask = NULL_); IT MUST NOT BE the callback-manager!!!!
////    eom_emssocket_Open(eom_emssocket_GetHandle(), NULL, ontxdone, NULL);
//    
//    // we activate the runner
//    eom_emsrunner_Start(eom_emsrunner_GetHandle());

//    
//    // EOtheCANservice: set on demand mode. then tx all canframes remained in the tx queues
//    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_ondemand);    
//    eo_canserv_TXstartAll(eo_canserv_GetHandle(), NULL, NULL);
//    eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), 5*eok_reltime1ms);

//#warning -> adjust theETHmonitor     
//    // tell the ethmonitor to alert no task, because the runner will tick it now at every cycle
////    eo_ethmonitor_SetAlert(eo_ethmonitor_GetHandle(), NULL, 0);


//    // pulse LED::four forever at 1 hz.
//    embot::app::theLEDmanager::getInstance().get(statusLED).pulse(1*embot::core::time1second, 0);

//    // we dont start services    
//}  


extern void eo_cfg_sm_EMSappl_hid_on_entry_ERR(EOsm *s)
{
    theSM::stateFATALERROR_onentry();
}
 

// -- on exit 

extern void eo_cfg_sm_EMSappl_hid_on_exit_CFG(EOsm *s)
{
    theSM::stateIDLE_onexit();
    #warning add something in here  
    // eom_emsappl_hid_userdef_on_exit_CFG(&s_emsappl_singleton);
}  

extern void eo_cfg_sm_EMSappl_hid_on_exit_RUN(EOsm *s)
{
    theSM::stateRUN_onexit();
}
//{
//    //#warning --> it is good thing to attempt to stop the hal timers in here as well. see comment below.
//    // marco.accame: if we exit from the runner in an un-expected way with a fatal error, then we dont 
//    // stop teh timers smoothly. thus we do it in here as well.
//    
//    // in normal case instead, the stop of the emsrunner is not executed in one function, but in steps 
//    // inside its rx-do-tx tasks.
//    
//    
//    eom_emsrunner_Stop(eom_emsrunner_GetHandle());
//    #warning add something in here 
//    // eom_emsappl_hid_userdef_on_exit_RUN(&s_emsappl_singleton);
//}

extern void eo_cfg_sm_EMSappl_hid_on_exit_ERR(EOsm *s)
{
    // we dont actually exit, but ...
    theSM::stateFATALERROR_onexit();
}  



// -- on trans

extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2run(EOsm *s)
{
    // nothing to do
}  

extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2err(EOsm *s)
{
    // nothing to do
}  

extern void eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2cfg(EOsm *s)
{
    // nothing to do
}  

extern void eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2err(EOsm *s)
{
    // nothing to do
}



// - EOMtheEMSconfigurator: redefinition of userdef functions
//   formerly inside overridden_configurator.c

#include "EOMtheEMSconfigurator_hid.h" 

extern void eom_emsconfigurator_hid_userdef_ProcessUserdef00Event(EOMtheEMSconfigurator* p)
{ 
    theSM::objectIDLE_on_evt(emsconfigurator_evt_userdef00);    
}


// marco.accame on 15 sept 15:  the event emsconfigurator_evt_userdef01 is send by a timer inside the EOtheCANdiscovery2 
// if we are in CFG state so that we can _Tick() it.

#warning must configure someone inside transitions of the SM to alert teh candiscovery2
extern void eom_emsconfigurator_hid_userdef_ProcessUserdef01Event(EOMtheEMSconfigurator* p)
{
    theSM::objectIDLE_on_evt(emsconfigurator_evt_userdef01);
}

extern void eom_emsconfigurator_hid_userdef_ProcessUserdef02Event(EOMtheEMSconfigurator* p)
{
    theSM::objectIDLE_on_evt(emsconfigurator_evt_userdef02);
}

#include "EoError.h"
#warning VERIFY: this callback is called by the EMSconfigurator both for parsing and forming of a packet 
// maybe it is better to rename eoerror_value_SYS_configurator_udptxfailure into eoerror_value_SYS_configurator_udprxtxfailure
extern void eom_emsconfigurator_hid_userdef_onemstransceivererror(EOMtheEMStransceiver* p)
{
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_configurator_udptxfailure);
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSconfigurator", &errdes); 
}

extern void eom_emsconfigurator_hid_userdef_DoJustAfterPacketParsing(EOMtheEMSconfigurator* p) {}
extern void eom_emsconfigurator_hid_userdef_ProcessUserdef03Event(EOMtheEMSconfigurator* p) {}
extern void eom_emsconfigurator_hid_userdef_ProcessTickEvent(EOMtheEMSconfigurator* p) {}
extern void eom_emsconfigurator_hid_userdef_ProcessTimeout(EOMtheEMSconfigurator* p) {}


// - EOMtheEMSrunner: redefinition of userdef functions
//   formerly inside overridden_runner.c

extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    theSM::objectRUN_RX_afterUDPparsing();
    
//    // i tick the can-discovery. 
//    // this function does something only if a discovery is active and if the search timer period has expired.
//    eo_candiscovery2_Tick(eo_candiscovery2_GetHandle());
//    
//    // i manage the can-bus reception. i parse everything. 
//    // it will be the can parser functions which will call the relevant objects which will do what they must.
//    // as an example, the broadcast skin can frames are always parsed and are given to EOtheSKIN which will decide what to do with them
//    eo_canserv_ParseAll(eo_canserv_GetHandle());    
//    
//#if defined(TESTRTC_IS_ACTIVE)     
//    testRTC_RUN_tick();
//#elif defined(enableTHESERVICETESTER)
//    servicetester_runtick();
//#endif  
}

#include "embot_app_eth_theServices.h"
#include "embot_app_eth_theFTservice.h"

extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    theSM::objectRUN_DO_activity();
    
//    eObool_t prevTXhadRegulars = eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle());
//    
//    embot::app::eth::theETHmonitor::getInstance().tick();
//    
//    // we tick the services ...
//    // see if we can use a single call embot::app::eth::theServices.tick() which ticks all the relevant ones.
//    // otherwise, better moving it into a configurable function
//    
//    embot::app::eth::theFTservice::getInstance().Tick(); 
//    
////    eo_motioncontrol_Tick(eo_motioncontrol_GetHandle());
////    eo_mais_Tick(eo_mais_GetHandle());
////    eo_strain_Tick(eo_strain_GetHandle());   
////    eo_psc_Tick(eo_psc_GetHandle());
////    eo_pos_Tick(eo_pos_GetHandle());
}

extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{
    theSM::objectRUN_TX_beforeUDPtx();
//    
//    uint8_t txcan1frames = 0;
//    uint8_t txcan2frames = 0;

//    eo_canserv_TXstartAll(eo_canserv_GetHandle(), &txcan1frames, &txcan2frames);    
//    eom_emsrunner_Set_TXcanframes(eom_emsrunner_GetHandle(), txcan1frames, txcan2frames);
}

extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    theSM::objectRUN_TX_afterUDPtx();
    
//    eObool_t prevTXhadRegulars = eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle());
//    
//    // ticks some services ... 
//    // marco.accame: i put them in here just after tx phase. however, we can move it even in eom_emsrunner_hid_userdef_taskDO_activity() 
//    // because eom_emsrunner_CycleHasJustTransmittedRegulars() keeps memory of previous tx cycle.
////    eo_skin_Tick(eo_skin_GetHandle(), prevTXhadRegulars); 
//    
////#if defined(TESTRTC_IS_ACTIVE)
////#warning ---------------> just for test
////    prevTXhadRegulars = eobool_true;
////#endif
//    
////    eo_inertials2_Tick(eo_inertials2_GetHandle(), prevTXhadRegulars); 
////    
////    eo_inertials3_Tick(eo_inertials3_GetHandle(), prevTXhadRegulars); 
////    
////    eo_temperatures_Tick(eo_temperatures_GetHandle(), prevTXhadRegulars);
//// 
//    
//    // ABSOLUTELY KEEP IT LAST: wait until can tx started by eo_canserv_TXstartAll() in eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission() is all done
//    const eOreltime_t timeout = 3*EOK_reltime1ms;
//    eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), timeout);   

//    return; 
}

extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p)
{
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runner_udptxfailure);
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSrunner", &errdes); 
}


extern void eom_emsrunner_hid_userdef_onemstransceivererror(EOMtheEMStransceiver *p)
{
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runner_transceivererror);
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSrunner", &errdes); 
}

extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p) {}
    
#include "EOMtheEMSerror_hid.h"     
    
extern void eom_emserror_hid_userdef_DoJustAfterPacketReceived(EOMtheEMSerror *p, EOpacket *rxpkt) {} 
extern void eom_emserror_hid_userdef_OnRXuserdefevent(eOemserror_event_t evt) {}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


