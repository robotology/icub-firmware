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

#ifndef _EMBOT_APP_APPLICATION_THECANTRACER_H_
#define _EMBOT_APP_APPLICATION_THECANTRACER_H_

#include "embot_common.h"

#include "embot_hw.h"

#include "embot_sys.h"

#include "embot_app_canprotocol.h"


#include <vector>
#include <string>

namespace embot { namespace app { namespace application {
           
    class theCANtracer
    {
    public:
        static theCANtracer& getInstance()
        {
            static theCANtracer* p = new theCANtracer();
            return *p;
        }
        
        
    public:
        struct Config
        {
            std::uint8_t        canaddress;
            Config() : canaddress(1) {}
        }; 
        
        
        bool initialise(Config &config);   

        
        embot::common::Time start();
        embot::common::relTime stop(const std::string &prefix, std::vector<embot::hw::can::Frame> &frames, embot::common::Time started = 0);    
        bool print(const std::string &text, std::vector<embot::hw::can::Frame> &frames);        

    private:
        theCANtracer(); 

    public:
        // remove copy constructors and copy assignment operators
        theCANtracer(const theCANtracer&) = delete;
        theCANtracer(theCANtracer&) = delete;
        void operator=(const theCANtracer&) = delete;
        void operator=(theCANtracer&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
