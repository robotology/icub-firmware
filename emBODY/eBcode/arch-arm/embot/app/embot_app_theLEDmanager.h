/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EMBOT_APP_THELEDMANAGER_H_
#define _EMBOT_APP_THELEDMANAGER_H_

#include "embot_common.h"
#include "embot_sys.h"
#include "embot_hw_led.h"

#include <initializer_list>
#include <memory>

namespace embot { namespace app {
    
    //using namespace embot::hw::led;

    // safe but does not copy        
    class LEDhandle 
    {
    public:
        // interface
        virtual bool supported() const = 0;
        virtual embot::hw::LED id() const = 0;
        virtual void on() = 0;
        virtual void off() = 0;
        virtual void toggle() = 0; 
        virtual void pulse(embot::common::relTime period, std::uint32_t times = 0) = 0;
        
    protected:
        virtual ~LEDhandle() {};    // cannot delete from interface but only from derived object
    };
    
    class theLEDmanager 
    {
    public:
    
        static theLEDmanager& getInstance();
               
        bool init(const std::initializer_list<embot::hw::LED> &leds);


        bool initialised(embot::hw::LED led) const;    
        LEDhandle & get(embot::hw::LED led) const;
                
    public:   
        ~theLEDmanager();

    private:
        theLEDmanager();  

    public:
        // even if i could allow a deinit, i prefer not adding it because i cannot find a way to disable the handle return by get()
        bool deinit(const std::initializer_list<embot::hw::LED> &leds);
        bool deinit(); 
    
    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };  


}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
