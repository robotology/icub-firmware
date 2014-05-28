/* 
 * Copyright (C) 2011 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author:	Valentina Gaggero
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
#ifndef _DSPIC30_STDINT_H_
#define _DSPIC30_STDINT_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/** @file	dspic30_stdint.h
   @brief	Since compiler C30 doesn't provide stdint.h file, this file supplies standard types defined in C99.
			if necessary, define min and max values for each type. (see limits.h file)
   @author	valentina.gaggero@iit.it
   @date	10/01/2011
*/

// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
#ifndef int8_t
typedef 	signed char			int8_t;
#define int8_t int8_t
#endif

#ifndef uint8_t
typedef 	unsigned char	uint8_t;
#define uint8_t uint8_t
#endif
//typedef 	short			int16_t;
////typedef		signed short	int16_t;
//typedef		unsigned short	uint16_t;

#ifndef int16_t
typedef		signed int		int16_t;
#define int16_t int16_t
#endif

#ifndef uint16_t
typedef		unsigned int	uint16_t;
#define unt16_t uint16_t
#endif

#ifndef int32_t
typedef		signed long 	int32_t;
#define int32_t int32_t
#endif

#ifndef uint32_t
typedef		unsigned long	uint32_t;
#define uint32_t uint32_t				
#endif
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
