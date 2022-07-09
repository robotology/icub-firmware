
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_chip_AS5045.h"
#include "embot_os.h"

// --------------------------------------------------------------------------------------------------------------------
// - test code. i place this section in here because .... we just need the .h file
// --------------------------------------------------------------------------------------------------------------------
  
#if defined(EMBOT_HW_CHIP_AS5045_enable_test)

static bool available {false};
static void cbk(void *p)
{
    available = true;
}

// it tests the chip and offers an example of use
bool embot::hw::chip::testof_AS5045()
{
    // this configuration tells about which spi bus to use, which are the control pins
    // and their low level GPIO configuration
    // some extra info:
    // 1. this configuration is typically used by the embot::hw::eeprom and defined
    //    inside embot::hw::eeprom::thebsp located inside mbot_hw_bsp_nameofboard.cpp
    // 2. the spi bus in here specified is initted by AS5045 code w/ a 
    //    call to embot::hw::spi::init() in a way that is specified by
    //    embot::hw::spi::thebsp typically placed inside embot_hw_bsp_nameofboard.cpp
    // 3. the control pins are initialised / deinitialised inside AS5045 only if
    //    embot::hw::chip::AS5045::Config::PinControl::config.isvalid()
    constexpr embot::hw::chip::AS5045::Config cfg
    {
        embot::hw::SPI::one,  // the spi bus
        {
            embot::hw::spi::Prescaler::sixtyfour,
            embot::hw::spi::DataSize::eight,
            embot::hw::spi::Mode::two,
            { {embot::hw::gpio::Pull::pullup, embot::hw::gpio::Pull::nopull,      // | miso | mosi |
               embot::hw::gpio::Pull::pulldown, embot::hw::gpio::Pull::pullup} }  // | sclk | sel  |
        }
    };
    
    embot::hw::chip::AS5045::Data data {};
    
    // step 01: create the object
    embot::hw::chip::AS5045 *chipAS5045 = new embot::hw::chip::AS5045;
    
    bool ok {false};
    
    // step 02: initialise it (for extra check i also deinit and init it again)
    chipAS5045->init(cfg);
    chipAS5045->deinit();
    if(true == chipAS5045->init(cfg))
    {
        // step 03: read an angle
        for (;;)
        {
            available = false;
            data.position = 0;
#if 1 // test non-blocking mode
            if(true == chipAS5045->read(data, {cbk, nullptr}))
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
            
            embot::core::print(ok ? "dummy test chipAS5045: OK --> " + std::to_string(data.position) + " " + std::to_string(data.status.bits) + " " + std::to_string(data.status.ok) : "dummy test chipAS5045: KO");
            
#else // test blocking mode
            if(true == chipAS5045->read(data, 5*embot::core::time1millisec))
            {
                ok = true;
            }

            // step 06: print result
            embot::core::print(ok ? "dummy test chipAS5045: OK --> " + std::to_string(data.position) + " " + std::to_string(data.status.bits) + " " + std::to_string(data.status.ok) : "dummy test chipAS5045: KO");
#endif
        }
    }

    // step 07: delete the object: the destructor also deinits
    delete chipAS5045;
    
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



struct embot::hw::chip::AS5045::Impl
{
    bool _initted {false};
    Config _config {};
    uint8_t _databuffer[3] = {0, 0, 0};
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
    
};


bool embot::hw::chip::AS5045::Impl::init(const Config &cfg)
{
    if((true == _initted) && (false == cfg.isvalid()))
    {
        return false;
    }
    
    _config = cfg;

    if(resOK == embot::hw::spi::init(_config.spi, _config.spicfg))
    {
        _initted =  true;
    }
    
    // init sslsel
    _sslsegopio = embot::hw::spi::bsp::getBSP().getPROP(_config.spi)->pinout[embot::core::tointegral(embot::hw::spi::Signal::SSEL)];
    embot::hw::gpio::init(_sslsegopio, {embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::pullup, embot::hw::gpio::Speed::veryhigh});
    
    return _initted;
}

bool embot::hw::chip::AS5045::Impl::deinit()
{
    if(_initted)
    {
        embot::hw::spi::deinit(_config.spi);
        _initted = false;
    }
    
    return !_initted;
}

void embot::hw::chip::AS5045::Impl::parseDataBuffer(Data &data)
{
    data.position = (_databuffer[0] << 4) | ((_databuffer[1] & 0xf0) >> 4 );
    data.status.bits = ((_databuffer[1] & 0x0f) >> 1);
    
    // Status Bit Outputs
    // Placing the magnet above the chip, angular values increase in clockwise direction by default. 
    // Data D11:D0 is valid, when the status bits have the following configurations:
    // |    OCF    |    COF    |    LIN    |    MagInc    |    MagDec    |           Parity           |
    // |           |           |           |      0       |       0      |                            |
    // |     1     |     0     |     0     |      0       |       1      | Even checksum of bits 1:15 |
    // |           |           |           |      1       |       0      |                            |
    // |           |           |           |      1*)     |      1*)     |                            |
    data.status.ok = (0x04 == data.status.bits) ? true : false;
    
    uint32_t whole_data = (_databuffer[0] << 10 | _databuffer[1] << 2 | _databuffer[2]);
    data.status.parity = (0 == __builtin_parity(whole_data)) ? Parity::even : Parity::odd;
}


bool embot::hw::chip::AS5045::Impl::read(Data &data, embot::core::relTime timeout)
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
   
   
void embot::hw::chip::AS5045::Impl::onCompletion(void *p)
{
    embot::hw::chip::AS5045::Impl *pimpl = reinterpret_cast<embot::hw::chip::AS5045::Impl*>(p);
    
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
   
   
bool embot::hw::chip::AS5045::Impl::read(Data &data, const embot::core::Callback &oncompletion)
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
    
    
embot::hw::chip::AS5045::AS5045()
: pImpl(new Impl)
{

}

embot::hw::chip::AS5045::~AS5045()
{
    delete pImpl;
}

bool embot::hw::chip::AS5045::init(const Config &config)
{
    return pImpl->init(config);
}

bool embot::hw::chip::AS5045::deinit()
{
    return pImpl->deinit();
}

bool embot::hw::chip::AS5045::read(Data &data, embot::core::relTime timeout)
{
    return pImpl->read(data, timeout);
}

bool embot::hw::chip::AS5045::read(Data &data, const embot::core::Callback &oncompletion)
{
    return pImpl->read(data, oncompletion);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

