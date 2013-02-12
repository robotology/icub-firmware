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

#ifndef _HAL_MUX_H_
#define _HAL_MUX_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_mux.h
    @brief      This header file implements interface to a hal mux 
    @author     marco.accame@iit.it
    @date       02/07/2013
**/

/** @defgroup doxy_group_hal_mux HAL MUX

    The HAL MUX ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_muxport_t 
    @brief      hal_muxport_t contains every possible mux sensor. the way the muxport is read from the peripherals
                of the MPU depends on internals of the muxport file. in case of muxports using spi and mux, then the mapping
                between muxports and peripherals is contained in the brdcfg file.
 **/ 
typedef enum  
{ 
    hal_mux_port1   = 0,
	hal_mux_port2   = 1,
    hal_mux_port3   = 2,         
    hal_mux_port4   = 3
} hal_mux_port_t;

enum { hal_mux_ports_num = 4 };


typedef enum
{
    hal_mux_selA    = 0,
    hal_mux_selB    = 1,
    hal_mux_selC    = 2,
    hal_mux_selNONE = 255
} hal_mux_sel_t;

enum { hal_mux_sels_num = 3 };


/** @typedef    typedef uint32_t hal_mux_position_t
    @brief      contains value of muxport
 **/
typedef uint32_t hal_mux_position_t;



/** @typedef    typedef struct hal_mux_cfg_t;
    @brief      contains configuration data of mux.
 **/
typedef struct
{
    uint8_t     nothing;        /**< nothing  */
} hal_mux_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_mux_cfg_t hal_mux_cfg_default;   // = { .nothing = 0 };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_mux_init(hal_mux_port_t muxport, const hal_mux_cfg_t *cfg)
    @brief  	This function initializes the mux.
    @param  	muxport 	    the mux port
    @param  	cfg 	        The configuration of the mux. 
    @return 	hal_res_NOK_generic in case the mux is not supported, else hal_res_OK
  */
extern hal_result_t hal_mux_init(hal_mux_port_t muxport, const hal_mux_cfg_t *cfg);


/** @fn			extern hal_result_t hal_mux_enable(hal_mux_port_t muxport, hal_mux_sel_t muxsel)
    @brief  	This function enables the mux port with a given selection.
    @param  	muxport         the mux port
    @param  	muxsel          the mux selection 
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_mux_enable(hal_mux_port_t muxport, hal_mux_sel_t muxsel);


/** @fn			extern hal_result_t hal_mux_disable(hal_mux_port_t muxport)
    @brief  	This function disables the mux. no selection is active.
    @param  	muxport         the mux port
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_mux_disable(hal_mux_port_t muxport);



/** @}            
    end of group doxy_group_hal_mux  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



