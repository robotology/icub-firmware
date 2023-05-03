/*******************************************************************************************************************//**
 * @file    adcm.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 27
 * @brief   A/D converter management for CM4 core
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __ADCM_H
#define __ADCM_H


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


/* Exported typedefs -------------------------------------------------------------------------------------------------*/

typedef enum
{
    ADCMOT_PHASE_U,
    ADCMOT_PHASE_V,
    ADCMOT_PHASE_W
} AdcMotPhase_t;


/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern HAL_StatusTypeDef AdcMotInit(void);
extern void AdcMotDeInit(void);
extern void AdcMotGetCurrents(int16_t *pU, int16_t *pV, int16_t *pW);
extern void AdcMotGetVoltages(uint16_t *pU, uint16_t *pV, uint16_t *pW);
extern uint32_t AdcMotGetOffset(AdcMotPhase_t Phase);
extern void AdcMotSetOffset(AdcMotPhase_t Phase, uint16_t Offset);


#if defined(MOTORHAL_changes)

    
// -- required types

// cannot include it unless we remove the extern c clause
//#include "embot_core.h"

// we keep int32_t even if the adc gets only int16_t values
struct adcm_Currents
{
    int32_t u {0};
    int32_t v {0};
    int32_t w {0};
    
    constexpr adcm_Currents() = default;
    constexpr adcm_Currents(int32_t a, int32_t b, int32_t c) : u(a), v(b), w(c) {}     
};


struct adcm_ADC_callback
{
    using Action = void (*)(void *owner, const adcm_Currents * const currents);
    
    Action action {nullptr};
    void* owner {nullptr}; 
    
    constexpr adcm_ADC_callback() = default;
    constexpr adcm_ADC_callback(Action a, void *o) : action(a), owner(o) {}
        
    void load(Action a, void *o) { action = a; owner = o; } 
    void load(const adcm_ADC_callback &obj) { action = obj.action; owner = obj.owner; }    
    bool isvalid() const { return (nullptr != action); }
    void execute(const adcm_Currents * const curs) const
    { 
        if(isvalid()) { action(owner, curs); } 
    }            
};


struct adcm_Configuration
{
    enum class ACQUISITION { immediate };
    enum class CALIBRATION { none, current };
    
    ACQUISITION acquisition { ACQUISITION::immediate };
    CALIBRATION calibration { CALIBRATION::none };
    
    adcm_ADC_callback onadc {};
    
    constexpr adcm_Configuration() = default;
    constexpr adcm_Configuration(CALIBRATION &c, const adcm_ADC_callback& o) : calibration(c), onadc(o) {}
    constexpr bool isvalid() const { return true; }
};

// -- public interfaces to be used in embot::hw::motor

// _Init() starts ADC acquisition straight away (ACQUISITION::immediate). 
// w/ CALIBRATION::current it also makes sure that it sets the adc values of the three currents equal zero when pwm is zero

bool adcm_Init(const adcm_Configuration &config);
bool adcm_Calibrate(uint64_t usectimeout);
void adcm_set_ADC_callback(const adcm_ADC_callback &cbk);
bool adcm_DeInit();



#endif // #if defined(MOTORHAL_changes)

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __ADCM_H */
/* END OF FILE ********************************************************************************************************/
