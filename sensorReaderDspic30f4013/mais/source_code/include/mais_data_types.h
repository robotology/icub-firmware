/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _MAIS_DATA_TYPES_H_
#define _MAIS_DATA_TYPES_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/** @file    mais_data_types.h
    @brief   In this file there are all data types used by mais application
    @author	 Valentina.gaggero@iit.it
    @date	
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"
#include "mais_config.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef struct
{
	uint8_t selected_ch;     // canale attualmente attivo
	uint16_t  values[ANALOG_CHANEL_NUM];
}mais_analog_ch_data_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


