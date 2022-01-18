/*******************************************************************************************************************//**
 * @file    enc.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 29
 * @brief   Incremental Encoder management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __ENC_H
#define __ENC_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "main.h"
#include "stm32h7xx.h"

/* Exported macro ----------------------------------------------------------------------------------------------------*/

/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern void EncCaptureIndex(void);
extern bool EncIndexDetected(void);
extern uint32_t EncGetCounter(void);
extern uint32_t EncGetCompare(void);
extern int32_t EncGetAbsolutePosition(void);
extern int32_t EncGetRotorPosition(void);
extern int32_t EncGetNuberOfTurns(void);

extern HAL_StatusTypeDef EncInit(void);
extern void EncDeInit(void);
extern void EncTest(void);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __ENC_H */
/* END OF FILE ********************************************************************************************************/
