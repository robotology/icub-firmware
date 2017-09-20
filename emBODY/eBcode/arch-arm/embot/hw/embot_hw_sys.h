
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

#ifndef _EMBOT_HW_SYS_H_
#define _EMBOT_HW_SYS_H_

#include "embot_common.h"
#include "embot_hw.h"



namespace embot { namespace hw { namespace sys {
    
    enum class CLOCK { syscore = 0, pclk1 = 1, pclk2 = 2, none = 32, maxnumberof = 3 };
      
    extern const std::uint32_t startOfFLASH;
    extern const std::uint32_t addressOfBootloader;
    extern const std::uint32_t maxsizeOfBootloader;
    extern const std::uint32_t addressOfStorage;
    extern const std::uint32_t maxsizeOfStorage;
    extern const std::uint32_t addressOfApplication;
    extern const std::uint32_t maxsizeOfApplication;
    
    std::uint32_t clock(CLOCK clk);
    
    void reset();
    
    bool canjump2address(std::uint32_t address);
    
    void jump2address(std::uint32_t address); // if it returns ... means that it has failed the jump because internally canjump2address() returns false
    
    void relocatevectortable(std::uint32_t offset);
    
    void delay(embot::common::Time t);
    
    std::uint32_t random();
    std::uint32_t minrandom();
    std::uint32_t maxrandom();
    
    int puts(const char* str);
    
}}} // namespace embot { namespace hw { namespace sys {




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


