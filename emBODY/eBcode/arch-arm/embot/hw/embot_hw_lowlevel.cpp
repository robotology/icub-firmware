
/*
 * Copyright (C) 2018 iCub Facility - Istituto Italiano di Tecnologia
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

#include "embot_hw_lowlevel.h"
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


using namespace std;



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace hw { namespace lowlevel {
    
#if     defined(STM32HAL_BOARD_NUCLEO64) || defined(STM32HAL_BOARD_MTB4) || defined(STM32HAL_BOARD_STRAIN2) || defined(STM32HAL_BOARD_RFE)

#if __ARMCOMPILER_VERSION > 6000000
int _execOperations(uint32_t i)
{
  int res = 0;
  __asm 
  (
    "DOWAITLOOP:                            \t\n"
    "SUBS %[input_i], %[input_i], #1        \t\n"
    "BNE DOWAITLOOP                         \t\n"
    : [result] "=&r" (res)
    : [input_i] "r" (i)
  );
  return 0;
}
#else
__asm void _execOperations(uint32_t numberof) 
{
   align
dowaitloop
   subs r0,r0,#1
   bne dowaitloop
   bx lr 
   align    
}
#endif

#else 
    #error you must define a _execassemblyoperations() function
#endif

    void asmEXECcycles(std::uint32_t numcycles)
    {
        _execOperations(numcycles);        
    }
  

}}} // namespace embot { namespace hw { namespace lowlevel {


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

