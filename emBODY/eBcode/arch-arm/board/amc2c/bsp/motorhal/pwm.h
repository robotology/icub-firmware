/*******************************************************************************************************************//**
 * @file    pwm.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 17
 * @brief   PWM management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __PWM_H
#define __PWM_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------------------------------------------------------------------*/

#if defined(USE_STM32HAL)
#include "stm32hal.h"     
#define MOTORHAL_changes
#else  
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>        
#include "main.h"
#include "stm32h7xx.h"
#endif

/* Exported macro ----------------------------------------------------------------------------------------------------*/

#define PWM_PHASE_U             (MOT_EN1_Pin)
#define PWM_PHASE_V             (MOT_EN2_Pin)
#define PWM_PHASE_W             (MOT_EN3_Pin)
#define PWM_PHASE_ALL           (MOT_EN1_Pin|MOT_EN2_Pin|MOT_EN3_Pin)
#define PWM_PHASE_NONE          (0)


/* Exported inline functions -----------------------------------------------------------------------------------------*/
/* Exported variables ------------------------------------------------------------------------------------------------*/
/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern void PwmPhaseSet(uint16_t u, uint16_t v, uint16_t w);
extern void PwmPhaseEnable(uint16_t mask);
extern void PwmSleep(FunctionalState enable);
extern void PwmReset(FunctionalState enable);
extern HAL_StatusTypeDef PwmFaultStatus(void);
extern HAL_StatusTypeDef PwmResetFault(void);

extern HAL_StatusTypeDef PwmInit(void);
extern void PwmDeInit(void);

extern void PwmTest(void);

#if defined(MOTORHAL_changes)


struct pwm_Configuration
{
    enum class START { immediate };
    
    START start { START::immediate };
        
    constexpr pwm_Configuration() = default;
    constexpr bool isvalid() const { return true; }
};

extern void pwm_Init(const pwm_Configuration &config);
extern void pwm_DeInit();

extern void pwm_Set(uint16_t u, uint16_t v, uint16_t w);


#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __PWM_H */
/* END OF FILE ********************************************************************************************************/
