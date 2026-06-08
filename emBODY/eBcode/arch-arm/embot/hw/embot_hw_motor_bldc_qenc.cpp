
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_qenc.h"


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


#if !defined(EMBOT_ENABLE_hw_motor_bldc_qenc)

#warning EMBOT_ENABLE_hw_motor_bldc_qenc is undefined, so we use dummy implementation 

namespace embot::hw::motor::bldc::qenc {
    
    bool init(embot::hw::MOTOR m, const Configuration &config)  { return false; }
    bool deinit(embot::hw::MOTOR m)                             { return false; }
    bool initialised(MOTOR m)                                   { return false; }
    bool start(embot::hw::MOTOR m, const Mode &mode)            { return false; }
    bool isstarted(embot::hw::MOTOR m)                          { return false; }
    float angle(embot::hw::MOTOR m, AngleQE aqe)                { return 0.0; }
    
}

#elif defined(EMBOT_ENABLE_hw_motor_bldc_qenc)


#include "embot_hw_motor_bldc_qenc_bsp.h"

namespace embot::hw::motor::bldc::qenc {
      

    struct enc_Internals
    {       
        struct Item
        {
            bool started {false};
            Configuration config {};
            Mode mode {};    
                
            Item() = default;

            void clear()
            {
                started = false;
                config.clear();
                mode.clear();  
            }
            
            void load(const Configuration &c) 
            {
                config = c;
            }
            
            void load(const Mode& mo)
            {
                mode = mo;
                
                if(mode.resolution <= 0)
                {
                    mode.resolution = 1;
                }
            }
        };
        
        uint8_t initialisedmask {0};
        std::array<Item, embot::hw::motor::bldc::MAXnumber> _items {};
        
        enc_Internals() = default;   
    };


    enc_Internals _enc_internals {};
        
        
    bool init(embot::hw::MOTOR m, const Configuration &config)
    {
        if(true == initialised(m))
        {
            deinit(m);
        }
        
        _enc_internals._items[embot::core::tointegral(m)].started = false;
        _enc_internals._items[embot::core::tointegral(m)].load(config);
        embot::core::binary::bit::set(_enc_internals.initialisedmask, embot::core::tointegral(m));        
        
        embot::hw::motor::bldc::qenc::bsp::getBSP().init(m);

        return true;
    }

    
    bool deinit(embot::hw::MOTOR m)
    { 
        embot::hw::motor::bldc::qenc::bsp::getBSP().deinit(m);
                
        _enc_internals._items[embot::core::tointegral(m)].clear();
        
        embot::core::binary::bit::clear(_enc_internals.initialisedmask, embot::core::tointegral(m));
            
        return true;
    }

    
    bool initialised(MOTOR m)
    {
        return embot::core::binary::bit::check(_enc_internals.initialisedmask, embot::core::tointegral(m));
    } 
    
    
    bool start(embot::hw::MOTOR m, const Mode& mode)
    {
        if(false == mode.isvalid())
        {
            return false;
        }

        if(true == isstarted(m))
        {   // if already started i first stop. 
            stop(m);
        }        
              
        _enc_internals._items[embot::core::tointegral(m)].load(mode);        
                                   
        embot::hw::motor::bldc::qenc::bsp::getBSP().start(m, mode);
           
        _enc_internals._items[embot::core::tointegral(m)].started = true;
        
        return true;
    }

    
    bool isstarted(embot::hw::MOTOR m)
    {
        return _enc_internals._items[embot::core::tointegral(m)].started;
    }

    
    bool stop(embot::hw::MOTOR m)
    {
        if(false == isstarted(m))
        {   
            return true;
        }        
               
        _enc_internals._items[embot::core::tointegral(m)].mode = {};
          
        embot::hw::motor::bldc::qenc::bsp::getBSP().stop(m);
           
        _enc_internals._items[embot::core::tointegral(m)].started = false;
        
        return true;
    }

    
    float angle(embot::hw::MOTOR m, AngleQE aqe)
    {        
        return embot::hw::motor::bldc::qenc::bsp::getBSP().angle(m, aqe);
    }


} // namespace embot::hw::motor::bldc::qenc {



#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc_qenc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

