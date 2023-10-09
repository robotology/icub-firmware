/*******************************************************************************************************************//**
 * @file    pwm.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 17
 * @brief   PWM management
 *
 **********************************************************************************************************************/
/*
 * PWM outputs are controlled by TIM1 configured in CENTER-ALIGNED-PWM mode.
 * TIM1 is connected to the APB2 bus and has an internal clock frequency of 200 MHz. 
 * TIM1_ARR register is loaded with value 1024 hence, given the CENTER-ALIGNED mode, the total clock division is 2048,
 * giving a PWM frequency of 200MHz/2048, or about 97.66 kHz, with a time resolution of 10 ns.
 */
 
// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif
 
#include "embot_core.h"
 
#if defined(USE_STM32HAL) 
// API
#include "pwm.h"
#endif
 
#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
/* Includes ***********************************************************************************************************/
#include "main.h"
#include "console.h"
#include "led.h"
#include "pwm.h"
#include "adcm.h"
#include "utilities.h"
#include "stm32h7xx_hal_tim.h"
#include "FreeRTOS.h"
#include "task.h"
#endif // #if defined(USE_STM32HAL)

/* External variables *************************************************************************************************/

#if defined(MOTORHAL_changes)

#include "embot_hw_motor_bsp_amc2c.h"
#define htim1 (embot::hw::motor::bsp::amc2c::hTIM1)

#else

/* Declared in main.c */
extern TIM_HandleTypeDef htim1;

#endif //

/* Local macros *******************************************************************************************************/
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
// do not enable the triggering of the external fault
#define USE_BREAK_INPUT     0
#else
/* Set to 1 to use emergency BREAK */
#define USE_BREAK_INPUT     0
#endif

// marco.accame: between [0, 1023], where original value is in [0, 64k) and use nearest integer
// using (p+32)/64
/* This macro reduce a PWM unsigned value to the value to be loaded in TIM1_CCRx registers */
#define PWM_COMPARE_VALUE(Pwm)  ((((unsigned)(Pwm))>=0xFFC0U)?0x03FFU:((Pwm+0x0020U)>>6u))


/* Local typedefs *****************************************************************************************************/

/* PWM phase values in timer count units */
static volatile uint16_t PwmComparePhaseU = 0;
static volatile uint16_t PwmComparePhaseV = 0;
static volatile uint16_t PwmComparePhaseW = 0;


/* Local variables ****************************************************************************************************/

/* Public Variables ***************************************************************************************************/

/* Local functions ****************************************************************************************************/

/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Called by TIM1 interrupt handler after an update event (start of a new PWM cycle)
 * @param   phtim   Not used
 * @return  void
 */
//static void PwmUpdateEvent_cb(TIM_HandleTypeDef *phtim)
//{
//    /* Update the compare registers of the PWM counter */
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PwmComparePhaseU);
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PwmComparePhaseV);
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PwmComparePhaseW);
//}

#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
#else
/*******************************************************************************************************************//**
 * @brief   Called by TIM1 interrupt handler after a BREAK event (hardware failure)
 * @param   phtim   Not used
 * @return  void
 */
static void PwmBreakEvent_cb(TIM_HandleTypeDef *phtim)
{
    PwmPhaseEnable(PWM_PHASE_NONE);
    /* Clear the compare registers of the PWM counter */
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PwmComparePhaseU = 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PwmComparePhaseV = 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PwmComparePhaseW = 0);
    /* Disable interrupt */
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_BREAK);
    /*DEBUG*/LedOnOff(DL8, 0);
}
#endif



/* Public functions ***************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  
 * @param  
 * @return void
 */
void PwmPhaseSet(uint16_t u, uint16_t v, uint16_t w)
{
    /* Reduce PWM values in counter units */
//    u = PWM_COMPARE_VALUE(u);
//    v = PWM_COMPARE_VALUE(v);
//    w = PWM_COMPARE_VALUE(w);


    /* Must be interrupt-safe */
//    taskDISABLE_INTERRUPTS();
//    PwmComparePhaseU = (u<1219) ? (u) : (1219);
//    PwmComparePhaseV = (v<1219) ? (v) : (1219);
//    PwmComparePhaseW = (w<1219) ? (w) : (1219);
//    taskENABLE_INTERRUPTS();

    // we remove from here the check vs max value because we assume that the caller has already done it 
    PwmComparePhaseU = u;
    PwmComparePhaseV = v;
    PwmComparePhaseW = w;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PwmComparePhaseU);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PwmComparePhaseV);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PwmComparePhaseW);    
}


/*******************************************************************************************************************//**
 * @brief   Enable/Disable the PWM driver outputs
 * @param   mask    Any combination of the following values
 *                  PWM_PHASE_U
 *                  PWM_PHASE_V
 *                  PWM_PHASE_W
 * @return  void
 *
 * WARNING: this function assumes that all the driver enable pins are in the same GPIO port. Check schematics
 */
void PwmPhaseEnable(uint16_t mask)
{
    MOT_EN1_GPIO_Port->BSRR =  ((uint32_t)mask & PWM_PHASE_ALL)             /* Set mask */
                            | (((uint32_t)~mask & PWM_PHASE_ALL) << 16);    /* Reset mask */
}


/*******************************************************************************************************************//**
 * @brief   Enter or Exit the SLEEP mode of the motor driver
 * @param   enable  It can assume one of the following values:
 *                  ENABLE      Motor driver in SLEEP mode: Power dissipation is limited but PWM is not allowed
 *                  DISABLE     Motor driver in active mode: PWM generation is allowed.
 * @return  void
 */
void PwmSleep(FunctionalState enable)
{
    /* SET or RESET the MOT_nSLEEP signal as required */
    HAL_GPIO_WritePin(MOT_nSLEEP_GPIO_Port, MOT_nSLEEP_Pin, (ENABLE==enable)?GPIO_PIN_RESET:GPIO_PIN_SET);
}
    

/*******************************************************************************************************************//**
 * @brief   Assert or release the RESET signal of the driver. It can be used to clear any fault condition
 * @param   enable  It can assume one of the following values:
 *                  ENABLE      Assert motor driver RESET
 *                  DISABLE     Release motor driver RESET
 * @return  void
 */
void PwmReset(FunctionalState enable)
{
    /* SET or RESET the MOT_nRESET signal as required */
    HAL_GPIO_WritePin(MOT_nRESET_GPIO_Port, MOT_nRESET_Pin, (ENABLE==enable)?GPIO_PIN_RESET:GPIO_PIN_SET);
}


/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  Function result
 */
HAL_StatusTypeDef PwmResetFault(void)
{
    PwmPhaseSet(0, 0, 0);
    PwmSleep(ENABLE); HAL_Delay(10);
    PwmSleep(DISABLE); HAL_Delay(10);
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
    #warning TODO: manage that
#endif    
    if (GPIO_PIN_SET == HAL_GPIO_ReadPin(MOT_nFAULT_GPIO_Port, MOT_nFAULT_Pin))
    {
        __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_BREAK);
        __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_BREAK);
        __HAL_TIM_MOE_ENABLE(&htim1); HAL_Delay(10);
        __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_BREAK);
        PwmPhaseEnable(PWM_PHASE_ALL);
        return HAL_OK;
    }
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   Test the fault status of the driver
 * @param   void
 * @return  HAL_OK      FAULT signal not active
 *          HAL_ERROR   FAULT signal active
 */
HAL_StatusTypeDef PwmFaultStatus(void)
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(MOT_nFAULT_GPIO_Port, MOT_nFAULT_Pin))?HAL_OK:HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   Complete the CubeMx TIM1 configuration for the PWM subsystem
 * @param   void
 * @return  void
 */
HAL_StatusTypeDef PwmInit(void)
{
    /* Stop any operation in progress */
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
//    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
#else    
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_BREAK);
#endif    
    HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_3);
    HAL_TIM_Base_Stop_IT(&htim1);

#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
    htim1.Instance->BDTR &= ~TIM_BDTR_BKE;
    htim1.Instance->AF1 &= ~TIM1_AF1_BKINE;
#else
    
#if USE_BREAK_INPUT != 1
    htim1.Instance->BDTR &= ~TIM_BDTR_BKE;
    htim1.Instance->AF1 &= ~TIM1_AF1_BKINE;
#endif

#endif
    
    /* Update the compare registers of the PWM counter */
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PwmComparePhaseU = 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PwmComparePhaseV = 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PwmComparePhaseW = 0);
   
    /* Register the TIM1 callback functions and start the PWM generator */
    if (
//        (HAL_OK == HAL_TIM_RegisterCallback(&htim1, HAL_TIM_PERIOD_ELAPSED_CB_ID, PwmUpdateEvent_cb)) &&
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
#else    
        (HAL_OK == HAL_TIM_RegisterCallback(&htim1, HAL_TIM_BREAK2_CB_ID, PwmBreakEvent_cb)) &&
        (HAL_OK == HAL_TIM_RegisterCallback(&htim1, HAL_TIM_BREAK_CB_ID, PwmBreakEvent_cb)) &&
#endif    
        (HAL_OK == HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1)) && 
        (HAL_OK == HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_2)) &&
        (HAL_OK == HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_3)) &&
        (HAL_OK == HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_6)) &&
        (HAL_OK == HAL_TIM_Base_Start_IT(&htim1)))
    {
        /* Enable the DRV8313RHH */
        PwmReset(DISABLE);
        PwmSleep(DISABLE);
        /* Enable Interrupts */
//#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
//        __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
//#else        
        __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_BREAK); 
//#endif         
        return HAL_OK;
    }

    /* Abort */
    PwmDeInit();
    /* Error(s) detected */
    return HAL_ERROR;
}

/*******************************************************************************************************************//**
 * @brief   Return the PWM subsystem to the not initialized state
 * @param   void
 * @return  void
 */
void PwmDeInit(void)
{
    PwmPhaseEnable(PWM_PHASE_NONE);
    PwmReset(ENABLE);
    PwmSleep(ENABLE);
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
//     __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
#else    
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_BREAK);
#endif    
    HAL_TIM_Base_Stop(&htim1);
    HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_6);
    HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_PERIOD_ELAPSED_CB_ID);
#if defined(MOTORHALCONFIG_MOT_BREAK_IRQ_remove)
#else    
    HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_BREAK_CB_ID);
    HAL_TIM_UnRegisterCallback(&htim1, HAL_TIM_BREAK2_CB_ID);
#endif    
}

#if defined(MOTORHALCONFIG_DONTUSE_TESTS)
void PwmTest(void) {}
#else

/*******************************************************************************************************************//**
 * @brief   
 * @param   void
 * @return  void
 */
void PwmTest(void)
{
    char CmdLine[80];
    const char *curs;
    uint32_t uu, vv, ww;
    if (HAL_OK == AdcMotInit())
    {
        if (HAL_OK == PwmInit())
        {
            coprintf("PWM test started\n");
            coprintf("Current sensors offsets: %u, %u, %u\n", 
                     AdcMotGetOffset(ADCMOT_PHASE_U),  AdcMotGetOffset(ADCMOT_PHASE_V),  AdcMotGetOffset(ADCMOT_PHASE_W));
            PwmPhaseEnable(PWM_PHASE_ALL);
            while (1)
            {
                coPutString("PWM? ");
                coEditString((char *)CmdLine, sizeof(CmdLine)); coPutChar('\n');
                curs = CmdLine;
                while ('\0' != skipblank(&curs))
                {
                    if (atoul(&curs, &uu) && charcmp(',', &curs) && (uu < 65536) &&
                        atoul(&curs, &vv) && charcmp(',', &curs) && (vv < 65536) &&
                        atoul(&curs, &ww) && (ww < 65536))
                    {
                        coprintf("Setting PWM=[%u,%u,%u]\n", uu, vv, ww);
                        PwmPhaseSet(uu, vv, ww);
                    }
                    else if (tokencmp("reset", &curs))
                    {
                        if (HAL_OK == PwmResetFault())
                        {
                            /* Enable Interrupts */
                            __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE|TIM_IT_BREAK);
                            coprintf("FAULT indication cleared\n");
                            /*DEBUG*/LedOnOff(0, DL8);
                        }
                        else coprintf("Cannot clear FAULT indication signal\n");
                    }
                    else if (tokencmp("x", &curs))
                    {
                        coprintf("Setting PWM=[0,0,0]\n");
                        PwmPhaseSet(0, 0, 0);
                    }
                    else if (tokencmp("exit", &curs))
                    {
                        coprintf("PWM test terminated\n");
                        PwmPhaseSet(0,0,0);
                        PwmPhaseEnable(PWM_PHASE_NONE);
                        AdcMotDeInit();
                        PwmDeInit();
                        return;
                    }
                    else if (tokencmp("adc", &curs))
                    {
                        int16_t uI, vI, wI;
                        uint16_t uV, vV, wV;
                        do
                        {
                            AdcMotGetCurrents(&uI, &vI, &wI);
                            AdcMotGetVoltages(&uV, &vV, &wV);
                            coprintf("\r%+06d/%05u, %+06d/%05u, %+06d/%05u ", uI, uV, vI, vV, wI, wV);  
                            HAL_Delay(100);
                        } while (!coRxReady() || ('\r' != coRxChar()));
                        coPutChar('\n');
                    }
                    else if (tokencmp("trg", &curs) && atoul(&curs, &uu) && ('\0' == skipblank(&curs)) && (1024 > uu))
                    {
                        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_6, uu);
                        coprintf("ADC trigger level set to %u\n", uu);
                    }
                    else
                    {
                        coprintf(">>> Syntax error: %s\n", curs);
                        break;
                    }
                }
                if (HAL_OK != PwmFaultStatus()) coprintf("WARNING: MOT_nFAULT asserted\n");
            }
        }
        else coprintf(">>> PwmInit ERROR\n");
        AdcMotDeInit();
    }
    else coprintf(">>> adcMotInit ERROR\n");
}

#endif // #if defined(MOTORHALCONFIG_DONTUSE_TESTS)


#if defined(MOTORHAL_changes)


#endif // #if defined(MOTORHAL_changes)


/* END OF FILE ********************************************************************************************************/

