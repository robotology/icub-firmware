

/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_APPLICATION_THECANPARSERMC2_H_
#define _EMBOT_APP_APPLICATION_THECANPARSERMC2_H_


#include <vector>
#include <memory>

#include "embot_app_application_CANagentMC2.h"

#include "embot_hw_motor.h"

namespace embot::app::application {
    
           
    class theCANparserMC2
    {
    public:
        static theCANparserMC2& getInstance();
        
        
    public:
        
        static constexpr std::initializer_list<embot::hw::MOTOR> themotors {embot::hw::MOTOR::one, embot::hw::MOTOR::two};    
        static constexpr size_t numberofmotors {themotors.size()};       
        
        struct Config
        {
            CANagentMC2* agent {nullptr};
            std::array<uint8_t, numberofmotors> address {1, 2};
            Config() = default;
            constexpr Config(CANagentMC2* agt, const std::array<uint8_t, numberofmotors> adr) : agent(agt), address(adr) {}
            bool isvalid() const { return (nullptr != agent) && (address[0]<4) && (address[1]<4); }
            uint8_t getaddress(embot::hw::MOTOR m)
            {
                uint8_t i = embot::core::tointegral(m);
                return (i < numberofmotors) ? address[i] : 255;
            }            
        }; 
        
        
        bool initialise(const Config &config); 
        
        
        enum class FILTER : uint8_t 
        { 
            doALL = 0,      // all messages that set<x> or get<x> 
            dontGET = 1,    // filters away the messages that get<x>, so ... allows only the set<x>
            onlyGET = 2     // allows only the get<x>
        }; 
        
        struct Result
        {
            bool recognised {false};
            bool processed {false};
        }; 
        // it processes the input frame on the basis of the chosen filter.  
        bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies, Result &result, FILTER filter = FILTER::doALL);
        

    private:
        theCANparserMC2(); 
        ~theCANparserMC2(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    };       


} // namespace embot::app::application {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
