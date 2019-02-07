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

#ifndef _EMBOT_APP_APPLICATION_THECANPARSERSKIN_H_
#define _EMBOT_APP_APPLICATION_THECANPARSERSKIN_H_

#include "embot_common.h"
#include "embot_hw.h"
#include "embot_hw_can.h"
#include "embot_sys.h"
#include <vector>
#include <memory>

namespace embot { namespace app { namespace application {
           
    class theCANparserSkin
    {
    public:
        static theCANparserSkin& getInstance();
        
        
    public:
        struct Config
        {
            std::uint32_t   dummy;
            Config() : dummy(0) {}
        }; 
        
        
        bool initialise(Config &config); 
        
        // returns true if the canframe has been recognised. if so, any reply is sent if replies.size() > 0
        bool process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);

    private:
        theCANparserSkin(); 
        ~theCANparserSkin(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
