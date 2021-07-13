
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

#include "embot_hw_motor.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theMCagent::Impl
{   
    Config config {};
        
    embot::hw::motor::Pwm pwm {0};
    embot::prot::can::motor::polling::ControlMode cm {embot::prot::can::motor::polling::ControlMode::Idle};    
    
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
 
    pImpl->pwm = info.current[0];  
    if(embot::prot::can::motor::polling::ControlMode::Idle != pImpl->cm)
    {
        embot::hw::motor::setpwm(embot::hw::MOTOR::one, pImpl->pwm);
    }
    return true;    
}


bool embot::app::application::theMCagent::get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info)
{   
    embot::core::print("received SET_CONTROL_MODE[]: " + embot::prot::can::motor::polling::tostring(info.controlmode) + " for motindex " + embot::prot::can::motor::polling::tostring(info.motorindex));

    pImpl->cm = info.controlmode;
    
    if(embot::prot::can::motor::polling::ControlMode::Idle == pImpl->cm)
    {
        pImpl->pwm = 0;
        embot::hw::motor::setpwm(embot::hw::MOTOR::one, 0);
    }
    return true;    
}

bool embot::app::application::theMCagent::get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo)
{    
    embot::core::print("received GET_CONTROL_MODE[]: for motindex " + embot::prot::can::motor::polling::tostring(info.motorindex));
    replyinfo.motorindex = info.motorindex;
    replyinfo.controlmode =  pImpl->cm;
    return true;    
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


