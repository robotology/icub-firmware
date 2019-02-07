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

#ifndef _EMBOT_APP_APPLICATION_THEIMU_H_
#define _EMBOT_APP_APPLICATION_THEIMU_H_

#include "embot_common.h"

#include "embot_hw.h"

#include "embot_sys.h"

#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_analog_polling.h"
#include "embot_app_canprotocol_analog_periodic.h"

#include "embot_hw_bno055.h"


#include <vector>

namespace embot { namespace app { namespace application {
           
    class theIMU
    {
    public:
        static theIMU& getInstance();
        
        
    public:
        struct Config
        {
            embot::hw::BNO055::Sensor   sensor;
            embot::hw::BNO055::Config   sensorconfig;
            embot::common::Event        tickevent;
            embot::common::Event        datareadyevent;
            embot::sys::Task*           totask;
            Config() :  
                sensor(embot::hw::BNO055::Sensor::one), 
                sensorconfig(embot::hw::BNO055::Config(embot::hw::i2c::Descriptor(embot::hw::I2C::two, 400000))), 
                tickevent(0), datareadyevent(0), totask(nullptr) 
                {}
            Config(embot::hw::BNO055::Sensor _s, const embot::hw::BNO055::Config& _sc, embot::common::Event _te, embot::common::Event _de, embot::sys::Task* _ts) :     
                sensor(_s),
                sensorconfig(_sc),
                tickevent(_te), 
                datareadyevent(_de), 
                totask(_ts) 
                {}    
        }; 
        
        
        bool initialise(Config &config);   

        bool configure(embot::app::canprotocol::analog::polling::Message_ACC_GYRO_SETUP::Info &cfg);
        bool start();  
        
        bool configure(embot::app::canprotocol::analog::polling::Message_IMU_CONFIG_SET::Info &info);
        bool get(embot::app::canprotocol::analog::polling::Message_IMU_CONFIG_GET::ReplyInfo &info);
        bool start(embot::common::relTime period);
 
        bool stop();        
        bool tick(std::vector<embot::hw::can::Frame> &replies);        
        bool processdata(std::vector<embot::hw::can::Frame> &replies);

    private:
        theIMU(); 
        ~theIMU(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;        
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
