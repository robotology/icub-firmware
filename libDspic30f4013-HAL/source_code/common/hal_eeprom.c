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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*	@file 	hal_eeprom.c
	@brief 	This file implements functionality to access the EEPROM.
	@author claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <string.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_eeprom.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_eeprom_read(uint8_t *buf, EE_address_t EE_address, uint16_t size) 
{
    if((NULL == buf) || (0 == size))
    {
        return(HAL_RES_NOK);
    }
    _memcpy_p2d16(buf, EE_address, size);
    
    return(HAL_RES_OK);

}

extern hal_result_t hal_eeprom_write(uint8_t *buf, EE_address_t EE_address, int16_t size) 
{

	int16_t *aux_ptr;
	int16_t i;
	_prog_addressT EE_addr = EE_address;

    if((NULL == buf) || (0 == size))
    {
        return(HAL_RES_NOK);
    }


	for(i=0 ; i < size; i++)
	{
		_erase_eedata(EE_addr++, _EE_WORD);
	  	_wait_eedata();
	}
	
	aux_ptr = (int16_t*) buf;
	for(i=0 ; i < size; i++)
	{
	  _write_eedata_word(EE_address, *aux_ptr );
	  EE_address+=2;
	  aux_ptr++;
	  _wait_eedata();
	}

    return(HAL_RES_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



