/*******************************************************************************************************************//**
 * @file    encoder.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 March, 9
 * @brief   Incremental Encoder management
 **********************************************************************************************************************/
 
// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif
 
#if defined(USE_STM32HAL) 
// API
#include "encoder.h"
#endif

/* Includes ***********************************************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#if !defined(HALCONFIG_DONTUSE_FLASH)
#include "flash.h"
#endif
#else
#include "FreeRTOS.h"
#include "encoder.h"
#include "flash.h"
#include "main.h"
#include "utilities.h"
#include "tim.h"
#include "stm32g4xx_hal_tim.h"
#endif

#if (USE_HAL_TIM_REGISTER_CALLBACKS != 1)
    #error Flag TIM2 in menu "Project Manager -> Advanced Settings -> Register CallBack" of CubeMx must be ENABLED
#endif

/* Private macros *****************************************************************************************************/
/* Private variables **************************************************************************************************/

/* Coversion factor from encoder step value to elctrical angle. It is given by:
 * encoderConvFactor = 65536 * number_of_poles / number_of_encoder_steps
 */
static int16_t encoderConvFactor = 112;   


/* Callbacks **********************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void encoderIndexCallback(TIM_HandleTypeDef *htim)
{
    /**************
     * TO BE DONE *
     **************/
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Start TIM2 as 32 bits incremental encoders
 * @param   void
 * @return  HAL_StatusTypeDef   Operation result
 */
HAL_StatusTypeDef encoderInit(void)
{
    TIM_Encoder_InitTypeDef sConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIMEx_EncoderIndexConfigTypeDef sEncoderIndexConfig = {0};

    if (0 == MainConf.encoder.mode)
    {
        MainConf.encoder.mode   = TIM_ENCODERMODE_TI12;
        MainConf.encoder.filter = 4;
        MainConf.encoder.idxpos = TIM_ENCODERINDEX_POSITION_00;
        MainConf.encoder.nsteps = 4096;
    }

    /* Forced, for now */
    encoderConvFactor = 112;   
    
    /* Re-configure TIM2 base, IC1 and IC2 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = MainConf.encoder.nsteps - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    sConfig.EncoderMode = MainConf.encoder.mode;
    sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = MainConf.encoder.filter;
    sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = MainConf.encoder.filter;
    if (HAL_OK != HAL_TIM_Encoder_Init(&htim2, &sConfig)) return HAL_ERROR;

    /* Force master mode witout slaves */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_OK != HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)) return HAL_ERROR;

    /* Configure the INDEX mode */
    sEncoderIndexConfig.Polarity = TIM_ENCODERINDEX_POLARITY_NONINVERTED;
    sEncoderIndexConfig.Prescaler = TIM_ENCODERINDEX_PRESCALER_DIV1;
    sEncoderIndexConfig.Filter = MainConf.encoder.filter;
    sEncoderIndexConfig.FirstIndexEnable = DISABLE;
    sEncoderIndexConfig.Position = MainConf.encoder.idxpos;
    sEncoderIndexConfig.Direction = TIM_ENCODERINDEX_DIRECTION_UP_DOWN;
    if (HAL_OK != HAL_TIMEx_ConfigEncoderIndex(&htim2, &sEncoderIndexConfig)) return HAL_ERROR;

    /* Register the callback function used to signal the activation of the Index pulse */
    if (HAL_OK != HAL_TIM_RegisterCallback(&htim2, HAL_TIM_ENCODER_INDEX_CB_ID, encoderIndexCallback)) return HAL_ERROR;

    /* Start timers in encoder mode */
    if (HAL_OK != HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL)) return HAL_ERROR;
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    return HAL_OK;
}


/*******************************************************************************************************************//**
 * @brief   Read encoder value
 * @param   void
 * @return  int32_t     Encoder value
 */
uint32_t encoderGetCounter(void)
{
    return (uint32_t)__HAL_TIM_GET_COUNTER(&htim2);
}


/*******************************************************************************************************************//**
 * @brief   Read encoder value and convert to electrical-angle
 * @param   void
 * @return  uint16_t    Encoder value
 */
uint16_t encoderGetElectricalAngle(void)
{
    return (__HAL_TIM_GET_COUNTER(&htim2) * encoderConvFactor) & 0xFFFF;
}


/* END OF FILE ********************************************************************************************************/
