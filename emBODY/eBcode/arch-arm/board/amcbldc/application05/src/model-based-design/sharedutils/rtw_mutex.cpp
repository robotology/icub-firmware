/*
 * Copyright (C) 2021 iCub Tech Facility - Istituto Italiano di Tecnologia
 * Author:  Ugo Pattacini
 * email:   ugo.pattacini@iit.it
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

#include "rtw_mutex.h"


// marco.accame: this code provides protection vs concurrent access between
// a user thread and the DMA IRQHandler for DMA1 Channel2. the mechanism
// is that the user-thread disables the interrupt to lock and reanables it
// to unlock. the IRQHandler must not do anything, so if we are in an 
// exception (irq handler) we dont lock / unlock. 
// you can use macro TEST_CALLING to verify the correct mechanism.

#include "stm32hal.h"

static bool IsException(void); 

//#define TEST_CALLING


#if defined(TEST_CALLING)
volatile int32_t test_numThreadMode {0};
volatile int32_t test_numHandlerMode {0};
#endif

void rtw_mutex_lock(void) 
{    
    if(false == IsException())
    {
        NVIC_DisableIRQ(DMA1_Channel2_IRQn);  
#if defined(TEST_CALLING)        
        test_numThreadMode++;
#endif                
    }  
#if defined(TEST_CALLING)    
    else
    {
        test_numHandlerMode++;
    }
#endif
}

void rtw_mutex_unlock(void) 
{
    if(false == IsException())
    { 
#if defined(TEST_CALLING)        
        test_numThreadMode--;    
#endif        
        NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    }
#if defined(TEST_CALLING)    
    else
    {
        test_numHandlerMode--;
    }  
#endif    
}


// static funtions
static bool IsException(void) 
{
    return(__get_IPSR() != 0U);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


