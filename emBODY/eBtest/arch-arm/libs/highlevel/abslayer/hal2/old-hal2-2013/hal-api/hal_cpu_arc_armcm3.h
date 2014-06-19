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

#ifndef _HAL_CPU_ARC_ARMCM3_H_
#define _HAL_CPU_ARC_ARMCM3_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_cpu_arc_armcm3.h
    @brief      This header file keeps public interface to the hal parts which are specific of cpu architecture: arm cm3.
    @author     marco.accame@iit.it
    @date       02/27/2013
**/

/** @defgroup doxy_group_hal_cpu_arc_armcm3 HAL CPU ARM CM3 ARCHITECTURE

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/



// - public #define  --------------------------------------------------------------------------------------------------



// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"

#include "hal_base.h"
#include "hal_sys.h"
#include "hal_cpu.h"



// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_cpu_arc_armcm3_irqn_t
    @brief      contains the irq numbers for the stm32f1x family of processors.
 **/
typedef enum 
{
    hal_cpu_arc_armcm3_NonMaskableInt_IRQn         = -14,    /**< Non Maskable Interrupt                             */
    hal_cpu_arc_armcm3_MemoryManagement_IRQn       = -12,    /**<  Cortex-M3 Memory Management Interrupt              */
    hal_cpu_arc_armcm3_BusFault_IRQn               = -11,    /**<  Cortex-M3 Bus Fault Interrupt                      */
    hal_cpu_arc_armcm3_UsageFault_IRQn             = -10,    /**<  Cortex-M3 Usage Fault Interrupt                    */
    hal_cpu_arc_armcm3_SVCall_IRQn                 = -5,     /**<  Cortex-M3 SV Call Interrupt                       */
    hal_cpu_arc_armcm3_DebugMonitor_IRQn           = -4,     /**<  Cortex-M3 Debug Monitor Interrupt                 */
    hal_cpu_arc_armcm3_PendSV_IRQn                 = -2,     /**<  Cortex-M3 Pend SV Interrupt                       */
    hal_cpu_arc_armcm3_SysTick_IRQn                = -1      /**<  Cortex-M3 System Tick Interrupt                   */
} hal_cpu_arc_armcm3_irqn_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group doxy_group_hal_cpu_arc_armcm3  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



