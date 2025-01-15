
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theIPnet.h"

#include "theIPnet_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheIPnet.h"
#include "embot_os.h"

#if !defined(THEIPNET_CONFIG_dontuse_theEnvironment)
#include "embot_app_eth_theEnvironment.h"
#endif

#include "embot_os_theScheduler.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------
 

struct embot::app::eth::theIPnet::Impl
{
    constexpr static char objectname[] = "theIPnet";
    
    Config _config {};
    bool _initted {false};
    IPconfig ipconfig {};
        
    Impl() = default;      
    
    bool initialise(const Config &config);
    bool isinitted() const { return _initted; }
    bool get(IPconfig &cfg);
        
};

//embot::app::eth::theIPnet::Impl::Impl()
//{
//    // we need it because ... we dont have default values for the C structs and better doing  
//}



bool embot::app::eth::theIPnet::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
    
    if(true == config.isvalid())
    {
        _config = config;
    }
    else
    {
        _config = theIPnet_Config;
    }
     
    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
    
    
    eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_MyCfg = 
    { 
        .numberofsockets = _config.sockets.numberofsockets,
        .maxdatagramenqueuedintx = _config.sockets.maxdatagramenqueuedintx
    };
    
    // start the ipnet
    eOmipnet_cfg_t eom_ipnet_Cfg = 
    { 
        .procpriority = embot::core::tointegral(_config.proc.priority),
        .procstacksize = _config.proc.stacksize, 
        .procmaxidletime = _config.maxidletime, 
        .procwakeuponrxframe = eobool_true, 
        .tickpriority = embot::core::tointegral(_config.tick.priority),
        .tickstacksize = _config.tick.stacksize
    };

    if(true == _config.ipal.useenvironment)
    {
#if !defined(THEIPNET_CONFIG_dontuse_theEnvironment)        
        embot::app::eth::theEnvironment::getInstance().get(ipconfig);
#else
        ipconfig = {_config.ipal.cfg2->eth->eth_mac, _config.ipal.cfg2->eth->eth_ip, _config.ipal.cfg2->eth->eth_mask};
#endif        
    }
    else
    {
        ipconfig = {_config.ipal.cfg2->eth->eth_mac, _config.ipal.cfg2->eth->eth_ip, _config.ipal.cfg2->eth->eth_mask};
    }

    eOmipnet_cfg_addr_t eomipnet_addr = {ipconfig.macaddr.v, ipconfig.ipaddr.v, ipconfig.ipmask.v};     
      
    ipal_cfg_any_t * ipal_cfg2use = _config.ipal.cfg2;
    
//    embot::core::print(std::string("embot::app::eth::theIPnet: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + 
//                       " starting with IP addr " + ipconfig.ipaddr.to_string() +
//                       ", IP mask " + ipconfig.ipmask.to_string() +
//                       ", MAC addr " + ipconfig.macaddr.to_string() 
//    
//    ); 
    
    theErrorManager::getInstance().emit(theErrorManager::Severity::trace, {objectname, thr}, {}, 
                        " starting with IP addr " + ipconfig.ipaddr.to_string() +
                        ", IP mask " + ipconfig.ipmask.to_string() +
                        ", MAC addr " + ipconfig.macaddr.to_string()        
        
    ); 

    // ok, we can safely initialise the IPnet
    eom_ipnet_Initialise(&eom_ipnet_Cfg,
                         ipal_cfg2use, 
                         &eomipnet_addr,
                         &eom_ipnet_dtgskt_MyCfg
                         );    

    
    _initted = true;
    return true;
}
      
//// for the ipal config we use (variable ipal_cfg2) we need:
//#include "ipal_cfg2.h"

//bool embot::app::eth::theIPnet::Impl::initialise(const Config &config)
//{
//    if(true == _initted)
//    {
//        return true;
//    }
//    
//    if(false == config.isvalid())
//    {
//        return false;
//    }
//    
//    _config = config;
//    
//    embot::core::print("embot::app::eth::theIPnet::Impl::initialise()");
//    
//    
//    constexpr eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_MyCfg = 
//    { 
//        .numberofsockets = 4,
//        .maxdatagramenqueuedintx = 4
//    };
//    
//    // start the ipnet
//    constexpr eOmipnet_cfg_t eom_ipnet_Cfg = 
//    { 
//        .procpriority = embot::core::tointegral(embot::os::Priority::high44), // default = 220 .. but we must give a different value
//        .procstacksize = 4*1024, // default = 1024 .. 
//        .procmaxidletime = 25*embot::core::time1millisec, 
//        .procwakeuponrxframe = eobool_true, 
//        .tickpriority = embot::core::tointegral(embot::os::Priority::high43), // default = 219 .. but we must give a different value
//        .tickstacksize = 4*1024 // default = 128 .. but that does not allow to run prints or else
//    };

//#if 0    
//    ipconfig = {ipal_cfg2.eth->eth_mac, ipal_cfg2.eth->eth_ip, ipal_cfg2.eth->eth_mask};
//#else
//    embot::app::eth::theEnvironment::getInstance().get(ipconfig);
//#endif    
//    eOmipnet_cfg_addr_t eomipnet_addr = {ipconfig.macaddr.v, ipconfig.ipaddr.v, ipconfig.ipmask.v};     
//      
//    ipal_cfg_any_t * ipal_cfg2use = &ipal_cfg2;
//    
//    embot::core::print(std::string("IPnet: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + 
//                       " starting with IP addr " + embot::app::eth::IPaddress(ipal_cfg2.eth->eth_ip).to_string()); 

//    // ok, we can safely initialise the IPnet
//    eom_ipnet_Initialise(&eom_ipnet_Cfg,
//                         ipal_cfg2use, 
//                         &eomipnet_addr,
//                         &eom_ipnet_dtgskt_MyCfg
//                         );    

//    
//    _initted = true;
//    return true;
//}


bool embot::app::eth::theIPnet::Impl::get(IPconfig &cfg)
{ 
    cfg = ipconfig;                         
    return true;
}

//bool embot::app::eth::theIPnet::Impl::synchronise()
//{
//    if(false == _initted)
//    {
//        return false;
//    }

//    eov_env_SharedData_Synchronise(eo_armenv_GetHandle()); 

//    return true;
//}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theIPnet& embot::app::eth::theIPnet::getInstance()
{
    static theIPnet* p = new theIPnet();
    return *p;
}

embot::app::eth::theIPnet::theIPnet()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theIPnet::~theIPnet() { }
        

bool embot::app::eth::theIPnet::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::eth::theIPnet::get(IPconfig &cfg)
{
    return pImpl->get(cfg);
}

bool embot::app::eth::theIPnet::isinitted() const
{
    return pImpl->isinitted();
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


