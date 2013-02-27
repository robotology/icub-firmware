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
#ifndef _HAL_CPU_HID_H_
#define _HAL_CPU_HID_H_


/* @file       hal_cpu_hid.h
    @brief      This header file implements hidden interface to hal.
    @author     marco.accame@iit.it
    @date       02/27/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_cpu.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------
// empty-section

// - definition of hidden structs -------------------------------------------------------------------------------------

typedef enum 
{
    hal_cpu_refclock_internal               = 0,
    hal_cpu_refclock_external_xtl           = 1,
    hal_cpu_refclock_external_osc           = 2,
    hal_cpu_refclock_pll_on_internal        = 3,
    hal_cpu_refclock_pll_on_external_xtl    = 4,   
    hal_cpu_refclock_pll_on_external_osc    = 5   
} hal_cpu_hid_refclock_t;


#if     defined(HAL_USE_CPU_FAM_STM32F1)
typedef struct 
{   // pll2clock = pll2mul*(hse/prediv2), speedcpu = pllmul*(pll2clock/prediv1) 
    uint32_t                        prediv2;    
    uint32_t                        pll2mul;
    uint32_t                        prediv1;
    uint32_t                        pllmul;
} hal_cpu_hid_pll_cfg_t;
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
typedef struct 
{   // pllfreq = n*(source/m), speedcpu = pllfreq/p, 48mhz = pllfreq/q 
    uint32_t                        m;    
    uint32_t                        n;
    uint32_t                        p;
    uint32_t                        q;
} hal_cpu_hid_pll_cfg_t;
#endif


typedef struct 
{
    uint32_t                        cpu;
    uint32_t                        fastbus;
    uint32_t                        slowbus;
} hal_cpu_hid_speeds_t;


typedef struct 
{
    hal_bool_t                      keepinternalclockatstartup; // if hal_true: it runs at intclockspeed at startup and changes to refclock inside hal_cpu_systeminit()
    hal_cpu_hid_refclock_t          refclock;                   // the clock or mixture of clock and pll to use          
    uint32_t                        intclockspeed;              // the speed of the internal clock   
    uint32_t                        extclockspeed;              // teh speed of teh external clock
    hal_cpu_hid_pll_cfg_t           pllcfg;                     // the way the pll, if used, is configured
} hal_cpu_hid_clock_cfg_t;


typedef struct
{
    hal_cpu_hid_speeds_t            speeds;             // the speeds for cpu, fast bus and slow bus that the system has
    hal_cpu_hid_clock_cfg_t         clockcfg;           // the configuration of clock etc., which are used to achieve the above speeds
} hal_cpu_hid_brdcfg_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_cpu_hid_getsize(const hal_cfg_t *cfg);
extern hal_result_t hal_cpu_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);


// - definition of extern hidden inline functions ---------------------------------------------------------------------
// empty-section

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




