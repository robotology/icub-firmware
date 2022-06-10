
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServiceFT.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theFTservice.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::eth {



// finally the implementation struct
struct embot::app::eth::theServiceFT::Impl
{
    Config config {};
    bool initted {false};   
    
    // methods used by theServiceFT 
    
    Impl() = default;      
    
    eOresult_t initialise();

private:  
    

};

//embot::app::eth::theServiceFT::Impl::Impl()
//{
//    // we need it because ... we dont have default values for the C structs and better doing  
//}
      
eOresult_t embot::app::eth::theServiceFT::Impl::initialise()
{
    if(true == initted)
    {
        return eores_OK;
    }
    
    embot::core::print("embot::app::eth::theServiceFT::Impl::initialise()");
    
    embot::app::eth::theFTservice::getInstance().initialise({}); 
    
    initted = true;
    return eores_OK;
}


} // namespace embot::app::eth {

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theServiceFT& embot::app::eth::theServiceFT::getInstance()
{
    static theServiceFT* p = new theServiceFT();
    return *p;
}

embot::app::eth::theServiceFT::theServiceFT()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theServiceFT::~theServiceFT() { }
        

eOresult_t embot::app::eth::theServiceFT::initialise(const Config &config)
{
    pImpl->config = config;
    return pImpl->initialise();
}

embot::app::eth::Service::Category embot::app::eth::theServiceFT::category() const 
{
    return embot::app::eth::Service::Category::none;
}

embot::app::eth::Service::State embot::app::eth::theServiceFT::state() const
{
    return embot::app::eth::Service::State::notsupported;
}


bool embot::app::eth::theServiceFT::verify(const eOmn_serv_configuration_t * servcfg, bool activateafterverify, fpOnEndOfOperation onendoperation, void *caller)
{
    return true;
}

bool embot::app::eth::theServiceFT::activate(const eOmn_serv_configuration_t * servcfg)
{
    return true;
}


bool embot::app::eth::theServiceFT::deactivate()
{
    return true;
}

bool embot::app::eth::theServiceFT::start()
{
    return true;
}

bool embot::app::eth::theServiceFT::stop()
{
    return true;
}

bool embot::app::eth::theServiceFT::setregulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    return true;
}

bool embot::app::eth::theServiceFT::tick(bool resetstatus)
{
    return true;
}

bool embot::app::eth::theServiceFT::report()
{
    return true;
}

bool embot::app::eth::theServiceFT::process(const DescriptorCANframe &canframedescriptor)
{
    return true;
}

bool embot::app::eth::theServiceFT::process(const DescriptorROP &ropdescriptor)
{
    return true;
}


// other objects




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


