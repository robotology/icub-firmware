
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

#include "stm32hal.h"


namespace embot { namespace hw {
    
    enum class result_t : std::int8_t { OK = 0, NOK = -1, NOKtimeout = -2 };
    
    const result_t resOK            = result_t::OK;
    const result_t resNOK           = result_t::NOK;
    const result_t resNOKtimeout    = result_t::NOKtimeout;
    
    
    enum class CLOCK : std::uint8_t { syscore = 0, pclk1 = 1, pclk2 = 2, none = 31, maxnumberof = 3 };
    
    enum class LED : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, none = 31, maxnumberof = 8 };
            
    enum class CAN : std::uint8_t { one = 0, two = 1, none = 31, maxnumberof = 2};
            
    enum class FLASH : std::uint8_t { whole = 0, bootloader = 1, application = 2, sharedstorage = 3, applicationstorage = 4, none = 31, maxnumberof = 5 };
    
    enum class BTN : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, none = 31, maxnumberof = 8 };
    
    enum class PGA308 : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, none = 31, maxnumberof = 6 };
    
    enum class SI7051 : std::uint8_t { one = 0, two = 1, none = 31, maxnumberof = 2 };
    
    enum class ADC : std::uint8_t { one = 0, none = 31, maxnumberof = 1};
    
    enum class ONEWIRE : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, none = 31, maxnumberof = 6 };
    
    enum class TIMER : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, nine = 8, 
                                      ten = 9, eleven = 10, twelve = 11, thirteen = 12, fourteen = 13, fifteen = 14, sixteen = 15, 
                                      none = 31, maxnumberof = 16 }; 

    enum class I2C : std::uint8_t { one = 0, two = 1, three = 2, none = 31, maxnumberof = 3 };     

    enum class BNO055 : std::uint8_t { one = 0, none = 31, maxnumberof = 1 };    
    
    enum class TLV493D : std::uint8_t { one = 0, two = 1, none = 31, maxnumberof = 2 };
    
    
    struct GPIO
    { 
        enum class PORT : std::uint8_t { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7, none = 31, maxnumberof = 8 };   
        enum class PIN : std::uint8_t { zero = 0, one = 1, two = 2, three = 3, four = 4, five = 5, six = 6, seven = 7, eight = 8, nine = 9, 
                                        ten = 10, eleven = 11, twelve = 12, thirteen = 13, fourteen = 14, fifteen = 15, none = 31, maxnumberof = 16 };
            
        PORT    port;
        PIN     pin;
        
        constexpr GPIO(PORT po, PIN pi) : port(po), pin(pi) {}
        constexpr GPIO() : port(PORT::none), pin(PIN::none) {}            
        constexpr bool isvalid() const { return (PORT::none == port) ? false : true; }       
    };
    
}} // namespace embot { namespace hw {






#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


