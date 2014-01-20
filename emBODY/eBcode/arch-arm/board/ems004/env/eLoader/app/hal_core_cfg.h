/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _HAL_CORE_CFG_H_
#define _HAL_CORE_CFG_H_

// --------------------------------------------------------------------------------------------------------------------
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

// <h> Dynamic configuration of core part of HAL
// <i> It holds configuration for objects used in core part of HAL with limitation on dynamic behaviour (RAM)



// <h> BASE module 
// <i> It contains ... nothing so far

// <h> nothing so far 
// </h>nothing so far

// </h>BASE module

// <h> CPU module 
// <i> It contains ... nothing so far

// <h> nothing so far 
// </h>nothing so far

// </h>CPU module


// <h> FLASH module 
// <i> It contains ... nothing so far

// <h> nothing so far 
// </h>nothing so far

// </h>FLASH module

// <h> SYS module 
// <i> It contains stack and heap size and some externally functions

//   <o> stack size         <0x0-0xFFFFFFFF:8>
//   <i> define how much stack you want.
#ifndef HAL_SYS_CFG_STACKSIZE
 #define HAL_SYS_CFG_STACKSIZE      0x00002000
#endif

//   <o> heap size         <0x0-0xFFFFFFFF:8>
//   <i> define how much heap you want.
#ifndef HAL_SYS_CFG_HEAPSIZE
 #define HAL_SYS_CFG_HEAPSIZE      0x0000A000
#endif

// </h>SYS module





// </h>






// --------------------------------------------------------------------------------------------------------------------
//------------- <<< end of configuration section >>> ------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


