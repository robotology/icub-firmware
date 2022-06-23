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

// -
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "if2hw_common.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//static void dd_bsp_delay(uint64_t t);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// commented out as it is not used
//static const char s_eobj_ownname[] = "EOaction";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


//extern void if2hw_common_delay(uint64_t usec)
//{
//    dd_bsp_delay(usec);
//}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//__asm static void dd_hl_sys_asm_xnumARMv7ops(uint32_t numberof) 
//{
//   align
//dowaitloop
//   subs r0,r0,#1
//   bne dowaitloop
//   bx lr 
//   align    
//}


//#if __ARMCOMPILER_VERSION > 6000000
//int _execOperations(uint32_t i)
//{
//  int res = 0;
//  __asm 
//  (
//    "DOWAITLOOP:                            \t\n"
//    "SUBS %[input_i], %[input_i], #1        \t\n"
//    "BNE DOWAITLOOP                         \t\n"
//    : [result] "=&r" (res)
//    : [input_i] "r" (i)
//  );
//  return 0;
//}
//#else
//__asm void _execOperations(uint32_t numberof) 
//{
//   align
//dowaitloop
//   subs r0,r0,#1
//   bne dowaitloop
//   bx lr 
//   align    
//}
//#endif

//static void dd_bsp_delay(uint64_t t)
//{   
//    static uint64_t s_hl_sys_numofops1sec = 0;
//    static uint32_t s_hl_sys_used_systemcoreclock = 0;
//    if(s_hl_sys_used_systemcoreclock != SystemCoreClock)
//    {

//        // to occupy a millisec i execute an operation for a number of times which depends on: 
//        // SystemCoreClock, cortex gain(1.25 dmips/mhz), flash access, etc.
//        // to overcome all this i just consider SystemCoreClock w/out 1.25 gain and i measures
//        // extra gain with on a simple assembly function which should take 4 cycles per iteration (?).      
//        //s_hl_sys_numofops1sec = (5*(SystemCoreClock)) / 4; 
//        s_hl_sys_used_systemcoreclock = SystemCoreClock;
//        s_hl_sys_numofops1sec = SystemCoreClock;
//        
//        // with art technology enabled the flash is seen as fast as the cpu. wow. 
//        s_hl_sys_numofops1sec /= 3;             
//        // at this point i normalise the variable to keep not the nymber of operations for 1 sec,
//        // but for 1024*1024 microsec. by doing so, later on i shift by 20 instead of using a division. 
//        s_hl_sys_numofops1sec <<= 20;
//        s_hl_sys_numofops1sec /= 1000000;
//    }
//    
//    
//    volatile uint64_t num = s_hl_sys_numofops1sec * t;
//    num >>= 20; 
//    //num -= offset; //we may remove some cycles to compensates for previous instructions, but ... we dont do it. it depends on c compiler optimisation 
//    if(0 == num)
//    {
//        return;
//    }
//    _execOperations((uint32_t)num);
//}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




