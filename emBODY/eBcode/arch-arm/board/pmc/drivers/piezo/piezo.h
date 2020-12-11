/*******************************************************************************************************************//**
 * @file    piezo.h
 * @author  G.Zini
 * @version 1.0
 * @date    2020 October, 16
 * @brief   Piezo motors management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion ******************************************************************************/
#ifndef __PIEZO_H
#define __PIEZO_H

#ifdef __cplusplus
  extern "C" {
#endif


/* Debug options ******************************************************************************************************/

#undef DEBUG_SPI
#undef DEBUG_DMA_TIMING


/* Include files ******************************************************************************************************/
#include <stdint.h>

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
#include "hal.h"
#endif


/* Exported typedefs **************************************************************************************************/

/* Motor ordinals */
typedef enum
{
    PIEZO_MOTOR1 = 0,
    PIEZO_MOTOR2,
    PIEZO_MOTOR3
} piezoMotor_t ;

typedef enum
{
    PIEZO_NORMAL,
    PIEZO_BRAKE,
    PIEZO_FREEWHEELING
} piezoMode_t;

typedef struct {
    const uint16_t *phaseTable;
    int phaseTableLen;
} piezoMotorCfg_t;

typedef enum {
	STATE_NOT_INIT,
	STATE_STEADY,
	STATE_NORMAL,
	STATE_RAMPING,
	STATE_OVERCURRENT,
} piezoMotorState_t;

/* Exported macros ****************************************************************************************************/

/* Exported functions *************************************************************************************************/

extern HAL_StatusTypeDef piezoInit(piezoMotorCfg_t *cfgM1, piezoMotorCfg_t *cfgM2, piezoMotorCfg_t *cfgM3);
extern HAL_StatusTypeDef piezoSetStepFrequency(piezoMotor_t motor, int32_t freq);
extern HAL_StatusTypeDef piezoGetStepFrequency(piezoMotor_t motor, int32_t *pFreq);
extern HAL_StatusTypeDef piezoSetMode(piezoMotor_t motor, piezoMode_t mode);
extern HAL_StatusTypeDef piezoGetMode(piezoMotor_t motor, piezoMode_t *mode);
extern HAL_StatusTypeDef piezoGetState(piezoMotor_t motor, piezoMotorState_t *state);
extern HAL_StatusTypeDef piezoOvercurrentClear(piezoMotor_t motor);

#ifdef __cplusplus
  } /* extern "C" */
#endif

#endif /* __PIEZO_H */
/* END OF FILE ********************************************************************************************************/
