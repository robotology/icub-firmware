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
static volatile uint8_t  hallStatus_old = 0;
static volatile int32_t  hallCounter = 0;
static volatile uint16_t hallAngle = 0;
static volatile uint8_t  hallOrder[3];

static volatile bool calibrating = true;

//struct adc_callback_set_T
//{
//    void (*callback_fn)(void *owner, int16_t Iuvw[3], void* rtu, void* rty);
//    void *owner;
//    void* rtu;
//    void* rty;
//};

//static volatile adc_callback_set_T adc_callback_set =
//{
//    NULL, NULL, NULL, NULL    
//};

static void s_pwm_dummy_adc_callback(void *owner, const pwmCurrents_t * const currents) {}
static pwmCurrents_t s_pwm_dummy_currents = {0};
    
static volatile pwm_ADC_callback_t s_pwm_ADC_cbk = 
{
    .callback = s_pwm_dummy_adc_callback,
    .owner = NULL
};


//void setADC_cb(void (*fn_cb)(void *, int16_t[3], void*, void*), void *owner, void* rtu, void* rty)
//{
//    adc_callback_set.callback_fn = fn_cb;
//    adc_callback_set.owner = owner;
//    adc_callback_set.rtu = rtu;
//    adc_callback_set.rty = rty;
//}


extern void set_ADC_callback(pwm_ADC_callback_t *cbk)
{
    if(NULL != cbk)
    {
        s_pwm_ADC_cbk.callback = (NULL != cbk->callback) ? cbk->callback : s_pwm_dummy_adc_callback;
        s_pwm_ADC_cbk.owner = cbk->owner;     
    }
}

#if defined(USE_STM32HAL) && defined(__cplusplus)

constexpr int16_t numHallSectors = 6;
constexpr float_t iCubDeg = 65536.0; // Range of angular values that can be represented with the encoder
constexpr float_t Deg2iCub = iCubDeg / 360.0;
constexpr int16_t hallAngleStep = 60.0 * Deg2iCub; // 60 degrees scaled by the range of possible values
constexpr int16_t minHallAngleDelta = 30.0 * Deg2iCub; // 30 degrees scaled by the range of possible values

constexpr uint16_t hallAngleTable[] =
{
    /* ABC  (�)  */
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
//    /* ABC  (�)  */
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
    /* ABC  (�)  */
    /* LLL ERROR */ 0,
    /* LLH  300  */ 300.0 * 65536.0 / 360.0 + 0.5, /* 54613 */
    /* LHL  180  */ 180.0 * 65536.0 / 360.0 + 0.5, /* 32768 */
    /* LHH  240  */ 240.0 * 65536.0 / 360.0 + 0.5, /* 43690 */
    /* HLL   60  */  60.0 * 65536.0 / 360.0 + 0.5, /* 10923 */
    /* HLH    0  */   0.0 * 65536.0 / 360.0 + 0.5, /*     0 */
    /* HHL  120  */ 120.0 * 65536.0 / 360.0 + 0.5, /* 21845 */
    /* HHH ERROR */ 0
};

static const int16_t hallSectorTable[] =
{
    /* ABC  (�)  */
    /* LLL ERROR */ 0,
    /* LLH  240  */ 4, /* 43690 */
    /* LHL  120  */ 2, /* 21845 */
    /* LHH  180  */ 3, /* 32768 */
    /* HLL    0  */ 0, /*     0 */
    /* HLH  300  */ 5, /* 54613 */
    /* HHL   60  */ 1, /* 10922 */
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
 
#define DECODE_HALLSTATUS (((HALL1_GPIO_Port->IDR & HALL1_Pin) >> MSB(HALL1_Pin)) << hallOrder[0])  \
                        | (((HALL2_GPIO_Port->IDR & HALL2_Pin) >> MSB(HALL2_Pin)) << hallOrder[1])  \
                        | (((HALL3_GPIO_Port->IDR & HALL3_Pin) >> MSB(HALL3_Pin)) << hallOrder[2])

static uint8_t updateHallStatus(void)
{
    hallStatus_old = hallStatus;
    static int8_t calibration_step = 0;
    
    static uint16_t border[6]={0};
    static uint8_t border_flag = 0;
    
    /* Read current value of HALL1, HALL2 and HALL3 signals in bits 2..0 */
    hallStatus = DECODE_HALLSTATUS;
    uint16_t angle = MainConf.pwm.hall_offset + hallAngleTable[hallStatus];
    
    // Check which sector between [0 ... 5] the rotor is in
    int16_t sector = ((minHallAngleDelta + angle) / hallAngleStep) % numHallSectors;
    static int16_t sector_old = sector;
    
    hallAngle = angle;
    
    if (MainConf.encoder.resolution == 0) // no encoder
    {
        encoderForce(angle);
    }
    else
    {
        if (!hallStatus_old)
        {            
            hallCounter = 0;
            encoderReset();
            encoderForce(angle);
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
                encoderForce(angle);
                
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    encoderReset();
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
                encoderForce(angle);
            
                // use the current sector if forward rotation or previous if reverse
                uint8_t sector_index = forward ? sector : (sector+1)%numHallSectors;
            
                // keep track of the encoder value between sectors
                border[sector_index] = encoderGetUncalibrated();

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
                    
                    encoderCalibrate(int16_t(offset));
                }
            }
            /*
            2) Update the forced value even if it is not used when the encoder is calibrated.
            Reset the encoder angle after a full rotation to avoid desynching
            */
            else if (calibration_step == 2)
            {
                encoderForce(angle);
                // reset the angle after full rotation
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    encoderReset();
                }
            }
        }
    }
    // update the old sector and hall status
    sector_old = sector;
    hallStatus_old = hallStatus;
    
    return hallStatus;
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
        updateHallStatus();
    }
}


/*******************************************************************************************************************//**
 * @brief   Called by DMA associated to ADC2 whenever a new set of phase currents is available
 * @param   i1, i2, i3  Currents measured in PHASE1, PHASE2 and PHASE3 of the motor
 * @return  void
 */
void pwmSetCurrents_cb(int16_t i1, int16_t i2, int16_t i3)
{   
    static pwmCurrents_t currents = {0};
    // as soon as possible: copy the currents
    currents.u = raw2mAmps(i1);
    currents.v = raw2mAmps(i2);
    currents.w = raw2mAmps(i3); 
        
    if (calibrating)
    {
        static int16_t counter = 0;
        
        if (counter < 1000)
        {
            ++counter;
            
            analogMovingAverage(i1, i2, i3);
        }
        else
        {            
            analogSetOffsetIph1(analogCph1() + analogGetOffsetIph1());
            analogSetOffsetIph2(analogCph2() + analogGetOffsetIph2());
            analogSetOffsetIph3(analogCph3() + analogGetOffsetIph3());
            
            calibrating = false;
        }
    }
    
    s_pwm_ADC_cbk.callback(s_pwm_ADC_cbk.owner, &currents);    
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Complete the CubeMx TIM1 configuration for the PWM subsystem
 * @param   void
 * @return  void
 */
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
    
    //updateHallStatus();

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

//HAL_StatusTypeDef pwmDeinit(void)
//{
//    pwmPhaseDisable(PWM_PHASE_ALL);
//    
//    return HAL_OK;
//}

/*******************************************************************************************************************//**
 * @brief   Complete the CubeMx TIM3 configuration for the PWM subsystem
 * @param   void
 * @return  void
 */
HAL_StatusTypeDef hallInit(void)
{
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

HAL_StatusTypeDef hallDeinit(void)
{
    return HAL_OK;
}

HAL_StatusTypeDef hallConfig(uint8_t swapBC, uint16_t pwm_hall_offset)
{
    MainConf.pwm.hall_offset = pwm_hall_offset;
    MainConf.pwm.swapBC = swapBC;
    
    hallOrder[0] = 0;
    hallOrder[1] = MainConf.pwm.swapBC ? 2 : 1;
    hallOrder[2] = MainConf.pwm.swapBC ? 1 : 2;

    updateHallStatus();
    
    /* Start position counter */
    hallCounter = 0;
    
    return HAL_OK;
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
        pwmPhaseEnable(PWM_PHASE_ALL);
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
    if (calibrating) u = v = w = 0;
    
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
