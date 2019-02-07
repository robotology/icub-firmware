
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

#ifndef _EMBOT_HW_H_
#define _EMBOT_HW_H_

#include "embot_common.h"


namespace embot { namespace hw {
    
    enum class result_t { OK = 0, NOK = -1, NOKtimeout = -2 };
    
    const result_t resOK            = result_t::OK;
    const result_t resNOK           = result_t::NOK;
    const result_t resNOKtimeout    = result_t::NOKtimeout;
    
    
    
    enum class CLOCK { syscore = 0, pclk1 = 1, pclk2 = 2, none = 31, maxnumberof = 3 };
    
    enum class LED : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, none = 31, maxnumberof = 8 };
            
    enum class CAN : std::uint8_t { one = 0, two = 1, none = 31, maxnumberof = 2};
            
    enum class FLASH : std::uint8_t { whole = 0, bootloader = 1, application = 2, sharedstorage = 3, applicationstorage = 4, none = 31, maxnumberof = 5 };
    
    enum class BTN : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, none = 31, maxnumberof = 8 };
    
    enum class PGA308 : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, none = 31, maxnumberof = 6 };
    
    enum class SI7051 { one = 0, two = 1, none = 31, maxnumberof = 2 };
    
    enum class ADC : std::uint8_t { one = 0, none = 31, maxnumberof = 1};
    
    enum class ONEWIRE : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, none = 31, maxnumberof = 6 };
    
    enum class TIMER : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eigth = 7, nine = 8, 
                                      ten = 9, eleven = 10, twelve = 11, thirteen = 12, fourteen = 13, fifteen = 14, sixteen = 15, 
                                      none = 31, maxnumberof = 16 }; 

    enum class I2C { one = 0, two = 1, none = 31, maxnumberof = 2 };     

    enum class BNO055 { one = 0, none = 31, maxnumberof = 1 };    
    
   
    struct GPIO
    {   // it is descripted by a pair{port, pin}. 
        void*           port;
        std::uint16_t   pin;
        
        GPIO(void *po, std::uint32_t pi) : port(po), pin(pi) {}
        GPIO() : port(nullptr), pin(0) {}            
        bool isvalid() const { if((nullptr == port) || (0 == pin)) { return false; } else { return true; } }       
    };
    
}} // namespace embot { namespace hw {






#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


