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

#ifndef _EMBOT_SYS_THEJUMPER_H_
#define _EMBOT_SYS_THEJUMPER_H_

#include "embot_common.h"

#include "embot_sys.h"

namespace embot { namespace sys {
    
    
    
    class theJumper
    {
    public:
        static theJumper& getInstance()
        {
            static theJumper* p = new theJumper();
            return *p;
        }
        
    public:
        struct Config
        {
            embot::common::fpWorker             restart;
            embot::common::fp_int8_par_uint32   jump2;    
            Config() : restart(nullptr), jump2(nullptr) {}
            Config(embot::common::fpWorker _restart, embot::common::fp_int8_par_uint32 _jump2) : restart(_restart), jump2(_jump2) {}
        }; 
        
        bool init(Config &config);
        
        bool eval(std::uint32_t &address, bool jumpnow = false);
        bool jump(std::uint32_t address);
        bool set(std::uint32_t address);
        bool theJumper::restart();
        

    private:
        theJumper();  

    public:
        // remove copy constructors and copy assignment operators
        theJumper(const theJumper&) = delete;
        theJumper(theJumper&) = delete;
        void operator=(const theJumper&) = delete;
        void operator=(theJumper&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
