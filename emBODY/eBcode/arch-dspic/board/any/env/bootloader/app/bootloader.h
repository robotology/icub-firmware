/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       bootloader.h
    @brief      this file ...
    @author     valentina.gaggero@iit.it
    @date       06/17/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEsharedInfo.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern void bl_init(void);
    @brief      initialises the bootloader application 
 **/
extern void bl_init(void);


/** @fn         extern void bl_ticktimer_start(void);
    @brief      starts the tick timer 
 **/
extern void bl_ticktimer_start(void);


extern eEresult_t bl_ticktimer_has_just_expired(void);


extern eEresult_t bl_on_ticktimer_expired(uint8_t *lastexpiry);

extern void bl_run_application(void);

extern uint32_t bl_address_get(void);

extern const eEsharinfoPartable_t* bl_partitiontable_get(void);

extern void bl_address_set(uint32_t adr);

extern void bl_canfilters_set(uint32_t adr);

extern void bl_boardinfo_getversion(uint8_t *major, uint8_t *minor);



// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


