
/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_RFE_H_
#define _EMBOT_HW_BSP_RFE_H_



#include "embot_hw_bsp.h"

#include "embot_hw_led.h"
//#include "embot_hw_si7051.h"
//#include "embot_hw_bno055.h"
//#include "embot_hw_pga308.h"

namespace embot { namespace hw { namespace bsp { namespace rfe {
    

    const embot::hw::led::LED ledBLUE = embot::hw::led::LED::one;
    
//    const embot::hw::i2c::Descriptor descrI2Cone = embot::hw::i2c::Descriptor(embot::hw::i2c::Bus::one, 400000);
//    const embot::hw::i2c::Descriptor descrI2Ctwo = embot::hw::i2c::Descriptor(embot::hw::i2c::Bus::two, 400000);
    
//    const embot::hw::SI7051::Sensor thermometerSGAUGES = embot::hw::SI7051::Sensor::one;
//    const embot::hw::SI7051::Config thermometerSGAUGESconfig = embot::hw::SI7051::Config(descrI2Cone);
        
        
//    const embot::hw::BNO055::Sensor imuBOSCH = embot::hw::BNO055::Sensor::one;
//    const embot::hw::BNO055::Config imuBOSCHconfig = embot::hw::BNO055::Config(descrI2Ctwo);   
    
//    const embot::hw::PGA308::Config configOfSGamps[6] = 
//    {
//        embot::hw::PGA308::Config(  embot::hw::gpio::GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin), embot::hw::gpio::State::SET, 
//                                    embot::hw::onewire::Channel::one, embot::hw::onewire::Config(embot::hw::gpio::GPIO(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin), embot::hw::onewire::Rate::tenKbps)),
//        embot::hw::PGA308::Config(  embot::hw::gpio::GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin), embot::hw::gpio::State::SET, 
//                                    embot::hw::onewire::Channel::two, embot::hw::onewire::Config(embot::hw::gpio::GPIO(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin), embot::hw::onewire::Rate::tenKbps)),
//        embot::hw::PGA308::Config(  embot::hw::gpio::GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin), embot::hw::gpio::State::SET, 
//                                    embot::hw::onewire::Channel::three, embot::hw::onewire::Config(embot::hw::gpio::GPIO(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin), embot::hw::onewire::Rate::tenKbps)),
//        embot::hw::PGA308::Config(  embot::hw::gpio::GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin), embot::hw::gpio::State::SET, 
//                                    embot::hw::onewire::Channel::four, embot::hw::onewire::Config(embot::hw::gpio::GPIO(W_STRAIN4_GPIO_Port, W_STRAIN4_Pin), embot::hw::onewire::Rate::tenKbps)),
//        embot::hw::PGA308::Config(  embot::hw::gpio::GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin), embot::hw::gpio::State::SET, 
//                                    embot::hw::onewire::Channel::five, embot::hw::onewire::Config(embot::hw::gpio::GPIO(W_STRAIN5_GPIO_Port, W_STRAIN5_Pin), embot::hw::onewire::Rate::tenKbps)),
//        embot::hw::PGA308::Config(  embot::hw::gpio::GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin), embot::hw::gpio::State::SET, 
//                                    embot::hw::onewire::Channel::six, embot::hw::onewire::Config(embot::hw::gpio::GPIO(W_STRAIN6_GPIO_Port, W_STRAIN2_Pin), embot::hw::onewire::Rate::tenKbps))                                    
//    };
        
      
}}}} // namespace embot { namespace hw { namespace bsp { namespace strain2 {




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


