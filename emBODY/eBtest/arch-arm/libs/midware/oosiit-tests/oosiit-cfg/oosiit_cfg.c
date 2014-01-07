/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       oosiit_cfg.c
	@brief      This file keeps internal implementation of the osiit.
	@author     marco.accame@iit.it
    @date       07/30/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "oosiit.h"
#include "stdlib.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "oosiit_cfg.h"



// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// - overriddable functions  
// --------------------------------------------------------------------------------------------------------------------


extern void oosiit_sys_error(oosiit_error_code_t errorcode) 
{
    volatile static uint32_t err = 0;

    //for(;;)
    {
        err = errorcode;
        err = err;
    }
}


extern void* oosiit_ext_calloc(uint32_t s, uint32_t n)
{
    return(calloc(s, n));
}


extern void oosiit_ext_free(void* m)
{
    free(m);
}


// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// - storage and some other function definitions for IIT extension and for RL-RTX. 
// - ...  
// --------------------------------------------------------------------------------------------------------------------

#if defined(HL_USE_BRD_MCBSTM32_F400)
    #undef OOSIIT_CLOCK
    #define OOSIIT_CLOCK 168000000
#endif

extern const oosiit_cfg_t oosiit_cfg_USER =
{
    .cpufreq                        = OOSIIT_CLOCK,               
    .ticktime                       = OOSIIT_TICK,     
    .capacityofpostpendcommandfifo  = OOSIIT_FIFOSZ,  
    .checkstackoverflow             = OOSIIT_STKCHECK,  
    .memorymode                     = OOSIIT_MEMMODE, //oosiit_memmode_static, //oosiit_memmode_dynamic,  
    .roundrobinenabled              = OOSIIT_ROBIN,         
    .roundrobinnumberofticks        = OOSIIT_ROBINTOUT,         


    .maxnumofusertasks              = OOSIIT_TASKCNT,             
        
    .numAdvTimer                    = OOSIIT_ADVTIMERCNT,      
    .numMutex                       = OOSIIT_MUTEXCNT,         
    .numSemaphore                   = OOSIIT_SEMAPHORECNT,     
    .numMessageBox                  = OOSIIT_MBOXCNT,          
    .numMessageBoxElements          = OOSIIT_MBOXELEMENTCNT,   
    .sizeof64alignedStack           = OOSIIT_GLOBALSTACK     
};


extern const oosiit_cfg_t *oosiit_cfg_USERptr = &oosiit_cfg_USER;







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



