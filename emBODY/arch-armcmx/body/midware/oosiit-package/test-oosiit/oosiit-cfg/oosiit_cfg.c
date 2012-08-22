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


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "oosiit_cfg.h"


// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// - user change-able function definitions for IIT extension. 
// - .....  
// --------------------------------------------------------------------------------------------------------------------

// empty



// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// - user change-able function definitions for RL-RTX  
// --------------------------------------------------------------------------------------------------------------------



void os_error (uint32_t err_code) {
  /* This function is called when a runtime error is detected. Parameter */
  /* 'err_code' holds the runtime error code (defined in RTL.H).         */

  /* HERE: include optional code to be executed on runtime error. */
  for (;;);
}




// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// - storage and some other function definitions for IIT extension and for RL-RTX. 
// - ...  
// --------------------------------------------------------------------------------------------------------------------


static const oosiit_params_cfg_t s_cfg =
{
    // task configuration
    .maxnumofusertasks              = OOSIIT_TASKCNT,             
    .checkStack                     = OOSIIT_STKCHECK,           
    .priviledgeMode                 = OOSIIT_RUNPRIV,             
    // systick timer configuration
    .osClock                        = OOSIIT_CLOCK,               
    .osTick                         = OOSIIT_TICK,                
    // system configuration
    .roundRobin                     = OOSIIT_ROBIN,               
    .roundRobinTimeout              = OOSIIT_ROBINTOUT,           
    .sizeISRFIFO                    = OOSIIT_FIFOSZ,              
    // iit extension
    .numAdvTimer                    = OOSIIT_ADVTIMERCNT,      
    .numMutex                       = OOSIIT_MUTEXCNT,         
    .numSemaphore                   = OOSIIT_SEMAPHORECNT,     
    .numMessageBox                  = OOSIIT_MBOXCNT,          
    .numMessageBoxElements          = OOSIIT_MBOXELEMENTCNT,   
    .sizeof64alignedStack           = OOSIIT_GLOBALSTACK     
};


extern const oosiit_params_cfg_t *oosiit_params_cfgMINE = &s_cfg;



#include "oosiit_storage.c"



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



