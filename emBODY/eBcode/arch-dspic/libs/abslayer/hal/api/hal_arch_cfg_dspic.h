/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

#ifndef _HAL_ARCH_CFG_DSPIC_H_
#define _HAL_ARCH_CFG_DSPIC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_arch_cfg_dspic.h
    @brief      This header file keeps public interface to the cfg of hal parts which are specific of dspic architecture.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/16/2011
**/

/** @defgroup hal_arch_cfg_dspic HAL CFG ARCH DSPIC

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct hal_arch_dspic_cfg_t 
    @brief      hal_arch_dspic_cfg_t contains hal configuration for the parts which are specifics only of DSPIC architecture.
 **/  
typedef struct
{   
    uint32_t        pll_freq;                               /**< in hz: 10*1000*1000 w/ cpu_freq = 80*1000*1000 */
    uint8_t         dspengine_moltipl_unsigned;
    uint8_t         dspengine_enable_fractional_mode;    
} hal_arch_dspic_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

#ifdef HAL_DONT_INLINE
extern uint32_t hal_arch_dspic_convert_halflash_address_to_dspicflash_address(uint32_t addr);
extern uint32_t hal_arch_dspic_convert_ptr_to_halflash_address(void *ptr);
#else
extern inline uint32_t hal_arch_dspic_convert_halflash_address_to_dspicflash_address(uint32_t addr)
{   // in libpic30.h: typedef unsigned long _prog_addressT; thus i use a uint32_t
    //return(addr/2);
    return(addr);
}

extern inline uint32_t hal_arch_dspic_convert_ptr_to_halflash_address(void *ptr)
{   // in libpic30.h: typedef unsigned long _prog_addressT; thus i use a uint32_t
    return(2L*(uint32_t)((uint16_t)ptr));
}
#endif

/** @}            
    end of group hal_arch_cfg_dspic  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



