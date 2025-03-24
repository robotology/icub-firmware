
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theApplication.h"
#include "theApplication_config.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os.h"

#include "embot_app_eth_theErrorManager.h"
#include "embot_app_eth_theEnvironment.h"
#include "embot_app_eth_theIPnet.h"
#include "embot_app_eth_theListener.h"
#include "embot_app_eth_theBackdoor.h"

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

#include "embot_os_eom.h"

#if defined(USE_ICC_COMM) 
#include "embot_app_eth_theICCservice.h"
#include "embot_app_eth_theICCserviceROP.h"
#include "embot_app_eth_theICCserviceCAN.h"
#include "embot_os_rtos.h"
#endif

#include "embot_hw_sys.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------
 


struct embot::app::eth::theApplication::Impl
{
    constexpr static char objectname[] = "theApplication"; 
    
    Impl() = default;         
    [[noreturn]] void start(const Config &config);   

    static void embotOSerror(void *p);    
    static void initSystem(embot::os::Thread *t, void* initparam);
    
    static constexpr embot::os::InitThread::Config initcfg = { embot::app::eth::theApplication_Config.OSstacksizeinit, initSystem, nullptr };
    static constexpr embot::os::IdleThread::Config idlecfg = { embot::app::eth::theApplication_Config.OSstacksizeidle, nullptr, nullptr, embot::app::eth::theApplication_Config.OSonidle };
    static constexpr embot::os::Config osconfig {embot::app::eth::theApplication_Config.OStick, initcfg, idlecfg, {embotOSerror, nullptr}};
    
    static void s_used_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);
    static constexpr embot::os::EOM::Config eomcfg {{s_used_errman_OnError}};  

    static uint64_t synchUID()
    {                
        uint64_t uid = embot::hw::sys::uniqueid();
#if defined(STM32HAL_BOARD_AMCFOC_2CM4)            
        if((embot::hw::sys::UIDinvalid == uid))
        {            
            embot::app::eth::icc::ItemROP::Variable varUID {embot::app::eth::icc::ItemROP::IDunique64, 8, &uid};           
            bool ok = embot::app::eth::icc::theICCserviceROP::getInstance().ask(varUID, 30*1000);
            if(true == ok)
            {
                embot::hw::sys::setuniqueid(uid);
            } 
            uid = embot::hw::sys::uniqueid();            
        }
#endif
        return uid;       
    }       
};

      
[[noreturn]] void embot::app::eth::theApplication::Impl::start(const Config &config)
{   
    embot::core::print(std::string(objectname) + "::Impl::start()");
    
    embot::os::init(osconfig); 
    embot::os::start();
}


void embot::app::eth::theApplication::Impl::initSystem(embot::os::Thread *t, void* initparam)
{        
    embot::os::theTimerManager::getInstance().start(embot::app::eth::theApplication_Config.TMRMANconfig);     
    embot::os::theCallbackManager::getInstance().start(embot::app::eth::theApplication_Config.CBKMANconfig);  
        
    embot::app::eth::theErrorManager::getInstance().initialise(embot::app::eth::theApplication_Config.ERRMANconfig);
        
    embot::app::theLEDmanager::getInstance().init(embot::app::eth::theApplication_Config.allLEDs);
    embot::app::theLEDmanager::getInstance().get(embot::app::eth::theApplication_Config.pulseLED).pulse(embot::app::eth::theApplication_Config.pulseFREQ, 0);

    
    // -- icc starts in here. we are in the tINIT, so we must somehow let the threads of the ICC run
    //    so, we shall put the thread to sleep for a bit and wait some more time so that the other core can be activated  
    embot::app::eth::icc::theICCservice::getInstance().initialise(embot::app::eth::icc::iccmastercfg);  
    embot::app::eth::icc::theICCserviceROP::getInstance().initialise({});
#if defined(useICCserviceCAN)
    embot::app::eth::icc::theICCserviceCAN::getInstance().initialise({});
#endif        

    embot::os::rtos::thread_sleep( embot::os::rtos::scheduler_thread_running(), 20*embot::core::time1millisec);        
    embot::core::wait(10*embot::core::time1millisec);
       
    volatile bool pinged {false};

    pinged = embot::app::eth::icc::theICCserviceROP::getInstance().ping(100*embot::core::time1millisec);

    theErrorManager::getInstance().emit(theErrorManager::Severity::trace, {objectname, t}, {}, pinged ? "the other core replies to ICC ping" : "the other core does not ack to ICC ping");    

    {
        embot::app::icc::Signature sign {};
        embot::app::eth::icc::ItemROP::Variable varSIGN {embot::app::eth::icc::ItemROP::IDsignature, 16, &sign};
        
        bool ok = embot::app::eth::icc::theICCserviceROP::getInstance().ask(varSIGN, 30*1000);
        embot::app::eth::theErrorManager::getInstance().emit(
                embot::app::eth::theErrorManager::Severity::trace, 
                {objectname, t}, {}, 
                ok ? "the other core has signature = " + sign.to_string() : "ask<IDsignature> to other core is KO"
        );         
    }
        
#if defined(STM32HAL_BOARD_AMCFOC_2CM4)    
    if(true == pinged)
    {                
        uint64_t uid = embot::app::eth::theApplication::Impl::synchUID(); 
        theErrorManager::getInstance().emit(theErrorManager::Severity::trace, {objectname, t}, {}, "UID is synched");            
    }  
#endif       
    // this starts initialization of legacy and usefuls EOM services to use embot objects:
    // EOVtheCallbackManager, EOVtheTimerManager, EOVtheSystem, ...
    embot::os::EOM::initialise(eomcfg);  
    
    // ok, now you can run whatever you want ... w/ EOM objects or their wrappers
    
    embot::app::eth::theEnvironment::getInstance().initialise();
    embot::app::eth::theIPnet::getInstance().initialise(embot::app::eth::theApplication_Config.IPNETconfig);
    embot::app::eth::theListener::getInstance().initialise(embot::app::eth::theApplication_Config.LISTENERconfig);       

#if defined(STM32HAL_BOARD_AMC) && defined(DEBUG_AEA3_stream_over_theBackdoor)    
    // start theBackdoor
    embot::app::eth::theBackdoor::getInstance().initialise({}); 
#endif        
    // and now we can launch whatever else we want
        
    embot::app::eth::theApplication_Config.OSuserdefinit.execute();
    
    theErrorManager::getInstance().emit(theErrorManager::Severity::trace, {objectname, t}, {}, "quitting the INIT thread. Normal scheduling starts");    
}


void embot::app::eth::theApplication::Impl::embotOSerror(void *p)
{
    embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::fatal, {objectname, nullptr}, {}, "embot::os::error");    
}


void embot::app::eth::theApplication::Impl::s_used_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    constexpr embot::app::eth::theErrorManager::Severity etypemap[] =
    {
        embot::app::eth::theErrorManager::Severity::trace,
        embot::app::eth::theErrorManager::Severity::info,
        embot::app::eth::theErrorManager::Severity::debug,
        embot::app::eth::theErrorManager::Severity::warning,
        embot::app::eth::theErrorManager::Severity::error,
        embot::app::eth::theErrorManager::Severity::fatal
    };
    
    embot::app::eth::theErrorManager::Severity s = (errtype<eo_errortype_numberof) ? etypemap[errtype] : embot::app::eth::theErrorManager::Severity::debug;
    embot::app::eth::theErrorManager::Descriptor desc {}; 
    if(nullptr != des)
    {
        desc.code = des->code;
        desc.sourceaddress = des->sourceaddress;
        desc.sourcedevice = des->sourcedevice;
        desc.par64 = des->par64;
        desc.par16 = des->par16;
    }   

    embot::app::eth::theErrorManager::Caller cllr {};
    if((nullptr != caller) && (caller->eobjstr != nullptr))
    {
        cllr.objectname = caller->eobjstr;
        cllr.owner = embot::os::theScheduler::getInstance().scheduled();
    }        
    
    embot::app::eth::theErrorManager::getInstance().emit(s, cllr, desc, (nullptr == info) ? "" : info);
    
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
}

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

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theApplication& embot::app::eth::theApplication::getInstance()
{
    static theApplication* p = new theApplication();
    return *p;
}

embot::app::eth::theApplication::theApplication()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theApplication::~theApplication() { }
        

[[noreturn]] void embot::app::eth::theApplication::start(const Config &config)
{
    pImpl->start(config);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


