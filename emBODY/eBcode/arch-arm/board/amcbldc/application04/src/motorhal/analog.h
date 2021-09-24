/*******************************************************************************************************************//**
 * @file    analog.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 January, 20
 * @brief   Analog signals management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __ANALOG_H
#define __ANALOG_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else 
#include <stdarg.h>        
#include "stm32g4xx.h"
#include "adc.h"
#endif
        
/* Exported macro ----------------------------------------------------------------------------------------------------*/


/* Exported typedefs -------------------------------------------------------------------------------------------------*/

/* Configuration structure:
 * FLASH manager must include a member called "analog" in MainConf structure
 */
typedef struct
{
    uint16_t    cinOffs;
    uint16_t    cphOffs[3];
} analogConfTypeDef;

/* Exported variables ------------------------------------------------------------------------------------------------*/


/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern int32_t analogConvertCurrent(int32_t raw);
extern HAL_StatusTypeDef analogInit(void);
extern uint32_t analogVcc(void);
extern uint32_t analogVph1(void);
extern uint32_t analogVph2(void);
extern uint32_t analogVph3(void);
extern uint32_t analogVin(void);
extern int32_t  analogCin(void);
extern int32_t  analogCph1(void);
extern int32_t  analogCph2(void);
extern int32_t  analogCph3(void);
extern int32_t  analogGetOffsetIin(void);
extern int32_t  analogSetOffsetIin(int32_t offs);
extern int32_t  analogGetOffsetIph1(void);
extern int32_t  analogSetOffsetIph1(int32_t offs);
extern int32_t  analogGetOffsetIph2(void);
extern int32_t  analogSetOffsetIph2(int32_t offs);
extern int32_t  analogGetOffsetIph3(void);
extern int32_t  analogSetOffsetIph3(int32_t offs);

extern void analogTest(void);

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */
#endif /* __ANALOG_H */
/* END OF FILE ********************************************************************************************************/
