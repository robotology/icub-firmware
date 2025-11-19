
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

#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"
#include "eEmemorymap.h" 

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------
 

namespace embot { namespace app { namespace eth { 

constexpr eEmoduleExtendedInfo_t eapplication_modinfo_extended2  __attribute__((section(EENV_MODULEINFO_APPLICATION_AT))) =
{
    .moduleinfo     =
    {
        .info           =
        {
            .entity     =
            {
                .type       = ee_entity_process,
                .signature  = embot::core::tointegral(theEnvironment_Config.applprop.process),
                .version    = 
                { 
                    .major = theEnvironment_Config.applprop.version.major, // theEnvironment::getconfig().applprop.version.major, 
                    .minor = theEnvironment_Config.applprop.version.minor
                },  
                .builddate  = 
                {
                    .year  = theEnvironment_Config.applprop.date.year,
                    .month = theEnvironment_Config.applprop.date.month,
                    .day   = theEnvironment_Config.applprop.date.day,
                    .hour  = theEnvironment_Config.applprop.date.hour,
                    .min   = theEnvironment_Config.applprop.date.minute
                }
            },
            .rom        = 
            {   
                .addr   = EENV_MEMMAP_EAPPLICATION_ROMADDR,
                .size   = EENV_MEMMAP_EAPPLICATION_ROMSIZE
            },
            .ram        = 
            {   
                .addr   = 0,
                .size   = 0
            },
            .storage    = 
            {
                .type   = ee_strg_none,
                .size   = 0,
                .addr   = 0
            },
            .communication  = ee_commtype_eth,  // later on we may also add can1 and can2
            .name           = "eAppAMC"
        },
        .protocols  =
        {
            .udpprotversion  = { .major = 1, .minor = 1},
            .can1protversion = { .major = 0, .minor = 0},
            .can2protversion = { .major = 0, .minor = 1},
            .gtwprotversion  = { .major = 1, .minor = 0}
        },
        .extra      = {"EXT"}
    },
    .compilationdatetime    = __DATE__ " " __TIME__,
    .userdefined            = {0}
};

}}}

constexpr uint32_t getoffset()
{
    constexpr uint32_t map[3] { EENV_MEMMAP_ELOADER_ROMADDR, EENV_MEMMAP_EUPDATER_ROMADDR, EENV_MEMMAP_EAPPLICATION_ROMADDR };    
    return map[embot::core::tointegral(embot::app::eth::theEnvironment_Config.applprop.process)];       
}

#include "embot_hw_sys.h"
extern "C" {    
    void _platform_post_lib_init(void)
    {
        embot::hw::sys::relocatevectortable(getoffset());        
    }      
}

struct embot::app::eth::theEnvironment::Impl
{
    bool _initted {false};
        
    Impl() = default;      
    
    bool initialise();
    bool get(IPconfig &cfg);
//    bool synchronise();
        
};

//embot::app::eth::theEnvironment::Impl::Impl()
//{
//    // we need it because ... we dont have default values for the C structs and better doing  
//}
      
bool embot::app::eth::theEnvironment::Impl::initialise()
{
    if(true == _initted)
    {
        return true;
    }
    
    
    embot::core::print("embot::app::eth::theEnvironment::Impl::initialise()");
    
    const eEmoduleInfo_t* info = (const eEmoduleInfo_t*)&embot::app::eth::eapplication_modinfo_extended2;
    
    constexpr eEboardInfo_t *brdinfo {nullptr}; 
    eo_armenv_Initialise(info, brdinfo);
    eov_env_SharedData_Synchronise(eo_armenv_GetHandle());
    
    _initted = true;
    return true;
}

bool embot::app::eth::theEnvironment::Impl::get(IPconfig &cfg)
{
    const eEipnetwork_t *ipnet = nullptr;
    if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
    {
        cfg.macaddr = ipnet->macaddress;
        cfg.ipaddr = ipnet->ipaddress;
        cfg.ipmask = ipnet->ipnetmask;
    }
    return true;
}

//bool embot::app::eth::theEnvironment::Impl::synchronise()
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
//bool embot::app::eth::theEnvironment::synchronise()
//{
//    return pImpl->synchronise();
//}

constexpr const embot::app::eth::theEnvironment::Config& embot::app::eth::theEnvironment::getconfig()
{      
    return theEnvironment_Config;
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


