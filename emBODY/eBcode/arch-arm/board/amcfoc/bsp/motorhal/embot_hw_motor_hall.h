
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
        uint8_t polarpairs {1};
        
        constexpr Mode() = default;
        constexpr Mode(SWAP s, uint16_t o, uint8_t n) : swap(s), offset(o), polarpairs(n) {}
        constexpr bool isvalid() const { 
            bool notok = false;
            return !notok;
        }
        void reset() { swap = SWAP::none; offset = 0; polarpairs = 1; }
    };

    bool init(embot::hw::MOTOR m, const Configuration &config);
    bool deinit(embot::hw::MOTOR m);
    bool start(embot::hw::MOTOR m, const Mode &mode);
    bool isstarted(embot::hw::MOTOR m);
    uint8_t getstatus(embot::hw::MOTOR m);    
    int32_t getangle(embot::hw::MOTOR m);    
    float angle(embot::hw::MOTOR m, embot::hw::motor::bldc::AngleType type); // in degrees
    uint8_t sector(embot::hw::MOTOR m);
    
} // namespace embot::hw::motor::hall {

namespace embot::hw::motor::hall::bsp {
    uint8_t triple(embot::hw::MOTOR m);
}

namespace embot::hw::motor::hall::bsp::debug {
    
    void set(embot::hw::MOTOR m, float angle, embot::hw::motor::bldc::AngleType type);
    
    // this is a waveform, so far only a triangolar wave w/ semiperiod T1. it starts from 0 and goes to MAX in =T1 steps
    // i use a struct but it would be nice to generalize it and made it a pimpl
    struct Wave
    {        
        struct Config
        {
            embot::core::relTime period {embot::core::time1second};
            float maxvalue {1000.0f};
            embot::core::relTime ticktime {1000*embot::core::time1microsec}; // {45*embot::core::time1microsec};

            Config() = default;
        };
        
        bool configure(const Config &cfg);
        void tick();
        
        float get() const;
        
        Wave();
        ~Wave();

    private:        
        struct Impl;
        Impl *pImpl;                
    };
}


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

