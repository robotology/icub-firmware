
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_bsp.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_motor_bldc)

namespace embot::hw::motor::bldc::bsp {
    
    constexpr BSP thebsp {}; 
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
    bool BSP::init(embot::hw::MOTOR h) const { return false; }   

    bool BSP::configure(embot::hw::MOTOR m, const Config &config) const { return false; }
    bool BSP::enable(MOTOR m, bool on) const { return false; }
    
    bool BSP::fault(MOTOR m, bool on) const { return false; }

    bool BSP::set(MOTOR m, const embot::hw::motor::bldc::OnCurrents &oncurrents) const { return false; }  
    
    HallStatus BSP::hallstatus(MOTOR m) const { return 0; }
    Angle BSP::angle(MOTOR m, AngleType type) const { return 0.0f; } 
    
    bool BSP::set(MOTOR m, const PWM3 &pwm) const { return false; }
    Voltage BSP::powersupply(MOTOR m) const { return 0.0f; } 
    
} // namespace embot::hw::motor::bldc::bsp {

namespace embot::hw::motor::bldc::bsp::stm32 {


} // namespace embot::hw::motor::bldc::bsp::stm32 {

#elif defined(EMBOT_ENABLE_hw_motor_bldc)


namespace embot::hw::motor::bldc::bsp::impl {
    
    bool init(embot::hw::MOTOR m);
    bool deinit(embot::hw::MOTOR m);
    bool configure(embot::hw::MOTOR m, const Config &cfg);
   
} // namespace embot::hw::motor::bldc::bsp::impl {


// this bsp uses almost exclusively the adc, enc, hall, pwm and analog
// so in here mostly we shall not use board specific code

#include "embot_hw_motor_bldc_adc.h"  
#include "embot_hw_motor_bldc_qenc.h"  
#include "embot_hw_motor_bldc_hall.h"  
#include "embot_hw_motor_bldc_pwm.h" 
#include "embot_hw_analog.h"

#include <array>

namespace embot::hw::motor::bldc::bsp {
    
    constexpr PROP propM1 {};
        
    constexpr std::initializer_list<embot::hw::MOTOR> themotors {
        embot::hw::MOTOR::one 
    };
    constexpr std::array<const PROP*, embot::hw::motor::bldc::MAXnumber> theirproperties  {
        &propM1
    };
    constexpr size_t numberofmotors {themotors.size()};
                   

    constexpr BSP thebsp 
    {     
        themotors,        
        theirproperties
    };   
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
                
    bool BSP::init(embot::hw::MOTOR m) const 
    {

        #if 0
        
        the logic of init() in here is simple.
        - we dont many checks in here because BSP::init() is called by embot::hw::motor::bldc::init()
          that already does the checks, so we dont check if:
          - the motor is correct,
          - the driver is already initted,
          - etc
        - in amcmj1 we have only one motor, so embot::hw::motor::bldc::bsp::impl::init(m) will manage 
          one motor only. obviously in amcfoc where there are two motors that must be sychcronized 
          the init() of any motor will initialize them both. amcmj1 has this logic removed.
        - in amcmj1 we have one connector only for either hall or qenc, so we init only the one sent
        with configuration message and we do that inside BSP:;configure()        
                
        #endif
        
        return embot::hw::motor::bldc::bsp::impl::init(m);

    }
    
    bool BSP::deinit(embot::hw::MOTOR m) const
    {
        #if 0
        
        the logic of BSP::deinit() is the same as init().
        
        #endif    
        
        return embot::hw::motor::bldc::bsp::impl::deinit(m);

    }
    
    bool BSP::configure(embot::hw::MOTOR m, const Config &cfg) const
    {
        #if 0
        
        BSP::configure() does not check vs initted etc because its called by embot::hw::motor::bldc::configure() that already does checks
        
        however, the amcmj1 can have either hall or quadenc as they share the same connector, so in here we must have a praticular care.
        so, we must call deinit() / init() at this stage: deinit() for both and init() only for the one in cfg
        
        #endif   
        
        return embot::hw::motor::bldc::bsp::impl::configure(m, cfg);      

    }
 
    
    bool BSP::enable(MOTOR m, bool on) const
    {
        embot::hw::motor::bldc::pwm::enable(m, on);
        return true;
    }     
    
//    bool BSP::enabled(MOTOR m) const
//    {
//        bool r {false};
//        
//        return r;
//    }     
   
    bool BSP::fault(MOTOR m, bool on) const
    {
        bool r {false};        
        // in this case it does NOTHING
        r = true;
        return r;
    }     


//    bool BSP::faulted(MOTOR m) const
//    {
//        bool r {false};
//        
//        return r;
//    }     



    bool BSP::set(MOTOR m, const embot::hw::motor::bldc::OnCurrents &oncurrents) const
    {
        return embot::hw::motor::bldc::adc::set(m, oncurrents);
    }     

    
    HallStatus BSP::hallstatus(MOTOR m) const
    {
        return embot::hw::motor::bldc::hall::getstatus(m);        
    }     


    Angle BSP::angle(MOTOR m, AngleType type) const
    {
        Angle r {0.0f};
        
        #warning fill BSP::angle(MOTOR m, AngleType type) for amcfoc
        if((type == AngleType::hall_electrical) || (type == AngleType::hall_mechanical))
        {
            r = embot::hw::motor::bldc::hall::angle(m, type);
        }
        else if(type == AngleType::quadenc_mechanical)
        {   
            r = embot::hw::motor::bldc::qenc::angle(m, embot::hw::motor::bldc::qenc::AngleQE::current);
        }
        else if(type == AngleType::quadenc_mechanical_lastindex)
        {
            r = embot::hw::motor::bldc::qenc::angle(m, embot::hw::motor::bldc::qenc::AngleQE::oflatestindexcrossing);
        }
  
        return r;
    }     

    
    bool BSP::set(MOTOR m, const PWM3 &pwm) const
    {
        embot::hw::motor::bldc::pwm::setperc(m, pwm.u, pwm.v, pwm.w);
        return true;
    }     


    Voltage BSP::powersupply(MOTOR m) const
    {
        return embot::hw::analog::get(embot::hw::analog::Value::Vin);
    }     

     
} // namespace embot::hw::motor::bldc::bsp {


// and in here the board specific implementation


#if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc)

namespace embot::hw::motor::bldc::bsp::impl {
    
    bool init(embot::hw::MOTOR m) 
    { 
        return true;
    }

    bool deinit(embot::hw::MOTOR m)
    {
        return true;
    }  

    bool configure(embot::hw::MOTOR m, const Config &cfg)
    {
        return true;
    }

#else
 
namespace embot::hw::motor::bldc::bsp::impl {
    
    std::array<Config, numberofmotors> _configs {};
    
    bool init(embot::hw::MOTOR m)
    {
        // i want to be sure that pwm and others are not active, so i do a deinit before any other thing 
        // boh, i think it may be redundant. but for sure it does not hurt and it is done at startup of amcmj1        
        embot::hw::motor::bldc::bsp::impl::deinit(m);
        
        // adc acquisition of the phase currents (and of voltages) is initialized here but it starts w/ pwm::init()
        // because it is triggered by the TIMx of the pwm
        embot::hw::motor::bldc::adc::init(m, {});                     

        // now i start pwm
        embot::hw::motor::bldc::pwm::init(m, {});  
            
        // and i start also acquisition of analog values (vin, etc)
        embot::hw::analog::init({});
            
        // now we calibrate adc acquisition of phase currents (and of voltages)
        embot::hw::motor::bldc::adc::calibrate(m, {});  
            
        // we dont init the hall and qenc because ... we may have one or the other attached to the same conenctor

        return true;            
    }

    bool deinit(embot::hw::MOTOR m)
    {
        _configs[embot::core::tointegral(m)].clear();
        
        embot::hw::motor::bldc::qenc::deinit(m); 
        embot::hw::motor::bldc::hall::deinit(m); 
        embot::hw::motor::bldc::adc::deinit(m);
        embot::hw::motor::bldc::pwm::deinit(m);
        embot::hw::analog::deinit();  

        return true;        
    } 

    bool configure(embot::hw::MOTOR m, const Config &cfg)
    {
        // this is called at runtime when the amcmj1 receives a motor configuration message
        
        bool r {false};
        
        // if we already have either a qenc or a hall initialized, then we must deinit it first       
        if(true == embot::hw::motor::bldc::hall::initialised(m))
        {
            embot::hw::motor::bldc::hall::deinit(m);
        }            

        if(true == embot::hw::motor::bldc::qenc::initialised(m))
        {
            embot::hw::motor::bldc::qenc::deinit(m);
        }        
        
        _configs[embot::core::tointegral(m)] = cfg;
        
        // we can init only one of qenc or hall. if both are present in configuration message, we give priority to qenc
        bool qencOK = (true == cfg.has_quad_enc) && (0 != cfg.enc_resolution) && (cfg.pwm_num_polar_couples > 0);
        bool hallOK = (true == cfg.pwm_has_hall_sens);
        
        if((false == qencOK) && (false == hallOK))
        {
            _configs[embot::core::tointegral(m)].clear();
            r = false;
        }        
        else if(true == qencOK)
        {
            // init 
            embot::hw::motor::bldc::qenc::init(m, {});
            // and start
            embot::hw::motor::bldc::qenc::Mode mode {cfg.enc_resolution, {}};
            embot::hw::motor::bldc::qenc::start(m, mode);
            r = true;
        }
        else if(true == hallOK)
        {
            // init
            embot::hw::motor::bldc::hall::init(m, {});
            // and start
            embot::hw::motor::bldc::hall::Mode mode { cfg.pwm_swapBC ?  embot::hw::motor::bldc::hall::Mode::Order::H2H3H1 :  embot::hw::motor::bldc::hall::Mode::Order::H3H2H1, cfg.pwm_num_polar_couples, {} };
            embot::hw::motor::bldc::hall::start(m, mode);
            r = true;
        }
                
        return r;
    }   
    
} // namespace embot::hw::motor::bldc::bsp::impl {

// --------------------------------------------------------------------------------------------------------------------
// stm32 msp functions and irq handlers 
// --------------------------------------------------------------------------------------------------------------------



namespace embot::hw::motor::bldc::bsp::stm32 {
        
        
} // namespace embot::hw::motor::bldc::bsp::stm32 {


extern "C"
{

}

#endif // else of EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc


#endif // #elif defined(EMBOT_ENABLE_hw_motor)





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

