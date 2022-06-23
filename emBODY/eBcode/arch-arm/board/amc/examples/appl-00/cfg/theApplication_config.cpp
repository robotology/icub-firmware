

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



#include "theApplication_config.h"

//#define REDIRECT_ERRORMANAGER_OVER_BACKDOOR

#include "embot_app_eth_theBackdoor.h"


namespace embot { namespace app { namespace eth {
    
    // user defined onidle
    void theApplication_Config_onIdle(embot::os::Thread *t, void* idleparam) 
    { 
        static volatile uint32_t cnt {0}; 
        cnt++; 
    }

    
    embot::os::EventThread *thr {nullptr};
    
    void startup(embot::os::Thread *t, void *p)
    {
        embot::core::print("startup: ....");
    }
    
    void onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p)
    {
        if(0 == eventmask)
        {   // timeout ...   

            embot::core::TimeFormatter tf(embot::core::now());        
            std::string str = "onevent: timeout @ time = " + tf.to_string();               
            //embot::core::print(str);
                    
            embot::app::eth::emit(sevINFO, {"testThread", t}, {}, str);
            
            return;
        }

        // eval other events
        
    }
    
    void tTEST(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }
    
    void emit_over_backdoor(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str);
    
    void bkdoor_onrx(EOpacket *rxp)
    {
        uint8_t *data {nullptr};
        uint16_t size {0};
        eo_packet_Payload_Get(rxp, &data, &size);
        embot::core::print("theBackdoor-> " + std::string(reinterpret_cast<char*>(data)));                       
    }
    
    
    void addservice();
    
    // user defined worker called by INIT thread just before start of scheduling
    void theApplication_Config_inituserdefined(void *p)
    {
        // add what yout want
        volatile uint32_t ciao {0};
        ciao++;
        embot::core::print("hello world... you can ping me now");
        
        
        // in here i start whatever i want
        
        // for example: 
        // - the object theBackdoor
        // - an EventThread periodically activated by its timeout

        // add the backdoor
        constexpr embot::app::eth::theBackdoor::Config bkdconfig =
        {
            { embot::os::Priority::belownorm23, 2*1024 },   // thread
            { {2, 64, 2, 512}, 6666 },                      // socket.size, socket.localport
            {embot::app::eth::IPlocalhost, 6666},           // hostaddress 
            bkdoor_onrx,                                    // onreception does ... nothing so far          
        };
        
        embot::app::eth::theBackdoor::getInstance().initialise(bkdconfig);


        constexpr embot::core::relTime timeout {5*1000*embot::core::time1millisec};

        embot::os::EventThread::Config tCfg { 
            6*1024, 
            embot::os::Priority::belownorm23, 
            startup, nullptr,
            timeout,
            onevent,
            "tTEST"
        };
           
            
        // create the test thread 
        thr = new embot::os::EventThread;          
        // and start it
        thr->start(tCfg, tTEST);
        
#if defined(REDIRECT_ERRORMANAGER_OVER_BACKDOOR)        
        // now link theErrorManager to send onto the theBackdoor
        embot::app::eth::theErrorManager::getInstance().set(emit_over_backdoor);
#endif
        
        embot::app::eth::addservice();
        
    }
    
    void theApplication_Config_errorman_onemit(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str)
    {
        std::string timenow = embot::core::TimeFormatter(embot::core::now()).to_string();
        std::string eobjstr = (true == caller.isvalid()) ? caller.objectname : "OBJ";
        std::string threadname = (true == caller.isvalid()) ? caller.owner->getName() : "THR";
        std::string severity = theErrorManager::to_cstring(sev);
        
        embot::core::print(std::string("[") + severity + "] @" + timenow + " (" + eobjstr + ", " + threadname + "): " + str);
        
        if(theErrorManager::Severity::trace == sev) 
        {
            return;
        } 

        // you may in here send the diagnostics message
        if(true == des.isvalid())
        {
            
        }
      
        if(theErrorManager::Severity::fatal == sev)
        {
            for(;;);
        }        
        
    }
    
    void emit_over_backdoor(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str)
    {
        // form a string and then send it to the ...

        std::string timenow = embot::core::TimeFormatter(embot::core::now()).to_string();
        std::string eobjstr = (true == caller.isvalid()) ? caller.objectname : "OBJ";
        std::string threadname = (true == caller.isvalid()) ? caller.owner->getName() : "THR";
        std::string severity = theErrorManager::to_cstring(sev);
        
        std::string out = std::string("[") + severity + "] @" + timenow + " (" + eobjstr + ", " + threadname + "): " + str + "\n";
        
        static EOpacket *pkt {nullptr};
        
        if(nullptr == pkt)
        {
            pkt = eo_packet_New(500); // max size
        }
        
        uint8_t *data = reinterpret_cast<uint8_t *>(const_cast<char*>(out.c_str()));
        eo_packet_Payload_Set(pkt, data, strlen(out.c_str()));
        eo_packet_Addressing_Set(pkt, EO_COMMON_IPV4ADDR(10, 0, 1, 104), 6666);
        
        embot::app::eth::theBackdoor::getInstance().transmit(pkt);
      
//        if(theErrorManager::Severity::fatal == sev)
//        {
//            for(;;);
//        }            
        
    }
    
            
}}}

//#include "embot_app_eth_theServices.h"
#include "embot_app_eth_theHandler.h"

namespace embot { namespace app { namespace eth {
    

//void init_ctrl_socket();
//void init_ctrl_transceiver();
//void init_configurator();
//void init_runner();
//void init_err();
//void init_sm();

void addservice()
{ 
    embot::app::eth::theHandler::getInstance().initialise({});
        
////    init_ctrl_socket();
//    init_ctrl_transceiver();
//    init_configurator();
//    init_runner();
//    init_err();
//    init_sm(); 
//    embot::app::eth::theServices::getInstance().initialise({});    
}


//#include "EOMtheEMSsocket.h"
//void init_ctrl_socket()
//{
//   
//    eOemssocket_cfg_t socketcfg {}; // to be specified
//        
//    eom_emssocket_Initialise(&socketcfg);   

//    eOipv4addr_t hostipv4addr {0};

//    // i try connection now, so that if the hostaddress does not change, then during transmission we dont do a connect anymore
//    eOresult_t res = eom_emssocket_Connect(eom_emssocket_GetHandle(), hostipv4addr, 5*EOK_reltime100ms); 
//}

//#include "EOMtheEMStransceiver.h"
//void init_ctrl_transceiver()
//{
//    eOemstransceiver_cfg_t transcfg;  // to be specified
//    eom_emstransceiver_Initialise(&transcfg);   

//    eOipv4addr_t hostipv4addr {0}; // to be specified

//    // i try connection now, so that if the hostaddress does not change, then during transmission we dont do a connect anymore
//    eOresult_t res = eom_emssocket_Connect(eom_emssocket_GetHandle(), hostipv4addr, 5*EOK_reltime100ms);

////    
////    if(eores_OK == res)
////    {
////        // set led0 to ON
////       eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
////    }
////    else
////    {
////        eo_ledpulser_Off(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
////    }
////    
////    snprintf(str, sizeof(str), "eom_emssocket_Connect is over. Link w/ host %s is %s", ipv4str, (eores_OK == res) ? "ON" : "OFF");
////    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);   
//}


//#include "EOMtheEMSconfigurator.h"
//void init_configurator()
//{
//    eOemsconfigurator_cfg_t cfgobjcfg {};
//    eom_emsconfigurator_Initialise(&cfgobjcfg);
//}


//#include "EOMtheEMSrunner.h"
//void init_runner()
//{
//    eOemsrunner_cfg_t cfgobjrun {};
//    eom_emsrunner_Initialise(&cfgobjrun);
//}

//#include "EOMtheEMSerror.h"
//void init_err()
//{
//    eOemserror_cfg_t cfgobj {};
//    eom_emserror_Initialise(&cfgobj);
//}

//#include "EOsm.h"
//#include "eOcfg_sm_EMSappl.h"
//EOsm *thesm {nullptr};

//void init_sm()
//{
//    thesm = eo_sm_New(eo_cfg_sm_EMSappl_Get());
//    eo_sm_Start(thesm);
//}

}}} // namespace embot { namespace app { namespace eth {

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
