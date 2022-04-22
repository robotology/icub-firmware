

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



#include "theApplication_config.h"


namespace embot { namespace app { namespace eth {
    
    // user defined onidle
    void theApplication_Config_onIdle(embot::os::Thread *t, void* idleparam) 
    { 
        static volatile uint32_t cnt {0}; 
        cnt++; 
    }

    // user defined worker called by INIT thread just before start of scheduling
    void theApplication_Config_inituserdefined(void *p)
    {
        // add what yout want
        volatile uint32_t ciao {0};
        ciao++;
        embot::core::print("hello world... you can ping me now");
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
