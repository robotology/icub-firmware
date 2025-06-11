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

#warning EMBOT_ENABLE_hw_motor_enc is undefined, so we use dummy implementation 

namespace embot::hw::motor::enc {
    
    bool  init(embot::hw::MOTOR m, const Configuration &config) { return false; }
    bool  deinit(embot::hw::MOTOR m)                            { return false; }
    bool  start(embot::hw::MOTOR m, const Mode &mode)           { return false; }
    bool  isstarted(embot::hw::MOTOR m)                         { return false; }
    float angle(embot::hw::MOTOR m)                             { return 0.0; }
    float GetencIndexAngle(embot::hw::MOTOR m)                  { return 0.0; }
    float GetencFirstIndexCrossAngle(embot::hw::MOTOR m)        { return 0.0; }
    
}

#elif defined(EMBOT_ENABLE_hw_motor_enc)

#if defined(STM32HAL_BOARD_AMCFOC_1CM7)
#include "embot_hw_motor_bldc_bsp_amcfoc_1cm7.h"
#endif


namespace embot::hw::motor::enc {
    
#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
   
    #define htimEnc1  (embot::hw::motor::bldc::bsp::amcfoc::cm7::hTimEnc1)
    #define htimEnc2  (embot::hw::motor::bldc::bsp::amcfoc::cm7::hTimEnc2)
            
    
    std::array<uint8_t, embot::hw::motor::bldc::MAXnumber> QEncMode {bldc::bsp::amcfoc::cm7::QEncoder1Mode,bldc::bsp::amcfoc::cm7::QEncoder2Mode};
    
#endif    


struct enc_Data
{
    uint8_t  encStatus = ENC_STATUS_IDLE;
    uint32_t encIndexCounter = 0;
    uint32_t encFirstIndexCross = 0;
    uint32_t encAbsoluteZero = 0;
//    static int32_t  EncDeltaCurrentRotation;

    bool firstacquisition {true};       
    
    void reset()
    {
        firstacquisition = true;
        encIndexCounter = 0;
    }
      
    enc_Data() = default;
};



struct enc_Internals
{
    
    struct Item
    {
        bool started {false};
        Configuration config {};
        Mode mode {};    
        enc_Data data {};
        
        uint8_t divider; //depend on timer settings
        float conversionfactor;
            
        Item() = default;

        void reset()
        {
            started = false;
            config.acquisition = Configuration::ACQUISITION::deferred;
            data.reset();            
        }
        
        void load(const Configuration &c) 
        {
            config = c;
        }
    };
    
    std::array<Item, embot::hw::motor::bldc::MAXnumber> _items {};
    
    enc_Internals() = default;   
};


enc_Internals _enc_internals {};
    
bool init(embot::hw::MOTOR m, const Configuration &config)
{
    deinit(m);
    _enc_internals._items[embot::core::tointegral(m)].started = false;
    _enc_internals._items[embot::core::tointegral(m)].config = config;
    return true;
}

bool deinit(embot::hw::MOTOR m)
{
    
    if (0 == embot::core::tointegral(m))
    {
        Enc1DeInit();
    }
    else if (1 == embot::core::tointegral(m))
    {
         Enc2DeInit();
    }


#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
//    embot::hw::motor::bsp::amc::cm7::DeInit_TIM5();
#endif      
        
    _enc_internals._items[embot::core::tointegral(m)].started = false;
    
    return true;
}

bool start(embot::hw::MOTOR m, const Mode& mode)
{
    if(false == mode.isvalid())
    {
        return false;
    }
    
    if(true == _enc_internals._items[embot::core::tointegral(m)].started)
    {   // if already started I first stop. i can do that by calling init({})
        init(m, {});
    }        
           

    _enc_internals._items[embot::core::tointegral(m)].mode = mode;
    
    #warning marco.accame: so far we dont manage mode.calibrate_with_hall and mode.use_index
       
#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
//    embot::hw::motor::bldc::bsp::amcfoc::cm7::Init_TIM5(_enc_internals.mode.resolution, _enc_internals.mode.num_polar_couples);
#endif      
    
    bool ret = true;
    
    if (0 == embot::core::tointegral(m))
    {
        ret = Enc1Init(m);
    }
    else if (1 == embot::core::tointegral(m))
    {
         ret = Enc2Init(m);
    }
    
//     and now i can do what is required ... i also start the index ... who cares
 
    _enc_internals._items[embot::core::tointegral(m)].started = ret;
    
    return ret;
}

bool isstarted(embot::hw::MOTOR m)
{
    return _enc_internals._items[embot::core::tointegral(m)].started;
}


static void Enc1Capture_cb(TIM_HandleTypeDef *htim)
{
    static bool firstcross = false;
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
            _enc_internals._items[0].data.encIndexCounter = (uint32_t)((delta>=0)? (le + ENC_UP_COUNTING_OFFSET)
                                                 : (le - ENC_DOWN_COUNTING_OFFSET));
            
            if(false == firstcross)
            {
                _enc_internals._items[0].data.encFirstIndexCross = _enc_internals._items[0].data.encIndexCounter;
                firstcross = true;    
            }
            
            /* Mark the absolute zero position */
            if (ENC_STATUS_WAIT == _enc_internals._items[0].data.encStatus)
            {
                /* Store the absolute zero */
                _enc_internals._items[0].data.encAbsoluteZero = _enc_internals._items[0].data.encIndexCounter;
                /* Do not repeat again */
                _enc_internals._items[0].data.encStatus = ENC_STATUS_READY;
            }
        }
    }
}


int32_t Enc1GetRotorPosition(void)
{
    /* Read counter 32 bits value */
    return __HAL_TIM_GetCounter(&htimEnc1) - _enc_internals._items[0].data.encIndexCounter;
}

bool Enc1Init(embot::hw::MOTOR m)
{
    uint8_t motorIndex = embot::core::tointegral(m);
    
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc1, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc1, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc1, TIM_CHANNEL_ALL);
        
    
    if(TIM_ENCODERMODE_TI12 == QEncMode[motorIndex]) 
    {
//        embot::core::print("Enc1 Divider = 4");
        _enc_internals._items[motorIndex].divider = 4;
    }
    else if ( ( TIM_ENCODERMODE_TI1  == QEncMode[motorIndex] ) || ( TIM_ENCODERMODE_TI2  == QEncMode[motorIndex] ) ) 
    {
//        embot::core::print("Enc1 Divider = 2");
        _enc_internals._items[motorIndex].divider = 2;
    }
    
    _enc_internals._items[motorIndex].conversionfactor = 360.0/(float)_enc_internals._items[motorIndex].divider/(float)_enc_internals._items[motorIndex].mode.resolution;
//    embot::core::print(
//        "Enc Resolution: " +
//        std::to_string(_enc_internals._items[motorIndex].mode.resolution)+
//        " Enc Conversionfactor: "+
//        std::to_string(_enc_internals._items[motorIndex].conversionfactor) 
//    );
    

    /* Register the callback function used to signal the activation of the Index pulse */
    if (HAL_OK == HAL_TIM_RegisterCallback(&htimEnc1, HAL_TIM_IC_CAPTURE_CB_ID, Enc1Capture_cb))
    {
        /* Clear local variables */
        _enc_internals._items[motorIndex].data.encStatus = ENC_STATUS_IDLE;
        _enc_internals._items[motorIndex].data.encAbsoluteZero = 0;
        _enc_internals._items[motorIndex].data.encIndexCounter = 0;
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




static void Enc2Capture_cb(TIM_HandleTypeDef *htim)
{
    static bool firstcross = false;
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
            _enc_internals._items[1].data.encIndexCounter = (uint32_t)((delta>=0)? (le + ENC_UP_COUNTING_OFFSET)
                                                 : (le - ENC_DOWN_COUNTING_OFFSET));
            if(false == firstcross)
            {
                _enc_internals._items[1].data.encFirstIndexCross =  _enc_internals._items[1].data.encIndexCounter;
                firstcross = true;    
            }
            
            /* Mark the absolute zero position */
            if (ENC_STATUS_WAIT == _enc_internals._items[1].data.encStatus)
            {
                /* Store the absolute zero */
                _enc_internals._items[1].data.encAbsoluteZero = _enc_internals._items[1].data.encIndexCounter;
                /* Do not repeat again */
                _enc_internals._items[1].data.encStatus = ENC_STATUS_READY;
            }
        }
    }
}

bool Enc2Init(embot::hw::MOTOR m)
{
    uint8_t motorIndex = embot::core::tointegral(m);
    
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc2, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc2, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc2, TIM_CHANNEL_ALL);
    
    if(TIM_ENCODERMODE_TI12 == QEncMode[motorIndex]) 
    {
//        embot::core::print("Enc2 Divider = 4");
        _enc_internals._items[motorIndex].divider = 4;
    }
    else if ( ( TIM_ENCODERMODE_TI1  == QEncMode[motorIndex] ) || ( TIM_ENCODERMODE_TI2  == QEncMode[motorIndex] ) ) 
    {
//        embot::core::print("Enc2 Divider = 2");
        _enc_internals._items[motorIndex].divider = 2;
    } 

    _enc_internals._items[motorIndex].conversionfactor = 360.0/(float)_enc_internals._items[motorIndex].divider/(float)_enc_internals._items[motorIndex].mode.resolution;
    

    /* Register the callback function used to signal the activation of the Index pulse */
    if (HAL_OK == HAL_TIM_RegisterCallback(&htimEnc2, HAL_TIM_IC_CAPTURE_CB_ID, Enc2Capture_cb))
    {
        /* Clear local variables */
        _enc_internals._items[motorIndex].data.encStatus = ENC_STATUS_IDLE;
        _enc_internals._items[motorIndex].data.encAbsoluteZero = 0;
        _enc_internals._items[motorIndex].data.encIndexCounter = 0;
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



int32_t Enc2GetRotorPosition(void)
{
    /* Read counter 32 bits value */
    return __HAL_TIM_GetCounter(&htimEnc2) - _enc_internals._items[1].data.encIndexCounter;
}


float angle(embot::hw::MOTOR m)
{
    float r = 0.0;
    
    if (0 == embot::core::tointegral(m))
    {          
        //check this conversion
        int32_t anglenew = __HAL_TIM_GetCounter(&htimEnc1);
        r = static_cast <float> (anglenew)*_enc_internals._items[embot::core::tointegral(m)].conversionfactor;
    }
    else if (1 == embot::core::tointegral(m))
    {
        int32_t anglenew = __HAL_TIM_GetCounter(&htimEnc2);
        r = static_cast <float> (anglenew)*_enc_internals._items[embot::core::tointegral(m)].conversionfactor;
    }

    return r;
}

float GetencIndexAngle(embot::hw::MOTOR m)
{
    uint8_t motorIndex = embot::core::tointegral(m);
    return _enc_internals._items[motorIndex].data.encIndexCounter*_enc_internals._items[motorIndex].conversionfactor;
}

float GetencFirstIndexCrossAngle(embot::hw::MOTOR m)
{
    uint8_t motorIndex = embot::core::tointegral(m);
    return _enc_internals._items[motorIndex].data.encFirstIndexCross*_enc_internals._items[motorIndex].conversionfactor;
}


//for debug
void encoder1_test(void)
{   
    static bool onceonly_initted {false};
    static uint8_t  Enc1Divider = 4;   
    static uint16_t Enc1SlotsNumber = 1024;    
    if(false == onceonly_initted)
    {
        if (true == Enc1Init((embot::hw::MOTOR) 0))
        {
            _enc_internals._items[0].started = true;
            onceonly_initted = true;
        }
    }
    static uint8_t ii=0;
    if (ii++%5 == 0)
    {
        int32_t anglenew = __HAL_TIM_GetCounter(&htimEnc1);
        embot::core::print
        ( 
    //                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_10)? "H" : "L" +
    //                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_11)? "H" : "L" +
    //                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_12)? "H" : "L " +
                    "tick Enc1GetRotorPosition: " +
                    std::to_string(Enc1GetRotorPosition()) +
                    " angle: " +
                    std::to_string(Enc1GetRotorPosition()/Enc1SlotsNumber*360.0/Enc1Divider) + 
                    "  Enc1RotorZero: " +
                    std::to_string( _enc_internals._items[0].data.encIndexCounter) +
                    " angle as counter*factor: " +
                    std::to_string((float)anglenew*360.0/Enc1Divider/Enc1SlotsNumber)                  
        );
    }

}

void Enc1DeInit(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc1, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc1, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc1, TIM_CHANNEL_ALL);
    HAL_TIM_UnRegisterCallback(&htimEnc1, HAL_TIM_IC_CAPTURE_CB_ID);
}

void Enc2DeInit(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc2, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc2, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc2, TIM_CHANNEL_ALL);
    HAL_TIM_UnRegisterCallback(&htimEnc2, HAL_TIM_IC_CAPTURE_CB_ID);
}


} // namespace embot::hw::motor::enc {


#endif // #elif defined(EMBOT_ENABLE_hw_motor_enc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

