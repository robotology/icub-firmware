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
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "main.h"
#include "stm32h7xx.h"

/* Exported macro ----------------------------------------------------------------------------------------------------*/


/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern HAL_StatusTypeDef HallInit(void);
extern void HallDeInit(void);
extern void HallTest(void);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __HALL_H */
/* END OF FILE ********************************************************************************************************/
