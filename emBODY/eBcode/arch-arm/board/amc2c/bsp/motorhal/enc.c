/*******************************************************************************************************************//**
 * @file    enc.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 29
 * @brief   Incremental Encoder management
 **********************************************************************************************************************/

// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif
 
#include "embot_core.h"
 
#if defined(USE_STM32HAL) 
// API
#include "enc.h"
#endif

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
/* Includes ***********************************************************************************************************/
#include "enc.h"
#include "main.h"
#include "led.h"
#include "stm32h7xx_hal_tim.h"
#include "console.h"
#include "FreeRTOS.h"
#include "task.h"
#endif // #if defined(USE_STM32HAL)

/* Private macros *****************************************************************************************************/

#if defined(MOTORHAL_changes)

// in ... maybe add an include file ...
namespace embot::hw::motor::bsp {
    extern TIM_HandleTypeDef hTIM5;
}

#define ENC_TIM (embot::hw::motor::bsp::hTIM5)

#else
/* Can be htim2 or htim5 */
#define ENC_TIM                         htim5
#endif

/* Select the index channels */
#define ENC_INDEX_LEADING_EDGE          TIM_CHANNEL_3
#define ENC_INDEX_IT_LEADING_EDGE       TIM_IT_CC3
#define ENC_INDEX_ACTIVE_LEADING_EDGE   HAL_TIM_ACTIVE_CHANNEL_3
#define ENC_INDEX_TRAILING_EDGE         TIM_CHANNEL_4
#define ENC_INDEX_IT_TRAILING_EDGE      TIM_IT_CC4
#define ENC_INDEX_ACTIVE_TRAILING_EDGE  HAL_TIM_ACTIVE_CHANNEL_4

/* Status register values */
#define ENC_STATUS_IDLE     (0)
#define ENC_STATUS_WAIT     (1)
#define ENC_STATUS_READY    (2)

/* Offset between the counting edge and the index edge */
#define ENC_UP_COUNTING_OFFSET      (1)
#define ENC_DOWN_COUNTING_OFFSET    (1)


/* Extenal variables **************************************************************************************************/

/* Declared in main.h */
extern TIM_HandleTypeDef ENC_TIM;


/* Private typedefs ***************************************************************************************************/

/* Private variables **************************************************************************************************/

static uint8_t  EncStatus = ENC_STATUS_IDLE;
static uint32_t EncRotorZero;
static uint32_t EncAbsoluteZero;


/* Local functions ****************************************************************************************************/

/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
static void EncCapture_cb(TIM_HandleTypeDef *htim)
{
    int32_t le, te, delta;
    /* Read the index trailing edge position */
    te = (int32_t)__HAL_TIM_GetCompare(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
    /* There must be a leading edge before */
    if (0 != __HAL_TIM_GET_FLAG(&ENC_TIM, ENC_INDEX_IT_LEADING_EDGE))
    {
        /* Read the index leading edge position */
        le = (int32_t)__HAL_TIM_GetCompare(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
        /* Take the difference between readings */
        delta = te - le;
        /* Avoid inversion of direction over the index */
        if (0 != delta)
        {
            /* Update the index position */
            EncRotorZero = (uint32_t)((delta>=0)? (le + ENC_UP_COUNTING_OFFSET)
                                                : (le - ENC_DOWN_COUNTING_OFFSET));
            /* Mark the absolute zero position */
            if (ENC_STATUS_WAIT == EncStatus)
            {
                /* Store the absolute zero */
                EncAbsoluteZero = EncRotorZero;
                /* Do not repeat again */
                EncStatus = ENC_STATUS_READY;
            }
        }
    }
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void EncCaptureIndex(void)
{
    /* Enable detection of the first index */
    EncStatus = ENC_STATUS_WAIT;
}
    
/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
bool EncIndexDetected(void)
{
    /* Check the status of the index */
    return (ENC_STATUS_READY == EncStatus)? true: false;
}
    

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
int32_t EncGetAbsolutePosition(void)
{
    /* Read counter 32 bits value */
    return __HAL_TIM_GetCounter(&ENC_TIM) - EncAbsoluteZero;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
int32_t EncGetRotorPosition(void)
{
    /* Read counter 32 bits value */
    return __HAL_TIM_GetCounter(&ENC_TIM) - EncRotorZero;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
HAL_StatusTypeDef EncInit(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&ENC_TIM, TIM_CHANNEL_ALL);

    /* Register the callback function used to signal the activation of the Index pulse */
    if (HAL_OK == HAL_TIM_RegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID, EncCapture_cb))
    {
        /* Clear local variables */
        EncStatus = ENC_STATUS_IDLE;
        EncAbsoluteZero = 0;
        EncRotorZero = 0;
        /* Clear counter */
        __HAL_TIM_SET_COUNTER(&ENC_TIM, 0);
        /* Start timers in encoder mode */
        if (HAL_OK == HAL_TIM_Encoder_Start(&ENC_TIM, TIM_CHANNEL_ALL))
        {
            /* Enable leading edge capture, without interrupts */
            HAL_TIM_IC_Start(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
            /* Enable trailing edge capture, with interrupts */
            HAL_TIM_IC_Start_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
            return HAL_OK;
        }
        /* Failed start of the timer */
        HAL_TIM_UnRegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
    }
    /* Errors detected */
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void EncDeInit(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&ENC_TIM, TIM_CHANNEL_ALL);
    HAL_TIM_UnRegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
}

#if defined(MOTORHALCONFIG_DONTUSE_TESTS)
void EncTest(void) {}
#else

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void EncTest(void)
{
    if (HAL_OK == EncInit())
    {
        coprintf("Encoder test started\n");
        while (1)
        {
            coprintf("\r  %u %+05d / %+011d ", 
                     EncIndexDetected()? 1 : 0,
                     EncGetRotorPosition(),
                     EncGetAbsolutePosition());
            if (coRxReady())
            {
                switch (coRxChar())
                {
                    case 'x':
                    case 'X':
                        EncCaptureIndex();
                        break;
                    case '\r':
                        coprintf("\nEncoder test terminated\n");
                        EncDeInit();
                        return;
                }
            }
            HAL_Delay(100);
        }
    }
}

#endif // #if defined(MOTORHALCONFIG_DONTUSE_TESTS)


#if defined(MOTORHAL_changes) 


HAL_StatusTypeDef enc_Config(uint8_t has_quad_enc, int16_t resolution, uint8_t num_polar_couples, uint8_t has_hall_sens)
{
    #warning TODO....
    return HAL_OK;
}


int32_t enc_GetElectricalAngle(void)
{   
    #warning TODO: review the values etc
    // in here ther is how it is for the amcbldc
//    if (MainConf.encoder.resolution == 0)
//    {
//        return s_encoder_ForcedValue;
//    }
//    
//    if (MainConf.encoder.has_hall_sens)
//    {
//        if (!s_encoder_Calibrated)
//        {
//            return s_encoder_ForcedValue;
//        }
//    }
//    
//    return s_encoder_electricalOffset + (__HAL_TIM_GET_COUNTER(&htim2) * encoderConvFactor) & 0xFFFF;
    
    int32_t v = __HAL_TIM_GetCounter(&ENC_TIM);
    
    int32_t s_encoder_electricalOffset = 0;
    int32_t encoderConvFactor = 1;
    
    return s_encoder_electricalOffset + (v * encoderConvFactor) & 0xFFFF;  
}

#endif // #if defined(MOTORHAL_changes) 


/* END OF FILE ********************************************************************************************************/

