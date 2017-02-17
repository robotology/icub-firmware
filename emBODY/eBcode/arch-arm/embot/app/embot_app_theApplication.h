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

#ifndef _EMBOT_APP_THEAPPLICATION_H_
#define _EMBOT_APP_THEAPPLICATION_H_

#include "embot_common.h"

#include "embot_sys.h"



namespace embot { namespace app {
       
    
    class theApplication
    {
    public:
        static theApplication& getInstance()
        {
            static theApplication* p = new theApplication();
            return *p;
        }
               
    public:
        struct Config
        {
            embot::common::relTime              osaltickperiod;
            embot::common::Callback             userdeflauncher;
            Config() : osaltickperiod(embot::common::time1millisec), userdeflauncher(nullptr, nullptr) {}
            Config(embot::common::relTime _osaltickperiod, embot::common::fpCallback _userdeflauncher, void* _param) : osaltickperiod(_osaltickperiod), userdeflauncher(_userdeflauncher, _param) {}
        }; 
                      
        void execute(Config &config); // it never returns ..
        
    private:
        theApplication(); 
 
    public:
        // remove copy constructors and copy assignment operators
        theApplication(const theApplication&) = delete;
        theApplication(theApplication&) = delete;
        void operator=(const theApplication&) = delete;
        void operator=(theApplication&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
