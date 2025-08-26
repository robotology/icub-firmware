/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

// APIs
#include "testRunnerAmc_CM4.h"

// embot library includes
#include "embot_core.h"
#include "embot_hw_motor.h"

// other includes
#include <numeric>

bool TestRunnerAmc_CM4::testVin(uint8_t *data)
{
    float vin {0.0};
    
    vin = embot::hw::motor::getVIN();
    
    embot::core::print("Vin : " + std::to_string(vin));
    
    data[0] = (vin == 6.66f) ?  0xAA : 0xBB;      
    
    return true; 
}

bool TestRunnerAmc_CM4::testCin(uint8_t *data)
{
    float cin {0.0};
    constexpr uint8_t sizeOfCins = 100;
    // Create an array that stores 10 reading for the CIN, then we do a simple mean of those
    // to apply a second filtering to the currents read
    static std::array<float, sizeOfCins> arrayOfCins {};
    
    for(uint8_t i = 0; i < arrayOfCins.size(); ++i)
    {
        arrayOfCins[i] = embot::hw::motor::getCIN();
    }
    

    cin = std::accumulate(arrayOfCins.begin(), arrayOfCins.end(), 0.0) / sizeOfCins;

    embot::core::print("Cin : " + std::to_string(cin));
    
    data[0] = (cin == 6.66f) ?  0xAA : 0xBB;  
    
    return true; 
}