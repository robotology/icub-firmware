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

#include "embot_app_canprotocol_skin_periodic.h"



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


 
    
namespace embot { namespace app { namespace canprotocol { namespace skin { namespace periodic {

    bool supported(std::uint8_t cmd)
    {
        return (CMD::none != convert(cmd))? true : false;
    }
        
    CMD convert(std::uint8_t cmd)
    {   // we have all messages from TRG00 to TRG15 ... hence we don't use the mask as for insance in the case of motor::periodic or analog::periodic
        if(cmd > 15)
        {
            return CMD::none;
        }
        return static_cast<CMD>(cmd);           
    } 

    std::uint8_t convert(CMD cmd)
    {
        return static_cast<std::uint8_t>(cmd);
    }      
    

    
    bool Message_TRG::load(const Info& inf)
    {
        info = inf;
      
        return true;
    }
        
    bool Message_TRG::get(embot::hw::can::Frame &outframe0, embot::hw::can::Frame &outframe1)
    {
        std::uint8_t data08[8] = {0};
        data08[0] = 0x40;
        std::memmove(&data08[1], &info.the12s[0], 7);
        Message::set(info.canaddress, 0xf, Clas::periodicSkin, info.trianglenum, data08, 8);
        std::memmove(&outframe0, &canframe, sizeof(embot::hw::can::Frame));
        
        data08[0] = 0xC0;
        std::memmove(&data08[1], &info.the12s[7], 5);
        // now outofrange and error flags
        std::uint8_t errorflags = 0; 
        // bit ErrorInTriangleBit::noack is set if any bit inside notackmaskofthe12s is set.
        if(0 != info.notackmaskofthe12s)
        {
            embot::binary::bit::set(errorflags, static_cast<std::uint8_t>(ErrorInTriangleBit::noack)); 
        }
        // bit ErrorInTriangleBit::notconnected is set if all 12 bits inside notconnectedmaskofthe12s are set.
        if(12 == embot::binary::bit::count(info.notconnectedmaskofthe12s))
        {
            embot::binary::bit::set(errorflags, static_cast<std::uint8_t>(ErrorInTriangleBit::notconnected)); 
        }            
        data08[6] = static_cast<std::uint8_t>((info.outofrangemaskofthe12s & 0x0ff0) >> 4);
        data08[7] = static_cast<std::uint8_t>((info.outofrangemaskofthe12s & 0x000f) << 4) | (errorflags & 0x0f);
        
        Message::set(info.canaddress, 0xf, Clas::periodicSkin, info.trianglenum, data08, 8);
        std::memmove(&outframe1, &canframe, sizeof(embot::hw::can::Frame));
        
        return true;
    }  

}}}}} // namespace embot { namespace app { namespace canprotocol { namespace skin { namespace periodic {
    
    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


