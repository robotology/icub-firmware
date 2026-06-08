

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



#include "theApplication_config.h"


#include "embot_app_eth_theBackdoor.h"

#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"


namespace embot { namespace app { namespace eth {
    
    // user defined onidle
    void theApplication_Config_onIdle(embot::os::Thread *t, void* idleparam) 
    { 
        static volatile uint32_t cnt {0}; 
        cnt++; 
    }

    
    
    void emit_over_backdoor(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str);
    
    void addservice();
    
    // user defined worker called by INIT thread just before start of scheduling
    void theApplication_Config_inituserdefined(void *p)
    {
        // add what yout want
        volatile uint32_t ciao {0};
        ciao++;
        
        
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};            
        embot::app::eth::theErrorManager::getInstance().trace("calling embot::app::eth::addservice()", {"theApplication_Config_inituserdefined()", thr});         
        
        
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
    
            
}}}


#include "embot_app_eth_theHandler.h"

namespace embot { namespace app { namespace eth {
    


void addservice()
{ 
    embot::app::eth::theHandler::getInstance().initialise({}); 
}


}}} // namespace embot { namespace app { namespace eth {

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
