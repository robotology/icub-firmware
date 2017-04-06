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
#include "embot_hw.h"



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
        
        struct StackSizes
        {   
            std::uint16_t       tasksysteminit;
            std::uint16_t       taskonidle; 
            StackSizes() : tasksysteminit(2048), taskonidle(512) {}  
            StackSizes(std::uint16_t sys, std::uint16_t idl) : tasksysteminit(sys), taskonidle(idl) {}                 
        };
        
        struct UserDefOperations
        {   
            embot::common::Callback     atsysteminit;
            embot::common::Callback     onidle; 
            embot::common::Callback     onfatalerror;
            UserDefOperations() : atsysteminit(nullptr, nullptr), onidle(nullptr, nullptr), onfatalerror(nullptr, nullptr) {}
            UserDefOperations(const embot::common::Callback &atsys, const embot::common::Callback &onid, const embot::common::Callback &onerr) : 
                        atsysteminit(atsys), onidle(onid), onfatalerror(onerr) 
                        {}                   
        };
        
        struct Config
        {            
            embot::common::relTime              osaltickperiod;
            StackSizes                          stacksizes;
            UserDefOperations                   operations;
            std::uint32_t                       addressofapplication;
            Config() :
                        osaltickperiod(embot::common::time1millisec),
                        // for structs we rely on their default ctor
                        addressofapplication(embot::hw::sys::addressOfApplication)
                        {}
            Config(embot::common::relTime _osaltickperiod, const StackSizes &_stacksizes, const UserDefOperations &_operations, std::uint32_t address = embot::hw::sys::addressOfApplication) :
                        osaltickperiod(_osaltickperiod), 
                        stacksizes(_stacksizes), 
                        operations(_operations),
                        addressofapplication(address)            
                        {}
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
