
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_chip_M95512DF.h"

// --------------------------------------------------------------------------------------------------------------------
// - test code. i place this section in here because .... we just need the .h file
// --------------------------------------------------------------------------------------------------------------------
  
#if defined(EMBOT_HW_CHIP_M95512DF_enable_test)    

// it tests the chip and offers an example of use
bool embot::hw::chip::testof_M95512DF()
{    
    // this configuration tells about which spi bus to use, which are the control pins
    // and their low level GPIO configuration
    // some extra info:
    // 1. this configuration is typically used by the embot::hw::eeprom and defined
    //    inside embot::hw::eeprom::thebsp located inside mbot_hw_bsp_nameofboard.cpp
    // 2. the spi bus in here specified is initted by M95512DF code w/ a 
    //    call to embot::hw::spi::init() in a way that is specified by
    //    embot::hw::spi::thebsp typically placed inside embot_hw_bsp_nameofboard.cpp
    // 3. the control pins are initialised / deinitialised inside M95512DF only if
    //    embot::hw::chip::M95512DF::Config::PinControl::config.isvalid()
    constexpr embot::hw::chip::M95512DF::Config cfg 
    {
        embot::hw::SPI::six,  // the spi bus
        //{}, // dummy spi config
        {embot::hw::spi::Prescaler::eight, embot::hw::spi::DataSize::eight, embot::hw::spi::Mode::zero},
        {   // the control pins
            {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::eight},    // nS
            {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::twelve},   // nW
            {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::thirteen}, // nHOLD   
            {   // GPIO configuration of the control pins
                embot::hw::gpio::Mode::OUTPUTpushpull,
                embot::hw::gpio::Pull::nopull,
                embot::hw::gpio::Speed::veryhigh
            }        
        }    
    };
    
    // address in EEPROM, data to write, destination for data to read
    constexpr embot::hw::chip::M95512DF::ADR adr {64};
    static constexpr uint8_t bytes2write[8] {1, 2, 3, 4, 5, 6, 7, 8};
    constexpr embot::core::Data data2write {bytes2write, sizeof(bytes2write)};
    uint8_t bytes2read[4] {0};
    embot::core::Data data2read {bytes2read, sizeof(bytes2read)}; 
        
    // step 01: create the object
    embot::hw::chip::M95512DF *chipM95512DF = new embot::hw::chip::M95512DF;
    
    bool ok {false};
    
    // step 02: initialise it (for extra check i also deinit and init it again)   
    chipM95512DF->init(cfg);
    chipM95512DF->deinit();
    if(true == chipM95512DF->init(cfg))
    {  
        // step 03: write some data to EEPROM
        if(true == chipM95512DF->write(adr, data2write))
        {
            // step 04: read data back
            if(true == chipM95512DF->read(adr+1, data2read))
            {
                // step 05: compare them
                ok = (bytes2read[0] == bytes2write[1]) ? true : false;
            }
        }                   
    }

    // step 06: print result
    embot::core::print(ok ? "test chipM95512DF: OK" : "test chipM95512DF: KO");   
    
    // step 07: delete the object: the destructor also deinits
    delete chipM95512DF;
    
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

//#define SPI_USE_NON_BLOCKING

using namespace std;

using namespace embot::hw;


    
struct embot::hw::chip::M95512DF::Impl
{             
    enum class CMD : uint8_t {
        WREN = 0x06, WRDI = 0x04, RDSR = 0x05, WRSR = 0x01, 
        READ = 0x03, WRITE = 0x02, RDID = 0x83, WRID = 0x82, 
        RDLS = 0x83
    };
        
    static constexpr embot::core::relTime deftimeout {100*embot::core::time1millisec}; 
    static constexpr embot::core::relTime writetimeout {5*embot::core::time1millisec};    
    
    bool _initted {false};
    Config _config {};
    
    Impl() = default;    
    ~Impl() { deinit(); }
    
    bool init(const Config &cfg);   
    bool deinit();
    bool write(ADR adr, const embot::core::Data &data);
    bool read(ADR adr, embot::core::Data &data);        
    void writeprotect(bool enable);

private:

    // bit values in EEPROM status register
    static constexpr uint8_t EE_SR_SRWD {0x80};
    static constexpr uint8_t EE_SR_BP1 {0x08};
    static constexpr uint8_t EE_SR_BP0 {0x04};
    static constexpr uint8_t EE_SR_WEL {0x02};
    static constexpr uint8_t EE_SR_WIP {0x01};
    
    // values for BP1 and BP0 bits in status register
    static constexpr uint8_t EE_SR_BP_MASK {EE_SR_BP1|EE_SR_BP0};
    static constexpr uint8_t EE_SR_BP_NONE {0|0};
    static constexpr uint8_t EE_SR_BP_TOPQUARTER {0|EE_SR_BP0};
    static constexpr uint8_t EE_SR_BP_TOPHALF {EE_SR_BP1|0};
    static constexpr uint8_t EE_SR_BP_ALL {EE_SR_BP1|EE_SR_BP0};    
    
    void chipselect(bool enable);      
    void hold(bool enable);
    bool send(const embot::core::Data &data);
    bool recv(embot::core::Data &dd);
    bool cmd(CMD c);
    bool cmdtx(CMD c, uint8_t v);
    bool cmdrx(CMD c, uint8_t &v);
    ADR adjustaddress(ADR adr);
    bool writeinpage(ADR adr, const embot::core::Data &data);
    bool initpincontrol();
    bool deinitpincontrol();
};    

bool embot::hw::chip::M95512DF::Impl::initpincontrol()
{
//    static constexpr embot::hw::gpio::Config cfg 
//    {
//        embot::hw::gpio::Mode::OUTPUTpushpull,
//        embot::hw::gpio::Pull::nopull,
//        embot::hw::gpio::Speed::veryhigh
//    };

    if(true == _config.pincontrol.config.isvalid())
    {
        // marco.accame: cube-mx sets the value of the pin before initialization,
        // so in here we do the same. NOTE that embot::hw::gpio::set() allows that.
        embot::hw::gpio::set(_config.pincontrol.nS, embot::hw::gpio::State::SET);
        embot::hw::gpio::set(_config.pincontrol.nHOLD, embot::hw::gpio::State::SET);
        embot::hw::gpio::set(_config.pincontrol.nW, embot::hw::gpio::State::RESET);
        
        embot::hw::gpio::init(_config.pincontrol.nS, _config.pincontrol.config);    
        embot::hw::gpio::init(_config.pincontrol.nHOLD, _config.pincontrol.config);    
        embot::hw::gpio::init(_config.pincontrol.nW, _config.pincontrol.config);
    }
    
    return true;
}
 

bool embot::hw::chip::M95512DF::Impl::deinitpincontrol()
{
    if(true == _config.pincontrol.config.isvalid())
    {
        embot::hw::gpio::set(_config.pincontrol.nS, embot::hw::gpio::State::SET);
        embot::hw::gpio::set(_config.pincontrol.nHOLD, embot::hw::gpio::State::SET);
        embot::hw::gpio::set(_config.pincontrol.nW, embot::hw::gpio::State::RESET);
        
        embot::hw::gpio::deinit(_config.pincontrol.nS);    
        embot::hw::gpio::deinit(_config.pincontrol.nHOLD);    
        embot::hw::gpio::deinit(_config.pincontrol.nW);
    }
    
    return true;
}

bool embot::hw::chip::M95512DF::Impl::init(const Config &cfg)
{    
    if((true == _initted) && (false == cfg.isvalid()))
    {
        return false;
    }
    
    _config = cfg;
    
    initpincontrol();
    
    hold(false);
    writeprotect(false);
    if(resOK == embot::hw::spi::init(_config.spi, _config.spicfg))
    {
        _initted =  true;
    }
    
    return _initted; 
}

bool embot::hw::chip::M95512DF::Impl::deinit()
{  
    if(_initted)
    {
        deinitpincontrol();    
        embot::hw::spi::deinit(_config.spi);
        _initted = false;
    }
    
    return !_initted; 
}

bool embot::hw::chip::M95512DF::Impl::write(ADR adr, const embot::core::Data &data)
{
    // bool r {false};
    
    if((false == data.isvalid()) || ((adr + data.capacity) > storagesize))
    {   // no need to check if(adr >= storagesize)
        return false;
    }
    constexpr size_t pagemask {pagelength-1};
    embot::core::Data remainingdata {data.pointer, data.capacity};
    size_t delta {pagelength - (adr & pagemask)};
    for(; remainingdata.capacity; )
    {
        delta = (delta > remainingdata.capacity) ? remainingdata.capacity : delta;
        
        // enable write and write to the status register
        if(true == cmd(CMD::WREN))
        {
            embot::core::Data data2writeinpage {remainingdata.pointer, delta};
            if(true == writeinpage(adr, data2writeinpage))
            {
                // now we read the status until it is ok, but no longer than 5 ms
                embot::core::Time expirytime {embot::core::now() + writetimeout};
                uint8_t status {0};
                while((true == cmdrx(CMD::RDSR, status)) && (0 != (status & EE_SR_WIP)))
                {
                    if(embot::core::now() > expirytime)
                    {
                        // timeout ....
                        return false;
                    }
                }                        
            }
            // update values
            remainingdata.capacity -= delta;
            adr += delta;
            remainingdata.pointer = reinterpret_cast<uint8_t*>(remainingdata.pointer) + delta;
            delta = pagelength;                
        }                                               
    }            
    return true;
}   


bool embot::hw::chip::M95512DF::Impl::read(ADR adr, embot::core::Data &data)
{   
    if((false == data.isvalid()) || ((adr + data.capacity) > storagesize))
    {   // no need to check if(adr >= storagesize)
        return false;
    }
    
    bool r {false};
    adr = adjustaddress(adr);
    chipselect(true);
    uint8_t cc {embot::core::tointegral(CMD::READ)};            
    if(true == send({&cc, 1}))
    {
        if(true == send({&adr, 2}))
        {
            r = recv(data);                    
        }
    }
    chipselect(false);
    return r;
} 
   
    
void embot::hw::chip::M95512DF::Impl::chipselect(bool enable)
{
    embot::hw::gpio::set(_config.pincontrol.nS, enable ? embot::hw::gpio::State::RESET : embot::hw::gpio::State::SET);
}

void embot::hw::chip::M95512DF::Impl::hold(bool enable)
{
    embot::hw::gpio::set(_config.pincontrol.nHOLD, enable ? embot::hw::gpio::State::RESET : embot::hw::gpio::State::SET);
}

void embot::hw::chip::M95512DF::Impl::writeprotect(bool enable)
{
    embot::hw::gpio::set(_config.pincontrol.nW, enable ? embot::hw::gpio::State::SET : embot::hw::gpio::State::RESET);
}    

#if defined(SPI_USE_NON_BLOCKING)
volatile bool done {false};
void oncompletion(void *p)
{ 
    done = true;
}
#endif

bool embot::hw::chip::M95512DF::Impl::send(const embot::core::Data &data)
{
#if defined(SPI_USE_NON_BLOCKING)
    done = false;
    embot::hw::spi::write(_config.spi, data, {oncompletion, nullptr});    
    for(;;) { if(done) break; }
    return true;  
#else    
    return (resOK == embot::hw::spi::write(_config.spi, data, deftimeout)) ? true : false;   
#endif    
}

bool embot::hw::chip::M95512DF::Impl::recv(embot::core::Data &dd)
{
#if defined(SPI_USE_NON_BLOCKING)
    done = false;
    embot::hw::spi::read(_config.spi, dd, {oncompletion, nullptr});
    for(;;) { if(done) break; }
    return true;    
#else    
    return (resOK == embot::hw::spi::read(_config.spi, dd, deftimeout)) ? true : false;  
#endif    
}

bool embot::hw::chip::M95512DF::Impl::cmd(CMD c)
{
    bool r {false};
    chipselect(true);
    uint8_t cc {embot::core::tointegral(c)};
    r = send({&cc, 1});
    chipselect(false);
    return r;
} 

bool embot::hw::chip::M95512DF::Impl::cmdtx(CMD c, uint8_t v)
{
    bool r {false};
    chipselect(true);
    uint8_t cc {embot::core::tointegral(c)};
    if(true == send({&cc, 1}))            
    {
        uint8_t vv {v};
        r = send({&vv, 1});
    }            
    chipselect(false);
    return r;                        
} 

bool embot::hw::chip::M95512DF::Impl::cmdrx(CMD c, uint8_t &v)
{
    bool r {false};
    chipselect(true);
    uint8_t cc {embot::core::tointegral(c)};
    if(true == send({&cc, 1}))            
    {
        embot::core::Data data {&v, 1};
        r = recv(data);
    }            
    chipselect(false);
    return r;                        
}

embot::hw::chip::M95512DF::ADR embot::hw::chip::M95512DF::Impl::adjustaddress(embot::hw::chip::M95512DF::ADR adr)
{
//    return __REV16(adr);
    static_assert(sizeof(embot::hw::chip::M95512DF::ADR) == 2, "this code is valid only for an address of 16 bits");
    return (adr>>8) | ((adr&0xff)<<8);
}

bool embot::hw::chip::M95512DF::Impl::writeinpage(ADR adr, const embot::core::Data &data)
{
    bool r {false};
    adr = adjustaddress(adr);
    chipselect(true);
    uint8_t cc {embot::core::tointegral(CMD::WRITE)};
    if(true == send({&cc, 1}))
    {
        if(true == send({&adr, 2}))
        {
            r = send(data);                    
        }
    }
    chipselect(false);
    return r;
} 
        

// --------------------------------------------------------------------------------------------------------------------
// - interface to the pimpl
// --------------------------------------------------------------------------------------------------------------------
    
    
embot::hw::chip::M95512DF::M95512DF()
: pImpl(new Impl)
{ 

}

embot::hw::chip::M95512DF::~M95512DF()
{   
    delete pImpl;
}

bool embot::hw::chip::M95512DF::init(const Config &config)
{
    return pImpl->init(config);
}

bool embot::hw::chip::M95512DF::deinit()
{
    return pImpl->deinit();
}

bool embot::hw::chip::M95512DF::read(ADR adr, embot::core::Data &data)
{
    return pImpl->read(adr, data);
}

bool embot::hw::chip::M95512DF::write(ADR adr, const embot::core::Data &data)
{
    return pImpl->write(adr, data);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

