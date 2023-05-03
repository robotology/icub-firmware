
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

namespace embot::app::board::amc2c::mbd {
    
//    void hei(void *p)
//    {
//        static volatile uint64_t prev {0};
//        static volatile uint64_t delta {0};
//        volatile uint64_t no = embot::core::now();
//        delta = no - prev;
//        prev = no;        
//    }    
//     embot::core::Callback cc {hei, nullptr};

    void Startup(embot::prot::can::Address adr)
    {
        // in debug mode we have to click the "RUN" button when stucked on main, before it will be unlocked by CM7
        // otherwise, it seems that this initialization is not performed.
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
                case 0x001:
                {
                    // test 1
                    PwmPhaseSet(1, 2, 3);
                } break;
                
                case 0x002:
                {
                    // test 2
                } break;    

                default:
                {
                } break;
            }
        } 

        
        embot::app::board::amc2c::theMBD::getInstance().tick(input, output);
        
        
//        static uint32_t cnt {0};
//        if(0 == (cnt++ % 1))
//        {
////            embot::core::print("MBD is ticking on an amc2c");
//        
//            uint8_t b0 = cnt&0xff;
//            uint8_t b1 = (cnt>>8)&0xff;
//            uint8_t b2 = (cnt>>16)&0xff;
//            uint8_t b3 = (cnt>>24)&0xff;
//            embot::prot::can::Frame fr1 {0x202, 4, {b0, b1, b2, b3, 0, 0, 0, 0}};
//            output.push_back(fr1);            
//        }
//        
//        size_t numRXframes = input.size();
//        
//        if(numRXframes > 0)
//        {
//            // just for test: i get the first only and i send it back 
//            embot::prot::can::Frame f = input[0];
//            output.push_back(f);    
//        }        
    }

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


