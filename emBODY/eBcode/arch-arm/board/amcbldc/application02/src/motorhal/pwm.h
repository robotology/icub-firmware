/*******************************************************************************************************************//**
 * @file    pwm.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 January, 19
 * @brief   PWM signals management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __PWM_H
#define __PWM_H

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
#include "analog.h"
#include "main.h"
#include "stm32g4xx.h"
#endif

/* Exported macro ----------------------------------------------------------------------------------------------------*/

#define MAX_PWM		(16383U)

#define PWM_CONF_MODE_HALL      (0x0001UL)
#define PWM_CONF_MODE_ENCODER   (0x0002UL)
#define PWM_CONF_MODE_MASK      (PWM_CONF_MODE_HALL|PWM_CONF_MODE_ENCODER)

#define PWM_PHASE_U             (EN1_Pin)
#define PWM_PHASE_V             (EN2_Pin)
#define PWM_PHASE_W             (EN3_Pin)
#define PWM_PHASE_ALL           (EN1_Pin|EN2_Pin|EN3_Pin)


/* Exported typedefs -------------------------------------------------------------------------------------------------*/

typedef struct
{
    uint32_t    mode;
    uint32_t    poles;
} pwmConfTypeDef;


/* Exported variables ------------------------------------------------------------------------------------------------*/



/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern HAL_StatusTypeDef hallInit(void);
extern int32_t hallGetCounter(void);
extern void hallSetCounter(int32_t count);
extern uint16_t hallGetAngle(void);
extern uint16_t hallGetStatus(void);

extern HAL_StatusTypeDef pwmInit(void);
extern void pwmSleep(FunctionalState enable);
extern void pwmReset(FunctionalState enable);
extern HAL_StatusTypeDef pwmResetFault(void);

extern void pwmSet(uint16_t u, uint16_t v, uint16_t w);
extern void pwmPhaseEnable(uint16_t mask);
extern void pwmPhaseDisable(uint16_t mask);

extern void pwmSetCurrents_cb(int16_t i1, int16_t i2, int16_t i3);
extern HAL_StatusTypeDef pwmSetValue(int32_t pwm); /* DEPRECATED */

extern void pwmTest(void);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */
#endif /* __PWM_H */
/* END OF FILE ********************************************************************************************************/
