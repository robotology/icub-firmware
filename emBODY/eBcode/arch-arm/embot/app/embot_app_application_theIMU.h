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

#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_app_application_theCANparserIMU.h"
#include "embot_hw_bno055.h"
#include <vector>

namespace embot { namespace app { namespace application {
           
    class theIMU : public CANagentIMU
    {
    public:
        static theIMU& getInstance();
        
        
    public:
        struct Config
        {
            embot::hw::BNO055           sensor;
            embot::hw::bno055::Config   sensorconfig;
            embot::os::Event            tickevent;
            embot::os::Event            datareadyevent;
            embot::os::Thread*          totask;
            Config() :  
                sensor(embot::hw::BNO055::one), 
                sensorconfig({}), 
                tickevent(0), datareadyevent(0), totask(nullptr) 
                {}
            Config(embot::hw::BNO055 _s, const embot::hw::bno055::Config& _sc, embot::os::Event _te, embot::os::Event _de, embot::os::Thread* _ts) :     
                sensor(_s),
                sensorconfig(_sc),
                tickevent(_te), 
                datareadyevent(_de), 
                totask(_ts) 
                {}    
        }; 
        
        
        bool initialise(Config &config);   

        bool start();          
        bool start(embot::core::relTime period);
 
        bool stop();        
        bool tick(std::vector<embot::prot::can::Frame> &replies);        
        bool processdata(std::vector<embot::prot::can::Frame> &replies);
        
        // interface to CANagentIMU
        virtual bool set(const embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::Info &info);
        virtual bool set(const embot::prot::can::analog::polling::Message_IMU_CONFIG_SET::Info &info);       
        virtual bool set(const embot::prot::can::analog::polling::Message_IMU_TRANSMIT::Info &info);  
        
        virtual bool get(const embot::prot::can::analog::polling::Message_IMU_CONFIG_GET::Info &info, embot::prot::can::analog::polling::Message_IMU_CONFIG_GET::ReplyInfo &replyinfo);    
        

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
