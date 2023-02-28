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

typedef struct
{
    uint16_t    mode;
    uint16_t    filter;
    uint16_t    idxpos;
#if defined(MOTORHAL_changes) 
    int16_t     resolution;
    uint16_t    has_hall_sens;
#else   
    uint16_t    nsteps;
#endif
} encoderConfTypeDef;


/* Exported macro ----------------------------------------------------------------------------------------------------*/

/* Number of step per revolution */
#define ENCODER_COUNT_PER_REVOLUTION    (4000)


/* Exported variables ------------------------------------------------------------------------------------------------*/
/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern HAL_StatusTypeDef encoderInit(void);
extern uint32_t encoderGetCounter(void);
extern uint32_t encoderGetElectricalAngle(void);
extern int32_t encoderGetSpeed(void);

#if defined(MOTORHAL_changes)

extern HAL_StatusTypeDef encoderDeinit(void);
extern HAL_StatusTypeDef encoderConfig(uint8_t has_quad_enc, int16_t resolution, uint8_t num_polar_couples, uint8_t has_hall_sens);
extern void encoderReset();
extern void encoderForce(uint32_t value);
extern void encoderCalibrate(uint32_t offset);
extern uint32_t encoderGetElectricalOffset();
extern uint32_t encoderGetUncalibrated(void);

#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */
#endif /* __ENCODER_H */
/* END OF FILE ********************************************************************************************************/
