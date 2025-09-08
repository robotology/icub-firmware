/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

// APIs
#include "testRunnerAmc_CM7.h"

// embot library includes
#include "embot_core.h"
#include "embot_hw_motor.h"
#include "embot_hw_button.h"
#include "embot_hw_eth.h"
#include "embot_hw_encoder.h"
#include "embot_hw_i2c.h"
#include "embot_hw_eeprom.h"
#include "embot_os_Timer.h"

#include "embot_app_theLEDmanager.h"
#include "embot_app_eth_theErrorManager.h"

// other includes
#include <numeric>

// Firmware versions - No need to be shared across multiple files - constexpress so that evaluated just at compile time
static constexpr uint8_t _fMajorVersion = 1;
static constexpr uint8_t _fMinorVersion = 1;
static constexpr uint8_t _fPatchVersion = 0;

bool TestRunnerAmc_CM7::testCanComm(uint8_t *data)
{ 
    embot::core::print("Called method from testRunnerAmc_CM7");
    embot::core::wait(600* embot::core::time1millisec);	
    data[0] = 0xAA;
    return true; 
}

// TODO - Japo - June 2025: For now it is enough to just test if the eth cip links are up or down. 
// In a second time we can think to add an UDP thread as in amc/examples/udpdeomobasic
bool TestRunnerAmc_CM7::testEthComm(uint8_t *data)
{
    embot::core::print("Called method from testRunnerAmc_CM7");
    embot::core::wait(300* embot::core::time1millisec);	// wait some time to be sure that the link went up
    bool link1isup = embot::hw::eth::islinkup(embot::hw::PHY::one);    
    bool link2isup = embot::hw::eth::islinkup(embot::hw::PHY::two);
    std::string msg = std::string("ETH link 1 is ") + (link1isup ? "UP" : "DOWN") + " ETH link 2 is " + (link2isup ? "UP" : "DOWN");
    embot::core::print(msg + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 
    data [0] = (link1isup && link2isup) ?  0xAA : 0xBB;     
    return true; 
}

bool TestRunnerAmc_CM7::testFwVersion(uint8_t *data)
{ 
    // wait some time so that the GUI application is ready to receive the message
    embot::core::wait(1000*embot::core::time1millisec);
    // saving fw versions to data in order	
    data[0] = _fMajorVersion;
    data[1] = _fMinorVersion;
    data[2] = _fPatchVersion;
    return true; 
}

bool TestRunnerAmc_CM7::testLed(uint8_t on)
{
    embot::core::print("Called method from testRunnerAmc_CM7");
    switch(on)
    {
        case 0:
        {
            embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();
            theleds.get(embot::hw::LED::two).off();
            theleds.get(embot::hw::LED::three).off();
            theleds.get(embot::hw::LED::four).off();
            
        } break;
        case 1:
        {
            embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();
            theleds.get(embot::hw::LED::two).on();
            theleds.get(embot::hw::LED::three).on();
            theleds.get(embot::hw::LED::four).on();
        } break;
        
        default: break;
    }
    
    return true; 
}

bool TestRunnerAmc_CM7::testMicroId(uint8_t *data)
{ 
    uint16_t id_ver = DBGMCU->IDCODE & 0xffff;
    uint16_t id_rev = (DBGMCU->IDCODE >> 16) & 0xffff;
    
    //clean data array
    size_t size = (sizeof(data))/(sizeof(data[0]));
    for(uint8_t i = 0; i < size; i++) {data[i] = 0;}
    
    // wait some time so that the GUI application is ready to receive the message
    embot::core::wait(1000*embot::core::time1millisec);
    uint8_t rev1 = id_rev >> 8; //20
    uint8_t rev2 = id_rev;      //03
    uint8_t id1 = id_ver >> 8;  //64
    uint8_t id2 = id_ver;       //50
    
    data[0] = rev1;
    data[1] = rev2;
    data[2] = id1;
    data[3] = id2;
    
    return true; 
}

bool TestRunnerAmc_CM7::testVauxOK(uint8_t *data)
{ 
    auto s = embot::hw::gpio::get(embot::hw::GPIO(embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::five));
    
    if(s == embot::hw::gpio::State::SET){embot::core::print("OK"); data[0] = 0xAA;}
    else{embot::core::print("NOK"); data[0] = 0xBB;}
    
    return true; 
}

bool TestRunnerAmc_CM7::testPwrnFailOK(uint8_t *data)
{ 
    // Set an embot::hw::GPIO tyoe variable to desired PORT::pin to be checked
    constexpr embot::hw::GPIO PWR_nFAIL {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::zero};
    constexpr embot::hw::gpio::Config PWR_nFAIL_Cfg {
        embot::hw::gpio::Mode::INPUT, 
        embot::hw::gpio::Pull::nopull, 
        embot::hw::gpio::Speed::medium
    };
    embot::hw::gpio::init(PWR_nFAIL, PWR_nFAIL_Cfg);
    
    embot::hw::gpio::set(PWR_nFAIL, embot::hw::gpio::State::SET);        
    HAL_Delay(10); // wait for 10 ms to stabilize ...
    
    
    auto s = embot::hw::gpio::get(embot::hw::GPIO(embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::zero));
    
    embot::core::wait(10*embot::core::time1millisec); // wait for 10 ms to stabilize ...
    
    if(s == embot::hw::gpio::State::SET){embot::core::print("OK"); data[0] = 0xAA;}
    else{embot::core::print("NOK"); data[0] = 0xBB;}
    
    // Deinit the GPIO at the end
    embot::hw::gpio::deinit(PWR_nFAIL);
    
    return true; 
}

bool TestRunnerAmc_CM7::testFault(uint8_t on, uint8_t *data)
{
    // Initializing EXTERNAL_FAULT_BUTTON
    bool EXTFAULTisPRESSED = false;
    embot::hw::BTN buttonEXTfault {embot::hw::BTN::one};
    embot::hw::LED ledEXTfault {embot::hw::LED::one};
    embot::hw::button::init(buttonEXTfault, {embot::hw::button::Mode::TriggeredOnPressAndRelease, {nullptr, nullptr}, 0});
    switch(on)
    {
        case 0:
        {
            EXTFAULTisPRESSED = embot::hw::button::pressed(buttonEXTfault);
    
            // superfluous but we can do it
            if(true == EXTFAULTisPRESSED)
            {
                embot::app::theLEDmanager::getInstance().get(ledEXTfault).on();
            }
            else
            {
                //embot::hw::motor::fault(embot::hw::MOTOR::one, false);
                embot::app::theLEDmanager::getInstance().get(ledEXTfault).off();
            }
            
            if(!EXTFAULTisPRESSED){embot::core::print("OK"); data[0] = 0xAA;}
            else{embot::core::print("NOK"); data[0] = 0xBB;}
            
        } break;
        
        case 1:
        {
            EXTFAULTisPRESSED = embot::hw::button::pressed(buttonEXTfault);
                    
            embot::core::print("Result of embot::hw::button::pressed is: " + std::to_string(embot::hw::button::pressed(buttonEXTfault)));
            // superfluous but we can do it
            if(true == EXTFAULTisPRESSED)
            {
                embot::app::theLEDmanager::getInstance().get(ledEXTfault).on();
            }
            else
            {
                //embot::hw::motor::fault(embot::hw::MOTOR::one, false);
                embot::app::theLEDmanager::getInstance().get(ledEXTfault).off();
            }
            
            // TODO: to be removed --> now we hardcode the true state since they embot::hw::button::pressed() dummy method returns always FALSE
            EXTFAULTisPRESSED = true;
            
            embot::core::print("EXTFAULTisPRESSED is equal to: " + std::to_string(EXTFAULTisPRESSED));
            
            if(EXTFAULTisPRESSED){embot::core::print("OK"); data[0] = 0xAA;}
            else{embot::core::print("NOK"); data[0] = 0xBB;}
            
        } break;
        
        default: break;
    }
    
    return true;
}	

bool TestRunnerAmc_CM7::testI2C(uint8_t *data)
{
    // Test all 3 i2c channels
    // Configure channels
    enum class MODE {blocking};
    constexpr embot::core::Time timeout {300*embot::core::time1microsec}; 
    volatile embot::hw::result_t hwres {embot::hw::resNOK};
        
    constexpr size_t nI2Cs {3}; // maxnumberof is 4 but we are only using i2c1, i2c2, i2c3
    constexpr MODE mode {MODE::blocking};
    constexpr embot::hw::i2c::Config cfg {};
    std::array<embot::hw::i2c::ADR, nI2Cs> i2cAddresses {}; // vector for the addresses pinged. We should have 1 per channel

    // Default Config. We just need init all 3 channels that we have
    for(size_t i=0; i<nI2Cs; i++)
    {
        // Inizialize return values in data to 0xAA (true). 
        // Pass a different value per each channel
        data[i] = 0xAA;
        embot::hw::I2C i2c {static_cast<embot::hw::I2C>(i)};
        embot::hw::i2c::init(i2c, cfg);
    }
    
    // Discovery on the 3 channels 
    char sss[512] = {0};
    std::string s {};
        
    bool res = false;
    
    
    std::vector<embot::hw::i2c::ADR> adrsvec = {};
    
    for(size_t i=0; i<nI2Cs; i++)
    {
        embot::hw::I2C i2c {static_cast<embot::hw::I2C>(i)};
        if(!embot::hw::i2c::discover(i2c, adrsvec))
        {
            data[i] = 0xBB;
            res |= true;
        }
        else
        {
            data[i] = 0xAA;
        }
        
        // We should have discivered just 1 address per channel
        i2cAddresses[i] = adrsvec[0];
        
        s += "\n";
        s += "I2C channel #" + std::to_string((uint8_t)i2c) + " adrs: ";
        for(size_t j=0; j<adrsvec.size(); j++)
        {
            s += std::to_string(adrsvec[j]);
            s += " ";                
        }
        s += "\n";
    }
    std::snprintf(sss, sizeof(sss), "%s\n", s.c_str());
    embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"print()", nullptr}, {}, sss); 
    
    if(!res)
    {
        return false;
    }
            
    // Ping
    memset(sss,0,sizeof(sss));
    s = {};
    
    embot::core::relTime pingTimeout = 500*embot::core::time1millisec;
    
    for(size_t i=0; i<nI2Cs; i++)
    {
        embot::hw::I2C i2c {static_cast<embot::hw::I2C>(i)};
        if(!(embot::hw::i2c::ping(i2c, i2cAddresses[i], pingTimeout)))
        {
            data[i] = 0xBB;
            res |= true;
        }
        else
        {
            data[i] = 0xAA;
        }
        s += "\n";
        s += "I2C channel #" + std::to_string((uint8_t)i2c) + " pinged adr: " + std::to_string((uint8_t)i2cAddresses[i]);
        
        s += "\n";
    }
    std::snprintf(sss, sizeof(sss), "%s\n", s.c_str());
    embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"print()", nullptr}, {}, sss); 
    
    if(!res)
    {
        return false;
    }
    
    
    for(size_t i=0; i<nI2Cs; i++)
    {
        embot::hw::I2C i2c {static_cast<embot::hw::I2C>(i)};
        embot::hw::i2c::deinit(i2c);
    }
    
    return true;
}

bool TestRunnerAmc_CM7::testSpiAeaEncoder(uint8_t *data)
{

    // Configure type of encoder chip depending on the requested one passed by arg
    constexpr embot::hw::encoder::Config cfgAEA3 {embot::hw::encoder::Type::chipMA730}; 
    // constexpr embot::hw::encoder::Config cfgAEA {embot::hw::encoder::Type::chipAS5045}; --> encoder chip for AEA
    // Configure three encoder since we have 3 spi channels and we wanna test all of them
    constexpr size_t nENCs {3};
    // now just set for blocking reading call
    // timeout for blocking call to read. For sake of test blocking call is fine. No need to define callbacks
    // set a timeout for letting the reading process complete before getting the raw value
    constexpr embot::core::Time timeout {300*embot::core::time1microsec}; 
    
    // data arrays
    std::array<embot::hw::encoder::POS, nENCs> spiRawValues {};
    std::array<embot::hw::ENCODER, nENCs> encs {};
    std::array<volatile bool, nENCs> done {};
        
    // result value for stopping flow if error before end of test
    // same type as for methods in embot::hw::encoder
    volatile embot::hw::result_t hwres {embot::hw::resNOK};
        
    // Initialize Encoders
    // Start the data array with some initial values
    // to be sure that if we are reading 0xFFFF we have actually problem with the chip
    // and not with initialization of random values
    for(size_t i=0; i<nENCs; i++)
    {
        spiRawValues[i] = 60666;
        embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
        hwres = embot::hw::encoder::init(enc, cfgAEA3);
        done[i] = (hwres != embot::hw::resOK) ? false : true;
        data[i] = (hwres != embot::hw::resOK) ? 0xBB : 0xAA;
        encs[i] = enc;
    }
    

    for(size_t i=0; i<nENCs; i++)
    {
        if(!done[i])
        {
            embot::core::print("AEA3 #" + std::to_string(i) + " init NOK"); 
            return false;
        }
    }
    
    // Get the spi encoder value
    for(size_t i=0; i<nENCs; i++)
    {
        embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
        hwres = embot::hw::encoder::read(enc, spiRawValues[i], timeout);
        // Override done array for storing all reads
        done[i] = (hwres != embot::hw::resOK) ? false : true;
    } 
    
    // Print results for debugging purposes
    char messageAEA3[512] = {0};
    embot::core::Time n {embot::core::now()};
    
    std::string s {embot::core::TimeFormatter(n).to_string() + ":"};
    for(size_t i=0; i<nENCs; i++)
    {
        if(!done[i])
        {
            s += " AEA3 #" + std::to_string(i) + " read NOK. val: ";
            data[i] = 0xBB;
        }
        else
        {
            s += " AEA3 #" + std::to_string(i) + " read OK. val: ";
            data[i] = 0xAA;
        }
        s += std::to_string(spiRawValues[i]);           
    } 
    std::snprintf(messageAEA3, sizeof(messageAEA3), "%s\n", s.c_str());
    embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"print()", nullptr}, {}, s); 
        
    embot::core::print(s);
           
    // Deinitialize Encoders at the end of the test
    for(size_t i=0; i<nENCs; i++)
    {
        embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
        hwres = embot::hw::encoder::deinit(enc);
        done[i] = (hwres != embot::hw::resOK) ? false : true;
    }
    
    return true;
}

uint32_t data2readeeprom[16] = {0};
constexpr embot::hw::EEPROM eeprom2test {embot::hw::EEPROM::one};

//void onForever(void *p)
//{
//    embot::os::Timer *t = reinterpret_cast<embot::os::Timer*>(p);
//    
//    std::memset(data2readeeprom, 0, sizeof(data2readeeprom));
//    embot::core::Data data {data2readeeprom, sizeof(data2readeeprom)};
//    
//    embot::hw::eeprom::read(embot::hw::EEPROM::one, 0, data, 3*embot::core::time1millisec);


//    embot::core::TimeFormatter tf(embot::core::now());        
//    embot::core::print("@ " + tf.to_string(embot::core::TimeFormatter::Mode::full) + ": "+ t->name() + " w/ shots: " + std::to_string(t->shots()));    
//    constexpr size_t max {5};
//    if(t->shots() > max)
//    {
//        embot::core::print("stop it");    
//        t->stop();     

//        embot::core::print("and restart it");    
//        t->start();                
//    }
//}

bool TestRunnerAmc_CM7::testEEPROM(uint8_t *data)
{
    // init eeprom
    embot::hw::eeprom::init(eeprom2test, {});
        
    // setup eeprom data
    constexpr size_t capacity {2048};
    uint8_t dd[capacity] = {0};
    constexpr size_t adr2use {0};
    volatile uint8_t stophere = 0;
    embot::core::Time startread {0};
    embot::core::Time readtime {0};
    embot::core::Time startwrite {0};
    embot::core::Time writetime {0};
    
    static size_t cnt = 0;
    static uint8_t shift = 0;
    
    data[0] = 0xBB;
    
    for(size_t i=0; i<1000; i++)
    {
        
        size_t numberofbytes = capacity >> shift;
        
        if(shift>8)
        {
            shift = 0;
        }
        else
        {
            shift++;
        }
        
        
        std::memset(dd, 0, sizeof(dd));
        embot::core::Data dataHolder {dd, numberofbytes};
        constexpr embot::core::relTime tout {3*embot::core::time1millisec};
        
        startread = embot::core::now(); 
        embot::hw::eeprom::read(eeprom2test, adr2use, dataHolder, 3*embot::core::time1millisec);
        readtime = embot::core::now() - startread;
        stophere++;
        
        std::memset(dd, cnt, sizeof(dd));
        startwrite = embot::core::now(); 
        embot::hw::eeprom::write(eeprom2test, adr2use, dataHolder, 3*embot::core::time1millisec);
        writetime = embot::core::now() - startwrite;
        stophere++;
        
    //    embot::hw::eeprom::erase(eeprom2test, adr2use+1, 200, 3*embot::core::time1millisec);
    //    embot::hw::eeprom::erase(eeprom2test, 3*embot::core::time1millisec);  
        
        std::memset(dd, 0, sizeof(dd));
        embot::hw::eeprom::read(eeprom2test, adr2use, dataHolder, 3*embot::core::time1millisec);
        stophere++;
        
        embot::core::print("numberofbytes = " + std::to_string(numberofbytes) + ", read time = " + embot::core::TimeFormatter(readtime).to_string() + ", write time = " + embot::core::TimeFormatter(writetime).to_string());
        
        cnt++;
    }
    
    data[0] = 0xAA;
    return true;
}

                    
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
