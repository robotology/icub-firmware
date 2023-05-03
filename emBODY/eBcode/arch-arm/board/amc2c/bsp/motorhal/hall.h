/*******************************************************************************************************************//**
 * @file    hall.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 29
 * @brief   Hall sensors management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __HALL_H
#define __HALL_H

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


/* Exported functions prototypes -------------------------------------------------------------------------------------*/


extern HAL_StatusTypeDef HallInit(void);
extern void HallDeInit(void);
extern void HallTest(void);

#if defined(MOTORHAL_changes)

struct hall_Configuration
{
    enum class ACQUISITION { deferred };
    
    ACQUISITION acquisition { ACQUISITION::deferred };
        
    constexpr hall_Configuration() = default;
    constexpr bool isvalid() const { return true; }
};


struct hall_Mode
{
    enum class SWAP { none, BC };
    SWAP swap {SWAP::none};
    uint16_t offset {0};
    
    constexpr hall_Mode() = default;
    constexpr hall_Mode(SWAP s, uint16_t o) : swap(s), offset(o) {}
    constexpr bool isvalid() const { 
        bool notok = false;
        return !notok;
    }
};

extern bool hall_Init(const hall_Configuration &config);
extern bool hall_DeInit();
extern bool hall_Start(const hall_Mode &mode);
extern uint8_t hall_GetStatus(void);

#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __HALL_H */
/* END OF FILE ********************************************************************************************************/
