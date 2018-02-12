
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

#ifndef _EMBOT_HW_GPIO_H_
#define _EMBOT_HW_GPIO_H_

#include "embot_common.h"
#include "embot_hw.h"



namespace embot { namespace hw { namespace gpio {
    
    struct GPIO
    {   // generic gpio descriptor: every micro has a port and a pin. 
        void*           port;
        std::uint16_t   pin;
        
        void load(void *po, std::uint32_t pi) { port = po; pin = pi; } 
        GPIO(void *po, std::uint32_t pi) { load(po, pi); }
        GPIO() { load(nullptr, 0); }
        bool isvalid() const { if((nullptr == port) || (0 == pin)) { return false; } return true; }       
    };
    
    
    enum class State { RESET = 0, SET = 1 };
    
    enum class Mode { OUTPUTopendrain = 0, OUTPUTpushpull = 1 };
    
    result_t configure(const GPIO &g, Mode m);
    
    result_t set(const GPIO &g, State s);
    
    State get(const GPIO &g);
       
}}} // namespace embot { namespace hw { namespace gpio 





#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


