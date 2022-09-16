
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_chip_MB049.h"


// --------------------------------------------------------------------------------------------------------------------
// - test code. i place this section in here because .... we just need the .h file
// --------------------------------------------------------------------------------------------------------------------
  
#if defined(EMBOT_HW_CHIP_MB049_enable_test)

static bool available {false};
static void cbk(void *p)
{
    available = true;
}

// it tests the chip and offers an example of use
bool embot::hw::chip::testof_MB049()
{
    // this configuration tells about which spi bus to use, which are the control pins
    // and their low level GPIO configuration
    // some extra info:
    // 1. this configuration is typically used by the embot::hw::eeprom and defined
    //    inside embot::hw::eeprom::thebsp located inside mbot_hw_bsp_nameofboard.cpp
    // 2. the spi bus in here specified is initted by MB049 code w/ a 
    //    call to embot::hw::spi::init() in a way that is specified by
    //    embot::hw::spi::thebsp typically placed inside embot_hw_bsp_nameofboard.cpp
    // 3. the control pins are initialised / deinitialised inside MB049 only if
    //    embot::hw::chip::MB049::Config::PinControl::config.isvalid()
    constexpr embot::hw::chip::MB049::Config cfg
    {
        embot::hw::SPI::one,  // the spi bus
        {
            embot::hw::spi::Prescaler::sixtyfour,    // onehundredtwentyeigth
            embot::hw::spi::DataSize::eight,
            embot::hw::spi::Mode::one,
            { {embot::hw::gpio::Pull::pullup, embot::hw::gpio::Pull::pulldown,      // | miso | mosi |
               embot::hw::gpio::Pull::pulldown, embot::hw::gpio::Pull::pullup} }    // | sclk | sel  |
        }
    };
    
    embot::hw::chip::MB049::Data data {};
    
    // step 01: create the object
    embot::hw::chip::MB049 *chipMB049 = new embot::hw::chip::MB049;
    
    bool ok {false};
    
    // step 02: initialise it (for extra check i also deinit and init it again)
    chipMB049->init(cfg);
    chipMB049->deinit();
    if(true == chipMB049->init(cfg))
    {
        // step 03: read an angle
        for (;;)
        {
            available = false;
            data.position = 0;
#if 0 // test non-blocking mode
            if(true == chipMB049->read(data, {cbk, nullptr}))
            {
                ok = true;
            }
            
            for(;;)
            {
                if(available)
                {
                    break;
                }
                else
                {
                    embot::core::wait(5); // us
                }
            }
            
            embot::core::print(ok ? "dummy test chipMB049: OK --> " + std::to_string(data.position) + " " + std::to_string(data.status.bits) + " " + std::to_string(data.status.ok) : "dummy test chipMB049: KO");
            
#else // test blocking mode
            if(true == chipMB049->read(data, 5*embot::core::time1millisec))
            {
                ok = true;
            }
            
            embot::core::wait(10*embot::core::time1millisec);

            // step 06: print result
            embot::core::print(ok ? "dummy test chipMB049: OK --> " + std::to_string(data.position) + " " + std::to_string(data.status.bits) + " " + std::to_string(data.status.ok) : "dummy test chipMB049: KO");
#endif
        }
    }

    // step 07: delete the object: the destructor also deinits
    delete chipMB049;
    
    return ok;
}


#endif


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------    

// we dont actually need stm32hal in here ... just embot::hw gpio and spi
//#if defined(USE_STM32HAL)
//    #include "stm32hal.h"
//#else
//    #warning this implementation is only for stm32hal
//#endif

#include "embot_hw_gpio.h"
#include "embot_hw_spi.h"
#include "embot_hw_spi_bsp.h"

using namespace std;

using namespace embot::hw;



struct embot::hw::chip::MB049::Impl
{
    bool _initted {false};
    Config _config {};
    uint8_t _databuffer[6] = {0, 0, 0, 0, 0, 0};
    embot::hw::GPIO _sslsegopio {};
    
    Data *_tmpdata {nullptr};
    embot::core::Callback _tmponcompletion {};
    
    Impl() = default;
    ~Impl() { deinit(); }
    
    bool init(const Config &cfg);
    bool deinit();
    bool read(Data &data, embot::core::relTime timeout);
    bool read(Data &data, const embot::core::Callback &oncompletion);
    
private:

    static void onCompletion(void *p);
    void parseDataBuffer(Data &data);
    uint8_t CRC_SPI_97_64bit(uint64_t dw_InputData);
    
};


bool embot::hw::chip::MB049::Impl::init(const Config &cfg)
{
    if((true == _initted) && (false == cfg.isvalid()))
    {
        return false;
    }
    
    _config = cfg;

     // init sslsel before init the spi
    _sslsegopio = embot::hw::spi::bsp::getBSP().getPROP(_config.spi)->pinout[embot::core::tointegral(embot::hw::spi::Signal::SSEL)];
    embot::hw::gpio::init(_sslsegopio, {embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::pullup, embot::hw::gpio::Speed::veryhigh});

    if(resOK == embot::hw::spi::init(_config.spi, _config.spicfg))
    {
        _initted =  true;
    }
    
    return _initted;
}

bool embot::hw::chip::MB049::Impl::deinit()
{
    if(_initted)
    {
        embot::hw::spi::deinit(_config.spi);
        _initted = false;
        
        // deinit the sslsel as well
        embot::hw::gpio::deinit(_sslsegopio);
    }
    
    return !_initted;
}

uint8_t embot::hw::chip::MB049::Impl::CRC_SPI_97_64bit(uint64_t dw_InputData)
{
     uint8_t b_Index = 0;
     uint8_t b_CRC = 0;
     b_Index = (uint8_t)((dw_InputData >> 56u) & (uint64_t)0x000000FFu);
     b_CRC = (uint8_t)((dw_InputData >> 48u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 40u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 32u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 24u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 16u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)((dw_InputData >> 8u) & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = (uint8_t)(dw_InputData & (uint64_t)0x000000FFu);
     b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
     b_CRC = ab_CRC8_LUT[b_Index];
     
     return b_CRC; 
}

void embot::hw::chip::MB049::Impl::parseDataBuffer(Data &data)
{
    // Encoder position data structure
    // -------------------------------
    // b47 : b32 Multiturn counter (if specified in part number) – Left aligned, MSB first. 
    // b31 : b10 Encoder position + zero padding bits – Left aligned, MSB first. 
    // b9 Error – If low, the position data is not valid. 
    // b8 Warning - If low, the position data is valid, but some operating conditions are close to limits. 
    // b7 : b0 Inverted CRC, 0x97 polynomial 
    
    data.multiturncounter = (_databuffer[0] << 8) | _databuffer[1]; // TODO: check for the first bit (maybe it must be discarded)
    data.position = (_databuffer[2] << 11) | (_databuffer[3] << 3) | ((_databuffer[4] & 0xe0) >> 5); // the last shift eliminates the non-position values (2) + zero padded bits (3)
    data.status.bits = (_databuffer[4] & 0x03);
    
    // Check for status bits errors
    data.status.ok = true;
    
    if(0x01 == data.status.bits || 0x0 == data.status.bits)
    {
        embot::core::print("Error - the position data is not valid.");
        data.status.ok = false;
    }
    else if(0x2 == data.status.bits)
    {
        embot::core::print("Warning - the position data is valid, but some operating conditions are close to limits.");
    }
    
    // check for CRC errors
    uint64_t dw_CRCinputData = 0;
    uint8_t calculated_crc=0;
    
    dw_CRCinputData = ((uint64_t)_databuffer[0] << 40) + ((uint64_t)_databuffer[1] << 32) +
                      ((uint64_t)_databuffer[2] << 24) + ((uint64_t)_databuffer[3] << 16) +
                      ((uint64_t)_databuffer[4] << 8)  + ((uint64_t)_databuffer[5]);
    
    calculated_crc = ~(CRC_SPI_97_64bit(dw_CRCinputData))& 0xFF; //inverted CRC (TODO: check if it must be 0 or 1)
    
    //embot::core::print(std::to_string(_databuffer[0]) + " " + std::to_string(_databuffer[1]) + " " + std::to_string(_databuffer[2]) + " " + std::to_string(_databuffer[3]) + " " + std::to_string(_databuffer[4]) + " " + std::to_string(_databuffer[5]));
}


bool embot::hw::chip::MB049::Impl::read(Data &data, embot::core::relTime timeout)
{
    bool r {false};
    
    // Using SSI we need to set CSn to logic low during the reading
    embot::hw::gpio::set(_sslsegopio, embot::hw::gpio::State::RESET);
    embot::core::Data da {_databuffer, sizeof(_databuffer)};
    
    // try to read the data
    if(resOK == embot::hw::spi::read(_config.spi, da, timeout))
    {
        // parse the absolute angular position data and the status output bits
        parseDataBuffer(data);
        r = true;
    }

    // set CSn to logic high
    embot::hw::gpio::set(_sslsegopio, embot::hw::gpio::State::SET);

    return r;
}
   
   
void embot::hw::chip::MB049::Impl::onCompletion(void *p)
{
    embot::hw::chip::MB049::Impl *pimpl = reinterpret_cast<embot::hw::chip::MB049::Impl*>(p);
    
    if(nullptr != pimpl->_tmpdata)
    {
        // set CSn to logic high
        embot::hw::gpio::set(pimpl->_sslsegopio, embot::hw::gpio::State::SET);
        
        // parse the absolute angular position data and the status output bits
        pimpl->parseDataBuffer(*(pimpl->_tmpdata));
    }
    
    pimpl->_tmponcompletion.execute();
    pimpl->_tmponcompletion.clear();
    pimpl->_tmpdata = nullptr;
}
   
   
bool embot::hw::chip::MB049::Impl::read(Data &data, const embot::core::Callback &oncompletion)
{
    bool r {false};
    
    _tmponcompletion = oncompletion;
    _tmpdata = &data;
    embot::core::Data da {_databuffer, sizeof(_databuffer)};
    
    // Using SSI we need to set CSn to logic low during the reading
    embot::hw::gpio::set(_sslsegopio, embot::hw::gpio::State::RESET);

    if(resOK == embot::hw::spi::read(_config.spi, da, {onCompletion, this}))
    {
        r = true;
    }
    return r;
}


// --------------------------------------------------------------------------------------------------------------------
// - interface to the pimpl
// --------------------------------------------------------------------------------------------------------------------
    
    
embot::hw::chip::MB049::MB049()
: pImpl(new Impl)
{

}

embot::hw::chip::MB049::~MB049()
{
    delete pImpl;
}

bool embot::hw::chip::MB049::init(const Config &config)
{
    return pImpl->init(config);
}

bool embot::hw::chip::MB049::deinit()
{
    return pImpl->deinit();
}

bool embot::hw::chip::MB049::read(Data &data, embot::core::relTime timeout)
{
    return pImpl->read(data, timeout);
}

bool embot::hw::chip::MB049::read(Data &data, const embot::core::Callback &oncompletion)
{
    return pImpl->read(data, oncompletion);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

