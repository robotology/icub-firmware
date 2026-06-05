
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_QENC_H_
#define __EMBOT_HW_MOTOR_BLDC_QENC_H_


#include "embot_hw_motor_bldc.h"


namespace embot::hw::motor::bldc::qenc {
    
    struct Configuration
    {
        uint32_t dummy {0};
            
        constexpr Configuration() = default;
        constexpr bool isvalid() const { return true; }
        void clear() { dummy = 0; }
    };

    
    struct Mode
    {
        uint16_t resolution {0};            // the pulses per revolution (PPR) value found in datasheet, typically 1024
        embot::core::Callback onindex {};   // executed at index detection
        
        constexpr Mode() = default;
        constexpr Mode(int16_t res, const embot::core::Callback &oni) : resolution(res), onindex(oni) {}
        constexpr bool isvalid() const { 
            bool notok = (0 == resolution);
            return !notok;
        }
        void clear() 
        {
            resolution = 0; 
            onindex.clear();
        }
    };

    bool init(embot::hw::MOTOR m, const Configuration &config);
    bool deinit(embot::hw::MOTOR m);
    bool initialised(embot::hw::MOTOR m);
    
    bool start(embot::hw::MOTOR m, const Mode& mode);
    bool isstarted(embot::hw::MOTOR m);
    bool stop(embot::hw::MOTOR m);
    

    enum class AngleQE : uint8_t { current = 0, oflatestindexcrossing = 1 };
    
    float angle(embot::hw::MOTOR m, AngleQE aqe);

    
} // namespace embot::hw::motor::bldc::qenc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

