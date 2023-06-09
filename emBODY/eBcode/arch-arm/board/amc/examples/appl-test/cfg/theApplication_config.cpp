

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



#include "theApplication_config.h"

#define REDIRECT_ERRORMANAGER_OVER_BACKDOOR

#include "embot_app_eth_theBackdoor.h"

namespace amc {
    
    void tTEST_startup(embot::os::Thread *t, void *p);    
    void tTEST_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p);
};

namespace embot { namespace app { namespace eth {
    
    // user defined onidle
    void theApplication_Config_onIdle(embot::os::Thread *t, void* idleparam) 
    { 
        static volatile uint32_t cnt {0}; 
        cnt++; 
    }
    
    embot::os::EventThread *thr {nullptr};
    
    
    void tTEST(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }
    
    void emit_over_backdoor(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str);
    
    void bkdoor_onrx(EOpacket *rxp)
    {
        uint8_t *data {nullptr};
        uint16_t size {0};
        eo_packet_Payload_Get(rxp, &data, &size);
        embot::core::print("theBackdoor-> " + std::string(reinterpret_cast<char*>(data)));                       
    }
    
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


        //constexpr embot::core::relTime timeout {5*1000*embot::core::time1millisec};
        constexpr embot::core::relTime timeout {100*embot::core::time1millisec};

        embot::os::EventThread::Config tCfg { 
            6*1024, 
            embot::os::Priority::belownorm23, 
            amc::tTEST_startup, nullptr,
            timeout,
            amc::tTEST_onevent,
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
       
    }    
            
}}}


#include "embot_core.h"
#include "stm32hal.h"
#include "embot_hw_icc_sig.h"
#include "embot_hw_icc_mem.h"
#include "embot_hw_icc_ltr.h"

namespace amc {
    

    
    void tTEST_startup(embot::os::Thread *t, void *p)
    {
        embot::core::print("startup: ....");
        
//        embot::hw::icc::sig::test::init(embot::hw::icc::DIR::rx);
        
        // explicity unlock mutex 0, so that an app on cm4 can start.
        __HAL_RCC_HSEM_CLK_ENABLE();
        HAL_HSEM_Release(0, 0);  
        
//        embot::hw::icc::ltr::test::init(embot::hw::icc::DIR::rx);
        
        embot::core::TimeFormatter tf(embot::core::now());        
        std::string str = "@ time = " + tf.to_string();               
        embot::core::print(str);
        
        
        embot::core::TimeFormatter tf1(embot::core::now());        
        std::string str1 = "@ time = " + tf.to_string();               
        embot::core::print(str1);        
    }
    
    void tTEST_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p)
    {
        if(0 == eventmask)
        {   // timeout ...   
            
            static bool initted {false};
            
            if(!initted)
            {
//                embot::hw::icc::sig::test::init(embot::hw::icc::DIR::txrx);
//                embot::hw::icc::mem::test::init(embot::hw::icc::DIR::tx);
                embot::hw::icc::ltr::test::init(embot::hw::icc::DIR::rx);
                initted = true;
            }

            uint64_t t0 {embot::core::now()};
            embot::core::TimeFormatter tf(t0);        
            std::string str = "onevent: timeout @ time = " + tf.to_string();               
//            embot::core::print(str);
            
            uint64_t t00 {embot::core::now()};
            
//            embot::hw::icc::sig::test::tick();
            
            // actually not required
            //embot::hw::icc::ltr::test::tick("");
            
//            uint64_t t1 {embot::core::now()};
//            embot::core::TimeFormatter tf1(t1-t00);        
//            std::string str1 = "unlocked after " + tf1.to_string();               
//            embot::core::print(str1);            
                    
//            embot::app::eth::emit(embot::app::eth::sevINFO, {"testThread", t}, {}, str);
            
            return;
        }
        
        
        

        // eval other events
        
    }

} // namespace amc {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
