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

#ifndef _EMBOT_PROT_CAN_ANALOG_PERIODIC_H_
#define _EMBOT_PROT_CAN_ANALOG_PERIODIC_H_

#include "embot_core.h"

#include "embot_prot_can.h"

#include "iCubCanProto_analogSensorMessages.h"

#include <array>

namespace embot { namespace prot { namespace can { namespace analog { namespace periodic {
        
    // the supported commands    
    enum class CMD { 
        none = 0xfe, 
        
        USERDEF = 0, 
        UNCALIBFORCE_VECTOR_DEBUGMODE = ICUBCANPROTO_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE, 
        UNCALIBTORQUE_VECTOR_DEBUGMODE = ICUBCANPROTO_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE,                    // strain 
        FORCE_VECTOR = ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR, 
        TORQUE_VECTOR = ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR,                                                      // strain
        // NEW messages:
        THERMOMETER_MEASURE = ICUBCANPROTO_PER_AS_MSG__THERMOMETER_MEASURE,
        POS = ICUBCANPROTO_PER_AS_MSG__POS
    };
    
    // NOTES
    // - only some messages are managed. in particular those for strain board.
    // - messages ICUBCANPROTO_PER_AS_MSG__HES0TO6 (0xc) and ICUBCANPROTO_PER_AS_MSG__HES7TO14 (0xd) are used by mais.
    // - messages ICUBCANPROTO_PER_AS_MSG__DIGITAL_GYROSCOPE (0) and ICUBCANPROTO_PER_AS_MSG__DIGITAL_ACCELEROMETER (1) are
    //   defined but are deprecated. they are not used by any board and they shall not be used in the future becase we 
    //   have a dedicated inertial broadcast class.
    
    
    
    // some utilities    
    bool supported(std::uint8_t cmd);        
    CMD convert(std::uint8_t cmd);
    std::uint8_t convert(CMD cmd);

}}}}} // namespace embot { namespace prot { namespace can { namespace analog { namespace periodic {   
 


   
  
namespace embot { namespace prot { namespace can { namespace analog { namespace periodic {
        

    // the management of commands   

    enum class ADCsaturation { NONE = 0, LOW = 1, HIGH = 2 };
    
    struct ADCsaturationInfo
    {
        bool            thereissaturation;
        ADCsaturation   channel[3]; 

        void reset() { thereissaturation = false;  channel[0] = channel[1] = channel[2] = ADCsaturation::NONE; }     
        ADCsaturationInfo() { reset(); }          
    };
     
    std::uint8_t convert2protocolbyte(ADCsaturationInfo &info);   
    
    class Message_UNCALIBFORCE_VECTOR_DEBUGMODE : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            bool                        adcsaturationispresent;
            ADCsaturationInfo           adcsaturationinfo;
            std::uint16_t               x;
            std::uint16_t               y;
            std::uint16_t               z;
            Info() : canaddress(0), adcsaturationispresent(false), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_UNCALIBFORCE_VECTOR_DEBUGMODE() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };    
    
    
    class Message_UNCALIBTORQUE_VECTOR_DEBUGMODE : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            bool                        adcsaturationispresent;
            ADCsaturationInfo           adcsaturationinfo;
            std::uint16_t               x;
            std::uint16_t               y;
            std::uint16_t               z;
            Info() : canaddress(0), adcsaturationispresent(false), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_UNCALIBTORQUE_VECTOR_DEBUGMODE() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };    

    
    class Message_FORCE_VECTOR : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            bool                        adcsaturationispresent;
            ADCsaturationInfo           adcsaturationinfo;
            std::uint16_t               x;
            std::uint16_t               y;
            std::uint16_t               z;
            Info() : canaddress(0), adcsaturationispresent(false), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_FORCE_VECTOR() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };    
    
    
    class Message_TORQUE_VECTOR : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            bool                        adcsaturationispresent;
            ADCsaturationInfo           adcsaturationinfo;            
            std::uint16_t               x;
            std::uint16_t               y;
            std::uint16_t               z;
            Info() : canaddress(0), adcsaturationispresent(false), x(0), y(0), z(0) { }
        };
        
        Info info;
        
        Message_TORQUE_VECTOR() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };


    class Message_THERMOMETER_MEASURE : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            std::uint8_t                mask;   // 0x00, 0x01, 0x02 or 0x03
            std::int16_t                temp0;  // in 0.1 Celsius Degrees
            std::int16_t                temp1;  // in 0.1 Celsius Degrees
            Info() : canaddress(0), temp0(0), temp1(0) { }
        };
        
        Info info;
        
        Message_THERMOMETER_MEASURE() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };    
        
    class Message_USERDEF : public Message
    {
        public:
            
        struct Info
        { 
            std::uint8_t                canaddress;
            std::uint8_t                data[8];
            Info() : canaddress(0) { std::memset(data, 0, sizeof(data)); }
        };
        
        Info info;
        
        Message_USERDEF() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };  


    class Message_POS : public Message
    {
        public:
                        
        struct Info
        {   // this message transports positions. 
            // the type of positions (e.g., angles in deci-degree, ...) and which ones of them (e.g., 2 starting from tag 0) 
            // are described by posDES descriptor.
            // their values are inside data[], which is formatted according to the content of descriptor.
            // for instance, if descriptor.type is posTYPE::angleDeciDeg, descriptor.firsttag is zero, and descriptor.numberoftags = 2 ...
            // inside data we have two value of type deciDeg, of two bytes each, placed in little-endian and the first value has tag = zero,
            // and the second has the consecutive tag which is ... one.            
            std::uint8_t                canaddress;
            posDES                      descriptor;
            std::uint8_t                data[6];
            Info() : canaddress(0) { std::memset(data, 0, sizeof(data)); }
            // at most three deciDeg in data[6]
            void loadDeciDeg(const posLABEL startl, const uint8_t n, const std::array<deciDeg, 3> &values) 
            {
                descriptor.type = posTYPE::angleDeciDeg;
                descriptor.startlabel = startl;
                descriptor.labelsnumberof = n;
                deciDeg *d = reinterpret_cast<deciDeg*>(data);
                //for(auto &v : values) { *d++ = v; }
                for(uint8_t i=0; i<n; i++) { d[i] = values[i]; }
            }
            // at most three deciDeg in data[6]
            void loadDeciMilliMeter(const posLABEL startl, const uint8_t n, const std::array<deciMilliMeter, 3> &values) 
            {
                descriptor.type = posTYPE::linearDeciMilliMeter;
                descriptor.startlabel = startl;
                descriptor.labelsnumberof = n;
                deciMilliMeter *d = reinterpret_cast<deciMilliMeter*>(data);
                //for(auto &v : values) { *d++ = v; }
                for(uint8_t i=0; i<n; i++) { d[i] = values[i]; }
            }            
        };
        
        Info info;
        
        Message_POS() {}
            
        bool load(const Info& inf);
            
        bool get(embot::prot::can::Frame &outframe);        
    };    
        
    
}}}}} // namespace embot { namespace prot { namespace can { namespace analog { namespace periodic {    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
