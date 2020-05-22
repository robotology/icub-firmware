
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

#ifndef _EMBOT_HW_BUTTON_H_
#define _EMBOT_HW_BUTTON_H_

#include "embot_core.h"
#include "embot_hw.h"



namespace embot { namespace hw { namespace button {
    
    enum class Mode : uint8_t 
    { 
        Polling = 0,                    // the gpio is read directly in order to assess if the button is pressed: you must call pressed()
        TriggeredOnPress = 1,           // triggered on press of the button with the aid of an IRQHandler 
        TriggeredOnRelease = 2,         // triggered on release of the button with the aid of an IRQhandler
        TriggeredOnDebouncedRelease = 3 // same as TriggeredOnRelease but triggered only if a a given time has passed in pressed mode
    };
    
    struct Config
    {   
        Mode mode {Mode::Polling};    
        embot::core::Callback callback {nullptr, nullptr}; // not used for Mode::Polling   
        embot::core::Time debouncetime {100*embot::core::time1millisec};           
        constexpr Config() = default;   
        constexpr Config(Mode mo, const embot::core::Callback &ca, embot::core::Time de) : mode(mo), callback(ca), debouncetime(de) {}         
    };    

    bool supported(BTN btn);
    
    bool initialised(BTN btn);
        
    result_t init(BTN btn, const Config &cfg);
    
    const Config & config(BTN btn);
     
        
    bool pressed(BTN btn);
    
    // must put it inside the handler of exti when the pin associated to the button is triggered
    void onexti(BTN btn);
       
}}} // namespace embot { namespace hw { namespace button 






#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


