/*******************************************************************************************************************//**
 * @file    encoder.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 March, 9
 * @brief   Incremental Encoder management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __ENCODER_H
#define __ENCODER_H

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
#include "stm32g4xx_hal_tim.h"
#endif

/* Exported typedefs -------------------------------------------------------------------------------------------------*/

typedef struct
{
    uint16_t    mode;
    uint16_t    filter;
    uint16_t    idxpos;
    uint16_t    nsteps;
} encoderConfTypeDef;


/* Exported macro ----------------------------------------------------------------------------------------------------*/
/* Exported variables ------------------------------------------------------------------------------------------------*/
/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern HAL_StatusTypeDef encoderInit(void);
extern uint32_t encoderGetCounter(void);
extern uint16_t encoderGetElectricalAngle(void);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */
#endif /* __ENCODER_H */
/* END OF FILE ********************************************************************************************************/
