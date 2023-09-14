
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __MOTORHAL_H_
#define __MOTORHAL_H_

#include "embot_core.h"
#include "embot_hw.h"

#if 0

this API exposes what is required to move the motor using embot::hw::motor

#endif


namespace embot::hw::motor::adc {

// we keep int32_t even if the adc gets only int16_t values
struct Currents
{
    int32_t u {0};
    int32_t v {0};
    int32_t w {0};
    
    constexpr Currents() = default;
    constexpr Currents(int32_t a, int32_t b, int32_t c) : u(a), v(b), w(c) {}     
};


struct OnCurrents
{
    using Action = void (*)(void *owner, const Currents * const currents);
    
    Action action {nullptr};
    void* owner {nullptr}; 
    
    constexpr OnCurrents() = default;
    constexpr OnCurrents(Action a, void *o) : action(a), owner(o) {}
        
    void load(Action a, void *o) { action = a; owner = o; } 
    void load(const OnCurrents &obj) { action = obj.action; owner = obj.owner; }    
    bool isvalid() const { return (nullptr != action); }
    void execute(const Currents * const curs) const
    { 
        if(isvalid()) { action(owner, curs); } 
    }            
};


struct Configuration
{
    enum class ACQUISITION { immediate };

    ACQUISITION acquisition { ACQUISITION::immediate };    
    OnCurrents oncurrents {};
    
    constexpr Configuration() = default;
    constexpr Configuration(const OnCurrents& o) : oncurrents(o) {}
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


bool init(const Configuration &config);
bool calibrate(const Calibration &calib);
void set(const OnCurrents &cbk);
bool deinit();
                   
} // namespace embot::hw::motor::adc {


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

bool init(const Configuration &config);
bool deinit();
bool start(const Mode& mode);
bool isstarted();
int32_t getvalue();    
    
} // namespace embot::hw::motor::enc {


namespace embot::hw::motor::hall {
    
struct Configuration
{
    enum class ACQUISITION { deferred };
    
    ACQUISITION acquisition { ACQUISITION::deferred };
        
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
};

bool init(const Configuration &config);
bool deinit();
bool start(const Mode &mode);
bool isstarted();
uint8_t getstatus();    
int32_t getangle();
    
} // namespace embot::hw::motor::hall {


namespace embot::hw::motor::pwm {
    

struct Configuration
{
    enum class START { immediate };
    
    START start { START::immediate };
        
    constexpr Configuration() = default;
    constexpr bool isvalid() const { return true; }
};

void init(const Configuration &config);
void deinit();
void enable(bool on);
void set(uint16_t u, uint16_t v, uint16_t w);   
void setperc(float u, float v, float w);
    
} // namespace embot::hw::motor::pwm {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

