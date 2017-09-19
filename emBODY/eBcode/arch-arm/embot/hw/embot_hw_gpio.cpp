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


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_gpio.h"
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace hw { namespace gpio {

#if     !defined(HAL_GPIO_MODULE_ENABLED)

    result_t configure(const GPIO &g, Mode m)      { return resNOK; }      
    result_t set(const GPIO &g, State s)        { return resNOK; }
    
#else    
   
    result_t configure(const GPIO &g, Mode m)
    {
        if(m == Mode::OUTPUTopendrain)
        {
            LL_GPIO_SetPinMode(static_cast<GPIO_TypeDef *>(g.port), g.pin, LL_GPIO_MODE_OUTPUT);		
            LL_GPIO_SetPinOutputType(static_cast<GPIO_TypeDef *>(g.port), g.pin, LL_GPIO_OUTPUT_OPENDRAIN);
        }
        else if(m == Mode::OUTPUTpushpull)
        {
            LL_GPIO_SetPinMode(static_cast<GPIO_TypeDef *>(g.port), g.pin, LL_GPIO_MODE_OUTPUT);		
            LL_GPIO_SetPinOutputType(static_cast<GPIO_TypeDef *>(g.port), g.pin, LL_GPIO_OUTPUT_PUSHPULL);            
        }
        return resOK;
    }  
    
    result_t set(const GPIO &g, State s)
    {
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef *>(g.port), g.pin, static_cast<GPIO_PinState>(s));    
        return resOK;        
    }
    
#endif
       
}}} // namespace embot { namespace hw { namespace gpio     



    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

