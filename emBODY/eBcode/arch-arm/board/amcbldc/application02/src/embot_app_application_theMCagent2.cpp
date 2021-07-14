
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theMCagent2.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor.h"

#include "embot_app_theCANboardInfo.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theMCagent2::Impl
{   
    Config config {};
        
    embot::hw::motor::Pwm pwm {0};
    embot::prot::can::motor::polling::ControlMode cm {embot::prot::can::motor::polling::ControlMode::Idle};    
    
    Impl() = default;  

    bool tick(std::vector<embot::prot::can::Frame> &outframes)
    {
        if(embot::prot::can::motor::polling::ControlMode::Idle != cm)
        {
            // add status of motor...
 
            embot::prot::can::Frame frame {};   
                            
            embot::prot::can::motor::periodic::Message_FOC msg;
            embot::prot::can::motor::periodic::Message_FOC::Info info;  

            
            info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();   
            info.current = pwm;
            info.velocity = 1; 
            info.position = 2;                
            
            msg.load(info);
            msg.get(frame);
            outframes.push_back(frame);              
        }
        return true;
    }    
};
        




// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------


embot::app::application::theMCagent2& embot::app::application::theMCagent2::getInstance()
{
    static theMCagent2* p = new theMCagent2();
    return *p;
}

embot::app::application::theMCagent2::theMCagent2()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theMCagent2::~theMCagent2() { }
        
bool embot::app::application::theMCagent2::initialise(const Config &config)
{
    pImpl->config = config;
    return true;
}

bool embot::app::application::theMCagent2::tick(std::vector<embot::prot::can::Frame> &outframes)
{
    return pImpl->tick(outframes);
}


// interface to CANagentMC

bool embot::app::application::theMCagent2::get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info)
{
    embot::core::print("received EMSTO2FOC_DESIRED_CURRENT[]: " + std::to_string(info.current[0]) + ", " + std::to_string(info.current[1]) + ", " + std::to_string(info.current[2]));
 
    pImpl->pwm = info.current[0];  
    if(embot::prot::can::motor::polling::ControlMode::Idle != pImpl->cm)
    {
        embot::hw::motor::setpwm(embot::hw::MOTOR::one, pImpl->pwm);
    }
    return true;    
}


bool embot::app::application::theMCagent2::get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info)
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

bool embot::app::application::theMCagent2::get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo)
{    
    embot::core::print("received GET_CONTROL_MODE[]: for motindex " + embot::prot::can::motor::polling::tostring(info.motorindex));
    replyinfo.motorindex = info.motorindex;
    replyinfo.controlmode =  pImpl->cm;
    return true;    
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


