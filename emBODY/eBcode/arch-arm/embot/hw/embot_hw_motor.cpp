
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
  

using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

static constexpr float DUMMY_HW_MOTOR_FLOAT_VALUE = 6.66f;

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
    //result_t gethallcounter(MOTOR h, Position &position)  { return resNOK; }    
    result_t gethallstatus(MOTOR h, HallStatus &hs)  { return resNOK; }     
    result_t setpwm(MOTOR h, Pwm v) { return resNOK; }      
    result_t setpwm(MOTOR h, Pwm u, Pwm v, Pwm w) { return resNOK; }       
    result_t setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner) { return resNOK; }   
    result_t init(MOTOR h, const Config &config) { return resNOK; } 
    result_t configure(embot::hw::MOTOR h, const Config &config) { return resNOK; }   
    result_t setPWM(MOTOR h, const PWMperc &p) { return resNOK; }
    float getVIN() { return DUMMY_HW_MOTOR_FLOAT_VALUE; }
    float getCIN() { return DUMMY_HW_MOTOR_FLOAT_VALUE; }
}}} // namespace embot { namespace hw { namespace MOTOR {


#elif defined(EMBOT_ENABLE_hw_motor)

#if defined(STM32HAL_BOARD_AMC2C)
    // the case we use for the second core of the amc (so motor control on cm4)
    #include "motorhal.h"     
#elif defined(STM32HAL_BOARD_AMCFOC_1CM7) || defined(STM32HAL_BOARD_AMCFOC_2CM4)
    // used by the amcfoc.mot running on cm7 or cm4
    #include "embot_hw_motor_adc.h"  
    #include "embot_hw_motor_enc.h"  
    #include "embot_hw_motor_hall.h"  
    #include "embot_hw_motor_pwm.h"  
#endif    


namespace embot { namespace hw { namespace motor {

    // masks       
    static std::uint32_t initialisedmask = 0;
    static std::uint32_t enabledmask = 0;
    static std::uint32_t faultedmask = 0;
    
    bool supported(MOTOR h)
    {
        return embot::hw::motor::bsp::getBSP().supported(h);
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
    result_t s_hw_configure(MOTOR h, const Config &cfg);
    Position s_hw_getencoder(MOTOR h);
    // Position s_hw_gethallcounter(MOTOR h);    
    
    
    HallStatus s_hw_gethallstatus(MOTOR h);
    result_t s_hw_setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w);
    result_t s_hw_setpwmUVWperc(MOTOR h, const PWMperc &p);
    result_t s_hw_motorEnable(MOTOR h);
    result_t s_hw_motorDisable(MOTOR h);
    result_t s_hw_setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner);
    
    // fault check functions
    bool s_hw_isHardwareFault(MOTOR h);
    bool s_hw_isHallSequenceError(MOTOR h);
    bool s_hw_isHallValueError(MOTOR h);
    bool s_hw_isOvercurrent(MOTOR h);
    uint32_t s_hw_get32bit2FOCstyleFaultMask(MOTOR h);
    float s_hw_getVin();
    float s_hw_getCin();
    
    result_t init(MOTOR h, const Config &cfg)
    {
        if(false == supported(h))
        {
            return resNOK;
        }
        
        if(true == initialised(h))
        {
            s_hw_deinit(h);
        }
                
        uint8_t index = embot::core::tointegral(h);
        
        // prepare the hw environment. something such as prepare GPIOs, TIMs, ADC, link them together, etc.
        embot::hw::motor::bsp::getBSP().init(h);                           
        
        // load config etc
        s_privatedata.config[index] = cfg;
      
        // init the hw. something such as start the TIMs, the ADCs, etc
        if(resOK != s_hw_init(h)) 
        {
            return resNOK;
        } 
        
        // placed in here so that we can call configure()
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(h));
        
        // make sure that the ... way we use the HW peripherals is with the correct parameters
        if(true == cfg.isvalid())
        {   // ok i know there is a double check vs validity of cfg
            configure(h, cfg);
        }            
        
        return resOK;
    }
  
    
    result_t configure(MOTOR h, const Config&cfg)
    {
        if(false == initialised(h))
        {
            return resNOK;
        }
        
        if(false == cfg.isvalid())
        {
            return resNOK;
        }
        
        s_privatedata.config[embot::core::tointegral(h)] = cfg;            
        return s_hw_configure(h, cfg);
    }     
 
    
    result_t config(MOTOR h, bool  has_quad_enc, int16_t  enc_resolution, 
                    uint8_t  pwm_num_polar_couples, bool  pwm_has_hall_sens,
                    bool  pwm_swapBC, uint16_t pwm_hall_offset)
    {
        if(false == initialised(h))
        {
            return resNOK;
        }
        
        Config cfg { enc_resolution, pwm_num_polar_couples, pwm_hall_offset,
                     has_quad_enc, pwm_has_hall_sens, pwm_swapBC };  
        return configure(h, cfg);
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
    }    

    
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
  

    result_t setpwm(MOTOR h, Pwm u, Pwm v, Pwm w)
    {        
        if(false == enabled(h))
        {
            return resNOK;
        }
        
        return s_hw_setpwmUVW(h, u, v, w);
    }

    result_t setPWM(MOTOR h, const PWMperc &p)
    {
        if(false == enabled(h))
        {
            return resNOK;
        }
        
        return s_hw_setpwmUVWperc(h, p);
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
    
    
//    result_t gethallcounter(MOTOR h, Position &position)
//    {
//        if(false == initialised(h))
//        {
//            return resNOK;
//        } 
//        
//        position = s_hw_gethallcounter(h);
//        
//        return resOK;               
//    }
    
    
    result_t gethallstatus(MOTOR h, HallStatus &hs)
    {
        if(false == initialised(h))
        {
            return resNOK;
        } 
        
        hs = s_hw_gethallstatus(h);
        
        return resOK;               
    }
    
//    float angle(embot::hw::MOTOR h)
//    {
//        return  embot::hw::motor::hall::angle(h);
//    }

       
    result_t setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner)
    {
        return s_hw_setCallbackOnCurrents(h, callback, owner);
    }
    
    bool isHallSequenceError(MOTOR h)
    {
        return s_hw_isHallSequenceError(h);
    }
    
    bool isHallValueError(MOTOR h)
    {
        return s_hw_isHallValueError(h);
    }
    
    bool isOvercurrent(MOTOR h)
    {
        return s_hw_isOvercurrent(h);
    }
    
    bool isHardwareFault(MOTOR h)
    {
        return s_hw_isHardwareFault(h);
    }
    
    float getVIN()
    {
        return s_hw_getVin();
    }

    float getCIN()
    {
        return s_hw_getCin();        
    }
    
// in here is the part for low level hw of the boards (amc2c or amcbldc or amcfoc)
// it is better, for now, to clearly separate the three cases      
    
#if defined(STM32HAL_BOARD_AMC2C)


    result_t s_hw_init(MOTOR h)
    {
        // i want to be sure that the pwm is not active       
        embot::hw::motor::pwm::deinit(); 

        // adc acquisition of the currents starts straigth away with ::init()
        embot::hw::motor::adc::init({}); 
         
        // then we init the encoder. we actually dont start acquisition because we do that in enc::start()            
        embot::hw::motor::enc::init({}); 
        // same applies for hall 
        embot::hw::motor::hall::init({});   
            
        // ok, we start pwm
        embot::hw::motor::pwm::init({});
          
        // now we calibrate adc acquisition
        embot::hw::motor::adc::calibrate({});

        // now we start analog acquisition
        embot::hw::analog::init({});
            
        // we may calibrate also the encoder so that it is aligned w/ hall values
        // but maybe better do it later

        return resOK; 
    }
    
    result_t s_hw_deinit(MOTOR h)
    {
        embot::hw::motor::adc::deinit(); ;
        embot::hw::motor::enc::deinit(); 
        embot::hw::motor::hall::deinit(); 
        embot::hw::motor::pwm::deinit();
        
        return resOK;
    }    
 
    result_t s_hw_configure(MOTOR h, const Config &cfg)
    { 
        result_t res {resNOK};
        
        if((true == cfg.has_quad_enc) && (0 != cfg.enc_resolution) && (cfg.pwm_num_polar_couples > 0))
        {
            // start the encoder
            embot::hw::motor::enc::Mode mode {cfg.enc_resolution, cfg.pwm_num_polar_couples, false, false};
            embot::hw::motor::enc::start(mode);
        }
        
        if(true == cfg.pwm_has_hall_sens)
        {
            // start the hall acquisition
             embot::hw::motor::hall::Mode mode { cfg.pwm_swapBC ?  embot::hw::motor::hall::Mode::SWAP::BC :  embot::hw::motor::hall::Mode::SWAP::none, cfg.pwm_hall_offset };
             embot::hw::motor::hall::start(mode);
        }
                
      
        return resOK; 
    }  
    
    result_t s_hw_motorEnable(MOTOR h)
    {
        embot::hw::motor::pwm::enable(true);
        return resOK;
    }
    
    result_t s_hw_motorDisable(MOTOR h)
    {
        embot::hw::motor::pwm::enable(false);
        return resOK;
    }    

    Position s_hw_getencoder(MOTOR h)
    {
        Position p {0};
        if(true == embot::hw::motor::enc::isstarted())
        {
           p = embot::hw::motor::enc::getvalue();
        }
        else if(true == embot::hw::motor::hall::isstarted())
        {
           p = embot::hw::motor::hall::getangle();
        }
        return p;
    }

//    Position s_hw_gethallcounter(MOTOR h)
//    {
//        #warning TODO-embot::hw::motor: .... get hall
//        return 0;        
////        return hallGetCounter();
//    }
        
    HallStatus s_hw_gethallstatus(MOTOR h)
    {       
        return  embot::hw::motor::hall::getstatus();
    }

    
    result_t s_hw_setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w)
    {   
        embot::hw::motor::pwm::set(u, v, w);        
        return resOK;
    }   

    result_t s_hw_setpwmUVWperc(MOTOR h, const PWMperc &p)
    {   
        embot::hw::motor::pwm::setperc(p.a, p.b, p.c);        
        return resOK;
    }  
    
   
    
    result_t s_hw_setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner)
    {        
        if((nullptr == callback))
        {
            return resNOK;
        }      
        
        embot::hw::motor::adc::OnCurrents cbk {reinterpret_cast<embot::hw::motor::adc::OnCurrents::Action>(callback), owner};
        embot::hw::motor::adc::set(cbk);
        return resOK;   
    }
    
    bool s_hw_isHardwareFault(MOTOR h){ return false; }
    bool s_hw_isHallSequenceError(MOTOR h){ return false; }
    bool s_hw_isHallValueError(MOTOR h){ return false; }
    bool s_hw_isOvercurrent(MOTOR h){ return false; }
    uint32_t s_hw_get32bit2FOCstyleFaultMask(MOTOR h){ return 0; }
    
    float s_hw_getVin()
    {
        return embot::hw::analog::getVin();
    }
    
    float s_hw_getCin()
    {
        return embot::hw::analog::getCin();
    }    

#elif defined(STM32HAL_BOARD_AMCBLDC)
    
    // even for the amcbldc we may have several generations of drivers.
    // we use macro EMBOT_AMCBLDC_APPxx to discriminate amongst them.
    
    #include "encoder.h"
    #include "pwm.h"
    #include "analog.h"
    #include "motorhal_config.h"
    #include "motorhal_faults.h"
    
    result_t s_hw_init(MOTOR h)
    {
        // motor+sensors configuration
        memset(&MainConf, 0, sizeof(MainConf));

        motorhal_clear_faultmask();
        
        analogInit();  
        encoderInit(); 
        hallInit();    
        pwmInit();
        
        return resOK; 
    }
    
    result_t s_hw_deinit(MOTOR h)
    {
        // pwm_Deinit(); ?? marco.accame: why is it commented out? 
        analog_Deinit();
        encoder_Deinit();
        pwm_hallDeinit();
        
        return resOK;
    }
    
    result_t s_hw_configure(MOTOR h, const Config &cfg)
    {
        result_t res = resOK;
        
//        s_hw_deinit(h);        
//        s_hw_init(h);
        
        if(HAL_OK != encoder_Config(cfg.has_quad_enc, cfg.enc_resolution, cfg.pwm_num_polar_couples, cfg.pwm_has_hall_sens)) res = resNOK;
        if(HAL_OK != pwm_hallConfig(cfg.pwm_swapBC, cfg.pwm_hall_offset)) res = resNOK;
      
        return res; 
    }
    
    Position s_hw_getencoder(MOTOR h)
    {
        return encoderGetElectricalAngle();
    }

//    Position s_hw_gethallcounter(MOTOR h)
//    {
//        return hallGetCounter();
//    }
        
    HallStatus s_hw_gethallstatus(MOTOR h)
    {
        return hallGetStatus();
    }
    
    result_t s_hw_setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w)
    {        
        pwmSet(u, v, w);       
        return resOK;
    }
    
    result_t s_hw_setpwmUVWperc(MOTOR h, const PWMperc &p)
    {   
        pwm_SetPerc(p.a, p.b, p.c);        
        return resOK;
    }      
        
    static_assert(sizeof(pwm_Currents_t) == sizeof(embot::hw::motor::Currents), "embot::hw::motor::Currents and pwm_Currents_t differs");
    static_assert(sizeof(pwm_Currents_t::u) == sizeof(embot::hw::motor::Currents::u), "embot::hw::motor::Currents and pwm_Currents_t differs");
    static_assert(sizeof(pwm_Currents_t::v) == sizeof(embot::hw::motor::Currents::v), "embot::hw::motor::Currents and pwm_Currents_t differs");
    static_assert(sizeof(pwm_Currents_t::w) == sizeof(embot::hw::motor::Currents::w), "embot::hw::motor::Currents and pwm_Currents_t differs");
   
    result_t s_hw_setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner)
    {
        if((nullptr == callback))
        {
            return resNOK;
        }      
        
        pwm_ADC_callback_t cbk {};
        cbk.callback = reinterpret_cast<pwm_fp_adc_callback_t>(callback);
        cbk.owner = owner;
        pwm_set_ADC_callback(&cbk);
        return resOK;      
    }

    
    result_t s_hw_motorEnable(MOTOR h)
    {
        pwmPhaseEnable(PWM_PHASE_ALL);
        return resOK;
    }
 
    
    result_t s_hw_motorDisable(MOTOR h)
    {
        motorhal_clear_faultmask();
        
        pwmPhaseDisable(PWM_PHASE_ALL);
        return resOK;
    }
    
    bool s_hw_isHardwareFault(MOTOR h)
    { 
        return motorhal_get_faultmask() != 0; 
    }
    
    bool s_hw_isHallSequenceError(MOTOR h)
    {
        return motorhal_check_fault(DHESInvalidSequence);
    }
    
    bool s_hw_isHallValueError(MOTOR h)
    { 
        return motorhal_check_fault(DHESInvalidValue);
    }
    
    bool s_hw_isOvercurrent(MOTOR h)
    {
       return motorhal_check_fault(OverCurrentFailure);
    }
    
    uint32_t s_hw_get32bit2FOCstyleFaultMask(MOTOR h)
    {
        return motorhal_get_faultmask();
    }
    
    float s_hw_getVin()
    {
#if defined(EMBOT_ENABLE_hw_analog_ish)        
        // Return Vin in Volts
        return analogVin() * 0.001;
#else
        return 18.0f;
#endif        
    }
    
    float s_hw_getCin()
    {
#if defined(EMBOT_ENABLE_hw_analog_ish)         
        return analogCin() * 0.001;
#else
        return 1.0f;
#endif        
    } 


#elif defined(STM32HAL_BOARD_AMCFOC_1CM7) || defined(STM32HAL_BOARD_AMCFOC_2CM4)

    // this part is actually .... used by embot::hw::motor::bldc that call embot::hw::motor that call these functions.
    // so, better move them in the future and use just embot::hw::motor::bldc tha directly calls them.
    // also: these functions much depend on the board, so it would be better to meove them in the bsp section    
    
    
    result_t s_hw_init(MOTOR h)
    {
        // i want to be sure that the pwm is not active       
        embot::hw::motor::pwm::deinit(h); 

        // adc acquisition of the currents starts straigth away with ::init()
        embot::hw::motor::adc::init(h, {}); 
         
        // then we init the encoder. we actually dont start acquisition because we do that in enc::start()            
        embot::hw::motor::enc::init({}); 
        // same applies for hall 
        embot::hw::motor::hall::init(h, {});   
            
        // ok, we start pwm
        embot::hw::motor::pwm::init(h, {});
          
        // now we calibrate adc acquisition
        embot::hw::motor::adc::calibrate(h, {});

        // now we start analog acquisition
        embot::hw::analog::init({});
            
        // we may calibrate also the encoder so that it is aligned w/ hall values
        // but maybe better do it later

        return resOK; 
    }
    
    result_t s_hw_deinit(MOTOR h)
    {
        embot::hw::motor::adc::deinit(h);
        embot::hw::motor::enc::deinit(); 
        embot::hw::motor::hall::deinit(h); 
        embot::hw::motor::pwm::deinit(h);
        
        return resOK;
    }    
 
    result_t s_hw_configure(MOTOR h, const Config &cfg)
    { 
        result_t res {resNOK};
        
        if((true == cfg.has_quad_enc) && (0 != cfg.enc_resolution) && (cfg.pwm_num_polar_couples > 0))
        {
            // start the encoder
            embot::hw::motor::enc::Mode mode {cfg.enc_resolution, cfg.pwm_num_polar_couples, false, false};
            embot::hw::motor::enc::start(mode);
        }
        
        if(true == cfg.pwm_has_hall_sens)
        {
            // start the hall acquisition
             embot::hw::motor::hall::Mode mode { cfg.pwm_swapBC ?  embot::hw::motor::hall::Mode::SWAP::BC :  embot::hw::motor::hall::Mode::SWAP::none, cfg.pwm_hall_offset };
             embot::hw::motor::hall::start(h, mode);
        }
                
      
        return resOK; 
    }  
    
    result_t s_hw_motorEnable(MOTOR h)
    {
        embot::hw::motor::pwm::enable(h, true);
        return resOK;
    }
    
    result_t s_hw_motorDisable(MOTOR h)
    {
        embot::hw::motor::pwm::enable(h, false);
        return resOK;
    }    

    Position s_hw_getencoder(MOTOR h)
    {
        Position p {0};
        if(true == embot::hw::motor::enc::isstarted())
        {
           p = embot::hw::motor::enc::getvalue();
        }
        else if(true == embot::hw::motor::hall::isstarted(h))
        {
           p = embot::hw::motor::hall::getangle(h);
        }
        return p;
    }

//    Position s_hw_gethallcounter(MOTOR h)
//    {
//        #warning TODO-embot::hw::motor: .... get hall
//        return 0;        
////        return hallGetCounter();
//    }
        
    HallStatus s_hw_gethallstatus(MOTOR h)
    {       
        return embot::hw::motor::hall::getstatus(h);
    }

    
    result_t s_hw_setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w)
    {   
        embot::hw::motor::pwm::set(h, u, v, w);        
        return resOK;
    }   

    result_t s_hw_setpwmUVWperc(MOTOR h, const PWMperc &p)
    {   
        embot::hw::motor::pwm::setperc(h, p.a, p.b, p.c);        
        return resOK;
    }  
    
   
    
    result_t s_hw_setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner)
    {        
        if((nullptr == callback))
        {
            return resNOK;
        }      
        // we dont use this ....
        return resNOK;   
    }
    
    
    bool s_hw_isHardwareFault(MOTOR h){ return false; }
    bool s_hw_isHallSequenceError(MOTOR h){ return false; }
    bool s_hw_isHallValueError(MOTOR h){ return false; }
    bool s_hw_isOvercurrent(MOTOR h){ return false; }
    uint32_t s_hw_get32bit2FOCstyleFaultMask(MOTOR h){ return 0; }
    
    float s_hw_getVin()
    {
        return embot::hw::analog::getVin();
    }
    
    float s_hw_getCin()
    {
        return embot::hw::analog::getCin();
    } 


#else
    #error define a board
#endif    
    
    
 
}}} // namespace embot { namespace hw { namespace motor {



#endif // #elif defined(EMBOT_ENABLE_hw_motor)
 



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

