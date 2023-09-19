/*******************************************************************************************************************//**
 * @file    adcm.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 27
 * @brief   A/D converter management for CM4 core
 **********************************************************************************************************************/

// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif
 
#include "embot_core.h"
 
#if defined(USE_STM32HAL) 
// API
#include "adcm.h"
#endif

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
/* Includes ***********************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "adcm.h"
#include "main.h"
#include "stm32h7xx_hal_adc.h"
#include "led.h"
#include "console.h"
#include "FreeRTOS.h"
#endif // #if defined(USE_STM32HAL)

/* Private macros *****************************************************************************************************/

/* Couples of ADC channels */
#define NUMBER_OF_ADC_CHANNELS  (3)



/* Extenal variables **************************************************************************************************/

#if defined(MOTORHAL_changes)

#include "embot_hw_motor_bsp_amc2c.h"
#define hadc1 (embot::hw::motor::bsp::amc2c::hADC1)
#define hadc2 (embot::hw::motor::bsp::amc2c::hADC2)

#else
/* Declared in main.h */
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
#endif

#if defined(MOTORHAL_changes)

// --------------------------------------------------------------------------------------------------------------------
// add in here minimal changes
// --------------------------------------------------------------------------------------------------------------------

extern adcm_fp_uint16_uint16_uint16_t adcm_FP_on_acquisition_of_currents = NULL;

#endif // #if defined(MOTORHAL_changes)


/* Private typedefs ***************************************************************************************************/

/* Structure to hold the ADC1/ADC2 DMA data */
typedef struct
{
    uint16_t adc1;
    uint16_t adc2;
} __attribute__((aligned(4),packed)) DualModeAdcData_t;


/* Private variables **************************************************************************************************/

/* DMA buffer for ADC1/ADC2 raw data */
static DualModeAdcData_t AdcMotRawData[2*NUMBER_OF_ADC_CHANNELS];

/* Unpacked data */
static uint16_t adcMotCurrents[NUMBER_OF_ADC_CHANNELS]; // in adc units [0, 64k)
static uint16_t adcMotVoltages[NUMBER_OF_ADC_CHANNELS];

/* Channels assigned to phase current measurement */
static const uint32_t AdcMotOffsetChannel[] =
{
    LL_ADC_CHANNEL_0, 
    LL_ADC_CHANNEL_1, 
    LL_ADC_CHANNEL_3
};

/* Offset to be applied to phase current measurement */
static const uint32_t AdcMotOffsetNumber[] =
{
    LL_ADC_OFFSET_1, 
    LL_ADC_OFFSET_2, 
    LL_ADC_OFFSET_3
};


/* Local functions ****************************************************************************************************/
// TODO: Remove, used only for debug
int32_t AVG0 = 0;
int32_t AVG1 = 0;
int32_t AVG2 = 0;

static void adcMotGetSamples(const DualModeAdcData_t *pBuf)
{
    unsigned i;
#if defined(MOTORHALCONFIG_TESTPOINT_DEBUG_remove) 
#else    
/*DEBUG*/HAL_GPIO_WritePin(TP4_GPIO_Port, TP4_Pin, GPIO_PIN_SET);
#endif    
    for (i = 0; i < NUMBER_OF_ADC_CHANNELS; i++)
    {
        adcMotCurrents[i] = pBuf[i].adc1;
        adcMotVoltages[i] = pBuf[i].adc2;
    }
#if defined(MOTORHALCONFIG_TESTPOINT_DEBUG_remove) 
#else    
/*DEBUG*/HAL_GPIO_WritePin(TP4_GPIO_Port, TP4_Pin, GPIO_PIN_RESET);
#endif
    
    
#if defined(MOTORHAL_changes) 
    if(NULL != adcm_FP_on_acquisition_of_currents)
    {        
        adcm_FP_on_acquisition_of_currents(adcMotCurrents[0], adcMotCurrents[1], adcMotCurrents[2]);        
    }
#endif    
}

/* Callback functions *************************************************************************************************/


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotHalfTransfer_cb(ADC_HandleTypeDef *hadc)
{
    /* Record the first half of the buffer */
    adcMotGetSamples(&(AdcMotRawData[0]));
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotTransferComplete_cb(ADC_HandleTypeDef *hadc)
{
    /* Record the second half of the buffer */
    adcMotGetSamples(&(AdcMotRawData[NUMBER_OF_ADC_CHANNELS]));
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
static void AdcMotTransferError_cb(ADC_HandleTypeDef *hadc)
{
    /* SHOULD NEVER HAPPEN */
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotGetCurrents(int16_t *pU, int16_t *pV, int16_t *pW)
{
    *pU = adcMotCurrents[0];
    *pV = adcMotCurrents[1];
    *pW = adcMotCurrents[2];
}    


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotGetVoltages(uint16_t *pU, uint16_t *pV, uint16_t *pW)
{
    *pU = adcMotVoltages[0];
    *pV = adcMotVoltages[1];
    *pW = adcMotVoltages[2];
}    


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
uint32_t AdcMotGetOffset(AdcMotPhase_t Phase)
{
    return LL_ADC_GetOffsetLevel(ADC1, AdcMotOffsetNumber[Phase]);
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotSetOffset(AdcMotPhase_t Phase, uint16_t Offset)
{
    LL_ADC_SetOffset(ADC1, AdcMotOffsetNumber[Phase], AdcMotOffsetChannel[Phase], Offset);
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
HAL_StatusTypeDef AdcMotInit(void)
{
    /* Calibrate ADC1 and ADC2 */
    if ((HAL_OK == HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED)) &&
        (HAL_OK == HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED)))
    {
        /* Register DMA callback functions */
        if ((HAL_OK == HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcMotTransferComplete_cb)) &&
            (HAL_OK == HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID, AdcMotHalfTransfer_cb)) &&
            (HAL_OK == HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_ERROR_CB_ID, AdcMotTransferError_cb)))
        {
            /* Start DMA operations */
#if defined(MOTORHAL_changes)
            return HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t *)AdcMotRawData, sizeof(AdcMotRawData)/sizeof(DualModeAdcData_t));
#else            
            return HAL_ADCEx_MultiModeStart_DMA(&hadc1, (void *)AdcMotRawData, sizeof(AdcMotRawData)/sizeof(DualModeAdcData_t));
#endif            
        }
    }
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotDeInit(void)
{
    HAL_ADCEx_MultiModeStop_DMA(&hadc1);
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID);
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_ERROR_CB_ID);
}


#if defined(MOTORHAL_changes)

// nothing else is required

#endif // #if defined(MOTORHAL_changes)


/* END OF FILE ********************************************************************************************************/
