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

#include "embot_hw_motor_bsp_amc2c.h"
#define ENC_TIM (embot::hw::motor::bsp::amc2c::hTIM5)

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


struct enc_Conversion
{
    int32_t offset {0};
    int32_t factor {1}; 
    
    int32_t convert(int32_t v) const
    {
       return offset + (factor*v);
    }

    constexpr enc_Conversion() = default;  
    constexpr enc_Conversion(int32_t o, int32_t f) : offset(o), factor(f) {}        
};

struct enc_Internals
{
    bool started {false};
    enc_Configuration config {};
    enc_Mode mode {};
    enc_Conversion conversion {};
        
    enc_Internals() = default;   
};


enc_Internals _enc_internals {};

extern bool enc_Init(const enc_Configuration &config)
{
    EncDeInit();
    _enc_internals.started = false;
    _enc_internals.config = config;
    return true;
}

extern bool enc_DeInit()
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&ENC_TIM, TIM_CHANNEL_ALL);

    embot::hw::motor::bsp::amc2c::DeInit_TIM5();    
    
    _enc_internals.started = false;
    
    return true;
}

extern bool enc_Start(const enc_Mode& mode)
{
    if(false == mode.isvalid())
    {
        return false;
    }
    
    if(true == _enc_internals.started)
    {   // if already started i first stop. i can do that by calling enc_Init({})
        enc_Init({});
    }        
           
    
    _enc_internals.mode = mode;
    
    #warning marco.accame: so far we dont manage mode.calibrate_with_hall and mode.use_index
    
    _enc_internals.conversion.factor = 65536L*_enc_internals.mode.num_polar_couples/_enc_internals.mode.resolution;
        
    embot::hw::motor::bsp::amc2c::Init_TIM5(_enc_internals.mode.resolution, _enc_internals.mode.num_polar_couples);
    
    // and now i can do what is required ... i also start the index ... who cares
    
    bool ret = true;

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
            ret = true;
        }
        else
        {
            /* Failed start of the timer */
            HAL_TIM_UnRegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
            ret = false;
        }
    }
    /* Errors detected */    
       
    _enc_internals.started = ret;
    
    return ret;
}


extern bool enc_IsStarted()
{
    return _enc_internals.started;
}

extern int32_t enc_GetValue()
{
    if(false == _enc_internals.started)
    {
        return 0;
    }
    
    int32_t v = __HAL_TIM_GetCounter(&ENC_TIM);
    
    return _enc_internals.conversion.convert(v);
}



#if 0 // old rubbish

// old rubbish

// 
////extern bool enc_Init();
//extern bool enc_Config(bool has_quad_enc, int16_t resolution, uint8_t num_polar_couples, bool has_hall_sens);
////extern bool enc_DeInit();
// 
//extern int32_t enc_GetElectricalAngle(void);
// 
//extern int16_t enc_GetResolution();
// 
//extern void enc_ForceAngle(int32_t angle);
// 
//extern void enc_Reset();
// 
//extern uint16_t enc_GetUncalibrated();
// 
//extern void enc_Calibrate(uint16_t offset);

// -- init and deinit of TIM5

namespace embot::hw::motor::bsp::amc2c {
    
//    void MY_TIM5_DeInit();
//    void MY_TIM5_Init(int16_t _resolution, uint8_t _num_polar_couples);
    
    
//    void MY_TIM5_DeInit()
//    {
//        // HAL_TIM_IC_ConfigChannel() inversa for TIM_CHANNEL_3 and TIM_CHANNEL_4
//        // HAL_TIMEx_MasterConfigSynchronization() inversa
//        HAL_TIM_Encoder_DeInit(&hTIM5);
//        
//        HAL_TIM_IC_DeInit(&hTIM5);
//        
//    }
    
//    _countermode (originally TIM_COUNTERMODE_UP):
//    TIM_COUNTERMODE_UP if resolution is positive
//    TIM_COUNTERMODE_DOWN is negative
//    _period (originally 4294967295): 
//    as for TIM2 of amcbldc is: (_resolution/_num_polar_couples + 0.5) - 1;    
//    _encodermode is originally TIM_ENCODERMODE_TI12  
//    it must be TIM_ENCODERMODE_TI12
//        
//    void MY_TIM5_Init(int16_t _resolution, uint8_t _num_polar_couples)
//    {
//        uint32_t _encodermode = TIM_ENCODERMODE_TI12;
//        uint32_t _countermode = TIM_COUNTERMODE_UP;
//        uint32_t _clockdivision = TIM_CLOCKDIVISION_DIV1; // originally = TIM_CLOCKDIVISION_DIV4, amcbldc = TIM_CLOCKDIVISION_DIV1
//        uint32_t _ic1filter = 4; // amcbldc is = 4, in here amc is originally = 8 
//        uint32_t _ic2filter = 4; // amcbldc is = 4, in here amc is originally = 15 
//        if(_resolution < 0)
//        {
//            _resolution = - _resolution;
//            _countermode = TIM_COUNTERMODE_DOWN;            
//        }

//        uint32_t _period = (_resolution/_num_polar_couples + 0.5) - 1;
//       

//    //        if (ResMgr_Request(RESMGR_ID_TIM5, RESMGR_FLAGS_ACCESS_NORMAL | \
//    //                      RESMGR_FLAGS_CPU2 , 0, NULL) != RESMGR_OK)
//    //        {
//    //        /* USER CODE BEGIN RESMGR_UTILITY_TIM5 */
//    //        Error_Handler();
//    //        /* USER CODE END RESMGR_UTILITY_TIM5 */
//    //        }
//    //        /* USER CODE BEGIN TIM5_Init 0 */

//    //        /* USER CODE END TIM5_Init 0 */

//        TIM_Encoder_InitTypeDef sConfig = {0};
//        TIM_MasterConfigTypeDef sMasterConfig = {0};
//        TIM_IC_InitTypeDef sConfigIC = {0};

//        /* USER CODE BEGIN TIM5_Init 1 */

//        /* USER CODE END TIM5_Init 1 */
//        hTIM5.Instance = TIM5;
//        hTIM5.Init.Prescaler = 0;
//        hTIM5.Init.CounterMode = _countermode;
//        hTIM5.Init.Period = _period;
//        hTIM5.Init.ClockDivision = _clockdivision;
//        hTIM5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//        if (HAL_TIM_IC_Init(&hTIM5) != HAL_OK)
//        {
//        Error_Handler();
//        }
//        sConfig.EncoderMode = _encodermode;
//        sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
//        sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
//        sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
//        sConfig.IC1Filter = _ic1filter;
//        sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
//        sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
//        sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
//        sConfig.IC2Filter = _ic2filter;
//        if (HAL_TIM_Encoder_Init(&hTIM5, &sConfig) != HAL_OK)
//        {
//        Error_Handler();
//        }
//        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//        if (HAL_TIMEx_MasterConfigSynchronization(&hTIM5, &sMasterConfig) != HAL_OK)
//        {
//        Error_Handler();
//        }
//        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
//        sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
//        sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
//        sConfigIC.ICFilter = 8;
//        if (HAL_TIM_IC_ConfigChannel(&hTIM5, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
//        {
//        Error_Handler();
//        }
//        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
//        sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
//        sConfigIC.ICFilter = 0;
//        if (HAL_TIM_IC_ConfigChannel(&hTIM5, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
//        {
//        Error_Handler();
//        }
//        /* USER CODE BEGIN TIM5_Init 2 */

//        /* USER CODE END TIM5_Init 2 */

//    }  

} // namespace embot::hw::motor::bsp::amc2c {


//typedef struct
//{
//    enc_Configuration_t config;
//    uint32_t            extra;  
//    int32_t             encoderConvFactor; 
//    int32_t             encoder_electricalOffset;    // imposed by enc_Calibrate(offset)
//    bool                encoder_Calibrated;          // imposed by enc_Calibrate(offset)
//} s_enc_ExtendedConfiguration_t;

//static s_enc_ExtendedConfiguration_t s_enc_ExtConfig {0};
//    
//static int32_t s_encoder_ForcedValue = 0;

////extern bool enc_Init()
////{
////    bool r = true;
////    
////    s_enc_ExtConfig.config.resolution = 0;
////    s_enc_ExtConfig.config.has_hall_sens = true;
////    s_enc_ExtConfig.config.has_quad_enc = false;
////    s_enc_ExtConfig.config.num_polar_couples = 0;
////    s_enc_ExtConfig.extra = 0;
////    s_enc_ExtConfig.encoderConvFactor = 1;
////    s_enc_ExtConfig.encoder_electricalOffset = 0;
////    s_enc_ExtConfig.encoder_Calibrated = false;
////    
////    EncDeInit();
////    
////    return r;
////}


//extern bool enc_Config(bool has_quad_enc, int16_t resolution, uint8_t num_polar_couples, bool has_hall_sens)
//{
//    bool r = true;
//    
//    // we do something in here only if has_quad_enc is true and encoder resolution is not zero 
//    // in such a case we use {resolution, num_polar_couples} to count correctly 
//    
//    s_enc_ExtConfig.config.resolution = resolution;
//    s_enc_ExtConfig.config.has_hall_sens = has_hall_sens;
//    s_enc_ExtConfig.config.has_quad_enc = has_quad_enc;
//    s_enc_ExtConfig.config.num_polar_couples = num_polar_couples;
//    s_enc_ExtConfig.extra = 0;    
//    s_enc_ExtConfig.encoderConvFactor = 1;
//    s_enc_ExtConfig.encoder_electricalOffset = 0;
//    s_enc_ExtConfig.encoder_Calibrated = false;    
//    
//    bool startEncoder = (true == has_quad_enc) && 
//                        (0 != resolution) && 
//                        (num_polar_couples > 0);    
//    
//    if(false == startEncoder)
//    {
//        s_enc_ExtConfig.config.resolution = 0;
//        s_enc_ExtConfig.config.has_quad_enc = false;
//        
//        return true;        
//    }  
//    
//    
//    // for our wrist motors we have num_polar_couples = 7 and
//    // we have num_polar_couples = 8 for motors w/ the optical encoder
//    
//    // numero che serve per convertire da tacche ad angolo
//    s_enc_ExtConfig.encoderConvFactor = 65536L*num_polar_couples/resolution;   
//        
//    // now something as EncInit() but w/ something different
//    
//    
//    // we probably should init again the timer hTIM5 so that it can count in a different way.
//      
//    enc_DeInit();
//    
//    embot::hw::motor::bsp:: MY_TIM5_Init(resolution, num_polar_couples);
//    
//    // and now i can do what is required ...

//    /* Register the callback function used to signal the activation of the Index pulse */
//    if (HAL_OK == HAL_TIM_RegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID, EncCapture_cb))
//    {
//        /* Clear local variables */
//        EncStatus = ENC_STATUS_IDLE;
//        EncAbsoluteZero = 0;
//        EncRotorZero = 0;
//        /* Clear counter */
//        __HAL_TIM_SET_COUNTER(&ENC_TIM, 0);
//        /* Start timers in encoder mode */
//        if (HAL_OK == HAL_TIM_Encoder_Start(&ENC_TIM, TIM_CHANNEL_ALL))
//        {
//            /* Enable leading edge capture, without interrupts */
//            HAL_TIM_IC_Start(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
//            /* Enable trailing edge capture, with interrupts */
//            HAL_TIM_IC_Start_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
//            return true;
//        }
//        /* Failed start of the timer */
//        HAL_TIM_UnRegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
//    }
//    /* Errors detected */
//    
//    return false;   
//    
//}


////extern bool enc_DeInit()
////{
////    bool r = true;
////    
////    /* Stop any pending operation */
////    HAL_TIM_IC_Stop_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
////    HAL_TIM_IC_Stop(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
////    HAL_TIM_Encoder_Stop(&ENC_TIM, TIM_CHANNEL_ALL);

////    embot::hw::motor::bsp::MY_TIM5_DeInit();    
////    
////    return r;    
////}


//extern int32_t enc_GetElectricalAngle(void)
//{
//    // first level: we dont have a quad enc     
//    if(0 == s_enc_ExtConfig.config.resolution)
//    {
//        return s_encoder_ForcedValue;
//    }
//    
//    // second level: we have a quad enc but also hall sensors. 
//    // in such a case i need to wait for calibration to be over so that I have a valid encoder_electricalOffset
//    if(true == s_enc_ExtConfig.config.has_hall_sens)
//    {
//        if (false == s_enc_ExtConfig.encoder_Calibrated)
//        {
//            return s_encoder_ForcedValue;
//        }
//    }

//    // ok, i have all the required ... MAYBE NOT: the timer must have been started ....
//    
//    int32_t v = __HAL_TIM_GetCounter(&ENC_TIM);
//        
//    return s_enc_ExtConfig.encoder_electricalOffset + (v * s_enc_ExtConfig.encoderConvFactor) & 0xFFFF;  
//}

//extern int16_t enc_GetResolution()
//{
//    return s_enc_ExtConfig.config.resolution;
//}


//extern void enc_ForceAngle(int32_t angle)
//{
//    s_encoder_ForcedValue = angle;
//}

//extern void enc_Reset()
//{
//    /* Clear counter */
//    __HAL_TIM_SET_COUNTER(&ENC_TIM, 0);
//// ??    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
//}

//extern uint16_t enc_GetUncalibrated()
//{
//    return (__HAL_TIM_GetCounter(&ENC_TIM) * s_enc_ExtConfig.encoderConvFactor) & 0xFFFF;
//}

//extern void enc_Calibrate(uint16_t offset)
//{   
//    s_enc_ExtConfig.encoder_electricalOffset = offset;   
//    s_enc_ExtConfig.encoder_Calibrated = true;
//}

#endif // old rubbish

#endif // #if defined(MOTORHAL_changes) 


/* END OF FILE ********************************************************************************************************/

