
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
    bool enabled(MOTOR h) { return false; }    
    bool faulted(MOTOR h) { return false; }    
    result_t fault(MOTOR h, bool on) { return resNOK; }    
    result_t config(
        MOTOR h,         
        uint8_t  has_quad_enc,
        int16_t  enc_resolution, 
        uint8_t  pwm_num_polar_couples, 
        uint8_t  pwm_has_hall_sens,
        uint8_t  pwm_swapBC,
        uint16_t pwm_hall_offset) { return resNOK; }    
    result_t enable(MOTOR h, bool on) { return resNOK; }    
    result_t getencoder(MOTOR h, Position &position) { return resNOK; }   
    result_t gethallcounter(MOTOR h, Position &position)  { return resNOK; }    
    result_t gethallstatus(MOTOR h, HallStatus &hs)  { return resNOK; }     
    result_t setpwm(MOTOR h, Pwm v) { return resNOK; }      
    result_t setpwm(MOTOR h, Pwm u, Pwm v, Pwm w) { return resNOK; }       
    result_t setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner) { return resNOK; }   
    result_t init(MOTOR h, const Config &config) { return resNOK; }   
    
}}} // namespace embot { namespace hw { namespace MOTOR {


#else

namespace embot { namespace hw { namespace motor {
              
    // masks       
    static std::uint32_t initialisedmask = 0;
    static std::uint32_t enabledmask = 0;
    static std::uint32_t faultedmask = 0;
    
    bool supported(MOTOR h)
    {
        return embot::hw::motor::getBSP().supported(h);
    }
    
    bool initialised(MOTOR h)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(h));
    }    
    
    bool enabled(MOTOR h)
    {
        return embot::core::binary::bit::check(enabledmask, embot::core::tointegral(h));
    }
        
    bool faulted(MOTOR h)
    {
        return embot::core::binary::bit::check(faultedmask, embot::core::tointegral(h));
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
    result_t s_hw_deinit(MOTOR h);
    result_t s_hw_configure(
        MOTOR h,
        uint8_t has_quad_enc,
        int16_t  enc_resolution, 
        uint8_t  pwm_num_polar_couples, 
        uint8_t  pwm_has_hall_sens,
        uint8_t  pwm_swapBC,
        uint16_t pwm_hall_offset);
    Position s_hw_getencoder(MOTOR h);
    Position s_hw_gethallcounter(MOTOR h);    
    // result_t s_hw_setpwm(MOTOR h, Pwm v);
    
    
    HallStatus s_hw_gethallstatus(MOTOR h);
    result_t s_hw_setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w);
    result_t s_hw_motorEnable(MOTOR h);
    result_t s_hw_motorDisable(MOTOR h);
    result_t s_hw_setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner);
    
    result_t fault(MOTOR h, bool on)
    {        
        if(false == supported(h))
        {
            return resNOK;
        }
        
        // we always must be able to set the fault on / off, also when we are not initialised yet.
        // because the fault(h, true/false) can be called also before init(h, {});
        
        if(true == on)
        {
            embot::core::binary::bit::set(faultedmask, embot::core::tointegral(h));
            // we disable the action of the motor
            setpwm(h, 0, 0, 0);
            s_hw_motorDisable(h);
            // and we also disable ...
            enable(h, false);
        }
        else
        {
            embot::core::binary::bit::clear(faultedmask, embot::core::tointegral(h));  
            // we dont enable the action of the motor. that must be done explicitly
        }
        
        return resNOK;
    }    
    
    result_t config(
        MOTOR h,         
        uint8_t  has_quad_enc,
        int16_t  enc_resolution, 
        uint8_t  pwm_num_polar_couples, 
        uint8_t  pwm_has_hall_sens,
        uint8_t  pwm_swapBC,
        uint16_t pwm_hall_offset)
    {
        return s_hw_configure(h,
            has_quad_enc,
            enc_resolution, 
            pwm_num_polar_couples, 
            pwm_has_hall_sens,
            pwm_swapBC,
            pwm_hall_offset);
        
        return resOK;
    }
    
    result_t init(MOTOR h, const Config &config)
    {
        if(false == supported(h))
        {
            return resNOK;
        }
        
        if(true == initialised(h))
        {
            s_hw_deinit(h);
            //return resOK;
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
   
    result_t enable(MOTOR h, bool on)
    {
        // we must be at least initialised ...
        if(false == initialised(h))
        {
            return resNOK;
        }
        
        // if we are faulted we cannot enable, so i force on to false    
        if(true == faulted(h))
        {
            on = false;
        }
        
        result_t r = resNOK;
        if(true == on)
        { 
            embot::core::binary::bit::set(enabledmask, embot::core::tointegral(h));
            s_hw_setpwmUVW(h, 0, 0, 0);
            r = s_hw_motorEnable(h);
        }
        else
        {
            embot::core::binary::bit::clear(enabledmask, embot::core::tointegral(h));
            s_hw_setpwmUVW(h, 0, 0, 0);
            r = s_hw_motorDisable(h);            
        }   
        
        return r;        
                
//        // then, we enable only if we are not faulted, else ... we set pwm to zero and disable motors anyway        
//        result_t r = resNOK;
//        if((true == on) && (false == faulted(h)))
//        { 
//            embot::core::binary::bit::set(enabledmask, embot::core::tointegral(h));
//            r = s_hw_motorEnable(h);
//        }
//        else
//        {
//            embot::core::binary::bit::clear(enabledmask, embot::core::tointegral(h));
//            s_hw_setpwmUVW(h, 0, 0, 0);
//            r = s_hw_motorDisable(h);            
//        }
//        return r;
        
    }    

    
    result_t getencoder(MOTOR h, Position &position)
    {
        if(false == initialised(h))
        {
            return resNOK;
        } 
        
        position = s_hw_getencoder(h);
        
        return resOK;               
    }
    
    
    result_t gethallcounter(MOTOR h, Position &position)
    {
        if(false == initialised(h))
        {
            return resNOK;
        } 
        
        position = s_hw_gethallcounter(h);
        
        return resOK;               
    }
    
    result_t gethallstatus(MOTOR h, HallStatus &hs)
    {
        if(false == initialised(h))
        {
            return resNOK;
        } 
        
        hs = s_hw_gethallstatus(h);
        
        return resOK;               
    }

    result_t setpwm(MOTOR h, Pwm v)
    {
#if 1
        // deprecated
        return resNOK;
#else        
// if faulted() we are not enabled() ... so we dont need this extra check        
//        if(true == faulted(h))
//        {
//            s_hw_setpwm(h, 0);
//            return resNOK;
//        }
        
        if(false == enabled(h))
        {
            return resNOK;
        }
        
        return s_hw_setpwm(h, v);
#endif        
    } 
    
    result_t setpwm(MOTOR h, Pwm u, Pwm v, Pwm w)
    {
// if faulted() we are not enabled() ... so we dont need this extra check          
//        if(true == faulted(h))
//        {
//            s_hw_setpwmUVW(h, 0, 0, 0);
//            return resNOK;
//        }
        
        if(false == enabled(h))
        {
            return resNOK;
        }
        
        return s_hw_setpwmUVW(h, u, v, w);
    }
       
    result_t setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner)
    {
        return s_hw_setCallbackOnCurrents(h, callback, owner);
    }
    
// in here is the part for low level hw of the amcbldc
    
#if !defined(STM32HAL_BOARD_AMCBLDC)
    
    #if defined(STM32HAL_BOARD_AMC2C)
    
    #warning TODO: fill in embot::hw::motor the calls to motorhal2 ... maybe move into the bsp.
    // and also clean up a lot of this styff in here
    
    #endif
    
    result_t s_hw_init(MOTOR h)
    {
        return resNOK; 
    }
    
    result_t s_hw_motorEnable(MOTOR h)
    {
        return resNOK;
    }
    
    result_t s_hw_motorDisable(MOTOR h)
    {
        return resNOK;
    }   

    result_t s_hw_configure(  
        MOTOR h,
        uint8_t has_quad_enc,
        int16_t  enc_resolution, 
        uint8_t  pwm_num_polar_couples, 
        uint8_t  pwm_has_hall_sens,
        uint8_t  pwm_swapBC,
        uint16_t pwm_hall_offset)
    { 
        return resNOK;
    }        
    
    result_t s_hw_setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner)
    {
        return resNOK;
    }

#else
    
    // even for the amcbldc we may have several generations of drivers.
    // we use macro EMBOT_AMCBLDC_APPxx to discriminate amongst them.
    
    #include "encoder.h"
    #include "pwm.h"
    #include "analog.h"
    #include "motorhal_config.h"
    
    result_t s_hw_init(MOTOR h)
    {
        // motor+sensors configuration
        memset(&MainConf, 0, sizeof(MainConf));

        analogInit();  
        encoderInit(); 
        hallInit();    
        pwmInit();

        return resOK; 
    }
    
    result_t s_hw_deinit(MOTOR h)
    {
//        pwm_Deinit();
        analog_Deinit();
        encoder_Deinit();
        pwm_hallDeinit();
        
        return resOK;
    }
    
    result_t s_hw_configure(
        MOTOR h,
        uint8_t has_quad_enc,
        int16_t  enc_resolution, 
        uint8_t  pwm_num_polar_couples, 
        uint8_t  pwm_has_hall_sens,
        uint8_t  pwm_swapBC,
        uint16_t pwm_hall_offset)
    {
        result_t res = resOK;
        
//        s_hw_deinit(h);
        
//        s_hw_init(h);
        
        if (HAL_OK != encoder_Config(has_quad_enc, enc_resolution, pwm_num_polar_couples, pwm_has_hall_sens)) res = resNOK;
        if (HAL_OK != pwm_hallConfig(pwm_swapBC, pwm_hall_offset)) res = resNOK;
      
        return res; 
    }
    
    Position s_hw_getencoder(MOTOR h)
    {
        return encoderGetElectricalAngle();
    }

    Position s_hw_gethallcounter(MOTOR h)
    {
        return hallGetCounter();
    }
        
    HallStatus s_hw_gethallstatus(MOTOR h)
    {
        return hallGetStatus();
    }


//    result_t s_hw_setpwm(MOTOR h, Pwm v)
//    { 
//        HAL_StatusTypeDef r = HAL_ERROR;
//      
//        //r = pwmSetValue(v);

//        return (HAL_OK == r) ? resOK : resNOK;      
//    }
    
    result_t s_hw_setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w)
    {        
        pwmSet(u, v, w);
        
        return resOK;
    }
    
    
#if defined(EMBOT_AMCBLDC_APP01) || defined(EMBOT_AMCBLDC_APP02) || defined(EMBOT_AMCBLDC_APP03)
#else    
    static_assert(sizeof(pwm_Currents_t) == sizeof(embot::hw::motor::Currents), "embot::hw::motor::Currents and pwm_Currents_t differs");
    static_assert(sizeof(pwm_Currents_t::u) == sizeof(embot::hw::motor::Currents::u), "embot::hw::motor::Currents and pwm_Currents_t differs");
    static_assert(sizeof(pwm_Currents_t::v) == sizeof(embot::hw::motor::Currents::v), "embot::hw::motor::Currents and pwm_Currents_t differs");
    static_assert(sizeof(pwm_Currents_t::w) == sizeof(embot::hw::motor::Currents::w), "embot::hw::motor::Currents and pwm_Currents_t differs");
#endif
    
    result_t s_hw_setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner)
    {
#if defined(EMBOT_AMCBLDC_APP01) || defined(EMBOT_AMCBLDC_APP02) || defined(EMBOT_AMCBLDC_APP03)
        return resNOK;
#else 
       
        if((nullptr == callback))
        {
            return resNOK;
        }      
        
        pwm_ADC_callback_t cbk {};
        cbk.callback = reinterpret_cast<pwm_fp_adc_callback_t>(callback);
        cbk.owner = owner;
        pwm_set_ADC_callback(&cbk);
        return resOK;
#endif          
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

