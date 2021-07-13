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

#include "embot_hw.h"



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



namespace embot { namespace prot { namespace can { namespace motor { namespace polling {

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
            (1ULL << static_cast<std::uint8_t>(CMD::SET_BOARD_ID)),
            // bits 64-127
            (1ULL << (static_cast<std::uint8_t>(CMD::GET_FIRMWARE_VERSION)-64)),
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
            
            default:
            {
                return "none";
            } break;

        } 

        return "none";
    }        
    
    ControlMode toControlMode(uint8_t v)
    {
        ControlMode cm {ControlMode::none};
        
        switch(v)
        {
            case 0x00:
            {
                cm = ControlMode::Idle;
            } break;

            case 0x06:
            {
                cm = ControlMode::Current;
            } break;
            
            case 0x0A:
            {
                cm = ControlMode::SpeedVoltage;
            } break;            
            
            case 0x50:
            {
                cm = ControlMode::OpenLoop;
            } break;   

            default:
            {
                cm = ControlMode::none;
            } break;

        }  

        return cm;        
    }
    
    uint8_t convert(ControlMode cm)
    {
        return embot::core::tointegral(cm);
    }
    
    std::string tostring(ControlMode cm)
    {

        switch(cm)
        {
            case ControlMode::Idle:
            {
                return "Idle";
            } break;

            case ControlMode::Current:
            {
                return "Current";
            } break;
            
            case ControlMode::SpeedVoltage:
            {
                return "SpeedVoltage";
            } break;

            case ControlMode::OpenLoop:
            {
                return "OpenLoop";
            } break;

            default:
            {
                return "none";
            } break;

        } 

        return "none";
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
        dd[0] = convert(replyinfo.controlmode); 
                       
        std::uint8_t datalen = 1;
        uint8_t mcindex = convert(replyinfo.motorindex);
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingMotorControl, static_cast<std::uint8_t>(CMD::GET_CONTROL_MODE), candata.from, dd, datalen, mcindex);
        frame_set_size(outframe, datalen+1);
        return true;
    }      
        
    
    bool Message_SET_CONTROL_MODE::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CONTROL_MODE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.motorindex = motorpollingframe2motindex(inframe);
        info.controlmode = toControlMode(candata.datainframe[0]);
                      
        return true;         
    }                    
        
    bool Message_SET_CONTROL_MODE::reply()
    {
        return false;
    } 
    
    
}}}}} // namespace embot { namespace prot { namespace can { namespace motor { namespace polling {
    
    

    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


