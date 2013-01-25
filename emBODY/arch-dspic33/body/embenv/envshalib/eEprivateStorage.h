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
#ifndef _EEPRIVATESTORAGE_H_
#define _EEPRIVATESTORAGE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEprivateStorage.h
    @brief      This header file implements public interface to ....
    @author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEsharedInfo.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section




// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef eEbool_t (*ee_privatestorage_correctness_fn_t) (void*);

typedef struct
{
    //ee_privatestorage_correctness_fn_t  correctness_fn;     /**< if NULL, internal correctness is used with CRC16 appended after data, else this function tells */
    uint16_t    size;
    void*       defval;
} ee_privatestorage_cfg_t;


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern eEresult_t ee_privatestorage_init(ee_privatestorage_cfg_t* cfg, void** pdata)
    @brief      init the private storage manager on the dspic33.
    @param      cfg             pointer to the configuration
    @param      pdata           return pointer to private data stores internally
    @return     ee_res_OK or ee_res_NOK_generic upon failure
    @warning    hal_eeprom and hal-crc must be properly initted beforehand
 **/
extern eEresult_t ee_privatestorage_init(ee_privatestorage_cfg_t* cfg, void** pdata);


/** @fn         extern eEresult_t ee_privatestorage_get(void** pdata, eEbool_t* itiscorrect)
    @brief      retrieve data using the private storage manager.
    @param      pdata           pointer to ram internally stored
    @return     ee_res_OK or ee_res_NOK_generic upon failure
    @warning    hal_eeprom and hal-crc must be properly initted beforehand
 **/
extern eEresult_t ee_privatestorage_get(void** pdata, eEbool_t* itiscorrect);


/** @fn         extern eEresult_t ee_privatestorage_set(const void* data)
    @brief      sets the data in the private storage manager.
    @param      data            pointer to ram we want to store
    @return     ee_res_OK or ee_res_NOK_generic upon failure
    @warning    hal_eeprom and hal-crc must be properly initted beforehand
 **/
extern eEresult_t ee_privatestorage_set(const void* data);


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



