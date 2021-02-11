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

#include "embot_prot_can_motor_periodic.h"




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
  
    
    
namespace embot { namespace prot { namespace can { namespace motor { namespace periodic {

      
    bool supported(std::uint8_t cmd)
    {
        return (CMD::none != convert(cmd))? true : false;
    }
        
    CMD convert(std::uint8_t cmd)
    {
        constexpr std::uint16_t mcpermask16 =   (1 << static_cast<std::uint8_t>(CMD::PRINT))                            |
                                                (1 << static_cast<std::uint8_t>(CMD::EMSTO2FOC_DESIRED_CURRENT))        ;

        if(cmd > 15)
        {
            return CMD::none;
        }

        if(true == embot::core::binary::bit::check(mcpermask16, cmd))
        {
            return static_cast<CMD>(cmd);          
        }
        
        return CMD::none;           
    } 

    std::uint8_t convert(CMD cmd)
    {
        return static_cast<std::uint8_t>(cmd);
    }      
    
    
    std::uint8_t Message_PRINT::textIDmod4 = 0;
    
    bool Message_PRINT::load(const Info& inf)
    {
        info = inf;  
        nchars = std::strlen(info.text);
        nframes = (std::strlen(info.text) + 5) / 6;   
        framecounter = 0;  
        textIDmod4++;
        textIDmod4 %= 4;
        
        return (nframes>0) ? true : false;
    }
    
    std::uint8_t Message_PRINT::numberofframes()
    {       
        return nframes;
    }
        
    bool Message_PRINT::get(embot::prot::can::Frame &outframe)
    {
        if((0 == nframes) || (framecounter >= nframes))
        {
            return false;
        }
        
        bool lastframe = ((framecounter+1) == nframes) ? true : false;
        
        std::uint8_t charsinframe = 6;
        if(lastframe)
        {
            charsinframe = nchars - 6*framecounter; // less than 6                    
        }
        
        if(charsinframe > 6)
        {   // just because i am a paranoic
            charsinframe = 6;
        }
        
        std::uint8_t data08[8] = {0};
        data08[0] = lastframe ? (static_cast<std::uint8_t>(CMD::PRINT) + 128) : (static_cast<std::uint8_t>(CMD::PRINT));
        data08[1] = ((textIDmod4 << 4) & 0xF0) | (framecounter & 0x0F);
        std::memmove(&data08[2], &info.text[6*framecounter], charsinframe);
        
        // ok, increment framecounter
        framecounter ++;
        
        Message::set(info.canaddress, 0xf, Clas::periodicMotorControl, static_cast<std::uint8_t>(CMD::PRINT), data08, 2+charsinframe);
        std::memmove(&outframe, &canframe, sizeof(embot::prot::can::Frame));
                    
        return true;
    } 


    bool Message_EMSTO2FOC_DESIRED_CURRENT::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::EMSTO2FOC_DESIRED_CURRENT) != frame2cmd(inframe))
        {
            return false; 
        }
        
        // little endian
        info.current[0] = static_cast<std::uint16_t>(candata.datainframe[0]) | (static_cast<std::uint16_t>(candata.datainframe[1]) << 8);
        info.current[1] = static_cast<std::uint16_t>(candata.datainframe[2]) | (static_cast<std::uint16_t>(candata.datainframe[3]) << 8);
        info.current[2] = static_cast<std::uint16_t>(candata.datainframe[4]) | (static_cast<std::uint16_t>(candata.datainframe[5]) << 8);
        info.current[3] = static_cast<std::uint16_t>(candata.datainframe[6]) | (static_cast<std::uint16_t>(candata.datainframe[7]) << 8);        
      
        return true;         
    }      

}}}}} // namespace embot { namespace prot { namespace can { namespace motor { namespace periodic {
    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


