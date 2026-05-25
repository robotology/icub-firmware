
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_ENC_H_
#define __EMBOT_HW_MOTOR_BLDC_ENC_H_


#include "embot_hw_motor_bldc.h"


namespace embot::hw::motor::bldc::enc {
    
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
    bool initialised(embot::hw::MOTOR m);
    
    bool start(embot::hw::MOTOR m, const Mode& mode);
    bool isstarted(embot::hw::MOTOR m);
//    int32_t getvalue(); 
//    void force(int32_t value);
    float angle(embot::hw::MOTOR m);

    
//    bool Enc1Init(embot::hw::MOTOR m);
//    bool Enc2Init(embot::hw::MOTOR m);
    
//    int32_t Enc1GetRotorPosition(void);
//    int32_t Enc2GetRotorPosition(void);
    
    // ok
    float GetencIndexAngle(embot::hw::MOTOR m);

//    float GetencFirstIndexCrossAngle(embot::hw::MOTOR m);
    
    
//    void Enc1DeInit(void);
//    void Enc2DeInit(void);
    
//    void encoder1_test(void);
//    int32_t Enc1GetCounter(void); 
    
} // namespace embot::hw::motor::bldc::enc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

