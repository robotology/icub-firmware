/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Maggiali, Marco Randazzo, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
#ifndef _ERRORS_H_
#define _ERRORS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/**	@file 	hal_errors.h
	@brief 	This file provides error management functions.
			Currently no application manages errors.
	@author claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------


#define HAL_RES_OK                  0       // general return value
#define HAL_RES_NOK                 255     // general return value


// error codes
#define ERR_MUX_INDEXING            1
#define ERR_DAC_VALUE2BIG           2

// CAN command parameters invalid 
#define ERR_CAN_PARAMETERS_INVALID  3
// CAN command unavailable 
#define ERR_CAN_COMMAND_UNAVAILABLE 4

// SG to TF matrix indexing invalid 
#define ERR_CAN_MATRIX_INDEXING     5

// IIR Filter Number of biquads too big  
#define ERR_CAN_IIR_NBQ2BIG         6
// IIR Filter Coeff Indexing error  
#define ERR_CAN_IIR_COEF_INDEXING   7

// SW CAN Messages Rx buffer overflow
#define ERR_CAN_RXBUFF_OVERFLOW     8



// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef uint16_t hal_result_t; 

typedef enum
{
	INFO_TRSET_isBOR = 9,
	INFO_TRSET_isPOR = 10,
	INFO_TRSET_isLVD = 11,
	INFO_TRSET_isMCLR = 12,
	INFO_TRSET_isWDTTO = 13,
	INFO_TRSET_isWDTWU = 14,
	INFO_TRSET_isWU = 15
}hal_error_reset_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn	extern hal_error_manage(uint8_t x);
  * @brief it is invoked when it is necessary to manage error, but actully it does nothing.
  * @param
  * @retval
  */
extern void hal_error_manage(uint8_t x); 


/**
  * @fn			extern void hal_error_canMsg_set( uint8_t *Txdata, int8_t *datalen, uint8_t can_cmd);
  * @brief		prepare CAN message that contains in the first byte of payload can_cmd value
                and in the following three "BUG". 
  * @param      Txdata: pointer to payload to build
                datalen : in output will contain length of payload
                can_cmd: the command that produces error.
  * @retval     none
  */
extern void hal_error_canMsg_set( uint8_t *Txdata, int8_t *datalen, uint8_t can_cmd);
/**
  * @fn		extern hal_error_reset_t hal_error_resetCause_get(void);
  * @brief  read reset register and return couse of the last reset
  * @param
  * @retval	one value of hal_error_reset_t that rappresents reset cause.
  */
extern hal_error_reset_t hal_error_resetCause_get(void);


/**
  * @fn		extern void hal_error_resetCause_send(uint8_t BoardAddress);
  * @brief  read reset register and send it by CAN
  * @param		BoardAddress: board CAN address (0-F) 
  * @retval
  */
extern void hal_error_resetCause_send(uint8_t BoardAddress);

/**
  * @fn		extern void hal_error_resetRegister_reset(void);
  * @brief  reset "Reset register"
  * @param	
  * @retval
  */
extern void hal_error_resetRegister_reset(void);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

