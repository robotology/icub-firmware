/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

#ifndef _DEBUG_UTIL_H_
#define _DEBUG_UTIL_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       debug_util.h
    @brief      This header file provides functions and macro to help you to debug 6SG application.
    @author     valentina.gaggero@iit.it
    @date       10/03/2011
**/

/** @defgroup 

 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include <stdint.h>
#include "hal.h"
#include <libpic30.h>


// - public #define  --------------------------------------------------------------------------------------------------
//this macro can be use to debug application with oscilloscope to measure apllication timing

#define ledRED          (LATBbits.LATB9)
#define ledRED_on       LATBbits.LATB9 = 1;
#define ledRED_off      LATBbits.LATB9 = 0;
#define ledRED_toogle   LATBbits.LATB9 = ~LATBbits.LATB9;

#define ledYELLOW          (LATAbits.LATA3)
#define ledYELLOW_on       LATAbits.LATA3 = 1;
#define ledYELLOW_off      LATAbits.LATA3 = 0;
#define ledYELLOW_toogle   LATAbits.LATA3 =  ~LATAbits.LATA3;

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



/** @}            
    end of group arm_hal_base  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



