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

#include "embot_app_theLEDmanager.h"

bool TestRunnerAmc_CM7::testCanComm(uint8_t *data)
{ 
    embot::core::print("Called method from testRunnerAmc_CM7");
    embot::core::wait(300* embot::core::time1millisec);	
    data[0] = 0xAA;
    return true; 
}

bool TestRunnerAmc_CM7::testFwVersion(){ return true; }

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

bool TestRunnerAmc_CM7::testVauxOK(){ return true; }

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
                    
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
