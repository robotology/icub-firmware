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
#define MOTORHAL_changes
#else 
#include <stdarg.h>        
#include "stm32g4xx.h"
#include "adc.h"
#endif
        
/* Exported macro ----------------------------------------------------------------------------------------------------*/
#if defined(MOTORHAL_changes)
#define ANALOG_AVG_FILTER_SHIFT     (7)
#define ANALOG_AVG_FILTER_LENGTH    (1U<<ANALOG_AVG_FILTER_SHIFT)        
#else
/* Used by the Moving Average Filter */
#define ANALOG_AVG_FILTER_SHIFT     (10)
#define ANALOG_AVG_FILTER_LENGTH    (1U<<ANALOG_AVG_FILTER_SHIFT)
#endif


/* Exported typedefs -------------------------------------------------------------------------------------------------*/

/* Configuration structure:
 * FLASH manager must include a member called "analog" in MainConf structure
 */
typedef struct
{
    uint16_t    cinOffs;
    uint16_t    cphOffs[3];
} analogConfTypeDef;

/* Moving average filter structure */
typedef struct
{
    volatile int32_t     avg;
    volatile uint32_t    idx;
    volatile int32_t     buf[ANALOG_AVG_FILTER_LENGTH];
} analogAvgFilterTypeDef;


/* Exported variables ------------------------------------------------------------------------------------------------*/


/* Exported functions prototypes -------------------------------------------------------------------------------------*/

/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  
 */
__STATIC_INLINE int32_t analogMovingAverageFromISR(analogAvgFilterTypeDef *filter, int32_t sample)
{
    filter->avg -= filter->buf[filter->idx];
    filter->avg += (filter->buf[filter->idx] = sample);
    if (++(filter->idx) >= ANALOG_AVG_FILTER_LENGTH) filter->idx = 0;
    return filter->avg;
}

extern int32_t analogMovingAverage(analogAvgFilterTypeDef *filter, int32_t sample);
extern int32_t analogConvertCurrent(int32_t raw);
extern HAL_StatusTypeDef analogInit(void);
extern uint32_t analogVcc(void);
extern uint32_t analogVph1(void);
extern uint32_t analogVph2(void);
extern uint32_t analogVph3(void);
extern uint32_t analogVin(void);
extern int32_t  analogCin(void);
extern uint32_t  temperature(void);
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


#if defined(MOTORHAL_changes)

extern HAL_StatusTypeDef analog_Deinit(void);
extern int32_t analog_RawCph1(void);
extern int32_t analog_RawCph2(void);
extern int32_t analog_RawCph3(void);
extern void analog_MovingAverage(int32_t i1, int32_t i2, int32_t i3);

#endif // #if defined(MOTORHAL_changes)

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */
#endif /* __ANALOG_H */
/* END OF FILE ********************************************************************************************************/
