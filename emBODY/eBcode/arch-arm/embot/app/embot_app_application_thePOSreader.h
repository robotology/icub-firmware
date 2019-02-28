/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_APPLICATION_THEPOSREADER_H_
#define _EMBOT_APP_APPLICATION_THEPOSREADER_H_

#include "embot_common.h"
#include "embot_hw.h"
#include "embot_sys.h"
#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_analog_polling.h"
#include "embot_app_canprotocol_analog_periodic.h"
#include "embot_hw_si7051.h"
#include <vector>
#include <array>
#include <memory>

namespace embot { namespace app { namespace application {
           
    class thePOSreader
    {
    public:
        static thePOSreader& getInstance();
        
        static constexpr std::uint8_t numberofpositions = 2;
        
        struct Sensor
        {
            embot::hw::SI7051           id;
            embot::hw::si7051::Config   config; 
            constexpr Sensor() : id(embot::hw::SI7051::none), config(embot::hw::I2C::none, 0) {}
            constexpr Sensor(embot::hw::SI7051 i, const embot::hw::si7051::Config& c) : id(i), config(c) {}
            // mettere altro costruttore etc
            constexpr bool isvalid() const { return true; }
        };
        
        struct Events
        {
            embot::common::Event                                acquire;
            std::array<embot::common::Event, numberofpositions> dataready;    
            constexpr Events() : acquire(0), dataready({0}) {}
            constexpr Events(embot::common::Event te, const std::array<embot::common::Event, numberofpositions>& re) : acquire(te), dataready(re) {}
            constexpr bool isvalid() const { if(0 == acquire) { return false; } return true; }
        }; 
        
        struct Config
        {  
            embot::sys::Task*                       owner;
            std::array<Sensor, numberofpositions>   sensors;
            Events                                  events;   
            constexpr Config() : owner(nullptr), sensors({}), events({}) {}
            constexpr Config(embot::sys::Task* ow, const std::array<Sensor, numberofpositions>& s, const Events& e) : owner(ow), sensors(s), events(e) {}             
            bool isvalid() const { return events.isvalid(); }
        }; 
        
        
        bool initialise(const Config &config);   

//        bool start();          
        bool start(embot::common::relTime period = 0);
        bool stop(); 
        
        bool process(embot::common::Event evt, std::vector<embot::hw::can::Frame> &replies);
        
        
//        bool set(embot::app::canprotocol::analog::polling::Message_THERMOMETER_CONFIG_SET::Info &info);
//        bool get(embot::app::canprotocol::analog::polling::Message_THERMOMETER_CONFIG_GET::ReplyInfo &info);
      
//        bool tick(std::vector<embot::hw::can::Frame> &replies);        
//        bool processdata(std::vector<embot::hw::can::Frame> &replies);
        


    private:
        thePOSreader(); 
        ~thePOSreader(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
