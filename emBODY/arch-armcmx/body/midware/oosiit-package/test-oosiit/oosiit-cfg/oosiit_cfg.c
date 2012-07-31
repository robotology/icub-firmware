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
// - user change-able function definitions for RL-RTX standard (V4.10). 
// - This part is the one contained into the RTX_Conf_CM.c after the the end of configuration section.  
// --------------------------------------------------------------------------------------------------------------------


// Standard library system mutexes
// ===============================
//  Define max. number system mutexes that are used to protect 
//  the arm standard runtime library. For microlib they are not used.
//#ifndef OS_MUTEXCNT
// #define OS_MUTEXCNT    0
//#endif

/*----------------------------------------------------------------------------
 *      RTX User configuration part END
 *---------------------------------------------------------------------------*/
 
//#define OS_TRV          ((uint32_t)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1)


/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/



/*--------------------------- os_idle_demon ---------------------------------*/

void os_idle_demon (void) {
  /* The idle demon is a system thread, running when no other thread is      */
  /* ready to run.                                                           */

  for (;;) {
  /* HERE: include optional user code to be executed when no thread runs.*/
  }
}


/*--------------------------- os_error --------------------------------------*/

void os_error (uint32_t err_code) {
  /* This function is called when a runtime error is detected. Parameter */
  /* 'err_code' holds the runtime error code (defined in RTL.H).         */

  /* HERE: include optional code to be executed on runtime error. */
  for (;;);
}




// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// - storage and some other function definitions for IIT extension and for RL-RTX standard (V4.10). 
// - ...  
// --------------------------------------------------------------------------------------------------------------------




static const oosiit_params_cfg_t s_cfg =
{
    // rtx v 4.13: task configuration
    OS_TASKCNT,             // uint8_t         numTask;                                // OS_TASKCNT
    OS_PRIVCNT,             // uint16_t        numTaskWithUserProvidedStack;           // OS_PRIVCNT
    OS_STKSIZE,             // uint16_t        sizeStack;                              // OS_STKSIZE
    OS_STKCHECK,            // uint8_t         checkStack;     // 1 bit                // OS_STKCHECK
    OS_RUNPRIV,             // uint8_t         priviledgeMode; // 1 bit                // OS_RUNPRIV
    // rtx v 4.13: systick timer configuration
    OS_CLOCK,               // uint32_t        osClock;                                // OS_CLOCK
    OS_TICK,                // uint32_t        osTick;                                 // OS_TICK
    // rtx v 4.13: system configuration
    OS_ROBIN,               // uint8_t         roundRobin; // 1 bit                    // OS_ROBIN
    OS_ROBINTOUT,           // uint32_t        roundRobinTimeout;                      // OS_ROBINTOUT
    OS_TIMERCNT,            // uint8_t         numTimer;                               // OS_TIMERCNT
    OS_FIFOSZ,              // uin8_t          sizeISRFIFO;                            // OS_FIFOSZ
    // iit extension
    OSIIT_ADVTIMERCNT,      // uint8_t         numAdvTimer;                            // OSIIT_ADVTIMERCNT 
    OSIIT_MUTEXCNT,         // uint8_t         numMutex;                               // OSIIT_MUTEXCNT
    OSIIT_SEMAPHORECNT,     // uint8_t         numSemaphore;                           // OSIIT_SEMAPHORECNT
    OSIIT_MBOXCNT,          // uint8_t         numMessageBox;                          // OSIIT_MBOXCNT
    OSIIT_MBOXELEMENTCNT,   // uint16_t        numMessageBoxElements;                  // OSIIT_MBOXELEMENTCNT
    OSIIT_GLOBALSTACK/8     // uint16_t        numElements64Stack;                     // OSIIT_GLOBALSTACK
};


extern const oosiit_params_cfg_t *oosiit_params_cfgMINE = &s_cfg;



#include "oosiit_storage.c"



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



