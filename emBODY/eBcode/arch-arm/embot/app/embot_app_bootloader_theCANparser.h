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


#include <vector>

namespace embot { namespace app { namespace bootloader {
           
    class theCANparser
    {
    public:
        static theCANparser& getInstance()
        {
            static theCANparser* p = new theCANparser();
            return *p;
        }
        
        
    public:
        struct Config
        {
            std::uint32_t   dummy;
            Config() : dummy(0) {}
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

    public:
        // remove copy constructors and copy assignment operators
        theCANparser(const theCANparser&) = delete;
        theCANparser(theCANparser&) = delete;
        void operator=(const theCANparser&) = delete;
        void operator=(theCANparser&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}}} // namespace embot { namespace app { namespace bootloader


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
