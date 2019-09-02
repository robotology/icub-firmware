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

#ifndef _EMBOT_APP_BOOTLOADER_THECANPARSER_H_
#define _EMBOT_APP_BOOTLOADER_THECANPARSER_H_

#include "embot_common.h"
#include "embot_hw.h"
#include "embot_hw_sys.h"
#include "embot_hw_can.h"
#include "embot_sys.h"
#include "embot_app_canprotocol.h"
#include <memory>
#include <vector>

namespace embot { namespace app { namespace bootloader {
           
    class theCANparser
    {
    public:
        static theCANparser& getInstance();
        
        
    public:
        struct Config
        {
            embot::app::canprotocol::Process owner {embot::app::canprotocol::Process::bootloader};
            Config() = default;
            Config(embot::app::canprotocol::Process pr) : owner (pr) {}
        }; 
        
        enum class Event { rxcanframe = 0};
        
        
        bool initialise(Config &config); 
        
        //bool process(embot::app::bootloader::theCANparser::Event evt);
        
        // in bootloader there are not multiple replies (as for reply to GET_ADDITIONAL_INFO or CANPRINT).
        // thus we dont need to use a vector ... but if we decide to support multiple replies ... 
        // in any case, if we must transmit we return true
        //bool process(const embot::hw::can::Frame &frame, vector<embot::hw::can::Frame> &replies);
        bool process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);

    private:
        theCANparser(); 
        ~theCANparser();


    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;         
    };       


}}} // namespace embot { namespace app { namespace bootloader


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
