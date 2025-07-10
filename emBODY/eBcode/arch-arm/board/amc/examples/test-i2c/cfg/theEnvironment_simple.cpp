
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theEnvironment.h"

#include "theEnvironment_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------
 



constexpr uint32_t getoffset()
{
    return 0;
//    constexpr uint32_t map[3] { EENV_MEMMAP_ELOADER_ROMADDR, EENV_MEMMAP_EUPDATER_ROMADDR, EENV_MEMMAP_EAPPLICATION_ROMADDR };    
//    return map[embot::core::tointegral(embot::app::eth::theEnvironment_Config.applprop.process)];       
}

#include "embot_hw_sys.h"
extern "C" {    
//    void _platform_post_lib_init(void)
//    {
//        embot::hw::sys::relocatevectortable(getoffset());        
//    }      
}

struct embot::app::eth::theEnvironment::Impl
{
    bool _initted {false};
        
    Impl() = default;      
    
    bool initialise();
    bool get(IPconfig &cfg);
        
};


bool embot::app::eth::theEnvironment::Impl::initialise()
{
    if(true == _initted)
    {
        return true;
    }
    
    
    embot::core::print("embot::app::eth::theEnvironment::Impl::initialise()");
    
    
    _initted = true;
    return true;
}

bool embot::app::eth::theEnvironment::Impl::get(IPconfig &cfg)
{
//    const eEipnetwork_t *ipnet = nullptr;
//    if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
//    {
//        cfg.macaddr = ipnet->macaddress;
//        cfg.ipaddr = ipnet->ipaddress;
//        cfg.ipmask = ipnet->ipnetmask;
//    }
    
    IPconfig def {};
    cfg.macaddr = def.macaddr;
    cfg.ipaddr = def.ipaddr;
    cfg.ipmask = def.ipmask;
    return true;
}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theEnvironment& embot::app::eth::theEnvironment::getInstance()
{
    static theEnvironment* p = new theEnvironment();
    return *p;
}

embot::app::eth::theEnvironment::theEnvironment()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theEnvironment::~theEnvironment() { }
        

bool embot::app::eth::theEnvironment::initialise()
{
    return pImpl->initialise();
}

bool embot::app::eth::theEnvironment::get(IPconfig &cfg)
{
    return pImpl->get(cfg);
}


constexpr const embot::app::eth::theEnvironment::Config& embot::app::eth::theEnvironment::getconfig()
{      
    return theEnvironment_Config;
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


