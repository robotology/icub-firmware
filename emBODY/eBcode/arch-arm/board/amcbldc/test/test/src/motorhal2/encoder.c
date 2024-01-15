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
 
#include "embot_core.h"
 
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
#if !defined(MOTORHALCONFIG_DONTUSE_FLASH)
#include "flash.h"
#endif
#else

#include "FreeRTOS.h"
#include "encoder.h"
#include "flash.h"
#include "led.h"
#include "main.h"
#include "utilities.h"
#include "tim.h"
#include "stm32g4xx_hal_tim.h"

#endif

#if (USE_HAL_TIM_REGISTER_CALLBACKS != 1)
    #error Flag TIM2 in menu "Project Manager -> Advanced Settings -> Register CallBack" of CubeMx must be ENABLED
#endif

/* Private macros *****************************************************************************************************/
#if defined(MOTORHAL_changes)
#else
#define ENCODER_AVERAGE_LENGTH  (10)
#endif

/* Private variables **************************************************************************************************/

/* Coversion factor from encoder step value to elctrical angle. It is given by:
 * encoderConvFactor = 16777216 * number_of_poles / number_of_encoder_steps
 */
static int32_t encoderConvFactor = 29360;   /* 29360.128 */

/* Encoder averaged speed */
static volatile int32_t encoderSpeed = 0;

#if defined(MOTORHAL_changes)

// --------------------------------------------------------------------------------------------------------------------
// add in here all the new types, variables, static function prototypes required by MOTORHAL
// --------------------------------------------------------------------------------------------------------------------

static volatile uint16_t s_encoder_electricalOffset = 0;
static volatile bool s_encoder_Calibrated = false;
static volatile uint16_t s_encoder_ForcedValue = 0;

#endif // #if defined(MOTORHAL_changes)

/* Callbacks **********************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void encoderIndexCallback(TIM_HandleTypeDef *htim)
{
//    UNUSED(htim);
    /**************
     * TO BE DONE *
     **************/
//    ledSet(LED1, 0);
//    ledSet(0, LED1);
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void encoderCaptureCallback(TIM_HandleTypeDef *htim)
{
//    UNUSED(htim);
    static int32_t last = 0;
#if defined(MOTORHAL_changes)
    enum { sizeOfbuffer = 10 };
    static int32_t buffer[sizeOfbuffer] = {0};
#else
    static int32_t buffer[10] = {0};
#endif    
    static unsigned index = 0;
//    ledSet(LED2, 0);
    /* Read the new capture value */
    int32_t count = READ_REG(htim->Instance->CCR4);
    /* Calculate the difference from last reading */
    int32_t delta = count - last; last = count;
    /* Adjust for over/under flow */
    if (delta >= ENCODER_COUNT_PER_REVOLUTION/2) delta -= ENCODER_COUNT_PER_REVOLUTION;
    else if (delta <= -ENCODER_COUNT_PER_REVOLUTION/2) delta += ENCODER_COUNT_PER_REVOLUTION;
    /* Update the running average (multiplied by 100) */
    int32_t speed = encoderSpeed - buffer[index];
    encoderSpeed = speed + (buffer[index++] = delta);
#if defined(MOTORHAL_changes)    
    if (index >= sizeOfbuffer) index = 0;
#else
    if (index >= lengthof(buffer)) index = 0;
#endif    
//    ledSet(0, LED2);
}
    

/* Exported functions *************************************************************************************************/

#if defined(MOTORHAL_changes) 

// in here we dont start the timer TIM2 straight away as in gzini implementation
// rather, we just collect some data which never changes.
// we start the timer in function encoder_Config() called when we receive the encoder properties
// the function encoder_Config() uses the remaining of the code of gzini's encoderInit()

HAL_StatusTypeDef encoderInit(void)
{
    /* here settings not managed by yarprobotinterface but still configurable */
    MainConf.encoder.mode   = TIM_ENCODERMODE_TI12;
    MainConf.encoder.filter = 4;
    MainConf.encoder.idxpos = TIM_ENCODERINDEX_POSITION_00;
    
    /* default values */
    MainConf.encoder.resolution = 0;
    MainConf.encoder.has_hall_sens = 1;
    
    return HAL_OK;
}

#else

/*******************************************************************************************************************//**
 * @brief   Start TIM2 as 32 bits incremental encoders and TIM15 as periodic counter.
 * @param   void
 * @return  HAL_StatusTypeDef   Operation result
 *
 * Start TIM2 as 32 bits incremental encoders in mode x 4 with index input. TIM15 is configured as periodic counter
 * with 1.5 ms period. The TRGO output of TIM15 is configured as UPDATE trigger output. The UPDATE is routed to
 * the input tim_itr6 of TIM2, and connected to capture/compare register CC4.
 * In this way TIM2_CC4 register stores the encoder counter every 1.5 ms. Register value is:
 * 
 * CC4 = Tsample * Nstep * SPEEDrpm / 60
 *
 * Given Tsample = 0.0015 s and Nstep = 4000, we get
 *
 * CC4 = SPEEDrpm/10.
 *
 * Function encoderCaptureCallback(), called by the CC4 interrupt every 1.5 ms, calculates the sum of 10 consecutive
 * speed samples and stores the result in encoderSpeed variables. The value rapresents a lightly filterd measure of
 * the motor speed in rpm.
 *
 * The sample rate must ensure at least 2 samples per encoder rotation. Therefore the maximum encoder speed is:
 *
 * SPEEDrpm(max) < 30/Tsample
 *
 * that is 20000 rpm in this implementetion
 */
HAL_StatusTypeDef encoderInit(void)
{
    TIM_Encoder_InitTypeDef sEncConf = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIMEx_EncoderIndexConfigTypeDef sEncoderIndexConfig = {0};
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};

    /* Default configuration */
    if (0 == MainConf.encoder.mode)
    {
        MainConf.encoder.mode   = TIM_ENCODERMODE_TI12;
        MainConf.encoder.filter = 4;
        MainConf.encoder.idxpos = TIM_ENCODERINDEX_POSITION_00;
        MainConf.encoder.nsteps = 4000;
    }

    /* Forced, for now */
    // numero che serve pe convertire da tacche ad angolo
    encoderConvFactor = 29360;   
    
    /* Re-configure TIM2 base, IC1 and IC2 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = MainConf.encoder.nsteps - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    sEncConf.EncoderMode = MainConf.encoder.mode;
    sEncConf.IC1Polarity = TIM_ICPOLARITY_RISING;
    sEncConf.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sEncConf.IC1Prescaler = TIM_ICPSC_DIV1;
    sEncConf.IC1Filter = MainConf.encoder.filter;
    sEncConf.IC2Polarity = TIM_ICPOLARITY_RISING;
    sEncConf.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sEncConf.IC2Prescaler = TIM_ICPSC_DIV1;
    sEncConf.IC2Filter = MainConf.encoder.filter;
    if (HAL_OK != HAL_TIM_Encoder_Init(&htim2, &sEncConf)) return HAL_ERROR;

    /* Disable master mode */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENCODER_CLK;
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

    htim15.Instance = TIM15;
    htim15.Init.Prescaler = 167;
    htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim15.Init.Period = 1499; /* Forced for now */
    htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim15.Init.RepetitionCounter = 0;
    htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim15) != HAL_OK) return HAL_ERROR;
    
    /* Configure the clock source */
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK) return HAL_ERROR;

    /* Disable master mode */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK) return HAL_ERROR;

    /* Select tim_itr6 = TIM15_TRGO */
    htim2.Instance->SMCR = (htim2.Instance->SMCR & ~TIM_SMCR_TS)
                         | TIM_SMCR_TS_3|TIM_SMCR_TS_1;

    /* Link CC4 to tim_itr6 */
    htim2.Instance->CCMR2 = TIM_CCMR2_CC4S;
    htim2.Instance->CCER |= TIM_CCER_CC4E;

    /* Reset counters */
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
    
    /* Register the callback function used to signal the activation of the Index pulse */
    if ((HAL_OK != HAL_TIM_RegisterCallback(&htim2, HAL_TIM_ENCODER_INDEX_CB_ID, encoderIndexCallback)) || 
        (HAL_OK != HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_CB_ID, encoderCaptureCallback))) return HAL_ERROR;
        
    /* Start base frequency for speed measurements */
    HAL_TIM_Base_Start(&htim15);

    /* Start timers in encoder mode */
    if (HAL_OK != HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL)) return HAL_ERROR;
    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_IDX|TIM_IT_CC4);
    return HAL_OK;
}

#endif // #if defined(MOTORHAL_changes) 


/*******************************************************************************************************************//**
 * @brief   Read encoder value
 * @param   void
 * @return  int32_t     Encoder value
 */
uint32_t encoderGetCounter(void)
{
    return (uint32_t)__HAL_TIM_GET_COUNTER(&htim2);
}

#if defined(MOTORHAL_changes) 

// in here we just add some controls and we call zgini implementation
// by adding an offset to it

uint32_t encoderGetElectricalAngle(void)
{   
    #warning acemor: attenzione al >> 8 che manca
    if (MainConf.encoder.resolution == 0)
    {
        return s_encoder_ForcedValue;
    }
    
    if (MainConf.encoder.has_hall_sens)
    {
        if (!s_encoder_Calibrated)
        {
            return s_encoder_ForcedValue;
        }
    }
    
    return s_encoder_electricalOffset + (__HAL_TIM_GET_COUNTER(&htim2) * encoderConvFactor) & 0xFFFF;
}

#else

/*******************************************************************************************************************//**
 * @brief   Read encoder value and convert to electrical-angle
 * @param   void
 * @return  uint16_t    Encoder value
 */
uint32_t encoderGetElectricalAngle(void)
{
    return ((__HAL_TIM_GET_COUNTER(&htim2) * encoderConvFactor) >> 8) & 0x0000FFFF;
}

#endif // #if defined(MOTORHAL_changes) 


/*******************************************************************************************************************//**
 * @brief   Read encoder value
 * @param   void
 * @return  int32_t     Encoder value
 */
int32_t encoderGetSpeed(void)
{
    return encoderSpeed;
}

#if defined(MOTORHAL_changes)

// --------------------------------------------------------------------------------------------------------------------
// add in here all the new functions required by MOTORHAL, be them extern or static
// --------------------------------------------------------------------------------------------------------------------


HAL_StatusTypeDef encoder_Deinit(void)
{
    // marco.accame: no changes in TIM2 across the two versions of gzini's  motorhal 
    HAL_TIM_Encoder_Stop(&htim2, TIM_CHANNEL_ALL);
    
    /* default values */
    MainConf.encoder.resolution = 0;
    MainConf.encoder.has_hall_sens = 1;
    
    /* deinitialize */
    encoderConvFactor  = 0;
    s_encoder_electricalOffset   = 0;
    s_encoder_Calibrated  = false;
    s_encoder_ForcedValue = 0;
    
    return HAL_OK;
}

#define USE_TIM15
// for speed measurement

HAL_StatusTypeDef encoder_Config(uint8_t has_quad_enc, int16_t resolution, uint8_t num_polar_couples, uint8_t has_hall_sens)
{
    TIM_Encoder_InitTypeDef sEncConf = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIMEx_EncoderIndexConfigTypeDef sEncoderIndexConfig = {0};
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    
    MainConf.encoder.has_hall_sens = has_hall_sens;
    MainConf.pwm.num_polar_couples = num_polar_couples;
    
    if(!has_quad_enc)
        resolution=0;
    
    MainConf.encoder.resolution = resolution;
    
    if (resolution == 0)
    {
        return HAL_OK;
    }

    if (resolution < 0)
    {
        resolution = -resolution;
        htim2.Init.CounterMode = TIM_COUNTERMODE_DOWN;
    }
    else
    {
        htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    }
    

    /* Forced, for now */
    // numero che serve pe convertire da tacche ad angolo
    encoderConvFactor = 65536L*num_polar_couples/resolution;   
    
    /* Re-configure TIM2 base, IC1 and IC2 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    //htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = (resolution/num_polar_couples + 0.5) - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    sEncConf.EncoderMode = MainConf.encoder.mode;
    sEncConf.IC1Polarity = TIM_ICPOLARITY_RISING;
    sEncConf.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sEncConf.IC1Prescaler = TIM_ICPSC_DIV1;
    sEncConf.IC1Filter = MainConf.encoder.filter;
    sEncConf.IC2Polarity = TIM_ICPOLARITY_RISING;
    sEncConf.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sEncConf.IC2Prescaler = TIM_ICPSC_DIV1;
    sEncConf.IC2Filter = MainConf.encoder.filter;
    if (HAL_OK != HAL_TIM_Encoder_Init(&htim2, &sEncConf)) return HAL_ERROR;

    /* Disable master mode */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENCODER_CLK;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_OK != HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)) return HAL_ERROR;

    if (MainConf.encoder.has_hall_sens)
    {
        /* no INDEX needed */
        HAL_TIMEx_DisableEncoderIndex(&htim2);
    }
    else
    {
        /* Configure the INDEX mode */        
        sEncoderIndexConfig.Polarity = TIM_ENCODERINDEX_POLARITY_NONINVERTED;
        sEncoderIndexConfig.Prescaler = TIM_ENCODERINDEX_PRESCALER_DIV1;
        sEncoderIndexConfig.Filter = MainConf.encoder.filter;
        sEncoderIndexConfig.FirstIndexEnable = DISABLE;
        sEncoderIndexConfig.Position = MainConf.encoder.idxpos;
        sEncoderIndexConfig.Direction = TIM_ENCODERINDEX_DIRECTION_UP_DOWN;
        if (HAL_OK != HAL_TIMEx_ConfigEncoderIndex(&htim2, &sEncoderIndexConfig)) return HAL_ERROR;
        
        
#if defined(USE_TIM15)
        htim15.Instance = TIM15;
        htim15.Init.Prescaler = 167;
        htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim15.Init.Period = 1499; /* Forced for now */
        htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim15.Init.RepetitionCounter = 0;
        htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_Base_Init(&htim15) != HAL_OK) return HAL_ERROR;
        
        /* Configure the clock source */
        sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
        if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK) return HAL_ERROR;

        /* Disable master mode */
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK) return HAL_ERROR;

        /* Select tim_itr6 = TIM15_TRGO */
        htim2.Instance->SMCR = (htim2.Instance->SMCR & ~TIM_SMCR_TS)
                             | TIM_SMCR_TS_3|TIM_SMCR_TS_1;

        /* Link CC4 to tim_itr6 */
        htim2.Instance->CCMR2 = TIM_CCMR2_CC4S;
        htim2.Instance->CCER |= TIM_CCER_CC4E;        
#endif

        /* Reset counters */
        __HAL_TIM_SET_COUNTER(&htim2, 0);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
        
        /* Register the callback function used to signal the activation of the Index pulse */
        if ((HAL_OK != HAL_TIM_RegisterCallback(&htim2, HAL_TIM_ENCODER_INDEX_CB_ID, encoderIndexCallback)) || 
            (HAL_OK != HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_CB_ID, encoderCaptureCallback))) return HAL_ERROR;
        
        /* Start base frequency for speed measurements */
        HAL_TIM_Base_Start(&htim15);       
    }
    
    /* Start timers in encoder mode */
    if (HAL_OK != HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL)) return HAL_ERROR;
    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_IDX|TIM_IT_CC4);
  
    return HAL_OK;
}


void encoder_Reset()
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
}

#warning acemor: attenzione al >> 8 che manca
uint16_t encoder_GetUncalibrated(void)
{
    return (__HAL_TIM_GET_COUNTER(&htim2) * encoderConvFactor) & 0xFFFF;
}

void encoder_Force(uint16_t value)
{
    s_encoder_ForcedValue = value;
}

void encoder_Calibrate(uint16_t offset)
{   
    s_encoder_electricalOffset = offset;
    
    s_encoder_Calibrated = true;
}

uint16_t encoder_GetElectricalOffset()
{
    return s_encoder_electricalOffset;
}

#endif // #if defined(MOTORHAL_changes) 


/* END OF FILE ********************************************************************************************************/
