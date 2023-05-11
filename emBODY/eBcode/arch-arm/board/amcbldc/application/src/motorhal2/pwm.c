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

#include "embot_core.h"

#if defined(USE_STM32HAL) 
// API
#include "pwm.h"
#include "motorhal_faults.h"
#endif

/* Includes ***********************************************************************************************************/
#if defined(USE_STM32HAL)
#include "stm32hal.h"
#if !defined(MOTORHALCONFIG_DONTUSE_UTILITIES)
#include "utilities.h"
#endif
#if !defined(MOTORHALCONFIG_DONTUSE_FLASH)
#include "flash.h"
#endif
#if !defined(MOTORHALCONFIG_DONTUSE_CONSOLE)
 #include "console.h"
#endif
#if !defined(MOTORHALCONFIG_DONTUSE_LED)
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
#include "task.h"
#include "main.h"
#include "stm32g4xx_hal_tim.h"
#endif

#if (USE_HAL_TIM_REGISTER_CALLBACKS != 1)
    #error Flag TIM1 in menu "Project Manager -> Advanced Settings -> Register CallBack" of CubeMx must be ENABLED
#endif

#define USE_HALL_SENSORS    1
//#define USE_ENCODER_SENSORS 1

#if !defined(USE_ENCODER_SENSORS)
#warning USE_ENCODER_SENSORS is not defined 
#endif

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
static volatile int16_t  hallCurrent = 0;
#if defined(MOTORHAL_changes)
#warning evalute how to manage hallFilteredCurrent which uses 4104 bytes of RAM
#endif
analogAvgFilterTypeDef hallFilteredCurrent;


#if defined(MOTORHAL_changes) && defined(__cplusplus)

constexpr int16_t numHallSectors = 6;
constexpr float iCubDeg = 65536.0; // Range of angular values that can be represented with the encoder
constexpr float Deg2iCub = iCubDeg / 360.0;
constexpr int16_t hallAngleStep = 60.0 * Deg2iCub; // 60 degrees scaled by the range of possible values
constexpr int16_t minHallAngleDelta = 30.0 * Deg2iCub; // 30 degrees scaled by the range of possible values

constexpr uint16_t hallAngleTable[] =
{
    /* ABC  (°)  */
    /* LLL ERROR */ 0,
    /* LLH  240  */ static_cast<uint16_t>(240.0 * Deg2iCub), /* 43690 */
    /* LHL  120  */ static_cast<uint16_t>(120.0 * Deg2iCub), /* 21845 */
    /* LHH  180  */ static_cast<uint16_t>(180.0 * Deg2iCub), /* 32768 */
    /* HLL    0  */ static_cast<uint16_t>(  0.0 * Deg2iCub), /*     0 */
    /* HLH  300  */ static_cast<uint16_t>(300.0 * Deg2iCub), /* 54613 */
    /* HHL   60  */ static_cast<uint16_t>( 60.0 * Deg2iCub), /* 10922 */
    /* HHH ERROR */ static_cast<uint16_t>(0)
};

//constexpr int16_t hallSectorTable[] =
//{
//    /* ABC  (°)  */
//    /* LLL ERROR */ static_cast<int16_t>(0),
//    /* LLH  240  */ static_cast<int16_t>(4), /* 43690 */
//    /* LHL  120  */ static_cast<int16_t>(2), /* 21845 */
//    /* LHH  180  */ static_cast<int16_t>(3), /* 32768 */
//    /* HLL    0  */ static_cast<int16_t>(0), /*     0 */
//    /* HLH  300  */ static_cast<int16_t>(5), /* 54613 */
//    /* HHL   60  */ static_cast<int16_t>(1), /* 10922 */
//    /* HHH ERROR */ static_cast<int16_t>(0)
//};

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

#endif // #if defined(MOTORHAL_changes) && defined(__cplusplus)


#if defined(MOTORHAL_changes)

// --------------------------------------------------------------------------------------------------------------------
// add in here all the new types, variables, static function prototypes required by MOTORHAL
// --------------------------------------------------------------------------------------------------------------------


static volatile uint8_t  s_pwm_hallStatus_old = 0;
static volatile uint8_t  s_pwm_hallOrder[3] = {0, 0, 0};

static volatile bool s_pwm_calibrating = true;

//struct pwm_adc_callback_set_T
//{
//    void (*callback_fn)(void *owner, int16_t Iuvw[3], void* rtu, void* rty);
//    void *owner;
//    void* rtu;
//    void* rty;
//};

//static volatile pwm_adc_callback_set_T s_pwm_adc_callback_set =
//{
//    NULL, NULL, NULL, NULL    
//};

static void s_pwm_dummy_adc_callback(void *owner, const pwm_Currents_t * const currents) {}
static pwm_Currents_t s_pwm_dummy_currents = {0};
    
static volatile pwm_ADC_callback_t s_pwm_ADC_cbk = 
{
    .callback = s_pwm_dummy_adc_callback,
    .owner = NULL
};


static uint8_t s_pwm_updateHallStatus(void);

#endif // #if defined(MOTORHAL_changes)

/* Public variables ***************************************************************************************************/
/* Private functions **************************************************************************************************/

#if defined(MOTORHAL_changes)

// we dont need: static void hallSetPWM(int16_t pwm)
// but a s_pwm_updateHallStatus()

#else

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
    /* Remeber previous value */
    uint8_t hall = hallStatus;
    
    /* Read current value of HALL1, HALL2 and HALL3 signals in bits 2..0 */
    hallStatus = (((HALL1_GPIO_Port->IDR & HALL1_Pin) >> MSB(HALL1_Pin)) << 2)
               | (((HALL2_GPIO_Port->IDR & HALL2_Pin) >> MSB(HALL2_Pin)) << 1)
               | (((HALL3_GPIO_Port->IDR & HALL3_Pin) >> MSB(HALL3_Pin)) << 0);

    /* CW torque */
    if (pwm >= 0)
    {
        switch(hallStatus)
        {
        case 0x05:
            /* PHASE1=pwm, PHASE2=-pwm, PHASE3=hiz */
            EN1_GPIO_Port->BSRR = EN1_Pin | EN2_Pin | (EN3_Pin<<16) ;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            /* Verify previous HALL sensors status */
            if      (hall == 0x01) ++hallCounter;
            else if (hall == 0x04) --hallCounter;
            break;

        case 0x04:
            /* PHASE1=pwm, PHASE2=hiz, PHASE3=-pwm */
            EN1_GPIO_Port->BSRR = EN1_Pin | (EN2_Pin<<16) | EN3_Pin ;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            /* Verify previous HALL sensors status */
            if      (hall == 0x05) ++hallCounter;
            else if (hall == 0x06) --hallCounter;
            break;

        case 0x06:
            /* PHASE1=hiz, PHASE2=pwm, PHASE3=-pwm */
            EN1_GPIO_Port->BSRR = (EN1_Pin<<16) | EN2_Pin | EN3_Pin;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pwm);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            /* Verify previous HALL sensors status */
            if      (hall == 0x04) ++hallCounter;
            else if (hall == 0x02) --hallCounter;
            break;

        case 0x02:
            /* PHASE1=-pwm, PHASE2=pwm, PHASE3=hiz */
            EN1_GPIO_Port->BSRR = EN1_Pin | EN2_Pin | (EN3_Pin<<16) ;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pwm);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            /* Verify previous HALL sensors status */
            if      (hall == 0x06) ++hallCounter;
            else if (hall == 0x03) --hallCounter;
            break;

        case 0x03:
            /* PHASE1=-pwm, PHASE2=hiz, PHASE3=pwm */
            EN1_GPIO_Port->BSRR = EN1_Pin | (EN2_Pin<<16) | EN3_Pin ;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pwm);
            /* Verify previous HALL sensors status */
            if      (hall == 0x02) ++hallCounter;
            else if (hall == 0x01) --hallCounter;
            break;

        case 0x01:
            /* PHASE1=hiz, PHASE2=-pwm, PHASE3=pwm */
            EN1_GPIO_Port->BSRR = (EN1_Pin<<16) | EN2_Pin | EN3_Pin;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pwm);
            /* Verify previous HALL sensors status */
            if      (hall == 0x03) ++hallCounter;
            else if (hall == 0x05) --hallCounter;
            break;

        default:
            /* ERROR: forbitten combination */
            ledSet(LED2, 0);
            EN1_GPIO_Port->BSRR = ((EN3_Pin|EN2_Pin|EN1_Pin)<<16);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            pwm = 0;
            break;
        }
    }
    /* CCW torque */
    else
    {
        /* Current hall sensors status */
        switch (hallStatus)
        {
            case 0x05:
                /* PHASE1=pwm, PHASE2=-pwm, PHASE3=hiz */
                EN1_GPIO_Port->BSRR = EN1_Pin | EN2_Pin | (EN3_Pin<<16) ;
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, -pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                /* Verify previous HALL sensors status */
                if      (hall == 0x01) ++hallCounter;
                else if (hall == 0x04) --hallCounter;
                break;

            case 0x04:
                /* PHASE1=pwm, PHASE2=hiz, PHASE3=-pwm */
                EN1_GPIO_Port->BSRR = EN1_Pin | (EN2_Pin<<16) | EN3_Pin ;
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, -pwm);
                /* Verify previous HALL sensors status */
                if      (hall == 0x05) ++hallCounter;
                else if (hall == 0x06) --hallCounter;
                break;

            case 0x06:
                /* PHASE1=hiz, PHASE2=pwm, PHASE3=-pwm */
                EN1_GPIO_Port->BSRR = (EN1_Pin<<16) | EN2_Pin | EN3_Pin;
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, -pwm);
                /* Verify previous HALL sensors status */
                if      (hall == 0x04) ++hallCounter;
                else if (hall == 0x02) --hallCounter;
                break;

            case 0x02:
                /* PHASE1=-pwm, PHASE2=pwm, PHASE3=hiz */
                EN1_GPIO_Port->BSRR = EN1_Pin | EN2_Pin | (EN3_Pin<<16) ;
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, -pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                /* Verify previous HALL sensors status */
                if      (hall == 0x06) ++hallCounter;
                else if (hall == 0x03) --hallCounter;
                break;

            case 0x03:
                /* PHASE1=-pwm, PHASE2=hiz, PHASE3=pwm */
                EN1_GPIO_Port->BSRR = EN1_Pin | (EN2_Pin<<16) | EN3_Pin ;
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, -pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                /* Verify previous HALL sensors status */
                if      (hall == 0x02) ++hallCounter;
                else if (hall == 0x01) --hallCounter;
                break;

            case 0x01:
                /* PHASE1=hiz, PHASE2=-pwm, PHASE3=pwm */
                EN1_GPIO_Port->BSRR = (EN1_Pin<<16) | EN2_Pin | EN3_Pin;
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, -pwm);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                /* Verify previous HALL sensors status */
                if      (hall == 0x03) ++hallCounter;
                else if (hall == 0x05) --hallCounter;
                break;

            default:
                /* ERROR: forbitten combination */
                ledSet(LED2, 0);
                EN1_GPIO_Port->BSRR = ((EN3_Pin|EN2_Pin|EN1_Pin)<<16);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
                pwm = 0;
                break;
        }
    }

    /* Update pwm status */
    pwmStatus = pwm;

    /* Update angle */
    hallAngle = hallAngleTable[hallStatus];
}


#endif // #if defined(MOTORHAL_changes)



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
        
        motorhal_set_fault(OverCurrentFailure);
        
#if defined(MOTORHAL_changes)
#else
        /* Null output voltage */
        pwmStatus = 0;
        /* Advertise the fault */
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
#if defined(MOTORHAL_changes)        
        s_pwm_updateHallStatus();
#else
        /* Generate the correct PWM configuration following the pwmStatus pattern */
        hallSetPWM(pwmStatus);
#endif        
    }
}

#if defined(MOTORHAL_changes)

void pwmSetCurrents_cb(int16_t i1, int16_t i2, int16_t i3)
{   
    static pwm_Currents_t currents = {0};
    // as soon as possible: copy the currents (must be converted from raw as i1, i2, i3 come from ADC)
    currents.u = analogConvertCurrent(i1); 
    currents.v = analogConvertCurrent(i2); 
    currents.w = analogConvertCurrent(i3); 
        
    // so far, calibration is done only at startup of current acquisition and not at runtime   
    if (s_pwm_calibrating)
    {
        // we must call the filtering of {1, i2, i3} at least for ANALOG_AVG_FILTER_LENGTH times.
        // because ... we want to load the full filter so that ANALOG_AVG_FILTER_LENGTH values are accumulated
        // correctly before we can get the average value with the division.          
        constexpr uint16_t maxIter {1024}; // {ANALOG_AVG_FILTER_LENGTH + 10};  // was 1000
        static uint16_t counter = 0;
    
        if (counter < maxIter)
        {
            ++counter;
#if defined(MOTORHALCONFIG_DONTUSE_RUNTIMECURR_FILTERING)
            // give the values to the filters
            analog_MovingAverage(i1, i2, i3);
#else
            //  dont do filtering because it will be done inside adc2_HalfTransferComplete_cb() 
            //  and adc2_TransferComplete_cb() 
#endif
        }
        else
        {
            // never mind if we have macro MOTORHALCONFIG_DONTUSE_RUNTIMECURR_FILTERING defined or not   
            // now we surely have filtered values of currents, so we can use
            // their raw values to impose their offsets
            analogSetOffsetIph1(analog_RawCph1() + analogGetOffsetIph1());
            analogSetOffsetIph2(analog_RawCph2() + analogGetOffsetIph2());
            analogSetOffsetIph3(analog_RawCph3() + analogGetOffsetIph3());             

            // and we dont need to calibrate anymore.
            s_pwm_calibrating = false;
            counter = 0;
            //if we set counter = 0 now, we can maybe decide to start / restart a runtime calibration 
            // by imposing s_pwm_calibrating = true inside some extern function
        }
    }
    
    s_pwm_ADC_cbk.callback(s_pwm_ADC_cbk.owner, &currents);    
}

#else
/*******************************************************************************************************************//**
 * @brief   Called by DMA associated to ADC2 whenever a new set of phase currents is available
 * @param   i1, i2, i3  Currents measured in PHASE1, PHASE2 and PHASE3 of the motor
 * @return  void
 */
void pwmSetCurrents_cb(int16_t i1, int16_t i2, int16_t i3)
{
    /* CW torque */
    if (pwmStatus >= 0)
    {
        switch (hallStatus)
        {
            case 0x05:
                /* PHASE1=pwm, PHASE2=low, PHASE3=hiz */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, -analogConvertCurrent(i2+i3)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x04:
                /* PHASE1=pwm, PHASE2=hiz, PHASE3=low */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, -analogConvertCurrent(i2+i3)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x06:
                /* PHASE1=hiz, PHASE2=pwm, PHASE3=low */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, -analogConvertCurrent(i1+i3)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x02:
                /* PHASE1=low, PHASE2=pwm, PHASE3=hiz */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, -analogConvertCurrent(i1+i3)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x03:
                /* PHASE1=low, PHASE2=hiz, PHASE3=pwm */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, -analogConvertCurrent(i1+i2)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x01:
                /* PHASE1=hiz, PHASE2=low, PHASE3=pwm */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, -analogConvertCurrent(i1+i2)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            default:
                /* ERROR: forbitten combination */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, 0);
                break;
        }
    }
    /* CCW torque */
    else
    {
        switch (hallStatus)
        {
            case 0x05:
                /* PHASE1=low, PHASE2=-pwm, PHASE3=hiz */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, analogConvertCurrent(i1+i3)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x04:
                /* PHASE1=low, PHASE2=hiz, PHASE3=-pwm */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, analogConvertCurrent(i1+i2)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x06:
                /* PHASE1=hiz, PHASE2=low, PHASE3=-pwm */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, analogConvertCurrent(i1+i2)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x02:
                /* PHASE1=-pwm, PHASE2=low, PHASE3=hiz */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, analogConvertCurrent(i2+i3)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x03:
                /* PHASE1=-pwm, PHASE2=hiz, PHASE3=low */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, analogConvertCurrent(i2+i3)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            case 0x01:
                /* PHASE1=hiz, PHASE2=-pwm, PHASE3=low */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, analogConvertCurrent(i1+i3)) >> ANALOG_AVG_FILTER_SHIFT;
                break;

            default:
                /* ERROR: forbitten combination */
                hallCurrent = analogMovingAverageFromISR(&hallFilteredCurrent, 0);
                break;
        }
    }
}

#endif // #if defined(MOTORHAL_changes)

/* Exported functions *************************************************************************************************/

#if defined(MOTORHAL_changes)

HAL_StatusTypeDef pwmInit(void)
{
    /* todo: no HALL sensor motor index startup calibration */    
    MainConf.pwm.mode = PWM_CONF_MODE_HALL;
    
    /* Clear any preceeding fault condition */
    pwmReset(ENABLE);
    pwmSleep(DISABLE);
    HAL_Delay(10);
    pwmReset(DISABLE);
    HAL_Delay(10);

//    if (0 == (MainConf.pwm.mode & PWM_CONF_MODE_MASK))
//    {
//        MainConf.pwm.mode = PWM_CONF_MODE_HALL;
//        MainConf.pwm.num_polar_couples = num_polar_couples;
//    }
        
    /* Register the required TIM1 callback functions */
    HAL_TIM_RegisterCallback(&htim1, HAL_TIM_BREAK_CB_ID, pwmMotorFault_cb);
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_BREAK);

    /* Reset the PWM value */
    pwmSet(0, 0 ,0);
    
    //s_pwm_updateHallStatus();

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

#else

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
    vTaskDelay(10);
    pwmReset(DISABLE);
    vTaskDelay(10);

    if (0 == (MainConf.pwm.mode & PWM_CONF_MODE_MASK))
    {
        MainConf.pwm.mode = PWM_CONF_MODE_HALL;
        MainConf.pwm.poles = 7;
    }
        // callback sul fault del motor, non ext fault (slo overcurrent e heat)
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

#endif // #if defined(MOTORHAL_changes)




/*******************************************************************************************************************//**
 * @brief   Complete the CubeMx TIM3 configuration for the PWM subsystem
 * @param   void
 * @return  void
 */
HAL_StatusTypeDef hallInit(void)
{
#if defined(MOTORHAL_changes)
#else
    /* Read value of HALL1, HALL2 and HALL3 signals in bits 2..0 */
    hallStatus = (((HALL1_GPIO_Port->IDR & HALL1_Pin) >> MSB(HALL1_Pin)) << 2)
               | (((HALL2_GPIO_Port->IDR & HALL2_Pin) >> MSB(HALL2_Pin)) << 1)
               | (((HALL3_GPIO_Port->IDR & HALL3_Pin) >> MSB(HALL3_Pin)) << 0);

    /* Init angle */
    hallAngle = hallAngleTable[hallStatus];
#endif // #if defined(MOTORHAL_changes)
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
    return hallCurrent;
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
    pwmReset(ENABLE); 
    vTaskDelay(10);
    pwmReset(DISABLE);
    vTaskDelay(10);
    if (GPIO_PIN_SET == HAL_GPIO_ReadPin(nMFAULT_GPIO_Port, nMFAULT_Pin))
    {
#if defined(MOTORHAL_changes)
        pwmPhaseEnable(PWM_PHASE_ALL);
#else
#ifdef USE_ENCODER_SENSORS
        pwmPhaseEnable(PWM_PHASE_ALL);
#endif
#endif        
        __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_BREAK);
        __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_BREAK);
        __HAL_TIM_MOE_ENABLE(&htim1); vTaskDelay(10);
        __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_BREAK);
        ledSet(0, LED2);
        return HAL_OK;
    }
    ledSet(LED2, 0);
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  void
 */
void pwmSet(uint16_t u, uint16_t v, uint16_t w)
{
#if defined(MOTORHAL_changes)
    if (s_pwm_calibrating) u = v = w = 0;
#endif    
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
#if defined(MOTORHAL_changes)
    return HAL_ERROR;
#else 
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
#endif    
}

#if defined(MOTORHALCONFIG_DONTUSE_TESTS)
void pwmTest(void) {}
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
		coprintf("D - Display HALL and ENCODER status\n"
                 "F - Clear motor FAULT indication\n"
                 "P - Set motor PWM\n"
                 "R - Reset Hall sensor counter\n"
                 "ESC : Exit test\n"
                 "? "); vTaskDelay(250);
        if (isgraph(ch = coRxChar())) coPutChar(ch);
        coPutChar('\n');

		switch (toupper(ch))
        {
            case 'D':
                while (1)
                {
                    coprintf("\rENCODER = %5u, HALL = %+11d, CURRENT=%+5d mA, speed=%+5d rpm ", 
                                encoderGetCounter(), hallGetCounter(), hallGetCurrent(), encoderGetSpeed());
                    vTaskDelay(100);
                    if (coRxReady())
                    {
                        ch = coRxChar();
                        if (('r'==ch)||('R'==ch)) hallSetCounter(0);
                        else if ('\x1B'==ch) break;
                    }
                }
                coprintf("\n");
                break;

            case 'F':
                if (HAL_OK != pwmResetFault())
                {
                    coprintf("ERROR: Cannot clear FAULT indication\n");
                }
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

#endif // MOTORHALCONFIG_DONTUSE_TESTS




#if defined(MOTORHAL_changes)

// --------------------------------------------------------------------------------------------------------------------
// add in here all the new functions required by MOTORHAL
// --------------------------------------------------------------------------------------------------------------------



extern void pwm_set_ADC_callback(pwm_ADC_callback_t *cbk)
{
    if(NULL != cbk)
    {
        s_pwm_ADC_cbk.callback = (NULL != cbk->callback) ? cbk->callback : s_pwm_dummy_adc_callback;
        s_pwm_ADC_cbk.owner = cbk->owner;     
    }
}

//HAL_StatusTypeDef pwm_Deinit(void)
//{
//    pwmPhaseDisable(PWM_PHASE_ALL);
//    
//    return HAL_OK;
//}


HAL_StatusTypeDef pwm_hallDeinit(void)
{
    return HAL_OK;
}

 
#define DECODE_HALLSTATUS (((HALL1_GPIO_Port->IDR & HALL1_Pin) >> MSB(HALL1_Pin)) << s_pwm_hallOrder[0])  \
                        | (((HALL2_GPIO_Port->IDR & HALL2_Pin) >> MSB(HALL2_Pin)) << s_pwm_hallOrder[1])  \
                        | (((HALL3_GPIO_Port->IDR & HALL3_Pin) >> MSB(HALL3_Pin)) << s_pwm_hallOrder[2])

static uint8_t s_pwm_updateHallStatus(void)
{
    static int8_t calibration_step = 0;
    
    static uint16_t border[6]={0};
    static uint8_t border_flag = 0;
    
    /* Read current value of HALL1, HALL2 and HALL3 signals in bits 2..0 */
    hallStatus = DECODE_HALLSTATUS;
    
    if (hallStatus == 0 || hallStatus == 7)
    {
        // this configuration is impossible
        // RAISE ERROR STATE
        motorhal_set_fault(DHESInvalidValue);
        
        hallStatus = s_pwm_hallStatus_old;
    }
    
    s_pwm_hallStatus_old = hallStatus;
    
    constexpr char hallSectorTable[] = {255, 4, 2, 3, 0, 5, 1};
    
    char hallSector = hallSectorTable[hallStatus];
    
    static char hallSector_old = hallSector;
    
    if (hallSector_old != hallSector)
    {
        char updown = (hallSector - hallSector_old + 6) % 6;
        
        if (updown != 1 && updown != 5)
        {
            // this transition is impossible
            // RAISE ERROR STATE
            motorhal_set_fault(DHESInvalidSequence);
            return 0;
        }
        
        hallSector_old = hallSector;
    }
    
    uint16_t angle = MainConf.pwm.hall_offset + hallAngleTable[hallStatus];
    
    // Check which sector between [0 ... 5] the rotor is in
    int16_t sector = ((minHallAngleDelta + angle) / hallAngleStep) % numHallSectors;
    static int16_t sector_old = sector;
    
    hallAngle = angle;
    
    if (MainConf.encoder.resolution == 0) // no encoder
    {
        encoder_Force(angle);
    }
    else
    {
        if (!s_pwm_hallStatus_old)
        {            
            hallCounter = 0;
            encoder_Reset();
            encoder_Force(angle);
        }
        else
        {
            // Check if the motor is rotating forward (counterclockwise)
            bool forward = ((sector-sector_old+numHallSectors)%numHallSectors)==1;
        
            if (forward) // forward
            {
                ++hallCounter;
                angle -= minHallAngleDelta; // -30 deg
            }
            else
            {
                --hallCounter;
                angle += minHallAngleDelta; // +30 deg
            }
             /*
            0) Use the Hall sensors to rotate until the wrap-around border is reached,
             then reset the encoder value
            */
            if (calibration_step == 0)
            {
                encoder_Force(angle);
                
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    encoder_Reset();
                    calibration_step = 1;
                }
            }
            /*
            1) Use the hall sensors to rotate. While rotating, store the encoder angle 
            every time a sector border is crossed. When all 6 borders are crossed,
            compute the offset to apply to the encoder by least squares fitting. After
            the offset is applied set encoderCalibrated to true
            */
            else if (calibration_step == 1)
            {
                encoder_Force(angle);
            
                // use the current sector if forward rotation or previous if reverse
                uint8_t sector_index = forward ? sector : (sector+1)%numHallSectors;
            
                // keep track of the encoder value between sectors
                border[sector_index] = encoder_GetUncalibrated();

                // found the s-th border, put a 1 in the mask
                border_flag |= 1 << sector_index;
                
                // If all sectors are found apply least squares fitting by computing average of
                // difference between measured values on borders and expected hall angle
                if (border_flag == 63) // 111111
                {
                    calibration_step = 2;
                
                    int32_t offset = int16_t(border[0] - MainConf.pwm.hall_offset + minHallAngleDelta);
                    offset += int16_t(border[1] - MainConf.pwm.hall_offset + minHallAngleDelta - hallAngleStep);
                    offset += int16_t(border[2] - MainConf.pwm.hall_offset + minHallAngleDelta - 2 * hallAngleStep);
                    offset += int16_t(border[3] - MainConf.pwm.hall_offset + minHallAngleDelta - 3 * hallAngleStep);
                    offset += int16_t(border[4] - MainConf.pwm.hall_offset + minHallAngleDelta - 4 * hallAngleStep);
                    offset += int16_t(border[5] - MainConf.pwm.hall_offset + minHallAngleDelta - 5 * hallAngleStep);
                
                    offset /= numHallSectors;
                     
                    embot::core::print("CALIBRATED\n");
                    
                    encoder_Calibrate(int16_t(offset));
                }
            }
            /*
            2) Update the forced value even if it is not used when the encoder is calibrated.
            Reset the encoder angle after a full rotation to avoid desynching
            */
            else if (calibration_step == 2)
            {
                encoder_Force(angle);
                // reset the angle after full rotation
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    encoder_Reset();
                }
            }
        }
    }
    // update the old sector and hall status
    sector_old = sector;
    s_pwm_hallStatus_old = hallStatus;
    
    return hallStatus;
}


HAL_StatusTypeDef pwm_hallConfig(uint8_t swapBC, uint16_t pwm_hall_offset)
{
    MainConf.pwm.hall_offset = pwm_hall_offset;
    MainConf.pwm.swapBC = swapBC;
    
    s_pwm_hallOrder[0] = 0;
    s_pwm_hallOrder[1] = MainConf.pwm.swapBC ? 2 : 1;
    s_pwm_hallOrder[2] = MainConf.pwm.swapBC ? 1 : 2;

    s_pwm_updateHallStatus();
    
    /* Start position counter */
    hallCounter = 0;
    
    return HAL_OK;
}

//void pwm_setADC_cb(void (*fn_cb)(void *, int16_t[3], void*, void*), void *owner, void* rtu, void* rty)
//{
//    s_pwm_adc_callback_set.callback_fn = fn_cb;
//    s_pwm_adc_callback_set.owner = owner;
//    s_pwm_adc_callback_set.rtu = rtu;
//    s_pwm_adc_callback_set.rty = rty;
//}



#endif // #if defined(MOTORHAL_changes)

/* END OF FILE ********************************************************************************************************/
