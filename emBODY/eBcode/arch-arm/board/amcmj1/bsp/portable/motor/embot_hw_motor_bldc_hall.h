
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_HALL_H_
#define __EMBOT_HW_MOTOR_BLDC_HALL_H_


#include "embot_hw_motor_bldc.h"


namespace embot::hw::motor::bldc::hall {
    
    static constexpr size_t numsectors {6};    
    
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
    bool initialised(embot::hw::MOTOR m);
    
    bool start(embot::hw::MOTOR m, const Mode &mode);
    bool stop(embot::hw::MOTOR m);    
    bool isstarted(embot::hw::MOTOR m);
    
    uint8_t getstatus(embot::hw::MOTOR m);    

    float angle(embot::hw::MOTOR m, embot::hw::motor::bldc::AngleType type); // in degrees
    uint8_t sector(embot::hw::MOTOR m); // in range [0, numsectors-1]
    
    bool status2sector(embot::hw::MOTOR m, uint8_t status, uint8_t &sector);
    bool sector2status(embot::hw::MOTOR m, uint8_t sector, uint8_t &status);
    
} // namespace embot::hw::motor::bldc::hall {


//namespace embot::hw::motor::bldc::hall::debug {
//    
//    void set(embot::hw::MOTOR m, float angle, embot::hw::motor::bldc::AngleType type);
//    
//    // this is a waveform, so far only a triangolar wave w/ semiperiod T1. it starts from 0 and goes to MAX in =T1 steps
//    // i use a struct but it would be nice to generalize it and made it a pimpl
//    struct Wave
//    {        
//        struct Config
//        {
//            embot::core::relTime period {embot::core::time1second};
//            float maxvalue {1000.0f};
//            embot::core::relTime ticktime {1000*embot::core::time1microsec}; // {45*embot::core::time1microsec};

//            Config() = default;
//        };
//        
//        bool configure(const Config &cfg);
//        void tick();
//        
//        float get() const;
//        
//        Wave();
//        ~Wave();

//    private:        
//        struct Impl;
//        Impl *pImpl;                
//    };
//    
//} // namespace embot::hw::motor::bldc::hall::debug {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

