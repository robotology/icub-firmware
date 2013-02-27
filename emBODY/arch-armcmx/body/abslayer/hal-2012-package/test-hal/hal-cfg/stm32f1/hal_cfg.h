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
#ifndef _HAL_CFG_H_
#define _HAL_CFG_H_

// --------------------------------------------------------------------------------------------------------------------
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

// <h> Configuration of HAL
// <i> It holds configuration for objects used in OSAl


// <h> Target 
// <i> Define the target 

//   <o> CPU family         <0=>   Cortex M3  <1=>   Cortex M4
//   <i> Only Cortex M3 / M4 are now supported.
#ifndef HAL_CPUFAM
 #define HAL_CPUFAM      0
#endif

//   <o> CPU type  <0x00=>   STM32F1x   <0x01=>   STM32F4x
//   <i> Only STM32F1x / STM32F4x are now supported.
#ifndef HAL_CPUTYPE
 #define HAL_CPUTYPE       0x00
#endif


// </h>Target


// <h> System 
// <i> sssssssss

//   <o> stack size         <0x0-0xFFFFFFFF:8>
//   <i> define how much stack you want.
#ifndef HAL_STACKSIZE
 #define HAL_STACKSIZE      0x00002000
#endif

//   <o> heap size         <0x0-0xFFFFFFFF:8>
//   <i> define how much heap you want.
#ifndef HAL_HEAPSIZE
 #define HAL_HEAPSIZE      0x00008000
#endif


// </h>System







// </h>






// --------------------------------------------------------------------------------------------------------------------
//------------- <<< end of configuration section >>> ------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


