/*******************************************************************************************************************//**
 * @file    adc.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 July, 14
 * @brief   A/D converter management for CM7 core
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __ADC_H
#define __ADC_H

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

extern HAL_StatusTypeDef AdcInit(void);

extern void AdcTest(void);
extern uint16_t AdcGetVcore(void);
extern uint16_t AdcGetVcc(void);
extern uint16_t AdcGetVaux(void);
extern uint16_t AdcGetVin(void);
extern int16_t AdcGetCin(void);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __ADC_H */
/* END OF FILE ********************************************************************************************************/
