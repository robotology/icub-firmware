/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
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
#ifndef _HAL_MPU_SYS_HID_H_
#define _HAL_MPU_SYS_HID_H_


/* @file       hal_mpu_sys_hid.h
    @brief      This header file implements hidden interface to sys
    @author     marco.accame@iit.it
    @date       09/12/2011
 **/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_mpu_stm32xx_include.h"
#include "hal_base.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_sys.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef enum 
{
    hal_sys_refclock_internal               = 0,
    hal_sys_refclock_external_xtl           = 1,
    hal_sys_refclock_external_osc           = 2,
    hal_sys_refclock_pll_on_internal        = 3,
    hal_sys_refclock_pll_on_external_xtl    = 4,   
    hal_sys_refclock_pll_on_external_osc    = 5   
} hal_sys_hid_refclock_t;


#if     defined(USE_STM32F1)
typedef struct 
{   // pll2clock = pll2mul*(hse/prediv2), speedcpu = pllmul*(pll2clock/prediv1) 
    uint32_t                        prediv2;    
    uint32_t                        pll2mul;
    uint32_t                        prediv1;
    uint32_t                        pllmul;
} hal_sys_hid_pll_cfg_t;
#elif   defined(USE_STM32F4)
typedef struct 
{   // pllfreq = n*(source/m), speedcpu = pllfreq/p, 48mhz = pllfreq/q 
    uint32_t                        m;    
    uint32_t                        n;
    uint32_t                        p;
    uint32_t                        q;
} hal_sys_hid_pll_cfg_t;
#endif


typedef struct 
{
    uint32_t                        cpu;
    uint32_t                        fastbus;
    uint32_t                        slowbus;
} hal_sys_hid_target_speeds_t;


typedef struct 
{
    hal_sys_hid_refclock_t          refclock;
    uint32_t                        intclockspeed;    
    uint32_t                        extclockspeed;
    hal_sys_hid_target_speeds_t     targetspeeds;
    hal_sys_hid_pll_cfg_t           pllcfg;
} hal_sys_hid_clock_cfg_t;


typedef struct
{
    hal_bool_t                      keepinternalclockatstartup;  // if hal_true: it runs at intclockspeed at startup and changes to refclock after hal_sys_systeminit()
    hal_sys_hid_clock_cfg_t         clock;
} hal_sys_hid_brdcfg_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_sys_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_sys_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




