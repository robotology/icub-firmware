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


// <h> CMSIS-RTOS RTX (V4.20)
// <i> The same as the CMSIS-RTOS RTX configuration

// #IIT comment#
// the following code is adapted from file RTX_Conf_CM.C, V4.20, in the section between:
// RTX User configuration part BEGIN
// and: 
// end of configuration section
// the copyrigth notice can be found in file RTX_Conf_CM.C
// we retained the name task instead of thread to keep compatibility with low level CMSIS-RTOS RTX APIs.
// we have removed the user-timer section of CMSIS-RTOS RTX as the timers are implemented as an IIT extension
// #/IIT comment#

// <h>Task Configuration
// =====================
//
//   <o>Number of concurrent running tasks <0-250>
//   <i> Define max. number of tasks that will run at the same time.
//   <i> Default: 6
#ifndef OS_TASKCNT
 #define OS_TASKCNT     14
#endif

//   <o>Number of tasks with user-provided stack <0-250>
//   <i> Define the number of tasks that will use a bigger stack.
//   <i> The memory space for the stack is provided by the user.
//   <i> Default: 0
#ifndef OS_PRIVCNT
 #define OS_PRIVCNT     0
#endif

//   <o>Task stack size [bytes] <20-4096:8><#/4>
//   <i> Set the stack size for tasks which is assigned by the system.
//   <i> Default: 200
#ifndef OS_STKSIZE
 #define OS_STKSIZE     64
#endif

// <q>Check for the stack overflow
// ===============================
// <i> Include the stack checking code for a stack overflow.
// <i> Note that additional code reduces the Kernel performance.
#ifndef OS_STKCHECK
 #define OS_STKCHECK    1
#endif

// <q>Run in privileged mode
// =========================
// <i> Run all Tasks in privileged mode.
// <i> Default: Unprivileged
#ifndef OS_RUNPRIV
 #define OS_RUNPRIV     1
#endif

// </h>
// <h>SysTick Timer Configuration
// =============================
//   <o>Timer clock value [Hz] <1-1000000000>
//   <i> Set the timer clock value for selected timer.
//   <i> Default: 6000000  (6MHz)
#ifndef OS_CLOCK
 #define OS_CLOCK       72000000
#endif

//   <o>Timer tick value [us] <1-1000000>
//   <i> Set the timer tick value for selected timer.
//   <i> Default: 10000  (10ms)
#ifndef OS_TICK
 #define OS_TICK        1000
#endif

// </h>

// <h>System Configuration
// =======================
// <e>Round-Robin Task switching
// =============================
// <i> Enable Round-Robin Task switching.
#ifndef OS_ROBIN
 #define OS_ROBIN       1
#endif

//   <o>Round-Robin Timeout [ticks] <1-1000>
//   <i> Define how long a task will execute before a task switch.
//   <i> Default: 5
#ifndef OS_ROBINTOUT
 #define OS_ROBINTOUT   50
#endif

// </e>

#warning ---> REMOVE USER TIMER and align it with CMSIS version

//   <o>Number of user timers <0-250>
//   <i> Define max. number of user timers that will run at the same time.
//   <i> Default: 0  (User timers disabled)
#ifndef OS_TIMERCNT
 #define OS_TIMERCNT    4
#endif

//   <o>ISR FIFO Queue size<4=>   4 entries  <8=>   8 entries
//                         <12=> 12 entries  <16=> 16 entries
//                         <24=> 24 entries  <32=> 32 entries
//                         <48=> 48 entries  <64=> 64 entries
//                         <96=> 96 entries
//   <i> ISR functions store requests to this buffer,
//   <i> when they are called from the interrupt handler.
//   <i> Default: 16 entries
#ifndef OS_FIFOSZ
 #define OS_FIFOSZ      16
#endif

// </h>

// </h>


// <h> IIT extension
// <i> It holds IIT extension

//   <o>Number of advanced IIT timers <0-250>
//   <i> Define max. number of user timers that will run at the same time.
//   <i> Default: 0  (User timers disabled)
#ifndef OSIIT_ADVTIMERCNT
 #define OSIIT_ADVTIMERCNT    6
#endif


//   <o>Number of mutexes <0-250>
//   <i> Define max. number of mutexes that will run at the same time.
//   <i> Default: 0  (Mutexes not enabled)
#ifndef OSIIT_MUTEXCNT
 #define OSIIT_MUTEXCNT    5
#endif


//   <o>Number of semaphores <0-250>
//   <i> Define max. number of semaphores that will run at the same time.
//   <i> Default: 0  (Semaphores not enabled)
#ifndef OSIIT_SEMAPHORECNT
 #define OSIIT_SEMAPHORECNT    5
#endif


//   <o>Number of message boxes <0-250>
//   <i> Define max. number of message boxes that that will run at the same time.
//   <i> Default: 0  (Message boxes not enabled)
#ifndef OSIIT_MBOXCNT
 #define OSIIT_MBOXCNT    5
#endif

//   <o>Total max number of elements in message boxes <0-1000>
//   <i> Define max. number of elements that can be contained in every message box.
//   <i> Default: 0  (Message boxes not enabled)
#ifndef OSIIT_MBOXELEMENTCNT
 #define OSIIT_MBOXELEMENTCNT    20
#endif


//   <o>Total max size for overall user defined stack [bytes] <0-16384:8>
//   <i> Define max. size in bytes of the global stack.
//   <i> Default: 0  (user defined stack not enabled)
#ifndef OSIIT_GLOBALSTACK
 #define OSIIT_GLOBALSTACK    4096
#endif


// </h>

// </h>

// --------------------------------------------------------------------------------------------------------------------
//------------- <<< end of configuration section >>> ------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


