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

#ifndef _EMBOT_APP_CANPROTOCOL_ANALOG_PERIODIC_H_
#define _EMBOT_APP_CANPROTOCOL_ANALOG_PERIODIC_H_

#include "embot_common.h"

#include "embot_app_canprotocol.h"



namespace embot { namespace app { namespace canprotocol { namespace analog { namespace periodic {
        
    // the supported commands    
    enum class CMD { 
        none = 0xfe, 
        
        UNCALIBFORCE_VECTOR_DEBUGMODE = 0x08, UNCALIBTORQUE_VECTOR_DEBUGMODE = 0x09,                    // strain 
        FORCE_VECTOR = 0x0A, TORQUE_VECTOR = 0x0B                                                       // strain
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

}}}}} // namespace embot { namespace app { namespace canprotocol { namespace analog { namespace periodic {   
 


   
  
namespace embot { namespace app { namespace canprotocol { namespace analog { namespace periodic {
        

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
            
        bool get(embot::hw::can::Frame &outframe);        
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
            
        bool get(embot::hw::can::Frame &outframe);        
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
            
        bool get(embot::hw::can::Frame &outframe);        
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
            
        bool get(embot::hw::can::Frame &outframe);        
    };    
    
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace analog { namespace periodic {    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
