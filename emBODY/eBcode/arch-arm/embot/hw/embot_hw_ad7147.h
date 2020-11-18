
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_AD7147_H_
#define _EMBOT_HW_AD7147_H_

#include "embot_core.h"
#include "embot_hw_types.h"


#include "embot_hw_i2c.h"

    
namespace embot { namespace hw { namespace ad7147 {
         
        
    // the ad7147 supports 12 taxels (values, stages, ...).
    constexpr size_t numberofTaxels {12};
        
    struct Config
    {   
        // this variable contains the value of the offset applied to every stage as described by following registers 
        // STAGEx_AFE_OFFSET: see table 36 of datasheet https://www.analog.com/media/en/technical-documentation/data-sheets/AD7147.pdf
        // where a value of 0xMMLL maps 0xMM into bits [15, 8] and 0xLL into bits [7, 0] of the of the register, so that:
        // (STAGEx_AFE_OFFSET & 0b1000000000000000) >> 15 -> bits[15]    -> POS_AFE_OFFSET_SWAP
        // (STAGEx_AFE_OFFSET & 0b0100000000000000) >> 14 -> bits[14]    -> Unused
        // (STAGEx_AFE_OFFSET & 0b0011111100000000) >> 8  -> bits[13:8]  -> POS_AFE_OFFSET
        // (STAGEx_AFE_OFFSET & 0b0000000010000000) >> 7  -> bits[7]     -> NEG_AFE_OFFSET_SWAP
        // (STAGEx_AFE_OFFSET & 0b0000000001000000) >> 6  -> bits[6]     -> Unused
        // (STAGEx_AFE_OFFSET & 0b0000000000111111) >> 0 ->  bits[5:0]   -> NEG_AFE_OFFSET
        // for teh case of the mtb4 we have 0x2200 = 0b0010001000000000
        // POS_AFE_OFFSET_SWAP = 0b0 (if 0 -> POS_AFE_OFFSET applied to CDC positive input)
        // Unused = 0b
        // POS_AFE_OFFSET = 0b100010 = 34 -> Positive AFE offset setting = 34 * 0.32 pF
        // NEG_AFE_OFFSET_SWAP = 0b0 -> (if 0 ->  NEG_AFE_OFFSET applied to CDC negative input)
        // Unused = 0b
        // NEG_AFE_OFFSET = 0b000000 = 0 -> Negative AFE offset setting = 0 * 0.32 pF       
        // instead, if we use 0x2000 = 0b0010000000000000 we ... apply 0b100000 = 32 * 0.32 pF to CDC positive input     
        uint16_t STAGEx_AFE_OFFSET {0x2000}; // mtb4 uses 0x2200. 
        
        constexpr Config() = default;
        // this constructor uses teh raw value of the register 
        constexpr Config(uint16_t registervalue) : STAGEx_AFE_OFFSET(registervalue) {}
        // this constructor uses the two values of the positive and negative offsets. each in range [0, 63]     
        constexpr Config(uint8_t POS_AFE_OFFSET, uint8_t NEG_AFE_OFFSET) 
          : STAGEx_AFE_OFFSET( ((POS_AFE_OFFSET & 0b111111)<<8) | (NEG_AFE_OFFSET & 0b111111) ) {}
    };
       
    // it keeps the values of the 12 stages
    using Values = std::array<std::uint16_t, numberofTaxels>; 
    
    
    bool supported(embot::hw::AD7147 s);    
    bool initialised(embot::hw::AD7147 s);    
    result_t init(embot::hw::AD7147 s, const Config &config);
    
    // after init() returns resOK, if needed we can configure the chip in a different way.
    result_t configure(AD7147 s, const Config &config);
            
    // after init() returns resOK we can check if it is alive. we can specify a timeout
    bool isalive(embot::hw::AD7147 s, embot::core::relTime timeout = 3*embot::core::time1millisec);
    
    // we must check that nobody is using the sensor, maybe in non-blocking mode some time earlier
    bool isacquiring(embot::hw::AD7147 s);
    
    // we check isacquiring() but also if any other device is using i2c bus
    bool canacquire(embot::hw::AD7147 s);    
        
    // we start acquisition
    // if returns resOK, we know that acquisition is over if it is called oncompletion() or when operationdone() is true;
    result_t acquisition(embot::hw::AD7147 s, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));

    // it tells if a previous operation of acquisition is over
    bool operationdone(embot::hw::AD7147 s);
    
    // ok, now we can read the values previously acquired
    result_t read(embot::hw::AD7147 s, Values &values);   

 
}}} // namespace embot { namespace hw { namespace ad7147 {
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


