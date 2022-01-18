/*******************************************************************************************************************//**
 * @file    can.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 July, 20
 * @brief   FDCAN management for CM7 core
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __CAN_H
#define __CAN_H

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

extern HAL_StatusTypeDef CanInit(void);
extern void CanTest(void);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __CAN_H */
/* END OF FILE ********************************************************************************************************/
