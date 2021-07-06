/*******************************************************************************************************************//**
 * @file    pwm.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 January, 19
 * @brief   PWM signals management
 **********************************************************************************************************************/

 
// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif

#if defined(USE_STM32HAL) 
// API
#include "pwm.h"
#endif

/* Includes ***********************************************************************************************************/
#if defined(USE_STM32HAL)
#include "stm32hal.h"
#if !defined(HALCONFIG_DONTUSE_UTILITIES)
#include "utilities.h"
#endif
#if !defined(HALCONFIG_DONTUSE_FLASH)
#include "flash.h"
#endif
#if !defined(HALCONFIG_DONTUSE_CONSOLE)
 #include "console.h"
#endif
#if !defined(HALCONFIG_DONTUSE_LED)
#include "led.h"
#endif
#else
#include "console.h"
#include "cordic.h"
#include "encoder.h"
#include "flash.h"
#include "led.h"
#include "pwm.h"
#include "tim.h"
#include "utilities.h"
#include "FreeRTOS.h"
#include "main.h"
#include "stm32g4xx_hal_tim.h"
#endif

#if (USE_HAL_TIM_REGISTER_CALLBACKS != 1)
    #error Flag TIM1 in menu "Project Manager -> Advanced Settings -> Register CallBack" of CubeMx must be ENABLED
#endif

#define USE_HALL_SENSORS    1
//#define USE_ENCODER_SENSORS 1


/* Private macros *****************************************************************************************************/

#define PWM_ENABLE_MASK     (EN1_Pin|EN2_Pin|EN3_Pin)

/* Current in six-step mode */
#define HALL_CURRENT_PHASE1 (0)
#define HALL_CURRENT_PHASE2 (1)
#define HALL_CURRENT_PHASE3 (2)


/* Private variables **************************************************************************************************/

/* Current value of the Hall sensors */ 
static volatile uint8_t  hallStatus = 0;
static volatile int16_t  pwmStatus = 0;
static volatile int32_t  hallCounter = 0;
static volatile uint16_t hallAngle = 0;
static volatile int16_t hallCurrentPhase = 0;
static volatile int16_t hallCurrent = 0;

#if defined(USE_STM32HAL) && defined(__cplusplus)
constexpr uint16_t hallAngleTable[] =
{
    /* ABC  (°)  */
    /* LLL ERROR */ 0,
    /* LLH  300  */ static_cast<uint16_t>(300.0 * 65536.0 / 360.0 + 0.5), /* 54613 */
    /* LHL  180  */ static_cast<uint16_t>(180.0 * 65536.0 / 360.0 + 0.5), /* 32768 */
    /* LHH  240  */ static_cast<uint16_t>(240.0 * 65536.0 / 360.0 + 0.5), /* 43690 */
    /* HLL   60  */ static_cast<uint16_t>( 60.0 * 65536.0 / 360.0 + 0.5), /* 10923 */
    /* HLH    0  */ static_cast<uint16_t>(  0.0 * 65536.0 / 360.0 + 0.5), /*     0 */
    /* HHL  120  */ static_cast<uint16_t>(120.0 * 65536.0 / 360.0 + 0.5), /* 21845 */
    /* HHH ERROR */ static_cast<uint16_t>(0)
};
#else
static const uint16_t hallAngleTable[] =
{
    /* ABC  (°)  */
    /* LLL ERROR */ 0,
    /* LLH  300  */ 300.0 * 65536.0 / 360.0 + 0.5, /* 54613 */
    /* LHL  180  */ 180.0 * 65536.0 / 360.0 + 0.5, /* 32768 */
    /* LHH  240  */ 240.0 * 65536.0 / 360.0 + 0.5, /* 43690 */
    /* HLL   60  */  60.0 * 65536.0 / 360.0 + 0.5, /* 10923 */
    /* HLH    0  */   0.0 * 65536.0 / 360.0 + 0.5, /*     0 */
    /* HHL  120  */ 120.0 * 65536.0 / 360.0 + 0.5, /* 21845 */
    /* HHH ERROR */ 0
};
#endif

/* Public variables ***************************************************************************************************/
/* Private functions **************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Read the current Hall sensor status and configure signals EN1, EN2, EN3, PHASE1, PHASE2, PHASE3 as for a
 *          CW or CCW torque
 * @param   pwm     Set the new PWM value. A positive value generate a CW torque, whereas a negative value generate a
 *                  CCW torque. Absolute value must be lower than MAX_PWM
 * @return  void
 *
 * This function configures the signals as in the following table
 *
 * |                   ||         CW           ||        CCW           |
 * | HALL1 HALL2 HALL3 || PHASE1 PHASE2 PHASE3 || PHASE1 PHASE2 PHASE3 |
 * |-------------------++----------------------++----------------------|
 * |   1     0     1   ||  PWM    LOW    HIZ   ||  LOW    PWM    HIZ   |
 * |   1     0     0   ||  PWM    HIZ    LOW   ||  LOW    HIZ    PWM   |
 * |   1     1     0   ||  HIZ    PWM    LOW   ||  HIZ    LOW    PWM   |
 * |   0     1     0   ||  LOW    PWM    HIZ   ||  PWM    LOW    HIZ   |
 * |   0     1     1   ||  LOW    HIZ    PWM   ||  PWM    HIZ    LOW   |
 * |   0     0     1   ||  HIZ    LOW    PWM   ||  HIZ    PWM    LOW   |
 * |-------------------||----------------------||----------------------|
 * |   0     0     0   ||  HIZ    HIZ    HIZ   ||  HIZ    HIZ    HIZ   | ERROR
 * |   1     1     1   ||  HIZ    HIZ    HIZ   ||  HIZ    HIZ    HIZ   | ERROR
 *
 * where:
 *      HIZ means that PHASEx is in high impedanace state (ENx = 0, PWMx = 0)
 *      LOW means that PHASEx is in LOW state (ENx = 1, PWMx = 0)
 *      PWM means that PHASEx is modulated with the pwm value (ENx = 1, PWMx = pwm)
 */
static void hallSetPWM(int16_t pwm)
{
#ifdef USE_HALL_SENSORS
    /* Remeber previous value */
    uint8_t hall = hallStatus;
#endif
    
    /* Read current value of HALL1, HALL2 and HALL3 signals in bits 2..0 */
    hallStatus = (((HALL1_GPIO_Port->IDR & HALL1_Pin) >> MSB(HALL1_Pin)) << 2)
               | (((HALL2_GPIO_Port->IDR & HALL2_Pin) >> MSB(HALL2_Pin)) << 1)
               | (((HALL3_GPIO_Port->IDR & HALL3_Pin) >> MSB(HALL3_Pin)) << 0);

#ifdef USE_HALL_SENSORS
    /* Current hall sensors status */
    switch (hallStatus)
    {
        case 0x05:
            /* PHASE1=pwm, PHASE2=-pwm, PHASE3=hiz */
            EN1_GPIO_Port->BSRR = EN1_Pin | EN2_Pin | (EN3_Pin<<16) ;
            /* CW rotation */
            if (pwm >= 0)
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                hallCurrentPhase = HALL_CURRENT_PHASE1;
            }
            /* CCW rotation */
            else
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, -pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                hallCurrentPhase = HALL_CURRENT_PHASE2;
            }
            /* Verify previous HALL sensors status */
            if      (hall == 0x01) ++hallCounter;
            else if (hall == 0x04) --hallCounter;
            break;

        case 0x04:
            /* PHASE1=pwm, PHASE2=hiz, PHASE3=-pwm */
            EN1_GPIO_Port->BSRR = EN1_Pin | (EN2_Pin<<16) | EN3_Pin ;
            /* CW rotation */
            if (pwm >= 0)
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                hallCurrentPhase = HALL_CURRENT_PHASE1;
            }
            /* CCW rotation */
            else
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, -pwm);
                hallCurrentPhase = HALL_CURRENT_PHASE3;
            }
            /* Verify previous HALL sensors status */
            if      (hall == 0x05) ++hallCounter;
            else if (hall == 0x06) --hallCounter;
            break;

        case 0x06:
            /* PHASE1=hiz, PHASE2=pwm, PHASE3=-pwm */
            EN1_GPIO_Port->BSRR = (EN1_Pin<<16) | EN2_Pin | EN3_Pin;
            /* CW rotation */
            if (pwm >= 0)
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                hallCurrentPhase = HALL_CURRENT_PHASE2;
            }
            /* CCW rotation */
            else
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, -pwm);
                hallCurrentPhase = HALL_CURRENT_PHASE3;
            }
            /* Verify previous HALL sensors status */
            if      (hall == 0x04) ++hallCounter;
            else if (hall == 0x02) --hallCounter;
            break;

        case 0x02:
            /* PHASE1=-pwm, PHASE2=pwm, PHASE3=hiz */
            EN1_GPIO_Port->BSRR = EN1_Pin | EN2_Pin | (EN3_Pin<<16) ;
            /* CW rotation */
            if (pwm >= 0)
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                hallCurrentPhase = HALL_CURRENT_PHASE2;
            }
            /* CCW rotation */
            else
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, -pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                hallCurrentPhase = HALL_CURRENT_PHASE1;
            }
            /* Verify previous HALL sensors status */
            if      (hall == 0x06) ++hallCounter;
            else if (hall == 0x03) --hallCounter;
            break;

        case 0x03:
            /* PHASE1=-pwm, PHASE2=hiz, PHASE3=pwm */
            EN1_GPIO_Port->BSRR = EN1_Pin | (EN2_Pin<<16) | EN3_Pin ;
            /* CW rotation */
            if (pwm >= 0)
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pwm);
                hallCurrentPhase = HALL_CURRENT_PHASE3;
            }
            /* CCW rotation */
            else
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, -pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                hallCurrentPhase = HALL_CURRENT_PHASE1;
            }
            /* Verify previous HALL sensors status */
            if      (hall == 0x02) ++hallCounter;
            else if (hall == 0x01) --hallCounter;
            break;

        case 0x01:
            /* PHASE1=hiz, PHASE2=-pwm, PHASE3=pwm */
            EN1_GPIO_Port->BSRR = (EN1_Pin<<16) | EN2_Pin | EN3_Pin;
            /* CW rotation */
            if (pwm >= 0)
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pwm);
                hallCurrentPhase = HALL_CURRENT_PHASE3;
            }
            /* CCW rotation */
            else
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, -pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                hallCurrentPhase = HALL_CURRENT_PHASE2;
            }
            /* Verify previous HALL sensors status */
            if      (hall == 0x03) ++hallCounter;
            else if (hall == 0x05) --hallCounter;
            break;

        default:
            /* ERROR: forbitten combination */
#if defined(HALCONFIG_DONTUSE_LED) 
#else 
        ledSet(LED2, 0);
#endif        
            EN1_GPIO_Port->BSRR = ((EN3_Pin|EN2_Pin|EN1_Pin)<<16);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            pwm = 0;
            break;
    }
#endif
    
    /* Update pwm status */
    pwmStatus = pwm;

    /* Update angle */
    hallAngle = hallAngleTable[hallStatus];
}


/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Called by TIM1 when signal nMFAULT is activated by the motor driver.
 * @param   htim    Pointer to TIM1 handler
 * @return  void
 */
static void pwmMotorFault_cb(TIM_HandleTypeDef *htim)
{
    /* Check TIM1 already initialized */
    if (TIM1 == htim->Instance)
    {
        /* Avoid re-entering in the isr */
        __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_BREAK);
        /* Disable the driver */
        EN1_GPIO_Port->BSRR = ((EN3_Pin|EN2_Pin|EN1_Pin)<<16);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
        /* Null output voltage */
        pwmStatus = 0;
        /* Advertise the fault */
#if defined(HALCONFIG_DONTUSE_LED) 
#else         
        ledSet(LED2, 0);
#endif        
    }
}


/*******************************************************************************************************************//**
 * @brief   Called by TIM3 whenever it detects an edge over HALL1, HALL2 and HALL3 signals
 * @param   htim    Pointer to TIM3 handler
 * @return  void
 */
static void hallStatusChange_cb(TIM_HandleTypeDef *htim)
{
    /* Check TIM3 already initialized */
    if (TIM3 == htim->Instance)
    {
        /* Generate the correct PWM configuration following the pwmStatus pattern */
        hallSetPWM(pwmStatus);
    }
}


/*******************************************************************************************************************//**
 * @brief   Called by DMA associated to ADC2 whenever a new set of phase currents is available
 * @param   i1, i2, i3  Currents measured in PHASE1, PHASE2 and PHASE3 of the motor
 * @return  void
 */
void pwmSetCurrents_cb(int16_t i1, int16_t i2, int16_t i3)
{
    switch (hallCurrentPhase)
    {
        case HALL_CURRENT_PHASE1:
            hallCurrent = i1;
            break;
        case HALL_CURRENT_PHASE2:
            hallCurrent = i2;
            break;
        case HALL_CURRENT_PHASE3:
            hallCurrent = i3;
            break;
    }        
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Complete the CubeMx TIM1 configuration for the PWM subsystem
 * @param   void
 * @return  void
 */
HAL_StatusTypeDef pwmInit(void)
{
    /* Clear any preceeding fault condition */
    pwmReset(ENABLE);
    pwmSleep(DISABLE);
    HAL_Delay(10);
    pwmReset(DISABLE);
    HAL_Delay(10);

    if (0 == (MainConf.pwm.mode & PWM_CONF_MODE_MASK))
    {
        MainConf.pwm.mode = PWM_CONF_MODE_HALL;
        MainConf.pwm.poles = 7;
    }
        
    /* Register the required TIM1 callback functions */
    HAL_TIM_RegisterCallback(&htim1, HAL_TIM_BREAK_CB_ID, pwmMotorFault_cb);
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_BREAK);

    /* Reset the PWM value */
    hallSetPWM(pwmStatus);

    /* Start TIM1 as 3-phase PWM generator */
    if (HAL_OK != HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) ||
        HAL_OK != HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) ||
        HAL_OK != HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3) ||
    HAL_OK != HAL_TIM_Base_Start_IT(&htim1))
    {
        HAL_TIM_Base_Stop(&htim1);
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
        return HAL_ERROR;
    }

    /* All done */
    return HAL_OK;
}


/*******************************************************************************************************************//**
 * @brief   Complete the CubeMx TIM3 configuration for the PWM subsystem
 * @param   void
 * @return  void
 */
HAL_StatusTypeDef hallInit(void)
{
    /* Read value of HALL1, HALL2 and HALL3 signals in bits 2..0 */
    hallStatus = (((HALL1_GPIO_Port->IDR & HALL1_Pin) >> MSB(HALL1_Pin)) << 2)
               | (((HALL2_GPIO_Port->IDR & HALL2_Pin) >> MSB(HALL2_Pin)) << 1)
               | (((HALL3_GPIO_Port->IDR & HALL3_Pin) >> MSB(HALL3_Pin)) << 0);

    /* Init angle */
    hallAngle = hallAngleTable[hallStatus];
   
    /* Start position counter */
    hallCounter = 0;

    /* Check TIM3 configuration and start it */
    return ((TIM3 == htim3.Instance) &&
            (HAL_OK == HAL_TIM_RegisterCallback(&htim3, HAL_TIM_IC_CAPTURE_CB_ID, hallStatusChange_cb)) &&
            (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1)) &&
            (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2)) &&
            (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3)) &&
            (HAL_OK == HAL_TIM_Base_Start_IT(&htim3)))? HAL_OK : HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   Read the position counter driven by the Hall sensors
 * @param   void
 * @return  int32_t     Current value of the position counter
 */
int32_t hallGetCounter(void)
{
    return hallCounter;
}


/*******************************************************************************************************************//**
 * @brief   Set the position counter driven by the Hall sensors
 * @param   count   New value of the position counter
 * @return  void
 */
void hallSetCounter(int32_t count)
{
    hallCounter = count;
}


/*******************************************************************************************************************//**
 * @brief   Read the angle measured by the Hall sensors
 * @param   void
 * @return  uint16_t    Current value of the angle
 */
uint16_t hallGetAngle(void)
{
    return hallAngle;
}


/*******************************************************************************************************************//**
 * @brief   Read the last measured status of the Hall sensors
 * @param   void
 * @return  uint16_t    bit2 <- HALL_1, bit1 <- HALL_2, bit0 <- HALL_3
 */
uint16_t hallGetStatus(void)
{
    return hallStatus;
}


/*******************************************************************************************************************//**
 * @brief   Read the phase current in hall 6-step mode
 * @param   void
 * @return  int16_t     Current in mA
 */
int16_t hallGetCurrent(void)
{
    return hallStatus;
}


/*******************************************************************************************************************//**
 * @brief   Enter or Exit the SLEEP mode of the motor driver
 * @param   enable  It can assume one of the following values:
 *                  ENABLE      Motor driver in SLEEP mode: Power dissipation is limited but PWM is not allowed
 *                  DISABLE     Motor driver in active mode: PWM generation is allowed.
 * @return  void
 */
void pwmSleep(FunctionalState enable)
{
    /* Configure signal nMSLEEP as required */
    HAL_GPIO_WritePin(nMSLEEP_GPIO_Port, nMSLEEP_Pin, (ENABLE==enable)?GPIO_PIN_RESET:GPIO_PIN_SET);
}
    

/*******************************************************************************************************************//**
 * @brief   Assert or release the nMRESET signal of the driver. It can be used to clear any fault condition
 * @param   enable  It can assume one of the following values:
 *                  ENABLE      Assert motor driver RESET
 *                  DISABLE     Release motor driver RESET
 * @return  void
 */
void pwmReset(FunctionalState enable)
{
    HAL_GPIO_WritePin(nMRESET_GPIO_Port, nMRESET_Pin, (ENABLE==enable)?GPIO_PIN_RESET:GPIO_PIN_SET);
}


/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  Function result
 */
HAL_StatusTypeDef pwmResetFault(void)
{
    pwmSet(0, 0, 0);
    pwmSleep(ENABLE); HAL_Delay(10);
    pwmSleep(DISABLE);
    if (GPIO_PIN_SET == HAL_GPIO_ReadPin(nMFAULT_GPIO_Port, nMFAULT_Pin))
    {
#ifdef USE_ENCODER_SENSORS
        pwmPhaseEnable(PWM_PHASE_ALL);
#endif
        __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_BREAK);
        __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_BREAK);
        __HAL_TIM_MOE_ENABLE(&htim1); HAL_Delay(10);
        __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_BREAK);
#if defined(HALCONFIG_DONTUSE_LED) 
#else        
        ledSet(0, LED2);
#endif        
        return HAL_OK;
    }
#if defined(HALCONFIG_DONTUSE_LED) 
#else     
    ledSet(LED2, 0);
#endif    
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  void
 */
void pwmSet(uint16_t u, uint16_t v, uint16_t w)
{
    /* Saturate arguments */
    if (u > (uint16_t)MAX_PWM) u = MAX_PWM;
    if (v > (uint16_t)MAX_PWM) v = MAX_PWM;
    if (w > (uint16_t)MAX_PWM) w = MAX_PWM;
    /* Update PWM generators */
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, u);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, v);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, w);
}


/*******************************************************************************************************************//**
 * @brief   Enable the PWM driver outputs
 * @param   mask    Any combination of the following values
 *                  PWM_PHASE_U
 *                  PWM_PHASE_V
 *                  PWM_PHASE_W
 * @return  void
 */
void pwmPhaseEnable(uint16_t mask)
{
    EN1_GPIO_Port->BSRR = ((uint32_t)mask & PWM_PHASE_ALL);
}


/*******************************************************************************************************************//**
 * @brief   Disable the PWM driver outputs
 * @param   mask    Any combination of the following values
 *                  PWM_PHASE_U
 *                  PWM_PHASE_V
 *                  PWM_PHASE_W
 * @return  void
 */
void pwmPhaseDisable(uint16_t mask)
{
    EN1_GPIO_Port->BSRR = ((uint32_t)mask & PWM_PHASE_ALL) << 16u;
}

/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  Function result
 */
HAL_StatusTypeDef pwmSetValue(int32_t pwm)
{
    if ((pwm >= -(int32_t)MAX_PWM) && (pwm <= (int32_t)MAX_PWM))
    {
#ifdef USE_HALL_SENSORS
        __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3);
        hallSetPWM(pwm);
        __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3);
#endif
        return HAL_OK;
    }
    return HAL_ERROR;
}

#if defined(HALCONFIG_DONTUSE_TESTS)
#else

/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  
 */
void pwmTest(void)
{
    char ch;
    char coBuffer[80];
    const char *curs;
    int32_t pwm;

    coprintf("Motor Test\n");
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(nMFAULT_GPIO_Port, nMFAULT_Pin))
    {
        ledSet(LED2, 0);
        coprintf("ATTENTION: FAULT indication active\n");
    }
    else ledSet(0, LED2);
    for (;;)
    {
		coprintf("F - Clear motor FAULT indication\n"
                 "H - Read HALL and ENCODER status\n"
                 "P - Set motor PWM\n"
                 "R - Reset Hall sensor counter\n"
                 "ESC : Exit test\n"
                 "? "); HAL_Delay(250);
        if (isgraph(ch = coRxChar())) coPutChar(ch);
        coPutChar('\n');

		switch (toupper(ch))
        {
            case 'F':
                if (HAL_OK != pwmResetFault())
                {
                    coprintf("ERROR: Cannot clear FAULT indication\n");
                }
                break;

            case 'H':
                while (1)
                {
                    coprintf("\rENCODER = %5u, HALL = %+11d, CURRENT=%+5dmA ", 
                                encoderGetCounter(), hallGetCounter(), analogConvertCurrent(hallCurrent));
                    HAL_Delay(100);
                    if (coRxReady())
                    {
                        ch = coRxChar();
                        if (('r'==ch)||('R'==ch)) hallSetCounter(0);
                        else if ('\x1B'==ch) break;
                    }
                }
                coprintf("\n");
                break;

            case 'P':
                coprintf("(%d)? ", pwmStatus);
                coEditString(coBuffer, sizeof(coBuffer));
                coprintf("\n");
                curs = &(coBuffer[0]);
                if ('\0' != skipblank(&curs))
                {
                    if (!atosl(&curs, &pwm) || (HAL_OK != pwmSetValue(pwm)))
                    {
                        coprintf("ERROR: %s\n", coBuffer); 
                    }
                }
            break;

            case 'R':
                hallSetCounter(0);
                break;

            case '\x1B':
                coprintf("Exit test\n");
                return;

            default:
                coprintf("ERROR: %c\n", ch);
                break;
        }
    }
}

#endif // HALCONFIG_DONTUSE_TESTS

/* END OF FILE ********************************************************************************************************/
