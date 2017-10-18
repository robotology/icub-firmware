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


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_canprotocol_analog_periodic.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"
#include "embot_common.h"
#include "embot_binary.h"

#include <cstring>

#include "embot_hw.h"



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

    
namespace embot { namespace app { namespace canprotocol { namespace analog { namespace periodic {
    
    
    bool supported(std::uint8_t cmd)
    {
        return (CMD::none != convert(cmd))? true : false;
    }
        
    CMD convert(std::uint8_t cmd)
    {
        static const std::uint16_t aspermask16 =    (1 << static_cast<std::uint8_t>(CMD::UNCALIBFORCE_VECTOR_DEBUGMODE))    |
                                                    (1 << static_cast<std::uint8_t>(CMD::UNCALIBTORQUE_VECTOR_DEBUGMODE))   |   
                                                    (1 << static_cast<std::uint8_t>(CMD::FORCE_VECTOR))                     |
                                                    (1 << static_cast<std::uint8_t>(CMD::TORQUE_VECTOR))                    |
                                                    (1 << static_cast<std::uint8_t>(CMD::TERMOMETER_MEASURE));

        if(cmd > 15)
        {
            return CMD::none;
        }

        if(true == embot::binary::bit::check(aspermask16, cmd))
        {
            return static_cast<CMD>(cmd);          
        }
        
        return CMD::none;              
    } 

    std::uint8_t convert(CMD cmd)
    {
        return static_cast<std::uint8_t>(cmd);
    } 


    struct ADCsaturationInfoFormatted
    {
        std::uint8_t    atleastinonechannel     :2;
        std::uint8_t    first                   :2;
        std::uint8_t    second                  :2;
        std::uint8_t    third                   :2; 
        ADCsaturationInfoFormatted() : atleastinonechannel(0), first(0), second(0), third(0) {}      
    };
 
    std::uint8_t convert2protocolbyte(ADCsaturationInfo &info)
    {
        std::uint8_t byte = 0;
        ADCsaturationInfoFormatted ff;
        ff.atleastinonechannel = (true == info.thereissaturation) ? (1) : (0);
        ff.first = static_cast<std::uint8_t>(info.channel[0]);
        ff.second = static_cast<std::uint8_t>(info.channel[1]);
        ff.third = static_cast<std::uint8_t>(info.channel[2]);
        std::memmove(&byte, &ff, 1);
        
        return byte;
    }    
    

    bool Message_UNCALIBFORCE_VECTOR_DEBUGMODE::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_UNCALIBFORCE_VECTOR_DEBUGMODE::get(embot::hw::can::Frame &outframe)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = static_cast<std::uint8_t>((info.x & 0x00ff));
        data08[1] = static_cast<std::uint8_t>((info.x & 0xff00) >> 8);
        data08[2] = static_cast<std::uint8_t>((info.y & 0x00ff));
        data08[3] = static_cast<std::uint8_t>((info.y & 0xff00) >> 8);  
        data08[4] = static_cast<std::uint8_t>((info.z & 0x00ff));
        data08[5] = static_cast<std::uint8_t>((info.z & 0xff00) >> 8);     
        data08[6] = 0;
        std::uint8_t size = 6;
        
        if(true == info.adcsaturationispresent)
        {
            size = 7;
            data08[6] = convert2protocolbyte(info.adcsaturationinfo);
        }
        
        Message::set(info.canaddress, 0xf, Clas::periodicAnalogSensor, static_cast<std::uint8_t>(CMD::UNCALIBFORCE_VECTOR_DEBUGMODE), data08, size);
        std::memmove(&outframe, &canframe, sizeof(embot::hw::can::Frame));
                    
        return true;
    }  




    bool Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::get(embot::hw::can::Frame &outframe)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = static_cast<std::uint8_t>((info.x & 0x00ff));
        data08[1] = static_cast<std::uint8_t>((info.x & 0xff00) >> 8);
        data08[2] = static_cast<std::uint8_t>((info.y & 0x00ff));
        data08[3] = static_cast<std::uint8_t>((info.y & 0xff00) >> 8);  
        data08[4] = static_cast<std::uint8_t>((info.z & 0x00ff));
        data08[5] = static_cast<std::uint8_t>((info.z & 0xff00) >> 8);  
        data08[6] = 0;
        std::uint8_t size = 6;
        
        if(true == info.adcsaturationispresent)
        {
            size = 7;
            data08[6] = convert2protocolbyte(info.adcsaturationinfo);
        }
        
        Message::set(info.canaddress, 0xf, Clas::periodicAnalogSensor, static_cast<std::uint8_t>(CMD::UNCALIBTORQUE_VECTOR_DEBUGMODE), data08, size);
        std::memmove(&outframe, &canframe, sizeof(embot::hw::can::Frame));
                    
        return true;
    }          
    
    
    bool Message_FORCE_VECTOR::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_FORCE_VECTOR::get(embot::hw::can::Frame &outframe)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = static_cast<std::uint8_t>((info.x & 0x00ff));
        data08[1] = static_cast<std::uint8_t>((info.x & 0xff00) >> 8);
        data08[2] = static_cast<std::uint8_t>((info.y & 0x00ff));
        data08[3] = static_cast<std::uint8_t>((info.y & 0xff00) >> 8);  
        data08[4] = static_cast<std::uint8_t>((info.z & 0x00ff));
        data08[5] = static_cast<std::uint8_t>((info.z & 0xff00) >> 8);  
        data08[6] = 0;
        std::uint8_t size = 6;
        
        if(true == info.adcsaturationispresent)
        {
            size = 7;
            data08[6] = convert2protocolbyte(info.adcsaturationinfo);
        }

        
        Message::set(info.canaddress, 0xf, Clas::periodicAnalogSensor, static_cast<std::uint8_t>(CMD::FORCE_VECTOR), data08, size);
        std::memmove(&outframe, &canframe, sizeof(embot::hw::can::Frame));
                    
        return true;
    }  




    bool Message_TORQUE_VECTOR::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_TORQUE_VECTOR::get(embot::hw::can::Frame &outframe)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = static_cast<std::uint8_t>((info.x & 0x00ff));
        data08[1] = static_cast<std::uint8_t>((info.x & 0xff00) >> 8);
        data08[2] = static_cast<std::uint8_t>((info.y & 0x00ff));
        data08[3] = static_cast<std::uint8_t>((info.y & 0xff00) >> 8);  
        data08[4] = static_cast<std::uint8_t>((info.z & 0x00ff));
        data08[5] = static_cast<std::uint8_t>((info.z & 0xff00) >> 8);   
        data08[6] = 0;
        std::uint8_t size = 6;
        
        if(true == info.adcsaturationispresent)
        {
            size = 7;
            data08[6] = convert2protocolbyte(info.adcsaturationinfo);
        }

        
        Message::set(info.canaddress, 0xf, Clas::periodicAnalogSensor, static_cast<std::uint8_t>(CMD::TORQUE_VECTOR), data08, size);
        std::memmove(&outframe, &canframe, sizeof(embot::hw::can::Frame));
                    
        return true;
    }  
        
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace analog { namespace periodic {
    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


