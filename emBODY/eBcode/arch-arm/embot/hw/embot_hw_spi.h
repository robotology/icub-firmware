
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_SPI_H_
#define __EMBOT_HW_SPI_H_

#include "embot_core.h"
#include "embot_hw_types.h"


#if 0

## Configuration of the `embot::hw::spi` driver

The driver can be configured through `embot::hw::spi::Config` which allows, so far, to adapt
speed, datasize and shape of the SPI dataframe.

The pinout of the clock (SCLK), master output/slave input (MOSI), master input/slave output (MISO), 
and slave select (SS) are for now not managed in here. They are initted at startup by `embot::hw::bsp::init()`.

### The `embot::hw::spi::Prescaler`

It allows to scale the bus used by SPI to match some discrete speeds. 

### The `embot::hw::spi::DataSize`

It allows to form a dataframe of different sizes. 

### The `embot::hw::spi::Mode`

It shapes the data frame. That can happens in four different modes whiche end up in binary values of 
two parameters: the clock polarity (CPOL) and the clock phase (CPHA). 

A CPOL = 0 means that the clock line idles low and similarly CPOL = 1 means clock line idles high.
And if CPHA = 0 the bits are sampled on the leading clock edge, else if CPHA = 1 ar ecampled on the trailing ege.

All four possible combinations are shown in the following figure, alongside with the values of `
embot::shw::spi::Mode` used in the `embot::hw::spi` driver.

        
``` 
                    CPHA = 0              CPHA = 1                               
              samples on leading     samples on trailing
                      |                     |  
CPOL = 0              v___               ___v      
CLK idles low       __|   |__         __|   |__
                    Mode::zero        Mode::one
    
              samples on leading     samples on trailing       
                      |                     |
CPOL = 1            __v    __         __    v__
CKL idles high        |___|             |___|
                    Mode::two        Mode::tree
```
**Figure**. Description of `embot::shw::spi::Mode`. 

#endif  

namespace embot { namespace hw { namespace spi {
       
    // speed is expressed by an integer in bps. it depends on the used prescaler.
    using Speed = uint32_t;
     
    enum class Prescaler : uint8_t 
    {   
        two = 0,   
        four = 1,   
        eight = 2,   
        sixteen = 3, 
        thirtytwo = 4,
        sixtyfour = 5,
        onehundredtwentyeigth = 6,
        twohundredfiftysix = 7,
        none = 255        
    }; 

    enum class DataSize : uint8_t 
    {   
        eight = 7,   
        sixteen = 15,   
        twentyfour = 23,   
        thirtytwo = 31, 
        none = 255 
    };
     
    enum class ClockPolarity : uint8_t { low = 0, high = 1};
    enum class ClockPhase : uint8_t { edge1 = 0, edge2 = 1};
    enum class Mode : uint8_t 
    {   
        zero = 0,   // (polarity, phase) = (0, 0) = (SPI_POLARITY_LOW, SPI_PHASE_1EDGE)
        one = 1,    // (polarity, phase) = (0, 1) = (SPI_POLARITY_LOW, SPI_PHASE_2EDGE)
        two = 2,    // (polarity, phase) = (1, 0) = (SPI_POLARITY_HIGH, SPI_PHASE_1EDGE)
        three = 3,  // (polarity, phase) = (1, 1) = (SPI_POLARITY_HIGH, SPI_PHASE_2EDGE) 
        none = 255 
    };
          
    
    constexpr bool mode2clockprops(const Mode m, ClockPolarity &polarity, ClockPhase &phase)
    {
        if(Mode::none == m) { return false; }
        phase = (false == embot::core::binary::bit::check(embot::core::tointegral(m), 0)) ? ClockPhase::edge1 : ClockPhase::edge2;
        polarity = (false == embot::core::binary::bit::check(embot::core::tointegral(m), 1)) ? ClockPolarity::low : ClockPolarity::high;
        return true;
    }
    
    constexpr Mode clockprops2mode(ClockPolarity polarity, ClockPhase phase)
    {
        constexpr Mode map[2][2] = { {Mode::zero, Mode::one}, {Mode::two, Mode::three} };  
        return map[embot::core::tointegral(polarity)][embot::core::tointegral(phase)];
    }    
          
    struct Config
    {      
        Prescaler prescaler {Prescaler::none};  
        DataSize datasize {DataSize::none};
        Mode mode {Mode::none};     
        constexpr Config() = default;
        constexpr Config(Prescaler p, DataSize d, Mode m) : prescaler(p), datasize(d), mode(m) {};
        constexpr bool isvalid() const { return (Prescaler::none != prescaler) && (DataSize::none != datasize) && (Mode::none != mode); }
    };
        
    bool supported(embot::hw::SPI b);    
    bool initialised(embot::hw::SPI b);    
    result_t init(embot::hw::SPI b, const Config &config);
    result_t deinit(embot::hw::SPI b);
    

    // we can use spi in blocking mode or non blocking mode
    
    // BLOCKING mode: has a embot::core::relTime timeout
    // in such a mode, the functions start operations and wait until completion or until a timeout has expired.
    // the timeout must always be specified. for example 3*embot::core::time1millisec, 500*embot::core::time1microsec, etc...
    //
    // NON-BLOCKING mode: has a const embot::core::Callback &oncompletion
    // in such a mode, the functions returns control to the calling thread in the shortest possible time.
    // if the bus is busy they return without doing any activity. if not busy they set the bus busy, start hardware operations,  
    // dont wait for completion and return control to the calling thread. 
    // the calling thread can now operate in two modes:
    // 1. wait until isbusy() returns false (because the hardware sets the bus free at completion)
    // 2. can continue its execution and be alerted by the specified callback that the operation is completed.

    bool isbusy(embot::hw::SPI b, embot::core::relTime timeout, embot::core::relTime &remaining);
    bool isbusy(embot::hw::SPI b);
    
    result_t read(embot::hw::SPI b, embot::core::Data &destination, embot::core::relTime timeout);
    result_t write(embot::hw::SPI b, const embot::core::Data &source, embot::core::relTime timeout);
    result_t writeread(embot::hw::SPI b, const embot::core::Data &source,  embot::core::Data &destination, embot::core::relTime timeout);    
    
    result_t read(embot::hw::SPI b, embot::core::Data &destination, const embot::core::Callback &oncompletion);
    result_t write(embot::hw::SPI b, const embot::core::Data &source, const embot::core::Callback &oncompletion);
    result_t writeread(embot::hw::SPI b, const embot::core::Data &source, embot::core::Data &destination, const embot::core::Callback &oncompletion);
      
    
}}} // namespace embot { namespace hw { namespace spi {
    
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


