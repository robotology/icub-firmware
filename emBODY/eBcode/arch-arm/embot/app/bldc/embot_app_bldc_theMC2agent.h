
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_THEMC2AGENT_H_
#define __EMBOT_APP_BLDC_THEMC2AGENT_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_motor.h"

#include "embot_app_bldc_MBD_interface.h"

#include <array>
#include <vector>
#include <memory>

namespace embot::app::bldc {
           
    class theMC2agent
    {
    public:
        static theMC2agent& getInstance();
        
        static constexpr std::initializer_list<embot::hw::MOTOR> themotors {embot::hw::MOTOR::one, embot::hw::MOTOR::two};    
        static constexpr size_t numberofmotors {themotors.size()}; // {embot::hw::motor::bldc::MAXnumber};         
 
        struct Config
        {
            std::array<uint8_t, numberofmotors> address {1, 2};
            embot::app::bldc::mbd::interface::IO2 io2 {};
            
            constexpr Config() = default; 
            constexpr Config(const std::array<uint8_t, numberofmotors> &a, const embot::app::bldc::mbd::interface::IO2 &x) 
              : address(a), io2(x) {}
                  
            uint8_t getaddress(embot::hw::MOTOR m)
            {
                uint8_t i = embot::core::tointegral(m);
                return (i < numberofmotors) ? address[i] : 255;
            }
            bool isvalid() const { return (address[0]<4) && (address[1]<4) && io2.isvalid(); }
        }; 
        
        
        bool initialise(const Config &config);  
        
        struct HWinfo
        {
            bool faultpressed {false};
            float vcc {48.0};
            
            HWinfo() = default;
            HWinfo(bool fp, float v) : faultpressed(fp), vcc(v) {}
        };

        bool tick(const std::vector<embot::prot::can::Frame> &input, const HWinfo &hwinfo, std::vector<embot::prot::can::Frame> &output);
 
              
    private:
        theMC2agent(); 
        ~theMC2agent(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       


} // namespace embot::app::bldc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
