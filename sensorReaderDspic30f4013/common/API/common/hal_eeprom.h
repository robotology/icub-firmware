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
#ifndef _EEPROM_H_
#define _EEPROM_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/**	@file 	hal_eeprom.h
	@brief 	This file provides functionality to access the EEPROM.
	@author claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/


// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"
#include "hal_errors.h"
#include <libpic30.h>
#include <p30f4013.h>

// - public #define  --------------------------------------------------------------------------------------------------
#define EE_address_t                                    _prog_addressT


#define HAL_EE_DECLARE_VAR_IN_EE(VAR_NAME, TYPE)        TYPE _EEDATA(1) VAR_NAME;

#define HAL_EE_GET_ADDR(EE_ADDR, VAR_NAME)              _init_prog_address(EE_ADDR, VAR_NAME);


 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


/**
  * @fn     extern hal_result_t hal_eeprom_read(uint8_t *buf, EE_address_t EE_address, uint16_t size);
  * @brief  copies @size bytes in @buf from eeprom since the addres of ee_data variable plus @offset
  * @param        buf: pointer where datas are copied
  * @param        EE_address: address of type "EEPROM" of a variable in EEPROM
  * @param        size: number of bytes to read
  * @retval  - HAL_RES_NOK if buff is NULL or size is 0.
             - HAL_RES_OK otherwise 
  */
extern hal_result_t hal_eeprom_read(uint8_t *buf, EE_address_t EE_address, uint16_t size);


/**
  * @fn     extern hal_result_t hal_eeprom_write(uint8_t *buf, EE_address_t EE_address, int16_t size);
  * @brief  writes in eeprom @size bytes pointed by @buf. 
			The writing starts from ee_data variable address plus @offset
  * @param        buf: pointer to data that will be copied
  * @param        EE_address: address of type "EEPROM" of a variable in EEPROM
  * @param        size: number of bytes that will be copied
  * @retval  - HAL_RES_NOK if buff is NULL or size is 0.
             - HAL_RES_OK otherwise 
  */
extern hal_result_t hal_eeprom_write(uint8_t *buf, EE_address_t EE_address, int16_t size);


// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


