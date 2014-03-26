/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

#ifndef _HAL_UNIQUEID_H_
#define _HAL_UNIQUEID_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_uniqueid.h
    @brief      This header file implements public interface to the hal uniqueid module.
    @author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       02/27/2013
**/

/** @defgroup hal_uniqueid HAL uniqueid

    The HAL uniqueid ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
    hal_uniqueid_id64bit        = 0,
    hal_uniqueid_macaddr        = 1
} hal_uniqueid_id_t;

enum { hal_uniqueid_ids_num = 2 };


/** @typedef    typedef uint64_t hal_uniqueid_id64bit_t
    @brief      expresses a ID of 64 bits which is unique, i.e., it is different in every micro-processor-unit.
 **/
typedef uint64_t hal_uniqueid_id64bit_t;


/** @typedef    typedef uint64_t hal_uniqueid64_t
    @brief      expresses a mac address of 48 bits, thus the 2 most significant bytes are not used.
 **/
typedef uint64_t hal_uniqueid_macaddr_t;


/** @typedef    typedef struct hal_uniqueid_cfg_t;
    @brief      contains configuration data of uniqueid peripheral.
 **/
typedef struct
{
    uint8_t                     dummy;                  /**< nothing to configure      */
} hal_uniqueid_cfg_t;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_uniqueid_cfg_t hal_uniqueid_cfg_default; //= { .dummy = 0 }



// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_uniqueid_init(const hal_uniqueid_cfg_t *cfg)
    @brief  	This function initializes the uniqueid peripheral. 
    @param      cfg             The configuration. If NULL it uses its default.
                                The normal uniqueid has a countdown range from 10 msec upto 10 seconds.
                                The window uniqueid has a countdown range from 5 ms upto 50 ms. The callback function has 
                                up to 800 usec to be executed.
    @return 	hal_res_NOK_generic in case the peripheral cannot be configured, hal_res_NOK_unsupported if it is not
                supported, hal_res_OK if successful
  */
extern hal_result_t hal_uniqueid_init(const hal_uniqueid_cfg_t *cfg);


/**
    @fn         extern hal_uniqueid_id64bit_t hal_uniqueid_id64bit_get(void)
    @brief      This function gives back an ID of 64 bits which is unique and can be used for instance for
                deriving a MAC address, etc.
    @return     The ID. If valid, it must be different from hal_NA64
 **/
extern hal_uniqueid_id64bit_t hal_uniqueid_id64bit_get(void);


/**
    @fn         extern hal_uniqueid_macaddr_t hal_uniqueid_macaddr_get(void)
    @brief      This function gives back the mac address.
    @return     The mac address. If valid, it must be different from hal_NA64
 **/
extern hal_uniqueid_macaddr_t hal_uniqueid_macaddr_get(void);


/** @}            
    end of group hal_uniqueid  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



