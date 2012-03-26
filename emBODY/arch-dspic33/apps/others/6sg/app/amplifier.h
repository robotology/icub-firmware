/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Andrea Merello
 * email:   valentina.gaggero@iit.it, andrea.merello@iit.it
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

#ifndef _AMPLIFIER_H_
#define _AMPLIFIER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       amplifier.h
    @brief      This file provides interface to use adc AD8555. 
    @author     valentina.gaggero@iit.it, andrea.merello@iit.it
    @date       07/14/2011
**/

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "6sg_config.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/**
    @fn         extern hal_result_t ampl_init(SIXsg_config_data_t *cfg_ptr);
    @brief      initialises amplifier
    @param      cfg_ptr         teh cfg
    @return     hal_res_NOK_nullpointer if cfg_ptr is NULL.(in this case amplifier may crash.)
                hal_res_OK otherwise
**/
extern hal_result_t ampl_init(SIXsg_config_data_t *cfg_ptr);



/**
    @fn         extern void ampl_set_gain_and_offset(void);
    @brief      Configures gain and offset values containg in memory pointed by SIXsg_config_data_t *cfg_ptr.
                (it was set by ampl_init function) 
 **/
extern void ampl_set_gain_and_offset(void);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


