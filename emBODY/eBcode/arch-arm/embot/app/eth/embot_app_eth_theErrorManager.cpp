
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




// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------
 


struct embot::app::eth::theErrorManager::Impl
{ 
    Config _config {};    
    Impl() = default;      
    
    bool initialise(const Config &config); 
    bool trace(const std::string &str);   
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
    embot::core::print("embot::app::eth::theErrorManager::Impl::initialise()");
    return true;
}

bool embot::app::eth::theErrorManager::Impl::trace(const std::string &str)
{
    embot::core::print(str);
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

bool embot::app::eth::theErrorManager::trace(const std::string &str)
{
    return pImpl->trace(str);
}

bool embot::app::eth::theErrorManager::emit(Severity sev, const Caller &caller, const Descriptor &des, const std::string &str)
{
    return pImpl->emit(sev, caller, des, str);
}
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


