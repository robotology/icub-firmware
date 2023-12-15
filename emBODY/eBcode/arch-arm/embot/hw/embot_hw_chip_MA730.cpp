
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_chip_MA730.h"

// --------------------------------------------------------------------------------------------------------------------
// - test code. i place this section in here because .... we just need the .h file
// --------------------------------------------------------------------------------------------------------------------
  
#if defined(EMBOT_HW_CHIP_MA730_enable_test)

// it tests the chip and offers an example of use
bool embot::hw::chip::testof_MA730()
{    
    // this configuration tells about which spi bus to use, which are the control pins
    // and their low level GPIO configuration
    // some extra info:
    // 1. this configuration is typically used by the embot::hw::eeprom and defined
    //    inside embot::hw::eeprom::thebsp located inside mbot_hw_bsp_nameofboard.cpp
    // 2. the spi bus in here specified is initted by MA730 code w/ a 
    //    call to embot::hw::spi::init() in a way that is specified by
    //    embot::hw::spi::thebsp typically placed inside embot_hw_bsp_nameofboard.cpp
    // 3. the control pins are initialised / deinitialised inside MA730 only if
    //    embot::hw::chip::MA730::Config::PinControl::config.isvalid()
    constexpr embot::hw::chip::MA730::Config cfg 
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
    
    embot::hw::chip::MA730::Data data {};
        
    // step 01: create the object
    embot::hw::chip::MA730 *chipMA730 = new embot::hw::chip::MA730;
    
    bool ok {false};
    
    // step 02: initialise it (for extra check i also deinit and init it again)   
    chipMA730->init(cfg);
    chipMA730->deinit();
    
    if(true == chipMA730->init(cfg))
    {  
        // maybe wait 1 us after chip initialization before reading: embot::hw::sys::delay(1); // us
        // step 03: read an angle        
        for (;;){
                
            //embot::hw::sys::delay(1000); // us

            if(true == chipMA730->read(data, 5*embot::core::time1millisec))
            {
                ok = true;
            }

            // step 06: print result
            embot::core::print(ok ? "dummy test chipMA730: OK --> " + std::to_string(data.position) + " " + std::to_string(data.status.bits) + " " + std::to_string(data.status.ok) : "dummy test chipMA730: KO");   
            
            embot::core::wait(1000); // perform a reading every 1 ms
        }
    }


    // step 07: delete the object: the destructor also deinits
    delete chipMA730;
    
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


   
struct embot::hw::chip::MA730::Impl
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
    
};    


bool embot::hw::chip::MA730::Impl::init(const Config &cfg)
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

bool embot::hw::chip::MA730::Impl::deinit()
{  
    if(_initted)
    {
        embot::hw::spi::deinit(_config.spi);
        _initted = false;
    }
    
    return !_initted; 
}


bool embot::hw::chip::MA730::Impl::read(Data &data, embot::core::relTime timeout)
{   
    bool r {false};
    
    embot::hw::gpio::set(_sslsegopio, embot::hw::gpio::State::RESET);
    embot::core::Data da {_databuffer, sizeof(_databuffer)};

    if(resOK == embot::hw::spi::read(_config.spi, da, timeout))
    {
        if((_databuffer[0] == 0xFF) && (_databuffer[1] == 0xFF)) 
        {
            // the SPI is not reading correctly (or the encoder is disconnected)
            data.status.ok = false;
            return r; //false
        }
        
        // AEA3 offers 14 bit of resolution (0-16383)
        // in this way we are reading 16 bits (1 bit dummy + 14 bit valid + 1 bit zero padded). The dummy bit has been masked.
        data.position = ((_databuffer[0] & 0x7F) << 7) | (_databuffer[1] >> 1);
        data.status.ok = true;
        r = true;
    }
    
    embot::hw::gpio::set(_sslsegopio, embot::hw::gpio::State::SET);

    return r;
} 
   
   
void embot::hw::chip::MA730::Impl::onCompletion(void *p)
{
    embot::hw::chip::MA730::Impl *pimpl = reinterpret_cast<embot::hw::chip::MA730::Impl*>(p);
    if(nullptr != pimpl->_tmpdata)
    {
        pimpl->_tmpdata->position = ((pimpl->_databuffer[0] & 0x7F) << 7) | (pimpl->_databuffer[1] >> 1);
        
        // When both data buffer read 0xFF the SPI is not reading (or the encoder is disconnected)
        pimpl->_tmpdata->status.ok = ((pimpl->_databuffer[0] == 0xFF) && (pimpl->_databuffer[1] == 0xFF)) ? false : true; 
    }
    pimpl->_tmponcompletion.execute();
    pimpl->_tmponcompletion.clear();
    pimpl->_tmpdata = nullptr;
}
   
   
bool embot::hw::chip::MA730::Impl::read(Data &data, const embot::core::Callback &oncompletion)
{   
    bool r {false};
    
    _tmponcompletion = oncompletion;
    _tmpdata = &data;
    embot::core::Data da {_databuffer, sizeof(_databuffer)};
    if(resOK == embot::hw::spi::read(_config.spi, da, {onCompletion, this}))
    {
        r = true;
    }
    
    return r;
}    


// --------------------------------------------------------------------------------------------------------------------
// - interface to the pimpl
// --------------------------------------------------------------------------------------------------------------------
    
    
embot::hw::chip::MA730::MA730()
: pImpl(new Impl)
{ 

}

embot::hw::chip::MA730::~MA730()
{   
    delete pImpl;
}

bool embot::hw::chip::MA730::init(const Config &config)
{
    return pImpl->init(config);
}

bool embot::hw::chip::MA730::deinit()
{
    return pImpl->deinit();
}

bool embot::hw::chip::MA730::read(Data &data, embot::core::relTime timeout)
{
    return pImpl->read(data, timeout);
}

bool embot::hw::chip::MA730::read(Data &data, const embot::core::Callback &oncompletion)
{
    return pImpl->read(data, oncompletion);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

