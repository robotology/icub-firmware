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
#include "hall.h"
#endif

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
/* Includes ***********************************************************************************************************/
#include "hall.h"
#include "main.h"
#include "led.h"
#include "adcm.h"
#include "enc.h"
#include "pwm.h"
#include "utilities.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal_tim.h"
#include "console.h"
#include "FreeRTOS.h"
#include "task.h"
#endif // #if defined(USE_STM32HAL)

/* Private macros *****************************************************************************************************/

#if defined(MOTORHAL_changes)

#include "embot_hw_motor_bsp_amc2c.h"
#define HALL_TIM (embot::hw::motor::bsp::amc2c::hTIM4)

#else
/* Can be htim2 or htim5 */
#define HALL_TIM        htim4
#endif // #if defined(MOTORHAL_changes)

/* Hardware related definitions */
#define HALL_INPUT_PORT     (MOT_HALL1_GPIO_Port->IDR)
#define HALL_INPUT_MASK     (MOT_HALL1_Pin|MOT_HALL2_Pin|MOT_HALL3_Pin)
#define HALL_INPUT_POS      (LSB(HALL_INPUT_MASK))
#define HALL_INPUT()        ((HALL_INPUT_PORT&HALL_INPUT_MASK)>>HALL_INPUT_POS)

// marco.accame, note-667: 
// HALL_INPUT() gets three bits in this order: H3|H2|H1, where:
// - H3 is the status of {MOT_HALL3_GPIO_Port, MOT_HALL3_Pin} = PD14
// - H2 is the status of {MOT_HALL2_GPIO_Port, MOT_HALL2_Pin} = PD13
// - H1 is the status of {MOT_HALL1_GPIO_Port, MOT_HALL1_Pin} = PD12
// because it reads the whole status of the port D, mask the bits 14, 13, 12 and then shifts down by 12 positions
// so: it is correct but we read a byte where bit0 is H1, bit 1 is H2 and bit 2 is H3

/* Shorthand */
#define _U_     PWM_PHASE_U
#define _V_     PWM_PHASE_V
#define _W_     PWM_PHASE_W


/* Extenal variables **************************************************************************************************/

/* Declared in main.h */
extern TIM_HandleTypeDef HALL_TIM;


/* Private typedefs ***************************************************************************************************/

/* Private variables **************************************************************************************************/

static volatile uint16_t HallCapture;
static volatile uint8_t HallStatus;
static volatile int32_t HallPwmValue;


/* Local functions ****************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  This function configures the PHASE outputs as in the following table
 * @param  pwm  Value to be loaded in PWM generator. Positive values are for clockwise torque
 * @return void
 *
 * |                   ||         CW           ||        CCW           |
 * | HALL3 HALL2 HALL1 || PHASEU PHASEV PHASEW || PHASEU PHASEV PHASEW |
 * |-------------------++----------------------++----------------------|
 * |   1     0     0   ||  HIZ    LOW    PWM   ||  HIZ    PWM    LOW   |
 * |   1     0     1   ||  PWM    LOW    HIZ   ||  LOW    PWM    HIZ   |
 * |   0     0     1   ||  PWM    HIZ    LOW   ||  LOW    HIZ    PWM   |
 * |   0     1     1   ||  HIZ    PWM    LOW   ||  HIZ    LOW    PWM   |
 * |   0     1     0   ||  LOW    PWM    HIZ   ||  PWM    LOW    HIZ   |
 * |   1     1     0   ||  LOW    HIZ    PWM   ||  PWM    HIZ    LOW   |
 * |-------------------||----------------------||----------------------|
 * |   0     0     0   ||  HIZ    HIZ    HIZ   ||  HIZ    HIZ    HIZ   | ERROR
 * |   1     1     1   ||  HIZ    HIZ    HIZ   ||  HIZ    HIZ    HIZ   | ERROR
 *
 * where:
 *      HIZ means that PHASEx is in high impedanace state (ENx = 0, PWMx = 0)
 *      LOW means that PHASEx is in LOW state (ENx = 1, PWMx = 0)
 *      PWM means that PHASEx is modulated with the pwm value (ENx = 1, PWMx = pwm)
 */
static void HallSetField(int32_t pwm)
{
    /* Read Hall sensor status */
    HallStatus = HALL_INPUT();
    /* DEBUG */
    if (HallStatus & 0x01) LedOnOff(DL3, 0); else LedOnOff(0, DL3);
    if (HallStatus & 0x02) LedOnOff(DL4, 0); else LedOnOff(0, DL4);
    if (HallStatus & 0x04) LedOnOff(DL8, 0); else LedOnOff(0, DL8);
    /* Clockwise torque */
    if (0 <= pwm)
    {
        switch (HallStatus)
        {
        case 0b100:
            /* Set outputs HIZ LOW PWM */
            PwmPhaseEnable( 0 |_V_|_W_);
            PwmPhaseSet   ( 0 , 0 ,pwm);
            break;
        case 0b101:
            /* Set outputs PWM LOW HIZ */
            PwmPhaseEnable(_U_|_V_| 0 );
            PwmPhaseSet   (pwm, 0 , 0 );
            break;
        case 0b001:
            /* Set outputs PWM HIZ LOW */
            PwmPhaseEnable(_U_| 0 |_W_);
            PwmPhaseSet   (pwm, 0 , 0 );
            break;
        case 0b011:
            /* Set outputs HIZ PWM LOW */
            PwmPhaseEnable( 0 |_V_|_W_);
            PwmPhaseSet   ( 0 ,pwm, 0 );
            break;
        case 0b010:
            /* Set outputs LOW PWM HIZ */
            PwmPhaseEnable(_U_|_V_| 0 );
            PwmPhaseSet   ( 0 ,pwm, 0 );
            break;
        case 0b110:
            /* Set outputs LOW HIZ PWM */
            PwmPhaseEnable(_U_| 0 |_W_);
            PwmPhaseSet   ( 0 , 0 ,pwm);
            break;
        case 0b000:
        case 0b111:
            /* Set outputs HIZ HIZ HIZ */
            PwmPhaseEnable( 0 | 0 | 0 );
            PwmPhaseSet   ( 0 , 0 , 0 );
            break;
        }
    }
    /* Counter-clockwise torque */
    else
    {
        pwm = -pwm;
        switch (HallStatus)
        {
        case 0b100:
            /* Set outputs HIZ PWM LOW */
            PwmPhaseEnable( 0 |_V_|_W_);
            PwmPhaseSet   ( 0 ,pwm, 0 );
            break;
        case 0b101:
            /* Set outputs LOW PWM HIZ */
            PwmPhaseEnable(_U_|_V_| 0 );
            PwmPhaseSet   ( 0 ,pwm, 0 );
            break;
        case 0b001:
            /* Set outputs LOW HIZ PWM */
            PwmPhaseEnable(_U_| 0 |_W_);
            PwmPhaseSet   ( 0 , 0 ,pwm);
            break;
        case 0b011:
            /* Set outputs HIZ LOW PWM */
            PwmPhaseEnable( 0 |_V_|_W_);
            PwmPhaseSet   ( 0 , 0 ,pwm);
            break;
        case 0b010:
            /* Set outputs PWM LOW HIZ */
            PwmPhaseEnable(_U_|_V_| 0 );
            PwmPhaseSet   (pwm, 0 , 0 );
            break;
        case 0b110:
            /* Set outputs PWM HIZ LOW */
            PwmPhaseEnable(_U_| 0 |_W_);
            PwmPhaseSet   (pwm, 0 , 0 );
            break;
        case 0b000:
        case 0b111:
            /* Set outputs HIZ HIZ HIZ */
            PwmPhaseEnable( 0 | 0 | 0 );
            PwmPhaseSet   ( 0 , 0 , 0 );
            break;
        }
    }
}
 
 
/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   The TIM4 capture channel 1 interrupt handler calls this "callback" function every time the hardware
 *          detects a change in the status of the Hall sensors.
 * @param   *htim   Handler of TIM4 (unused)
 * @return  void
 */
static void HallCapture_cb(TIM_HandleTypeDef *htim)
{
#if defined(MOTORHAL_changes) && !defined(MOTORHALCONFIG_enabletests) 
    HallCapture = HAL_TIM_ReadCapturedValue(&HALL_TIM, TIM_CHANNEL_1);
    HallStatus = HALL_INPUT();
#else    
    /* Read the capture register (speed) */
    HallCapture =  HAL_TIM_ReadCapturedValue(&HALL_TIM, TIM_CHANNEL_1);
    /* Update the rotating field */
    HallSetField(HallPwmValue);
#endif    
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void HallSetPrescaler(uint16_t Prescaler)
{
    HALL_TIM.Instance->PSC = Prescaler;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void HallSetPwm(int32_t pwm)
{
    taskENTER_CRITICAL();
    HallSetField(HallPwmValue = pwm);
    taskEXIT_CRITICAL();
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
HAL_StatusTypeDef HallInit(void)
{
    /* Stop counter */
    HAL_TIM_Base_Stop(&HALL_TIM);
    /* Stop interrupt */
    HAL_TIM_IC_Stop(&HALL_TIM, TIM_CHANNEL_1);
    /* Register the callback function for CH1 capture event */
    if (HAL_OK == HAL_TIM_RegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID, HallCapture_cb))
    {
        /* Start counter */
        if (HAL_OK == HAL_TIM_Base_Start(&HALL_TIM))
        {
            /* Zero amplitude field */
            HallSetPwm(0);
            /* Start channel 1 in interrupt mode */
            if (HAL_OK == HAL_TIM_IC_Start_IT(&HALL_TIM, TIM_CHANNEL_1)) return HAL_OK;
            /* Stop counter */
            HAL_TIM_Base_Stop(&HALL_TIM);
            /* Disable all */
            PwmPhaseEnable(PWM_PHASE_NONE);
        }
        /* Remove the callback function */
        HAL_TIM_UnRegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
    }
    /* Errors detected */
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void HallDeInit(void)
{
    /* Stop counter */
    HAL_TIM_Base_Stop(&HALL_TIM);
    /* Stop interrupt */
    HAL_TIM_IC_Stop(&HALL_TIM, TIM_CHANNEL_1);
    /* Remove the callback function */
    HAL_TIM_UnRegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
}

#if defined(MOTORHALCONFIG_DONTUSE_TESTS)
void HallTest(void) {}
#else

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void HallTest(void)
{
    char buff[16];
    const char *curs;
    int32_t pwm, set;
    if ((HAL_OK == PwmInit()) && (HAL_OK == HallInit()) && (HAL_OK == EncInit()))
    {
        coprintf("Hall sensors test started\n");
        HallSetPwm(pwm = 0);
        while (1)
        {
            coprintf("pwm? ");
            coEditString(buff, sizeof(buff));
            coPutChar('\n');
            curs = &buff[0];
            if ('\0' == skipblank(&curs)) break;
            if (atosl(&curs, &set) && ('\0' == skipblank(&curs)))
            {
                if ((-65536L <= set) && (65535 >= set))
                {
                    if (set >= pwm)
                    {
                        for ( ; pwm < set-100 ; pwm += 100) 
                        {
                            HallSetPwm(pwm);
                            HAL_Delay(10);
                        }
                    }
                    else
                    {
                        for ( ; pwm > set-100 ; pwm -= 100) 
                        {
                            HallSetPwm(pwm);
                            HAL_Delay(10);
                        }
                    }
                    HallSetPwm(pwm = set);
                }
                else coprintf("Out of range: %d\n", pwm);
            }
            else if (tokencmp("encoder", &curs))
            {
                EncCaptureIndex();
                do
                {
                    coprintf("\r %+011d ", EncGetAbsolutePosition());
                    HAL_Delay(100);
                } while (!coRxReady() || ('\r' != coRxChar()));
                coPutChar('\n');
            }
            else coprintf("Syntax error: %s\n", curs);
        }
        HallSetPwm(0);
        coprintf("Hall sensors test terminated\n");
    }
    EncDeInit();
    HallDeInit();
    PwmDeInit();
}

#endif // #if defined(MOTORHALCONFIG_DONTUSE_TESTS)



#if defined(MOTORHAL_changes)

uint8_t hall_INPUT() { return HALL_INPUT(); }

#endif // #if defined(MOTORHAL_changes)


/* END OF FILE ********************************************************************************************************/
