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

#include "embot_app_canprotocol_inertial_periodic.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"
#include "embot_core.h"
#include "embot_binary.h"

#include <cstring>

#include "embot_hw.h"



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



    
    
    
namespace embot { namespace prot { namespace can { namespace inertial { namespace periodic {
    
    
    bool supported(std::uint8_t cmd)
    {
        return (CMD::none != convert(cmd))? true : false;
    }
        
    CMD convert(std::uint8_t cmd)
    {   
        constexpr std::uint16_t ispermask16 =    (1 << static_cast<std::uint8_t>(CMD::DIGITAL_GYROSCOPE))        |
                                                    (1 << static_cast<std::uint8_t>(CMD::DIGITAL_ACCELEROMETER))    |
                                                    (1 << static_cast<std::uint8_t>(CMD::IMU_TRIPLE))               |
                                                    (1 << static_cast<std::uint8_t>(CMD::IMU_QUATERNION))           |
                                                    (1 << static_cast<std::uint8_t>(CMD::IMU_STATUS));

        if(cmd > 15)
        {
            return CMD::none;
        }

        if(true == embot::core::binary::bit::check(ispermask16, cmd))
        {
            return static_cast<CMD>(cmd);          
        }
        
        return CMD::none;           
    } 

    std::uint8_t convert(CMD cmd)
    {
        return static_cast<std::uint8_t>(cmd);
    } 

    

    bool Message_DIGITAL_GYROSCOPE::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_DIGITAL_GYROSCOPE::get(embot::prot::can::Frame &outframe)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = static_cast<std::uint8_t>((info.x & 0x00ff));
        data08[1] = static_cast<std::uint8_t>((info.x & 0xff00) >> 8);
        data08[2] = static_cast<std::uint8_t>((info.y & 0x00ff));
        data08[3] = static_cast<std::uint8_t>((info.y & 0xff00) >> 8);  
        data08[4] = static_cast<std::uint8_t>((info.z & 0x00ff));
        data08[5] = static_cast<std::uint8_t>((info.z & 0xff00) >> 8);            
        Message::set(info.canaddress, 0xf, Clas::periodicInertialSensor, static_cast<std::uint8_t>(CMD::DIGITAL_GYROSCOPE), data08, 6);
        std::memmove(&outframe, &canframe, sizeof(embot::prot::can::Frame));
                    
        return true;
    }  

    bool Message_DIGITAL_ACCELEROMETER::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_DIGITAL_ACCELEROMETER::get(embot::prot::can::Frame &outframe)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = static_cast<std::uint8_t>((info.x & 0x00ff));
        data08[1] = static_cast<std::uint8_t>((info.x & 0xff00) >> 8);
        data08[2] = static_cast<std::uint8_t>((info.y & 0x00ff));
        data08[3] = static_cast<std::uint8_t>((info.y & 0xff00) >> 8);  
        data08[4] = static_cast<std::uint8_t>((info.z & 0x00ff));
        data08[5] = static_cast<std::uint8_t>((info.z & 0xff00) >> 8);            
        Message::set(info.canaddress, 0xf, Clas::periodicInertialSensor, static_cast<std::uint8_t>(CMD::DIGITAL_ACCELEROMETER), data08, 6);
        std::memmove(&outframe, &canframe, sizeof(embot::prot::can::Frame));
                    
        return true;
    }    


    bool Message_IMU_TRIPLE::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_IMU_TRIPLE::get(embot::prot::can::Frame &outframe)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = info.seqnumber;
        data08[1] = static_cast<std::uint8_t>(info.sensor);
        data08[2] = static_cast<std::uint8_t>((info.value.x & 0x00ff));
        data08[3] = static_cast<std::uint8_t>((info.value.x & 0xff00) >> 8);  
        data08[4] = static_cast<std::uint8_t>((info.value.y & 0x00ff));
        data08[5] = static_cast<std::uint8_t>((info.value.y & 0xff00) >> 8);   
        data08[6] = static_cast<std::uint8_t>((info.value.z & 0x00ff));
        data08[7] = static_cast<std::uint8_t>((info.value.z & 0xff00) >> 8);          
        Message::set(info.canaddress, 0xf, Clas::periodicInertialSensor, static_cast<std::uint8_t>(CMD::IMU_TRIPLE), data08, 8);
        std::memmove(&outframe, &canframe, sizeof(embot::prot::can::Frame));
                    
        return true;
    }  

    bool Message_IMU_QUATERNION::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_IMU_QUATERNION::get(embot::prot::can::Frame &outframe)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = static_cast<std::uint8_t>((info.value.w & 0x00ff));
        data08[1] = static_cast<std::uint8_t>((info.value.w & 0xff00) >> 8); 
        data08[2] = static_cast<std::uint8_t>((info.value.x & 0x00ff));
        data08[3] = static_cast<std::uint8_t>((info.value.x & 0xff00) >> 8);  
        data08[4] = static_cast<std::uint8_t>((info.value.y & 0x00ff));
        data08[5] = static_cast<std::uint8_t>((info.value.y & 0xff00) >> 8);   
        data08[6] = static_cast<std::uint8_t>((info.value.z & 0x00ff));
        data08[7] = static_cast<std::uint8_t>((info.value.z & 0xff00) >> 8);          
        Message::set(info.canaddress, 0xf, Clas::periodicInertialSensor, static_cast<std::uint8_t>(CMD::IMU_QUATERNION), data08, 8);
        std::memmove(&outframe, &canframe, sizeof(embot::prot::can::Frame));
                    
        return true;
    }


    bool Message_IMU_STATUS::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_IMU_STATUS::get(embot::prot::can::Frame &outframe)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = info.seqnumber;
        data08[1] = static_cast<std::uint8_t>(info.gyrcalib); 
        data08[2] = static_cast<std::uint8_t>(info.acccalib);
        data08[3] = static_cast<std::uint8_t>(info.magcalib);  
        std::memmove(&data08[4], &info.acquisitiontime, 4);
        Message::set(info.canaddress, 0xf, Clas::periodicInertialSensor, static_cast<std::uint8_t>(CMD::IMU_STATUS), data08, 8);
        std::memmove(&outframe, &canframe, sizeof(embot::prot::can::Frame));
                    
        return true;
    }         

}}}}} // namespace embot { namespace prot { namespace can { namespace inertial { namespace periodic {
    
    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


