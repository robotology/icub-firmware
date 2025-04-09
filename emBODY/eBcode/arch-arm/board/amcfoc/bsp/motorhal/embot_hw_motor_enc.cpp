/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_pwm.h"

#include "embot_hw_motor_enc.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include <array>

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_motor_enc)


#elif defined(EMBOT_ENABLE_hw_motor_enc)

#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
#include "embot_hw_motor_bldc_bsp_amcfoc_1cm7.h"
#endif



namespace embot::hw::motor::enc {
    
#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
   
    #define htimEnc1    (embot::hw::motor::bldc::bsp::amcfoc::cm7::htim5)
    #define htimEnc2 (embot::hw::motor::bldc::bsp::amcfoc::cm7::htim2)
#endif    
    
    
#define ENC_INDEX_LEADING_EDGE          TIM_CHANNEL_3
#define ENC_INDEX_IT_LEADING_EDGE       TIM_IT_CC3
//#define ENC_INDEX_ACTIVE_LEADING_EDGE   HAL_TIM_ACTIVE_CHANNEL_3
#define ENC_INDEX_TRAILING_EDGE         TIM_CHANNEL_4
#define ENC_INDEX_IT_TRAILING_EDGE      TIM_IT_CC4
//#define ENC_INDEX_ACTIVE_TRAILING_EDGE  HAL_TIM_ACTIVE_CHANNEL_4
/* Status register values */
#define ENC_STATUS_IDLE             (0)
#define ENC_STATUS_WAIT             (1)
#define ENC_STATUS_READY            (2)
/* Offset between the counting edge and the index edge */
#define ENC_UP_COUNTING_OFFSET      (1)
#define ENC_DOWN_COUNTING_OFFSET    (1)

static uint8_t  Enc1Status = ENC_STATUS_IDLE;
static uint32_t Enc1RotorZero;
static uint32_t Enc1AbsoluteZero;

static uint8_t  Enc2Status = ENC_STATUS_IDLE;
static uint32_t Enc2RotorZero;
static uint32_t Enc2AbsoluteZero;

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
    Configuration config {};
    Mode mode {};
    enc_Conversion conversion {};
    int32_t forcedvalue {0};    
    
    
    uint8_t  encStatus = ENC_STATUS_IDLE;
    uint32_t encRotorZero = 0;
    uint32_t encAbsoluteZero = 0;
    
    
    
    
    enc_Internals() = default;   
};


enc_Internals _enc_internals {};

extern bool init(const Configuration &config)
{
//    EncDeInit();
    _enc_internals.started = false;
    _enc_internals.config = config;
    return true;
}

extern bool deinit()
{
    Enc1DeInit();
    Enc2DeInit();
    /* Stop any pending operation */
//    HAL_TIM_IC_Stop_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
//    HAL_TIM_IC_Stop(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
//    HAL_TIM_Encoder_Stop(&ENC_TIM, TIM_CHANNEL_ALL);

#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
//    embot::hw::motor::bsp::amc::cm7::DeInit_TIM5();
#endif      
        
    
    _enc_internals.started = false;
    
    return true;
}

extern bool start(const Mode& mode)
{
    if(false == mode.isvalid())
    {
        return false;
    }
    
    if(true == _enc_internals.started)
    {   // if already started i first stop. i can do that by calling init({})
        init({});
    }        
           
    
    _enc_internals.mode = mode;
    
    #warning marco.accame: so far we dont manage mode.calibrate_with_hall and mode.use_index
    
    _enc_internals.conversion.factor = 65536L*_enc_internals.mode.num_polar_couples/_enc_internals.mode.resolution;
    
#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
//    embot::hw::motor::bldc::bsp::amcfoc::cm7::Init_TIM5(_enc_internals.mode.resolution, _enc_internals.mode.num_polar_couples);
#endif       
    
    Enc1Init();
    Enc2Init();
    
    
    // and now i can do what is required ... i also start the index ... who cares
    
    bool ret = true;
    
    #warning TODO: we can use EncInit() in here

//    /* Register the callback function used to signal the activation of the Index pulse */
//    if (HAL_OK == HAL_TIM_RegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID, EncCapture_cb))
//    {
//        /* Clear local variables */
////        EncStatus = ENC_STATUS_IDLE;
////        EncAbsoluteZero = 0;
////        EncRotorZero = 0;
//        /* Clear counter */
//        __HAL_TIM_SET_COUNTER(&ENC_TIM, 0);
//        /* Start timers in encoder mode */
//        if (HAL_OK == HAL_TIM_Encoder_Start(&ENC_TIM, TIM_CHANNEL_ALL))
//        {
//            /* Enable leading edge capture, without interrupts */
//            HAL_TIM_IC_Start(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
//            /* Enable trailing edge capture, with interrupts */
//            HAL_TIM_IC_Start_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
//            ret = true;
//        }
//        else
//        {
//            /* Failed start of the timer */
//            HAL_TIM_UnRegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
//            ret = false;
//        }
//    }
//    /* Errors detected */    
       
    _enc_internals.started = ret;
    
    return ret;
}

extern bool isstarted()
{
    return _enc_internals.started;
}

extern int32_t getvalue()
{
    if(false == _enc_internals.started)
    {
        return _enc_internals.forcedvalue;
    }
    
    int32_t v = 0; //__HAL_TIM_GetCounter(&ENC_TIM);
    
    return _enc_internals.conversion.convert(v);
}

void force(int32_t value)
{
    _enc_internals.forcedvalue = value;
}


//////bool EncInit(TIM_HandleTypeDef htimEncx, void (*EncxCapture_cb) , enc_Internals enc_internals)
//////{
//////    /* Stop any pending operation */
//////    HAL_TIM_IC_Stop_IT(&htimEncx, ENC_INDEX_LEADING_EDGE);
//////    HAL_TIM_IC_Stop(&htimEncx, ENC_INDEX_TRAILING_EDGE);
//////    HAL_TIM_Encoder_Stop(&htimEncx, TIM_CHANNEL_ALL);

//////    /* Register the callback function used to signal the activation of the Index pulse */
//////    if (HAL_OK == HAL_TIM_RegisterCallback(&htimEncx, HAL_TIM_IC_CAPTURE_CB_ID, EncxCapture_cb))
//////    {
//////        /* Clear local variables */
//////           
//////        enc_internals.encStatus = ENC_STATUS_IDLE;
//////        enc_internals.encRotorZero = 0;
//////        enc_internals.encAbsoluteZero = 0;

//////        /* Clear counter */
//////        __HAL_TIM_SET_COUNTER(&htimEncx, 0);
//////        /* Start timers in encoder mode */
//////        if (HAL_OK == HAL_TIM_Encoder_Start(&htimEncx, TIM_CHANNEL_ALL))
//////        {
//////            /* Enable leading edge capture, without interrupts */
//////            HAL_TIM_IC_Start(&htimEncx, ENC_INDEX_LEADING_EDGE);
//////            /* Enable trailing edge capture, with interrupts */
//////            HAL_TIM_IC_Start_IT(&htimEncx, ENC_INDEX_TRAILING_EDGE);
//////            return true;
//////        }
//////        /* Failed start of the timer */
//////        HAL_TIM_UnRegisterCallback(&htimEncx, HAL_TIM_IC_CAPTURE_CB_ID);
//////    }
//////    /* Errors detected */
//////    return false;
//////}


static void Enc1Capture_cb(TIM_HandleTypeDef *htim)
{
    int32_t le, te, delta;
    /* There must be a leading edge before */
    if (0 != __HAL_TIM_GET_FLAG(&htimEnc1, ENC_INDEX_IT_LEADING_EDGE))
    {
        /* Read the index trailing edge position */
        te = (int32_t)__HAL_TIM_GetCompare(&htimEnc1, ENC_INDEX_TRAILING_EDGE);
        /* Read the index leading edge position */
        le = (int32_t)__HAL_TIM_GetCompare(&htimEnc1, ENC_INDEX_LEADING_EDGE);
        /* Take the difference between readings */
        delta = te - le;
        /* Avoid inversion of direction over the index */
        if (0 != delta)
        {
            /* Update the index position */
            Enc1RotorZero = (uint32_t)((delta>=0)? (le + ENC_UP_COUNTING_OFFSET)
                                                 : (le - ENC_DOWN_COUNTING_OFFSET));
            /* Mark the absolute zero position */
            if (ENC_STATUS_WAIT == Enc1Status)
            {
                /* Store the absolute zero */
                Enc1AbsoluteZero = Enc1RotorZero;
                /* Do not repeat again */
                Enc1Status = ENC_STATUS_READY;
            }
        }
    }
}


int32_t Enc1GetRotorPosition(void)
{
    /* Read counter 32 bits value */
    return __HAL_TIM_GetCounter(&htimEnc1) - Enc1RotorZero;
}
bool Enc1Init(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc1, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc1, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc1, TIM_CHANNEL_ALL);

    /* Register the callback function used to signal the activation of the Index pulse */
    if (HAL_OK == HAL_TIM_RegisterCallback(&htimEnc1, HAL_TIM_IC_CAPTURE_CB_ID, Enc1Capture_cb))
    {
        /* Clear local variables */
        Enc1Status = ENC_STATUS_IDLE;
        Enc1AbsoluteZero = 0;
        Enc1RotorZero = 0;
        /* Clear counter */
        __HAL_TIM_SET_COUNTER(&htimEnc1, 0);
        /* Start timers in encoder mode */
        if (HAL_OK == HAL_TIM_Encoder_Start(&htimEnc1, TIM_CHANNEL_ALL))
        {
            /* Enable leading edge capture, without interrupts */
            HAL_TIM_IC_Start(&htimEnc1, ENC_INDEX_LEADING_EDGE);
            /* Enable trailing edge capture, with interrupts */
            HAL_TIM_IC_Start_IT(&htimEnc1, ENC_INDEX_TRAILING_EDGE);
            return true;
        }
        /* Failed start of the timer */
        HAL_TIM_UnRegisterCallback(&htimEnc1, HAL_TIM_IC_CAPTURE_CB_ID);
    }
    /* Errors detected */
    return false;
}


void Enc1DeInit(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc1, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc1, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc1, TIM_CHANNEL_ALL);
    HAL_TIM_UnRegisterCallback(&htimEnc1, HAL_TIM_IC_CAPTURE_CB_ID);
}




static void Enc2Capture_cb(TIM_HandleTypeDef *htim)
{
    int32_t le, te, delta;
    /* There must be a leading edge before */
    if (0 != __HAL_TIM_GET_FLAG(&htimEnc2, ENC_INDEX_IT_LEADING_EDGE))
    {
        /* Read the index trailing edge position */
        te = (int32_t)__HAL_TIM_GetCompare(&htimEnc2, ENC_INDEX_TRAILING_EDGE);
        /* Read the index leading edge position */
        le = (int32_t)__HAL_TIM_GetCompare(&htimEnc2, ENC_INDEX_LEADING_EDGE);
        /* Take the difference between readings */
        delta = te - le;
        /* Avoid inversion of direction over the index */
        if (0 != delta)
        {
            /* Update the index position */
            Enc2RotorZero = (uint32_t)((delta>=0)? (le + ENC_UP_COUNTING_OFFSET)
                                                 : (le - ENC_DOWN_COUNTING_OFFSET));
            /* Mark the absolute zero position */
            if (ENC_STATUS_WAIT == Enc2Status)
            {
                /* Store the absolute zero */
                Enc2AbsoluteZero = Enc2RotorZero;
                /* Do not repeat again */
                Enc2Status = ENC_STATUS_READY;
            }
        }
    }
}

bool Enc2Init(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc2, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc2, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc2, TIM_CHANNEL_ALL);

    /* Register the callback function used to signal the activation of the Index pulse */
    if (HAL_OK == HAL_TIM_RegisterCallback(&htimEnc2, HAL_TIM_IC_CAPTURE_CB_ID, Enc2Capture_cb))
    {
        /* Clear local variables */
        Enc2Status = ENC_STATUS_IDLE;
        Enc2AbsoluteZero = 0;
        Enc2RotorZero = 0;
        /* Clear counter */
        __HAL_TIM_SET_COUNTER(&htimEnc2, 0);
        /* Start timers in encoder mode */
        if (HAL_OK == HAL_TIM_Encoder_Start(&htimEnc2, TIM_CHANNEL_ALL))
        {
            /* Enable leading edge capture, without interrupts */
            HAL_TIM_IC_Start(&htimEnc2, ENC_INDEX_LEADING_EDGE);
            /* Enable trailing edge capture, with interrupts */
            HAL_TIM_IC_Start_IT(&htimEnc2, ENC_INDEX_TRAILING_EDGE);
            return true;
        }
        /* Failed start of the timer */
        HAL_TIM_UnRegisterCallback(&htimEnc2, HAL_TIM_IC_CAPTURE_CB_ID);
    }
    /* Errors detected */
    return false;
}

void Enc2DeInit(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc2, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc2, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc2, TIM_CHANNEL_ALL);
    HAL_TIM_UnRegisterCallback(&htimEnc2, HAL_TIM_IC_CAPTURE_CB_ID);
}

int32_t Enc2GetRotorPosition(void)
{
    /* Read counter 32 bits value */
    return __HAL_TIM_GetCounter(&htimEnc2) - Enc2RotorZero;
}
    
void encoder1_test(void)
{   
    uint8_t encoder_slots_number =25;
    do
    {
        embot::core::print
        ( 
                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_10)? "H" : "L" +
                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_11)? "H" : "L" +
                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_12)? "H" : "L " +
                    std::to_string(Enc1GetRotorPosition()) +
                    " angle:" +
                    std::to_string((float)Enc1GetRotorPosition()/(float)encoder_slots_number*360.0)
        );
        embot::core::wait(100);
    } while (1);

}


} // namespace embot::hw::motor::enc {





#endif // #elif defined(EMBOT_ENABLE_hw_motor_enc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

