
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theMBDagent.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor.h"
#include "embot_app_theCANboardInfo.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theMBDagent::Impl
{   
    Config config {};        
    bool initted {false};
        
    embot::hw::motor::Pwm pwm {0};
    embot::prot::can::motor::polling::ControlMode cm {embot::prot::can::motor::polling::ControlMode::Idle};    
    bool applychanges {false};
    
    Impl() = default;  
    
    bool initialise();
    bool tick(std::vector<embot::prot::can::Frame> &outframes);    
};
        

bool embot::app::application::theMBDagent::Impl::initialise()
{
    if(true == initted)
    {
        return true;
    }   

    // something in here?
        
    initted = true;        
    return initted;   
}

bool embot::app::application::theMBDagent::Impl::tick(std::vector<embot::prot::can::Frame> &outframes)
{
    
    // something in here?
    
    
    return true;
}    


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------


embot::app::application::theMBDagent& embot::app::application::theMBDagent::getInstance()
{
    static theMBDagent* p = new theMBDagent();
    return *p;
}

embot::app::application::theMBDagent::theMBDagent()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theMBDagent::~theMBDagent() { }
        
bool embot::app::application::theMBDagent::initialise(const Config &config)
{
    pImpl->config = config;   
    return pImpl->initialise();
}

bool embot::app::application::theMBDagent::tick(std::vector<embot::prot::can::Frame> &outframes)
{
    return pImpl->tick(outframes);
}


// interface to CANagentMBD

bool embot::app::application::theMBDagent::get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info)
{
    embot::core::print("theMBDagent (legacy mode) received EMSTO2FOC_DESIRED_CURRENT[]: " + std::to_string(info.current[0]) + ", " + std::to_string(info.current[1]) + ", " + std::to_string(info.current[2]));
 
    pImpl->pwm = info.current[0];  
    pImpl->applychanges = true;
    
    return true;    
}


bool embot::app::application::theMBDagent::get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info)
{   
    embot::core::print("theMBDagent (legacy mode) received SET_CONTROL_MODE[]: " + embot::prot::can::motor::polling::tostring(info.controlmode) + " for motindex " + embot::prot::can::motor::polling::tostring(info.motorindex));

    pImpl->cm = info.controlmode;
    pImpl->applychanges = true;
    
    return true;    
}

bool embot::app::application::theMBDagent::get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo)
{    
    embot::core::print("theMBDagent (legacy mode) received GET_CONTROL_MODE[]: for motindex " + embot::prot::can::motor::polling::tostring(info.motorindex));
    replyinfo.motorindex = info.motorindex;
    replyinfo.controlmode =  pImpl->cm;
    return true;    
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


