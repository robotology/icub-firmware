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


/* @file       hl_cfg_plus_board_stm32f4.c
	@brief      This file contains configuration variables for the hl utilities which are specific for a given board.
	@author     marco.accame@iit.it
    @date       10/21/2013
**/


// - modules to be built: contains the HL_USE_* macros ----------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "hl_common.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files
#include "hl_arch.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

//#include "hl_common_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


#if     defined(HL_USE_UTIL_SYS)

#include "hl_sys.h"


#if     defined(HL_USE_BRD_MCBSTM32_F400)

#elif   defined(HL_USE_BRD_EMS004)

#endif//defined(HL_USE_BRD_EMS004)


#endif//defined(HL_USE_UTIL_SYS)



#if     defined(HL_USE_UTIL_I2C)

#include "hl_i2c.h"

#if     defined(HL_USE_BRD_MCBSTM32_F400)

#elif   defined(HL_USE_BRD_EMS004)


#elif   defined(HL_USE_BRD_EMS4RD)


#endif

#endif//defined(HL_USE_UTIL_I2C)


#if     defined(HL_USE_UTIL_ETH)

#include "hl_eth.h"


#if     defined(HL_USE_BRD_MCBSTM32_F400)


#elif   defined(HL_USE_BRD_EMS004)

#elif   defined(HL_USE_BRD_EMS4RD)

#endif//defined(HL_USE_BRD_EMS4RD)

#endif//defined(HL_USE_UTIL_ETH)




#if defined(HL_USE_UTIL_ETHTRANS)

#include "hl_ethtrans.h"


#if defined(HL_USE_BRD_MCBSTM32_F400)


#elif defined(HL_USE_BRD_EMS004)


#elif defined(HL_USE_BRD_EMS4RD)


#endif//defined(HL_USE_BRD_EMS4RD)

#endif//defined(HL_USE_UTIL_ETHTRANSCEIVER)



#if     defined(HL_USE_UTIL_CAN)

#include "hl_can.h"

#if     defined(HL_USE_BRD_MCBSTM32_F400)


#elif   defined(HL_USE_BRD_EMS004)


#elif   defined(HL_USE_BRD_EMS4RD)


#endif

#endif//defined(HL_USE_UTIL_CAN)


#if     defined(HL_USE_UTIL_TIMER)

#include "hl_timer.h"

#if     defined(HL_USE_BRD_MCBSTM32_F400)


#elif   defined(HL_USE_BRD_EMS004)


#elif   defined(HL_USE_BRD_EMS4RD)


#endif

#endif//defined(HL_USE_UTIL_TIMER)



    

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-session




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



