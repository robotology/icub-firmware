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

#include "embot_prot_can_motor_polling.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_core_binary.h"

#include <cstring>



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


namespace embot::prot::can::motor::polling {

    bool supported(std::uint8_t cmd)
    {
        return (CMD::none != convert(cmd))? true : false;
    }
        
    CMD convert(std::uint8_t cmd)
    {
        static const std::uint64_t mcpollmask256[4] = 
        {
            // bits 0-63
            (1ULL << static_cast<std::uint8_t>(CMD::GET_CONTROL_MODE))              |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_CONTROL_MODE))              |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_BOARD_ID))                  ,
            // bits 64-127
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_CURRENT_LIMIT)-64))        |
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_CURRENT_LIMIT)-64))        | 
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_CURRENT_PID)-64))          | 
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_CURRENT_PID)-64))          | 
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_VELOCITY_PID)-64))         | 
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_VELOCITY_PID)-64))         | 
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_MOTOR_CONFIG)-64))         | 
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_MOTOR_CONFIG)-64))         |                
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_FIRMWARE_VERSION)-64))     |
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_TEMPERATURE_LIMIT)-64))    |
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_TEMPERATURE_LIMIT)-64))    , 

            // bits 128-191    
            0, 
            // bits 191-255
            0 
        }; 

        std::uint8_t ind = cmd >> 6;
        std::uint8_t pos = cmd & 0x3f;
        if(true == embot::core::binary::bit::check(mcpollmask256[ind], pos))
        {
            return static_cast<CMD>(cmd);            
        }
        
        return CMD::none;        
    } 

    std::uint8_t convert(CMD cmd)
    {
        return static_cast<std::uint8_t>(cmd);
    }  

    
    // Message_GET_FIRMWARE_VERSION and Message_SET_BOARD_ID are derived in the .h file from the Message_base_GET_FIRMWARE_VERSION and Message_base_SET_ID and 
      
    // fill with other future Messages. 
    
    MotIndex motorpollingframe2motindex(const embot::prot::can::Frame &frame)
    {  
        return static_cast<MotIndex>(frame.data[0] >> 7);
    }
    
    MotIndex toMotIndex(uint8_t v)
    {
        MotIndex m {MotIndex::none};
        
        switch(v)
        {
            case 0:
            {
                m = MotIndex::one;
            } break;

            case 1:
            {
                m = MotIndex::two;
            } break;
            
            default:
            {
                m = MotIndex::none;
            } break;

        }  

        return m;        
    }
    
    uint8_t convert(MotIndex mo)
    {
        return embot::core::tointegral(mo);
    }
           
    std::string tostring(MotIndex mo)
    {

        switch(mo)
        {
            case MotIndex::one:
            {
                return "one";
            } break;

            case MotIndex::two:
            {
                return "two";
            } break;
            
            case MotIndex::three:
            {
                return "three";
            } break;  

            case MotIndex::four:
            {
                return "four";
            } break;
            
            default:
            {
                return "none";
            } break;

        } 

        return "none";
    }        
      
        
        
    
    bool Message_SET_CONTROL_MODE::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CONTROL_MODE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.controlmode = embot::prot::can::motor::Converter::toControlMode(candata.datainframe[0]);
                      
        return true;         
    }                    
 
    
    bool Message_SET_CONTROL_MODE::reply()
    {
        return false;
    } 
    
    
    bool Message_GET_CONTROL_MODE::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CONTROL_MODE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    
    bool Message_GET_CONTROL_MODE::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = embot::prot::can::motor::Converter::convert(replyinfo.controlmode); 
                       
        std::uint8_t datalen = 1;
        uint8_t mcindex = convert(replyinfo.motorindex);
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_CONTROL_MODE), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }  

    
    bool Message_SET_CURRENT_LIMIT::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CURRENT_LIMIT) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);

        info.currents.x = embot::core::binary::word::memory2value<uint8_t>(&candata.datainframe[0]);
        info.currents.nominal = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[1]);
        info.currents.peak = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[3]);        
        info.currents.overload = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[5]);
                      
        return true;         
    }                    
        
    bool Message_SET_CURRENT_LIMIT::reply()
    {
        return false;
    } 

    bool Message_GET_CURRENT_LIMIT::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CURRENT_LIMIT) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    bool Message_GET_CURRENT_LIMIT::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        
        std::uint8_t dd[7] = {0};
        // fill the 7 bytes
        std::uint8_t datalen = 7;
        
        embot::core::binary::word::load2memory<uint8_t>(replyinfo.currents.x, &dd[0]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.currents.nominal, &dd[1]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.currents.peak, &dd[3]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.currents.overload, &dd[5]);       

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_CURRENT_LIMIT), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }          


    bool Message_SET_CURRENT_PID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CURRENT_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.pid.type = embot::prot::can::motor::PID::Type::CURR;
        info.pid.kp = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[0]);
        info.pid.ki = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[2]);
        info.pid.kd = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[4]);
        info.pid.ks = embot::core::binary::word::memory2value<uint8_t>(&candata.datainframe[6]);
                     
        return true;         
    }                    
        
    bool Message_SET_CURRENT_PID::reply()
    {
        return false;
    } 

    bool Message_GET_CURRENT_PID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CURRENT_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    bool Message_GET_CURRENT_PID::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        
        std::uint8_t dd[7] = {0};
        // fill the 7 bytes
        std::uint8_t datalen = 7;
        
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pid.kp, &dd[0]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pid.ki, &dd[2]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pid.kd, &dd[4]);
        embot::core::binary::word::load2memory<uint8_t>(replyinfo.pid.ks, &dd[6]);       

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_CURRENT_PID), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
 

    bool Message_SET_VELOCITY_PID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_VELOCITY_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.pid.type = embot::prot::can::motor::PID::Type::VEL;
        info.pid.kp = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[0]);
        info.pid.ki = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[2]);
        info.pid.kd = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[4]);
        info.pid.ks = embot::core::binary::word::memory2value<uint8_t>(&candata.datainframe[6]);
                     
        return true;         
    }                    
        
    bool Message_SET_VELOCITY_PID::reply()
    {
        return false;
    } 

    bool Message_GET_VELOCITY_PID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_VELOCITY_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    bool Message_GET_VELOCITY_PID::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        
        std::uint8_t dd[7] = {0};
        // fill the 7 bytes
        std::uint8_t datalen = 7;
        
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pid.kp, &dd[0]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pid.ki, &dd[2]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pid.kd, &dd[4]);
        embot::core::binary::word::load2memory<uint8_t>(replyinfo.pid.ks, &dd[6]);       

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_VELOCITY_PID), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    } 


    bool Message_SET_MOTOR_CONFIG::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_MOTOR_CONFIG) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);

        info.config.flags = embot::core::binary::word::memory2value<uint8_t>(&candata.datainframe[0]);
        info.config.rotorEncoderResolution = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[1]);
        info.config.rotorIndexOffset = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[3]);
        info.config.motorPoles = embot::core::binary::word::memory2value<uint8_t>(&candata.datainframe[5]);
        info.config.rotEncTolerance = embot::core::binary::word::memory2value<uint8_t>(&candata.datainframe[6]);
                     
        return true;         
    }                    
        
    bool Message_SET_MOTOR_CONFIG::reply()
    {
        return false;
    } 

    bool Message_GET_MOTOR_CONFIG::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_MOTOR_CONFIG) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    bool Message_GET_MOTOR_CONFIG::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        
        std::uint8_t dd[7] = {0};
        // fill the 7 bytes
        std::uint8_t datalen = 7;        
       
        embot::core::binary::word::load2memory<uint8_t>(replyinfo.config.flags, &dd[0]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.config.rotorEncoderResolution, &dd[1]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.config.rotorIndexOffset, &dd[3]);
        embot::core::binary::word::load2memory<uint8_t>(replyinfo.config.motorPoles, &dd[5]); 
        embot::core::binary::word::load2memory<uint8_t>(replyinfo.config.rotEncTolerance, &dd[6]);        

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_MOTOR_CONFIG), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }     


    bool Message_SET_TEMPERATURE_LIMIT::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_TEMPERATURE_LIMIT) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);

        info.hardwarelimit = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[0]);
                     
        return true;         
    }                    
        
    bool Message_SET_TEMPERATURE_LIMIT::reply()
    {
        return false;
    } 

    bool Message_GET_TEMPERATURE_LIMIT::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_TEMPERATURE_LIMIT) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    bool Message_GET_TEMPERATURE_LIMIT::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        
        std::uint8_t dd[7] = {0};
        // fill the 2 bytes
        std::uint8_t datalen = 2;        
       
        embot::core::binary::word::load2memory<int16_t>(replyinfo.hardwarelimit, &dd[0]);     

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_TEMPERATURE_LIMIT), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    
}   // namespace embot::prot::can::motor::polling {
    
    

    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


