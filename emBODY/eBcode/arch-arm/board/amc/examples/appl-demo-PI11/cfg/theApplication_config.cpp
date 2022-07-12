

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



#include "theApplication_config.h"
#include "embot_hw_encoder.h"
#include "embot_hw_chip_AS5045.h"
#include "embot_hw_can.h"


#define REDIRECT_ERRORMANAGER_OVER_BACKDOOR
#define ADEMO_MC_AEA

#include "embot_app_eth_theBackdoor.h"


namespace embot { namespace app { namespace eth {
    
    // user defined onidle
    void theApplication_Config_onIdle(embot::os::Thread *t, void* idleparam) 
    { 
        static volatile uint32_t cnt {0}; 
        cnt++; 
    }

    embot::hw::encoder::Config cfgEncONE {.type = embot::hw::encoder::Type::chipAS5045};
    embot::hw::ENCODER encoder_ONE {embot::hw::ENCODER::one};
//    constexpr embot::os::Event evtCAN1tx = embot::core::binary::mask::pos2mask<embot::os::Event>(0);

    embot::os::EventThread *thr {nullptr};
    
    void send(uint32_t v, uint32_t t)
    {
        static bool initted {false};

        if(false == initted)
        {
            embot::hw::can::init(embot::hw::CAN::one, {8, 2, {}, {}, {} });
            embot::hw::can::enable(embot::hw::CAN::one);        
            initted = true;        
        }

        embot::hw::can::Frame fr {};
        fr.id = 0x111;
        fr.size = 8;
        uint32_t *pv = reinterpret_cast<uint32_t*>(&fr.data[0]);
        *pv = v;
        uint32_t *pt = reinterpret_cast<uint32_t*>(&fr.data[4]);         
        *pt = t;        
        embot::hw::can::put(embot::hw::CAN::one, fr);
        embot::hw::can::transmit(embot::hw::CAN::one);   
    }
    
    void startup(embot::os::Thread *t, void *p)
    {
        embot::core::print("startup: ....");
        
        // init and start the encoder reading
        embot::hw::encoder::init(encoder_ONE, cfgEncONE);
        embot::hw::encoder::startRead(encoder_ONE);
        
        // init can
        embot::hw::can::Config canconfig {};
        embot::hw::can::init(embot::hw::CAN::one, canconfig);
        embot::hw::can::setfilters(embot::hw::CAN::one, 1);   
        embot::hw::can::enable(embot::hw::CAN::one);
    }
    
    void onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p)
    {
        if(0 == eventmask)
        {   // timeout ...
            
            uint32_t current_time = embot::core::now();
            embot::core::TimeFormatter tf(current_time);
            std::string str = "onevent: timeout @ time = " + tf.to_string();
            uint16_t posONE {0};
            
            // try to get the raw value from the hw encoder            
            if(embot::hw::resOK == embot::hw::encoder::getValue(encoder_ONE, posONE))
            {
                // send the value over CAN and start a new reading
                send(posONE, (current_time / 1000));
                str += " rawValue: " + std::to_string(posONE);
                embot::hw::encoder::startRead(encoder_ONE);
            }
            else {
                // send a fake value over CAN and start a new reading
                send(0, (current_time / 1000));
            }
            
            
            embot::core::print(str);
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


//static void s_used_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
//{
//    // this handler is called by object EOtheErrorManager which is the error handler used by the embOBJ framework
//    // we can put in here what we want. In here we use emBOT:: calls
//    
//#if 1
//    
//    std::string timenow = embot::core::TimeFormatter(embot::core::now()).to_string();
//    std::string eobjstr = (nullptr == caller) ? ("EO?") : ((nullptr == caller->eobjstr) ? ("EO?") : (std::string(caller->eobjstr)));
//    std::string infostr = (nullptr == info) ? ("no extra info") : (std::string(info));
//    std::string taskid = (nullptr == caller) ? ("0") : (std::to_string(caller->taskid));
//    std::string errortype = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
//    static volatile uint32_t counter = 0;
//    
//    if(eo_errortype_trace == errtype)
//    {
//        // if it is a simple trace we just return
//        embot::core::print(std::string("[TRACE] (") + eobjstr + std::string(" @") + timenow + ") -> " + infostr);  
//        return;            
//    }
//    else
//    {
//        // we print the error 
//        embot::core::print(std::string("[eobj: ") + eobjstr + ", tsk: " + taskid + std::string("] ") + errortype + ": " + infostr);
//        
//        // and if it is NOT fatal we return
//        if(errtype <= eo_errortype_error)
//        {
//            return;
//        }

//        // else we stay in here forever

//        for(;;)
//        {
//            counter ++;
//        }
//        
//    }
//        

//#else   
//    // but we keep embOBJ style code. just for comparison
//    const char empty[] = "EO?";
//    const char *eobjstr = (NULL == caller) ? (empty) : ((NULL == caller->eobjstr) ? (empty) : (caller->eobjstr));
//    const uint32_t taskid = (NULL == caller) ? (0) : (caller->taskid);
//    
//    char text[128];
//    uint64_t tt = eov_sys_LifeTimeGet(eov_sys_GetHandle());
//    uint32_t sec = tt/(1000*1000);
//    uint32_t tmp = tt%(1000*1000);
//    uint32_t msec = tmp / 1000;
//    uint32_t usec = tmp % 1000;    
//    
//    if(eo_errortype_trace == errtype)
//    {   // it is a trace
//        
//        if(NULL != info)
//        {
//            snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> %s.", eobjstr, sec, msec, usec, info); 
//        }
//        else
//        {
//            snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> ...", eobjstr, sec, msec, usec); 
//        }
//        hal_trace_puts(text);
//        return;            
//    }    
//    
//    snprintf(text, sizeof(text), "[eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype), info);
//    hal_trace_puts(text);

//    if(errtype <= eo_errortype_error)
//    {
//        return;
//    }

//    for(;;);
//#endif    
//}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
