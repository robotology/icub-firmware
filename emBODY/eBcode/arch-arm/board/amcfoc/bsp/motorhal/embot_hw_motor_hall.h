
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_HALL_H_
#define __EMBOT_HW_MOTOR_HALL_H_

#include "embot_core.h"
#include "embot_hw.h"

#include "embot_hw_motor_bldc.h"

#if 0

this API exposes what is required to move the motor using embot::hw::motor

#endif


namespace embot::hw::motor::hall {
    
    struct Configuration
    {
        enum class ACQUISITION { deferred };
        enum class ENCODERtuning { none, forcevalue, adjust };
        
        ACQUISITION acquisition { ACQUISITION::deferred };
        ENCODERtuning encodertuning { ENCODERtuning::none };
            
        constexpr Configuration() = default;
        constexpr bool isvalid() const { return true; }
    };


    struct Mode
    {
        enum class SWAP { none, BC };
        SWAP swap {SWAP::none};
        uint16_t offset {0};
        
        constexpr Mode() = default;
        constexpr Mode(SWAP s, uint16_t o) : swap(s), offset(o) {}
        constexpr bool isvalid() const { 
            bool notok = false;
            return !notok;
        }
        void reset() { swap = SWAP::none; offset = 0; }
    };

    bool init(embot::hw::MOTOR m, const Configuration &config);
    bool deinit(embot::hw::MOTOR m);
    bool start(embot::hw::MOTOR m, const Mode &mode);
    bool isstarted(embot::hw::MOTOR m);
    uint8_t getstatus(embot::hw::MOTOR m);    
    int32_t getangle(embot::hw::MOTOR m);    
    float angle(embot::hw::MOTOR m); // in degrees
    
} // namespace embot::hw::motor::hall {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

