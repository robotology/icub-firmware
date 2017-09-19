
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

#ifndef _EMBOT_HW_ADC_H_
#define _EMBOT_HW_ADC_H_

#include "embot_common.h"
#include "embot_hw.h"


namespace embot { namespace hw { namespace adc {
 
    // the adc wrapper is very basic and tailored to the need (so far) of the strain2 board.
    // its api will be surely improved when new needs come
    
    enum class Port { one = 0, none = 32, maxnumberof = 1};
    enum class ItemSize { U16 = 2 };
    enum class Mode { dma = 0 }; 
    enum class Acquisition { single = 0, continuous = 1 };
    
    
    struct Config
    {
        Mode                        mode;
        Acquisition                 acquisition;
        ItemSize                    itemsize;
        void*                       destination;
        std::uint8_t                numberofitems;
        embot::common::Callback     oncompletion; 
        Config() : mode(Mode::dma), acquisition(Acquisition::single), itemsize(ItemSize::U16), destination(nullptr), numberofitems(0), oncompletion(nullptr, nullptr) {}
    };
    
    bool supported(Port p);
    
    bool initialised(Port p);
    
    result_t init(Port p, const Config &config);
    
    
    // they all change the initial configuration after the call of init(). if acquisition is running it is stopped.
    result_t configure(Port p, void *destination, std::uint8_t numberofitems); 
    result_t configure(Port p, Acquisition acquisition);    
    result_t configure(Port p, const embot::common::Callback &oncompletion);
    
    // on strain2 , a single acquisition of the 6 channels takes 560 usec, hence the oncompletion() callback is required.
    result_t start(Port p);
    
    // tells is an acquisition is ongoing. 
    // in case of Acquisition::single it returns false as soon as the acquision is terminated. 
    // in case of Acquisition::continuous it returns true until a stop() is called. 
    bool isrunning(Port p);
    
    result_t get(Port p, void *items);
    
    result_t stop(Port p);   
 
}}} // namespace embot { namespace hw { namespace adc {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


