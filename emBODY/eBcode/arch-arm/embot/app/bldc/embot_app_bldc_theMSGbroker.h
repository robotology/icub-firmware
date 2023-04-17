
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_THEMSGBROKER_H_
#define __EMBOT_APP_BLDC_THEMSGBROKER_H_


#include <memory>
#include <vector>
#include "embot_prot_can.h"
#include "embot_os_Action.h"


namespace embot { namespace app { namespace bldc {
           
    class theMSGbroker 
    {
    public:
        
        static theMSGbroker& getInstance();
    
        struct Config
        {
            size_t rxCapacity {32};
            size_t txCapacity {32};       
            constexpr Config() = default;
            constexpr Config(size_t r, size_t t) : rxCapacity(r), txCapacity(t) {}
            constexpr bool isvalid() const { return (rxCapacity > 0) && (txCapacity > 0); }
        };
        
        enum class Direction : uint8_t { INP, OUT };
        
        static constexpr size_t MAXcapacity = 999;   
                
        bool initialise(const Config &config);   
        
        size_t size(Direction dir) const;
        

        bool add(Direction dir, const embot::prot::can::Frame &frame);
        bool add(Direction dir, const std::vector<embot::prot::can::Frame> &frames);  

        // if ::OUT, whenever someone call any add(::OUT, ) then the action is called
        // the same for ::IN
        bool subscribe(Direction dir, const embot::os::Action &action);        

        bool rem(Direction dir, size_t &remaining, embot::prot::can::Frame &frame);
        bool rem(Direction dir, size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve = theMSGbroker::MAXcapacity);          
        
    private:
        theMSGbroker(); 
        ~theMSGbroker(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       
    
}}} // namespace embot { namespace app { namespace bldc


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
