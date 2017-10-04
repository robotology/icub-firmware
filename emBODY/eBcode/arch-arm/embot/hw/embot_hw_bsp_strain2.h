
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

#ifndef _EMBOT_HW_BSP_STRAIN2_H_
#define _EMBOT_HW_BSP_STRAIN2_H_



#include "embot_hw_bsp.h"

#include "embot_hw_led.h"
#include "embot_hw_si7051.h"
#include "embot_hw_bno055.h"

namespace embot { namespace hw { namespace bsp { namespace strain2 {
    

    const embot::hw::led::LED ledBLUE = embot::hw::led::LED::one;
    
    const embot::hw::i2c::Config configI2Cone = embot::hw::i2c::Config(400000);
    const embot::hw::i2c::Config configI2Ctwo = embot::hw::i2c::Config(400000);
    
    const embot::hw::SI7051::Sensor termometerSGAUGES = embot::hw::SI7051::Sensor::one;
    const embot::hw::SI7051::Config termometerSGAUGESconfig = embot::hw::SI7051::Config(embot::hw::i2c::Bus::one, configI2Cone);

        
    const embot::hw::SI7051::Sensor termometerCASE = embot::hw::SI7051::Sensor::two;
    const embot::hw::SI7051::Config termometerCASEconfig = embot::hw::SI7051::Config(embot::hw::i2c::Bus::two, configI2Ctwo);   
    
    
    const embot::hw::BNO055::Sensor imuBOSCH = embot::hw::BNO055::Sensor::one;
    const embot::hw::BNO055::Config imuBOSCHconfig = embot::hw::BNO055::Config(embot::hw::i2c::Bus::two, configI2Ctwo);   
        
      
}}}} // namespace embot { namespace hw { namespace bsp { namespace strain2 {




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


