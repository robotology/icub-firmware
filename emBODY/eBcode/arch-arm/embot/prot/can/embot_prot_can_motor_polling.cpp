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
    
    
    
}}}}} // namespace embot { namespace prot { namespace can { namespace motor { namespace polling {
    
    

    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


