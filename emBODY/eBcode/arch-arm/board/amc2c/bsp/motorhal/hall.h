/*******************************************************************************************************************//**
 * @file    hall.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 29
 * @brief   Hall sensors management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __HALL_H
#define __HALL_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------------------------------------------------------------------*/

#if defined(USE_STM32HAL)
#include "stm32hal.h"     
#define MOTORHAL_changes
#else 
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>        
#include "main.h"
#include "stm32h7xx.h"
#endif

/* Exported macro ----------------------------------------------------------------------------------------------------*/


/* Exported functions prototypes -------------------------------------------------------------------------------------*/


extern HAL_StatusTypeDef HallInit(void);
extern void HallDeInit(void);
extern void HallTest(void);

#if defined(MOTORHAL_changes)

extern uint8_t hall_INPUT();

#endif // #if defined(MOTORHAL_changes) 


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __HALL_H */
/* END OF FILE ********************************************************************************************************/
