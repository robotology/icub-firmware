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

#ifndef _SYSTICKSERVICES_H_
#define _SYSTICKSERVICES_H_

// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"

#include "hl_core.h"

extern void SysTick_Handler(void);

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef void (*systickserv_void_fn_void_t)(void);

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

extern volatile uint32_t systickserv_numofticks; 


// - declaration of extern public functions ---------------------------------------------------------------------------

extern void systickserv_start_systick(uint32_t microsec, systickserv_void_fn_void_t onsystick);

extern void systickserv_wait_for(uint32_t microsec);



#endif  // include-guard



