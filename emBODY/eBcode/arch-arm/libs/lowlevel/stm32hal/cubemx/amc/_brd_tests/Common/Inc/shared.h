/*******************************************************************************************************************//**
 * @file    shared.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 July, 14
 * @brief   Shared variables
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __SHARED_H
#define __SHARED_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ***********************************************************************************************************/
#include <stdbool.h>
#include "main.h"
#include "stm32h7xx.h"
#include "FreeRTOS.h"


/* Public Functions ***************************************************************************************************/

#if defined CORE_CM7
extern HAL_StatusTypeDef SharedTest(void);
#elif defined CORE_CM4
extern bool SharedRxReady(void);
extern bool SharedTxReady(void);
#endif

extern HAL_StatusTypeDef SharedInit(void);
extern HAL_StatusTypeDef SharedTxChar(uint8_t Value, TickType_t Timeout);
extern HAL_StatusTypeDef SharedRxChar(uint8_t *pValue, TickType_t Timeout);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __SHARED_H */
/* END OF FILE ********************************************************************************************************/
