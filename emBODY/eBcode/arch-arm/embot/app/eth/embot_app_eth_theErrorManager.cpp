
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theErrorManager.h"
#include "theErrorManager_config.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "embot_os_rtos.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------
 


struct embot::app::eth::theErrorManager::Impl
{ 
    Config _config {};    
    embot::os::rtos::mutex_t *mtx {nullptr};
    Impl() = default;      
    
    bool initialise(const Config &config); 
    bool trace(const std::string &str, const Caller &caller);   
    bool emit(Severity sev, const Caller &caller, const Descriptor &des, const std::string &str);    
    static void onemitdummy(Severity sev, const Caller &caller, const Descriptor &des, const std::string &str)
    {
        embot::core::print("theErrorManager::onemitdummy() -> " + str);
    }        
};

      
bool embot::app::eth::theErrorManager::Impl::initialise(const Config &config)
{  
    if(config.isvalid())
    {
        _config = config;
    }
    else
    {
        _config = theErrorManager_Config;
    }
    
    if(false == _config.isvalid())
    {
        _config.onemit = onemitdummy;
    }
    // create the mutex
    mtx = embot::os::rtos::mutex_new();
    return true;
}


bool embot::app::eth::theErrorManager::Impl::trace(const std::string &str, const Caller &caller)
{
    embot::core::TimeFormatter tf(embot::core::now());
    embot::core::print(caller.to_string() + " @" + tf.to_string() + ": " + str);
    return true;
}

bool embot::app::eth::theErrorManager::Impl::emit(Severity sev, const Caller &caller, const Descriptor &des, const std::string &str)
{
    _config.onemit(sev, caller, des, str);
    if(Severity::fatal == sev)
    {
        static volatile uint8_t xxx {0};
        for(;;)
        {
            xxx++;
        }
    }
    return true;
}

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theErrorManager& embot::app::eth::theErrorManager::getInstance()
{
    static theErrorManager* p = new theErrorManager();
    return *p;
}

embot::app::eth::theErrorManager::theErrorManager()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theErrorManager::~theErrorManager() { }
        

bool embot::app::eth::theErrorManager::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::eth::theErrorManager::trace(const std::string &str, const Caller &caller)
{
    return pImpl->trace(str, caller);
}

bool embot::app::eth::theErrorManager::emit(Severity sev, const Caller &caller, const Descriptor &des, const std::string &str)
{
    return pImpl->emit(sev, caller, des, str);
}


bool embot::app::eth::theErrorManager::set(fpOnEmit onemit)
{
    if(nullptr == onemit)
    {
        return false;
    }
    
    // a mutex is required in here to avoid damage
    embot::os::rtos::mutex_take(pImpl->mtx, embot::core::reltimeWaitForever);
    pImpl->_config.onemit = onemit;
    embot::os::rtos::mutex_release(pImpl->mtx);
    
    return true;    
}

bool embot::app::eth::emit(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str)    
{
    return embot::app::eth::theErrorManager::getInstance().emit(sev, caller, des, str);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


