
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __MOTORHAL_ENC_H_
#define __MOTORHAL_ENC_H_

#include "embot_core.h"
#include "embot_hw.h"

#if 0

this API exposes what is required to move the motor using embot::hw::motor

#endif



namespace embot::hw::motor::enc {
    
    struct Configuration
    {
        enum class ACQUISITION { deferred };
        
        ACQUISITION acquisition { ACQUISITION::deferred };
            
        constexpr Configuration() = default;
        constexpr bool isvalid() const { return true; }
    };

    struct Mode
    {
        int16_t resolution {0};
        uint8_t num_polar_couples {0};
        bool calibrate_with_hall {false};
        bool use_index {false};
        
        constexpr Mode() = default;
        constexpr Mode(int16_t r, uint8_t n, bool c = false, bool i = false) 
            : resolution(r), num_polar_couples(n), calibrate_with_hall(c), use_index(i) {}
        constexpr bool isvalid() const { 
            bool notok = (0 == resolution) || (0 == num_polar_couples);
            return !notok;
        }
    };

    bool init(embot::hw::MOTOR m, const Configuration &config);
    bool deinit(embot::hw::MOTOR m);
    bool start(embot::hw::MOTOR m, const Mode& mode);
    bool isstarted(embot::hw::MOTOR m);
//    int32_t getvalue(); 
//    void force(int32_t value);
    float angle(embot::hw::MOTOR m);
    
    
    bool Enc1Init(void);
    bool Enc2Init(void);
    
    int32_t Enc1GetRotorPosition(void);
    int32_t Enc2GetRotorPosition(void);
    void Enc1DeInit(void);
    void Enc2DeInit(void);
    
    void encoder1_test(void);
    
} // namespace embot::hw::motor::enc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

