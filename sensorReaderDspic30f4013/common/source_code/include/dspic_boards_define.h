/* 
 * Copyright (C) 2011 RobotCub Consortium, European Commission FP6 Project IST-004370
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
#ifndef _BOARDS_DEFINE_H_
#define _BOARDS_DEFINE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/** @file    board_define.h
    @brief   in this file there are board type macros and data types used for all application
    @author	 valentina.gaggero@iit.it
    @date
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------
//board types
#define BOARD_TYPE_STRAIN  0x06
#define BOARD_TYPE_MAIS    0x07

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef struct
{
	uint8_t version; //is equivalent to major number
	uint8_t release; //is equivalent to minor number
	uint8_t build;
}version_info_t; 

typedef struct
{
	version_info_t		fw_ExeFile;
	version_info_t		canProtocol; //can protocol version use only version and release fileds ==> (major.minor)
}version_srcCode_info_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

