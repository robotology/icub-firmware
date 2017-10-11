
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

#ifndef _EMBOT_HW_BSP_H_
#define _EMBOT_HW_BSP_H_

#include "embot_common.h"
#include "embot_hw.h"


namespace embot { namespace hw { namespace bsp {
    
    
    struct stm32halConfig
    {   // what the the stm32 hal requires to operate it internals. if nullptr, the stm32hal uses fake implemention.
        embot::common::fpWorker     init1millitick;    
        embot::common::fpGetU32     get1millitick;  
        stm32halConfig() : init1millitick(nullptr), get1millitick(nullptr) {}  
        stm32halConfig(embot::common::fpWorker ini, embot::common::fpGetU32 get) : init1millitick(ini), get1millitick(get) {}             
    };
    
    struct Config
    {
        stm32halConfig              stm32hal; 
        embot::common::fpGetU64     get1microtime;  // in usec. it is used as extra precision
        Config() : get1microtime(nullptr) {}
        Config(const stm32halConfig &_stm32, embot::common::fpGetU64 _tmicro) : stm32hal(_stm32), get1microtime(_tmicro) {}
    }; 
    
        
    bool initialised();
    
    result_t init(const Config &config);
    
    // it returns time in microseconds. the precision is usec or ms and depends on how the bsp was configured
    embot::common::Time now();
              
}}} // namespace embot { namespace hw { namespace bsp {




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


