
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amc2c_mbd.h"
#include "embot_app_board_amc2c_theMBD.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_timer.h"

#include "adcm.h"
#include "enc.h"
#include "hall.h"
#include "pwm.h"

#include "embot_hw_motor.h"

extern void HallSetPwm(int32_t pwm);

using namespace embot::hw;

namespace embot::app::board::amc2c::mbd {
    
    // forward declaration
    result_t hallTest(int32_t set, bool enc_reading_enabled);
    
    result_t adcTest();


    void Startup(embot::prot::can::Address adr)
    {
        // in debug mode questa init non viene chiamata a meno che non si clicca "RUN" manualmente prima che venga sbloccato dal CM7
        embot::hw::motor::init(embot::hw::MOTOR::one, {});
        embot::app::board::amc2c::theMBD::getInstance().initialise({adr});
        
//        embot::hw::timer::Config cfg{embot::core::time1millisec, embot::hw::timer::Mode::periodic, cc};
//        embot::hw::timer::init(embot::hw::TIMER::one, cfg);
//        embot::hw::timer::start(embot::hw::TIMER::one);  
        
        
    }
    
    void OnTick(const std::vector<embot::prot::can::Frame> &input, std::vector<embot::prot::can::Frame> &output)
    {
        
        size_t numRXframes = input.size();
        
        if(numRXframes > 0)
        {
            embot::prot::can::Frame f = input[0];
            
            switch(f.id)
            {
                // Test Hall Sensors + Motor spinning + quad encoder reading [WIP]
                // ID = 1 and PAYLOAD = PWM(%), where -100 < PWM(%) < 100
                case 0x001:
                {
                    int32_t set {0};
                    static bool enc_reading_enabled {false};
                    
                    set = 65535 / 100 * f.data[0]; // pwm value to set
                    enc_reading_enabled = (f.data[1] == 1) ? true : false;
                    hallTest(set, enc_reading_enabled);
                } break;
                
                // Test PWM Output (ADC currents reading)
                case 0x002:
                {
                    // TODO: WIP
                    adcTest();
                    
                } break;

                default:
                {
                } break;
            }
        }
        
        embot::app::board::amc2c::theMBD::getInstance().tick(input, output);

        
//        if(numRXframes > 0)
//        {
//            // just for test: i get the first only and i send it back 
//            embot::prot::can::Frame f = input[0];
//            output.push_back(f);    
//        }
    }

    int32_t abs_pos {0};
    void qEncReading(embot::os::Thread *t, void *param)
    {
        EncCaptureIndex();
        abs_pos = EncGetAbsolutePosition();
    }
    
    result_t hallTest(int32_t set, bool enc_reading_enabled)
    {

        // NB: PwmInit, HallInit and EncInit must be called before this test.
        //     in general they are called in embot::hw::motor::init
        result_t res = resOK;

        // setup
        static int32_t pwm {0};
        
        // do 6-step control
        if ((-65536L <= set) && (65535 >= set))
        {
                if (set >= pwm)
                {
                    for ( ; pwm < set-100 ; pwm += 100) 
                    {
                        HallSetPwm(pwm);
                        embot::core::wait(10 * embot::core::time1millisec);
                    }
                }
                else
                {
                    for ( ; pwm > set-100 ; pwm -= 100) 
                    {
                        HallSetPwm(pwm);
                        embot::core::wait(10 * embot::core::time1millisec);
                    }
                }
                HallSetPwm(pwm = set);
        }
        else
        {
            // PWM set value is out of range
            res = resNOK;
        }
        
        
        
        static bool th_qEncReading_started { false };

        if (enc_reading_enabled)
        {
            if(false == th_qEncReading_started)
            {
                // start thread here
                
                
                embot::os::PeriodicThread::Config cfg {
                    2048,
                    embot::os::Priority::abovenorm32,
                    nullptr,
                    nullptr,
                    100 * embot::core::time1millisec,
                    qEncReading,
                    "qEncReadingThreadCfg"
                };
                
                embot::os::PeriodicThread *periodic_thread_qEnc = new embot::os::PeriodicThread;
                periodic_thread_qEnc->start(cfg);
                
                th_qEncReading_started = true;
            }
        }
        
        // TODO: embot::hw::motor::deinit?
        
        return res;
    }
    
    result_t adcTest()
    {
            int16_t uI, vI, wI;
            uint16_t uV, vV, wV;
            do
            {
                AdcMotGetCurrents(&uI, &vI, &wI);
                AdcMotGetVoltages(&uV, &vV, &wV);
                embot::core::wait(100 * embot::core::time1millisec);
            } while (1);
    }

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


