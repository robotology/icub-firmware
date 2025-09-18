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
        constexpr std::uint64_t mcpollmask256[4] = 
        {
            // bits 0-63
            (1ULL << static_cast<std::uint8_t>(CMD::SET_MOTOR_PARAM))               |
            (1ULL << static_cast<std::uint8_t>(CMD::GET_MOTOR_PARAM))               |
            (1ULL << static_cast<std::uint8_t>(CMD::SET))                           |
            (1ULL << static_cast<std::uint8_t>(CMD::GET))                           |                
            (1ULL << static_cast<std::uint8_t>(CMD::GET_CONTROL_MODE))              |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_CONTROL_MODE))              |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_BOARD_ID))                  ,
            // bits 64-127
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_CURRENT_LIMIT)-64))        |
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_POS_PID)-64))              | 
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_POS_PID)-64))              | 
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_POS_PIDLIMITS)-64))        |
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_POS_PIDLIMITS)-64))        |            
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_FIRMWARE_VERSION)-64))     |
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_CURRENT_PID)-64))          | 
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_CURRENT_PID)-64))          | 
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_CURRENT_PIDLIMITS)-64))    |
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_CURRENT_PIDLIMITS)-64))    |
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_VELOCITY_PID)-64))         | 
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_VELOCITY_PID)-64))         | 
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_VELOCITY_PIDLIMITS)-64))   |
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_VELOCITY_PIDLIMITS)-64))   |   
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_MOTOR_CONFIG)-64))         | 
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_TEMPERATURE_LIMIT)-64))    |             
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_TEMPERATURE_LIMIT)-64))    |
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_MOTOR_CONFIG)-64))         |   
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_CURRENT_LIMIT)-64))        |
            (1ULL << (static_cast<std::uint8_t>(CMD::SET_PID)-64))                  |
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_PID)-64))                  |                
            0,                 

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
           
    std::string tostring(const MotIndex mo)
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


    bool Message_SET_POS_PID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_POS_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.pid.type = embot::prot::can::motor::PIDtype::POS;
        info.pid.kp = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[0]);
        info.pid.ki = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[2]);
        info.pid.kd = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[4]);
        info.pid.ks = embot::core::binary::word::memory2value<uint8_t>(&candata.datainframe[6]);
                     
        return true;         
    }                    
        
    bool Message_SET_POS_PID::reply()
    {
        return false;
    } 

    bool Message_GET_POS_PID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_POS_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    bool Message_GET_POS_PID::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
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
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_POS_PID), candata.from, dd, datalen, mcindex);
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
        info.pid.type = embot::prot::can::motor::PIDtype::VEL;
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

    bool Message_SET_CURRENT_PID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CURRENT_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.pid.type = embot::prot::can::motor::PIDtype::CURR;
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
    
    
    bool Message_SET_POS_PIDLIMITS::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_POS_PIDLIMITS) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.pidlimits.type = embot::prot::can::motor::PIDtype::POS;
        info.pidlimits.offset = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[0]);
        info.pidlimits.limitonoutput = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[2]);
        info.pidlimits.limitonintegral = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[4]);
                     
        return true;         
    }                    
        
    bool Message_SET_POS_PIDLIMITS::reply()
    {
        return false;
    } 

    bool Message_GET_POS_PIDLIMITS::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_POS_PIDLIMITS) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    bool Message_GET_POS_PIDLIMITS::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        
        std::uint8_t dd[7] = {0};
        // fill the 6 bytes
        std::uint8_t datalen = 6;
        
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pidlimits.offset, &dd[0]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pidlimits.limitonoutput, &dd[2]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pidlimits.limitonintegral, &dd[4]);    

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_POS_PIDLIMITS), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    
    
    bool Message_SET_CURRENT_PIDLIMITS::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CURRENT_PIDLIMITS) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.pidlimits.type = embot::prot::can::motor::PIDtype::CURR;
        info.pidlimits.offset = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[0]);
        info.pidlimits.limitonoutput = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[2]);
        info.pidlimits.limitonintegral = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[4]);
                     
        return true;         
    }                    
        
    bool Message_SET_CURRENT_PIDLIMITS::reply()
    {
        return false;
    } 

    bool Message_GET_CURRENT_PIDLIMITS::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CURRENT_PIDLIMITS) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    bool Message_GET_CURRENT_PIDLIMITS::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        
        std::uint8_t dd[7] = {0};
        // fill the 6 bytes
        std::uint8_t datalen = 6;
        
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pidlimits.offset, &dd[0]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pidlimits.limitonoutput, &dd[2]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pidlimits.limitonintegral, &dd[4]);    

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_CURRENT_PIDLIMITS), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
 

    bool Message_SET_VELOCITY_PIDLIMITS::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_VELOCITY_PIDLIMITS) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.pidlimits.type = embot::prot::can::motor::PIDtype::VEL;
        info.pidlimits.offset = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[0]);
        info.pidlimits.limitonoutput = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[2]);
        info.pidlimits.limitonintegral = embot::core::binary::word::memory2value<int16_t>(&candata.datainframe[4]);
                     
        return true;         
    }                    
        
    bool Message_SET_VELOCITY_PIDLIMITS::reply()
    {
        return false;
    } 

    bool Message_GET_VELOCITY_PIDLIMITS::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_VELOCITY_PIDLIMITS) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        
        return true;
    }  

    bool Message_GET_VELOCITY_PIDLIMITS::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        
        std::uint8_t dd[7] = {0};
        // fill the 6 bytes
        std::uint8_t datalen = 6;
        
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pidlimits.offset, &dd[0]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pidlimits.limitonoutput, &dd[2]);
        embot::core::binary::word::load2memory<int16_t>(replyinfo.pidlimits.limitonintegral, &dd[4]);       

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_VELOCITY_PIDLIMITS), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }    


    bool Message_SET_PID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.descriptor.load(&candata.datainframe[0]);
                 
        return true;         
    }  
    
    
    bool Message_SET_PID::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address to, Address sender)
    {
        constexpr std::uint8_t datalen {7};        
        std::uint8_t dd[datalen] = {0};
         
        memmove(&dd[0], info.descriptor.getstream(), datalen);
        uint8_t mcindex = convert(info.motorindex);
        
        outframe.clear();
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::SET_PID), to, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);        
                
        return true;
    }  

    bool Message_SET_PID::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(static_cast<std::uint8_t>(CMD::SET_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        Message msg {inframe};
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.descriptor.load(&msg.candata.datainframe[0]);

        return true;   
    } 

    void Message_SET_PID::Examples::sender()
    {
        // the sender board has address = 0. but it could be any ...
        constexpr Address ownaddress {0};
        // we decide what we want to impose to another board w/ address 3.
        // for instance CURR PID KP equal 3.3         
        embot::prot::can::motor::pid::Descriptor des {embot::prot::can::motor::pid::Type::CURR, embot::prot::can::motor::pid::Param::KP, 3.3};
        Info info {des, MotIndex::one};
        Address destination {3}; 
        // we prepare a outframe
        embot::prot::can::Frame outframe {}; 
        Message_SET_PID::Sender::form(outframe, info, destination, ownaddress);
        // we transmit outframe and that's it
    }
    
    void Message_SET_PID::Examples::receiver()
    {
        constexpr Address ownaddress {3};
        // we have received an inframe w/ a SET_PID<> from a given board and we want to extract a Info from it
        embot::prot::can::Frame inframe {};
        Address senderaddress = frame2sender(inframe);
            
        Info info {};
        Message_SET_PID::Receiver::parse(info, inframe);
        // inside info is what we are asked to set
        MotIndex motorindex = info.motorindex;
        embot::prot::can::motor::pid::Type type = info.descriptor.getType();
        embot::prot::can::motor::pid::Param param = info.descriptor.getParam();   
    }       
    

    
    
    bool Message_GET_PID::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.type = static_cast<pid::Type>(candata.datainframe[0]);
        info.param = static_cast<pid::Param>(candata.datainframe[1]);
        
        return true;
    }  

    bool Message_GET_PID::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        constexpr std::uint8_t datalen = 7;
        std::uint8_t dd[datalen] = {0};
        
        memmove(&dd[0], replyinfo.descriptor.getstream(), datalen);    

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_PID), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }      


    bool Message_GET_PID::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address to, Address sender)
    {
        constexpr std::uint8_t datalen {2};        
        std::uint8_t dd[datalen] = {0};
        
        dd[0] = embot::core::tointegral(info.type);
        dd[1] = embot::core::tointegral(info.param);            

        uint8_t mcindex = convert(info.motorindex);
        
        outframe.clear();
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_PID), to, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);        
                
        return true;
    }  

    bool Message_GET_PID::Sender::parse(ReplyInfo &repinfo, const embot::prot::can::Frame &inframe)
    {
        if(static_cast<std::uint8_t>(CMD::GET_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        Message msg {inframe};
        
        repinfo.motorindex = motorpollingframe2motindex(inframe);
        repinfo.descriptor.load(&msg.candata.datainframe[0]);

        return true;   
    }   
    
    bool Message_GET_PID::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(static_cast<std::uint8_t>(CMD::GET_PID) != frame2cmd(inframe))
        {
            return false; 
        }
        
        Message msg {inframe};
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.type = static_cast<pid::Type>(msg.candata.datainframe[0]);
        info.param = static_cast<pid::Param>(msg.candata.datainframe[1]);

        return true;   
    }
    
    bool Message_GET_PID::Receiver::form(embot::prot::can::Frame &outframe, const ReplyInfo &repinf, Address to, Address sender)
    {        
        constexpr std::uint8_t datalen {7};        
        std::uint8_t dd[datalen] = {0};
        
        memmove(&dd[0], repinf.descriptor.getstream(), datalen);
                
        uint8_t mcindex = convert(repinf.motorindex);
        
        outframe.clear();
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_PID), to, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);        
                
        return true;
    }   
 
    
    void Message_GET_PID::Examples::sender()
    {
        // the sender board has address = 0. but it could be any ...
        constexpr Address ownaddress {0};
        // we decide what we want to ask to another board w/ address 3
        Info info {embot::prot::can::motor::pid::Type::CURR, embot::prot::can::motor::pid::Param::KP, MotIndex::one};
        Address destination {3}; 
        // we prepare a outframe
        embot::prot::can::Frame outframe {}; 
        Message_GET_PID::Sender::form(outframe, info, destination, ownaddress);
        // we transmit outframe and we wait for a inframe from which we extract the ReplyInfo
        embot::prot::can::Frame inframe {};
        ReplyInfo replyinfo {};
        Message_GET_PID::Sender::parse(replyinfo, inframe);
        // we use replyinfo
        float currKP = 0.0;
        if((embot::prot::can::motor::pid::Type::CURR == replyinfo.descriptor.getType()) && 
           (embot::prot::can::motor::pid::Param::KP == replyinfo.descriptor.getParam()) &&
           (MotIndex::one == replyinfo.motorindex))
        {
            currKP = replyinfo.descriptor.getValue(); 
        }        
    }
    
    void Message_GET_PID::Examples::receiver()
    {
        constexpr Address ownaddress {3};
        // we have received an inframe w/ a GET_PID<> from a given board and we want to extract a Info from it
        embot::prot::can::Frame inframe {};
        Address senderaddress = frame2sender(inframe);
            
        Info info {};
        Message_GET_PID::Receiver::parse(info, inframe);
        // inside info is what we are asked. suppose CURR and KD for motor one and suppose that the value is 3.3
        // we prepare a ReplyInfo 
        embot::prot::can::motor::pid::Descriptor des {embot::prot::can::motor::pid::Type::CURR, embot::prot::can::motor::pid::Param::KP, 3.3};
        ReplyInfo replyinfo {des, MotIndex::one};
        // we fill a outframe that we send back          
        embot::prot::can::Frame outframe {}; 
        Message_GET_PID::Sender::form(outframe, info, senderaddress, ownaddress);
        // we transmit outframe and that's it!      
    }    
    

    
    bool Message_SET_MOTOR_PARAM::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_MOTOR_PARAM) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.descriptor.load(&candata.datainframe[0]);
                
        return true;         
    }   


    bool Message_SET_MOTOR_PARAM::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address to, Address sender)
    {
        constexpr std::uint8_t datalen {7};        
        std::uint8_t dd[datalen] = {0};
         
        memmove(&dd[0], info.descriptor.getstream(), datalen);
        uint8_t mcindex = convert(info.motorindex);
        
        outframe.clear();
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::SET_MOTOR_PARAM), to, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);        
                
        return true;
    }  

    bool Message_SET_MOTOR_PARAM::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(static_cast<std::uint8_t>(CMD::SET_MOTOR_PARAM) != frame2cmd(inframe))
        {
            return false; 
        }
        
        Message msg {inframe};
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.descriptor.load(&msg.candata.datainframe[0]);

        return true;   
    } 

    void Message_SET_MOTOR_PARAM::Examples::sender()
    {
        // the sender board has address = 0. but it could be any ...
        constexpr Address ownaddress {0};
        
        // suppose we want to set a vBMEF w/ kbemf = 3.0
        motorparam::Data<motorparam::vBEMF> bmef {};
        bmef.value().kbemf = 3.0;
                        
        embot::prot::can::motor::motorparam::Descriptor descriptor {motorparam::ID::BEMF, bmef.serialize()};
        Info info {descriptor, MotIndex::one};

        Address destination {3}; 
        // we prepare a outframe
        embot::prot::can::Frame outframe {}; 
        Message_SET_MOTOR_PARAM::Sender::form(outframe, info, destination, ownaddress);
        // we transmit outframe and that's it
    }
    
    void Message_SET_MOTOR_PARAM::Examples::receiver()
    {
        constexpr Address ownaddress {3};
        // we have received an inframe w/ a SET_PID<> from a given board and we want to extract a Info from it
        embot::prot::can::Frame inframe {};
        Address senderaddress = frame2sender(inframe);
            
        Info info {};
        Message_SET_MOTOR_PARAM::Receiver::parse(info, inframe);
        // inside info is what we are asked to set
        MotIndex motorindex = info.motorindex;
        motorparam::ID id = info.descriptor.getID();        
        switch(id)
        {
            case motorparam::ID::BEMF:
            {
                motorparam::Data<motorparam::vBEMF> bmef {}; 
                bmef.load(info.descriptor.getdata());
                float kbmef = bmef.value().kbemf; 
                // and now info.motorindex will use kbmef                    
            } break;
            
            default: {} break;
        }         
    }     
        


   
    bool Message_GET_MOTOR_PARAM::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_MOTOR_PARAM) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.id = motorparam::toID(candata.datainframe[0]);
        
        return true;
    }  

    bool Message_GET_MOTOR_PARAM::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        constexpr std::uint8_t datalen {7};
        std::uint8_t dd[datalen] = {0};

        memmove(&dd[0], replyinfo.descriptor.getstream(), datalen);    

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_MOTOR_PARAM), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    } 

    bool Message_GET_MOTOR_PARAM::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address to, Address sender)
    {
        constexpr std::uint8_t datalen {1};        
        std::uint8_t dd[datalen] = {0};
        
        dd[0] = embot::core::tointegral(info.id);
           
        uint8_t mcindex = convert(info.motorindex);
        
        outframe.clear();
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_MOTOR_PARAM), to, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);        
                
        return true;
    }  

    bool Message_GET_MOTOR_PARAM::Sender::parse(ReplyInfo &repinfo, const embot::prot::can::Frame &inframe)
    {
        if(static_cast<std::uint8_t>(CMD::GET_MOTOR_PARAM) != frame2cmd(inframe))
        {
            return false; 
        }
        
        Message msg {inframe};
        
        repinfo.motorindex = motorpollingframe2motindex(inframe);
        repinfo.descriptor.load(&msg.candata.datainframe[0]);

        return true;   
    }   
    
    bool Message_GET_MOTOR_PARAM::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(static_cast<std::uint8_t>(CMD::GET_MOTOR_PARAM) != frame2cmd(inframe))
        {
            return false; 
        }
        
        Message msg {inframe};
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.id = motorparam::toID(msg.candata.datainframe[0]);

        return true;   
    }
    
    bool Message_GET_MOTOR_PARAM::Receiver::form(embot::prot::can::Frame &outframe, const ReplyInfo &repinf, Address to, Address sender)
    {        
        constexpr std::uint8_t datalen {7};        
        std::uint8_t dd[datalen] = {0};
        
        memmove(&dd[0], repinf.descriptor.getstream(), datalen);
                
        uint8_t mcindex = convert(repinf.motorindex);
        
        outframe.clear();
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_MOTOR_PARAM), to, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);        
                
        return true;
    }   
 
    
    void Message_GET_MOTOR_PARAM::Examples::sender()
    {
        // the sender board has address = 0. but it could be any ...
        constexpr Address ownaddress {0};
        // we decide what we want to ask to another board w/ address 3   
        Info info {motorparam::ID::BEMF, MotIndex::one};
        Address destination {3}; 
        // we prepare a outframe
        embot::prot::can::Frame outframe {}; 
        Message_GET_MOTOR_PARAM::Sender::form(outframe, info, destination, ownaddress);
        // we transmit outframe and we wait for a inframe from which we extract the ReplyInfo
        embot::prot::can::Frame inframe {};
        ReplyInfo replyinfo {};
        Message_GET_MOTOR_PARAM::Sender::parse(replyinfo, inframe);
        // we use replyinfo
        motorparam::ID id = replyinfo.descriptor.getID();  
        if((motorparam::ID::BEMF == id) && (MotIndex::one == replyinfo.motorindex))
        {
            motorparam::Data<motorparam::vBEMF> bmef { replyinfo.descriptor.getdata() };
            // get get and use the value 
            float kbemf = bmef.value().kbemf; 
        }            
    }
    
    void Message_GET_MOTOR_PARAM::Examples::receiver()
    {
        constexpr Address ownaddress {3};
        // we have received an inframe w/ a GET_MOTOR_PARAM<> from a given board and we want to extract a Info from it
        embot::prot::can::Frame inframe {};
        Address senderaddress = frame2sender(inframe);
            
        Info info {};
        Message_GET_MOTOR_PARAM::Receiver::parse(info, inframe);
        // inside info is what we are asked. suppose BMEF and the value is 3.3
        motorparam::Data<motorparam::vBEMF> databmef {};
        databmef.value().kbemf = 3.3;
            
        // we prepare a ReplyInfo 
        embot::prot::can::motor::motorparam::Descriptor des { motorparam::ID::BEMF, databmef.serialize() };
        ReplyInfo replyinfo {des, MotIndex::one};
        // we fill a outframe that we send back          
        embot::prot::can::Frame outframe {}; 
        Message_GET_MOTOR_PARAM::Sender::form(outframe, info, senderaddress, ownaddress);
        // we transmit outframe and that's it!      
    }    

    
 
    bool Message_SET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.descriptor.load(&candata.datainframe[0]);
                
        return true;         
    }   


    bool Message_SET::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address to, Address sender)
    {
        constexpr std::uint8_t datalen {7};        
        std::uint8_t dd[datalen] = {0};
         
        memmove(&dd[0], info.descriptor.getstream(), datalen);
        uint8_t mcindex = convert(info.motorindex);
        
        outframe.clear();
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::SET), to, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);        
                
        return true;
    }  

    bool Message_SET::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(static_cast<std::uint8_t>(CMD::SET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        Message msg {inframe};
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.descriptor.load(&msg.candata.datainframe[0]);

        return true;   
    } 

    void Message_SET::Examples::sender()
    {
        // the sender board has address = 0. but it could be any ...
        constexpr Address ownaddress {0};
        
        // suppose we want to set a vTargetPOS w/ {position, withvelocity} = {3.0, 2.0}
        generic::Data<generic::vTargetPOS> tp {};
        tp.value().position = 3.0;
        tp.value().withvelocity = 2.0;
                        
        embot::prot::can::motor::generic::Descriptor descriptor {generic::ID::TargetPOS, tp.serialize()};
        Info info {descriptor, MotIndex::one};

        Address destination {3}; 
        // we prepare a outframe
        embot::prot::can::Frame outframe {}; 
        Message_SET::Sender::form(outframe, info, destination, ownaddress);
        // we transmit outframe and that's it
    }
    
    void Message_SET::Examples::receiver()
    {
        constexpr Address ownaddress {3};
        // we have received an inframe w/ a SET<> from a given board and we want to extract a Info from it
        embot::prot::can::Frame inframe {};
        Address senderaddress = frame2sender(inframe);
            
        Info info {};
        Message_SET::Receiver::parse(info, inframe);
        // inside info is what we are asked to set
        MotIndex motorindex = info.motorindex;
        generic::ID id = info.descriptor.getID();        
        switch(id)
        {
            case generic::ID::TargetPOS:
            {
                generic::Data<generic::vTargetPOS> tp {}; 
                tp.load(info.descriptor.getdata());
                float pos = tp.value().position; 
                // and now info.motorindex will use pos                    
            } break;
            
            default: {} break;
        }         
    }     
        


   
    bool Message_GET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.id = generic::toID(candata.datainframe[0]);
        
        return true;
    }  

    bool Message_GET::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        constexpr std::uint8_t datalen {7};
        std::uint8_t dd[datalen] = {0};

        memmove(&dd[0], replyinfo.descriptor.getstream(), datalen);    

        uint8_t mcindex = convert(replyinfo.motorindex);
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    } 

    bool Message_GET::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address to, Address sender)
    {
        constexpr std::uint8_t datalen {1};        
        std::uint8_t dd[datalen] = {0};
        
        dd[0] = embot::core::tointegral(info.id);
           
        uint8_t mcindex = convert(info.motorindex);
        
        outframe.clear();
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET), to, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);        
                
        return true;
    }  

    bool Message_GET::Sender::parse(ReplyInfo &repinfo, const embot::prot::can::Frame &inframe)
    {
        if(static_cast<std::uint8_t>(CMD::GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        Message msg {inframe};
        
        repinfo.motorindex = motorpollingframe2motindex(inframe);
        repinfo.descriptor.load(&msg.candata.datainframe[0]);

        return true;   
    }   
    
    bool Message_GET::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(static_cast<std::uint8_t>(CMD::GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        Message msg {inframe};
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.id = generic::toID(msg.candata.datainframe[0]);

        return true;   
    }
    
    bool Message_GET::Receiver::form(embot::prot::can::Frame &outframe, const ReplyInfo &repinf, Address to, Address sender)
    {        
        constexpr std::uint8_t datalen {7};        
        std::uint8_t dd[datalen] = {0};
        
        memmove(&dd[0], repinf.descriptor.getstream(), datalen);
                
        uint8_t mcindex = convert(repinf.motorindex);
        
        outframe.clear();
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET), to, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);        
                
        return true;
    }   
 
    
    void Message_GET::Examples::sender()
    {
        // the sender board has address = 0. but it could be any ...
        constexpr Address ownaddress {0};
        // we decide what we want to ask to another board w/ address 3   
        Info info {generic::ID::TargetPOS, MotIndex::one};
        Address destination {3}; 
        // we prepare a outframe
        embot::prot::can::Frame outframe {}; 
        Message_GET::Sender::form(outframe, info, destination, ownaddress);
        // we transmit outframe and we wait for a inframe from which we extract the ReplyInfo
        embot::prot::can::Frame inframe {};
        ReplyInfo replyinfo {};
        Message_GET::Sender::parse(replyinfo, inframe);
        // we use replyinfo
        generic::ID id = replyinfo.descriptor.getID();  
        if((generic::ID::TargetPOS == id) && (MotIndex::one == replyinfo.motorindex))
        {
            generic::Data<generic::vTargetPOS> tp { replyinfo.descriptor.getdata() };
            // get get and use the value 
            float pos = tp.value().position; 
            float withvel = tp.value().withvelocity;
        }            
    }
    
    void Message_GET::Examples::receiver()
    {
        constexpr Address ownaddress {3};
        // we have received an inframe w/ a GET<> from a given board and we want to extract a Info from it
        embot::prot::can::Frame inframe {};
        Address senderaddress = frame2sender(inframe);
            
        Info info {};
        Message_GET::Receiver::parse(info, inframe);
        // inside info is what we are asked. suppose BMEF and the value is 3.3
        generic::Data<generic::vTargetPOS> tp {};
        tp.value().position = 3.3;
        tp.value().withvelocity = 1.0;
            
        // we prepare a ReplyInfo 
        embot::prot::can::motor::generic::Descriptor des { generic::ID::TargetPOS, tp.serialize() };
        ReplyInfo replyinfo {des, MotIndex::one};
        // we fill a outframe that we send back          
        embot::prot::can::Frame outframe {}; 
        Message_GET::Sender::form(outframe, info, senderaddress, ownaddress);
        // we transmit outframe and that's it!      
    }    
    
    
}   // namespace embot::prot::can::motor::polling {
    
    

    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


