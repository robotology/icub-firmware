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
    
}

#elif defined(EMBOT_ENABLE_hw_motor_enc)

#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
#include "embot_hw_motor_bldc_bsp_amcfoc_1cm7.h"
#endif



namespace embot::hw::motor::enc {
    
#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
   
    #define htimEnc1  (embot::hw::motor::bldc::bsp::amcfoc::cm7::hTimEnc1)
    #define htimEnc2  (embot::hw::motor::bldc::bsp::amcfoc::cm7::hTimEnc2)
            
    std::array<TIM_HandleTypeDef, embot::hw::motor::bldc::MAXnumber> htimEnc {htimEnc1,htimEnc2};
    
    std::array<uint8_t, embot::hw::motor::bldc::MAXnumber> QEncMode {bldc::bsp::amcfoc::cm7::QEncoder1Mode,bldc::bsp::amcfoc::cm7::QEncoder2Mode};
    
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

static int32_t  Enc1CounterZeroCross;
static int32_t  Enc1DeltaFirstCross;
//static float    Enc1Angle;
static int32_t  Enc1DeltaCurrentRotation;
static uint8_t  Enc1Divider;
static uint16_t  Enc1SlotsNumber = 1024;
//static float    Enc1Conversionfactor;


static uint8_t  Enc2Status = ENC_STATUS_IDLE;
static uint32_t Enc2RotorZero;
static uint32_t Enc2AbsoluteZero;

static int32_t  Enc2CounterZeroCross;
static int32_t  Enc2DeltaFirstCross;
//static float    Enc2Angle;
static int32_t  Enc2DeltaCurrentRotation;
static uint8_t  Enc2Divider;
//static float    Enc2Conversionfactor;


//struct enc_Conversion
//{
//    int32_t offset {0};
//    int32_t factor {1}; 
//    
//    int32_t convert(int32_t v) const
//    {
//       return offset + (factor*v);
//    }

//    constexpr enc_Conversion() = default;  
//    constexpr enc_Conversion(int32_t o, int32_t f) : offset(o), factor(f) {}        
//};



struct enc_Data
{
  
//    uint8_t  encStatus = ENC_STATUS_IDLE;
    uint32_t encRotorZero = 0;
    uint32_t encFirstIndexRotorZero = 0;
//    uint32_t encAbsoluteZero = 0;
    

    bool firstacquisition {true};       
    
    
    
    
    void reset()
    {
        firstacquisition = true;
        encRotorZero = 0;
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
//            mode.reset();
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
//    /* Stop any pending operation */
//    HAL_TIM_IC_Stop_IT(&(htimEnc[embot::core::tointegral(m)]), ENC_INDEX_TRAILING_EDGE);
//    HAL_TIM_IC_Stop(&htimEnc[embot::core::tointegral(m)], ENC_INDEX_LEADING_EDGE);
//    HAL_TIM_Encoder_Stop(&htimEnc[embot::core::tointegral(m)], TIM_CHANNEL_ALL);
//    HAL_TIM_UnRegisterCallback(&htimEnc[embot::core::tointegral(m)], HAL_TIM_IC_CAPTURE_CB_ID);
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
    
//    _enc_internals._items[embot::core::tointegral(m)].data.conversionFactor = 360.0/_enc_internals._items[embot::core::tointegral(m)].mode.resolution;
    
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

//int32_t getvalue()
//{
//    if(false == _enc_internals.started)
//    {
//        return _enc_internals.forcedvalue;
//    }
//    
//    int32_t v = 0; //__HAL_TIM_GetCounter(&ENC_TIM);
//    
//    return _enc_internals.conversion.convert(v);
//}

//void force(int32_t value)
//{
//    _enc_internals.forcedvalue = value;
//}




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
            Enc1RotorZero = (uint32_t)((delta>=0)? (le + ENC_UP_COUNTING_OFFSET)
                                                 : (le - ENC_DOWN_COUNTING_OFFSET));
            
            _enc_internals._items[0].data.encRotorZero = Enc1RotorZero;
            
            if(false == firstcross)
            {
                _enc_internals._items[0].data.encFirstIndexRotorZero = Enc1RotorZero;
                firstcross = true;    
            } 
            
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
float Enc1GetAngle(void)
{
    return (float)__HAL_TIM_GetCounter(&htimEnc1)*_enc_internals._items[0].conversionfactor;
    //return 360.0*(Enc1CounterZeroCross + (float)(Enc1DeltaFirstCross + (__HAL_TIM_GetCounter(&htimEnc1) - Enc1RotorZero))/(float)(Enc1SlotsNumber*Enc1Divider));
}

int32_t Enc1GetRotorPosition(void)
{
    /* Read counter 32 bits value */
    return __HAL_TIM_GetCounter(&htimEnc1) - Enc1RotorZero;
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
        embot::core::print("Enc Divider = 4");
        _enc_internals._items[motorIndex].divider = 4;
        Enc1Divider = 4;
    }
    else if (TIM_ENCODERMODE_TI2  == QEncMode[motorIndex]) 
    {
//        embot::core::print("Enc1Divider = 2");
        _enc_internals._items[motorIndex].divider = 2;
        Enc1Divider = 2;
    }
    else if (TIM_ENCODERMODE_TI1  == QEncMode[motorIndex]) 
    {
//        embot::core::print("Enc1Divider = 1");
        _enc_internals._items[motorIndex].divider = 1;
        Enc1Divider = 1;
    }

    _enc_internals._items[motorIndex].conversionfactor = 360.0/(float)_enc_internals._items[motorIndex].divider/(float)_enc_internals._items[motorIndex].mode.resolution;
//    Enc1Conversionfactor = 360.0/(float)Enc1Divider/(float)_enc_internals._items[0].mode.resolution;
    embot::core::print(
        "Enc Resolution: " +
        std::to_string(_enc_internals._items[motorIndex].mode.resolution)+
        " Enc Conversionfactor: "+
        std::to_string(_enc_internals._items[motorIndex].conversionfactor)
    
    );
    
    
    /* Register the callback function used to signal the activation of the Index pulse */
    if (HAL_OK == HAL_TIM_RegisterCallback(&htimEnc1, HAL_TIM_IC_CAPTURE_CB_ID, Enc1Capture_cb))
    {
        /* Clear local variables */
        Enc1Status = ENC_STATUS_IDLE;
        Enc1AbsoluteZero = 0;
        Enc1RotorZero = 0;
        Enc1CounterZeroCross = 0; 
        Enc1DeltaFirstCross = 0;
        /* Clear counter */
        __HAL_TIM_SET_COUNTER(&htimEnc1, 2147483647); //half timer period
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
            Enc2RotorZero = (uint32_t)((delta>=0)? (le + ENC_UP_COUNTING_OFFSET)
                                                 : (le - ENC_DOWN_COUNTING_OFFSET));
            
            _enc_internals._items[1].data.encRotorZero = Enc2RotorZero;
            if(false == firstcross)
            {
                _enc_internals._items[1].data.encFirstIndexRotorZero = Enc2RotorZero;
                firstcross = true;    
            }
            
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

bool Enc2Init(embot::hw::MOTOR m)
{
    uint8_t motorIndex = embot::core::tointegral(m);
    
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc2, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc2, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc2, TIM_CHANNEL_ALL);
    
    if(TIM_ENCODERMODE_TI12 == QEncMode[motorIndex]) 
    {
        embot::core::print("Enc Divider = 4");
        _enc_internals._items[motorIndex].divider = 4;
        Enc2Divider = 4;
    }
    else if (TIM_ENCODERMODE_TI2  == QEncMode[motorIndex]) 
    {
//        embot::core::print("Enc1Divider = 2");
        _enc_internals._items[motorIndex].divider = 2;
        Enc2Divider = 2;
    }
    else if (TIM_ENCODERMODE_TI1  == QEncMode[motorIndex]) 
    {
//        embot::core::print("Enc1Divider = 1");
        _enc_internals._items[motorIndex].divider = 1;
        Enc2Divider = 1;
    }

    _enc_internals._items[motorIndex].conversionfactor = 360.0/(float)_enc_internals._items[motorIndex].divider/(float)_enc_internals._items[motorIndex].mode.resolution;
//    Enc2Conversionfactor = 360.0/(float)Enc2Divider/(float)_enc_internals._items[1].mode.resolution; 
    

    /* Register the callback function used to signal the activation of the Index pulse */
    if (HAL_OK == HAL_TIM_RegisterCallback(&htimEnc2, HAL_TIM_IC_CAPTURE_CB_ID, Enc2Capture_cb))
    {
        /* Clear local variables */
        Enc2Status = ENC_STATUS_IDLE;
        Enc2AbsoluteZero = 0;
        Enc2RotorZero = 0;
        /* Clear counter */
        __HAL_TIM_SET_COUNTER(&htimEnc2, 2147483647);
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
    return __HAL_TIM_GetCounter(&htimEnc2) - Enc2RotorZero;
}


float angle(embot::hw::MOTOR m)
{
    float r = 0.0;
    
    if (0 == embot::core::tointegral(m))
    {          
        //warning!!!!! check this conversion
        float anglenew1 = __HAL_TIM_GetCounter(&htimEnc1)-2147483647;
        r = anglenew1*_enc_internals._items[embot::core::tointegral(m)].conversionfactor;
//        embot::core::print
//        ( 
//                    "counter: " +
//                    std::to_string(__HAL_TIM_GetCounter(&htimEnc1)) +
////                    std::to_string(Enc1GetRotorPosition()) +
//                    "angle: " +
//                    std::to_string((float)Enc1GetRotorPosition()/(float)Enc1SlotsNumber*360.0/Enc1Divider) 
//                        //+ 
////                    "  Enc1RotorZero: " +
////                    std::to_string( Enc1RotorZero) +
////                    " Enc 1 angle (my): " +
////                    std::to_string(Enc1GetAngle())    
//        );
    }
    else if (1 == embot::core::tointegral(m))
    {
        r = (float)(__HAL_TIM_GetCounter(&htimEnc2)-2147483647)*_enc_internals._items[embot::core::tointegral(m)].conversionfactor;
    }

    return r;
}




void encoder1_test(void)
{   
    static bool onceonly_initted {false};
                
    if(false == onceonly_initted)
    {
        if (true == Enc1Init((embot::hw::MOTOR) 0))
        {
            _enc_internals._items[0].started = true;
            Enc1SlotsNumber = 1024;
        }
        onceonly_initted = true;
    }
    static uint8_t ii=0;
    if (ii++%5 == 0)
    {
        static float angle1 = 0, angleold1 = 0;
        
//        r = (float)__HAL_TIM_GetCounter(&htimEnc1)*_enc_internals._items[embot::core::tointegral(m)].conversionfactor;
        float anglenew1 = (float)Enc1GetRotorPosition();
        float delta1 = anglenew1 - angleold1;
        angle1 = angle1 + delta1;
        angleold1 = anglenew1;
        embot::core::print
        ( 
    //                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_10)? "H" : "L" +
    //                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_11)? "H" : "L" +
    //                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_12)? "H" : "L " +
                    "angle Enc1GetRotorPosition: " +
                    std::to_string(Enc1GetRotorPosition()) +
                    " angle: " +
                    std::to_string(Enc1GetRotorPosition()/(float)Enc1SlotsNumber*360.0/Enc1Divider) + 
                    "  Enc1RotorZero: " +
                    std::to_string( Enc1RotorZero) +
                    " delta: " +
                    std::to_string(delta1) +
                    " angle as counter*factor: " +
                    std::to_string((float)angle1*360/Enc1Divider/Enc1SlotsNumber)
                                     
        );
    }

}



void Enc2DeInit(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc2, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc2, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc2, TIM_CHANNEL_ALL);
    HAL_TIM_UnRegisterCallback(&htimEnc2, HAL_TIM_IC_CAPTURE_CB_ID);
}

void Enc1DeInit(void)
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&htimEnc1, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&htimEnc1, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&htimEnc1, TIM_CHANNEL_ALL);
    HAL_TIM_UnRegisterCallback(&htimEnc1, HAL_TIM_IC_CAPTURE_CB_ID);
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


} // namespace embot::hw::motor::enc {





#endif // #elif defined(EMBOT_ENABLE_hw_motor_enc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

