/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HAL_CANTRANSCEIVER_H_
#define _HAL_CANTRANSCEIVER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_cantransceiver.h
    @brief      This header file implements public interface to the hal cantransceiver module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

/** @defgroup doxy_group_hal_cantransceiver HAL CANTRANSCEIVER

    The HAL CANTRANSCEIVER ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"
#include "hal_can.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_cantransceiver_t 
    @brief      hal_cantransceiver_t contains the usable can trasceiver: one per can.
 **/
typedef enum  
{ 
    hal_cantransceiver1 = 0,           /**< for hal_can1        */
    hal_cantransceiver2 = 1            /**< fro hal_can2        */
} hal_cantransceiver_t; 

enum { hal_cantransceivers_number = 2 };


/** @typedef    typedef struct hal_cantransceiver_cfg_t;
    @brief      contains configuration data of cantransceiver.
 **/
typedef struct
{
    uint8_t dummy;          /**< dummy...     */
} hal_cantransceiver_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_cantransceiver_cfg_t hal_cantransceiver_cfg_default;   // = { .dummy = 0};


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_cantransceiver_init(hal_cantransceiver_t id, const hal_cantransceiver_cfg_t *cfg)
    @brief  	This function initializes the cantransceiver attached to the MPU
    @param      port            The port attached to the transceiver
    @param  	cfg 	        The configuration of the cantransceiver. It can be NULL.
    @return 	hal_res_NOK_generic in case the cantransceiver cannot be configured, else hal_res_OK
    @warning    a given transceiver can be initted only once. The second call of hal_cantransceiver_init(id, cfg) will do nothing.
                The return value will be hal_res_OK if the parameter cfg is the same as one used the first time,
                otherwise it will be hal_res_NOK_generic to express the fact that the device is not initted as wanted.
  */
extern hal_result_t hal_cantransceiver_init(hal_cantransceiver_t id, const hal_cantransceiver_cfg_t *cfg);

extern hal_result_t hal_cantransceiver_enable(hal_cantransceiver_t id);

extern hal_result_t hal_cantransceiver_disable(hal_cantransceiver_t id);


/** @fn			extern hal_bool_t hal_cantransceiver_initted_is(void)
    @brief  	This function tells if the cantransceiver is already initialised.
    @return 	hal_true or hal_false.
  */
extern hal_bool_t hal_cantransceiver_initted_is(hal_cantransceiver_t id);

/** @}            
    end of group doxy_group_hal_cantransceiver  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



