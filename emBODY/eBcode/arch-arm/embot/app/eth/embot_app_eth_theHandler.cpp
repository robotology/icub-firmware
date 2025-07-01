
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

#include "embot_app_eth_theBackdoor.h"

#include "embot_os_theScheduler.h"

#if defined(USE_ICC_COMM) 
#include "embot_app_eth_theICCservice.h"
#include "embot_app_eth_icc_ItemCANframe.h"
#include "embot_app_eth_theICCserviceROP.h"
#include "embot_app_eth_theICCserviceCAN.h"
#endif

#include "EOtheCANservice.h"
#include "EOtheCANdiscovery2.h"
#include "EOtheCANprotocol.h"
#include "embot_app_theLEDmanager.h"

#include "embot_app_eth_theServices.h"
#include "embot_app_eth_theServiceFT.h"
#include "embot_app_eth_theEncoderReader.h"

#if defined(USE_EMBOT_theServicesMC)
#include "embot_app_eth_theServiceMC.h"
#endif

#include "EOtheEntities.h"

#include "EOMtheEMSrunner_hid.h" 

#include "theIPnet_config.h"
#if !defined(THEIPNET_CONFIG_dontuse_theEnvironment)
#include "embot_app_eth_theEnvironment.h"
#endif

#if defined(STM32HAL_BOARD_AMCFOC_2CM4)        
#include "embot_hw_bsp_amcfoc_2cm4.h"
#endif

#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#define CANICCflushMODE_DO_phase

//#define TEST_ICCROP

#if defined(TEST_ICCROP)
void testiccrop();
#endif // TEST_ICCROP

// the theCTRLsocket objects wraps the use of EOMtheEMSsocket + EOMtheEMStransceiver inside theHandler
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
        EOaction_strg astg2 = {0};
        EOMtask *tsk2alert {nullptr};
        
        if(embot::app::eth::theHandler::State::IDLE == s)
        {
            // open/reopen the ems socket so that it must alert the EOMtheEMSconfigurator upon RX of packets
            EOaction *onrx = reinterpret_cast<EOaction*>(&astg);
            tsk2alert = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
            eo_action_SetEvent(onrx, emssocket_evt_packet_received, tsk2alert);            
            EOaction *ontxrequest = reinterpret_cast<EOaction*>(&astg2);
            eo_action_SetEvent(ontxrequest, emsconfigurator_evt_ropframeTx, tsk2alert);
                        
            eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL, ontxrequest);
                      
            // if any rx packets is already in the socket then alert the cfg task
            if(0 != eom_emssocket_NumberOfReceivedPackets(eom_emssocket_GetHandle()))
            {
                eom_task_SetEvent(tsk2alert, emssocket_evt_packet_received);        
            }
            
            // also send a tx request just in case. 
            // because cfg state transmits only when requested and we dont want to have missed a previous request.
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

            EOaction *ontxrequest = reinterpret_cast<EOaction*>(&astg2);
            eo_action_SetEvent(ontxrequest, emsconfigurator_evt_ropframeTx, tsk2alert);            
            
            eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL, ontxrequest);
            
            // if any rx packets are already in socket then alert the err task
            if(0 != eom_emssocket_NumberOfReceivedPackets(eom_emssocket_GetHandle()))
            {
                eom_task_SetEvent(tsk2alert, emssocket_evt_packet_received);        
            }
            
            // also send a tx request just in case
            // because err state transmits only when requested and we dont want to have missed a previous request.
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


// the theSM object wraps:
// - the state machine of the application w/ IDLE, RUN and FATALERROR states,
// - the customisation of the associated on entry, on exit and on transition functions
// - the management of objects which model the states + their user-defined functions:
//   - EOMtheEMSconfigurator (IDLE),
//   - EOMtheEMSrunner (RUN),
//   - EOMtheEMSerror (FATALERROR)
// - the user-defined functions of the above objects 

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
            
        return true;
    }
    
    static bool start(EOsm *sm)
    {
        if(nullptr == sm)
        {
            return false;
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

    static void s_can_cbkonrx_idle(void *arg)
    {
        EOMtask *task = (EOMtask *)arg;
        eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
    }
#if defined(USE_ICC_COMM)     
    static void s_icc_cbkonrx_idle(void *arg)
    {
        eom_task_isrSetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_userdef03);
    }    
#endif    
    static void s_can_cbkonrx_fatalerror(void *arg)
    {
        eom_task_isrSetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emserror_evt_userdef01);
    } 
#if defined(USE_ICC_COMM)     
    static void s_icc_cbkonrx_fatalerror(void *arg)
    {
        eom_task_isrSetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emserror_evt_userdef01);
    }    
#endif    
    static void stateIDLE_onentry()
    {
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"stateIDLE_onentry()", thr}, {}, "called");        
        
        // set the correct protocol state 
        theSM::set(applstate_config);        

        theCTRLsocket::set(embot::app::eth::theHandler::State::IDLE);    
           
        // EOtheCANservice: set straight mode and force parsing of all packets in the RX queues.
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
        eo_canserv_ParseAll(eo_canserv_GetHandle());
#if defined(USE_ICC_COMM) 
#if defined(debugNOicc)
#else            

 
#if defined(useICCserviceCAN)
        embot::app::eth::icc::theICCserviceCAN::getInstance().set(embot::app::eth::icc::theICCservice::modeTX::instant);  
        embot::app::eth::icc::theICCserviceCAN::getInstance().parse();
        embot::app::eth::icc::theICCserviceCAN::getInstance().flush(5*embot::core::time1millisec); 
        embot::app::eth::icc::theICCserviceCAN::getInstance().set({s_icc_cbkonrx_idle, nullptr}); 
#else
       // theICCservice
        embot::app::eth::icc::theICCservice::getInstance().set(embot::app::eth::icc::theICCservice::Pipe::one, embot::app::eth::icc::theICCservice::modeTX::instant);  
        embot::app::eth::icc::theICCservice::getInstance().parse(embot::app::eth::icc::theICCservice::Pipe::one);
        embot::app::eth::icc::theICCservice::getInstance().flush(embot::app::eth::icc::theICCservice::Pipe::one, 5*embot::core::time1millisec); 
        embot::app::eth::icc::theICCservice::getInstance().set(embot::app::eth::icc::theICCservice::Pipe::one, {s_icc_cbkonrx_idle, nullptr});  
#endif // #if defined(useICCserviceCAN)

   
#endif
#endif 

        // in here i can test the icc rop
        
        #if defined(TEST_ICCROP)
        testiccrop();
        #endif // TEST_ICCROP

       
        // can discovery
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetEvent(act, emsconfigurator_evt_userdef01, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
        eo_candiscovery2_SetTicker(eo_candiscovery2_GetHandle(), act);

        // tell embot::app::eth::theETHmonitor to alert thread of configurator w/ event emsconfigurator_evt_userdef02    
        embot::app::eth::theETHmonitor::getInstance().set({ticker_ethmonitor_idle, nullptr});
        
        // pulse LED::four forever at 0.50 hz.       
        embot::app::theLEDmanager::getInstance().get(statusLED).pulse(2*embot::core::time1second, 0);
    }

    static void stateRUN_onentry()
    {
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"stateRUN_onentry()", thr}, {}, "called");
        
        // set the correct protocol state 
        theSM::set(applstate_running);

        theCTRLsocket::set(embot::app::eth::theHandler::State::RUN);  

        // we activate the runner
        eom_emsrunner_Start(eom_emsrunner_GetHandle());
        
        // EOtheCANservice: set on demand mode. then tx all canframes remained in the tx queues
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_ondemand);    
        eo_canserv_TXstartAll(eo_canserv_GetHandle(), NULL, NULL);
        eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), 5*eok_reltime1ms);
#if defined(USE_ICC_COMM)  
#if defined(debugNOicc)
#else            

#if defined(useICCserviceCAN)
        embot::app::eth::icc::theICCserviceCAN::getInstance().set(embot::app::eth::icc::theICCservice::modeTX::onflush);  
        embot::app::eth::icc::theICCserviceCAN::getInstance().flush(5*embot::core::time1millisec);  
        embot::app::eth::icc::theICCserviceCAN::getInstance().set(embot::core::Callback()); 
#else
        // theICCservice
        embot::app::eth::icc::theICCservice::getInstance().set(embot::app::eth::icc::theICCservice::Pipe::one, embot::app::eth::icc::theICCservice::modeTX::onflush);  
        embot::app::eth::icc::theICCservice::getInstance().flush(embot::app::eth::icc::theICCservice::Pipe::one, 5*embot::core::time1millisec);  
        embot::app::eth::icc::theICCservice::getInstance().set(embot::app::eth::icc::theICCservice::Pipe::one, embot::core::Callback());  
#endif // #if defined(useICCserviceCAN)

#endif

#endif         
           
        
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
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"stateFATALERROR_onentry()", thr}, {}, "called");
            
        // set the correct protocol state 
        theSM::set(applstate_error);  

        theCTRLsocket::set(embot::app::eth::theHandler::State::FATALERROR);    

        // pulse LED::four forever at 4 hz.
        embot::app::theLEDmanager::getInstance().get(statusLED).pulse(embot::core::time1second/4, 0);
      
        // EOtheCANservice: set straight mode and force parsing of all packets in the RX queues.
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
        eo_canserv_ParseAll(eo_canserv_GetHandle());  
        //#warning shall we set s_can_cbkonrx_fatalerror ??  we cannot do it for EOtheCANservice  
#if defined(USE_ICC_COMM) 
#if defined(debugNOicc)
#else            

#if defined(useICCserviceCAN)
        embot::app::eth::icc::theICCserviceCAN::getInstance().set(embot::app::eth::icc::theICCservice::modeTX::instant);  
        embot::app::eth::icc::theICCserviceCAN::getInstance().parse();   
        embot::app::eth::icc::theICCserviceCAN::getInstance().set({s_icc_cbkonrx_fatalerror, eom_emserror_GetTask(eom_emserror_GetHandle())}); 
#else
        // theICCservice
        embot::app::eth::icc::theICCservice::getInstance().set(embot::app::eth::icc::theICCservice::Pipe::one, embot::app::eth::icc::theICCservice::modeTX::instant);  
        embot::app::eth::icc::theICCservice::getInstance().parse(embot::app::eth::icc::theICCservice::Pipe::one);   
        embot::app::eth::icc::theICCservice::getInstance().set(embot::app::eth::icc::theICCservice::Pipe::one, {s_icc_cbkonrx_fatalerror, eom_emserror_GetTask(eom_emserror_GetHandle())});             
#endif // #if defined(useICCserviceCAN)

        
#endif
#endif                      
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
        // do nothing: the relevant on_entry_xxx function will do what is needed    
    }
    
    static void stateRUN_onexit()
    {
//        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
//        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"stateRUN_onexit()", thr}, {}, "called");
            
        // marco.accame: 
        // the stop of the EOMtheEMSrunner can be safely called also when it is already stopped
        // so we call it again, because:        
        // in the normal case, the stop of the EOMtheEMSrunner is executed by stopping the HW timers
        // in steps inside its rx-do-tx tasks. 
        // but if we exit from the RUN state in an un-expected way with a fatal error, then we dont 
        // stop the timers smoothly. thus we call eom_emsrunner_Stop() in here.        
        eom_emsrunner_Stop(eom_emsrunner_GetHandle());

        // EOtheCANservice: set straigth mode and force parsing of all the frames in the RX queues.
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
        eo_canserv_ParseAll(eo_canserv_GetHandle());  
#if defined(USE_ICC_COMM) 
        
#if defined(debugNOicc)
#else        

#if defined(useICCserviceCAN)
        embot::app::eth::icc::theICCserviceCAN::getInstance().set(embot::app::eth::icc::theICCservice::modeTX::instant);  
        embot::app::eth::icc::theICCserviceCAN::getInstance().parse();  
        embot::app::eth::icc::theICCserviceCAN::getInstance().set({s_icc_cbkonrx_idle, nullptr});  
#else
        // theICCservice
        embot::app::eth::icc::theICCservice::getInstance().set(embot::app::eth::icc::theICCservice::Pipe::one, embot::app::eth::icc::theICCservice::modeTX::instant);  
        embot::app::eth::icc::theICCservice::getInstance().parse(embot::app::eth::icc::theICCservice::Pipe::one);  
        embot::app::eth::icc::theICCservice::getInstance().set(embot::app::eth::icc::theICCservice::Pipe::one, {s_icc_cbkonrx_idle, nullptr});   

#endif // #if defined(useICCserviceCAN)     
#endif
#endif        
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
#if defined(USE_ICC_COMM) 
            case emsconfigurator_evt_userdef03:
            {
#if defined(debugNOicc)
#else                


#if defined(useICCserviceCAN)
                embot::app::eth::icc::theICCserviceCAN::getInstance().parse();
#else
                embot::app::eth::icc::theICCservice::getInstance().parse(embot::app::eth::icc::theICCservice::Pipe::one);                  
#endif                
                
#endif                
            } break;
#endif            
            default: {} break;                                  
        }
        
    }

#if defined(STM32HAL_BOARD_AMC) && defined(DEBUG_AEA3_stream_over_theBackdoor)    
    EOpacket *packet {nullptr};    
    eOipv4addr_t hostip {EO_COMMON_IPV4ADDR(10, 0, 1, 104)};
    eOipv4port_t hostport {6666};
#endif
    
    static void objectRUN_RX_afterUDPparsing()
    {
        // i tick the can-discovery. 
        // this function does something only if a discovery is active and if the search timer period has expired.
        eo_candiscovery2_Tick(eo_candiscovery2_GetHandle());
        
        // i manage the can-bus reception. i parse everything. 
        // it will be the can parser functions which will call the relevant objects which will do what they must.
        // as an example, the broadcast skin can frames are always parsed and are given to EOtheSKIN which will decide what to do with them
        eo_canserv_ParseAll(eo_canserv_GetHandle());    
#if defined(USE_ICC_COMM) 
#if defined(debugNOicc)
#else        


#if defined(useICCserviceCAN)
        embot::app::eth::icc::theICCserviceCAN::getInstance().parse(); 
#else
        // theICCservice
        embot::app::eth::icc::theICCservice::getInstance().parse(embot::app::eth::icc::theICCservice::Pipe::one); 
#endif
        
#endif
#endif
        
#if defined(TESTRTC_IS_ACTIVE)     
        testRTC_RUN_tick();
#elif defined(enableTHESERVICETESTER)
        servicetester_runtick();
#endif          
    }

    static void flushCANtransmission()
    {
        // test the TX of additional frames of 8 bytes
//        eObrd_canlocation_t loc {eOcanport1, 8, eobrd_caninsideindex_first};
//        uint8_t da[8] = {0};
//        icubCanProto_imu_config_t cc {};
//        eOcanprot_command_t cmd {eocanprot_msgclass_pollingAnalogSensor, ICUBCANPROTO_POL_AS_CMD__IMU_CONFIG_SET, {0, 0}, &cc};
//        eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &cmd, loc);
//        eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &cmd, loc);
//        eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &cmd, loc);        
        
        uint8_t txcan1frames = 0;
        uint8_t txcan2frames = 0;        
        eo_canserv_TXstartAll(eo_canserv_GetHandle(), &txcan1frames, &txcan2frames);    
        eom_emsrunner_Set_TXcanframes(eom_emsrunner_GetHandle(), txcan1frames, txcan2frames);        
    }

#if defined(USE_ICC_COMM)     
    // marco.accame on 24 nov 2023
    // for now we just flush in no blocking mode using a dummy callback
    // then we wait using bool flushed() method.
    // BUT: much better to use a callback that releases a semaphore and wait for the
    //      release of it
    
    static void flushICCtransmission()
    {
#if defined(debugNOicc)
#else        
        // theICCservice: i call the flush() with a callback so it returns immediately
        
#if defined(useICCserviceCAN)        
        constexpr embot::core::Callback donothingonflushdone {};
        embot::app::eth::icc::theICCserviceCAN::getInstance().flush(donothingonflushdone);
#else
        // theICCservice: i call the flush() with a callback so it returns immediately
        constexpr embot::core::Callback donothingonflushdone {};
        embot::app::eth::icc::theICCservice::getInstance().flush(embot::app::eth::icc::theICCservice::Pipe::one, donothingonflushdone); 
#endif
            
#endif        
    }
    
    static void waitICCisflushed()
    {
#if defined(debugNOicc)
#else
     

#if defined(useICCserviceCAN)
        if(true == embot::app::eth::icc::theICCserviceCAN::getInstance().flushed())
        {
            return;   
        }
        
        // theICCservice
        embot::core::Time expirytime {embot::core::now() + 3*embot::core::time1millisec};
        for(;;)
        {
            if(true == embot::app::eth::icc::theICCserviceCAN::getInstance().flushed())
            {
                break;
            }
            if(embot::core::now() >= expirytime)
            {
                break;
            }
        }
#else
        if(true == embot::app::eth::icc::theICCservice::getInstance().flushed(embot::app::eth::icc::theICCservice::Pipe::one))
        {
            return;   
        }
        
        // theICCservice
        embot::core::Time expirytime {embot::core::now() + 3*embot::core::time1millisec};
        for(;;)
        {
            if(true == embot::app::eth::icc::theICCservice::getInstance().flushed(embot::app::eth::icc::theICCservice::Pipe::one))
            {
                break;
            }
            if(embot::core::now() >= expirytime)
            {
                break;
            }
        }        
#endif // #if defined(useICCserviceCAN)

#endif        
    }
#endif    
    
    static void objectRUN_DO_activity()
    {
        embot::app::eth::theETHmonitor::getInstance().tick();
        
        eObool_t prevTXhadRegulars = eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle());
        
        // we tick the services ...
        // we do that with a single call embot::app::eth::theServices.tick() which ticks all the started ones.

    
        embot::app::eth::theServices::getInstance().tick(); 
        
#if defined(STM32HAL_BOARD_AMC) && defined(DEBUG_AEA3_stream_over_theBackdoor)
        embot::app::eth::theEncoderReader::getInstance().log();        
#endif        
        
        // marco.accame on 20oct2023:
        // we could start the transmission of the CAN frames at the end of the DO phase
        // in an attempt to reduce the wait time at the end of the TX phase         
#if defined(CANICCflushMODE_DO_phase)
        flushCANtransmission();
#if defined(USE_ICC_COMM)         
        flushICCtransmission();
#endif        
#endif        
    }   
    

    static void objectRUN_TX_beforeUDPtx()
    {   
#if !defined(CANICCflushMODE_DO_phase)        
        flushCANtransmission();
#if defined(USE_ICC_COMM)         
        flushICCtransmission();
#endif        
#endif        
    }    

    static void objectRUN_TX_afterUDPtx()
    {
        // it is not necessary to call any Tick() in here.
                

#if defined(USE_ICC_COMM)         
        // keep it last but one       
        waitICCisflushed();
#endif
        
        // ABSOLUTELY KEEP IT LAST: 
        // it lock the thread TX until all CAN frame has exited the board
        const eOreltime_t timeout = 3*EOK_reltime1ms;
        eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), timeout);   
    }

    static void moveto(EOsm *sm, embot::app::eth::theHandler::State st)
    {
        embot::app::eth::legacy::thehandler::eoApplEVENT applevent = embot::app::eth::legacy::thehandler::toevent(st);
        eOsmEventsEMSappl_t smevt = static_cast<eOsmEventsEMSappl_t>(applevent);  
        eo_sm_ProcessEvent(sm, smevt);        
    }        
//        
//    private:
//    ~theSM() {}        
};
    


struct embot::app::eth::theHandler::Impl
{
    constexpr static char objectname[] = "theHandler";    
    
    Config _config {};  
        
    State _state {State::IDLE};    
    
    EOsm *_sm {nullptr};

    Impl() = default; 
    
    bool initialise(const Config &cfg);    
    
    bool forcetransmission();
    bool transmit(const eOropdescriptor_t &ropdes);
    
    bool moveto(State state);
    
    State state() const { State s = static_cast<State>(eo_sm_GetActiveState(_sm)); return s; } ;
    
    bool process(Command cmd);
    
    
private:

    void redefine_errorhandler(); 

};


bool embot::app::eth::theHandler::Impl::initialise(const Config &cfg)
{
    _config = cfg;   
    _state = State::IDLE;

    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
    
    embot::app::eth::theETHmonitor::getInstance().initialise(theHandler_theETHmonitor_Config);    
    
    theCTRLsocket::init();
    _sm = eo_sm_New(eo_cfg_sm_EMSappl_Get());
    theSM::initialise(_sm);

    redefine_errorhandler();
    theCTRLsocket::set(State::IDLE);
    
    // initialise theServices      
    embot::app::eth::theServices::getInstance().initialise({});   
        
    // and then initialise each Service and load it inside theServices
    // TODO: put in some config (e.g. theHandler_theServices_Config) all the services that we want to init ....
        
    // FT
#if defined(USE_EMBOT_theServicesFT)        
    embot::app::eth::theServiceFT::getInstance().initialise({});          
    embot::app::eth::theServices::getInstance().load(embot::app::eth::theServiceFT::getInstance().service());
#endif
        
    // MC        
#if defined(USE_EMBOT_theServicesMC)
    embot::app::eth::theServiceMC::getInstance().initialise({});          
    embot::app::eth::theServices::getInstance().load(embot::app::eth::theServiceMC::getInstance().service());  
#endif
        

    // load and start the state machine        
    theSM::start(_sm);
        
    // tell the world that we have started
    theErrorManager::getInstance().emit(theErrorManager::Severity::trace, {objectname, thr}, {}, "started state machine w/ CFG, RUN, ERR");         

    return true;
} 



bool embot::app::eth::theHandler::Impl::process(Command cmd)
{   
    // this asks to the runner, configurator etc to gracefully move to a new state
    // it is the old eom_emsappl_ProcessGo2stateRequest() 

    if(Command::dummy == cmd)
    {
        return false;
    }
    
    _state = state();
    State rq = embot::app::eth::theHandler::tostate(cmd); 
    
    if(_state == rq)
    {
        // already in requested state
        return true;
    }
    
    if(_state == State::FATALERROR)
    {   // currently it is not possible to exit from error
        return false;
    }
    
    eOresult_t res {eores_NOK_generic};
     
    embot::app::eth::legacy::thehandler::eoApplEVENT applevent = embot::app::eth::legacy::thehandler::toevent(rq);
    eOsmEventsEMSappl_t smevt = static_cast<eOsmEventsEMSappl_t>(applevent);
    
    switch(_state)
    {
        case State::IDLE:
        {   // from here i can go only into RUN or FATALERROR
            res = eom_emsconfigurator_GracefulStopAndGoTo(eom_emsconfigurator_GetHandle(), smevt);
        } break;
        
        case State::RUN:
        {   // if i am here i can go only int CFG or FATALERROR. i will go smoothly to CFG because i need to end first the RX-DO-TX cycle
            res = eom_emsrunner_GracefulStopAndGoTo(eom_emsrunner_GetHandle(), smevt);     
        } break;
        
        case State::FATALERROR:
        default:
        {
            res = eores_NOK_generic;
        } break;

    }    
       
    return res == eores_OK;
}


bool embot::app::eth::theHandler::Impl::forcetransmission()
{
    return theCTRLsocket::transmissionrequest();
}


bool embot::app::eth::theHandler::Impl::transmit(const eOropdescriptor_t &ropdes)
{    
    return theCTRLsocket::transmit(ropdes); 
}


bool embot::app::eth::theHandler::Impl::moveto(State st)
{
    // this makes the sm directly evolve to a new state. 
    // it is called by EOMtheEMSrunner or EOMtheEMSconfigurator after a call to their _GracefulStopAndGoTo() methods
    // it is the old eom_emsappl_SM_ProcessEvent
    theSM::moveto(_sm, st);
    _state = state();
    return true;
}


// - in here we start all the required services.


extern void xxx_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes);


static void s_manage_dispatch(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);
static void s_manage_fatal(const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);
static void s_manage_haltrace(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *edes);

void xxx_errorman_onemit(embot::app::eth::theErrorManager::Severity sev, const embot::app::eth::theErrorManager::Caller &caller, const embot::app::eth::theErrorManager::Descriptor &des, const std::string &str)
{
    if(true == des.isvalid())
    {
        eOerrmanErrorType_t et {static_cast<eOerrmanErrorType_t>(sev)};
        eOerrmanCaller_t cl {0, caller.objectname};
        eOerrmanDescriptor_t ed {};
        ed.code = des.code;
        ed.sourcedevice = des.sourcedevice;
        ed.sourceaddress = des.sourceaddress;
        ed.par16 = des.par16;
        ed.par64 = des.par64;
            
        s_manage_haltrace(et, str.c_str(), &cl, &ed);
    
        if(embot::app::eth::theErrorManager::Severity::trace == sev) 
        {
            return;
        } 

        s_manage_dispatch(et, str.c_str(), &cl, &ed);

        if(embot::app::eth::theErrorManager::Severity::fatal == sev)
        {         
            s_manage_fatal(str.c_str(), &cl, &ed);
        }               
    }
    else
    {
        std::string timenow = embot::core::TimeFormatter(embot::core::now()).to_string();
        std::string eobjstr = (true == caller.isvalid()) ? caller.objectname : "OBJ";
        std::string threadname = (true == caller.isvalid()) ? caller.owner->getName() : "THR";
        std::string severity = embot::app::eth::theErrorManager::to_cstring(sev);
        
        embot::core::print(std::string("[[") + severity + "]] @" + timenow + " (" + eobjstr + ", " + threadname + "): " + str);  

        if(embot::app::eth::theErrorManager::Severity::trace == sev) 
        {
            return;
        } 
        
        if(embot::app::eth::theErrorManager::Severity::fatal == sev)
        {
            for(;;);
        }    
    }
    
}



#include "embot_os_rtos.h"
embot::os::rtos::mutex_t* onerrormutex {nullptr};
embot::os::rtos::semaphore_t* blockingsemaphore {nullptr};

void embot::app::eth::theHandler::Impl::redefine_errorhandler()
{
    onerrormutex = embot::os::rtos::mutex_new();
    blockingsemaphore = embot::os::rtos::semaphore_new(2, 0);
    
    eo_errman_SetOnErrorHandler(eo_errman_GetHandle(), xxx_OnError);
    
    embot::app::eth::theErrorManager::getInstance().set(xxx_errorman_onemit);


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


extern void xxx_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes)
{
    s_manage_haltrace(errtype, info, caller, errdes);
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
    const uint32_t taskid = 666; // because in embot application the caller->taskid is meaningless. much better to use the thread name

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

    
    embot::os::rtos::mutex_release(onerrormutex);   

//#endif

}

static void s_manage_fatal(const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    // i am going to error state, thus i set the correct state in eOmn_appl_status_t variable, which is used by robotInterface
    // to understand the status of the ems: cfg, run, err.
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = (eOmn_appl_status_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    status->currstate = applstate_error;
 

    eom_emserror_SetFatalError(eom_emserror_GetHandle(), des);
    embot::app::eth::theHandler::getInstance().process(embot::app::eth::theHandler::Command::go2FATALERROR);
    eom_task_SetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emserror_evt_fatalerror);    
    

    // if the caller is not the error state, then we block it execution.
    // however, we cannot block the execution of the error state otherwise ... we lose operativity
    // of error task and amongst others we lose communication with the remote host.
    // when we are in error state the remote host must be able to know it.
    
    for(;;); 
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

embot::app::eth::theHandler::State embot::app::eth::theHandler::state() const
{
    return pImpl->state();
}
 
//bool embot::app::eth::theHandler::forcetransmission()
//{
//   return pImpl->forcetransmission(); 
//}

bool embot::app::eth::theHandler::transmit(const eOropdescriptor_t &ropdes)
{
   return pImpl->transmit(ropdes); 
}

bool embot::app::eth::theHandler::moveto(State st)
{
    return pImpl->moveto(st);
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

// CFG.onentry
extern void eo_cfg_sm_EMSappl_hid_on_entry_CFG(EOsm *s)
{
    theSM::stateIDLE_onentry();
}


// RUN.onentry
extern void eo_cfg_sm_EMSappl_hid_on_entry_RUN(EOsm *s)
{
    theSM::stateRUN_onentry();
}

// ERR.onentry
extern void eo_cfg_sm_EMSappl_hid_on_entry_ERR(EOsm *s)
{
    theSM::stateFATALERROR_onentry();
}
 

// -- on exit 

// CFG.onexit
extern void eo_cfg_sm_EMSappl_hid_on_exit_CFG(EOsm *s)
{
    theSM::stateIDLE_onexit();
}  

// RUN.onexit
extern void eo_cfg_sm_EMSappl_hid_on_exit_RUN(EOsm *s)
{
    theSM::stateRUN_onexit();
}

// ERR.onexit
extern void eo_cfg_sm_EMSappl_hid_on_exit_ERR(EOsm *s)
{
    // we dont actually exit, but ...
    theSM::stateFATALERROR_onexit();
}  


// -- on trans

// CFG->RUN
extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2run(EOsm *s)
{
    // nothing to do
}  

// CFG->ERR
extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2err(EOsm *s)
{
    // nothing to do
}  

// RUN->CFG
extern void eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2cfg(EOsm *s)
{
    // nothing to do
}  

// RUN->ERR
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


// marco.accame on 15 sept 15:  the event emsconfigurator_evt_userdef01 is sent by a timer inside the EOtheCANdiscovery2 
// if we are in CFG state so that we can _Tick() it.


extern void eom_emsconfigurator_hid_userdef_ProcessUserdef01Event(EOMtheEMSconfigurator* p)
{
    theSM::objectIDLE_on_evt(emsconfigurator_evt_userdef01);
}

extern void eom_emsconfigurator_hid_userdef_ProcessUserdef02Event(EOMtheEMSconfigurator* p)
{
    theSM::objectIDLE_on_evt(emsconfigurator_evt_userdef02);
}

#if defined(USE_ICC_COMM) 
extern void eom_emsconfigurator_hid_userdef_ProcessUserdef03Event(EOMtheEMSconfigurator* p)
{
    theSM::objectIDLE_on_evt(emsconfigurator_evt_userdef03);
}
#else
extern void eom_emsconfigurator_hid_userdef_ProcessUserdef03Event(EOMtheEMSconfigurator* p) {}
#endif

#include "EoError.h"

// VERIFY: this callback is called by the EMSconfigurator both for parsing and forming of a packet 
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


#if !defined(TEST_theICCservice)
extern void eom_emsconfigurator_hid_userdef_ProcessTickEvent(EOMtheEMSconfigurator* p) {}
extern void eom_emsconfigurator_hid_userdef_ProcessTimeout(EOMtheEMSconfigurator* p) {}

#else
    
    #warning TEST_theICCservice is defined .....................................................
    
void sendicc()
{
    static constexpr embot::core::Time period {4000*embot::core::time1millisec};    
    static embot::core::Time lastcall {0};    
    embot::core::Time now = embot::core::now();
    embot::core::Time delta = now - lastcall;    
    if(delta < period)
    {
        return;
    }    
    lastcall = now;
    
    // ok, and now the rest
    
    static uint8_t n {0};
    n++;
    #warning DEBUG... remove it later on
    uint32_t ID = 0x101; // i want the sender to have 0 address .........
    embot::prot::can::Frame frame1 {ID, 8, {n, 1, 1, 1, 1, 1, 1, 1}};
    embot::app::msg::Location loc1 {embot::app::msg::BUS::icc1, 3};
    embot::prot::can::Frame frame2 {ID, 8, {n, 2, 2, 2, 2, 2, 2, 2}}; 
    embot::app::eth::icc::ItemCANframe i1 {loc1, frame1};    
    embot::app::eth::icc::theICCservice::getInstance().put(embot::app::eth::icc::theICCservice::Pipe::one, i1.item());
    embot::app::eth::icc::ItemCANframe i2 {loc1, frame2};
    embot::app::eth::icc::theICCservice::getInstance().put(embot::app::eth::icc::theICCservice::Pipe::one, i2.item());
    
    embot::core::print(embot::core::TimeFormatter(now).to_string() + ": thread tCFG will transmit some messages via ICC to the amc2c application");

    embot::app::eth::icc::theICCservice::getInstance().flush(embot::app::eth::icc::theICCservice::Pipe::one);    
}
extern void eom_emsconfigurator_hid_userdef_ProcessTickEvent(EOMtheEMSconfigurator* p) 
{
    sendicc();
}
extern void eom_emsconfigurator_hid_userdef_ProcessTimeout(EOMtheEMSconfigurator* p) 
{
    sendicc();
}
#endif

// - EOMtheEMSrunner: redefinition of userdef functions
//   formerly inside overridden_runner.c

extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    theSM::objectRUN_RX_afterUDPparsing(); 
}

#include "embot_app_eth_theServices.h"
#include "embot_app_eth_theFTservice.h"

extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    theSM::objectRUN_DO_activity();
}

extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{
    theSM::objectRUN_TX_beforeUDPtx();
}

extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    theSM::objectRUN_TX_afterUDPtx();
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
extern void eom_emserror_hid_userdef_OnRXuserdefevent(eOemserror_event_t evt)
{
    #warning TODO: add management of several things in FATALERROR state
    // they are: ethmonitor, icc rx and can rx in FATALERROR state
    // if emserror_evt_userdef01 -> can and icc rx in fatal error state
    // if emserror_evt_userdef02 -> ticker of ethmonitor
}


// other initialisation functions: MN protocol

#include "EoProtocolMN.h"    
#include "EOnv_hid.h"
    
extern void eoprot_fun_INIT_mn_comm_status(const EOnv* nv)
{
    eOmn_comm_status_t* status = (eOmn_comm_status_t*)nv->ram;
    
    // 1. init the management protocol version
    
    eOversion_t* version = &status->managementprotocolversion;
    
    version->major = eoprot_version_mn_major;
    version->minor = eoprot_version_mn_minor;
    
    
    // 2. init the transceiver
    
    eOmn_transceiver_properties_t* transp = &status->transceiver;
    
    transp->listeningPort = embot::app::eth::theHandler_EOMtheEMSsocket_Config.localport;
    transp->destinationPort = embot::app::eth::theHandler_EOMtheEMStransceiver_Config.hostipv4port;
    transp->maxsizeRXpacket = embot::app::eth::theHandler_EOMtheEMSsocket_Config.inpdatagramsizeof;
    transp->maxsizeTXpacket = embot::app::eth::theHandler_EOMtheEMSsocket_Config.outdatagramsizeof;
    transp->maxsizeROPframeRegulars = embot::app::eth::theHandler_EOMtheEMStransceiver_Config.sizes.capacityofropframeregulars;
    transp->maxsizeROPframeReplies = embot::app::eth::theHandler_EOMtheEMStransceiver_Config.sizes.capacityofropframereplies;
    transp->maxsizeROPframeOccasionals = embot::app::eth::theHandler_EOMtheEMStransceiver_Config.sizes.capacityofropframeoccasionals;
    transp->maxsizeROP = embot::app::eth::theHandler_EOMtheEMStransceiver_Config.sizes.capacityofrop;
    transp->maxnumberRegularROPs = embot::app::eth::theHandler_EOMtheEMStransceiver_Config.sizes.maxnumberofregularrops;
    memset(transp->filler06, 0, sizeof(transp->filler06));     

} 


extern void eoprot_fun_INIT_mn_appl_config(const EOnv* nv)
{
    eOmn_appl_config_t config = {0};
 
    config.cycletime =  embot::app::eth::theHandler_EOMtheEMSrunner_Config.period;
    config.txratedivider = embot::app::eth::theHandler_EOMtheEMSrunner_Config.defaultTXdecimationfactor;
       
    // set it
    eo_nv_Set(nv, &config, eobool_true, eo_nv_upd_dontdo);        
}



extern void eoprot_fun_INIT_mn_appl_status(const EOnv* nv)
{
    eOmn_appl_status_t status = {0};


    // build date
    status.buildate.year    = embot::app::eth::theApplication_Config.property.date.year;
    status.buildate.month   = embot::app::eth::theApplication_Config.property.date.month;
    status.buildate.day     = embot::app::eth::theApplication_Config.property.date.day;
    status.buildate.hour    = embot::app::eth::theApplication_Config.property.date.hour;
    status.buildate.min     = embot::app::eth::theApplication_Config.property.date.minute;
    
    // version    
    status.version.major    = embot::app::eth::theApplication_Config.property.version.major;
    status.version.minor    = embot::app::eth::theApplication_Config.property.version.minor;
    
    // control loop timings 
    status.cloop_timings[0] = embot::app::eth::theHandler_EOMtheEMSrunner_Config.execDOafter;
	status.cloop_timings[1] = embot::app::eth::theHandler_EOMtheEMSrunner_Config.execTXafter - embot::app::eth::theHandler_EOMtheEMSrunner_Config.execDOafter;
	status.cloop_timings[2] = embot::app::eth::theHandler_EOMtheEMSrunner_Config.period - embot::app::eth::theHandler_EOMtheEMSrunner_Config.execTXafter;
    status.txdecimationfactor = embot::app::eth::theHandler_EOMtheEMSrunner_Config.defaultTXdecimationfactor;
    
    // name
#if defined(STM32HAL_BOARD_AMC)
    static const char * nn[] = {"amc"};
#elif defined(STM32HAL_BOARD_AMCFOC_2CM4)
    static const char * nn[] = {"amcfoc"};
#endif
    memcpy(status.name, nn, std::min(sizeof(status.name), sizeof(nn)));
       
    // curr state
    status.currstate = applstate_config;
#if defined(STM32HAL_BOARD_AMC)
    status.boardtype = eobrd_ethtype_amc;
#elif defined(STM32HAL_BOARD_AMCFOC_2CM4)
    status.boardtype = eobrd_ethtype_amcfoc;
#endif
    
    

    // set it
    eo_nv_Set(nv, &status, eobool_true, eo_nv_upd_dontdo);
}



#if defined(TEST_ICCROP)

#define useICCserviceROP

void testiccrop()
{

//#if defined(STM32HAL_DUALCORE) && defined(CORE_CM4)
//        
//        volatile bool pinged = embot::app::eth::icc::theICCserviceROP::getInstance().ping(100*embot::core::time1millisec);

//        if(true == pinged)
//        {
//            
//            uint64_t u = embot::hw::sys::uniqueid();
//            if((embot::hw::sys::UIDinvalid == u))
//            {               
//                uint64_t uid {0};                
//                bool ok {false};
//                embot::app::eth::icc::ItemROP::Variable varUID {embot::app::eth::icc::ItemROP::IDunique64, 8, &uid};           
//                ok = embot::app::eth::icc::theICCserviceROP::getInstance().ask(varUID, 30*1000);
//                if(true == ok)
//                {
//                    embot::hw::sys::setuniqueid(uid);
//                }                
//            }
//            
//        }
//                      
//#endif

    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};


#if defined(useICCserviceROP)
        
        volatile bool pp = embot::app::eth::icc::theICCserviceROP::getInstance().ping(30*1000);
        pp = pp;   
        
        uint64_t tol {0};
        uint64_t first {0};
        volatile int64_t delta {0};
        uint32_t du32 {7};
        uint64_t uid {0};
        embot::app::icc::Signature sign {};
            
        bool ok {false};

        embot::app::eth::icc::ItemROP::Variable varTOL {embot::app::eth::icc::ItemROP::IDtimeoflife, 8, &tol};        
        embot::app::eth::icc::ItemROP::Variable varDU32 {embot::app::eth::icc::ItemROP::IDdummyUINT32, 4, &du32};
        embot::app::eth::icc::ItemROP::Variable varUID {embot::app::eth::icc::ItemROP::IDunique64, 8, &uid};
        embot::app::eth::icc::ItemROP::Variable varSIGN {embot::app::eth::icc::ItemROP::IDsignature, 16, &sign};
        
        ok = embot::app::eth::icc::theICCserviceROP::getInstance().ask(varUID, 30*1000);
        ok = ok;
        
        embot::app::eth::theErrorManager::getInstance().emit(
                embot::app::eth::theErrorManager::Severity::trace, 
                {"testiccrop()", thr}, 
                {}, 
                ok ? "ask<IDunique64> is OK = " + std::to_string(uid) : "ask<IDunique64> is KO"
        );    

 

        ok = embot::app::eth::icc::theICCserviceROP::getInstance().ask(varSIGN, 30*1000);
        ok = ok;

        embot::app::eth::theErrorManager::getInstance().emit(
                embot::app::eth::theErrorManager::Severity::trace, 
                {"testiccrop()", thr}, 
                {}, 
                ok ? "ask<IDsignature> is OK = " + sign.to_string() : "ask<IDsignature> is KO"
        ); 
                    
        du32 = du32;  

        ok = embot::app::eth::icc::theICCserviceROP::getInstance().ask(varDU32, 30*1000);
                    
        embot::app::eth::theErrorManager::getInstance().emit(
                embot::app::eth::theErrorManager::Severity::trace, 
                {"testiccrop()", thr}, 
                {}, 
                ok ? "ask<IDdummyUINT32> is OK = " + std::to_string(du32) : "ask<IDdummyUINT32> is KO"
        );  
                    
        du32 = du32;
        du32 = 1;
        ok = embot::app::eth::icc::theICCserviceROP::getInstance().set(varDU32, 30*1000);
                    
        embot::app::eth::theErrorManager::getInstance().emit(
                embot::app::eth::theErrorManager::Severity::trace, 
                {"testiccrop()", thr}, 
                {}, 
                ok ? "set<IDdummyUINT32, " + std::to_string(du32) + "> is OK" : "set<IDdummyUINT32, " + std::to_string(du32) + "> is KO"
        );  
                    
        du32 = 0; 
        ok = embot::app::eth::icc::theICCserviceROP::getInstance().ask(varDU32, 30*1000);
                    
        embot::app::eth::theErrorManager::getInstance().emit(
                embot::app::eth::theErrorManager::Severity::trace, 
                {"testiccrop()", thr}, 
                {}, 
                ok ? "ask<IDdummyUINT32> is OK = " + std::to_string(du32) : "ask<IDdummyUINT32> is KO"
        );  
                    
        du32 = du32;        
        
        volatile uint64_t n0 = embot::core::now();
        ok = embot::app::eth::icc::theICCserviceROP::getInstance().ask(varTOL, 30*1000);

        first = tol;
        
        ok = embot::app::eth::icc::theICCserviceROP::getInstance().ask(varTOL, 30*1000);
        
        volatile uint64_t n1 = embot::core::now();
        volatile int64_t d = n1 - n0;            
        delta = tol - first;
        delta = delta;
                    
        embot::app::eth::theErrorManager::getInstance().emit(
                embot::app::eth::theErrorManager::Severity::trace, 
                {"testiccrop()", thr}, 
                {}, 
                ok ? "two ask<IDtimeoflife> are OK w/ difference = " + std::to_string(delta) + " [us] and exec time = " + std::to_string(d) + " [us]" : "two ask<IDtimeoflife> are KO"
        ); 

        delta = delta;                    

#else         

#endif // useICCserviceROP

} // void testiccrop()

#endif // TEST_ICCROP 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


