
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_motor_bsp.h"

#include <cstring>
#include <vector>

#include "embot_core_binary.h"
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


#if defined(EMBOT_ENABLE_hw_motor_emulatedMODE)

#warning EMBOT_ENABLE_hw_motor_emulatedMODE is defined, the hw driver of the motor will ... 

//#include "embot_os_theCallbackManager.h"
//#include "embot_os_Timer.h"

namespace embot { namespace hw { namespace motor {

// we place in here what we need to simulate the motor

}}}

#endif

using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

std::string embot::hw::motor::to_string(embot::hw::MOTOR id)
{
    constexpr std::array<const char *, embot::core::tointegral(embot::hw::MOTOR::maxnumberof)> MOTOR_map =
    { 
        "MOTOR::one", "MOTOR::two", "MOTOR::three", "MOTOR::four"
    };
    uint8_t pos = embot::core::tointegral(id);
    return (pos < MOTOR_map.size()) ? MOTOR_map[pos] : "MOTOR::none";    
}


#if !defined(EMBOT_ENABLE_hw_motor)

namespace embot { namespace hw { namespace motor {

    bool supported(MOTOR h) { return false; }
    bool initialised(MOTOR h) { return false; }
    result_t init(MOTOR h, const Config &config) { return resNOK; }    

}}} // namespace embot { namespace hw { namespace MOTOR {


#else

namespace embot { namespace hw { namespace motor {
              
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(MOTOR h)
    {
        return embot::hw::motor::getBSP().supported(h);
    }
    
    bool initialised(MOTOR h)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(h));
    }    
    
         

    struct TBDef
    {
        static constexpr std::uint8_t rxdatasize = 8;
        MOTOR id;
        volatile bool done;
        volatile bool ongoing;
        volatile Position position;
        embot::core::Callback userdefCBK;
        void init(MOTOR _id) { clear(); id = _id; }        
        void clear() { done = false; ongoing = false; position = 0; userdefCBK.clear(); }         
    };
    
    
    struct PrivateData
    {  
        std::array<Config, embot::core::tointegral(MOTOR::maxnumberof)> config;        
        std::array<TBDef, embot::core::tointegral(MOTOR::maxnumberof)> tbdef;
        PrivateData() { }
    };
    

    // this device works with no register addressing.
    // static const embot::hw::i2c::Reg registerToRead {embot::hw::i2c::Reg::addrNONE, embot::hw::i2c::Reg::Size::eightbits};
    
    static PrivateData s_privatedata;
    
    result_t s_hw_init(MOTOR h);
    Position s_hw_getencoder(MOTOR h);
    Position s_hw_gethallcounter(MOTOR h);
    
    uint8_t s_hw_gethallstatus(MOTOR h);
    result_t s_hw_setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w);
    result_t s_hw_setADCcallback(MOTOR h, void (*fn_cb)(int16_t[3], void*, void*), void* rtu, void* rty);
    result_t s_hw_motorEnable(MOTOR h);
    result_t s_hw_motorDisable(MOTOR h);

    result_t init(MOTOR h, const Config &config)
    {
        if(false == supported(h))
        {
            return resNOK;
        }
        
        if(true == initialised(h))
        {
            return resOK;
        }
        
        
        std::uint8_t index = embot::core::tointegral(h);
               
        
#if !defined(EMBOT_ENABLE_hw_motor_emulatedMODE)        
        // init peripheral
        embot::hw::motor::getBSP().init(h);                             
        // else
#endif 
        
        // load config etc
        s_privatedata.config[index] = config;

#if !defined(EMBOT_ENABLE_hw_motor_emulatedMODE)          
        // proper init
        if(resOK != s_hw_init(h))
        {
            return resNOK;
        }            
#else
        // we emulate the ...
//        emulatedMODE_timers4callback[embot::core::tointegral(h)] = new embot::os::Timer;  
#endif

        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(h));                
        return resOK;
    }

    result_t motorEnable(MOTOR h)
    {
        if(false == supported(h))
        {
            return resNOK;
        }
        
        return s_hw_motorEnable(h);
    }
    
    result_t motorDisable(MOTOR h)
    {
        if(false == supported(h))
        {
            return resNOK;
        }
        
        return s_hw_motorDisable(h);
    }
    
    result_t getencoder(MOTOR h, Position &position)
    {
        if(false == initialised(h))
        {
            return resNOK;
        } 
        
        //std::uint8_t index = embot::core::tointegral(h);
        //position = s_privatedata.tbdef[index].position;
        position = s_hw_getencoder(h);
        
        return resOK;               
    }
    
    
    result_t gethallcounter(MOTOR h, Position &position)
    {
        if(false == initialised(h))
        {
            return resNOK;
        } 
        
        //std::uint8_t index = embot::core::tointegral(h);
        //position = s_privatedata.tbdef[index].position;
        position = s_hw_gethallcounter(h);
        
        return resOK;               
    }
    
    result_t gethallstatus(MOTOR h, uint8_t &hs)
    {
        if(false == initialised(h))
        {
            return resNOK;
        } 
        
        //std::uint8_t index = embot::core::tointegral(h);
        //position = s_privatedata.tbdef[index].position;
        hs = s_hw_gethallstatus(h);
        
        return resOK;               
    }
    
    result_t setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w)
    {
        return s_hw_setpwmUVW(h, u, v, w);
    }
    
    result_t setADCcallback(MOTOR h, void (*fn_cb)(int16_t[3], void*, void*), void* rtu, void* rty)
    {
        return s_hw_setADCcallback(h, fn_cb, rtu, rty);
    }
    
// in here is the part for low level hw of the amcbldc
    
#if !defined(STM32HAL_BOARD_AMCBLDC)
    result_t s_hw_init(MOTOR h)
    {
        return resNOK; 
    }

#else
    
    #include "encoder.h"
    #include "pwm.h"
    #include "analog.h"
    #include "motorhal_config.h"
    
    result_t s_hw_init(MOTOR h)
    {
        // step 1: what cube mx does
        MX_ADC1_Init();
        MX_ADC2_Init();
        
        MX_TIM1_Init();
        MX_TIM3_Init();
        MX_TIM2_Init();
        MX_CORDIC_Init();
        MX_FMAC_Init();
        MX_CRC_Init();   

        // step 2: what giorgio zini does
        memset(&MainConf, 0, sizeof(MainConf));
        
        analogInit();
        encoderInit();
        hallInit();
        pwmInit();

        HAL_GPIO_WritePin(VAUXEN_GPIO_Port, VAUXEN_Pin, GPIO_PIN_SET);
        HAL_Delay(10); 


        // ok, we are now ready to use the driver
        // or not. we should test it
        
        return resOK; 
    }
    
    Position s_hw_getencoder(MOTOR h)
    {
        return encoderGetElectricalAngle();
    }

    Position s_hw_gethallcounter(MOTOR h)
    {
        return hallGetCounter();
    }
    
    uint8_t s_hw_gethallstatus(MOTOR h)
    {
        return hallGetStatus();
    }
    
    result_t s_hw_setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w)
    {        
        pwmSet(u, v, w);
        return resOK;
    }
    
    result_t s_hw_setADCcallback(MOTOR h, void (*fn_cb)(int16_t[3], void*, void*), void* rtu, void* rty)
    {
        setADC_cb(fn_cb, rtu, rty);
        return resOK;
    }
    
    result_t s_hw_motorEnable(MOTOR h)
    {
        pwmPhaseEnable(PWM_PHASE_ALL);
        return resOK;
    }
    result_t s_hw_motorDisable(MOTOR h)
    {
        pwmPhaseDisable(PWM_PHASE_ALL);
        return resOK;
    }
    
#endif    
    
    
 
}}} // namespace embot { namespace hw { namespace motor {






#endif //defined(EMBOT_MOTOR_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

