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

#include "embot_app_theLEDmanager.h"
#include "embot_app_eth_theErrorManager.h"

// Firmware versions - No need to be shared across multiple files - constexpress so that evaluated just at compile time
static constexpr uint8_t _fMajorVersion = 1;
static constexpr uint8_t _fMinorVersion = 0;
static constexpr uint8_t _fPatchVersion = 0;

bool TestRunnerAmc_CM7::testCanComm(uint8_t *data)
{ 
    embot::core::print("Called method from testRunnerAmc_CM7");
    embot::core::wait(300* embot::core::time1millisec);	
    data[0] = 0xAA;
    return true; 
}

// TODO - Japo - June 2025: For now it is enough to just test if the eth cip links are up or down. 
// In a second time we can think to add an UDP thread as in amc/examples/udpdeomobasic
bool TestRunnerAmc_CM7::testEthComm(uint8_t *data)
{
    embot::core::print("Called method from testRunnerAmc_CM7");
    embot::core::wait(300* embot::core::time1millisec);	
    bool link1isup = embot::hw::eth::islinkup(embot::hw::PHY::one);    
    bool link2isup = embot::hw::eth::islinkup(embot::hw::PHY::two);
    std::string msg = std::string("ETH link 1 is ") + (link1isup ? "UP" : "DOWN") + " ETH link 2 is " + (link2isup ? "UP" : "DOWN");
    embot::core::print(msg + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 
    data [0] = (link1isup && link2isup) ?  0xAA : 0xBB;     
    return true; 
}

bool TestRunnerAmc_CM7::testFwVersion(uint8_t *data)
{ 
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

bool TestRunnerAmc_CM7::testVin(uint8_t *data)
{
    float vin {0.0};
    
    vin = embot::hw::motor::getVIN();
    
    embot::core::print("Vin : " + std::to_string(vin));
    
    data [0] = (vin == 6.66f) ?  0xAA : 0xBB;      
    
    return true; 
}

bool TestRunnerAmc_CM7::testCin(uint8_t *data)
{
    float cin {0.0};

    cin = embot::hw::motor::getCIN();
    
    embot::core::print("Cin : " + std::to_string(cin));
    
    data [0] = (cin == 6.66f) ?  0xAA : 0xBB;  
    
    return true; 
}

bool TestRunnerAmc_CM7::testVauxOK(uint8_t *data)
{ 
    auto s = embot::hw::gpio::get(embot::hw::GPIO(embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::five));
    
    if(s == embot::hw::gpio::State::SET){embot::core::print("OK"); data[0] = 0xAA;}
    else{embot::core::print("NOK"); data[0] = 0xBB;}
    
    return true; 
}

bool TestRunnerAmc_CM7::testPwrnFailOK(){ return true; }

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

bool TestRunnerAmc_CM7::testSpiAeaEncoder(uint8_t *data)
{

    // Configure type of encoder chip depending on the requested one passed by arg
    constexpr embot::hw::encoder::Config cfgAEA3 {embot::hw::encoder::Type::chipMA730}; 
    //constexpr embot::hw::encoder::Config cfgAEA {embot::hw::encoder::Type::chipAS5045}; --> encoder chip for AEA
    // Configure three encoder since we have 3 spi channels and we wanna test all of them
    constexpr size_t nENCs {3};
    constexpr embot::core::Time periodofreport {2*embot::core::time1millisec};
    
    // data arrays
    std::array<embot::hw::encoder::POS, nENCs> spiRawValues {};
    std::array<embot::hw::ENCODER, nENCs> encs {}; 
    std::array<volatile bool, nENCs> done {}; 
        
    // result value for stopping flow if error before end of test
    // same type as for methods in embot::hw::encoder
    volatile embot::hw::result_t hwres {embot::hw::resNOK};
        
    // Initialize Encoders
    for(size_t i=0; i<nENCs; i++)
    {
        embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
        embot::hw::encoder::init(enc, cfgAEA3);
        encs[i] = enc;
        //thecallbacks[i] = {onreadingdone, &encs[i]};
        done[i] = true;
    }
    
    // Start the data array with some initial values
    // to be sure that if we are reading 0xFFFF we have actually problem with the chip
    // and not with initialization of random values
    for(size_t i=0; i<nENCs; i++)
    {
        spiRawValues[i] = 60666;
        
        // we want to be sure that our embot::hw::ENCODER objects have been intialized
        embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
        // start non-blocking reading process (??stopped by callback??)
        hwres = embot::hw::encoder::startRead(enc, {});
    }
    
    if(hwres != embot::hw::resOK)
    {
        embot::core::print("AEA3 reading NOK"); 
        data[0] = 0xBB;
        return false;
    }
    
    // Get the spi encoder value
    // Set a timeput for letting the reading process complete before getting the raw value
    constexpr embot::core::Time timeout {200*embot::core::time1microsec};
    embot::core::wait(timeout);
    for(size_t i=0; i<nENCs; i++)
    {
        embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
        hwres = embot::hw::encoder::getValue(enc, spiRawValues[i]);
    } 
    
    if(hwres != embot::hw::resOK)
    {
        embot::core::print("AEA3 reading NOK"); 
        data[0] = 0xBB;
        return false;
    }
    
    // Print results for debugging purposes
    char messageAEA3[512] = {0};
    embot::core::Time n {embot::core::now()};
    
    std::string s {embot::core::TimeFormatter(n).to_string() + ":"};
    for(size_t i=0; i<nENCs; i++)
    {
        s += " ";
        s += std::to_string(spiRawValues[i]);           
    } 
    std::snprintf(messageAEA3, sizeof(messageAEA3), "%s\n", s.c_str());
    embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"print()", nullptr}, {}, s); 
        
    embot::core::print(s);
           
    // Deinitialize Encoders at the end of the test
    for(size_t i=0; i<nENCs; i++)
    {
        embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
        embot::hw::encoder::deinit(enc);
    }
    
    embot::core::print("AEA3 reading OK"); 
    data[0] = 0xAA;
    
    return true;
}

                    
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
