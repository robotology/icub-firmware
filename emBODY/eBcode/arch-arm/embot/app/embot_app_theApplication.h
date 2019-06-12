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
#include <memory>


namespace embot { namespace app {
       
    
    class theApplication
    {
    public:
        static theApplication& getInstance();
               
    public:
                        
        struct Config
        {            
            embot::common::relTime  ticktime;
            embot::sys::Operation   init;
            embot::sys::Operation   idle;
            embot::sys::Operation   fatal;
            std::uint32_t           addressofapplication;
            Config() :
                        ticktime(embot::common::time1millisec),
                        // for structs we rely on their default ctor
                        addressofapplication(0)
                        {}
            Config(embot::common::relTime _t, const embot::sys::Operation &_ini, const embot::sys::Operation &_idl, const embot::sys::Operation &_fat, std::uint32_t _addr) :
                        ticktime(_t), 
                        init(_ini), 
                        idle(_idl),
                        fatal(_fat),
                        addressofapplication(_addr)            
                        {}
            bool isvalid() const { if((false == init.isvalid()) || (false == idle.isvalid()) || (false == fatal.isvalid()) || (0 == ticktime) || (0 == addressofapplication)) { return false; } else { return true; } } 
        }; 
                      
        bool execute(const Config &config); // it never returns ..
        
    private:
        theApplication(); 
        ~theApplication();

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;         
    };       


}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
