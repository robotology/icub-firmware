
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theMCagent.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theMCagent::Impl
{   
    Config config {};
        
    Impl() = default;        
};
        




// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------


embot::app::application::theMCagent& embot::app::application::theMCagent::getInstance()
{
    static theMCagent* p = new theMCagent();
    return *p;
}

embot::app::application::theMCagent::theMCagent()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theMCagent::~theMCagent() { }
        
bool embot::app::application::theMCagent::initialise(const Config &config)
{
    pImpl->config = config;
    return true;
}

// interface to CANagentMC

bool embot::app::application::theMCagent::get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info)
{
    embot::core::print("received EMSTO2FOC_DESIRED_CURRENT[]: " + std::to_string(info.current[0]) + ", " + std::to_string(info.current[1]) + ", " + std::to_string(info.current[2]));
    return true;    
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


