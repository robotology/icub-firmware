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

#ifndef _EMBOT_PROT_CAN_MOTOR_PERIODIC_H_
#define _EMBOT_PROT_CAN_MOTOR_PERIODIC_H_

#include "embot_core.h"

#include "embot_prot_can.h"

#include <cstring>


    
namespace embot { namespace prot { namespace can { namespace motor { namespace periodic {
  
    // the supported commands    
    enum class CMD { 
        none = 0xfe, 
        
        PRINT = 6,
        EMSTO2FOC_DESIRED_CURRENT = 15        
    };

    // NOTES
    // - only a commands subset is supported. at date of 23 aug 17 there is only the ICUBCANPROTO_PER_MC_MSG__PRINT

    
    // some utilities
    bool supported(std::uint8_t cmd);        
    CMD convert(std::uint8_t cmd);
    std::uint8_t convert(CMD cmd);    
       
}}}}} // namespace embot { namespace prot { namespace can { namespace motor { namespace periodic { 
       

  
  
namespace embot { namespace prot { namespace can { namespace motor { namespace periodic {
         
    // the management of commands
    
    class Message_PRINT : public Message
    {
        public:
            
        struct Info
        { 
            char                        text[31];   // 30 chars + '0' : at most 5 can messages ('0' is not transmitted)
            std::uint8_t                canaddress;
            Info() { std::memset(text, 0, sizeof(text)); canaddress = 0; }
        };
        
        Info info;
        
        Message_PRINT() { framecounter = nframes = nchars = 0; }
            
        bool load(const Info& inf);
            
       // bool get(std::vector<embot::prot::can::Frame> &frames); 
            
        std::uint8_t numberofframes();
        bool get(embot::prot::can::Frame &outframe);  

        private:
        
        std::uint8_t nframes;    
        std::uint8_t framecounter;    
        std::uint8_t nchars;           
        static std::uint8_t textIDmod4;  // 0, 1, 2, 3, 0, 1, 2, etc      
    }; 

    
    class Message_EMSTO2FOC_DESIRED_CURRENT : public Message
    {
        public:
            
        struct Info
        {
            int16_t current[4] {0};   
            Info() = default;
        };
        
        Info info {};
        
        Message_EMSTO2FOC_DESIRED_CURRENT() = default;
            
        bool load(const embot::prot::can::Frame &inframe);                        
    }; 

    
}}}}} // namespace embot { namespace prot { namespace can { namespace motor { namespace periodic {    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
