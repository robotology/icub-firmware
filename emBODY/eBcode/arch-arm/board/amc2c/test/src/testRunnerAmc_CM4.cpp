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
#include "motorhal.h"
#include "embot_hw_button.h"
#include "embot_app_theLEDmanager.h"

// hal includes
#include "adcm.h"
#include "enc.h"
#include "hall.h"

// std includes
#include <numeric>

// Firmware versions - No need to be shared across multiple files - constexpress so that evaluated just at compile time
static constexpr uint8_t _fMajorVersion = 1;
static constexpr uint8_t _fMinorVersion = 0;
static constexpr uint8_t _fPatchVersion = 0;


bool TestRunnerAmc_CM4::testCanComm(uint8_t *data)
{
    embot::core::print("Called method from testRunnerAmc_CM4");
    embot::core::wait(300* embot::core::time1millisec);	
    data[0] = 0xAA;
    return true; 
}

bool TestRunnerAmc_CM4::testVin(uint8_t *data)
{
    // wait some time for letting the Vin stabilize
    embot::core::wait(10*embot::core::time1second);
    float vin {0.0};
    
    vin = embot::hw::motor::getVIN();
    
    embot::core::print("Vin : " + std::to_string(vin));
    
    uint16_t vin2Int = (static_cast<uint16_t>(vin * 1000));
    data[2] = ((vin2Int) & 0x0000ff00) >> 8;
    data[1] = ((vin2Int) & 0x000000ff);
    
    if(vin > 17.0 && vin < 20.0) data[0] = 0xAA;
    else data[0] = 0xBB;
    
    return true; 
}

bool TestRunnerAmc_CM4::testCin(uint8_t *data)
{        
    // wait some time for letting the Cin stabilize
    embot::core::wait(4*embot::core::time1second);
    
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
    
    int16_t cin2Int = (static_cast<int16_t>(cin * 1000));
    data[2] = ((cin2Int) & 0x0000ff00) >> 8;
    data[1] = ((cin2Int) & 0x000000ff);
    
    if(cin > 0.020 && cin < 0.080) data[0] = 0xAA;          
    else data[0] = 0xBB;
    
    return true; 
}

bool TestRunnerAmc_CM4::testFault(uint8_t on, uint8_t *data)
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
            
            embot::core::print("EXTFAULTisPRESSED is equal to: " + std::to_string(EXTFAULTisPRESSED));
            
            if(EXTFAULTisPRESSED){embot::core::print("OK"); data[0] = 0xAA;}
            else{embot::core::print("NOK"); data[0] = 0xBB;}
            
        } break;
        
        default: break;
    }
    
    return true;
}	

bool TestRunnerAmc_CM4::testHallSensor(uint8_t *data)
{	   
    embot::hw::motor::HallStatus hallStatus {0};
    uint16_t res {0}; // used for accumulating the hallStatus
    
    // disable ABZ enc, init HALL sensors and start them
    embot::hw::motor::enc::deinit(); // this is done just for security on the status of the GPIO since ABZ and HALL share same connector (they have different GPIO anc they can be used together though)
    embot::hw::motor::hall::init({});
    embot::hw::motor::hall::start({});
        

    for(int i=0; i<1000; i++)
    {
        
        if(embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, hallStatus) != embot::hw::resOK)
        {
            data[0] = 0xBB;
            return false;
        }
        else
        {
            // we can improve check on correctness of status change --> hallStatus might vary as: 0->1->2->3->0..., 
            // thus we can check that status alternation, which will tell us we are not reading random statuses too.
            data[0] = (hallStatus != 0x00) ? 0xAA : 0xBB;
        }
        
        embot::core::print("hallStatus has value: " + std::to_string(hallStatus));
        res += (uint16_t)hallStatus;
        embot::core::wait(10* embot::core::time1millisec);
    }
    // final check on accumulation of values
    data[0] = (res != 0) ? 0xAA : 0xBB;
    data[2] = ((res) & 0x0000ff00) >> 8;
    data[1] = ((res) & 0x000000ff);
    
    // disable hall
    embot::hw::motor::hall::deinit();
    return true;
}

bool TestRunnerAmc_CM4::testQuadEncoder(uint8_t *data)
{    
    uint8_t res {0};
    int32_t enc_position {0};
    embot::hw::motor::enc::Mode enc_mode = {64, 1, false, false};
    
    // disable HALL, init ABZ encoder and start them
    embot::hw::motor::hall::deinit();
    embot::hw::motor::enc::init({});
    embot::hw::motor::enc::start(enc_mode);
        
    for(int i=0; i<1000; i++){	
        if((HAL_GPIO_ReadPin(MOT_ENCA_GPIO_Port, MOT_ENCA_Pin)     != GPIO_PIN_RESET)) res |= 1;
        else res |= 2;
        if((HAL_GPIO_ReadPin(MOT_ENCB_GPIO_Port, MOT_ENCB_Pin)     != GPIO_PIN_RESET)) res |= 4;
        else res |= 8;
        if((HAL_GPIO_ReadPin(MOT_ENCZ_GPIO_Port, MOT_ENCZ_Pin)     != GPIO_PIN_RESET)) res |= 16;
        else res |= 32;

        embot::hw::motor::getencoder(embot::hw::MOTOR::one, enc_position);
        embot::core::wait(10* embot::core::time1millisec);
    }

    embot::core::print("Encoder Position:" + std::to_string(enc_position));
    embot::core::print("Result read from quad enc:" + std::to_string(res));
    if(res == 63) 
    {
        data[0] = 0xAA;
        data[4] = ((enc_position) & 0xff000000) >> 24;
        data[3] = ((enc_position) & 0x00ff0000) >> 16;
        data[2] = ((enc_position) & 0x0000ff00) >> 8;
        data[1] = ((enc_position) & 0x000000ff);
    }
    else// 42 if no status chnage. Steady in PIN_RESET
    {
        data[0] = 0xBB;
        return false;
    }
    
    
    // disable encoder
    embot::hw::motor::enc::deinit();
    return true;
}

static embot::hw::motor::Currents pwmCurrents(0, 0, 0);
constexpr int32_t currentThresholdMin = 260; //mA
constexpr int32_t currentThresholdMax = 340; //mA

void onCurrents_FOC_innerloop(void *owner, const embot::hw::motor::Currents * const currents)
{
    pwmCurrents.u = currents->u;
    pwmCurrents.v = currents->v;
    pwmCurrents.w = currents->w;
}
    
bool TestRunnerAmc_CM4::testPWMChannelx(const uint8_t channel, uint8_t *data)
{		
    uint8_t res {0};
    
    embot::hw::motor::setCallbackOnCurrents(embot::hw::MOTOR::one, onCurrents_FOC_innerloop, nullptr);
    
    data[0] = 0xBB;
    embot::hw::motor::PWMperc pwmPerc(0, 0, 0);
    switch(channel)
    {
        case 0:
        {
            pwmPerc.a = 50.0;
            pwmPerc.b = 0.0;
            pwmPerc.c = 0.0;
        } break;
        case 1:
        {
            pwmPerc.a = 0.0;
            pwmPerc.b = 50.0;
            pwmPerc.c = 0.0;
        } break;
        case 2:
        {
            pwmPerc.a = 0.0;
            pwmPerc.b = 0.0;
            pwmPerc.c = 50.0;
        } break;
    }
            
    // enable power supply, it should be already enabled, just check
    // enable motorX driver
    if((embot::hw::motor::enable(embot::hw::MOTOR::one, true)) == embot::hw::resOK)
    {
        // set the PWM pulses and check currents on the ADC
        // to configure I will use the embot::hw::motor::PWMperc()
        // or with embot::core::hw::motor::setpwm(h, u, v, w)
        //embot::hw::motor::init(embot::hw::MOTOR::one, {});
        
        // delay
        embot::core::wait(500* embot::core::time1millisec);
        
        if((embot::hw::motor::setPWM(embot::hw::MOTOR::one, pwmPerc)) != embot::hw::resOK)
        {
            data[0] = 0xBB;
            return false;
        }
        
        // delay
        embot::core::wait(5000* embot::core::time1millisec);
        
        embot::core::print("\nCurrents read are phase1: " + std::to_string(pwmCurrents.u) + " phase2: " + std::to_string(pwmCurrents.v) + " phase3: " + std::to_string(pwmCurrents.w));
        //check currents
        switch(channel)
        {
            case 0:
            {
                if(pwmCurrents.u >= currentThresholdMin && pwmCurrents.u <= currentThresholdMax)
                {
                    data[0] = 0xAA;
                }
            } break;
            case 1:
            {
                if(pwmCurrents.v >= currentThresholdMin && pwmCurrents.v <= currentThresholdMax)
                {
                    data[0] = 0xAA;
                }
            } break;
            case 2:
            {
                if(pwmCurrents.w >= currentThresholdMin && pwmCurrents.w <= currentThresholdMax)
                {
                    data[0] = 0xAA;
                }
            } break;
        }
        
        data[3] = (static_cast<int16_t>(pwmCurrents.u & 0xffff)) & 0x00ff;
        data[2] = ((static_cast<int16_t>(pwmCurrents.u & 0xffff)) & 0xff00) >> 8;
        data[5] = (static_cast<int16_t>(pwmCurrents.v & 0xffff)) & 0x00ff;
        data[4] = ((static_cast<int16_t>(pwmCurrents.v & 0xffff)) & 0xff00) >> 8;
        data[7] = (static_cast<int16_t>(pwmCurrents.w & 0xffff)) & 0x00ff;
        data[6] = ((static_cast<int16_t>(pwmCurrents.w & 0xffff)) & 0xff00) >> 8;
      
    }
    
    
    // delay
    embot::core::wait(2000* embot::core::time1millisec);
    
    // disable motorX driver
    // disable power supply/ absorption
    if((embot::hw::motor::enable(embot::hw::MOTOR::one, false)) != embot::hw::resOK)
    {
        data[0] = 0xBB;
    }
    
    return true;
}
