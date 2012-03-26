
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _CMSIS_STM32F1_H_
#define _CMSIS_STM32F1_H_


/** @file       cmsis_stm32f1.h
	@brief      This header file implements public interface to the cmsis for the stm32 from stmicroelectronics 
	@author     marco.accame@iit.it
	@date       07/29/2010
**/

/* @defgroup cmsis_stm32f1 
    
    @todo   maybe to place in here some decription of the api used for tha hal ...
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stm32f10x.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern void cmsis_stm32f1_hid_set_system_core_clock(void);



/* @}            
    end of group cmsis_stm32f1  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

