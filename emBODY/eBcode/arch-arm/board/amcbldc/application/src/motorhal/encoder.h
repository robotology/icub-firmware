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
#define MOTORHAL_changes
#else
#include <stdarg.h>
#include "stm32g4xx.h"
#include "stm32g4xx_hal_tim.h"
#endif

/* Exported typedefs -------------------------------------------------------------------------------------------------*/

#if defined(MOTORHAL_changes) 
        
typedef struct
{
    uint16_t    mode;
    uint16_t    filter;
    uint16_t    idxpos;
    int16_t     resolution;
    uint16_t    has_hall_sens;
} encoder_ConfTypeDef;

#else

typedef struct
{
    uint16_t    mode;
    uint16_t    filter;
    uint16_t    idxpos;
    uint16_t    nsteps;
} encoderConfTypeDef;

#endif




/* Exported macro ----------------------------------------------------------------------------------------------------*/
/* Exported variables ------------------------------------------------------------------------------------------------*/
/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern HAL_StatusTypeDef encoderInit(void);
extern uint32_t encoderGetCounter(void);
extern uint16_t encoderGetElectricalAngle(void);


#if defined(MOTORHAL_changes)

extern HAL_StatusTypeDef encoder_Deinit(void);
extern HAL_StatusTypeDef encoder_Config(uint8_t has_quad_enc, int16_t resolution, uint8_t num_polar_couples, uint8_t has_hall_sens);
extern void encoder_Reset();
extern void encoder_Force(uint16_t value);
extern void encoder_Calibrate(uint16_t offset);
extern uint16_t encoder_GetElectricalOffset();
extern uint16_t encoder_GetUncalibrated(void);

#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */
#endif /* __ENCODER_H */
/* END OF FILE ********************************************************************************************************/
