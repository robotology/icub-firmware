
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

#include "embot_hw_bsp_config.h"
#include "embot_hw_lowlevel.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


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
    
#if defined(STM32HAL_STM32L4) || defined(STM32HAL_STM32H7) || defined(STM32HAL_STM32G4)

#if defined(__clang__) || defined(__GNUC__)
// gcc-style inline assembly: valid for armclang (AC6), Arm Toolchain for Embedded (ATfE) and gcc.
// the counter is a read-write operand ("+r") because the loop modifies it, and the label is a local numeric label
// so that the asm block stays valid even if the compiler duplicates or inlines it.
int _execOperations(uint32_t i)
{
  __asm volatile
  (
    "1:                                     \n"
    "SUBS %[counter], %[counter], #1        \n"
    "BNE 1b                                 \n"
    : [counter] "+r" (i)
    :
    : "cc"
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
    #error you must define a _execOperations() function
#endif

    void asmEXECcycles(std::uint32_t numcycles)
    {
        _execOperations(numcycles);        
    }
  

}}} // namespace embot { namespace hw { namespace lowlevel {


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

