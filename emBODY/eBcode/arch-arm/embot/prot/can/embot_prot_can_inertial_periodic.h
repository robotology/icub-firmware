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

#ifndef _EMBOT_PROT_CAN_INERTIAL_PERIODIC_H_
#define _EMBOT_PROT_CAN_INERTIAL_PERIODIC_H_

#include "embot_core.h"
#include "embot_core_utils.h"

#include "embot_prot_can.h"

namespace embot { namespace prot { namespace can { namespace inertial { namespace periodic {
    
    // the supported commands    
    enum class CMD { 
        none = 0xfe, 
        
        DIGITAL_GYROSCOPE = 0, DIGITAL_ACCELEROMETER = 1,
        // NEW messages:
        IMU_TRIPLE = 3, IMU_QUATERNION = 4, IMU_STATUS = 5    
    };
    
    // NOTES
    // - the ICUBCANPROTO_PER_IS_MSG__ANALOG_ACCELEROMETER (2) is not supported as it is not managed by the mtb
    // - there is room to add new proper messages for IMU.   
    
    // some utilities    
    bool supported(std::uint8_t cmd);        
    CMD convert(std::uint8_t cmd);
    std::uint8_t convert(CMD cmd);       
    
}}}}} // namespace embot { namespace prot { namespace can { namespace inertial { namespace periodic {     

  
namespace embot { namespace prot { namespace can { namespace inertial { namespace periodic {
    
        
    // the management of commands        
    
    class Message_DIGITAL_GYROSCOPE : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            std::int16_t                x;
            std::int16_t                y;
            std::int16_t                z;
            Info() : canaddress(0), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_DIGITAL_GYROSCOPE() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };
    
    class Message_DIGITAL_ACCELEROMETER : public Message
    {
        public:
                        
        struct Info
        { 
            std::uint8_t                canaddress;
            std::int16_t                x;
            std::int16_t                y;
            std::int16_t                z;
            Info() : canaddress(0), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_DIGITAL_ACCELEROMETER() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    }; 
    
    
    class Message_IMU_TRIPLE : public Message
    {
        public:
                    
        struct Info
        {
            std::uint8_t                                canaddress;
            std::uint8_t                                seqnumber;
            embot::prot::can::analog::imuSensor  sensor;
            embot::core::utils::Triple<std::int16_t>          value;
            Info() : canaddress(0), seqnumber(0), sensor(embot::prot::can::analog::imuSensor::none) { value.clear(); }
        };
        
        Info info;
        
        Message_IMU_TRIPLE() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };
    
    
    class Message_IMU_QUATERNION : public Message
    {
        public:
            

        struct Info
        {
            std::uint8_t                            canaddress;
            embot::core::utils::Quadruple<std::int16_t>   value;
            Info() : canaddress(0) { value.clear(); }
        };
        
        Info info;
        
        Message_IMU_QUATERNION() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };  
    

    class Message_IMU_STATUS : public Message
    {
        public:
            
        enum class Calibration { none = 0, poor = 1, medium = 2, good = 3};
            
        struct Info
        {
            std::uint8_t                            canaddress;
            std::uint8_t                            seqnumber;
            Calibration                             gyrcalib; 
            Calibration                             acccalib;
            Calibration                             magcalib;
            embot::core::relTime                  acquisitiontime; 
            Info() : canaddress(0), seqnumber(0), gyrcalib(Calibration::none), acccalib(Calibration::none), magcalib(Calibration::none), acquisitiontime(0)  { }
        };
        
        Info info;
        
        Message_IMU_STATUS() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };      
    
}}}}} // namespace embot { namespace prot { namespace can { namespace inertial { namespace periodic {    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
