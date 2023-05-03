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
#if defined(MOTORHAL_changes)  
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

struct hall_Table
{
    static constexpr int16_t numHallSectors = 6;
    static constexpr float_t iCubDeg = 65536.0; // Range of angular values that can be represented with the encoder
    static constexpr float_t Deg2iCub = iCubDeg / 360.0;
    static constexpr int16_t hallAngleStep = 60.0 * Deg2iCub; // 60 degrees scaled by the range of possible values
    static constexpr int16_t minHallAngleDelta = 30.0 * Deg2iCub; // 30 degrees scaled by the range of possible values    

    static constexpr uint16_t hallAngleTable[] =
    {
        /* ABC  (°)  */
        /* LLL ERROR */ static_cast<uint16_t>(0),
        /* LLH  240  */ static_cast<uint16_t>(240.0 * Deg2iCub), /* 43690 */
        /* LHL  120  */ static_cast<uint16_t>(120.0 * Deg2iCub), /* 21845 */
        /* LHH  180  */ static_cast<uint16_t>(180.0 * Deg2iCub), /* 32768 */
        /* HLL    0  */ static_cast<uint16_t>(  0.0 * Deg2iCub), /*     0 */
        /* HLH  300  */ static_cast<uint16_t>(300.0 * Deg2iCub), /* 54613 */
        /* HHL   60  */ static_cast<uint16_t>( 60.0 * Deg2iCub), /* 10922 */
        /* HHH ERROR */ static_cast<uint16_t>(0)
    };
    
    static constexpr uint16_t status2angle(uint8_t hallstatus)
    {
        return hallAngleTable[hallstatus & 0x07];
    }
    
    static constexpr uint8_t angle2sector(uint16_t angle)
    {    
        return ((minHallAngleDelta + angle) / hallAngleStep) % numHallSectors;
    }

    constexpr hall_Table() = default;
};   

struct hall_Data
{
    uint8_t order[3] = {0, 1, 2};
    int32_t counter {0};
    uint16_t angle {0};
    volatile uint8_t status {0};
    
    void reset(hall_Mode::SWAP s = hall_Mode::SWAP::none)
    {
        counter = 0;
        angle = 0;
        status = 0;
        set(s);
    }
    
    void set(hall_Mode::SWAP s)
    {
        order[0] = 0; 
        order[1] = (hall_Mode::SWAP::BC == s) ? 2 : 1;
        order[2] = (hall_Mode::SWAP::BC == s) ? 1 : 2;        
    }
    
    hall_Data() = default;
};


struct hall_Internals
{
    bool started {false};
    hall_Configuration config {};
    hall_Mode mode {};    
    hall_Data data {};       

    void reset()
    {
        started = false;
        config.acquisition = hall_Configuration::ACQUISITION::deferred;
        mode.swap = hall_Mode::SWAP::none;
        mode.offset = 0;
        data.reset();
    }        
        
    hall_Internals() = default;   
};


hall_Internals _hall_internals {};


static uint8_t hall_Get()
{
    uint8_t x = HALL_INPUT(); // we get the values of the three bits: H3|H2|H1. they are also called C|B|A
    // and now we move them according to order[]. 
    // if we have order = {0, 1, 2} we dont reorder anything, so we have v = x = H3H2H1 = CBA.
    // but in case swapBC is true, then we have  order {0, 2, 1} which swap second w/third, so v = H2H3H1 =  BCA
    
    uint8_t v = (((v>>0)&0x1) >> _hall_internals.data.order[0])      |   // H1 (pos 0) is moved to the final position
                (((v>>1)&0x1) >> _hall_internals.data.order[1])      |   // H2 (pos 1) is moved to the final position
                (((v>>2)&0x1) >> _hall_internals.data.order[2]);         // H3 (pos 2) is moved to the final position
    return v;
}


static void hall_OnCapture(TIM_HandleTypeDef *htim)
{
    _hall_internals.data.status = hall_Get();
    
    // in here we DONT ... perform any calibration of the encoder ....
    // because in the wrist we dont have encoder. we may implement this calibration later on
}


extern bool hall_DeInit()
{
    _hall_internals.reset();
       
    HallDeInit();
    
    return true;     
}

extern bool hall_Init(const hall_Configuration &config)
{ 
    HallDeInit();
    
    _hall_internals.reset();
    
    _hall_internals.config = config;
    
    // _hall_internals.config.acquisition is always ACQUISITION::deferred
    
    return true;     
}

extern bool hall_Start(const hall_Mode &mode)
{ 
    bool ret {true};  

    _hall_internals.mode = mode;        
    
    _hall_internals.data.set(_hall_internals.mode.swap);
    
    _hall_internals.data.status = hall_Get();

    // marco.accame: i stop things
    
    /* Stop counter */
    HAL_TIM_Base_Stop(&HALL_TIM);
    /* Stop interrupt */
    HAL_TIM_IC_Stop(&HALL_TIM, TIM_CHANNEL_1);
    
    // marco.accame: i register the callback and start counter.
    /* Register the callback function for CH1 capture event */
    if (HAL_OK == HAL_TIM_RegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID, hall_OnCapture))
    {
        /* Start counter */
        if (HAL_OK == HAL_TIM_Base_Start(&HALL_TIM))
        {
// marco.accame: i DONT do this call HallSetPwm() 
//            /* Zero amplitude field */
//            HallSetPwm(0);
            /* Start channel 1 in interrupt mode */
            if (HAL_OK == HAL_TIM_IC_Start_IT(&HALL_TIM, TIM_CHANNEL_1))
            {
                ret = true;
            }
            else
            {
                ret = false;
                /* Stop counter */
                HAL_TIM_Base_Stop(&HALL_TIM);
                /* Disable all */
//                PwmPhaseEnable(PWM_PHASE_NONE);
            }
        }
        else
        {
            ret = false;
            /* Remove the callback function */
            HAL_TIM_UnRegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
        }
    }
    
    // i read it again
    _hall_internals.data.status = hall_Get();
    
    
    return ret;
}

uint8_t hall_GetStatus(void)
{
    return _hall_internals.data.status;
}

#if 0

#error it is old code. i keep it because we have a draft correction of encoder using the hall w/ s_hall_updateHallStatus()
    
extern bool hall_Init()
{
    bool ret {false);
        
    s_hall_Settings.swapBC = false;
    s_hall_Settings.offset = 0;
    s_hall_Settings.order[0] = 0;
    s_hall_Settings.order[1] = 1;
    s_hall_Settings.order[2] = 2;
    s_hall_Settings.counter = 0;
    s_hall_Settings.angle = 0;

  
// on amcbldc it is just like taht    
//    /* Start position counter */
//    hallCounter = 0;
// 
//    /* Check TIM3 configuration and start it */
//    return ((TIM3 == htim3.Instance) &&
//            (HAL_OK == HAL_TIM_RegisterCallback(&htim3, HAL_TIM_IC_CAPTURE_CB_ID, hallStatusChange_cb)) &&
//            (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1)) &&
//            (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2)) &&
//            (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3)) &&
//            (HAL_OK == HAL_TIM_Base_Start_IT(&htim3)))? HAL_OK : HAL_ERROR;
//  

// on amc2c we just ...

#warning attenzione: HallInit() potrebbe fare cose che non vogliamo tipo mettere il pwm a zero usando gli hall che magari non abbiamo
// quindi riscrivo le cose

    // marco.accame: i get code from HallInit() and i adapt it

    // marco.accame: i stop things
    
    /* Stop counter */
    HAL_TIM_Base_Stop(&HALL_TIM);
    /* Stop interrupt */
    HAL_TIM_IC_Stop(&HALL_TIM, TIM_CHANNEL_1);
    
    // marco.accame: i register the callback and start counter.
    /* Register the callback function for CH1 capture event */
    if (HAL_OK == HAL_TIM_RegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID, hall_OnCapture))
    {
        /* Start counter */
        if (HAL_OK == HAL_TIM_Base_Start(&HALL_TIM))
        {
// marco.accame: i DONT do this call HallSetPwm() 
//            /* Zero amplitude field */
//            HallSetPwm(0);
            /* Start channel 1 in interrupt mode */
            if (HAL_OK == HAL_TIM_IC_Start_IT(&HALL_TIM, TIM_CHANNEL_1))
            {
                ret = true;
            }
            else
            {
                ret = false;
                /* Stop counter */
                HAL_TIM_Base_Stop(&HALL_TIM);
                /* Disable all */
//                PwmPhaseEnable(PWM_PHASE_NONE);
            }
        }
        else
        {
            ret = false;
            /* Remove the callback function */
            HAL_TIM_UnRegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
        }
    }
    
    return ret;
}


extern bool hall_DeInit()
{
    s_hall_Settings.swapBC = false;
    s_hall_Settings.offset = 0;
    s_hall_Settings.order[0] = 0;
    s_hall_Settings.order[1] = 1;
    s_hall_Settings.order[2] = 2;
    s_hall_Settings.counter = 0;
    s_hall_Settings.angle = 0;
    
    HallDeInit();
    
    return true;    
}


extern bool hall_Config(bool swapBC, uint16_t pwm_hall_offset)
{
    #warning TODO-embot::hw::motor: hall_Config()
    
    s_hall_Settings.swapBC = swapBC;
    s_hall_Settings.offset = pwm_hall_offset;
    
    s_hall_Settings.order[0] = 0;
    s_hall_Settings.order[1] = (true == swapBC) ? 2 : 1;
    s_hall_Settings.order[2] = (true == swapBC) ? 1 : 2;
    
    s_hall_Settings.counter = 0;
    s_hall_Settings.angle = 0;
    
    // now we need to force an update of the hall status.
    // this update is surely done inside the callback HallCapture_cb() associated to HAL_TIM_IC_CAPTURE_CB_ID
    // however the callback happens only when the rotor moves and in here we want   
    // to have measures even if it does not.
    // in amcbldc we have function s_pwm_updateHallStatus()
    #warning DOUBT: should we stop the callback of the IRQ Handler in here? what if it triggers when we execute?
        
    s_hall_updateHallStatus();
    
    // ma perche' si fa dopo? ....
    s_hall_Settings.counter = 0;

//    s_pwm_updateHallStatus();
//    
//    /* Start position counter */
//    hallCounter = 0;
    
    return true;
}

uint8_t hall_GetStatus(void)
{
    return HallStatus;
}

static volatile uint8_t s_hall_hallStatus_old = 0;

static void s_hall_updateHallStatus(void)
{
    hall_status = hall_Get();
#if 0    
    s_hall_hallStatus_old = HallStatus;
    static int8_t calibration_step = 0;
    
    static uint16_t border[6]={0};
    static uint8_t border_flag = 0;
    
    /* Read current value of HALL1, HALL2 and HALL3 signals in bits 2..0 */
    HallStatus = hall_Get();
    uint16_t angle = s_hall_Settings.offset + hallAngleTable[HallStatus];
    
    // Check which sector between [0 ... 5] the rotor is in
    int16_t sector = ((minHallAngleDelta + angle) / hallAngleStep) % numHallSectors;
    static int16_t sector_old = sector;
    
    s_hall_Settings.angle = angle;
    
    if(0 == enc_GetResolution()) // no encoder
    {
        enc_ForceAngle(angle);
    }
    else
    {
        if (!s_hall_hallStatus_old) // madonna, ma che significa??? 
        {   
            // forse: che HallStatus e' diverso da zero (e' zero allo startup) e che quindi qualcuno ha gia' chiamato la sua lettura 
            // e quindi ... si esce usando l'angolo ottenuto. 
            // attenzione pero': questo codice e' preso dalla amcbldc dove forse ci sono inizializzazioni diverse ...          
            s_hall_Settings.counter = 0;
            enc_Reset();
            enc_ForceAngle(angle);
        }
        else
        {
            // Check if the motor is rotating forward (counterclockwise)
            bool forward = ((sector-sector_old+numHallSectors)%numHallSectors)==1;
        
            if (forward) // forward
            {
                ++s_hall_Settings.counter;
                angle -= minHallAngleDelta; // -30 deg
            }
            else
            {
                --s_hall_Settings.counter;
                angle += minHallAngleDelta; // +30 deg
            }
             /*
            0) Use the Hall sensors to rotate until the wrap-around border is reached,
             then reset the encoder value
            */
            if (calibration_step == 0)
            {
                enc_ForceAngle(angle);
                
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    enc_Reset();
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
                enc_ForceAngle(angle);
            
                // use the current sector if forward rotation or previous if reverse
                uint8_t sector_index = forward ? sector : (sector+1)%numHallSectors;
            
                // keep track of the encoder value between sectors
                border[sector_index] = enc_GetUncalibrated();

                // found the s-th border, put a 1 in the mask
                border_flag |= 1 << sector_index;
                
                // If all sectors are found apply least squares fitting by computing average of
                // difference between measured values on borders and expected hall angle
                if (border_flag == 63) // 111111
                {
                    calibration_step = 2;
                
                    int32_t offset = int16_t(border[0] - s_hall_Settings.offset + minHallAngleDelta);
                    offset += int16_t(border[1] - s_hall_Settings.offset + minHallAngleDelta - hallAngleStep);
                    offset += int16_t(border[2] - s_hall_Settings.offset + minHallAngleDelta - 2 * hallAngleStep);
                    offset += int16_t(border[3] - s_hall_Settings.offset + minHallAngleDelta - 3 * hallAngleStep);
                    offset += int16_t(border[4] - s_hall_Settings.offset + minHallAngleDelta - 4 * hallAngleStep);
                    offset += int16_t(border[5] - s_hall_Settings.offset + minHallAngleDelta - 5 * hallAngleStep);
                
                    offset /= numHallSectors;
                     
                    embot::core::print("CALIBRATED\n");
                    
                    enc_Calibrate(int16_t(offset));
                }
            }
            /*
            2) Update the forced value even if it is not used when the encoder is calibrated.
            Reset the encoder angle after a full rotation to avoid desynching
            */
            else if (calibration_step == 2)
            {
                enc_ForceAngle(angle);
                // reset the angle after full rotation
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    enc_Reset();
                }
            }
        }
    }
    // update the old sector and hall status
    sector_old = sector;
    s_hall_hallStatus_old = HallStatus;
    
    //return hallStatus;
#endif    
}

#endif

#endif // #if defined(MOTORHAL_changes)

/* END OF FILE ********************************************************************************************************/
