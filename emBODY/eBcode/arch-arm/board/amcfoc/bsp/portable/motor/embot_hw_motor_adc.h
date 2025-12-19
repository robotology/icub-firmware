
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_ADC_H_
#define __EMBOT_HW_MOTOR_ADC_H_

#include "embot_core.h"
#include "embot_hw.h"

#include "embot_hw_motor_bldc.h"

#if 0

this API exposes what is required to move the motor using embot::hw::motor

#endif


namespace embot::hw::motor::adc {

//    // we keep int32_t even if the adc gets only int16_t values
//    // the unit is [mA]
//    struct Currents
//    {
//        int32_t u {0};
//        int32_t v {0};
//        int32_t w {0};
//        
//        constexpr Currents() = default;
//        constexpr Currents(int32_t a, int32_t b, int32_t c) : u(a), v(b), w(c) {}     
//    };


//    struct OnCurrents
//    {
//        using Action = void (*)(void *owner, const Currents * const currents);
//        
//        Action action {nullptr};
//        void* owner {nullptr}; 
//        
//        constexpr OnCurrents() = default;
//        constexpr OnCurrents(Action a, void *o) : action(a), owner(o) {}
//            
//        void load(Action a, void *o) { action = a; owner = o; } 
//        void load(const OnCurrents &obj) { action = obj.action; owner = obj.owner; }    
//        bool isvalid() const { return (nullptr != action); }
//        void execute(const Currents * const curs) const
//        { 
//            if(isvalid()) { action(owner, curs); } 
//        }            
//    };


//    struct OnMotorCurrents
//    {
//        using Action = void (*)(void *owner, const embot::hw::MOTOR m, const Currents * const currents);
//        
//        Action action {nullptr};
//        void* owner {nullptr}; 
//        
//        constexpr OnMotorCurrents() = default;
//        constexpr OnMotorCurrents(Action a, void *o) : action(a), owner(o) {}
//            
//        void load(Action a, void *o) { action = a; owner = o; } 
//        void load(const OnMotorCurrents &obj) { action = obj.action; owner = obj.owner; }    
//        bool isvalid() const { return (nullptr != action); }
//        void execute(const embot::hw::MOTOR m, const Currents * const curs) const
//        { 
//            if(isvalid()) { action(owner, m, curs); } 
//        }            
//    };

//    struct OnMotorPhaseCurrents
//    {
//        using Action = embot::hw::motor::OnPhaseCurrents;
//        //void (*)(void *owner, const embot::hw::MOTOR m, const embot::hw::motor::PhaseCurrents * const phasecurrents);
//        
//        Action action {nullptr};
//        void* owner {nullptr}; 
//        
//        constexpr OnMotorPhaseCurrents() = default;
//        constexpr OnMotorPhaseCurrents(Action a, void *o) : action(a), owner(o) {}
//            
//        void load(Action a, void *o) { action = a; owner = o; } 
//        void load(const OnMotorPhaseCurrents &obj) { action = obj.action; owner = obj.owner; }    
//        bool isvalid() const { return (nullptr != action); }
//        void execute(const embot::hw::MOTOR m, const embot::hw::motor::PhaseCurrents * const curs) const
//        { 
//            if(isvalid()) { action(m, curs, owner); } 
//        }            
//    };
    
    struct Configuration
    {
        enum class ACQUISITION { immediate };

        ACQUISITION acquisition { ACQUISITION::immediate };    
        embot::hw::motor::bldc::OnCurrents oncurrents {};
        
        constexpr Configuration() = default;
        constexpr Configuration(const embot::hw::motor::bldc::OnCurrents& o) : oncurrents(o) {}
        constexpr bool isvalid() const { return true; }
    };

    struct Calibration
    {
        enum class CALIBRATION { none, current };

        CALIBRATION calibration {CALIBRATION::current};
        embot::core::relTime timeout {400*embot::core::time1millisec};
        
        constexpr Calibration() = default;
        constexpr Calibration(CALIBRATION &c, embot::core::relTime t) : calibration(c), timeout(t) {}

        constexpr bool isvalid() const { return true; }
    };

    // -- public interfaces to be used in embot::hw::motor

    // init() starts ADC acquisition straight away (ACQUISITION::immediate). 


    bool init(embot::hw::MOTOR m, const Configuration &config);
    bool deinit(embot::hw::MOTOR m);
    bool calibrate(embot::hw::MOTOR m, const Calibration &calib);
    bool set(embot::hw::MOTOR m, const embot::hw::motor::bldc::OnCurrents &cbk);
                   
} // namespace embot::hw::motor::adc {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

