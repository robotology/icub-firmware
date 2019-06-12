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

#ifndef _EMBOT_APP_THEJUMPER_H_
#define _EMBOT_APP_THEJUMPER_H_

#include "embot_common.h"
#include "embot_sys.h"
#include <new>
#include <memory>

namespace embot { namespace app {
    
        
    class theJumper
    {
    public:
        static theJumper& getInstance()
        {
            static theJumper* p = new theJumper();
            return *p;
        }
        
    public:
        
        enum class Command { none = 0, jump = 1, stay = 2};
                      
        bool set(Command command, std::uint32_t param);
        Command get(std::uint32_t& param);
        bool jump(std::uint32_t address);   
        
        bool restart();
        

    private:
        theJumper(); 
        void *operator new(std::size_t size) noexcept(false);  

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


}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
