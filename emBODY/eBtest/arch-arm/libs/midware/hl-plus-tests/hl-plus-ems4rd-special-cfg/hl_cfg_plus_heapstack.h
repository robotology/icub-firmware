/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HL_CFG_PLUS_HEAPSTACK_H_
#define _HL_CFG_PLUS_HEAPSTACK_H_

// in case of error from assembler, you surely forgot --cpreproc in menu asm-options/misc-controls
// or may also to include it into asm path in menu asm-options/include-paths

// -- definitions of macros used in hl

// <<< Use Configuration Wizard in Context Menu >>>   


// <h> heap configuration
//   <o> heap size (in bytes) <0x0-0xFFFFFFFF:8>
// </h>
#define HL_PLUS_SIZEOF_HEAP         0x00001000 

// <h> stack configuration
//   <o> stack size (in bytes) <0x0-0xFFFFFFFF:8>
// </h>
#define HL_PLUS_SIZEOF_STACK        0x00001000


#endif  // include-guard



