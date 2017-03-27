
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


namespace embot { namespace hw {
    
    using result_t = std::int8_t;
    
    const result_t resOK   = 0;
    const result_t resNOK  = -1;
    
    
    struct GPIO
    {   // generic gpio descriptor: every micro has a port and a pin. 
        void*           port;
        std::uint32_t   pin;
    };
    
}} // namespace embot { namespace hw {



namespace embot { namespace hw { namespace bsp {
    
    struct Config
    {
        embot::common::fpGetU32     get1mstick; 
        Config() : get1mstick(nullptr) {}
        Config(embot::common::fpGetU32 _get1mstick) : get1mstick(_get1mstick) {}
    }; 
    
        
    bool initialised();
    
    result_t init(const Config &config);
        
      
}}} // namespace embot { namespace hw { namespace bsp {




namespace embot { namespace hw { namespace led {
    
    // the namespace ::led can be used as template for development of simple hw features.
    // follow these rules:
    //
    // 1. if more that one entity is [may be] available, define an enum with always: none and maxnumberof
    // 2. expose a bool returning supported() function
    // 3. expose a bool returning initialised() funtion
    // 4. expose a result_t returning init() function
    // 5. design the needed functions. they shall return embot::hw::result_t (0 is ok, negative is bad).
    
    enum class LED { zero = 0, one = 1, two = 2, three = 3, four = 4, five = 5, six = 6, seven = 7, none = 32, maxnumberof = 8 };

    bool supported(LED led);
    
    bool initialised(LED led);
        
    result_t init(LED led);
    result_t on(LED led);
    result_t off(LED led);
    result_t toggle(LED led);
        
    result_t init_legacy(LED led, const void *par);
       
}}} // namespace embot { namespace hw { namespace led 



namespace embot { namespace hw { namespace button {
    
    enum class BTN { zero = 0, one = 1, two = 2, three = 3, four = 4, five = 5, six = 6, seven = 7, none = 32, maxnumberof = 8 };

    bool supported(BTN btn);
    
    bool initialised(BTN btn);
        
    result_t init(BTN btn);
        
    bool pressed(BTN btn);
       
}}} // namespace embot { namespace hw { namespace button 



namespace embot { namespace hw { namespace sys {
      
    extern const std::uint32_t startOfFLASH;
    extern const std::uint32_t addressOfBootloader;
    extern const std::uint32_t maxsizeOfBootloader;
    extern const std::uint32_t addressOfStorage;
    extern const std::uint32_t maxsizeOfStorage;
    extern const std::uint32_t addressOfApplication;
    extern const std::uint32_t maxsizeOfApplication;
    
    std::uint32_t clock();
    
    void reset();
    
    bool canjump2address(std::uint32_t address);
    
    void jump2address(std::uint32_t address); // if it returns ... means that it has failed the jump because internally canjump2address() returns false
    
    void relocatevectortable(std::uint32_t offset);
    
    void delay(embot::common::Time t);
    
}}} // namespace embot { namespace hw { namespace sys {


namespace embot { namespace hw { namespace flash {
      
    extern const std::uint32_t startOfFLASH;
    extern const std::uint32_t sizeOfFLASH;
    extern const std::uint32_t sizeOfPAGE;
    extern const std::uint32_t maxNumOfPAGEs;
    
    bool isaddressvalid(std::uint32_t address);    
    std::uint32_t address2page(std::uint32_t address); // returns [0, maxNumOfPAGEs). if address not valid, it returns maxNumOfPAGEs.
    
    bool erase(std::uint32_t page);
    bool erase(std::uint32_t address, std::uint32_t size);
    bool read(std::uint32_t address, std::uint32_t size, void *data);
    bool write(std::uint32_t address, std::uint32_t size, const void *data);
    
}}} // namespace embot { namespace hw { namespace flash {



namespace embot { namespace hw { namespace can {
    
    struct Frame            
    {
        std::uint32_t       id;
        std::uint8_t        size;
        std::uint8_t        filler[3];
        std::uint8_t        data[8];  
        Frame() : id(0), size(0) { std::uint64_t *d = reinterpret_cast<std::uint64_t*>(data); *d = 0; }
    };
    
    enum class Port { one = 0, two = 1, none = 32, maxnumberof = 2};
    
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
    
    bool initialised(Port p);
    
    result_t init(Port p, const Config &config);
    
    result_t enable(Port p);
    
    result_t disable(Port p);
    
    result_t put(Port p, const Frame &frame);
    
    std::uint8_t outputqueuesize(Port p);
    
    result_t transmit(Port p);
    
    std::uint8_t inputqueuesize(Port p);
    
    result_t get(Port p, Frame &frame, std::uint8_t &remaining);
    
    
    
}}} // namespace embot { namespace hw { namespace can {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


