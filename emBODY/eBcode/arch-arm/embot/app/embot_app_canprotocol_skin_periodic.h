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

#ifndef _EMBOT_APP_CANPROTOCOL_SKIN_H_
#define _EMBOT_APP_CANPROTOCOL_SKIN_H_

#include "embot_common.h"

#include "embot_app_canprotocol.h"

#include <cstring>


namespace embot { namespace app { namespace canprotocol { namespace skin { namespace periodic {
    
    // the supported commands    
    enum class CMD { 
        none = 0xfe,
        
        TRG00 = 0, TRG01 = 1, TRG02 = 2, TRG03 = 3, TRG04 = 4, TRG05 = 5, TRG06 = 6, TRG07 = 7, 
        TRG08 = 8, TRG09 = 9, TRG10 = 10, TRG11 = 11, TRG12 = 12, TRG13 = 13, TRG14 = 14, TRG15 = 15
    };
    
    // NOTES
    // - all 16 commands are supported. this class is full.    
    
    // some utilities    
    bool supported(std::uint8_t cmd);        
    CMD convert(std::uint8_t cmd);
    std::uint8_t convert(CMD cmd);
    
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace skin { namespace periodic { 



  
namespace embot { namespace app { namespace canprotocol { namespace skin { namespace periodic {
                  
    // the management of commands    
    
    class Message_TRG : public Message
    {
        public:
            
        enum class ErrorInTriangleBit { noack = 0, notconnected = 1 };
            
        struct Info
        { 
            std::uint8_t                the12s[12];
            std::uint16_t               outofrangemaskofthe12s;         // bit in pos x is 1 if the12s[x] is out of range.
            std::uint16_t               notconnectedmaskofthe12s;       // 0 no error, 1 error of no ack, 2 error of none connected. 
            std::uint16_t               notackmaskofthe12s;
            std::uint8_t                trianglenum;
            std::uint8_t                canaddress;
            Info() { std::memset(the12s, 0, sizeof(the12s)); trianglenum = canaddress = outofrangemaskofthe12s = notconnectedmaskofthe12s = notackmaskofthe12s = 0;}
        };
        
        Info info;
        
        Message_TRG() {}
            
        bool load(const Info& inf);
            
        bool get(embot::hw::can::Frame &outframe0, embot::hw::can::Frame &outframe1);        
    };
    
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace skin { namespace periodic {    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
