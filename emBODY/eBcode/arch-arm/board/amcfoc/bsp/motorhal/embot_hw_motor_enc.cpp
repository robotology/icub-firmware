
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
//    #define ENC_TIM (embot::hw::motor::bldc::bsp::amcfoc::cm7::hTIM5)
#endif    
    
    
//#define ENC_INDEX_LEADING_EDGE          TIM_CHANNEL_3
//#define ENC_INDEX_IT_LEADING_EDGE       TIM_IT_CC3
//#define ENC_INDEX_ACTIVE_LEADING_EDGE   HAL_TIM_ACTIVE_CHANNEL_3
//#define ENC_INDEX_TRAILING_EDGE         TIM_CHANNEL_4
//#define ENC_INDEX_IT_TRAILING_EDGE      TIM_IT_CC4
//#define ENC_INDEX_ACTIVE_TRAILING_EDGE  HAL_TIM_ACTIVE_CHANNEL_4
    

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


    
} // namespace embot::hw::motor::enc {





#endif // #elif defined(EMBOT_ENABLE_hw_motor_enc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

