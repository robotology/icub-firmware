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

 
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _OOSIIT_CFG_H_
#define _OOSIIT_CFG_H_

// --------------------------------------------------------------------------------------------------------------------
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------


// <h> Configuration of OOSIIT
// <i> It holds params both for CMSIS-RTOS RTX and for IIT extension


// <h> basics
// <i> The basic configuration

// #IIT comment#
// the following code is adapted from file RTX_Conf_CM.C, V4.20 or successive, in the section between:
// RTX User configuration part BEGIN
// and: 
// end of configuration section
// the copyrigth notice can be found in file RTX_Conf_CM.C
// we retained the name task instead of thread to keep compatibility with low level CMSIS-RTOS RTX APIs.
// we have removed the user-timer section of CMSIS-RTOS RTX as the timers are implemented as an IIT extension
// #/IIT comment#


// <h>timing configuration
// =============================
//   <o> cpu speed [Hz] <1-1000000000>
//   <i> set the cpu speed.
//   <i> Default: 72000000  (72MHz)
#ifndef OOSIIT_CLOCK
 #define OOSIIT_CLOCK       72000000
#endif

//   <o> oosiit tick value [us] <1-1000000>
//   <i> set the tick value for the oosiit.
//   <i> Default: 1000  (1000 usec)
#ifndef OOSIIT_TICK
 #define OOSIIT_TICK        1000
#endif


// </h>


// <h> system configuration

//   <o>capacity of fifo queue used for oosiit calls inside ISRs <4=>   4 entries  <8=>   8 entries
//                         <12=> 12 entries  <16=> 16 entries
//                         <24=> 24 entries  <32=> 32 entries
//                         <48=> 48 entries  <64=> 64 entries
//                         <96=> 96 entries
//   <i> ISR functions store requests to this buffer,
//   <i> when they are called from the interrupt handler.
//   <i> Default: 16 entries
#ifndef OOSIIT_FIFOSZ
 #define OOSIIT_FIFOSZ      16
#endif


// <q> check stack overflow
// ===============================
// <i> includes the stack checking code for a stack overflow.
#ifndef OOSIIT_STKCHECK
 #define OOSIIT_STKCHECK    1
#endif

// =======================
// <e>round-robin task switching
// =============================
// <i> enable round-robin task switching.
#ifndef OOSIIT_ROBIN
 #define OOSIIT_ROBIN       1
#endif

//   <o>round-robin timeout [ticks] <1-1000>
//   <i> define how long a task will execute before a task switch.
//   <i> default: 5
#ifndef OOSIIT_ROBINTOUT
 #define OOSIIT_ROBINTOUT   50
#endif

// </e>



// </h>



// </h>


// <h>memory mode

//   <o>memory mode <0=>   static    <1=>   dynamic    
//   <i> if dynamic the following numbers are not relevant anymore as they are allocated on demand
//   <i> moreover, the task control block is allocated on task creation
#ifndef OOSIIT_MEMMODE
 #define OOSIIT_MEMMODE    0
#endif

// </h>









// <h> static mode configuration
// <i> It holds the maximum number of items that the static memory mode can support



//   <o> max number of user tasks <0-250>
//   <i> Default: 6
#ifndef OOSIIT_TASKCNT
 #define OOSIIT_TASKCNT     8
#endif



//   <o> max number of advanced timers <0-250>
//   <i> define max. number of timers that will run at the same time.
//   <i> default: 0  (timers disabled)
#ifndef OOSIIT_ADVTIMERCNT
 #define OOSIIT_ADVTIMERCNT    8
#endif


//   <o> max number of mutexes <0-250>
//   <i> Define max. number of mutexes that will run at the same time.
//   <i> Default: 0  (Mutexes not enabled)
#ifndef OOSIIT_MUTEXCNT
 #define OOSIIT_MUTEXCNT    8
#endif


//   <o> max number of semaphores <0-250>
//   <i> Define max. number of semaphores that will run at the same time.
//   <i> Default: 0  (Semaphores not enabled)
#ifndef OOSIIT_SEMAPHORECNT
 #define OOSIIT_SEMAPHORECNT    8
#endif


//   <o> max number of message boxes <0-250>
//   <i> Define max. number of message boxes that that will run at the same time.
//   <i> Default: 0  (Message boxes not enabled)
#ifndef OOSIIT_MBOXCNT
 #define OOSIIT_MBOXCNT    8
#endif

//   <o> max number of elements in message boxes <0-1000>
//   <i> Define max. number of elements that can be contained in every message box.
//   <i> Default: 0  (Message boxes not enabled)
#ifndef OOSIIT_MBOXELEMENTCNT
 #define OOSIIT_MBOXELEMENTCNT    80
#endif


//   <o> total internal stack [bytes] <0-16384:8>
//   <i> Define max. size in bytes of the global stack.
//   <i> Default: 0  
#ifndef OOSIIT_GLOBALSTACK
 #define OOSIIT_GLOBALSTACK    8000
#endif


// </h>

// </h>

// --------------------------------------------------------------------------------------------------------------------
//------------- <<< end of configuration section >>> ------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


