
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef __TEST_H_
#define __TEST_H_

#include "embot_core.h"
#include "embot_os_common.h"


class TesterSingleton 
{
private:
    // Private constructor to prevent direct instantiation
    TesterSingleton() 
    {
        embot::core::print("Tester Singleton Instance Created");
    }

public:
    
    // Constexpr to be used in the tests
    static constexpr embot::os::Event evRXcanframe = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
    static inline const std::uint8_t canRXcapacity = 32;
    static inline const std::uint8_t canTXcapacity = 32;
    
    // Delete copy constructor and assignment operator
    TesterSingleton(const TesterSingleton&) = delete;
    TesterSingleton& operator=(const TesterSingleton&) = delete;
    
    // Static method to provide access to the single instance
    static TesterSingleton& getInstance() 
    {
        static TesterSingleton instance;  // Guaranteed to be created only once (C++11 thread-safe)
        return instance;
    }

    void showMessage() {
        embot::core::print("Hello from Tester Singleton");
    }
    
    void test_embot_hw_init(void* p);
    void test_embot_hw_tick();
    void test_embot_hw_run();
};



#endif 

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

