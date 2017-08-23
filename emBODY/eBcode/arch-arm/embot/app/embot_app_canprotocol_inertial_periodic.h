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

#ifndef _EMBOT_APP_CANPROTOCOL_INERTIAL_PERIODIC_H_
#define _EMBOT_APP_CANPROTOCOL_INERTIAL_PERIODIC_H_

#include "embot_common.h"

#include "embot_app_canprotocol.h"


namespace embot { namespace app { namespace canprotocol { namespace inertial { namespace periodic {
    
    // the supported commands    
    enum class CMD { 
        none = 0xfe, 
        
        DIGITAL_GYROSCOPE = 0, DIGITAL_ACCELEROMETER = 1
    };
    
    // NOTES
    // - the ICUBCANPROTO_PER_IS_MSG__ANALOG_ACCELEROMETER (2) is not supported as it is not managed by the mtb
    // - there is room to add new proper messages for IMU.   
    
    // some utilities    
    bool supported(std::uint8_t cmd);        
    CMD convert(std::uint8_t cmd);
    std::uint8_t convert(CMD cmd);       
    
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace inertial { namespace periodic {     

  
namespace embot { namespace app { namespace canprotocol { namespace inertial { namespace periodic {
        
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
            
        bool get(embot::hw::can::Frame &outframe);        
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
            
        bool get(embot::hw::can::Frame &outframe);        
    }; 
    
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace inertial { namespace periodic {    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
