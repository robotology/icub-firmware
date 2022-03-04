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
    uint16_t    hall_offset;
    uint16_t    sector_offset;
    uint8_t     num_polar_couples;
    uint8_t     swapBC;
} pwmConfTypeDef;

// we keep int32_t even if the adc gets only int16_t values
typedef struct
{
    int32_t     u;
    int32_t     v;
    int32_t     w;    
} pwmCurrents_t;


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

extern HAL_StatusTypeDef pwmSetValue(int32_t pwm); /* DEPRECATED: it returns HAL_ERROR */

extern void pwmTest(void);
//extern void setADC_cb(void (*fn_cb)(void *, int16_t[3], void*, void*), void *owner, void* rtu, void* rty);

// a function of this type is used by the ADC IRQ Handler to report the measured currents to higher levels
// we need two param to to the job:
// - owner: is a pointer to the high level calling module. we need it so that inside the callback
//   we just reinterpret_cast<> it to the calling class 
// - currents: is a pointer to the three currents. the memory pointed by currents belongs to owner
//   so that the caller of the callback will just copy the memory of the ADC (typically DMA memory)
//   straigth away to destination to avoid corruptions. 
typedef void (*pwm_fp_adc_callback_t) (void *owner, const pwmCurrents_t * const currents);


typedef struct 
{
    pwm_fp_adc_callback_t   callback;
    void*                   owner; 
} pwm_ADC_callback_t;

// this funtion sets the callback of the ADC. we need to specify the callback itself but also 
// the owner of the callback (useful if it is a static method of a class) and the destination of
// the currents.
// and in here is explained the mechanism of the ADC:
// the ADC uses DMA to gets its values. the DMA peridically calls function pwmSetCurrents_cb() 
// at half transfer and at end transfer. this function performs calibration if required, and
// calls a user-defined function which we can set with set_ADC_callback() so that the values
// of currents can be used by other software modules.

extern void set_ADC_callback(pwm_ADC_callback_t *cbk);

// this function must be used only inside the file analog.c ..... 
// it is called at the completion of dma transfer (both half and full). then this function may call a user defined
// callback spefified by set_ADC_callback().
// to do: i may remove it from the public API to avoid confusion 
extern void pwmSetCurrents_cb(int16_t i1, int16_t i2, int16_t i3);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */
#endif /* __PWM_H */
/* END OF FILE ********************************************************************************************************/
