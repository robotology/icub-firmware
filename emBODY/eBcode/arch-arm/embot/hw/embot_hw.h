
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

#ifndef _EMBOT_HW_H_
#define _EMBOT_HW_H_

#include "embot_common.h"
#include "embot_i2h.h"

#include <cstring>

namespace embot { namespace hw {
    
    using result_t = std::int8_t;
    
    const result_t resOK   = 0;
    const result_t resNOK  = -1;
    
}} // namespace embot { namespace hw {

namespace embot { namespace hw { namespace bsp {
    
        
    bool isinitted();
    
    result_t init();
      
}}} // namespace embot { namespace hw { namespace bsp {



namespace embot { namespace hw { namespace bsp {

    // we need at least one init where to initialise everything related to the board ...
    // we also may need some unique labels ... for instance led0 etc ...
    
    enum class LED { zero = 0, one = 1, two = 2, three = 3, none = 255 };
    enum class BTN { zero = 0, one = 1, two = 2, three = 3, none = 255 };
    
    result_t led_init_par(LED led, const void *par);
    result_t led_init(LED led);
    result_t led_on(LED led);
    result_t led_off(LED led);
    result_t led_toggle(LED led);
    
    result_t btn_init(BTN btn);
    bool btn_ispressed(BTN btn);
    
}}} // namespace embot { namespace hw { namespace bsp 

namespace embot { namespace hw { namespace sys {
    
    const std::uint32_t startOfFLASH            = 0x08000000;
    const std::uint32_t addressOfBootloader     = 0x08000000;
    const std::uint32_t maxsizeOfBootloader     = 124*1024;
    const std::uint32_t addressOfStorage        = 0x0801F000;
    const std::uint32_t maxsizeOfStorage        = 4*1024;
    const std::uint32_t addressOfApplication    = 0x08020000;
    const std::uint32_t maxsizeOfApplication    = 128*1024;
    
    std::uint32_t clock();
    
    void reset();
    
    bool canjump2address(std::uint32_t address);
    
    void jump2address(std::uint32_t address); // if it returns ... means that it has failed the jump because internally canjump2address() returns false
    
    void relocatevectortable(std::uint32_t offset);
    
    void delay(embot::common::Time t);
    
}}} // namespace embot { namespace hw { namespace sys {



namespace embot { namespace hw { namespace can {
    
    struct Frame            
    {
        std::uint32_t       id;
        std::uint8_t        size;
        std::uint8_t        filler[3];
        std::uint8_t        data[8];  
        Frame() : id(0), size(0) { std::memset(data, 0, sizeof(data)); }
    };
    
    enum class Port { one = 0, two = 1 };
    enum class PortNumber { value = 2 };
    
    struct Config
    {
        std::uint8_t                txcapacity;
        std::uint8_t                rxcapacity;
        embot::common::Callback     ontxframe; 
        embot::common::Callback     txqueueempty; 
        embot::common::Callback     onrxframe;
        Config() : txcapacity(8), rxcapacity(8), ontxframe(nullptr, nullptr), txqueueempty(nullptr, nullptr), onrxframe(nullptr, nullptr) {}
    };
    
    bool supported(Port p);
    
    bool initted(Port p);
    
    result_t init(Port p, const Config &config);
    
    result_t enable(Port p);
    
    result_t disable(Port p);
    
    result_t put(Port p, const Frame &frame);
    
    std::uint8_t outputqueuesize(Port p);
    
    result_t transmit(Port p);
    
    std::uint8_t inputqueuesize(Port p);
    
    result_t get(Port p, Frame &frame, std::uint8_t &remaining);
    
    void rxHandler(Port p);
    
    
}}} // namespace embot { namespace hw { namespace can {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


