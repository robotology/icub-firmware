/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EMBOT_APP_APPLICATION_THETHERMO_H_
#define _EMBOT_APP_APPLICATION_THETHERMO_H_

#include "embot_common.h"
#include "embot_hw.h"
#include "embot_sys.h"
#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_analog_polling.h"
#include "embot_app_canprotocol_analog_periodic.h"
#include "embot_hw_si7051.h"
#include <vector>
#include <memory>

namespace embot { namespace app { namespace application {
           
    class theTHERMO
    {
    public:
        static theTHERMO& getInstance();
        
        
    public:
        struct Config
        {
            embot::hw::SI7051           sensor;
            embot::hw::si7051::Config   sensorconfig;            
            embot::common::Event        tickevent;
            embot::common::Event        datareadyevent;
            embot::sys::Task*           totask;
            Config() :  
                sensor(embot::hw::SI7051::one), 
                sensorconfig(embot::hw::si7051::Config(embot::hw::i2c::Descriptor(embot::hw::I2C::one, 400000))),  
                tickevent(0), datareadyevent(0), totask(nullptr) 
                {}
            Config(embot::hw::SI7051 _s, const embot::hw::si7051::Config& _sc, embot::common::Event _te, embot::common::Event _de, embot::sys::Task* _ts) : 
                sensor(_s),
                sensorconfig(_sc),
                tickevent(_te), 
                datareadyevent(_de), 
                totask(_ts) 
                {}    
        }; 
        
        
        bool initialise(Config &config);   

        bool start();  
        
        bool configure(embot::app::canprotocol::analog::polling::Message_THERMOMETER_CONFIG_SET::Info &info);
        bool get(embot::app::canprotocol::analog::polling::Message_THERMOMETER_CONFIG_GET::ReplyInfo &info);
        bool start(embot::common::relTime period);
 
        bool stop();        
        bool tick(std::vector<embot::hw::can::Frame> &replies);        
        bool processdata(std::vector<embot::hw::can::Frame> &replies);

    private:
        theTHERMO(); 
        ~theTHERMO(); 


    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
