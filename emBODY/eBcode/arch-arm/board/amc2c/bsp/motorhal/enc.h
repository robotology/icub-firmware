/*******************************************************************************************************************//**
 * @file    enc.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 29
 * @brief   Incremental Encoder management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __ENC_H
#define __ENC_H

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

extern void EncCaptureIndex(void);
extern bool EncIndexDetected(void);
extern uint32_t EncGetCounter(void);
extern uint32_t EncGetCompare(void);
extern int32_t EncGetAbsolutePosition(void);
extern int32_t EncGetRotorPosition(void);
extern int32_t EncGetNuberOfTurns(void);

extern HAL_StatusTypeDef EncInit(void);
extern void EncDeInit(void);
extern void EncTest(void);

#if defined(MOTORHAL_changes) 


// -- public interfaces to be used in embot::hw::motor

struct enc_Configuration
{
    enum class ACQUISITION { deferred };
    
    ACQUISITION acquisition { ACQUISITION::deferred };
        
    constexpr enc_Configuration() = default;
    constexpr bool isvalid() const { return true; }
};

struct enc_Mode
{
    int16_t resolution {0};
    uint8_t num_polar_couples {0};
    bool calibrate_with_hall {false};
    bool use_index {false};
    
    constexpr enc_Mode() = default;
    constexpr enc_Mode(int16_t r, uint8_t n, bool c = false, bool i = false) 
        : resolution(r), num_polar_couples(n), calibrate_with_hall(c), use_index(i) {}
    constexpr bool isvalid() const { 
        bool notok = (0 == resolution) || (0 == num_polar_couples);
        return !notok;
    }
};

extern bool enc_Init(const enc_Configuration &config);

extern bool enc_DeInit();

extern bool enc_Start(const enc_Mode& mode);

extern bool enc_IsStarted();

int32_t enc_GetValue();

#endif // #if defined(MOTORHAL_changes)


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __ENC_H */
/* END OF FILE ********************************************************************************************************/
